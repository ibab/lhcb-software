//$Id: XmlStringCnvSvc.cpp,v 1.3 2001-11-23 18:08:59 andreav Exp $
#include <string>

#include "XmlStringCnvSvc.h"

#include "XmlStringAddress.h"

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/TimePoint.h"

/// Instantiation of a static factory to create instances of this service
static SvcFactory<XmlStringCnvSvc>          XmlStringCnvSvc_factory;
const ISvcFactory& XmlStringCnvSvcFactory = XmlStringCnvSvc_factory;

//----------------------------------------------------------------------------

/// Constructor
XmlStringCnvSvc::XmlStringCnvSvc( const std::string& name, 
				  ISvcLocator* svc)
  : ConversionSvc(name, svc, XMLSTRING_StorageType)
{
  setAddressCreator( this );
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
  log << MSG::DEBUG << "Specific initialization starting" << endreq;

  // Locate the DetectorDataSvc
  IDataProviderSvc* pDDS = 0;
  status = serviceLocator()->getService 
    ("DetectorDataSvc",  IID_IDataProviderSvc, (IInterface*&)pDDS);
  if ( !status.isSuccess() ) {
    log << MSG::ERROR << "Could not locate DetectorDataSvc" << endreq;
    return status;
  }
  
  // Set the detector data store
  status = setDataProvider ( pDDS );
  if ( !status.isSuccess() ) {
    log << MSG::ERROR << "Could not set transient store" << endreq;
    return status;
  }
  
  log << MSG::DEBUG << "Specific initialization completed" << endreq;
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

/// Create an address using explicit arguments to identify a single object.
StatusCode XmlStringCnvSvc::createAddress( unsigned char svc_type,
					   const CLID& clid,
					   const std::string* par, 
					   const unsigned long*,
					   IOpaqueAddress*& refpAddress) {

  MsgStream log(msgSvc(), "XmlStringCnvSvc" );

  // First check that address is of type XMLSTRING_StorageType
  if ( svc_type!= XMLSTRING_StorageType ) {
    log << MSG::ERROR 
	<< "Cannot create addresses of type " << (int)svc_type 
	<< " which is different from " << (int)XMLSTRING_StorageType 
	<< endreq;
    return StatusCode::FAILURE;
  }
  
  // Now create the address
  refpAddress = new XmlStringAddress ( clid, par[0] );
  return StatusCode::SUCCESS;

}


//----------------------------------------------------------------------------

