//$Id: BootDetectorStore.cpp,v 1.2 2001-11-29 13:56:24 andreav Exp $
#include <stdio.h>

#include "BootDetectorStore.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

/// Instantiation of a static factory to create instances of this algorithm
static const AlgFactory<BootDetectorStore> Factory;
const IAlgFactory& BootDetectorStoreFactory = Factory;

/// If not specifed, type and classID are discovered at runtime in the CondDB.
/// Use invalid values as keywords: CLID_NULL and TEST_StorageType.
/// Use the same values defined for ConditionsDBAddress.
const CLID&          ConditionsDBAddress_undefinedClassID   = CLID_NULL;
const unsigned char& ConditionsDBAddress_undefinedStringType= TEST_StorageType;

//----------------------------------------------------------------------------

/// Constructor
BootDetectorStore::BootDetectorStore( const std::string&  name, 
				      ISvcLocator*        pSvcLocator )
  : Algorithm(name, pSvcLocator)
{
  m_conditionStoreRoot = "/dd"; 
}

//----------------------------------------------------------------------------

/// Initialization of the algorithm. 

StatusCode BootDetectorStore::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize()" << endreq;

  // Query the IDataManagerSvc interface of the detector data service
  StatusCode sc = detSvc()->queryInterface(IID_IDataManagerSvc, 
					   (void**) &m_detDataManager);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 
	<< "Could not query IDataManagerSvc interface of DetectorDataSvc" 
	<< endreq;
    return sc;
  } else {
    log << MSG::DEBUG 
	<< "Retrieved IDataManagerSvc interface of DetectorDataSvc" 
	<< endreq;
  }

  // Locate the IAddressCreator interface of the ConditionsDBCnvSvc
  sc = serviceLocator()->service 
    ( "ConditionsDBCnvSvc", m_conditionsDBCnvSvc );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't locate ConditionsDBCnvSvc" << endreq;
    return sc;
  } else {
    log << MSG::INFO << "Succesfully located ConditionDBCnvSvc" << endreq;
  }

  // Now register condition folders
  log << MSG::DEBUG << "Now register condition folders" << endreq;
  if ( !i_registerCondition("/Conditions/LHCb/Slow/temp").isSuccess() ||
       !i_registerCondition("/Conditions/LHCb/Ecal/Slow/temp").isSuccess() ) {
    log << MSG::ERROR 
	<< "Could not register a condition folder" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG 
      << "Successfully registered all condition folders" << endreq;

  // Initialization completed
  log << MSG::INFO << "Initialization completed" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

StatusCode BootDetectorStore::execute( ) {
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode BootDetectorStore::finalize( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Finalize()" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Convert the folder name in the CondDB to the transient data store path
StatusCode 
BootDetectorStore::i_getNameInCondDB  ( std::string& folderName,
					const std::string& tdsName ) {

  MsgStream log(msgSvc(), name() );
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
BootDetectorStore::i_getNameInStore ( std::string& path,
				      const std::string& folderName ) {

  MsgStream log(msgSvc(), name() );
  path = m_conditionStoreRoot + folderName;
  if ( path.length() == 0 || path == m_conditionStoreRoot ) {
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

/// Register a condition in the detector store by CondDB folder name
/// Return an error if an entry exists already at the corresponding path
/// In the address, specify the global tag (owned by the ConditionsDBCnvSvc),
/// the technology type for the strings stored in the CondDB and the classID.
StatusCode 
BootDetectorStore::i_registerCondition  ( const std::string&   folderName,
					  const CLID&          classID,
					  const unsigned char& type )
{
  MsgStream log(msgSvc(), name() );
  StatusCode sc;
  log << MSG::DEBUG 
      << "Register in the detector store condition data from folder:" 
      << endreq;
  log << MSG::DEBUG << folderName << endreq;

  // Convert the folder name into a data store path
  std::string path;
  sc = i_getNameInStore( path, folderName );
  if ( !sc.isSuccess() ) return sc;
  log << MSG::DEBUG 
      << "DataObject will be registered in the store as:" << endreq;
  log << MSG::DEBUG << path << endreq;
  
  // First, make sure that a valid root exists
  DataObject* pRoot;
  if ( !detSvc()->retrieveObject(m_detDataManager->rootName(),pRoot)
       .isSuccess() ) {
    log << MSG::DEBUG 
	<< "Detector data store has no valid root yet" << endreq;
    log << MSG::DEBUG 
	<< "Set new root DataObject for data store: " 
	<< m_detDataManager->rootName() << endreq;
    pRoot = new DataObject();
    sc = m_detDataManager->setRoot(m_detDataManager->rootName(),pRoot);
    if ( !sc.isSuccess() ) {
      log << MSG::ERROR << "Cannot set root for data store" << endreq;
      return sc;
    }
  }

  // Create an address
  IOpaqueAddress* theAddress;
  const std::string par[2] = {folderName, "DEFAULT"};
  const unsigned long ipar[1] = {type};
  log << MSG::DEBUG
      << "Create a new address associated to this entry" << endreq;
  sc = m_conditionsDBCnvSvc->createAddress
    ( CONDDB_StorageType, classID, par, ipar, theAddress );
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Cannot create address" << endreq;
    return sc;
  } else {
    log << MSG::DEBUG 
	<< "Created new address of type=" << (int)theAddress->svcType() 
	<< " classID=" << classID
	<< " stringType=" << (int)type << endreq;
  }

  // Force the creation of all intermediate leaves if they do not exist yet
  // NB You need to set "ForceLeaves" to true in the job options
  int sep = path.find_last_of(IDataProviderSvc::SEPARATOR);
  if ( sep == 0 ) {
    log << MSG::ERROR << "Invalid terminal path " << path << endreq;
    return StatusCode::FAILURE;
  }
  DataObject* pParent;
  std::string parentPath = std::string( path, 0, sep );
  sc = detSvc()->retrieveObject( parentPath, pParent );
  if ( !sc.isSuccess() ) {
    log << MSG::DEBUG << "Cannot retrieve parent " << parentPath << endreq;
    log << MSG::DEBUG << "Create a new DataObject registered as parent " 
	<< parentPath << endreq;
    pParent = new DataObject();
    sc = detSvc()->registerObject( parentPath, pParent );
    if ( !sc.isSuccess() ) {
      log << MSG::ERROR << "Cannot register parent " << parentPath << endreq;
      log << MSG::ERROR 
	  << "Have you forgotten to set DetectorDataSvc.ForceLeaves={true}?" 
	  << endreq;
      return sc;
    } else {
      log << MSG::DEBUG 
	  << "Successfully registered parent " << parentPath << endreq;
    }  
  } else {
    log << MSG::DEBUG 
	<< "Successfully retrieved parent " << parentPath << endreq;
  }  

  // Now create a RegistryEntry at the given path and register the address
  sc = m_detDataManager->registerAddress( path, theAddress );
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Cannot register address at path " << path << endreq;
    return sc;
  } else {
    log << MSG::DEBUG 
	<< "Address successfully registered at " << path << endreq;
  }
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Register a condition in the detector store by CondDB folder name
/// Return an error if an entry exists already at the corresponding path
/// If not specifed, type and classID are discovered at runtime in the CondDB
StatusCode 
BootDetectorStore::i_registerCondition  ( const std::string& folderName )
{
  return i_registerCondition (folderName,
			      ConditionsDBAddress_undefinedClassID, 
			      ConditionsDBAddress_undefinedStringType );
}

//----------------------------------------------------------------------------

