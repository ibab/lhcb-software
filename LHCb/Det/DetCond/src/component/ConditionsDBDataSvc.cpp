//$Id: ConditionsDBDataSvc.cpp,v 1.7 2001-11-29 10:51:31 andreav Exp $
#include <string>

#include "ConditionsDBDataSvc.h"

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITime.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/SvcFactory.h"

/// Instantiation of a static factory to create instances of this service
static SvcFactory<ConditionsDBDataSvc>          ConditionsDBDataSvc_factory;
const ISvcFactory& ConditionsDBDataSvcFactory = ConditionsDBDataSvc_factory;

//----------------------------------------------------------------------------

/// Constructor
ConditionsDBDataSvc::ConditionsDBDataSvc( const std::string& name, 
					  ISvcLocator* svc )
  : ConditionDataSvc(name,svc)
{
  m_rootName           = "/dd";
  m_rootCLID           = CLID_Catalog;
}

//----------------------------------------------------------------------------

/// Destructor
ConditionsDBDataSvc::~ConditionsDBDataSvc()
{
}

//----------------------------------------------------------------------------

/// Initialize the service.
StatusCode ConditionsDBDataSvc::initialize()
{

  // Initialize the base class
  StatusCode sc = ConditionDataSvc::initialize();
  if ( !sc.isSuccess() ) return sc;

  // Now we can get a handle to the MessageSvc
  // Set the output level for name "ConditionsDBDataSvc"
  msgSvc()->setOutputLevel( "ConditionsDBDataSvc", m_outputLevel );
  MsgStream log(msgSvc(), "ConditionsDBDataSvc" );
  log << MSG::DEBUG << "Specific initialization starting" << endreq;

  // Locate the DetectorPersistencySvc and set it as data loader
  IConversionSvc* pers_svc;
  sc = serviceLocator()->service("DetectorPersistencySvc", pers_svc, true);
  if( sc .isFailure() ) {
    log << MSG::ERROR 
	<< " Unable to locate DetectorPersistencySvc" << endreq; 
    return sc;
  } else {
    sc = setDataLoader( pers_svc );
    if( sc .isFailure() ) {
      log << MSG::ERROR 
	  << " Unable to set DetectorPersistencySvc as data loader" << endreq; 
      return sc;
    }
  }

  log << MSG::DEBUG << "Specific initialization completed" << endreq;
  return sc;
}

//----------------------------------------------------------------------------

/// Finalize the service.
StatusCode ConditionsDBDataSvc::finalize()
{

  MsgStream log(msgSvc(), "ConditionsDBDataSvc" );
  log << MSG::DEBUG << "Finalizing" << endreq;

  // Finalize the base class
  return ConditionDataSvc::finalize();

}

//----------------------------------------------------------------------------

