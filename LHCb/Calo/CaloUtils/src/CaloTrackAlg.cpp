// $Id: CaloTrackAlg.cpp,v 1.4 2006-06-06 11:59:52 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
#include "GaudiKernel/MsgStream.h" 
#include "Event/Track.h"
#include "CaloUtils/CaloTrackAlg.h"
// ============================================================================
/** @file 
 * 
 *  Implementation file for class CaloCluster2TrackAlg
 *  @see CaloCluster2TrackAlg
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloTrackAlg::CaloTrackAlg
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : GaudiAlgorithm ( name , svcloc ) 
  , m_use () 
{
  //
  m_use.declareProperties ( this ) ;
  //
  setProperty ( "CheckTracks" , "true" ) ;
} ;
// ============================================================================
/// destructor
// ============================================================================
CaloTrackAlg::~CaloTrackAlg() {};
// ============================================================================
/// standard algorithm initialization 
// ============================================================================
StatusCode CaloTrackAlg::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  always() << m_use << endreq ;
  return StatusCode::SUCCESS ;  
};
// ============================================================================
/// standard algorithm finalization 
StatusCode CaloTrackAlg::finalize  () { return GaudiAlgorithm::finalize() ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
