//$Id: TestConditionsDBDataSvc.cpp,v 1.4 2001-11-29 11:04:32 andreav Exp $
#include <stdio.h>

#include "ConditionData.h"
#include "TestConditionsDBDataSvc.h"

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

  std::string pathName = "/dd/Conditions/LHCb/Slow/temp";
  log << MSG::INFO 
      << "Retrieve valid condition from path" << pathName << endreq;

  DataObject* anObject;
  StatusCode sc;
  sc = i_retrieveValidCondition ( anObject, pathName );
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

/// Create a valid DataObject by CondDB folder name (for default tag and key),
/// then load it in the TDS using the implicit naming convention:
/// if the DataObject exists already, update it instead (if necessary).
/// Specify the classID of the DataObject and the technology type for 
/// the strings stored in the CondDB. 
StatusCode 
TestConditionsDBDataSvc::i_retrieveValidCondition( DataObject*& refpObject, 
						   const std::string& path ) {

  MsgStream log(msgSvc(), "TestConditionsDBDataSvc" );
  log << MSG::DEBUG 
      << "Retrieving a valid DataObject from path:" << endreq;
  log << MSG::DEBUG << path << endreq;

  // Check if the event time is set
  if ( !m_detDataSvc->validEventTime() ) {
    log << MSG::ERROR << "The event time is not set yet" << endreq;
    return StatusCode::FAILURE;
  }

  // Retrieve or update the DataObject associated to that RegistryEntry
  if ( !detSvc()->findObject(path,refpObject).isSuccess() ) {
    log << MSG::DEBUG 
	<< "DataObject not loaded yet: retrieve it" << endreq;
    return detSvc()->retrieveObject( path, refpObject );
  } else {
    log << MSG::DEBUG 
	<< "DataObject already loaded: update it if necessary" << endreq;
    return detSvc()->updateObject( refpObject );
  }

}

//----------------------------------------------------------------------------
