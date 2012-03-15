// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "CaloTrackMatchAlg.h"
#include "ToString.h"
// ============================================================================
/** @file 
 *  Implementation file for class CaloTrackMatchAlg
 *  @date 2006-06-16 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
/// Standard protected constructor
// ============================================================================
CaloTrackMatchAlg::CaloTrackMatchAlg
( const std::string& name , 
  ISvcLocator*       pSvc ) 
  : CaloTrackAlg ( name , pSvc ) 
  , m_tracks    ()
  , m_calos     ()
  , m_output    () 
  , m_toolName  () 
  , m_filter    () 
  , m_threshold ( 10000 )
  //
  , m_match     ( 0 ) 
{
  declareProperty ( "Tracks"    , m_tracks    ) ;
  declareProperty ( "Calos"     , m_calos     ) ;
  declareProperty ( "Output"    , m_output    ) ;
  declareProperty ( "Tool"      , m_toolName  ) ;
  declareProperty ( "Filter"    , m_filter    ) ;
  declareProperty ( "Threshold" , m_threshold ) ;
  // context-dependent default track container 
  m_tracks.clear();
  m_tracks = LHCb::CaloAlgUtils::TrackLocations( context() ) ;
}
// ============================================================================
/// standard algorithm itinialization
// ============================================================================
StatusCode CaloTrackMatchAlg::initialize() 
{
  StatusCode sc = CaloTrackAlg::initialize();
  if ( sc.isFailure() ) { return sc ; }
  //
  if ( m_tracks   .empty() ) { Warning ( "empty 'Tracks'-list"   ).ignore() ; }
  if ( m_calos    .empty() ) { Warning ( "empty 'Calos'-list"    ).ignore() ; }
  if ( m_output   .empty() ) { Warning ( "empty 'Output'-value"  ).ignore() ; }
  if ( m_toolName .empty() ) { Warning ( "empty 'Tool'-value"    ).ignore() ; }
  if ( m_filter   .empty() ) { Warning ( "empty 'Filter'-value"  ).ignore() ; }
  //
  return StatusCode::SUCCESS;
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


