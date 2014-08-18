// $Id: TriggerSelectionTisTosInHlt.cpp,v 1.10 2010-08-17 08:54:52 graven Exp $
// Include files
#include <algorithm>
#include <vector>
#include <sstream>

// from Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"

// local
#include "TriggerSelectionTisTosInHlt.h"

#include "Event/HltDecReports.h"
#include "Event/HltCandidate.h"

#include "HltBase/HltSelection.h"

using namespace LHCb;

namespace
{

template <typename C,
          typename T = typename std::remove_const<typename C::candidate_type>::type>
std::vector<T*> convert( const C& container )
{
    std::vector<T*> v;
    v.reserve( container.size() );
    std::transform(
        std::begin( container ), std::end( container ), std::back_inserter( v ),
        []( typename C::const_reference i ) { return const_cast<T*>( i ); } );
    return v;
}

template <typename T, typename C>
std::vector<T*> convertCandidate( const C& container )
{
    std::vector<T*> v;
    v.reserve( container.size() );
    std::transform( std::begin( container ), std::end( container ),
                    std::back_inserter( v ), []( typename C::const_reference i ) {
        return const_cast<T*>( i->currentStage()->template get<T>() );
    } );
    return v;
}

struct getSummary
{
    LHCb::HltObjectSummary::Container& container;

  public:
    getSummary( LHCb::HltObjectSummary::Container& c ) : container( c )
    {
    }

    template <typename T>
    HltObjectSummary* operator()( const T* object )
    {
        if ( !object ) return nullptr;
        auto i = std::find_if( std::begin( container ), std::end( container ),
                               [object]( const HltObjectSummary* p ) {
            return p->summarizedObjectCLID() == object->clID() &&
                   p->summarizedObject() == object;
        } );
        HltObjectSummary* hos = ( i != std::end( container ) ? *i : nullptr );
        if ( !hos ) {
            container.push_back( new HltObjectSummary() );
            hos = container.back();
            hos->setSummarizedObjectCLID( object->clID() );
            hos->setSummarizedObject( object );
        }
        return hos;
    }
};

struct isTisTos
{
    ITriggerSelectionTisTos* parent;
    unsigned int tisRequirement, tosRequirement, tpsRequirement;

  public:
    isTisTos( ITriggerSelectionTisTos* p, unsigned int tisReq, unsigned int tosReq,
              unsigned int tpsReq )
        : parent{p}
        , tisRequirement{tisReq}
        , tosRequirement{tosReq}
        , tpsRequirement{tpsReq}
    {
    }

    template <typename T>
    bool operator()( const T& object )
    {
        unsigned int result = parent->tisTos( object );
        bool tis = result & ITriggerSelectionTisTos::kTIS;
        bool tos = result & ITriggerSelectionTisTos::kTOS;
        bool tps = result & ITriggerSelectionTisTos::kTPS;
        return ( ( tisRequirement >= ITriggerSelectionTisTos::kAnything ) ||
                 ( tis == tisRequirement ) ) &&
               ( ( tosRequirement >= ITriggerSelectionTisTos::kAnything ) ||
                 ( tos == tosRequirement ) ) &&
               ( ( tpsRequirement >= ITriggerSelectionTisTos::kAnything ) ||
                 ( tps == tpsRequirement ) );
    }
};

#define DISABLE_IF_CONVERTIBLE( X, Y )                                              \
    typename std::enable_if<!std::is_convertible<X, Y>::value, void>::type* = nullptr

struct applyTus
{
    TriggerSelectionTisTosInHlt* parent;

  public:
    explicit applyTus( TriggerSelectionTisTosInHlt& p ) : parent( &p )
    {
    }
    bool operator()( const std::vector<std::string>& dependencies )
    {
        return parent->ITriggerSelectionTisTos::tusSelection( dependencies );
    }
    template <typename T, DISABLE_IF_CONVERTIBLE(T, const std::vector<std::string>&)>
    bool operator()( T&& objects )
    {
        return parent->IParticleTisTos::tus( std::forward<T>( objects ) );
    }
};

struct applyTis
{
    TriggerSelectionTisTosInHlt* parent;

  public:
    explicit applyTis( TriggerSelectionTisTosInHlt& p ) : parent( &p )
    {
    }
    bool operator()( const std::vector<std::string>& dependencies )
    {
        return parent->ITriggerSelectionTisTos::tisSelection( dependencies );
    }
    template <typename T, DISABLE_IF_CONVERTIBLE(T, const std::vector<std::string>&)>
    bool operator()( T&& objects )
    {
        return parent->IParticleTisTos::tis( std::forward<T>( objects ) );
    }
};

struct applyTos
{
    TriggerSelectionTisTosInHlt* parent;
  public:
    explicit applyTos( TriggerSelectionTisTosInHlt& p ) : parent( &p )
    {
    }
    bool operator()( const std::vector<std::string>& dependencies )
    {
        return parent->ITriggerSelectionTisTos::tosSelection( dependencies );
    }
    template <typename T, DISABLE_IF_CONVERTIBLE(T, const std::vector<std::string>&)>
    bool operator()( T&& objects )
    {
        return parent->IParticleTisTos::tos( std::forward<T>( objects ) );
    }
};
}

//-----------------------------------------------------------------------------
// Implementation file for class : TriggerSelectionTisTosInHlt
//
// 2009-10-22 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TriggerSelectionTisTosInHlt )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TriggerSelectionTisTosInHlt::TriggerSelectionTisTosInHlt( const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent )
    : ParticleTisTos( type, name, parent )
    , m_hltDecReports{nullptr}
    , m_hltDataSvc{nullptr}
    , m_hltInspectorSvc{nullptr}
    , m_objectSummaries{nullptr}
{
    declareInterface<ITriggerSelectionTisTos>( this );

    declareProperty( "HltDecReportsLocation",
                     m_HltDecReportsLocation =
                         LHCb::HltDecReportsLocation::Default );

    m_cached_tisTosTob.reserve( 500 );
}

//=============================================================================
// Destructor
//=============================================================================
TriggerSelectionTisTosInHlt::~TriggerSelectionTisTosInHlt()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TriggerSelectionTisTosInHlt::initialize()
{
    StatusCode sc = ParticleTisTos::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    debug() << "==> Initialize" << endmsg;

    IIncidentSvc* incidentSvc( 0 );
    if ( !service( "IncidentSvc", incidentSvc ).isSuccess() )
        return StatusCode::FAILURE;
    // add listener to be triggered by first BeginEvent
    bool rethrow = false;
    bool oneShot = false;
    long priority = 0;
    incidentSvc->addListener( this, IncidentType::BeginEvent, priority, rethrow,
                              oneShot );
    incidentSvc->release();

    m_hltDataSvc = svc<Hlt::IData>( "Hlt::Service", true );
    m_hltInspectorSvc = svc<Hlt::IInspector>( "Hlt::Service", true );

    m_newEvent = true;

    setOfflineInput();

    return StatusCode::SUCCESS;
}

void TriggerSelectionTisTosInHlt::handle( const Incident& )
{
    m_hltDecReports = nullptr;
    setOfflineInput();
    m_newEvent = true;
    m_objectSummaries = nullptr;
}

// ------------------------------------------------------------------------------------
void TriggerSelectionTisTosInHlt::getHltSummary()
{
    if ( !m_hltDecReports ) {
        m_hltDecReports = getIfExists<HltDecReports>( m_HltDecReportsLocation );
        if ( !m_hltDecReports ) {
            Error( " No HltDecReports at " + m_HltDecReportsLocation.value(),
                   StatusCode::FAILURE, 2 ).setChecked();
        }
    }
}

//=============================================================================
// -------------- offline inputs -----------------------------------------------
//=============================================================================

// erase previous input ---------------------------------------------------------
void TriggerSelectionTisTosInHlt::setOfflineInput()
{
    setSignal();
    clearCache();
}

//    hit list input ---------------------------------------------------------------
void TriggerSelectionTisTosInHlt::addToOfflineInput(
    const std::vector<LHCb::LHCbID>& hitlist )
{
    if ( addToSignal( hitlist ) ) clearCache();
}

//    Track input ---------------------------------------------------------------
void TriggerSelectionTisTosInHlt::addToOfflineInput( const LHCb::Track& track )
{
    if ( addToSignal( track ) ) clearCache();
}

//    Proto-particle input
// -----------------------------------------------------------------------
void TriggerSelectionTisTosInHlt::addToOfflineInput(
    const LHCb::ProtoParticle& protoParticle )
{
    if ( addToSignal( protoParticle ) ) clearCache();
}

//    Particle input
// -----------------------------------------------------------------------
void TriggerSelectionTisTosInHlt::addToOfflineInput( const LHCb::Particle& particle )
{
    if ( addToSignal( particle ) ) clearCache();
}

//=============================================================================

//=============================================================================
// -------------- outputs:
//=============================================================================

// single complete Trigger Selection TisTos  (define Offline Input before calling)
unsigned int
TriggerSelectionTisTosInHlt::tisTosSelection( const std::string& selectionName )
{
    const Gaudi::StringKey name( selectionName );

    unsigned int result = 0;
    if ( findInCache( name, result ) ) return result;

    getHltSummary();

    bool decision( false );
    // get decision from HltDecReports if can find it
    bool decFound( false );
    if ( m_hltDecReports ) {
        const HltDecReport* rep = m_hltDecReports->decReport( name.str() );
        if ( rep ) {
            decision = rep->decision();
            decFound = true;
        }
    }
    if ( decision ) result |= kDecision;

    // find producer of the selection
    const IAlgorithm* producer = m_hltInspectorSvc->producer( name );
    const Hlt::Selection* sel =
        producer ? m_hltDataSvc->retrieve( producer, name ) : nullptr;
    // unsuccessful selections not allowed to be TisTossed
    if ( !sel || !sel->decision() ) {
        storeInCache( name, result );
        return result;
    }

    // it is possible that decision was not stored in HltDecReports if this was an
    // intermediate step, thus
    //   if not found in HltDecReports, set it here
    // do not overwrite decisions from HltDecReports, since positive selection could
    // have been killed by postscale
    if ( !decFound ) decision = true;

    // classify the decision
    if ( !sel->empty() ) {

        if ( sel->classID() == Hlt::Candidate::classID() ) {
            const Hlt::TSelection<Hlt::Candidate>& tsel =
                dynamic_cast<const Hlt::TSelection<Hlt::Candidate>&>( *sel );
            // grab the first one, and pray for a uniform list...
            const Hlt::Candidate* cand = tsel.front();
            const Hlt::Stage* stage = cand->currentStage();
            if ( stage->get<LHCb::Track>() ) {
                result = IParticleTisTos::tisTos( convertCandidate<Track>( tsel ) );
            } else if ( stage->get<LHCb::RecVertex>() ) {
                result =
                    IParticleTisTos::tisTos( convertCandidate<RecVertex>( tsel ) );
            } else {
                error()
                    << " got a candidate which is neither Track nor RecVertex... "
                    << endmsg;
            }

        } else if ( sel->classID() == LHCb::Track::classID() ) {

            auto& tsel = dynamic_cast<const Hlt::TrackSelection&>( *sel );
            result = IParticleTisTos::tisTos( convert( tsel ) );

        } else if ( sel->classID() == LHCb::RecVertex::classID() ) {

            auto& tsel = dynamic_cast<const Hlt::VertexSelection&>( *sel );
            result = IParticleTisTos::tisTos( convert( tsel ) );

        } else if ( sel->classID() == LHCb::Particle::classID() ) {

            auto& tsel =
                dynamic_cast<const Hlt::TSelection<LHCb::Particle>&>( *sel );
            result = IParticleTisTos::tisTos( convert( tsel ) );

        } else {
            auto& ids = sel->inputSelectionsIDs();
            if ( !ids.empty() ) {
                result = ITriggerSelectionTisTos::tisTosSelection( std::vector<std::string>{ std::begin(ids), std::end(ids) });
            } else {
                // @TODO: warning: hltDataSvc doesn't know about selInput...
            }
        }
    }

    if ( decision ) result |= kDecision;
    storeInCache( name, result );
    return result;
}

// single complete Trigger Selection TisTos  (define Offline Input before calling)
std::string TriggerSelectionTisTosInHlt::analysisReportSelection(
    const std::string& selectionName )
{
    std::ostringstream report;
    report << offset() << " Selection " + selectionName + " ";
    const Gaudi::StringKey name{selectionName};

    unsigned int result = 0;

    getHltSummary();

    bool decision( false );
    // get decision from HltDecReports if can find it
    bool decFound( false );
    if ( m_hltDecReports ) {
        const HltDecReport* rep = m_hltDecReports->decReport( name.str() );
        if ( rep ) {
            decision = rep->decision();
            decFound = true;
            report << " HltDecReport decision=" << decision;
        }
    }
    if ( decision ) result |= kDecision;

    // find producer of the selection
    const IAlgorithm* producer = m_hltInspectorSvc->producer( name );
    if ( !producer ) {
        report << "not known to HltInspectorSvc " << std::endl;
        return report.str();
    }
    const Hlt::Selection* sel = m_hltDataSvc->retrieve( producer, name );
    if ( !sel ) {
        report << "not found in HltDataSvc " << std::endl;
        return report.str();
    }
    // unsuccessful selections not allowed to be TisTossed
    if ( !( sel->decision() ) ) {
        report << "decision=false " << std::endl;
        return report.str();
    }

    // it is possible that decision was not stored in HltDecReports if this was an
    // intermediate step, thus
    //   if not found in HltDecReports, set it here
    // do not overwrite decisions from HltDecReports, since positive selection could
    // have been killed by postscale
    if ( !decFound ) decision = true;

    report << " size " << sel->size() << std::endl;

    if ( !sel->empty() ) {

        if ( sel->classID() == LHCb::Track::classID() ) {

            auto& tsel = dynamic_cast<const Hlt::TrackSelection&>( *sel );
            auto tracks = convert( tsel );
            report << analysisReports( tracks );
            result = IParticleTisTos::tisTos( tracks );

        } else if ( sel->classID() == LHCb::RecVertex::classID() ) {

            auto& tsel = dynamic_cast<const Hlt::VertexSelection&>( *sel );
            auto v = convert( tsel );
            report << analysisReports( v );
            result = IParticleTisTos::tisTos( v );

        } else if ( sel->classID() == LHCb::Particle::classID() ) {

            auto& tsel =
                dynamic_cast<const Hlt::TSelection<LHCb::Particle>&>( *sel );
            auto p = convert( tsel );
            report << analysisReports( p );
            result = IParticleTisTos::tisTos( p );

        } else {
            std::vector<std::string> dependencies( sel->inputSelectionsIDs().begin(),
                                                   sel->inputSelectionsIDs().end() );
            if ( !dependencies.empty() ) {
                report << offset()
                       << " Input Selections Vector size=" << dependencies.size()
                       << std::endl;
                ++m_reportDepth;
                for ( const std::string& isel : dependencies ) {
                    report << analysisReportSelection( isel );
                }
                --m_reportDepth;
                result = ITriggerSelectionTisTos::tisTosSelection( dependencies );
            } else {
                report << " not a Track/RecVertex/Particle selection and no Input "
                          "Selections " << std::endl;
            }
        }
    }
    if ( decision ) result |= kDecision;
    TisTosTob res( result );
    report << offset() << " Selection " + name.str() + " "
           << " TIS= " << res.tis() << " TOS= " << res.tos() << " TPS= " << res.tps()
           << " decision= " << res.decision() << std::endl;
    return report.str();
}

// fast check for TOS
bool TriggerSelectionTisTosInHlt::tosSelection( const std::string& selectionName )
{
    return selection( selectionName, applyTos( *this ) );
}

bool TriggerSelectionTisTosInHlt::tisSelection( const std::string& selectionName )
{
    return selection( selectionName, applyTis( *this ) );
}

bool TriggerSelectionTisTosInHlt::tusSelection( const std::string& selectionName )
{
    return selection( selectionName, applyTus( *this ) );
}

template <typename T>
bool TriggerSelectionTisTosInHlt::selection( const std::string& selectionName,
                                             T action )
{
    getHltSummary();
    if ( m_hltDecReports ) {
        const HltDecReport* rep = m_hltDecReports->decReport( selectionName );
        if ( rep && !rep->decision() ) return false;
    }
    const Gaudi::StringKey name{selectionName};
    const IAlgorithm* producer = m_hltInspectorSvc->producer( name );
    const Hlt::Selection* sel =
        producer ? m_hltDataSvc->retrieve( producer, name ) : nullptr;

    if ( !sel || !sel->decision() || sel->empty() ) return false;

    if ( sel->classID() == Hlt::Candidate::classID() ) {
        const Hlt::TSelection<Hlt::Candidate>& tsel =
            dynamic_cast<const Hlt::TSelection<Hlt::Candidate>&>( *sel );
        const Hlt::Stage* stage = tsel.front()->currentStage();
        if ( stage->get<LHCb::Track>() ) {
            return action( convertCandidate<Track>( tsel ) );
        }
        if ( stage->get<LHCb::RecVertex>() ) {
            return action( convertCandidate<RecVertex>( tsel ) );
        }
        error() << " got a candidate which is neither Track nor RecVertex... "
                << endmsg;
        return false;
    }
    auto* tracks = sel->down_cast<LHCb::Track>();
    if ( tracks ) return action( convert( *tracks ) );
    auto* vtxs = sel->down_cast<LHCb::RecVertex>();
    if ( vtxs ) return action( convert( *vtxs ) );
    auto* parts = sel->down_cast<LHCb::Particle>();
    if ( parts ) return action( convert( *parts ) );
    // check dependencies -- if any...
    return !sel->inputSelectionsIDs().empty() &&
           action( std::vector<std::string>{std::begin( sel->inputSelectionsIDs() ),
                                            std::end( sel->inputSelectionsIDs() )} );
}

// ------------ auxiliary output:  list of LHCbIDs corresponding to present offline
// input
std::vector<LHCb::LHCbID> TriggerSelectionTisTosInHlt::offlineLHCbIDs()
{
    return signal();
}

// ------------  additional functionality:  lists of object summaries for
// tracks/vertices/particles from trigger selections
//               satisfying TIS, TOS requirements

template <typename T>
std::vector<const LHCb::HltObjectSummary*>
TriggerSelectionTisTosInHlt::hltSelectionObjectSummaries(
    const Hlt::TSelection<T>& sel, unsigned int tisRequirement,
    unsigned int tosRequirement, unsigned int tpsRequirement )
{
    std::vector<const LHCb::HltObjectSummary*> matched;
    matched.reserve( sel.size() );
    isTisTos pred{this, tisRequirement, tosRequirement, tpsRequirement};
    getSummary get_summary{*m_objectSummaries};
    for ( T* object : convert( sel ) ) {
        if ( pred( *object ) ) matched.push_back( get_summary( object ) );
    }
    return matched;
}

std::vector<const LHCb::HltObjectSummary*>
TriggerSelectionTisTosInHlt::hltSelectionObjectSummaries(
    const std::string& selectionName, unsigned int tisRequirement,
    unsigned int tosRequirement, unsigned int tpsRequirement )
{
    getHltSummary();
    if ( !m_objectSummaries )
        m_objectSummaries =
            getOrCreate<HltObjectSummary::Container, HltObjectSummary::Container>(
                "/Event/Hlt/TriggerSelectionTisTosInHltStore" );

    if ( !m_objectSummaries ) {
        error() << " Unable to create summary container object " << endmsg;
    }

    // find producer of the selection
    const Gaudi::StringKey name{selectionName};
    const IAlgorithm* producer = m_hltInspectorSvc->producer( name );
    const Hlt::Selection* sel =
        producer ? m_hltDataSvc->retrieve( producer, name ) : nullptr;

    // dispatch to specifically typed function
    if ( sel && !sel->empty() ) {
        if ( sel->classID() == LHCb::Track::classID() ) {
            return hltSelectionObjectSummaries(
                dynamic_cast<const Hlt::TrackSelection&>( *sel ), tisRequirement,
                tosRequirement, tpsRequirement );
        }
        if ( sel->classID() == LHCb::RecVertex::classID() ) {
            return hltSelectionObjectSummaries(
                dynamic_cast<const Hlt::VertexSelection&>( *sel ), tisRequirement,
                tosRequirement, tpsRequirement );
        }
        if ( sel->classID() == LHCb::Particle::classID() ) {
            return hltSelectionObjectSummaries(
                dynamic_cast<const Hlt::TSelection<LHCb::Particle>&>( *sel ),
                tisRequirement, tosRequirement, tpsRequirement );
        }
        // oops...  unknown classID!
    }
    return {};
}
