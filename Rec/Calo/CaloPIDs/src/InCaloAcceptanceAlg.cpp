// ============================================================================
// Boost 
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
// Relations
// ============================================================================
#include "Relations/Relation1D.h"
// ============================================================================
// Local
// ============================================================================
#include "InCaloAcceptanceAlg.h"
// ============================================================================
/** @file 
 *  Implementation file for class InCaloAcceptanceAlg
 *  @Date 2006-06-17 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( InCaloAcceptanceAlg )
// ============================================================================
/// Standard protected constructor
// ============================================================================
InCaloAcceptanceAlg::InCaloAcceptanceAlg 
( const std::string& name , 
  ISvcLocator*       pSvc ) 
  : CaloTrackAlg( name , pSvc ) 
  , m_inputs   ( 1 , LHCb::TrackLocation::Default ) 
  , m_output   (   ) 
  , m_toolName (   ) 
  , m_tool     ( 0 )
{
  declareProperty ( "Inputs"      , m_inputs   ) ;
  declareProperty ( "Output"      , m_output   ) ;
  declareProperty ( "Tool"        , m_toolName ) ;
  // context-dependent default track container 
  m_inputs.clear();
  m_inputs = LHCb::CaloAlgUtils::TrackLocations( context() ) ;
}
// ============================================================================
// algorithm initialization 
// ============================================================================
StatusCode InCaloAcceptanceAlg::initialize() 
{
  StatusCode sc = CaloTrackAlg::initialize(); 
  if ( sc.isFailure() ) { return sc ; }
  //
  if ( m_inputs.empty() ) { Warning ( "Empty 'Inputs'-list"  ).ignore() ; }
  if ( m_output.empty() ) { Warning ( "Empty 'Output'-value" ).ignore() ; }
  // 
  m_tool = tool<IInAcceptance> ( m_toolName , this ) ;
  //
  return StatusCode::SUCCESS;
} 
// ============================================================================
// algorithm execution  
// ============================================================================
StatusCode InCaloAcceptanceAlg::execute() 
{
  typedef LHCb::Relation1D<LHCb::Track,bool>   Table  ;
  typedef LHCb::Track                          Track  ;
  typedef LHCb::Track::Container               Tracks ;
  
  // check the proper convertability
  BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::Calo2Track::ITrAccTable));
  
  // some more trivial checks 
  Assert ( !m_inputs.empty() , "No input data specified!"               ) ;
  Assert ( !m_output.empty() , "No input output location is specified!" ) ;
  Assert ( 0 != m_tool       , "InAcceptance-tool  is invalid!"         ) ;
  
  // create the table and register it in TES 
  Table* table = new Table ( 100 ) ;
  put ( table , m_output ) ;
  
  size_t nTracks = 0 ;
  size_t nAccept = 0 ;
  // loop over all track containers  
  for ( Inputs::const_iterator input = m_inputs.begin() ;m_inputs.end() != input ; ++input ){
    if( !exist<Tracks>( *input ) )continue;
    const Tracks* tracks = get<Tracks> ( *input ) ;
    // loop over all tracks in the container 
    for ( Tracks::const_iterator itrack = tracks->begin() ; 
          tracks->end() != itrack ; ++itrack ){
      const Track* track = *itrack ;
      ++nTracks ;
      if ( !use ( track ) ) { continue ; }                      // CONTINUE
      const bool result = m_tool->inAcceptance ( track ) ;
      if ( result ) { ++nAccept ; }
      // fill the relation table 
      table -> i_push ( track , result ) ;   // ATTENTION: i-push is used 
    }
  } 
  
  if ( 0 == nTracks ) { if (msgLevel(MSG::DEBUG)) debug() <<  "No good tracks have been selected" << endmsg ; }
  // MANDATORY: i_sort after i_push
  table -> i_sort () ;
  
  // a bit of statistics 
  if ( statPrint() || msgLevel ( MSG::DEBUG ) )
  {
    counter ( "#tracks" ) += nTracks ;
    counter ( "#accept" ) += nAccept ;
    counter ( "#links"  ) += table->i_relations().size() ;
    counter (Gaudi::Utils::toString(m_inputs) + "=>" + m_output ) += table->i_relations().size() ;
  }
    
  return StatusCode::SUCCESS ;
}
