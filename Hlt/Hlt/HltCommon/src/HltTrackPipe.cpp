// $Id: HltTrackPipe.cpp,v 1.2 2010-07-29 12:16:24 ibelyaev Exp $
// ============================================================================
// Incldue files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
#include <string>
#include <memory>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h" 
// ============================================================================
// Boost
// ============================================================================
#include "boost/algorithm/string/join.hpp"
// ============================================================================
// HtlBase 
// ============================================================================
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
// ============================================================================
// Event model
// ============================================================================
#include "Event/Track.h" 
// ============================================================================
// LoKi :-) 
// ============================================================================
#include "LoKi/ITrHybridFactory.h"
#include "LoKi/BasicFunctors.h"
#include "LoKi/TrackTypes.h"
#include "LoKi/apply.h"
#include "LoKi/Filters.h"
// ============================================================================
/** @file
 *  The basic track-piping algorithm for Hlt-framework
 *  @author Gerhard RAVEN 
 */
// ============================================================================
namespace Hlt 
{
  // ==========================================================================
  /** @class TrackFilter 
   *  The basic track-piping algorithm for Hlt-framework
   *  @author Gerhard RAVEN 
   */
  class TrackPipe : public HltAlgorithm 
  {
  public:
    // ========================================================================
    /// Standard constructor
    TrackPipe( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~TrackPipe( ) {};         ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute();       ///< Algorithm execution
    virtual StatusCode finalize();      ///< Algorithm finalization
    // ========================================================================
  private:
    // ========================================================================
    StatusCode decode();
    // ========================================================================
    typedef LoKi::TrackTypes::TrPipe Pipe; 
    
    Hlt::SelectionContainer2<LHCb::Track,LHCb::Track> m_selection;
    
    Pipe                            m_pipe;
    
    std::string                                       m_factory;
    void updateCode      ( Property& );
    void updatePreambulo ( Property& );
    
    std::vector<std::string> m_preambulo_ ;             // the preambulo itself
    std::string m_preambulo ;                           // the preambulo itself
    std::string m_code;
    
    bool m_code_updated;
    bool m_preambulo_updated;
    
    std::string m_preMonitor, m_postMonitor;
    bool m_monitor;
  };
  // ==========================================================================
} //                                                       end of namespace Hlt 
// ============================================================================
namespace 
{
  // ==========================================================================
  /// the most trivial pipe
  const LoKi::TrackTypes::TrPipe s_PIPE = 
    LoKi::Identity<LHCb::Track::ConstVector>() ;
  // ==========================================================================
}
// ============================================================================
Hlt::TrackPipe::TrackPipe( const std::string& name,
                           ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selection    ( *this  )
  , m_pipe         ( s_PIPE )
  , m_code_updated ( false  )
  , m_preambulo_updated(false)
{
  m_selection.declareProperties( );
  declareProperty("Factory", m_factory = "LoKi::Hybrid::TrTool/TrHybridFactory:PUBLIC");
  declareProperty("Code", m_code) ->declareUpdateHandler( &TrackPipe::updateCode, this );
  declareProperty("Preambulo", m_preambulo_)->declareUpdateHandler(&TrackPipe::updatePreambulo , this);
  declareProperty("Monitor", m_monitor = false );
  declareProperty("InputMonitor", m_postMonitor = "" );
  declareProperty("OutputMonitor", m_preMonitor = "" );
}
//=============================================================================
// Initialization
//=============================================================================
StatusCode Hlt::TrackPipe::initialize() 
{
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  m_selection.retrieveSelections();
  m_selection.registerSelection();
  return decode();
}
// ============================================================================
// Main execution
// ============================================================================
StatusCode Hlt::TrackPipe::execute() 
{
  counter("#input")  +=  m_selection.input<1>()->size();
  if (m_monitor) { }
  //TODO: make m_pipe a vector of pipes...
  assert( m_selection.output()->empty() );
  //
  /// set of rather dirty lines, 
  ///    to be moved into $LOKICOREROOT/LoKi/Streamers.h
  /// 
  const LHCb::Track::Vector&      _vct  = m_selection.input<1>() -> vct () ;
  const LHCb::Track::ConstVector* _vct_ =
    reinterpret_cast<const LHCb::Track::ConstVector*>( &_vct  ) ;
  const LHCb::Track::ConstVector  _out  = m_pipe     ( *_vct_ ) ;
  const LHCb::Track::Vector*      _out_ =
    reinterpret_cast<const LHCb::Track::Vector*>( &_out ) ;
  //
  m_selection.output()->insert
    ( m_selection.output()->end() , 
      _out_ -> begin () ,
      _out_ -> end   () ) ;
  //
  if (m_monitor) { }
  //
  setFilterPassed( !m_selection.output()->empty() );
  return StatusCode::SUCCESS;
}
// ============================================================================
// Finalization
// ============================================================================
StatusCode Hlt::TrackPipe::finalize() 
{
  m_pipe = s_PIPE ; // delete at finalize to make sure that if the predicate
  // has picked up some tools, they get released...
  return HltAlgorithm::finalize();
}
// ============================================================================
// decode the cuts 
// ============================================================================
StatusCode Hlt::TrackPipe::decode() 
{
  LoKi::ITrHybridFactory* factory = tool<LoKi::ITrHybridFactory>( m_factory,this ) ;
  //
  m_pipe        = s_PIPE ;
  StatusCode sc = factory->get( m_code, m_pipe, m_preambulo );
  if (sc.isFailure()) return sc;
  this->release(factory);
  m_code_updated      = false ;
  m_preambulo_updated = false ;
  return StatusCode::SUCCESS;
}
// ============================================================================
// update handlers
// ============================================================================
void Hlt::TrackPipe::updateCode ( Property& /* p */ )    
{
  /// mark as "to-be-updated"
  m_code_updated = true ;
  // no action if not yet initialized
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  // postpone the action
  if ( !m_preambulo_updated ) { return ; }
  // perform the actual immediate decoding
  StatusCode sc = decode  () ;
  Assert ( sc.isFailure() , "Error from Hlt::TrackPipe::decode()" , sc ) ;
}
// ============================================================================
// update handlers
// ============================================================================
void Hlt::TrackPipe::updatePreambulo ( Property& /* p */ )
{
  // concatenate the preambulo:
  m_preambulo = boost::algorithm::join( m_preambulo_ , "\n" );
  /// mark as "to-be-updated"
  m_preambulo_updated = true ;
  // no further action if not yet initialized
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  // postpone the action
  if ( !m_code_updated ) { return ; }

  // perform the actual immediate decoding
  StatusCode sc = decode  () ;
  Assert ( sc.isFailure() , "Error from Hlt::TrackPipe::decode()" , sc ) ;
}


// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Hlt, TrackPipe );
// ============================================================================
// The END 
// ============================================================================
