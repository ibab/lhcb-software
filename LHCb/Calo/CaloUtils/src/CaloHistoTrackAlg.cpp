// $Id: CaloHistoTrackAlg.cpp,v 1.1 2005-05-06 17:48:33 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
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
#include "CaloUtils/CaloHistoTrackAlg.h"
// ============================================================================

// ============================================================================
/** @file 
 * 
 *  Implementation file for class CaloHistoTrackAlg
 *  @see CaloHistoTrackAlg
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
CaloHistoTrackAlg::CaloHistoTrackAlg
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : CaloHistoAlg ( name , svcloc ) 
  , m_use () 
{
  //
  m_use.declareProperties ( this ) ;
} ;
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
CaloHistoTrackAlg::~CaloHistoTrackAlg() {};
// ============================================================================

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloHistoTrackAlg::initialize()
{
  StatusCode sc = CaloHistoAlg::initialize() ;
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
StatusCode CaloHistoTrackAlg::finalize  () 
{ return CaloHistoAlg::finalize() ; }
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
