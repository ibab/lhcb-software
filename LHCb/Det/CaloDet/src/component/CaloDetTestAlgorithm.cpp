// $Id: CaloDetTestAlgorithm.cpp,v 1.2 2005-12-16 17:12:40 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2001/12/15 18:28:17  ibelyaev
//  update for new policy of versions and new test algorithm
//
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h" 
// CaloDet
#include "CaloDet/DeCalorimeter.h" 
// local
#include "CaloDetTestAlgorithm.h"

// ============================================================================
/** @file CaloDetTestAlgorithm.cpp
 * 
 *  Implementation file for class : CaloDetTestAlgorithm
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloDetTestAlgorithm>         s_Factory ;
const        IAlgFactory&CaloDetTestAlgorithmFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloDetTestAlgorithm::CaloDetTestAlgorithm( const std::string& name   ,
                            ISvcLocator*       svcloc )
  : GaudiAlgorithm ( name , svcloc ) 
  , m_DetData( DeCalorimeterLocation::Ecal  )
{
  declareProperty("DetDataLocation" , m_DetData ) ;
};

// ============================================================================
/** destructor
 */
// ============================================================================
CaloDetTestAlgorithm::~CaloDetTestAlgorithm() {};

// ============================================================================
/** standard algorithm initialization 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloDetTestAlgorithm::initialize() 
{
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  
  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!",sc);}
  
  return StatusCode::SUCCESS;
};

// ============================================================================
/** standard algorithm finalization 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloDetTestAlgorithm::finalize() 
{
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;
  
  /// finalize the base class 
  return GaudiAlgorithm::finalize();
};

// ============================================================================
/** standard algorithm execution 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloDetTestAlgorithm::execute() 
{
  using namespace CaloCellCode;
  
  static unsigned int s_nExecute = 0 ;
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  if( 0 != s_nExecute++ ) { return StatusCode::SUCCESS ; } ///< RETURN !!!
  
  const DeCalorimeter* calo = getDet<DeCalorimeter>( m_DetData ) ;
  
  /// only for Ecal
  if( 2 != CaloNumFromName( m_DetData ) ){ return StatusCode::SUCCESS ; }
  
  /// some "arbitrary" cells 
  {  
    const LHCb::CaloCellID cell1( CaloNumFromName( m_DetData) , 0 , 20 , 20 ) ;
    log << MSG::DEBUG 
        << " info for cell id "    << cell1                        << endreq ;
    log << " valid      = "           << calo->valid      ( cell1 ) << endreq ;
    log << " cellX      = "           << calo->cellX      ( cell1 ) << endreq ;
    log << " celly      = "           << calo->cellY      ( cell1 ) << endreq ;
    log << " cellZ      = "           << calo->cellZ      ( cell1 ) << endreq ;
    log << " cellSize   = "           << calo->cellSize   ( cell1 ) << endreq ;
    log << " cellSine   = "           << calo->cellSine   ( cell1 ) << endreq ;
    log << " cellGain   = "           << calo->cellGain   ( cell1 ) << endreq ;
    log << " cellTime   = "           << calo->cellTime   ( cell1 ) << endreq ;
    log << " cellIndex  = "           << calo->cellIndex  ( cell1 ) << endreq ;
    log << " cardNumber = "           << calo->cardNumber ( cell1 ) << endreq ;
    log << " cardRow    = "           << calo->cardRow    ( cell1 ) << endreq ;
    log << " cardColumn = "           << calo->cardColumn ( cell1 ) << endreq ;
  }
  {  
    const LHCb::CaloCellID cell1( CaloNumFromName( m_DetData) , 1 , 16 , 36 ) ;
    log << MSG::DEBUG 
        << " info for cell id "    << cell1                        << endreq ;
    log << " valid      = "           << calo->valid      ( cell1 ) << endreq ;
    log << " cellX      = "           << calo->cellX      ( cell1 ) << endreq ;
    log << " celly      = "           << calo->cellY      ( cell1 ) << endreq ;
    log << " cellZ      = "           << calo->cellZ      ( cell1 ) << endreq ;
    log << " cellSize   = "           << calo->cellSize   ( cell1 ) << endreq ;
    log << " cellSine   = "           << calo->cellSine   ( cell1 ) << endreq ;
    log << " cellGain   = "           << calo->cellGain   ( cell1 ) << endreq ;
    log << " cellTime   = "           << calo->cellTime   ( cell1 ) << endreq ;
    log << " cellIndex  = "           << calo->cellIndex  ( cell1 ) << endreq ;
    log << " cardNumber = "           << calo->cardNumber ( cell1 ) << endreq ;
    log << " cardRow    = "           << calo->cardRow    ( cell1 ) << endreq ;
    log << " cardColumn = "           << calo->cardColumn ( cell1 ) << endreq ;
  }
  {  
    const LHCb::CaloCellID cell1( CaloNumFromName( m_DetData) , 2 , 20 , 15 ) ;
    log << MSG::DEBUG 
        << " info for cell id "    << cell1                        << endreq ;
    log << " valid      = "           << calo->valid      ( cell1 ) << endreq ;
    log << " cellX      = "           << calo->cellX      ( cell1 ) << endreq ;
    log << " celly      = "           << calo->cellY      ( cell1 ) << endreq ;
    log << " cellZ      = "           << calo->cellZ      ( cell1 ) << endreq ;
    log << " cellSize   = "           << calo->cellSize   ( cell1 ) << endreq ;
    log << " cellSine   = "           << calo->cellSine   ( cell1 ) << endreq ;
    log << " cellGain   = "           << calo->cellGain   ( cell1 ) << endreq ;
    log << " cellTime   = "           << calo->cellTime   ( cell1 ) << endreq ;
    log << " cellIndex  = "           << calo->cellIndex  ( cell1 ) << endreq ;
    log << " cardNumber = "           << calo->cardNumber ( cell1 ) << endreq ;
    log << " cardRow    = "           << calo->cardRow    ( cell1 ) << endreq ;
    log << " cardColumn = "           << calo->cardColumn ( cell1 ) << endreq ;
  }
  

  return StatusCode::SUCCESS;

};

// ============================================================================
// The End 
// ============================================================================
