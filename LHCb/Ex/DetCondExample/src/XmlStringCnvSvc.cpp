//$Id: XmlStringCnvSvc.cpp,v 1.2 2001-11-01 13:50:53 andreav Exp $
#include <string>

#include "XmlStringCnvSvc.h"

#include "XmlStringAddress.h"

#include "GaudiKernel/AddrFactory.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/IDataDirectory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/TimePoint.h"

/// Reference to the address factory for this storage type
extern const IAddrFactory& XmlStringAddressFactory;

/// Instantiation of a static factory to create instances of this service
static SvcFactory<XmlStringCnvSvc>          XmlStringCnvSvc_factory;
const ISvcFactory& XmlStringCnvSvcFactory = XmlStringCnvSvc_factory;

//----------------------------------------------------------------------------

/// Constructor
XmlStringCnvSvc::XmlStringCnvSvc( const std::string& name, 
				  ISvcLocator* svc)
  : ConversionSvc(name, svc, XMLSTRING_StorageType)
{
  setAddressFactory( &XmlStringAddressFactory );
}

//----------------------------------------------------------------------------

/// Destructor
XmlStringCnvSvc::~XmlStringCnvSvc()
{
}

//----------------------------------------------------------------------------

/// Initialize the service.
StatusCode XmlStringCnvSvc::initialize()
{

  // Before anything else, initialize the base class
  StatusCode status = ConversionSvc::initialize();
  if ( !status.isSuccess() ) return status;

  // Now we can get a handle to the MessageSvc
  MsgStream log(msgSvc(), "XmlStringCnvSvc" );
  log << MSG::DEBUG << "Initialization starting" << endreq;

  // Locate the DetectorDataSvc
  IDataProviderSvc* pDDS = 0;
  status = serviceLocator()->getService 
    ("DetectorDataSvc",  IID_IDataProviderSvc, (IInterface*&)pDDS);
  if ( !status.isSuccess() ) {
    log << MSG::ERROR << "Could not locate DetectorDataSvc" << endreq;
    return status;
  }
  
  // Set the detector data store
  status = setStore ( pDDS );
  if ( !status.isSuccess() ) {
    log << MSG::ERROR << "Could not set transient store" << endreq;
    return status;
  }
  
  log << MSG::DEBUG << "Initialization completed" << endreq;
  return status;
}

//----------------------------------------------------------------------------

/// Stop the service.
StatusCode XmlStringCnvSvc::finalize()
{
  MsgStream log(msgSvc(), "XmlStringCnvSvc" );
  log << MSG::DEBUG << "Finalizing" << endreq;
  return ConversionSvc::finalize();
}

//----------------------------------------------------------------------------

/// Create a transient representation from another representation of an object.
StatusCode XmlStringCnvSvc::createObj ( IOpaqueAddress* pAddress, 
					DataObject*&    refpObject) {
  
  /// This overloaded method:
  /// - does NOT register the object in the data store
  /// - does NOT associate the address to the object it created
  /// - therefore, its implementation does NOT call the base class method

  MsgStream log(msgSvc(), "XmlStringCnvSvc" );
  log << MSG::DEBUG << "Method createObj() starting" << endreq;

  const CLID&  obj_class = pAddress->clID();
  IConverter*  cnv       = converter(obj_class);
  StatusCode   status    = NO_CONVERTER;
  if ( 0 != cnv )   {

    status = updateServiceState(pAddress);
    if ( status.isSuccess() )  {

      status = cnv->createObj(pAddress, refpObject);
      if ( status.isSuccess() )  {

	status = cnv->fillObjRefs(pAddress, refpObject);
	if ( status.isSuccess() )  {
	  
	  log << MSG::DEBUG 
	      << "Method createObj() exiting successfully" << endreq;
	  return status;
	  
	} else {
	  
	  log << MSG::ERROR 
	      << "Could not fill object references" << endreq;
	}

      } else {
	
	log << MSG::ERROR 
	    << "Could not create object" << endreq;
      }

    } else {

      log << MSG::ERROR 
	  << "Could not update service state" << endreq;
    }

  } else {

    log << MSG::ERROR 
	<< "No converter found for classID " << obj_class << endreq;

  }

  // On fatal errors throw exception
  refpObject = 0;
  return status;

}

//----------------------------------------------------------------------------
/// Update a transient representation from another representation of an object.
StatusCode XmlStringCnvSvc::updateObj ( IOpaqueAddress* pAddress, 
					DataObject*     pObject) {
  
  /// This overloaded method:
  /// - does NOT register the object in the data store
  /// - does NOT associate the address to the object it created
  /// - therefore, its implementation does NOT call the base class method

  MsgStream log(msgSvc(), "XmlStringCnvSvc" );
  log << MSG::DEBUG << "Method updateObj() starting" << endreq;

  const CLID&  obj_class = pAddress->clID();
  IConverter*  cnv       = converter(obj_class);
  StatusCode   status    = NO_CONVERTER;
  if ( 0 != cnv )   {

    status = updateServiceState(pAddress);
    if ( status.isSuccess() )  {

      status = cnv->updateObj(pAddress, pObject);
      if ( status.isSuccess() )  {

	status = cnv->updateObjRefs(pAddress, pObject);
	if ( status.isSuccess() )  {
	  
	  log << MSG::DEBUG 
	      << "Method updateObj() exiting successfully" << endreq;
	  return status;
	  
	} else {
	  
	  log << MSG::ERROR 
	      << "Could not update object references" << endreq;
	}

      } else {
	
	log << MSG::ERROR 
	    << "Could not update object" << endreq;
      }

    } else {

      log << MSG::ERROR 
	  << "Could not update service state" << endreq;
    }

  } else {

    log << MSG::ERROR 
	<< "No converter found for classID " << obj_class << endreq;

  }

  // On fatal errors throw exception
  return status;

}

//----------------------------------------------------------------------------

