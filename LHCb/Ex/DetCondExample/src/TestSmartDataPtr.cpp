//$Id: TestSmartDataPtr.cpp,v 1.3 2001-11-27 18:31:15 andreav Exp $
#include <stdio.h>

#include "ConditionData.h"
#include "TestSmartDataPtr.h"

#include "DetCond/IConditionsDBDataSvc.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/TimePoint.h"

/// Instantiation of a static factory to create instances of this algorithm
static const AlgFactory<TestSmartDataPtr> Factory;
const IAlgFactory& TestSmartDataPtrFactory = Factory;

//----------------------------------------------------------------------------

/// Constructor
TestSmartDataPtr::TestSmartDataPtr( const std::string&  name, 
				    ISvcLocator*        pSvcLocator )
  : Algorithm(name, pSvcLocator)
{
}

//----------------------------------------------------------------------------

/// Initialization of the algorithm. 
/// It creates the environment needed for processing the events. 
/// In this example this is the main method, where we play with the CondDB.

StatusCode TestSmartDataPtr::initialize() {

  // Retrieve sample conditions from the data store through smart pointers
  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "============= Test of SmartDataPtr starting ========================"
      << endreq;
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

  log << MSG::INFO << "Initialization completed" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

StatusCode TestSmartDataPtr::execute( ) {

  MsgStream log(msgSvc(), name());
  StatusCode sc;
  log << MSG::INFO 
      << "************* new event  *******************************************"
      << endreq;
  log << MSG::INFO << "Execute()" << endreq;

  std::string tdsName;
  std::string folderName = "/Conditions/LHCb/Ecal/Slow/temp";
  StatusCode status = 
    m_conditionsDBDataSvc->getNameInStore( tdsName, folderName );
  if ( !status.isSuccess() ) {
    log << MSG::ERROR 
	<< "Can't find name in store for folder " << folderName << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO 
      << "Retrieve smart data pointer to path " << tdsName << endreq;
  log << MSG::INFO 
      << "Data come from CondDB folder " << folderName << endreq;
  
  // HACK: init registry entries by ONE call to retrieveValidCondition
  // This cannot be done in initialize() because the event time must be set
  static bool first = true;
  if ( first ) {
    first = false;
    log << MSG::INFO 
	<< "Initialise registry entry tree for " 
	<< tdsName << endreq;
    DataObject* anObject;
    log << MSG::DEBUG 
	<< "Retrieve valid condition from folder" << folderName << endreq;
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
  }

  // Now retrieve the pointers: NB you have to update them manually!
  SmartDataPtr<ConditionData> aCondition( detSvc(), tdsName );
  if( !aCondition ) {
    log << MSG::ERROR 
	<< "Can't retrieve condition in the store: " << tdsName << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG 
      << "Retrieved smart data pointer to path " << tdsName << endreq;
  log << MSG::INFO 
      << "Before update, condition at time " 
      << m_detDataSvc->eventTime().absoluteTime() << ":" << endreq;
  log << MSG::INFO << (ConditionData*)aCondition << endreq;
  log << MSG::INFO << "Now update the condition" << endreq;
  if ( !( ( detSvc()->updateObject( (DataObject*)aCondition ) 
	    ).isSuccess() ) ) {
    log << MSG::ERROR
	<< "Can't update condition in the store: " << tdsName << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO 
      << "After update, condition at time " 
      << m_detDataSvc->eventTime().absoluteTime() << ":" << endreq;
  log << MSG::INFO << (ConditionData*)aCondition << endreq;
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode TestSmartDataPtr::finalize( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Finalize()" << endreq;
  
  log << MSG::INFO 
      << "============= Test of SmartDataPtr ending =========================="
      << endreq;

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------
