//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/src/XmlStringTemperatureCnv.cpp,v 1.1.1.1 2001-09-14 15:45:03 andreav Exp $
#include <string>

#include "XmlStringTemperatureCnv.h"
#include "XmlStringAddress.h"

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"

/// Instantiation of a static factory to create instances of this converter
static CnvFactory<XmlStringTemperatureCnv> s_factory;
const ICnvFactory& XmlStringTemperatureCnvFactory = s_factory;

//----------------------------------------------------------------------------

/// Constructor
XmlStringTemperatureCnv::XmlStringTemperatureCnv( ISvcLocator* svc ) 
  : Converter ( XMLSTRING_StorageType, CLID_Temperature, svc )
{
}

//----------------------------------------------------------------------------

/// Initialize the converter
StatusCode XmlStringTemperatureCnv::initialize()      {

  // Initialize the base class
  StatusCode status = Converter::initialize();
  if ( !status.isSuccess() ) return StatusCode::FAILURE;
  
  // Now you can get a handle to the MsgSvc
  // Set the output levelequal to that of the conversion service
  msgSvc()->setOutputLevel( "XmlStringTemperatureCnv",
			    msgSvc()->outputLevel( "XmlStringCnvSvc" ) );
  MsgStream log( msgSvc(), "XmlStringTemperatureCnv" );
  log << MSG::DEBUG << "Initializing" << endreq;
  
  // Locate the Detector Data Service
  IDataProviderSvc* dp;
  status = serviceLocator()->getService 
    ( "DetectorDataSvc", IID_IDataProviderSvc, (IInterface*&)dp );
  if ( !status.isSuccess() ) { 
    log << MSG::ERROR << "Could not locate DetectorDataSvc" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Succesfully located DetectorDataSvc" << endreq;
  }
  setDataProvider( dp );

  // Initialization completed
  log << MSG::DEBUG << "Initialization completed" << endreq;
  return status;

}

//----------------------------------------------------------------------------

/// Finalize the converter
StatusCode XmlStringTemperatureCnv::finalize()      {
  
  MsgStream log(msgSvc(), "XmlStringTemperatureCnv" );
  log << MSG::DEBUG 
      << "Finalizing convertor for CLID=" << objType() << endreq;

  // Finalize the base class
  return Converter::finalize();

}

//----------------------------------------------------------------------------

/// Create a transient object from a persistent representation
StatusCode XmlStringTemperatureCnv::createObj( IOpaqueAddress* pAddress,
					       DataObject*&    refpObject ) {

  MsgStream log(msgSvc(), "XmlStringTemperatureCnv" );
  log << MSG::DEBUG << "Method createObj starting" << endreq;

  // Create empty Temperature object
  log << MSG::DEBUG << "Create a new (empty) Temperature object" << endreq;
  Temperature* aTemp = new Temperature ( );
  
  // Set its attributes by updating it
  log << MSG::DEBUG << "Update attributes of the Temperature object" << endreq;
  StatusCode sc = updateObj( pAddress, aTemp );
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not update object" << endreq;
    delete aTemp; 
    refpObject = 0;
  } else {
    refpObject = aTemp;
  }
    
  // Now create the new DataObject
  log << MSG::DEBUG << "Method createObj exiting" << endreq;
  return sc;

}

//----------------------------------------------------------------------------

/// Create a transient object from a persistent representation
StatusCode XmlStringTemperatureCnv::updateObj( IOpaqueAddress* pAddress,
					       DataObject*     pObject ) {

  MsgStream log(msgSvc(), "XmlStringTemperatureCnv" );
  log << MSG::DEBUG << "Method updateObj starting" << endreq;

  // Is the object a Temperature?
  if ( 0 == pObject ) {
    log << MSG::ERROR << "There is no DataObject to update" << endreq;
    return StatusCode::FAILURE;
  }
  Temperature* aTemp = dynamic_cast< Temperature* >( pObject );
  if ( 0 == aTemp ) {
    log << MSG::ERROR << "DataObject is not a Temperature" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG << "DataObject is a Temperature" << endreq;

  // Is the address an XmlStringAddress?
  if ( 0 == pAddress ) {
    log << MSG::ERROR << "There is no address to update the object" << endreq;
    return StatusCode::FAILURE;
  }
  XmlStringAddress* addr = dynamic_cast< XmlStringAddress* >( pAddress );
  if ( 0 == addr ) {
    log << MSG::ERROR << "Address is not a XmlStringAddress" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG << "IOpaqueAddress is a XmlStringAddress" << endreq;

  // Interpret the Xml string
  std::string xmlString = addr->xmlString();
  log << MSG::DEBUG << "xmlString is '" << xmlString << "'" << endreq;
  double temperature;
  std::string prefix = "<temperature value=";
  std::string suffix = ">";
  if ( xmlString.find(prefix) == 0 &&
       xmlString.find(suffix) == 3+prefix.size() ) {
    const char* ch3 = xmlString.substr(prefix.size(),3).c_str();
    if ( strspn ( ch3, " .0123456789" ) == 3 ) {
      temperature = atof ( ch3 );
      log << MSG::DEBUG
	  << "Xml string decoded: temperature = " << temperature << endreq;
    } else {
      log << MSG::ERROR 
	  << "Xml string does not have the required format" << endreq;
      return StatusCode::FAILURE;
    }
  } else {
    log << MSG::ERROR 
	<< "Xml string does not have the required format" << endreq;
    return StatusCode::FAILURE;
  }  

  // Now update the Temperature object
  log << MSG::DEBUG << "Updating Temperature object" << endreq;
  aTemp->setTemperature( temperature );
    
  log << MSG::DEBUG << "Method updateObj exiting" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

