//$Id: ConditionDataSvc.cpp,v 1.3 2001-11-26 19:06:49 andreav Exp $
#include <string>

#include "ConditionDataSvc.h"

#include "DetCond/IConditionAddress.h"

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/TimePoint.h"

//----------------------------------------------------------------------------

/// Constructor
ConditionDataSvc::ConditionDataSvc( const std::string& name,
				    ISvcLocator* svc        ) 
  : DataSvc              ( name, svc )   
  , m_eventTimeDefined   ( false     ) 
  , m_eventTime          ( 0         ) 
{
}

//----------------------------------------------------------------------------

/// Destructor
ConditionDataSvc::~ConditionDataSvc() 
{
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

  return sc;
}

//----------------------------------------------------------------------------

/// Finalize the service.
StatusCode ConditionDataSvc::finalize()
{

  MsgStream log(msgSvc(), "ConditionDataSvc" );
  log << MSG::DEBUG << "Finalizing" << endreq;

  // Delete the associated event time
  if( 0 != m_eventTime ) delete m_eventTime; 
  m_eventTimeDefined = false;

  // Finalize the base class
  return DataSvc::finalize();

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
  m_eventTimeDefined = true;
  if( 0 != m_eventTime ) delete m_eventTime; 
  m_eventTime = new TimePoint( time );   
  MsgStream log( msgSvc(), "ConditionDataSvc" );
  log << MSG::DEBUG 
      << "Event Time set to " << m_eventTime->absoluteTime() << endreq;
}

//----------------------------------------------------------------------------

/// Check if the event time has been set
const bool ConditionDataSvc::checkEventTime ( ) { 
  return m_eventTimeDefined; 
}

//----------------------------------------------------------------------------

/// Get the event time  
const ITime& ConditionDataSvc::eventTime ( ) { 
  return *m_eventTime; 
}

//----------------------------------------------------------------------------

/// Update object
/// TODO: update also its ancestors in the data store if necessary
StatusCode ConditionDataSvc::updateObject( DataObject* toUpdate ) {

  MsgStream log( msgSvc(), "ConditionDataSvc" );
  log << MSG::DEBUG << "Method updateObject starting" << endreq;

  // Check that object to update exists
  if ( 0 == toUpdate ) { 
    log << MSG::ERROR
	<< "There is no DataObject to update" << endreq;
    return INVALID_OBJECT; 
  }

  // Retrieve IValidity interface of object to update
  IValidity* condition = dynamic_cast<IValidity*>( toUpdate );
  if ( 0 == condition ) {
    log << MSG::WARNING
	<< "Cannot update DataObject: DataObject does not implement IValidity"
	<< endreq;
    return StatusCode::SUCCESS;
  }

  // Check if the event time has been defined
  if ( !checkEventTime() ) {
    log << MSG::WARNING
	<< "Cannot update DataObject: event time undefined"
	<< endreq; 
    return StatusCode::SUCCESS;
  }

  // Set the new event time in the address (even if object is still valid!)
  if ( 0 == toUpdate->registry() )    {
    log << MSG::ERROR 
	<< "The object is not registered in a DataDirectory" << endreq; 
    return INVALID_OBJ_ADDR;
  }
  if ( 0 == toUpdate->registry()->address() )    {
    return INVALID_OBJ_ADDR;
    log << MSG::ERROR 
	<< "The object has no associated address" << endreq; 
  } 
  IConditionAddress* addr;
  addr = dynamic_cast< IConditionAddress* >
    ( toUpdate->registry()->address() );
  if ( 0 == addr ) {
    log << MSG::ERROR 
	<< "Object address is not an IConditionAddress" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG 
      << "Setting new event time in the object address" << endreq;
  addr->setTime( eventTime() );

  // No need to update if condition is valid
  if ( condition->isValid( eventTime() ) ) {
    log << MSG::DEBUG 
	<< "DataObject is valid: no need to update" << endreq;
    return StatusCode::SUCCESS;
  } else {
    log << MSG::DEBUG 
	<< "DataObject is invalid: update it" << endreq;
  }

  // TODO: before loading updated object, update HERE its parent in data store

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
  condition = dynamic_cast<IValidity*>(toUpdate);
  if ( 0 == condition ) {
    log << MSG::ERROR
	<< "Updated DataObject does not implement IValidity" << endreq;
    return StatusCode::FAILURE;
  }
  if ( !condition->isValid( eventTime() ) ) {
    log << MSG::ERROR
	<< "Updated DataObject is not valid" << endreq;
    log << MSG::ERROR
	<< "Are you sure the conversion service has updated it?" << endreq;
    return StatusCode::FAILURE;
  } 

  // DataObject was successfully updated
  log << MSG::DEBUG << "Method updateObject exiting successfully" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------



