// $Id: CaloHistoTrackAlg.cpp,v 1.5 2007-01-15 07:52:59 cattanem Exp $
// ============================================================================
// Include files
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
