//$Id: ConditionsDBCnvSvc.cpp,v 1.8 2001-12-16 15:58:24 andreav Exp $
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>

#include "ConditionsDBCnvSvc.h"
#include "ConditionsDBGate.h"

#include "DetDesc/Condition.h"

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/TimePoint.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"

/// Instantiation of a static factory to create instances of this service
static SvcFactory<ConditionsDBCnvSvc>          ConditionsDBCnvSvc_factory;
const ISvcFactory& ConditionsDBCnvSvcFactory = ConditionsDBCnvSvc_factory;

//----------------------------------------------------------------------------

/// Constructor
ConditionsDBCnvSvc::ConditionsDBCnvSvc( const std::string& name, 
				        ISvcLocator* svc)
  : ConversionSvc ( name, svc, CONDDB_StorageType )
  , m_conditionsDBGate ( 0 )
{
  // The default global tag (unless set in the JobOptions) is "HEAD"
  declareProperty( "condDBGlobalTag",  m_globalTag = "HEAD" );
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
  log << MSG::INFO << "Specific initialization starting" << endreq;

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

  // Query the IDetDataSvc interface of the detector data service
  sc = pDDS->queryInterface(IID_IDetDataSvc, 
			    (void**) &m_detDataSvc);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 
	<< "Cannot query IDetDataSvc interface of DetectorDataSvc" 
	<< endreq;
    return sc;
  } else {
    log << MSG::DEBUG 
	<< "Retrieved IDetDataSvc interface of DetectorDataSvc" 
	<< endreq;
  }

  // Locate IConversionSvc interface of the DetectorPersistencySvc
  sc = serviceLocator()->service 
    ("DetectorPersistencySvc", m_detPersSvc, true);
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

  // Get properties from the JobOptionsSvc
  sc = setProperties();
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not set jobOptions properties" << endreq;
    return sc;
  }
  log << MSG::DEBUG << "Properties were read from jobOptions" << endreq;
  log << MSG::INFO
      << "Global tag name: " << m_globalTag << endreq;
  if ( m_globalTag == "DEFAULT" ) {
    log << MSG::ERROR 
	<< "Invalid global tag DEFAULT: this is a reserved word" << endreq;
    return StatusCode::FAILURE;
  }

  log << MSG::INFO << "Specific initialization completed" << endreq;
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
					   DataObject*&    refpObject ) {

  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  log << MSG::DEBUG << "Method createObj starting" << endreq;

  // Check that the event time has been defined
  // The event time is obtained from the ConditionDataSvc
  if ( !m_detDataSvc->validEventTime() ) {
    log << MSG::ERROR
	<< "Cannot create DataObject: event time undefined"
	<< endreq; 
    return StatusCode::FAILURE;
  } else {
    ITime::AbsoluteTime absTime;
    absTime = m_detDataSvc->eventTime().absoluteTime();
    log << MSG::DEBUG
	<< "Event time: " << absTime
	<< " (msb:" << (long)(   absTime         >> 32 )
	<< ", lsb:" << (long)( ( absTime << 32 ) >> 32 )
	<< ")" << endreq; 
  }

  // Create the object according to folder, tag, time, clid, string type
  // Notice that the ConditionsDBCnvSvc has no converters of its own:
  // object creation is delegated to another CnvSvc via a temporary address
  // The IOpaqueAddress specifies folderName and clid
  // The tagName is a property of this service
  // The secondary storage type is always discovered dynamically
  StatusCode sc;
  sc = createConditionData( refpObject, 
			    pAddress->par()[0], 
			    globalTag(), 
			    pAddress->par()[1], 
			    m_detDataSvc->eventTime(), 
			    pAddress->clID(), 
			    pAddress->registry() );
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not create condition DataObject" << endreq;
  }

  log << MSG::DEBUG << "Method createObj exiting" << endreq;
  return sc;

}

//----------------------------------------------------------------------------

/// Resolve the references of the created transient object.
/// Overloaded from ConversionSvc because ConditionsDBCnvSvc has no converters.
StatusCode ConditionsDBCnvSvc::fillObjRefs ( IOpaqueAddress* /*pAddress*/, 
					     DataObject*     /*pObject */ ) {
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  log << MSG::DEBUG << "Method fillObjRefs is not implemented" << endreq;
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

  // Check that the event time has been defined
  // The event time is obtained from the ConditionDataSvc
  if ( !m_detDataSvc->validEventTime() ) {
    log << MSG::ERROR
	<< "Cannot update DataObject: event time undefined"
	<< endreq; 
    return StatusCode::FAILURE;
  } else {
    ITime::AbsoluteTime absTime;
    absTime = m_detDataSvc->eventTime().absoluteTime();
    log << MSG::DEBUG
	<< "Event time: " << absTime
	<< " (msb:" << (long)(   absTime         >> 32 )
	<< ", lsb:" << (long)( ( absTime << 32 ) >> 32 )
	<< ")" << endreq; 
  }

  // Update the object according to folder, tag, time, clid, string type
  // Notice that the ConditionsDBCnvSvc has no converters of its own:
  // object creation is delegated to another CnvSvc via a temporary address
  // The IOpaqueAddress specifies folderName and clid
  // The tagName is a property of this service
  // The secondary storage type is always discovered dynamically
  if( 0 == pObject ) {
    log << MSG::ERROR << "There is no object to update" << endreq;
    return StatusCode::FAILURE;
  }
  IValidity* pValidity = dynamic_cast<IValidity*>(pObject);
  if ( 0 == pValidity ) {
    log << MSG::WARNING
	<< "Object to update does not implement IValidity: assume up-to-date" 
	<< endreq;
    return StatusCode::SUCCESS;
  }
  log << MSG::DEBUG << "Old condition DataObject was valid since "
      << pValidity->validSince().absoluteTime() << " till "
      << pValidity->validTill().absoluteTime()  << endreq;
  StatusCode sc;
  sc = updateConditionData( pObject, 
			    pAddress->par()[0], 
			    globalTag(), 
			    pAddress->par()[1], 
			    m_detDataSvc->eventTime(), 
			    pAddress->clID(), 
			    pAddress->registry() );
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not update condition DataObject" << endreq;
    return sc;
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
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  log << MSG::WARNING << "Method updateObjRefs is not implemented" << endreq;
  return StatusCode::SUCCESS;
}
  
//----------------------------------------------------------------------------

/// Convert a transient object to a requested representation.
/// Overloaded from ConversionSvc because ConditionsDBCnvSvc has no converters.
StatusCode ConditionsDBCnvSvc::createRep( DataObject* /*pObject*/,
					  IOpaqueAddress*& /*refpAddress*/ ) {

  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  log << MSG::WARNING << "Method createRep is not implemented" << endreq;
  return StatusCode::SUCCESS;
}
  
//----------------------------------------------------------------------------

/// Resolve the references of a converted object.
/// Overloaded from ConversionSvc because ConditionsDBCnvSvc has no converters.
StatusCode ConditionsDBCnvSvc::fillRepRefs ( IOpaqueAddress* /*pAddress*/, 
					     DataObject*     /*pObject */ ) {
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  log << MSG::WARNING << "Method fillRepRefs is not implemented" << endreq;
  return StatusCode::SUCCESS;
}
  
//----------------------------------------------------------------------------

/// Update a converted representation of a transient object.
/// Overloaded from ConversionSvc because ConditionsDBCnvSvc has no converters.
StatusCode ConditionsDBCnvSvc::updateRep ( IOpaqueAddress* /*pAddress*/, 
					   DataObject*     /*pObject */ ) {
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  log << MSG::WARNING << "Method updateRep is not implemented" << endreq;
  return StatusCode::SUCCESS;
}
  
//----------------------------------------------------------------------------

/// Update the references of an already converted object.
/// Overloaded from ConversionSvc because ConditionsDBCnvSvc has no converters.
StatusCode ConditionsDBCnvSvc::updateRepRefs ( IOpaqueAddress* /*pAddress*/, 
					       DataObject*     /*pObject */ ) {
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  log << MSG::WARNING << "Method updateRepRefs is not implemented" << endreq;
  return StatusCode::SUCCESS;
}
  
//----------------------------------------------------------------------------

/// Create an address using explicit arguments to identify a single object.
/// Par[0] is folder name in the ConditionsDB.
/// Par[1] is entry name in the string (which may contain many conditions,
/// for instance in the case of XML files with more than one element).
StatusCode ConditionsDBCnvSvc::createAddress( unsigned char svc_type,
					      const CLID& clid,
					      const std::string* par, 
					      const unsigned long* /*ipar*/,
					      IOpaqueAddress*& refpAddress ) {

  // First check that requested address is of type CONDDB_StorageType
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  if ( svc_type!= CONDDB_StorageType ) {
    log << MSG::ERROR 
	<< "Cannot create addresses of type " << (int)svc_type 
	<< " which is different from " << (int)CONDDB_StorageType 
	<< endreq;
    return StatusCode::FAILURE;
  }
  
  // Par[0] is folder name in the ConditionsDB.
  std::string folderName = par[0];

  // Par[1] is entry name in the string (which may contain many conditions, 
  // for instance in the case of XML files with more than one element).
  std::string entryName = par[1];

  // Now create the address
  refpAddress = new GenericAddress( CONDDB_StorageType, 
				    clid, 
				    folderName,
				    entryName );
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Create a condition DataObject by folder name, tag and time.
/// This method does not register DataObject in the transient data store,
/// but may register TDS addresses for its children if needed (e.g. Catalog).
/// The string storage type is discovered at runtime in the CondDB.
/// The entry name identifies a condition amongst the many in the string.
/// Implementation:
/// - create a temporary address containing storage type and classID;
/// - dispatch to appropriate conversion service according to storage type;
/// - this will dispatch to appropriate converter according to CLID
///   (ConditionsDBCnvSvc has no converters of its own).
StatusCode 
ConditionsDBCnvSvc::createConditionData( DataObject*&         refpObject,
					 const std::string&   folderName,
					 const std::string&   tagName,
					 const std::string&   entryName,
					 const ITime&         time,
					 const CLID&          classID,
					 IRegistry*           entry )
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

  // Discover the string storage type in the ConditionsDB
  unsigned char theType;
  log << MSG::DEBUG 
      << "Read string storage type in the folder description" << endreq;
  std::string description;
  status = m_conditionsDBGate->readCondDBFolder( description, 
						 folderName );
  if ( !status.isSuccess() ) {
    log << MSG::ERROR 
	<< "Could not read folder description in the CondDB" << endreq;
    return status;
  }
  status  = decodeDescription( description, theType );
  if (!status.isSuccess() ) {
    log << MSG::ERROR << "Could not decode folder description" << endreq;
    return status;
  }

  // Create temporary address for the relevant type and classID 
  log << MSG::DEBUG 
      << "Delegate address creation to the persistency service" << endreq;
  log << MSG::DEBUG << "Creating an address of type " 
      << (int)theType << " for class " << classID << endreq;
  log << MSG::VERBOSE << "String data is:" << endl << stringData << endreq;
  IOpaqueAddress* tmpAddress;
  const std::string par[2] = { stringData, entryName};
  status = addressCreator()->createAddress
    ( theType, classID, par, 0, tmpAddress );
  if ( !status.isSuccess() ) {
    log << MSG::ERROR 
	<< "Persistency service could not create a new address" << endreq;
    return status;
  }  
  log << MSG::DEBUG << "Temporary address successfully created" << endreq;
  tmpAddress->addRef();

  // Set the transient store registry for the object associated to this address
  tmpAddress->setRegistry( entry );

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
    log << MSG::WARNING
	<< "Created object does not implement IValidity: cannot set validity"
	<< endreq;
  } else {
    pValidity->setValidity ( since, till );
  }

  log << MSG::DEBUG << "New object successfully created" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Update a condition DataObject by folder name, tag and time.
/// Always update even if condition DataObject is valid at the specified time:
/// previous DataObject may refer to a different tag at the same time.
/// This method does not register DataObject in the transient data store,
/// but may register TDS addresses for its children if needed (e.g. Catalog).
/// The string storage type is discovered at runtime in the CondDB.
/// The entry name identifies a condition amongst the many in the string.
/// Implementation:
/// - create a temporary address containing storage type and classID;
/// - dispatch to appropriate conversion service according to storage type;
/// - this will dispatch to appropriate converter according to CLID
///   (the ConditionsDBCnvSvc has no converters of its own).
StatusCode 
ConditionsDBCnvSvc::updateConditionData( DataObject*          pObject,
					 const std::string&   folderName,
					 const std::string&   tagName,
					 const std::string&   entryName,
					 const ITime&         time,
					 const CLID&          classID,
					 IRegistry*           entry )
{
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  StatusCode status;

  // Is there an object to be updated?
  if ( 0 == pObject ) {
    log << MSG::ERROR << "There is no DataObject to update" << endreq;
    return StatusCode::FAILURE;
  }

  // Discover the string storage type in the ConditionsDB
  unsigned char theType;
  log << MSG::DEBUG 
      << "Read string storage type in the folder description" << endreq;
  std::string description;
  status = m_conditionsDBGate->readCondDBFolder( description, 
						 folderName );
  if ( !status.isSuccess() ) {
    log << MSG::ERROR 
	<< "Could not read folder description in the CondDB" << endreq;
    return status;
  }
  status  = decodeDescription( description, theType );
  if (!status.isSuccess() ) {
    log << MSG::ERROR << "Could not decode folder description" << endreq;
    return status;
  }

  // Is object an instance of the specified class?
  if ( classID != pObject->clID() ) {
    log << MSG::ERROR << "Update requested for clID " << classID
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
  log << MSG::DEBUG << "Creating an address of type " 
      << (int)theType << " for class " << classID << endreq;
  log << MSG::VERBOSE << "String data is:" << endl << stringData << endreq;
  IOpaqueAddress* tmpAddress;
  const std::string par[2] = { stringData, entryName };
  status = addressCreator()->createAddress
    ( theType, classID, par, 0, tmpAddress );
  if ( !status.isSuccess() ) {
    log << MSG::ERROR 
	<< "Persistency service could not create a new address" << endreq;
    return status;
  }  
  log << MSG::DEBUG << "Temporary address successfully created" << endreq;
  tmpAddress->addRef();

  // Set the transient store registry for the object associated to this address
  tmpAddress->setRegistry( entry );

  // Now update the object
  log << MSG::DEBUG 
      << "Update the object: create a new one and copy it into the old object"
      << endreq;
  log << MSG::DEBUG << "Delegate creation to the persistency service" << endreq;
  DataObject* pNewObject;
  status = m_detPersSvc->createObj ( tmpAddress, pNewObject );
  tmpAddress->release();
  if ( !status.isSuccess() ) {
    log << MSG::ERROR 
	<< "Persistency service could not create object" << endreq;
    return status;
  }
  // Since the DataObject::operator= operator is not virtual, dynamic cast first!
  // The overloaded virtual Condition::update() method must be properly defined!
  // The memory pointed to by the old pointer must contain the new object    
  Condition* pCond = dynamic_cast<Condition*>(pObject);
  Condition* pNewCond = dynamic_cast<Condition*>(pNewObject);
  if ( 0 == pCond || 0 == pNewCond ) {
    log << MSG::ERROR
	<< "Cannot update objects other than Condition: update() must be defined!"
	<< endreq;
    return StatusCode::FAILURE;
  }
  // Deep copy the new Condition into the old DataObject
  pCond->update( *pNewCond );  
  // Delete the useless Condition
  delete pNewCond;

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

/// Decode the string storage type from the folder description string
StatusCode 
ConditionsDBCnvSvc::decodeDescription( const std::string& description,
				       unsigned char& type )
{
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  log << MSG::DEBUG << "Decoding description: " << description << endreq;
  std::string theDesc = description;  

  // Find keywords
  std::string s1 = "<description type=";
  if ( theDesc.find(s1) != 0 ) {
    log << MSG::ERROR 
	<< "Could not find '" <<  s1 << "' in description" << endreq;
    return StatusCode::FAILURE;
  }
  theDesc.erase( 0, s1.length() );

  // Find numerical data
  std::string s_type = theDesc.substr(0,3);
  const char* c_type = s_type.c_str();
  if ( strspn ( c_type, " .0123456789" ) != s_type.length() ) {
    log << MSG::ERROR 
	<< "Could not find numerical data" << endreq;
    return StatusCode::FAILURE;
  }
  theDesc.erase( 0, s_type.length() );

  // Find keywords
  std::string s2 = ">";
  if ( theDesc.find(s2) != 0 ) {
    log << MSG::ERROR << "Could not find '" <<  s2 
	<< "' in description: " << theDesc << endreq;
    return StatusCode::FAILURE;
  }
  theDesc.erase( 0, s2.length() );

  // Any extra characters?
  if ( theDesc.length() != 0 ) {
    log << MSG::ERROR 
	<< "Extra characters in description: " <<  theDesc << endreq;
    return StatusCode::FAILURE;
  }

  type    = (unsigned char)atoi( c_type );
  log << MSG::DEBUG << "Decoded: stringType=" << (int)type << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Encode the string storage type into the folder description string
StatusCode 
ConditionsDBCnvSvc::encodeDescription( const unsigned char& type,
				       std::string& description )
{
  MsgStream log(msgSvc(), "ConditionsDBCnvSvc" );
  unsigned int i_type = (unsigned int) type;
  log << MSG::DEBUG << "Encoding: type=" << i_type << endreq;
  char c_type[4];
  if ( 3 != sprintf ( c_type, "%3.3i", i_type ) ) 
    log << MSG::ERROR << "Error encoding type="    << i_type << endreq;
  std::string s_type  = std::string( c_type );
  std::string s1 = "<description type=";
  std::string s2 = ">";
  description = s1 + s_type + s2;
  log << MSG::DEBUG << "Encoded description: " << description << endreq;
  return StatusCode::SUCCESS;
} 

//----------------------------------------------------------------------------

/// Get the global tag  
const std::string& ConditionsDBCnvSvc::globalTag ( ) { 
  return m_globalTag; 
}

//----------------------------------------------------------------------------

/// Handle to the ConditionsDBGate
IConditionsDBGate* ConditionsDBCnvSvc::conditionsDBGate ( ) {
  return m_conditionsDBGate;
}

//----------------------------------------------------------------------------







