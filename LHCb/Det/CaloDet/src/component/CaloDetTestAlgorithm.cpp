// $Id: CaloDetTestAlgorithm.cpp,v 1.1 2001-12-15 18:28:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
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
  : CaloAlgorithm ( name , svcloc ) 
{};

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
  
  StatusCode sc = CaloAlgorithm::initialize();
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
  return CaloAlgorithm::finalize();
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
  
  SmartDataPtr<DeCalorimeter> calo( detSvc() , detData() ) ;
  if( !calo ) 
    { return Error("Could not locate the calorimneter='"+detData()+"'");} 
  log << MSG::DEBUG 
      << (const DeCalorimeter*) calo 
      << endreq;
  
  /// only for Ecal
  if( 2 != CaloNumFromName( detData() ) ) { return StatusCode::SUCCESS ; }
  
  /// some "arbitrary" cells 
  {  
    const CaloCellID cell1( CaloNumFromName( detData() ) , 0 , 20 , 20 ) ;
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
    const CaloCellID cell1( CaloNumFromName( detData() ) , 1 , 16 , 36 ) ;
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
    const CaloCellID cell1( CaloNumFromName( detData() ) , 2 , 20 , 15 ) ;
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
