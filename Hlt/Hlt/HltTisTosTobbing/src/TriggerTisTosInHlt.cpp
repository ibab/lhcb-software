// $Id: TriggerTisTosInHlt.cpp,v 1.4 2010-07-21 21:22:17 tskwarni Exp $
// Include files 
#include <algorithm>
#include <functional>
#include <type_traits>
#include <vector>
#include <sstream>

// from Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TriggerTisTosInHlt.h"

#include "boost/regex.hpp"

#include "Event/HltDecReports.h"


using namespace LHCb;

namespace {
    template <typename T  > class is_equal_to_t {
        const T& _target;
      public: 
        is_equal_to_t(const T& target) : _target(target) {}
        template<typename U> 
        bool operator()(const U& u) const { return _target == u ; }
    };

    template <typename T> 
    is_equal_to_t<T> is_equal_to(const T& target) {
        return is_equal_to_t<T>(target);
    }

    // would be better to use a container type which 
    // guarantees unique entries... (aka 'set' behaviour)
    // until then, we have an insert interator which 'adapts'
    // the underlying container's behaviour...
    // FIXME:TODO and which makes insertion a quardratic operation!!!
    // better to use eg. Gaudi::VectorMap< StringKey, struct empty {} >
    // as container...
    template <typename Container>
    class push_back_if_unique_iterator
        : public std::iterator<std::output_iterator_tag, void, void, void, void>
    {
        Container* container;

      public:
        template<typename T>
        explicit push_back_if_unique_iterator( T& x ) : container( &x )
        {
        }
        push_back_if_unique_iterator&
        operator=( typename Container::const_reference value )
        {
            if ( std::none_of( std::begin( *container ), std::end( *container ),
                               is_equal_to( value ) ) ) {
                container->push_back( value );
            }
            return *this;
        }
        push_back_if_unique_iterator& operator*()
        {
            return *this;
        }
        push_back_if_unique_iterator& operator++()
        {
            return *this;
        }
        push_back_if_unique_iterator& operator++( int )
        {
            return *this;
        }
    };

    template <typename Container> 
    push_back_if_unique_iterator<Container> unique_back_inserter( Container& container) {
        return push_back_if_unique_iterator<Container>(container); 
    }
}

//-----------------------------------------------------------------------------
// Implementation file for class : TriggerTisTosInHlt
//
// 2007-08-20 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TriggerTisTosInHlt )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TriggerTisTosInHlt::TriggerTisTosInHlt( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
    : TriggerSelectionTisTosInHlt( type, name, parent ), m_inspector( nullptr )
{
    declareInterface<ITriggerTisTos>( this );

    declareProperty( "TriggerInputWarnings", m_trigInputWarn = false );
    declareProperty( "AllowIntermediateSelections",
                     m_allowIntermediateSelections = false );
}
//=============================================================================
// Destructor
//=============================================================================
TriggerTisTosInHlt::~TriggerTisTosInHlt()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TriggerTisTosInHlt::initialize()
{
    StatusCode sc =
        TriggerSelectionTisTosInHlt::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    debug() << "==> Initialize" << endmsg;

    m_inspector = svc<Hlt::IInspector>( "Hlt::Service", true );

    setOfflineInput();
    setTriggerInput();

    return StatusCode::SUCCESS;
}

//=============================================================================

void TriggerTisTosInHlt::getTriggerNames()
{
    if ( m_newEvent ) {
        m_triggerNames.clear();
        m_newEvent = false;
    }

    // done before ?
    if ( !m_triggerNames.empty() ) return;

    // get trigger names from HltDecReports and HltSelReports
    if ( !m_hltDecReports ) getHltSummary();
    //    for the same TCK this should be fixed list for events which passed Hlt
    if ( m_hltDecReports ) m_triggerNames = m_hltDecReports->decisionNames();

    if ( m_allowIntermediateSelections ) {
        Hlt::IInspector::KeyList selIDs;
        m_inspector->selections( selIDs );
        // FIXME:TODO: why is m_triggerNames not eg. a
        // Gaudi::VectorMap<StringKey,stuct{} > ?
        std::copy( std::begin( selIDs ), std::end( selIDs ),
                   unique_back_inserter( m_triggerNames ) );
        // duplicates don't matter here, before use this list gets copied 
        // into m_triggerInput_Selections... should only check once for
        // duplicates!!!
    }

    if ( m_triggerNames.empty() ) {
        Warning( "No known trigger names found", StatusCode::FAILURE, 1 )
            .setChecked();
    }
}

void TriggerTisTosInHlt::setTriggerInput()
{
    m_triggerInput_Selections.clear();
}

void
TriggerTisTosInHlt::addToTriggerInput( const std::string& selectionNameWithWildChar )
{
    unsigned int sizeAtEntrance( m_triggerInput_Selections.size() );
    getTriggerNames();
    boost::regex expr( selectionNameWithWildChar );
    std::copy_if( std::begin( m_triggerNames ), std::end( m_triggerNames ),
                  unique_back_inserter( m_triggerInput_Selections ),
                  [&]( const std::string& inpt ) { return boost::regex_match( inpt, expr );
    } );

    if ( m_trigInputWarn &&
         ( m_triggerInput_Selections.size() == sizeAtEntrance ) ) {
        std::ostringstream mess;
        mess << " addToTriggerInput called with selectionNameWithWildChar="
             << selectionNameWithWildChar
             << " added no selection to the Trigger Input, which has size="
             << m_triggerInput_Selections.size();
        Warning( mess.str(), StatusCode::SUCCESS, 50 ).setChecked();
    }
}

unsigned int TriggerTisTosInHlt::tisTosTrigger()
{
    unsigned int result = 0;
    if ( m_trigInputWarn && m_triggerInput_Selections.empty() ) {
        Warning( " tisTosTrigger called with empty Trigger Input" ).setChecked();
    }
    for ( const std::string& triggerSelection : m_triggerInput_Selections ) {
        unsigned int res = tisTosSelection( triggerSelection );
        if ( res & kDecision ) result |= res;
        if ( ( result & kTOS ) && ( result & kTIS ) && ( result & kTPS ) ) break;
    }
    return result;
}

// analysisReport for Trigger (define Trigger & Offline Input before calling)
std::string TriggerTisTosInHlt::analysisReportTrigger()
{
    std::ostringstream report;
    report << offset() << " Trigger #-of-sel " << m_triggerInput_Selections.size()
           << std::endl;
    unsigned int result = 0;
    if ( m_triggerInput_Selections.empty() ) {
        report << "Trigger Input empty" << std::endl;
        return report.str();
    }
    for ( const std::string& triggerSelection : m_triggerInput_Selections ) {
        unsigned int res = tisTosSelection( triggerSelection );
        ++m_reportDepth;
        if ( res & kDecision ) {
            report << analysisReportSelection( triggerSelection );
            result |= res;
        } else {
            report << offset() << " Selection " + triggerSelection +
                                      " decision=false " << std::endl;
        }
        --m_reportDepth;

        //    if( (result & kTOS) && (result & kTIS) && (result & kTPS ) )break;
    }
    TisTosTob res( result );
    report << offset() << " Trigger #-of-sel " << m_triggerInput_Selections.size()
           << " TIS= " << res.tis() << " TOS= " << res.tos() << " TPS= " << res.tps()
           << " decision= " << res.decision() << std::endl;
    return report.str();
}

// fast check for TOS
bool TriggerTisTosInHlt::tosTrigger()
{
    if ( m_trigInputWarn &&  m_triggerInput_Selections.empty() ) {
            Warning( " tosTrigger called with empty Trigger Input" ).setChecked();
    }
    return std::any_of(
        std::begin( m_triggerInput_Selections ),
        std::end( m_triggerInput_Selections ),
        [&]( const std::string& sel ) { return this->tosSelection( sel ); } );
}

// fast check for TIS
bool TriggerTisTosInHlt::tisTrigger()
{
    if (m_trigInputWarn && m_triggerInput_Selections.empty() ) {
            Warning( " tisTrigger called with empty Trigger Input" ).setChecked();
    }
    return std::any_of(
        std::begin( m_triggerInput_Selections ),
        std::end( m_triggerInput_Selections ),
        [&]( const std::string& sel ) { return this->tisSelection( sel ); } );
}

// fast check for TIS
bool TriggerTisTosInHlt::tusTrigger()
{
    if ( m_trigInputWarn && m_triggerInput_Selections.empty() ) {
            Warning( " tpsTrigger called with empty Trigger Input" ).setChecked();
    }
    return std::any_of(
        std::begin( m_triggerInput_Selections ),
        std::end( m_triggerInput_Selections ),
        [&]( const std::string& sel ) { return this->tusSelection( sel ); } );
}

std::vector<std::string> TriggerTisTosInHlt::triggerSelectionNames(
    unsigned int decisionRequirement, unsigned int tisRequirement,
    unsigned int tosRequirement, unsigned int tpsRequirement )
{
    if ( ( decisionRequirement >= kAnything ) && ( tisRequirement >= kAnything ) &&
         ( tosRequirement >= kAnything ) && ( tpsRequirement >= kAnything ) ) {
        return m_triggerInput_Selections;
    }
    std::vector<std::string> selections;
    std::copy_if( std::begin( m_triggerInput_Selections ),
                  std::end( m_triggerInput_Selections ),
                  std::back_inserter( selections ), [&]( const std::string& sel ) {
        unsigned int result = this->tisTosSelection( sel );
        bool decision = result & kDecision;
        bool tos = result & kTOS;
        bool tis = result & kTIS;
        bool tps = result & kTPS;
        return ( ( decisionRequirement >= kAnything ) ||
                 ( decision == decisionRequirement ) ) &&
               ( ( tisRequirement >= kAnything ) || ( tis == tisRequirement ) ) &&
               ( ( tosRequirement >= kAnything ) || ( tos == tosRequirement ) ) &&
               ( ( tpsRequirement >= kAnything ) || ( tps == tpsRequirement ) );
    } );
    return selections;
}

std::vector<const LHCb::HltObjectSummary*>
TriggerTisTosInHlt::hltObjectSummaries( unsigned int tisRequirement,
                                        unsigned int tosRequirement,
                                        unsigned int tpsRequirement )
{
    std::vector<const LHCb::HltObjectSummary*> hosVec;
    if ( m_trigInputWarn &&  m_triggerInput_Selections.empty() ) {
        Warning( " hltObjectSummaries called with empty Trigger Input" )
            .setChecked();
    }
    for ( const std::string& triggerSelection : m_triggerInput_Selections ) {
        std::vector<const LHCb::HltObjectSummary*> selHosVec =
            hltSelectionObjectSummaries( triggerSelection, tisRequirement,
                                         tosRequirement, tpsRequirement );
        hosVec.insert( hosVec.end(), selHosVec.begin(), selHosVec.end() );
    }
    return hosVec;
}
