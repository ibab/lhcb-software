// $Id: CaloTrackAlg.cpp,v 1.3 2004-10-27 12:40:08 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/10/26 20:51:09  ibelyaev
//  minor improvement in printout
//
// Revision 1.1  2004/10/26 20:35:58  ibelyaev
//  improve properties of all Track-related algorithms
//
// ============================================================================
// Include files
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/MsgStream.h" 
// ============================================================================
// Event 
// ============================================================================
#include "Event/TrStoredTrack.h"
// ============================================================================
// CaloKernel
// ============================================================================
#include "CaloKernel/CaloPrint.h"
// ============================================================================
// local
// ============================================================================
#include "CaloTrackAlg.h"
// ============================================================================

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
  : CaloAlgorithm ( name , svcloc ) 
  , m_use () 
{
  //
  m_use.declareProperties ( this ) ;
} ;
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
CaloTrackAlg::~CaloTrackAlg() {};
// ============================================================================

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTrackAlg::initialize()
{
  StatusCode sc = CaloAlgorithm::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  {
    std::string msg =  m_use.rejectedTypes() ;
    if ( !msg.empty()    ) 
    {
      msg =  "Rejected Track types      : " + msg ;
      info() << msg << endreq ;
      Warning ( msg , StatusCode::SUCCESS , 0 ) ;
    }
  }
  
  { 
    std::string msg =  m_use.rejectedCategories() ;
    if ( !msg.empty()     ) 
    {
      msg = "Rejected Track categories : " + msg ;
      info() << msg << endreq ;
      Warning ( msg , StatusCode::SUCCESS , 0 ) ;
    }
  }
  
  {
    std::string msg =  m_use.rejectedAlgorithms() ;
    if ( !msg.empty()    ) 
    {
      msg = "Rejected Track algorithms : " + msg ;
      info() << msg << endreq ;
      Warning ( msg , StatusCode::SUCCESS , 0 ) ;
    }
  }
  
  return StatusCode::SUCCESS ;  
};
// ============================================================================

// ============================================================================
/** standard algorithm finalization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTrackAlg::finalize  () { return CaloAlgorithm::finalize() ; }
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
