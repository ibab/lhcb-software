//$Id: TestSmartDataPtr.cpp,v 1.4 2001-11-29 11:00:48 andreav Exp $
#include <stdio.h>

#include "ConditionData.h"
#include "TestSmartDataPtr.h"

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

  log << MSG::INFO << "Initialization completed" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

StatusCode TestSmartDataPtr::execute( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "************* new event  *******************************************"
      << endreq;
  log << MSG::INFO << "Execute()" << endreq;

  std::string tdsName = "/dd/Conditions/LHCb/Ecal/Slow/temp";
  log << MSG::INFO 
      << "Retrieve smart data pointer to path " << tdsName << endreq;
  
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
