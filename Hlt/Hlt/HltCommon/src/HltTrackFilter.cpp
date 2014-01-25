// $Id: HltTrackFilter.cpp,v 1.15 2010-08-17 08:47:19 graven Exp $
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/algorithm/string/join.hpp"
#include "boost/format.hpp"
// ============================================================================
// HltBase
// ============================================================================
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// LoKi :-)
// ============================================================================
#include "LoKi/ITrackFunctorFactory.h"
#include "LoKi/BasicFunctors.h"
#include "LoKi/TrackTypes.h"
// ============================================================================
/** @file
 *  The basic track-filtering algorithm for Hlt-framework
 *  @author Gerhard RAVEN
 */
// ============================================================================
namespace Hlt
{
// ==========================================================================
/** @class TrackFilter
 *  The basic track-filtering algorithm for Hlt-framework
 *  @author Gerhard RAVEN
 */
class TrackFilter : public HltAlgorithm
{
  public:
    // ========================================================================
    /// Standard constructor
    TrackFilter( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~TrackFilter() {}; ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm execution
    virtual StatusCode finalize();   ///< Algorithm finalization
    // ========================================================================
  private:
    // ========================================================================
    StatusCode decode();
    // ========================================================================
  private:
    // ========================================================================
    typedef LoKi::TrackTypes::TrCut Predicate;
    Hlt::SelectionContainer2<LHCb::Track, LHCb::Track> m_selection;
    // group the following three together...
    class Filter
    {
      public:
        // ======================================================================
        Filter( const Predicate& predicate, StatEntity* counterPass,
                StatEntity* counterCand )
            : m_predicate( predicate )
            , m_counterPass( counterPass )
            , m_counterCand( counterCand )
        {
        }
        Filter()
            : m_predicate(
                  LoKi::BasicFunctors<const LHCb::Track*>::BooleanConstant( false ) )
            , m_counterPass( 0 )
            , m_counterCand( 0 )
        {
        }
        const Predicate& predicate()
        {
            return m_predicate;
        }
        StatEntity& counterPass()
        {
            return *m_counterPass;
        }
        StatEntity& counterCand()
        {
            return *m_counterCand;
        }

      private:
        Predicate m_predicate;
        StatEntity* m_counterPass;
        StatEntity* m_counterCand;
    };
    // ========================================================================
    std::vector<Filter> m_predicates;
    // ========================================================================
    std::string m_factory;
    void updateCode( Property& );
    void updatePreambulo( Property& );

    std::vector<std::string> m_preambulo_; // the preambulo itself
    std::string m_preambulo;               // the preambulo itself
    std::vector<std::string> m_code;

    bool m_code_updated;
    bool m_preambulo_updated;
};
// ==========================================================================
} //                                                       end of namespace Hlt
// ============================================================================
namespace
{
// ==========================================================================
template <typename T>
class adaptor_
{
  public:
    // ========================================================================
    typedef typename T::result_type result_type;
    typedef typename T::argument_type argument_type;
    adaptor_( T& t ) : m_t( t )
    {
    }
    result_type operator()( argument_type x ) const
    {
        return !m_t( x );
    }
    // ========================================================================
  private:
    // ========================================================================
    T& m_t;
    // ========================================================================
};
// ==========================================================================
template <typename T>
adaptor_<T> adapt( T& t )
{
    return adaptor_<T>( t );
}
// ==========================================================================
}
// ============================================================================
// constructor
// ============================================================================
Hlt::TrackFilter::TrackFilter( const std::string& name, ISvcLocator* pSvcLocator )
    : HltAlgorithm( name, pSvcLocator )
    , m_selection( *this )
    , m_factory( "LoKi::Hybrid::TrackFunctorFactory/TrackFunctorFactory:PUBLIC" )
    , m_code_updated( false )
    , m_preambulo_updated( false )
{
    m_selection.declareProperties();
    declareProperty(
        "Factory", m_factory,
        "The Hybrid factory to be used for decoding of LoKi/Bender python cuts" );
    declareProperty( "Code", m_code,
                     "the actual LoKi/Bender python code to beused for filtyering" )
        ->declareUpdateHandler( &TrackFilter::updateCode, this );
    declareProperty( "Preambulo", m_preambulo_,
                     "The 'Preambulo' strings, to be used to for decoding of cuts" )
        ->declareUpdateHandler( &TrackFilter::updatePreambulo, this );
}
// ============================================================================
// Initialization
// ============================================================================
StatusCode Hlt::TrackFilter::initialize()
{
    StatusCode sc = HltAlgorithm::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm
    m_selection.retrieveSelections();
    m_selection.registerSelection();
    return decode();
}
// ============================================================================
// Main execution
// ============================================================================
StatusCode Hlt::TrackFilter::execute()
{

    Hlt::TSelection<LHCb::Track>* out = m_selection.output();

    assert( out->empty() );
    *m_selection.input<1>() >> *out;
    for ( std::vector<Filter>::iterator i = m_predicates.begin();
          i != m_predicates.end(); ++i ) {
        out->erase(
            std::remove_if( out->begin(), out->end(),
                            adapt( i->predicate() ) ) // move adapt into *i... write
                                                      // an apply for this
                                                      // erase/remove_if construct
            ,
            out->end() );
        i->counterPass() += !out->empty();
        if ( out->empty() ) break;
        i->counterCand() += out->size();
    }
    setFilterPassed( !out->empty() );
    return StatusCode::SUCCESS;
}
// ============================================================================
// Finalization
// ============================================================================
StatusCode Hlt::TrackFilter::finalize()
{
    // delete at finalize to make sure that if the predicate
    m_predicates.clear();
    return HltAlgorithm::finalize();
}
// ============================================================================
StatusCode Hlt::TrackFilter::decode()
{
    LoKi::ITrackFunctorFactory* factory =
        tool<LoKi::ITrackFunctorFactory>( m_factory, this );
    //
    m_predicates.clear();
    m_predicates.reserve( m_code.size() );
    for ( std::vector<std::string>::const_iterator i = m_code.begin();
          i != m_code.end(); ++i ) {
        //
        LoKi::TrackTypes::TrCut cut =
            LoKi::BasicFunctors<const LHCb::Track*>::BooleanConstant( false );
        StatusCode sc = factory->get( *i, cut, m_preambulo );
        if ( sc.isFailure() ) {
            error() << "Failed to decode " << *i << endmsg;
            error() << "with preamble " << m_preambulo << endmsg;
            return sc;
        }
        std::string title =
            boost::str( boost::format( "%02d:%s" ) % m_predicates.size() % *i );
        m_predicates.push_back(
            Filter( cut, &counter( title ), &counter( title + " candidates" ) ) );
    }
    //
    this->release( factory );
    m_code_updated = false;
    m_preambulo_updated = false;
    return StatusCode::SUCCESS;
}
// ============================================================================
// update handlers
// ============================================================================
void Hlt::TrackFilter::updateCode( Property& /* p */ )
{
    /// mark as "to-be-updated"
    m_code_updated = true;
    // no action if not yet initialized
    if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) {
        return;
    }
    // postpone the action
    if ( !m_preambulo_updated ) {
        return;
    }
    // perform the actual immediate decoding
    StatusCode sc = decode();
    Assert( sc.isFailure(), "Error from Hlt::TrackFilter::decode()", sc );
}
// ============================================================================
// update handlers
// ============================================================================
void Hlt::TrackFilter::updatePreambulo( Property& /* p */ )
{
    // concatenate the preambulo:
    m_preambulo = boost::algorithm::join( m_preambulo_, "\n" );
    /// mark as "to-be-updated"
    m_preambulo_updated = true;
    // no further action if not yet initialized
    if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) {
        return;
    }
    // postpone the action
    if ( !m_code_updated ) {
        return;
    }

    // perform the actual immediate decoding
    StatusCode sc = decode();
    Assert( sc.isFailure(), "Error from Hlt::TrackFilter::decode()", sc );
}
// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Hlt, TrackFilter )
// ============================================================================
// The END
// ============================================================================
