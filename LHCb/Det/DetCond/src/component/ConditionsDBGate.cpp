//$Id: ConditionsDBGate.cpp,v 1.8 2002-03-01 17:01:08 andreav Exp $
#include <string>

#ifdef __CondDBObjy__
#  include "ConditionsDB/CondDBObjyDBMgrFactory.h"
#  ifdef __linux__
#    include <unistd.h>    /* Use gethostname on Linux    */
#    include <sys/param.h> /* Use MAXHOSTNAMELEN on Linux */
#  else
#    error __CondDBObjy__ implementation only exists for linux
#  endif
#else
#  ifdef __CondDBOracle__
#    include "ConditionsDB/CondDBOracleDBMgrFactory.h"
#  else
#    error Either __CondDBObjy__ or __CondDBOracle__ must be defined
#  endif
#endif

#include "ConditionsDBGate.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/TimePoint.h"

#include "ConditionsDB/CondDBObjFactory.h"
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
  declareProperty( "condDBUser", m_condDBUser = "" );
  declareProperty( "condDBPswd", m_condDBPswd = "" );
  declareProperty( "condDBHost", m_condDBHost = "" );
  declareProperty( "condDBName", m_condDBName = "" );
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
  log << MSG::INFO << "Specific initialization starting" << endreq;

  // Get properties from the JobOptionsSvc
  status = setProperties();
  if ( !status.isSuccess() ) {
    log << MSG::ERROR << "Could not set jobOptions properties" << endreq;
    return status;
  }
  log << MSG::DEBUG << "Properties were read from jobOptions" << endreq;
  log << MSG::DEBUG << "CondDB user:     " << endreq;
  log << MSG::DEBUG << m_condDBUser << endreq;
  log << MSG::DEBUG << "CondDB password: " << endreq;
  log << MSG::DEBUG << "********" << endreq;
  log << MSG::DEBUG << "CondDB host:     " << endreq;
  log << MSG::DEBUG << m_condDBHost << endreq;
  log << MSG::DEBUG << "CondDB name:     " << endreq;
  log << MSG::DEBUG << m_condDBName << endreq;

  // Now initialize CERN-IT CondDB
  try  {

#ifdef __CondDBObjy__

    //--- Begin of Objy implementation-specific code    

    m_condDBmgr = CondDBObjyDBMgrFactory::createCondDBMgr();

    // Initialise the database
    std::string condDBInfo;
    status = i_buildCondDBInfo( condDBInfo );
    if ( !status.isSuccess() ) return status;
    log << MSG::INFO << "CondDB Objy boot path is " << condDBInfo << endreq;
    m_condDBmgr->init( condDBInfo );

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

#else
#ifdef __CondDBOracle__

    //--- Begin of Oracle implementation-specific code    

    m_condDBmgr = CondDBOracleDBMgrFactory::createCondDBMgr();

    // Initialise the database
    std::string condDBInfo;
    status = i_buildCondDBInfo( condDBInfo );
    if ( !status.isSuccess() ) return status;
    m_condDBmgr->init( condDBInfo );
    log << MSG::VERBOSE << "Database successfully initialized" << endreq;

    // Create the database if one does not exist yet 
    // Use the default path (normally the database is created elsewhere)
    // If the database exists already, you cannot create a new one
    if ( m_condDBmgr->isCondDBcreated() ) {
      log << MSG::INFO 
	  << "Database already exists: no need to create a new one" << endreq;
    } else {
      log << MSG::INFO 
	  << "Database does not exist yet: create a new one" << endreq;
      m_condDBmgr->createCondDB(); 
      log << MSG::VERBOSE << "Database successfully created" << endreq;
    }

    // Open the database
    m_condDBmgr->openDatabase(); 
    log << MSG::VERBOSE << "Database successfully opened" << endreq;

    //--- End of Oracle implementation-specific code

#else
#error Either __CondDBObjy__ or __CondDBOracle__ must be defined
#endif
#endif

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

  log << MSG::INFO << "Specific initialization completed" << endreq;
  return status;
}

//----------------------------------------------------------------------------

/// Stop the service.
StatusCode ConditionsDBGate::finalize()
{
  MsgStream log(msgSvc(), "ConditionsDBGate" );
  log << MSG::DEBUG << "Finalizing" << endreq;
  if( m_condDBmgr != 0 )  {
#ifdef __CondDBObjy__
    CondDBObjyDBMgrFactory::destroyCondDBMgr( m_condDBmgr );
#else
#ifdef __CondDBOracle__
    CondDBOracleDBMgrFactory::destroyCondDBMgr( m_condDBmgr );
#else
#error Either __CondDBObjy__ or __CondDBOracle__ must be defined
#endif
#endif
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
  log << MSG::VERBOSE 
      << "In absTime = [" << refValidSince.absoluteTime()
      << ","   << refValidTill.absoluteTime()
      << "] :" << std::endl << data << endreq;
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

/// Build the technology-specific init string to access the database
StatusCode 
ConditionsDBGate::i_buildCondDBInfo( std::string& condDBInfo )
{  

#ifdef __CondDBObjy__

  /// Build the init string to access the Objy database

  MsgStream log(msgSvc(), "ConditionsDBGate" );
  log << MSG::DEBUG 
      << "Building the full Objy path to the ConditionsDB boot file" << endreq;

  // Try to retrieve the host for the boot file from the jobOptions
  // Assume it is on current host if no host is specified:
  // retrieve it via gethostname (MAXHOSTNAMELEN is defined in sys/param.h)
  if ( m_condDBHost != "" ) {
    log << MSG::DEBUG
	<< "Using condDBHost from jobOptions:" << endreq; 
    log << MSG::DEBUG << m_condDBHost << endreq;    
  } else {
    log << MSG::DEBUG << "Using current host as condDBHost:" << endreq; 
    char* hostname = new char[MAXHOSTNAMELEN];
    if ( gethostname(hostname, MAXHOSTNAMELEN) != 0 ) {
      log << MSG::ERROR << "System method gethostname failed" << endreq;
      return StatusCode::FAILURE;
    }
    m_condDBHost = hostname;
    log << MSG::DEBUG << m_condDBHost << endreq;    
    delete[] hostname;
  }
  
  // Try to retrieve the boot file name from the jobOptions
  if ( m_condDBName != "" ) {
    log << MSG::DEBUG 
	<< "Using condDBName from jobOptions:" << endreq; 
    log << MSG::DEBUG << m_condDBName << endreq;    
  } else {
    log << MSG::ERROR 
	<< "Property condDBName not set in jobOptions" << endreq; 
    return StatusCode::FAILURE;
  }

  // Create the full path to the boot file
  condDBInfo = m_condDBHost + "::" + m_condDBName;

  return StatusCode::SUCCESS;

#else
#ifdef __CondDBOracle__

  /// Build the init string to access the Oracle CondDB

  MsgStream log(msgSvc(), "ConditionsDBGate" );
  log << MSG::DEBUG 
      << "Building the Oracle init string for the ConditionsDB" << endreq;

  // Try to retrieve the user for the Oracle database from the jobOptions
  if ( m_condDBUser != "" ) {
    log << MSG::DEBUG
	<< "Using condDBUser from jobOptions:" << endreq; 
    log << MSG::DEBUG << m_condDBUser << endreq;    
  } else {
    log << MSG::ERROR 
	<< "Property condDBUser not set in jobOptions" << endreq; 
    return StatusCode::FAILURE;
  }
  
  // Try to retrieve the password for the Oracle database from the jobOptions
  if ( m_condDBPswd != "" ) {
    log << MSG::DEBUG
	<< "Using condDBPswd from jobOptions:" << endreq; 
    log << MSG::DEBUG << "********" << endreq;    
  } else {
    log << MSG::ERROR 
	<< "Property condDBPswd not set in jobOptions" << endreq; 
    return StatusCode::FAILURE;
  }
  
  // Try to retrieve the host for the Oracle database from the jobOptions
  if ( m_condDBHost != "" ) {
    log << MSG::DEBUG
	<< "Using condDBHost from jobOptions:" << endreq; 
    log << MSG::DEBUG << m_condDBHost << endreq;    
  } else {
    log << MSG::ERROR 
	<< "Property condDBHost not set in jobOptions" << endreq; 
    return StatusCode::FAILURE;
  }
  
  // Try to retrieve the ConditionsDB name from the jobOptions
  if ( m_condDBName != "" ) {
    log << MSG::DEBUG 
	<< "Using condDBName from jobOptions:" << endreq; 
    log << MSG::DEBUG << m_condDBName << endreq;    
  } else {
    log << MSG::ERROR 
	<< "Property condDBName not set in jobOptions" << endreq; 
    return StatusCode::FAILURE;
  }

  // Create the full dummy path to the boot file
  condDBInfo  = "user="     + m_condDBUser; // Oracle user
  condDBInfo += ",passwd=********";         // Oracle password
  condDBInfo += ",db="      + m_condDBHost; // Oracle DB (look in tnsnames.ora)
  condDBInfo += ",cond_db=" + m_condDBName; // User-defined CondDB name
  log << MSG::INFO << "CondDB Oracle path is " << condDBInfo << endreq;

  // Create the full path to the boot file
  condDBInfo  = "user="     + m_condDBUser; // Oracle user
  condDBInfo += ",passwd="  + m_condDBPswd; // Oracle password
  condDBInfo += ",db="      + m_condDBHost; // Oracle DB (look in tnsnames.ora)
  condDBInfo += ",cond_db=" + m_condDBName; // User-defined CondDB name

  return StatusCode::SUCCESS;

#else

#error Either __CondDBObjy__ or __CondDBOracle__ must be defined

#endif
#endif

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
