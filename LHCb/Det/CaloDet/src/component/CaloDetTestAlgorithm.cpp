// $Id: CaloDetTestAlgorithm.cpp,v 1.6 2008-09-26 15:45:39 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2008/06/05 06:37:24  cattanem
// fix compiler warning
//
// Revision 1.4  2007/12/05 16:36:20  odescham
// update CaloDetTestAlgorithm
//
// Revision 1.3  2006/12/14 10:48:18  ranjard
// v8r0 - InstallArea and new Plugins
//
// Revision 1.2  2005/12/16 17:12:40  odescham
// v8r0 - LHCb v20 migration + cleaning
//
// Revision 1.1  2001/12/15 18:28:17  ibelyaev
//  update for new policy of versions and new test algorithm
//
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h" 
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
DECLARE_ALGORITHM_FACTORY(CaloDetTestAlgorithm)
// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloDetTestAlgorithm::CaloDetTestAlgorithm( const std::string& name   ,
                            ISvcLocator*       svcloc )
  : GaudiAlgorithm ( name , svcloc ) 
  , m_DetData(  )
{
  declareProperty("DetDataLocation" , m_DetData ) ;
  
  int index = name.find_last_of(".") +1 ; // return 0 if '.' not found --> OK !!
  std::string detectorName = name.substr( index, 4 ); 
  if ( name.substr(index,3) == "Prs" ) detectorName = "Prs";
  if ( name.substr(index,3) == "Spd" ) detectorName = "Spd";
  if("Ecal" == detectorName){
    m_DetData = DeCalorimeterLocation::Ecal; 
  }else  if("Hcal" == detectorName){
    m_DetData = DeCalorimeterLocation::Hcal; 
  }else  if("Prs" == detectorName){
    m_DetData = DeCalorimeterLocation::Prs; 
  }else  if("Spd" == detectorName){
    m_DetData = DeCalorimeterLocation::Spd; 
  }
  
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


  const DeCalorimeter* calo = getDet<DeCalorimeter>( m_DetData ) ;
  // Hack to fix compiler warning
  debug() << "Detector element found at " << calo << endmsg;
  
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

  return StatusCode::SUCCESS;

};

// ============================================================================
// The End 
// ============================================================================
