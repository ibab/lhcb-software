// $Id: CaloTrackMatchAlg.cpp,v 1.3 2009-05-15 12:53:38 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
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
  , m_tracks    ( 1 , LHCb::TrackLocation::Default )
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

  if( "HLT" == context() ){
    m_tracks.clear();
    //m_tracks.push_back( "Hlt/Track/ForwardCLEANED" );
    m_tracks.push_back(  LHCb::TrackLocation::HltForward );
  }  
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
} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


