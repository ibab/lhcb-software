//$Id: ConditionsDBDataSvc.cpp,v 1.5 2001-11-27 18:21:53 andreav Exp $
#include <string>

#include "ConditionsDBDataSvc.h"
#include "ConditionsDBAddress.h"

#include "DetCond/IConditionsDBCnvSvc.h"

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

/// External constants
extern const CLID&          ConditionsDBAddress_undefinedClassID;
extern const unsigned char& ConditionsDBAddress_undefinedStringType;

//----------------------------------------------------------------------------

/// Constructor
ConditionsDBDataSvc::ConditionsDBDataSvc( const std::string& name, 
					  ISvcLocator* svc )
  : ConditionDataSvc(name,svc)
{
  m_rootName           = "/dd";
  m_rootCLID           = CLID_Catalog;
  m_conditionStoreRoot = m_rootName; 
  declareProperty( "condDBGlobalTag",  m_tagName = "HEAD" ); // Def: HEAD
}

//----------------------------------------------------------------------------

/// Destructor
ConditionsDBDataSvc::~ConditionsDBDataSvc()
{
}

//----------------------------------------------------------------------------

/// Query interface
StatusCode ConditionsDBDataSvc::queryInterface(const IID& riid, 
					       void** ppvInterface)
{
  if ( IID_IConditionsDBDataSvc == riid )  {
    // With the highest priority return the specific interface of this service
    *ppvInterface = (IConditionsDBDataSvc*)this;
  } else  {
    // Interface is not directly available: try out a base class
    return ConditionDataSvc::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
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

  // Get properties from the JobOptionsSvc
  sc = setProperties();
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not set jobOptions properties" << endreq;
    return sc;
  }
  log << MSG::DEBUG << "Properties were read from jobOptions" << endreq;
  log << MSG::DEBUG 
      << "Global tag name:        " << m_tagName         << endreq;

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

/// Set the new global tag
void ConditionsDBDataSvc::setTagName ( const std::string& tag ) { 
  m_tagName = tag; 
}

//----------------------------------------------------------------------------

/// Get the global tag  
const std::string& ConditionsDBDataSvc::tagName ( ) { 
  return m_tagName; 
}

//----------------------------------------------------------------------------

/// Convert the folder name in the CondDB to the transient data store path
StatusCode 
ConditionsDBDataSvc::getNameInCondDB  ( std::string& folderName,
					const std::string& tdsName ) {
  MsgStream log(msgSvc(), "ConditionsDBDataSvc" );
  if ( tdsName.find(m_conditionStoreRoot) != 0 ) {
    log << MSG::DEBUG 
	<< "DataObject path in the Data Store " << tdsName 
	<< " does not contain root path " << m_conditionStoreRoot << endreq;
    return StatusCode::FAILURE;  
  }
  folderName = std::string( tdsName, 
			    m_conditionStoreRoot.length(), 
			    tdsName.length() );
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

/// Convert the transient data store path to the folder name in the CondDB.
/// Check that the path name is not a reserved name.
StatusCode 
ConditionsDBDataSvc::getNameInStore ( std::string& path,
				      const std::string& folderName ) {

  MsgStream log(msgSvc(), "ConditionsDBDataSvc" );
  path = m_conditionStoreRoot + folderName;
  if ( path.length() == 0 || path == m_rootName ) {
    log << MSG::ERROR
	<< "Invalid path for DataObject: " << path << endreq;
    log << MSG::ERROR
	<< "This path is reserved for the data store root" << endreq;
    return StatusCode::FAILURE;
  }
  else if ( path[0] != IDataProviderSvc::SEPARATOR ) {
    log << MSG::ERROR
	<< "Invalid path for DataObject: " << path << endreq;
    log << MSG::ERROR
	<< "Specify a full path instead of a relative path" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

/// Create a valid DataObject by CondDB folder name (for default tag and key),
/// then load it in the TDS using the implicit naming convention:
/// if the DataObject exists already, update it instead (if necessary).
/// Specify the classID of the DataObject and the technology type for 
/// the strings stored in the CondDB. 
StatusCode 
ConditionsDBDataSvc::retrieveValidCondition  ( DataObject*&         refpObject,
					       const std::string&   folderName,
					       const CLID&          classID,
					       const unsigned char& type )
{
  MsgStream log(msgSvc(), "ConditionsDBDataSvc" );
  StatusCode sc;
  log << MSG::DEBUG 
      << "Retrieving a valid DataObject from ConditionsDB folder:" << endreq;
  log << MSG::DEBUG << folderName << endreq;

  // Check if the event time is set
  if ( !validEventTime() ) {
    log << MSG::ERROR << "The event time is not set yet" << endreq;
    return StatusCode::FAILURE;
  }

  // Convert the folder name into a data store path
  std::string path;
  sc = getNameInStore( path, folderName );
  if ( !sc.isSuccess() ) return sc;
  log << MSG::DEBUG 
      << "DataObject will be registered in the store as:" << endreq;
  log << MSG::DEBUG << path << endreq;
  
  // Check if a DataObject is already registered in the store with this name
  // Check if the object address is consistent to what we are asking for
  // If the object does not exist, load it
  // If the object exists, update it
  IRegistry* entry;
  if ( checkRoot () ) {

    entry = m_root->find(path);
    if ( 0 != entry ) {

      log << MSG::DEBUG 
	  << "A RegistryEntry already exists for " << path << endreq;

      // Check if address associated to this entry is consistent to our needs
      // TODO

      // Check if an object is already loaded in this RegistryEntry
      refpObject = entry->object();
      if ( 0 == refpObject ) {
	log << MSG::DEBUG 
	    << "DataObject not loaded yet: retrieve it" << endreq;
	return retrieveObject( path, refpObject );
      } else {
	log << MSG::DEBUG 
	    << "DataObject already loaded: update it if necessary" << endreq;
	return updateObject( refpObject );
      }

    } else { 

      log << MSG::DEBUG << "No RegistryEntry exists at " << path << endreq;

    }

  } else {

    log << MSG::DEBUG << "Data store " << name()
	<< " has no valid root yet" << endreq;

  }
  
  // No RegistryEntry found for this name
  // Create it and create all intermediate directories 
  sc = i_mkdir( path, entry );
  if ( !sc.isSuccess() ) return sc;

  // Create an address and associate it to the RegistryEntry
  log << MSG::DEBUG
      << "Create a new address and associate it to:" << endreq;
  log << MSG::DEBUG << path << endreq;
  IOpaqueAddress* theAddress = new ConditionsDBAddress
    ( folderName, m_tagName, classID, type );
  log << MSG::DEBUG << "Address of CondDB type=" << (int)theAddress->svcType() 
      << " classID=" << classID << " stringType=" << (int)type << endreq;
  theAddress->setRegistry(entry);
  entry->setAddress(theAddress);
  theAddress->addRef();

  // Now retrieve the DataObject associated to that RegistryEntry
  log << MSG::DEBUG << "Now retrieve the object at " << path << endreq;
  return retrieveObject( path, refpObject );
  
}

//----------------------------------------------------------------------------

/// If not specifed, type and classID are discovered at runtime in the CondDB
StatusCode 
ConditionsDBDataSvc::retrieveValidCondition( DataObject*&        refpObject,
					     const std::string&  folderName ) {
  return retrieveValidCondition ( refpObject, 
				  folderName,
				  ConditionsDBAddress_undefinedClassID, 
				  ConditionsDBAddress_undefinedStringType );
}

//----------------------------------------------------------------------------

/// Create a Registry Entry in the data store with a given name.
/// Also create all intermediate directories (~ UNIX "mkdir -p").
/// Fill each intermediate directory with a dummy DataObject.
StatusCode
ConditionsDBDataSvc::i_mkdir ( const std::string& pathName,
			       IRegistry*& entry )
{
  
  MsgStream log(msgSvc(), "ConditionsDBDataSvc" );
  log << MSG::DEBUG << "Creating a RegistryEntry for " << pathName << endreq;

  // Iterate over intermediate objects in the path, from parent to son
  // Make sure that every intermediate object exists, otherwise create it
  StatusCode sc;
  std::string lPath = "";       // path to current directory
  DataObject* lObject = 0;      // object in current directory
  bool isRoot = true;           // is current directory the oldest parent?
  std::string path = pathName;  // path still to be decomposed
  int sep;
  while ( (sep = path.find(IDataProviderSvc::SEPARATOR,1) ) > 0 ) {

    // Extract lPath for current directory in the tree
    lPath = lPath + std::string( path, 0,   sep           );
    path  =         std::string( path, sep, path.length() );    

    // Does the corresponding object exist or not?
    if( (retrieveObject(lPath,lObject)).isSuccess() ) {

      // Intermediate directory exists already
      log << MSG::DEBUG
          << "Catalog " << lPath << " exists: no need to create it" << endreq;
      // The root must exist already
      isRoot = false;

    } else {

      // Intermediate directory does not exist
      log << MSG::DEBUG
	  << "Catalog at " << lPath << " does not exist: create it" << endreq;
      lObject = new DataObject();

      // Is this the root for the data store?
      if ( isRoot ) {

	// Register the DataObject as an the root of the data store
	// If there is a valid root already (with another name), return error
	log << MSG::DEBUG 
	    << "Path " << lPath << " represents root for store " 
	    << name() << endreq;
	if ( checkRoot() ) {
	  log << MSG::ERROR << "Data store " 
	      << name() << " already has a valid root" << endreq;
	  return StatusCode::FAILURE;
	} else {
	  log << MSG::DEBUG << "Data store " << name()
	      << " has no valid root yet" << endreq;
	}
	log << MSG::DEBUG 
	    << "Set new root DataObject for data store " 
	    << name() << ": " << lPath << endreq;
	sc = setRoot(lPath,lObject);
	if ( !sc.isSuccess() ) {
	  log << MSG::ERROR << "Cannot set root for data store" << endreq;
	  return sc;
	}
	isRoot = false;
	
      } else {

	// Register the DataObject as an intermediate directory in the store
	log << MSG::DEBUG 
	    << "Register DataObject in the store at " << lPath << endreq;
	sc = registerObject(lPath,lObject);
	if ( !sc.isSuccess() ) {
	  log << MSG::ERROR 
	      << "Could not register DataObject in the store at " 
	      << lPath << endreq;
	  return sc;
	} else {
	  log << MSG::DEBUG 
	      << "Registered DataObject in the store at " << lPath << endreq;
	}

      } // test if directory is the root

    } // test if intermediate directory exists

  } // loop on intermediate directories

  // Now create the RegistryEntry at pathName
  DataSvcHelpers::RegistryEntry* parent;
  try {
    parent = 
      dynamic_cast<DataSvcHelpers::RegistryEntry*> ( m_root->find(lPath) );
  } catch (...) {
  }
  if (0 == parent ) {
    log << MSG::ERROR 
	<< "Could not locate RegistryEntry in the store at " 
	<< lPath << endreq;
    return StatusCode::FAILURE;
  }
  if (0 != m_root->find( pathName ) ) {
    log << MSG::ERROR 
	<< "RegistryEntry already exists in the store at " 
	<< pathName << endreq;
    return StatusCode::FAILURE;
  }
  entry = new DataSvcHelpers::RegistryEntry ( pathName );
  parent->add ( entry );
  if (0 == m_root->find( pathName ) ) {
    log << MSG::ERROR 
	<< "Unable to create RegistryEntry at " << pathName << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG << "Created a RegistryEntry for " << pathName << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

