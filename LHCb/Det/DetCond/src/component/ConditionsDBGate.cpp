//$Id: ConditionsDBGate.cpp,v 1.3 2001-11-27 18:19:59 andreav Exp $
#include <string>
#include <unistd.h>
#include <sys/param.h>

#include "ConditionsDBGate.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/TimePoint.h"

#include "ConditionsDB/CondDBObjFactory.h"
#include "ConditionsDB/CondDBObjyDBMgrFactory.h"
#include "ConditionsDB/ICondDBFolder.h"
#include "ConditionsDB/ICondDBFolderMgr.h"

//----------------------------------------------------------------------------

/// Constructor
ConditionsDBGate::ConditionsDBGate( const std::string& name, 
				    ISvcLocator* svc)
  : Service(name, svc)
  , m_condDBmgr          ( 0     )
  , m_condDBDataAccess   ( 0     )
  , m_condDBFolderMgr    ( 0     )
{
  declareProperty( "condDBBootHost",   m_condDBBootHost  = ""     );
  declareProperty( "condDBBootDir",    m_condDBBootDir   = ""     );
  declareProperty( "condDBBootFile",   m_condDBBootFile  = ""     );
}

//----------------------------------------------------------------------------

/// Destructor
ConditionsDBGate::~ConditionsDBGate()
{
}

//----------------------------------------------------------------------------

/// Initialize the service.
StatusCode ConditionsDBGate::initialize()
{

  // Before anything else, we have to initialize grandfather
  StatusCode status = Service::initialize();
  if ( !status.isSuccess() ) return status;

  // Now we can get a handle to the MessageSvc
  MsgStream log(msgSvc(), "ConditionsDBGate" );
  log << MSG::DEBUG << "Specific initialization starting" << endreq;

  // Get properties from the JobOptionsSvc
  status = setProperties();
  if ( !status.isSuccess() ) {
    log << MSG::ERROR << "Could not set jobOptions properties" << endreq;
    return status;
  }
  log << MSG::DEBUG << "Properties were read from jobOptions" << endreq;
  log << MSG::DEBUG << "CondDB boot host:       " << endreq;
  log << MSG::DEBUG << m_condDBBootHost  << endreq;
  log << MSG::DEBUG << "CondDB boot directory:  " << endreq;
  log << MSG::DEBUG << m_condDBBootDir   << endreq;
  log << MSG::DEBUG << "CondDB boot file:       " << endreq;
  log << MSG::DEBUG << m_condDBBootFile  << endreq;

  // Now initialize CERN-IT CondDB
  try  {

    m_condDBmgr = CondDBObjyDBMgrFactory::createCondDBMgr();

    //--- Begin of Objy implementation-specific code    

    // Initialise the database
    std::string condDBBootPath;
    status = i_buildCondDBBootPath( condDBBootPath );
    if ( !status.isSuccess() ) return status;
    m_condDBmgr->init( condDBBootPath );

    // Create the database 
    // Use the default path (normally the database is created elsewhere)
    // If the database exists already, this method does nothing
    m_condDBmgr->startUpdate();
    m_condDBmgr->createCondDB(); 
    m_condDBmgr->commit();

    // Open the database
    m_condDBmgr->startRead();
    m_condDBmgr->openDatabase(); 
    m_condDBmgr->commit();

    //--- End of Objy implementation-specific code

    // Get dataAccess and folderMgr handles
    m_condDBDataAccess = m_condDBmgr->getCondDBDataAccess();
    m_condDBFolderMgr  = m_condDBmgr->getCondDBFolderMgr();
        
  } catch (CondDBException &e) {

    log << MSG::ERROR << "Error in CondDB initialization!"         << endreq;
    log << MSG::ERROR << "*** ConditionsDB exception caught:"      << endreq;
    log << MSG::ERROR << "***  error message:" << e.getMessage()   << endreq;
    log << MSG::ERROR << "***  error code:   " << e.getErrorCode() << endreq;
    return StatusCode::FAILURE;

  }

  log << MSG::DEBUG << "Specific initialization completed" << endreq;
  return status;
}

//----------------------------------------------------------------------------

/// Stop the service.
StatusCode ConditionsDBGate::finalize()
{
  MsgStream log(msgSvc(), "ConditionsDBGate" );
  log << MSG::DEBUG << "Finalizing" << endreq;
  if( m_condDBmgr != 0 )  {
    CondDBObjyDBMgrFactory::destroyCondDBMgr( m_condDBmgr );
  }
  return Service::finalize();
}

//----------------------------------------------------------------------------

/// Query interface
StatusCode ConditionsDBGate::queryInterface(const IID& riid, 
					    void** ppvInterface)
{
  if ( IID_IConditionsDBGate == riid )  {
    // With the highest priority return the specific interface of this service
    *ppvInterface = (IConditionsDBGate*)this;
  } else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

/// Read range and data of a CondDBObject by folder name, tag and time
StatusCode 
ConditionsDBGate::readCondDBObject      ( ITime&              refValidSince,
					  ITime&              refValidTill,
					  std::string&        data,
					  const std::string&  folderName,
					  const std::string&  tagName,
					  const ITime&        time ) {

  MsgStream log(msgSvc(), "ConditionsDBGate" );
  log << MSG::DEBUG << "Retrieving CondDBObject from the CondDB" << endreq;
  log << MSG::DEBUG << "FolderName=" << folderName << endreq;
  log << MSG::DEBUG << "AbsoluteTime=" << time.absoluteTime() << endreq; 

  // Convert ITime to CondDBKey
  CondDBKey key;
  StatusCode status = i_convertToKey ( key, time );
  if ( !status.isSuccess() ) return status;
  log << MSG::DEBUG << "Key=" << key << endreq; 

  // Create an object (must delete it at the end)
  ICondDBObject* aCondDBObject;
  status = i_findCondDBObject ( aCondDBObject, folderName, tagName, key ); 
  if ( !status.isSuccess() ) return status;

  // Set required parameters
  status = i_convertToITime ( refValidSince, aCondDBObject->validSince() );
  if ( !status.isSuccess() ) return status;
  status = i_convertToITime ( refValidTill,  aCondDBObject->validTill()  );
  if ( !status.isSuccess() ) return status;
  aCondDBObject->data( data );

  // Delete the object and return
  delete aCondDBObject;
  log << MSG::DEBUG << "CondDBObject data succesfully read" << endreq;
  log << MSG::DEBUG 
      << "In absTime = [ " << refValidSince.absoluteTime()
      << " , "   << refValidTill.absoluteTime()
      << " ] : '" << data << "'" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Read the description of a folder in the CondDB
StatusCode 
ConditionsDBGate::readCondDBFolder ( std::string&        description,
				     const std::string&  folderName )
{

  MsgStream log(msgSvc(), "ConditionsDBGate" );
  log << MSG::DEBUG << "Reading folder description from the CondDB " << endreq;
  log << MSG::DEBUG << "FolderName=" << folderName << endreq;

  try {

    ICondDBFolder* folder;
    m_condDBmgr->startRead();
    m_condDBFolderMgr->getCondDBFolder( folderName, folder );
    description = folder->getDescription();
    delete folder;
    m_condDBmgr->commit();
    
  } catch (CondDBException &e) {      
    
    description = "";
    log << MSG::ERROR
	<< "Could not read CondDBFolder description from the CondDB" << endreq;
    log << MSG::ERROR << "*** ConditionsDB exception caught:"      << endreq;
    log << MSG::ERROR << "***  error message:" << e.getMessage()   << endreq;
    log << MSG::ERROR << "***  error code:   " << e.getErrorCode() << endreq;
    return StatusCode::FAILURE;
    
  }
  
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Find a CondDB object in the CondDB by folder name, tag and key
StatusCode 
ConditionsDBGate::i_findCondDBObject ( ICondDBObject*&     refpCobject,
				       const std::string&  folderName,
				       const std::string&  tagName,
				       const CondDBKey&    key)
{

  MsgStream log(msgSvc(), "ConditionsDBGate" );
  try {

    m_condDBmgr->startRead();
    m_condDBDataAccess->findCondDBObject
      ( refpCobject, folderName, key, tagName );
    m_condDBmgr->commit();
    
  } catch (CondDBException &e) {      
    
    refpCobject = 0;
    log << MSG::ERROR
	<< "Could not retrieve CondDBObject from the CondDB" << endreq;
    log << MSG::ERROR << "*** ConditionsDB exception caught:"      << endreq;
    log << MSG::ERROR << "***  error message:" << e.getMessage()   << endreq;
    log << MSG::ERROR << "***  error code:   " << e.getErrorCode() << endreq;
    return StatusCode::FAILURE;
    
  }
  
  // Check whether this is the NULL interval
  if ( refpCobject->isNullInterval() ) {
    log << MSG::ERROR
	<< "There is no data stored in the database at key " << key << endreq;
    log << MSG::ERROR
	<< "NULL interval retrieved:" << endreq;
    std::string data;
    refpCobject->data( data );
    log << MSG::ERROR 
	<< "In key = [ " << refpCobject->validSince()
	<< " , "   << refpCobject->validTill()
	<< " ] : '" << data << "'" << endreq;
    refpCobject = 0;
    return StatusCode::FAILURE;
  }    

  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Build the full path to the Objy boot file for the Federation
StatusCode 
ConditionsDBGate::i_buildCondDBBootPath( std::string& condDBBootPath )
{  

  MsgStream log(msgSvc(), "ConditionsDBGate" );
  log << MSG::DEBUG 
      << "Building the full Objy path to the ConditionsDB boot file" << endreq;

  // Try to retrieve the host for the boot file from the jobOptions
  // Assume it is on current host if no host is specified:
  // retrieve it via gethostname (MAXHOSTNAMELEN is defined in sys/param.h)
  if ( m_condDBBootHost != "" ) {
    log << MSG::DEBUG 
	<< "Using condDBBootHost from jobOptions file:" << endreq; 
    log << MSG::DEBUG << m_condDBBootHost << endreq;    
  } else {
    log << MSG::DEBUG << "Using current host as condDBBootHost:" << endreq; 
    char* hostname = new char[MAXHOSTNAMELEN];
    if ( gethostname(hostname, MAXHOSTNAMELEN) != 0 ) {
      log << MSG::ERROR << "System method gethostname failed" << endreq;
      return StatusCode::FAILURE;
    }
    m_condDBBootHost = hostname;
    log << MSG::DEBUG << m_condDBBootHost << endreq;    
    delete[] hostname;
  }
  
  // Try to retrieve the boot file directory from the jobOptions
  if ( m_condDBBootDir != "" ) {
    log << MSG::DEBUG 
	<< "Using condDBBootDir from jobOptions file:" << endreq; 
    log << MSG::DEBUG << m_condDBBootDir << endreq;    
  } else {
    log << MSG::ERROR 
	<< "Property condDBBootDir not set in jobOptions file" << endreq; 
    return StatusCode::FAILURE;
  }

  // Try to retrieve the boot file name from the jobOptions
  if ( m_condDBBootFile != "" ) {
    log << MSG::DEBUG 
	<< "Using condDBBootFile from jobOptions file:" << endreq; 
    log << MSG::DEBUG << m_condDBBootFile << endreq;    
  } else {
    log << MSG::ERROR 
	<< "Property condDBBootFile not set in jobOptions file" << endreq; 
    return StatusCode::FAILURE;
  }

  // Create the full path to the boot file
  condDBBootPath =
    m_condDBBootHost + "::" + 
    m_condDBBootDir + "/" +
    m_condDBBootFile;

  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Convert CondDBKey to Gaudi ITime
StatusCode ConditionsDBGate::i_convertToITime ( ITime& refTime, 
						const CondDBKey& key ) {
  refTime -= refTime;
  refTime += TimePoint( key );
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Convert Gaudi ITime to CondDBKey
StatusCode ConditionsDBGate::i_convertToKey ( CondDBKey& key,
					      const ITime& refTime ) {
  
  key = refTime.absoluteTime();
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------
