//$Id: TestConditionsDBDataSvc.cpp,v 1.3 2001-11-27 18:31:41 andreav Exp $
#include <stdio.h>

#include "ConditionData.h"
#include "TestConditionsDBDataSvc.h"

#include "DetCond/IConditionsDBDataSvc.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/TimePoint.h"

/// Instantiation of a static factory to create instances of this algorithm
static const AlgFactory<TestConditionsDBDataSvc> Factory;
const IAlgFactory& TestConditionsDBDataSvcFactory = Factory;

//----------------------------------------------------------------------------

/// Constructor
TestConditionsDBDataSvc::TestConditionsDBDataSvc ( const std::string& name, 
						   ISvcLocator* pSvcLocator )
  : Algorithm             ( name, pSvcLocator )
  , m_detDataSvc          ( 0 )
  , m_conditionsDBDataSvc ( 0 )
{
}

//----------------------------------------------------------------------------

/// Initialization of the algorithm. 
/// It creates the environment needed for processing the events. 
/// In this example this is the main method, where we play with the CondDB.

StatusCode TestConditionsDBDataSvc::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize()" << endreq;
  StatusCode sc;

  // Query the IDetDataSvc interface of the detector data service
  sc = detSvc()->queryInterface(IID_IDetDataSvc, 
				(void**) &m_detDataSvc);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 
	<< "Could not query IDetDataSvc interface of DetectorDataSvc" 
	<< endreq;
    return sc;
  } else {
    log << MSG::DEBUG 
	<< "Retrieved IDetDataSvc interface of DetectorDataSvc" 
	<< endreq;
  }

  // Query the IConditionsDBDataSvc interface of the detector data service
  sc = detSvc()->queryInterface(IID_IConditionsDBDataSvc, 
				(void**) &m_conditionsDBDataSvc);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 
	<< "Could not query IConditionsDBDataSvc interface of DetectorDataSvc" 
	<< endreq;
    return sc;
  } else {
    log << MSG::DEBUG 
	<< "Retrieved IConditionsDBDataSvc interface of DetectorDataSvc" 
	<< endreq;
  }

  // Retrieve sample conditions from the data store through ConditionsDBDataSvc
  log << MSG::INFO 
      << "============= Test of ConditionsDBDataSvc starting ================="
      << endreq;
  
  log << MSG::INFO << "Initialization completed" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

StatusCode TestConditionsDBDataSvc::execute( ) {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "************* new event  *******************************************"
      << endreq;
  log << MSG::INFO << "Execute()" << endreq;

  std::string folderName = "/Conditions/LHCb/Slow/temp";
  log << MSG::INFO 
      << "Retrieve valid condition from folder" << folderName << endreq;

  DataObject* anObject;
  StatusCode sc;
  sc = m_conditionsDBDataSvc->retrieveValidCondition ( anObject, 
						       folderName );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not retrieve DataObject" << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "A DataObject was retrieved" << endreq;
  }
  ConditionData* aCondition = dynamic_cast<ConditionData*>(anObject);
  if ( 0 == aCondition ) {
    log << MSG::ERROR 
	<< "Retrieved object is not a ConditionData" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Condition at time " 
      << m_detDataSvc->eventTime().absoluteTime() << ":" << endreq;
  log << MSG::INFO << aCondition << endreq;
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode TestConditionsDBDataSvc::finalize( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Finalize()" << endreq;
  log << MSG::INFO 
      << "============= Test of ConditionsDBDataSvc ending ==================="
      << endreq;  
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------
