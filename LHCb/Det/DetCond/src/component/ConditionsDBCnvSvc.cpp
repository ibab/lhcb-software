//$Id: ConditionsDBCnvSvc.cpp,v 1.5 2001-11-26 20:16:35 andreav Exp $
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>

#include "ConditionsDBCnvSvc.h"
#include "ConditionsDBAddress.h"
#include "ConditionsDBGate.h"

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/TimePoint.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"

/// Instantiation of a static factory to create instances of this service
static SvcFactory<ConditionsDBCnvSvc>          ConditionsDBCnvSvc_factory;
const ISvcFactory& ConditionsDBCnvSvcFactory = ConditionsDBCnvSvc_factory;

/// External constants
extern const CLID&          ConditionsDBAddress_undefinedClassID;
extern const unsigned char& ConditionsDBAddress_undefinedStringType;

//----------------------------------------------------------------------------

/// Constructor
ConditionsDBCnvSvc::ConditionsDBCnvSvc( const std::string& name, 
				        ISvcLocator* svc)
  : ConversionSvc(name, svc, CONDDB_StorageType)
  , m_conditionsDBGate ( 0     )
{
}

//----------------------------------------------------------------------------

/// Destructor
ConditionsDBCnvSvc::~ConditionsDBCnvSvc()
{
}

//----------------------------------------------------------------------------

/// Initialize the service.
StatusCode ConditionsDBCnvSvc::initialize()
{

  // Before anything else, we need to initialise the base class
  StatusCode sc = ConversionSvc::initialize();
  if ( !sc.isSuccess() ) return sc;

  // Now we can get a handle to the MessageSvc
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  log << MSG::DEBUG << "Specific initialization starting" << endreq;

  // Create the ConditionsDBGate as a private service of ConditionsDBCnvSvc
  log << MSG::DEBUG << "Creating ConditionsDBGate" << endreq;
  m_conditionsDBGate = new ConditionsDBGate( "ConditionsDBGate",
					     serviceLocator() );

  // Initialize the ConditionsDBGate
  log << MSG::DEBUG << "Initializing ConditionsDBGate" << endreq;
  sc = m_conditionsDBGate->initialize();
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not initialize ConditionsDBGate" << endreq;
    return sc;
  }

  // Locate the Detector Data Service
  IDataProviderSvc* pDDS = 0;
  sc = serviceLocator()->getService 
    ("DetectorDataSvc",  IID_IDataProviderSvc, (IInterface*&)pDDS);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not locate DetectorDataSvc" << endreq;
    return sc;
  }

  // Set the DetectorDataSvc as data provider service
  sc = setDataProvider ( pDDS );
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not set data provider" << endreq;
    return sc;
  }

  // Locate IConversionSvc interface of the DetectorPersistencySvc
  sc = serviceLocator()->service 
    ("DetectorPersistencySvc",  m_detPersSvc, true);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 
	<< "Cannot locate IConversionSvc interface of DetectorPersistencySvc"
	<< endreq;
    return sc;
  } else {
    log << MSG::DEBUG 
	<< "Retrieved IConversionSvc interface of DetectorPersistencySvc"
	<< endreq;
  }
  
  // Query the IAddressCreator interface of the detector persistency service
  IAddressCreator* iAddrCreator;
  sc = m_detPersSvc->queryInterface(IID_IAddressCreator, 
				    (void**) &iAddrCreator);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 
	<< "Cannot query IAddressCreator interface of DetectorPersistencySvc" 
	<< endreq;
    return sc;
  } else {
    log << MSG::DEBUG 
	<< "Retrieved IAddressCreator interface of DetectorPersistencySvc" 
	<< endreq;
  }
  log << MSG::DEBUG 
      << "Set it as the address creator of the ConditionsDBCnvSvc"
      << endreq;
  sc = setAddressCreator( iAddrCreator );
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 
	<< "Cannot set the address creator" 
	<< endreq;
    return sc;
  }

  log << MSG::DEBUG << "Specific initialization completed" << endreq;
  return sc;
}

//----------------------------------------------------------------------------

/// Finalize the service.
StatusCode ConditionsDBCnvSvc::finalize()
{
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  log << MSG::DEBUG << "Finalizing" << endreq;
  m_conditionsDBGate->finalize();
  delete m_conditionsDBGate;
  m_conditionsDBGate = 0;
  return ConversionSvc::finalize();
}

//----------------------------------------------------------------------------

/// Query interface
StatusCode ConditionsDBCnvSvc::queryInterface(const IID& riid, 
					      void** ppvInterface)
{
  if ( IID_IConditionsDBCnvSvc == riid )  {
    // With the highest priority return the specific interface of this service
    *ppvInterface = (IConditionsDBCnvSvc*)this;
  } else  {
    // Interface is not directly available: try out a base class
    return ConversionSvc::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

/// Create a transient representation from another representation of an object.
/// Overloaded from ConversionSvc because ConditionsDBCnvSvc has no converters.
StatusCode ConditionsDBCnvSvc::createObj ( IOpaqueAddress* pAddress, 
					   DataObject*&    refpObject) {

  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  log << MSG::DEBUG << "Method createObj starting" << endreq;

  /// Dynamic cast the IOpaqueAddress to a ConditionsDBAddress
  /// The ConditionsDBAddress specifies the requested folder, time, tag
  ConditionsDBAddress* 
    pCaddress = dynamic_cast< ConditionsDBAddress* >( pAddress );
  if ( 0 == pCaddress ) {
    log << MSG::ERROR << "Address is not a ConditionsDBAddress" << endreq;
    log << MSG::ERROR << "type=" << (int)pAddress->svcType() 
	<< " classID=" << pAddress->clID() << endreq;
    return StatusCode::FAILURE;
  }

  /// Next, create a new condition DataObject for the given folder, time, tag
  //  Notice that the ConditionsDBCnvSvc has no converters of its own:
  //  object creation is delegated to another CnvSvc via a temporary address
  StatusCode status = createConditionData ( refpObject, 
					    pCaddress->folderName(), 
					    pCaddress->tagName(), 
					    pCaddress->time(), 
					    pCaddress->clID(), 
					    pCaddress->stringType() );
  if ( !status.isSuccess() ) {
    log << MSG::ERROR << "Could not create condition DataObject" << endreq;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << "Method createObj exiting" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Resolve the references of the created transient object.
/// Overloaded from ConversionSvc because ConditionsDBCnvSvc has no converters.
StatusCode ConditionsDBCnvSvc::fillObjRefs ( IOpaqueAddress* /*pAddress*/, 
					     DataObject*     /*pObject */ ) {

  // Do nothing for the moment
  return StatusCode::SUCCESS;

}
  
//----------------------------------------------------------------------------

/// Update a transient representation from another representation of an object.
/// Overloaded from ConversionSvc because ConditionsDBCnvSvc has no converters.
/// Always update even if ConditionData is valid at the specified time:
/// previous ConditionData may refer to a different tag at the same time.
StatusCode ConditionsDBCnvSvc::updateObj ( IOpaqueAddress* pAddress, 
					   DataObject*     pObject  ) {

  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  log << MSG::DEBUG << "Method updateObj starting" << endreq;

  /// Dynamic cast the IOpaqueAddress to a ConditionsDBAddress
  /// The ConditionsDBAddress specifies the requested folder, time, tag
  ConditionsDBAddress* 
    pCaddress = dynamic_cast< ConditionsDBAddress* >( pAddress );
  if ( 0 == pCaddress ) {
    log << MSG::ERROR << "Address is not a ConditionsDBAddress" << endreq;
    log << MSG::ERROR << "type=" << (int)pAddress->svcType() 
	<< " classID=" << pAddress->clID() << endreq;
    return StatusCode::FAILURE;
  }

  // Next, update the object according to the specifications of the address
  if( 0 == pObject ) {
    log << MSG::ERROR << "There is no object to update" << endreq;
    return StatusCode::FAILURE;
  }
  IValidity* pValidity = dynamic_cast<IValidity*>(pObject);
  if ( 0 == pValidity ) {
    log << MSG::ERROR 
	<< "Object to update does not implement IValidity" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG << "Old condition DataObject was valid since "
      << pValidity->validSince().absoluteTime() << " till "
      << pValidity->validTill().absoluteTime()  << endreq;
  StatusCode status = updateConditionData( pObject, 
					   pCaddress->folderName(), 
					   pCaddress->tagName(), 
					   pCaddress->time(), 
					   pCaddress->clID(), 
					   pCaddress->stringType() );
  if ( !status.isSuccess() ) {
    log << MSG::ERROR << "Could not update condition DataObject" << endreq;
    return StatusCode::FAILURE;
  }

  // Last, check that everything is OK
  pValidity = dynamic_cast<IValidity*>(pObject);
  if ( 0 == pValidity ) {
    log << MSG::ERROR 
	<< "Updated object does not implement IValidity" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG << "New condition DataObject is valid since "
      << pValidity->validSince().absoluteTime() << " till "
      << pValidity->validTill().absoluteTime()  << endreq;

  log << MSG::DEBUG << "Method updateObj exiting" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Update the references of an updated transient object.
/// Overloaded from ConversionSvc because ConditionsDBCnvSvc has no converters.
StatusCode ConditionsDBCnvSvc::updateObjRefs ( IOpaqueAddress* /*pAddress*/, 
					       DataObject*     /*pObject */ ) {

  // Do nothing for the moment
  return StatusCode::SUCCESS;

}
  
//----------------------------------------------------------------------------

/// Create a condition DataObject by folder name, tag and time.
/// This method does not register DataObject in the transient data store.
/// Implementation:
/// - create a temporary address containing storage type and classID;
/// - dispatch to appropriate conversion service according to storage type;
/// - this will dispatch to appropriate converter according to CLID
///   (ConditionsDBCnvSvc has no converters of its own).
StatusCode 
ConditionsDBCnvSvc::createConditionData ( DataObject*&         refpObject,
					  const std::string&   folderName,
					  const std::string&   tagName,
					  const ITime&         time,
					  const CLID&          classID,
					  const unsigned char& type )
{
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );

  // Lookup condition object in the CondDB
  std::string stringData;
  TimePoint since;
  TimePoint till;
  StatusCode status = m_conditionsDBGate->readCondDBObject
    ( since, till, stringData, folderName, tagName, time );
  if ( !status.isSuccess() ) {
    log << MSG::ERROR << "Could not read CondDBObject data" << endreq;
    return status;
  }
  log << MSG::DEBUG << "CondDBObject data successfully read" << endreq;

  // Should classID and type be discovered in the ConditionsDB?
  CLID          theClassID;
  unsigned char theType;
  if ( ConditionsDBAddress_undefinedStringType == type &&
       ConditionsDBAddress_undefinedClassID    == classID ) {
    // Read folder description in the CondDB
    log << MSG::DEBUG 
	<< "Read service type and classID in the folder description" << endreq;
    std::string description;
    status = m_conditionsDBGate->readCondDBFolder( description, 
						     folderName );
    if ( !status.isSuccess() ) {
      log << MSG::ERROR 
	  << "Could not read folder description in the CondDB" << endreq;
      return status;
    }
    status  = decodeDescription( description, theClassID, theType );
    if (!status.isSuccess() ) {
      log << MSG::ERROR << "Could not decode folder description" << endreq;
      return status;
    }
  } else {
    theClassID = classID;
    theType    = type;
  }

  // Create temporary address for the relevant type and classID 
  log << MSG::DEBUG 
      << "Delegate address creation to the persistency service" << endreq;
  log << MSG::DEBUG 
      << "Creating an address of type " 
      << (int)theType << " for class " << theClassID << endreq;
  log << MSG::DEBUG << "String data is '" << stringData << "'" << endreq;
  IOpaqueAddress* tmpAddress;
  const std::string par[2] = {stringData, ""};
  status = addressCreator()->createAddress
    ( theType, theClassID, par, 0, tmpAddress);
  if ( !status.isSuccess() ) {
    log << MSG::ERROR 
	<< "Persistency service could not create a new address" << endreq;
    return status;
  }  
  log << MSG::DEBUG << "Temporary address successfully created" << endreq;
  tmpAddress->addRef();

  // Now create the object
  log << MSG::DEBUG 
      << "Delegate object creation to the persistency service" << endreq;
  status = m_detPersSvc->createObj ( tmpAddress, refpObject );
  tmpAddress->release();
  if ( !status.isSuccess() ) {
    log << MSG::ERROR 
	<< "Persistency service could not create a new object" << endreq;
    return status;
  }

  // Set validity of created object
  IValidity* pValidity = dynamic_cast<IValidity*>(refpObject);
  if ( 0 == pValidity ) {
    log << MSG::ERROR 
	<< "Created object does not implement IValidity" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG << "New object successfully created" << endreq;
  pValidity->setValidity ( since, till );
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// If not specifed, type and classID are discovered at runtime in the CondDB
StatusCode 
ConditionsDBCnvSvc::createConditionData (DataObject*&         refpObject,
					 const std::string&   folderName,
					 const std::string&   tagName,
					 const ITime&         time )
{
  return createConditionData ( refpObject,
			       folderName,
			       tagName,
			       time,
			       ConditionsDBAddress_undefinedClassID, 
			       ConditionsDBAddress_undefinedStringType );
}

//----------------------------------------------------------------------------

/// Update a condition DataObject by folder name, tag and time.
/// Always update even if condition DataObject is valid at the specified time:
/// previous DataObject may refer to a different tag at the same time.
/// This method does not register DataObject in the transient data store.
/// Implementation:
/// - create a temporary address containing storage type and classID;
/// - dispatch to appropriate conversion service according to storage type;
/// - this will dispatch to appropriate converter according to CLID
///   (the ConditionsDBCnvSvc has no converters of its own).
StatusCode 
ConditionsDBCnvSvc::updateConditionData ( DataObject*          pObject,
					  const std::string&   folderName,
					  const std::string&   tagName,
					  const ITime&         time,
					  const CLID&          classID,
					  const unsigned char& type )
{
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  StatusCode status;

  // Is there an object to be updated?
  if ( 0 == pObject ) {
    log << MSG::ERROR << "There is no DataObject to update" << endreq;
    return StatusCode::FAILURE;
  }

  // Should classID and type be discovered in the ConditionsDB?
  CLID          theClassID;
  unsigned char theType;
  if ( ConditionsDBAddress_undefinedStringType == type &&
       ConditionsDBAddress_undefinedClassID    == classID ) {
    // Read folder description in the CondDB
    log << MSG::DEBUG 
	<< "Read service type and classID in the folder description" << endreq;
    std::string description;
    status = m_conditionsDBGate->readCondDBFolder( description, 
						     folderName );
    if ( !status.isSuccess() ) {
      log << MSG::ERROR 
	  << "Could not read folder description in the CondDB" << endreq;
      return status;
    }
    status  = decodeDescription( description, theClassID, theType );
    if (!status.isSuccess() ) {
      log << MSG::ERROR << "Could not decode folder description" << endreq;
      return status;
    }
  } else {
    theClassID = classID;
    theType    = type;
  }

  // Is object an instance of the specified class?
  if ( theClassID != pObject->clID() ) {
    log << MSG::ERROR << "Update requested for clID " << theClassID
	<< " while DataObject is of clID " 
	<< pObject->clID() << endreq;
    return StatusCode::FAILURE;
  }

  // Lookup condition object in the CondDB
  std::string stringData;
  TimePoint since;
  TimePoint till;
  status = m_conditionsDBGate->readCondDBObject
    ( since, till, stringData, folderName, tagName, time );
  if ( !status.isSuccess() ) {
    log << MSG::ERROR << "Could not read CondDBObject data" << endreq;
    return status;
  }
  log << MSG::DEBUG << "CondDBObject data successfully read" << endreq;

  // Create temporary address for the relevant type and classID 
  log << MSG::DEBUG 
      << "Delegate address creation to the persistency service" << endreq;
  log << MSG::DEBUG 
      << "Creating an address of type " 
      << (int)theType << " for class " << theClassID << endreq;
  log << MSG::DEBUG << "String data: " << stringData << endreq;
  IOpaqueAddress* tmpAddress;
  const std::string par[2] = {stringData, ""};
  status = addressCreator()->createAddress
    ( theType, theClassID, par, 0, tmpAddress);
  if ( !status.isSuccess() ) {
    log << MSG::ERROR 
	<< "Persistency service could not create a new address" << endreq;
    return status;
  }  
  log << MSG::DEBUG << "Temporary address successfully created" << endreq;
  tmpAddress->addRef();

  // Now update the object
  log << MSG::DEBUG << "Delegate update to the persistency service" << endreq;
  status = m_detPersSvc->updateObj ( tmpAddress, pObject );
  tmpAddress->release();
  if ( !status.isSuccess() ) {
    log << MSG::ERROR 
	<< "Persistency service could not update object" << endreq;
    return status;
  }

  // Set validity of updated object
  IValidity* pValidity = dynamic_cast<IValidity*>(pObject);
  if ( 0 == pValidity ) {
    log << MSG::ERROR 
	<< "Updated object does not implement IValidity" << endreq;
    return StatusCode::FAILURE;
  }
  pValidity->setValidity ( since, till );
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// If not specifed, type and classID are discovered at runtime in the CondDB
StatusCode 
ConditionsDBCnvSvc::updateConditionData (DataObject*          pObject,
					 const std::string&   folderName,
					 const std::string&   tagName,
					 const ITime&         time )
{
  return updateConditionData ( pObject,
			       folderName,
			       tagName,
			       time,
			       ConditionsDBAddress_undefinedClassID, 
			       ConditionsDBAddress_undefinedStringType );
}

//----------------------------------------------------------------------------

/// Decode classID and storage type from the folder description string
StatusCode 
ConditionsDBCnvSvc::decodeDescription   ( const std::string&  description,
					  CLID&               classID,
					  unsigned char&      type)
{
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  log << MSG::DEBUG << "Decoding description: " << description << endreq;
  std::string theDesc = description;  

  //log << MSG::DEBUG << "Decoding: '" <<  theDesc << "'" << endreq; 
  std::string s1 = "<description type=";
  if ( theDesc.find(s1) != 0 ) {
    log << MSG::ERROR 
	<< "Could not find '" <<  s1 << "' in description" << endreq;
    return StatusCode::FAILURE;
  }
  theDesc.erase( 0, s1.length() );

  //log << MSG::DEBUG << "Decoding: '" <<  theDesc << "'" << endreq; 
  std::string s_type = theDesc.substr(0,3);
  const char* c_type = s_type.c_str();
  if ( strspn ( c_type, " .0123456789" ) != s_type.length() ) {
    log << MSG::ERROR << "Could not find '" <<  s1
	<< "' in description: " << theDesc << endreq;
    return StatusCode::FAILURE;
  }
  theDesc.erase( 0, s_type.length() );

  //log << MSG::DEBUG << "Decoding: '" <<  theDesc << "'" << endreq; 
  std::string s2 = " classID=";
  if ( theDesc.find(s2) != 0 ) {
    log << MSG::ERROR << "Could not find '" <<  s2 
	<< "' in description: " << theDesc << endreq;
    return StatusCode::FAILURE;
  }
  theDesc.erase( 0, s2.length() );

  //log << MSG::DEBUG << "Decoding: '" <<  theDesc << "'" << endreq; 
  std::string s_clas = theDesc.substr(0,9);
  const char* c_clas = s_clas.c_str();
  if ( strspn ( c_clas, " .0123456789" ) != s_clas.length() ) {
    log << MSG::ERROR 
	<< "Non-numeric characters in classID field: " << s_clas << endreq;
    return StatusCode::FAILURE;
  }
  theDesc.erase( 0, s_clas.length() );

  //log << MSG::DEBUG << "Decoding: '" <<  theDesc << "'" << endreq; 
  std::string s3 = ">";
  if ( theDesc.find(s3) != 0 ) {
    log << MSG::ERROR << "Could not find '" <<  s3 
	<< "' in description: " << theDesc << endreq;
    return StatusCode::FAILURE;
  }
  theDesc.erase( 0, s3.length() );

  //log << MSG::DEBUG << "Decoding: '" <<  theDesc << "'" << endreq; 
  if ( theDesc.length() != 0 ) {
    log << MSG::ERROR 
	<< "Extra characters in description: " <<  theDesc << endreq;
    return StatusCode::FAILURE;
  }

  type    = (unsigned char)atoi( c_type );
  classID = (CLID         )atoi( c_clas );
  log << MSG::DEBUG << "Decoded: stringType=" << (int)type 
      << " class=" << classID << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Encode classID and storage type into the folder description string
StatusCode 
ConditionsDBCnvSvc::encodeDescription   ( const CLID&          classID,
					  const unsigned char& type,
					  std::string&         description )
{
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  unsigned int i_type = (unsigned int) type;
  unsigned int i_clas = (unsigned int) classID;
  log << MSG::DEBUG << "Encoding: type=" << i_type 
      << " classID=" << i_clas << endreq;
  char c_type[4];
  char c_clas[10];
  if ( 3 != sprintf ( c_type, "%3.3i", i_type ) ) 
    log << MSG::ERROR << "Error encoding type="    << i_type << endreq;
  if ( 9 != sprintf ( c_clas, "%9.9i", i_clas ) ) 
    log << MSG::ERROR << "Error encoding classID=" << i_clas << endreq;
  std::string s_type  = std::string( c_type );
  std::string s_class = std::string( c_clas );
  std::string s1 = "<description type=";
  std::string s2 = " classID=";
  std::string s3 = ">";
  description = s1 + s_type + s2 + s_class + s3;
  log << MSG::DEBUG << "Encoded description: " << description << endreq;
  return StatusCode::SUCCESS;
} 

//----------------------------------------------------------------------------

/// Handle to the ConditionsDBGate
IConditionsDBGate* ConditionsDBCnvSvc::conditionsDBGate ( ) {
  return m_conditionsDBGate;
}

//----------------------------------------------------------------------------






