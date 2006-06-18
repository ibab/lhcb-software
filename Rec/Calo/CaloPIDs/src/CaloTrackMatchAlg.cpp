// $Id: CaloTrackMatchAlg.cpp,v 1.1 2006-06-18 18:35:28 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
}
// ============================================================================
/// standard algorithm itinialization
// ============================================================================
StatusCode CaloTrackMatchAlg::initialize() 
{
  StatusCode sc = CaloTrackAlg::initialize();
  if ( sc.isFailure() ) { return sc ; }
  //
  if ( m_tracks   .empty() ) { Warning ( "empty 'Tracks'-list"   ) ; }
  if ( m_calos    .empty() ) { Warning ( "empty 'Calos'-list"    ) ; }
  if ( m_output   .empty() ) { Warning ( "empty 'Output'-value"  ) ; }
  if ( m_toolName .empty() ) { Warning ( "empty 'Tool'-value"    ) ; }
  if ( m_filter   .empty() ) { Warning ( "empty 'Filter'-value"  ) ; }
  //
  return StatusCode::SUCCESS;
} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


