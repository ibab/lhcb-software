//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetCond/src/component/ConditionDataSvc.cpp,v 1.1.1.1 2001-09-14 15:07:21 andreav Exp $
#include <string>

#include "ConditionDataSvc.h"

#include "DetCond/ConditionData.h"
#include "DetCond/IConditionAddress.h"

#include "GaudiKernel/AddrFactory.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/TimePoint.h"

//----------------------------------------------------------------------------

/// Constructor
ConditionDataSvc::ConditionDataSvc(const std::string& name,
				   ISvcLocator* svc) 
  : DataSvc(name,svc)   
  , m_eventTime          ( 0 )
{
}

//----------------------------------------------------------------------------

/// Destructor
ConditionDataSvc::~ConditionDataSvc()  {
  if( 0 != m_eventTime ) delete m_eventTime; 
  //DataSvc::~DataSvc;
  setDataLoader(0);
  clearStore();
}

//----------------------------------------------------------------------------

/// Initialize the service.
StatusCode ConditionDataSvc::initialize()
{

  // Initialize the base class
  StatusCode sc = DataSvc::initialize();
  if ( !sc.isSuccess() ) return sc;

  // Now we can get a handle to the MessageSvc
  // Set the output level for name "ConditionDataSvc"
  msgSvc()->setOutputLevel( "ConditionDataSvc", m_outputLevel );
  MsgStream log(msgSvc(), "ConditionDataSvc" );
  log << MSG::DEBUG << "Initialize()" << endreq;

  return sc;
}

//----------------------------------------------------------------------------

/// Query interface
StatusCode ConditionDataSvc::queryInterface(const IID& riid, 
					    void** ppvInterface)
{
  if ( IID_IConditionDataSvc == riid )  {
    // With the highest priority return the specific interface of this service
    *ppvInterface = (IConditionDataSvc*)this;
  } else  {
    // Interface is not directly available: try out a base class
    return DataSvc::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

/// Set the new event time 
void ConditionDataSvc::setEventTime ( const ITime& time ) {
  if( 0 != m_eventTime ) delete m_eventTime; 
  m_eventTime = new TimePoint( time );   
  MsgStream log( msgSvc(), "ConditionDataSvc" );
  log << MSG::DEBUG 
      << "Event Time set to " << m_eventTime->absoluteTime() << endreq;
}

//----------------------------------------------------------------------------

/// Get the event time  
const ITime* ConditionDataSvc::eventTime ( ) { 
  return m_eventTime; 
}

//----------------------------------------------------------------------------

/// Update object
/// TODO: make sure that all parent objects are up-to-date as well
StatusCode ConditionDataSvc::updateObject( DataObject* toUpdate ) {

  MsgStream log( msgSvc(), "ConditionDataSvc" );
  log << MSG::DEBUG << "Method updateObject starting" << endreq;

  if ( 0 == toUpdate ) { 
    log << MSG::ERROR
	<< "There is no DataObject to update" << endreq;
    return INVALID_OBJECT; 
  }

  ConditionData* condition = dynamic_cast<ConditionData*>( toUpdate );
  if ( 0 == condition ) {
    log << MSG::ERROR
	<< "DataObject to update is not a ConditionData" << endreq;
    return StatusCode::FAILURE;
  }

  // Set the new event time in the address (even if object is still valid!)
  if ( 0 == toUpdate->directory() )    {
    log << MSG::ERROR 
	<< "The object is not registered in a DataDirectory" << endreq; 
    return INVALID_OBJ_ADDR;
  }
  if ( 0 == toUpdate->directory()->address() )    {
    return INVALID_OBJ_ADDR;
    log << MSG::ERROR 
	<< "The object has no associated address" << endreq; 
  } 
  IConditionAddress* addr;
  addr = dynamic_cast< IConditionAddress* >
    ( toUpdate->directory()->address() );
  if ( 0 == addr ) {
    log << MSG::ERROR 
	<< "Object address is not an IConditionAddress" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG 
      << "Setting new event time in the object address" << endreq;
  addr->setTime( *m_eventTime );

  // No need to update if condition is valid
  if ( condition->isValid( *m_eventTime ) ) {
    log << MSG::DEBUG 
	<< "DataObject is a valid ConditionData: no need to update" << endreq;
    return StatusCode::SUCCESS;
  } else {
    log << MSG::DEBUG 
	<< "DataObject is an invalid ConditionData: update it" << endreq;
  }

  // Now delegate update to the conversion service by calling the base class
  log << MSG::DEBUG 
      << "Delegate update to relevant conversion service" << endreq;
  StatusCode status = DataSvc::updateObject(toUpdate);
  if ( !status.isSuccess() ) {
    log << MSG::ERROR 
	<< "Could not update DataObject" << endreq; 
    if ( status == NO_DATA_LOADER )
      log << MSG::ERROR << "There is no data loader" << endreq; 
    return status;
  } 

  // Now cross-check that the new condition is valid
  condition = dynamic_cast<ConditionData*>(toUpdate);
  if ( 0 == condition ) {
    log << MSG::ERROR
	<< "Updated DataObject is not a ConditionData" << endreq;
    return StatusCode::FAILURE;
  }
  if ( !condition->isValid( *m_eventTime ) ) {
    log << MSG::ERROR
	<< "Updated ConditionData is not valid" << endreq;
    log << MSG::ERROR
	<< "Are you sure the conversion service has updated it?" << endreq;
    return StatusCode::FAILURE;
  } 

  // DataObject was successfully updated
  log << MSG::DEBUG << "Method updateObject exiting successfully" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------



