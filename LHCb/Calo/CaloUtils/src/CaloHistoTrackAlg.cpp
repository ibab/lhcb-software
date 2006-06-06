// $Id: CaloHistoTrackAlg.cpp,v 1.4 2006-06-06 11:59:52 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/03/22 18:25:06  odescham
// Migration to new Event Model
//
// Revision 1.2  2005/11/07 11:57:13  odescham
// v5r0 - Adapt to the new Track Event Model
//
// Revision 1.1  2005/05/06 17:48:33  ibelyaev
//  add new base classes: Calo(Histo)TrackAlg
// Revision 1.2 2005/10/14 odescham
// adapt to new track model
// ============================================================================
// Include files
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/MsgStream.h" 
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
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
  : GaudiHistoAlg ( name , svcloc ) 
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
  StatusCode sc = GaudiHistoAlg::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  always() << m_use << endreq ;
  
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
{ return GaudiHistoAlg::finalize() ; }
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
