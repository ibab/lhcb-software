//$Id: ConditionsDBGate.cpp,v 1.15 2004-12-08 17:12:07 marcocle Exp $
#include <string>

#ifdef WIN32
#pragma warning ( disable : 4786 )
#endif

#ifdef __CondDBObjy__
#  include "ConditionsDB/CondDBObjyDBMgrFactory.h"
#  ifdef __linux__
#    include <unistd.h>    /* Use gethostname on Linux    */
#    include <sys/param.h> /* Use MAXHOSTNAMELEN on Linux */
#  else
#    error __CondDBObjy__ implementation only exists for linux
#  endif
#endif

#ifdef __CondDBMySQL__
#  include "ConditionsDB/CondDBMySQLMgrFactory.h"
#  include "ConditionsDB/CondDBObjFactory.h"
#endif

#ifdef __CondDBOracle__
#  include "ConditionsDB/CondDBOracleMgrFactory.h"
#  include <stdlib.h> /* For getenv */
#  include "ConditionsDB/ICondDBObject.h"
#endif

#include "ConditionsDBGate.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/TimePoint.h"

#include "ConditionsDB/ICondDBFolder.h"
#include "ConditionsDB/ICondDBBasicFolderMgr.h"
#include "ConditionsDB/ICondDBBasicDataAccess.h"

//----------------------------------------------------------------------------
using namespace ConditionsDBGateImplementation;

/// Constructor
ConditionsDBGate::ConditionsDBGate( const std::string& name, 
                                    ISvcLocator* svc)
  : Service(name, svc)
  , m_condDBmgr          ( 0 )
  , m_condDBDataAccess   ( 0 )
  , m_condDBFolderMgr    ( 0 )
{
  declareProperty( "condDBImpl",     m_condDBImpl = "" );
  declareProperty( "condDBUser",     m_condDBUser = "" );
  declareProperty( "condDBPswd",     m_condDBPswd = "" );
  declareProperty( "condDBHost",     m_condDBHost = "" );
  declareProperty( "condDBName",     m_condDBName = "" );
  declareProperty( "showCondDBPswd", m_showCondDBPswd = false );
  m_condDBImplCode = 0;  
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

  // Hack for undefined Windows environment variables
  // If I use "$VARIABLE" in the job options, it is not translated!
  if ( m_condDBImpl.c_str()[0] == '$' ) m_condDBImpl = "";  
  if ( m_condDBHost.c_str()[0] == '$' ) m_condDBHost = "";  
  if ( m_condDBUser.c_str()[0] == '$' ) m_condDBUser = "";  
  if ( m_condDBPswd.c_str()[0] == '$' ) m_condDBPswd = "";  
  if ( m_condDBName.c_str()[0] == '$' ) m_condDBName = "";  

  // Print out the properties that were read
  log << MSG::DEBUG << "CondDB implementation technology:" << endreq;
  if ( m_condDBImpl == "" ) {
    log << MSG::ERROR << "No implementation technology specified!" << endreq;
    return StatusCode::FAILURE;
  } else if ( m_condDBImpl == "CondDBObjy" ) {    
    m_condDBImplCode = CONDDBOBJY;
    log << MSG::DEBUG << m_condDBImpl << endreq;
  } else if ( m_condDBImpl == "CondDBOracle" ) {    
    m_condDBImplCode = CONDDBORACLE;
    log << MSG::DEBUG << m_condDBImpl << endreq;
  } else if ( m_condDBImpl == "CondDBMySQL" ) {
    m_condDBImplCode = CONDDBMYSQL;
    log << MSG::DEBUG << m_condDBImpl << endreq;
  } else {
    log << MSG::DEBUG << "Unknown implementation technology: " 
        << m_condDBImpl << endreq;
    return StatusCode::FAILURE;
  } 
  log << MSG::DEBUG << "CondDB user:" << endreq;
  log << MSG::DEBUG << m_condDBUser << endreq;
  log << MSG::DEBUG << "Show CondDB password?" << endreq;
  log << MSG::DEBUG << m_showCondDBPswd << endreq;
  log << MSG::DEBUG << "CondDB password:" << endreq;
  if ( m_showCondDBPswd ) {
    log << MSG::DEBUG << m_condDBPswd << endreq;
  } else {
    log << MSG::DEBUG << "********" << endreq;
  }
  log << MSG::DEBUG << "CondDB host:" << endreq;
  log << MSG::DEBUG << m_condDBHost << endreq;
  log << MSG::DEBUG << "CondDB name:" << endreq;
  log << MSG::DEBUG << m_condDBName << endreq;

  // Now initialize CERN-IT CondDB
  try  {

    switch ( m_condDBImplCode ) {
    case CONDDBOBJY:
      status = i_initDBObjy();
      break;
    case CONDDBORACLE:
      status = i_initDBOracle();
      break;
    case CONDDBMYSQL:
      status = i_initDBMySQL();
      break;
    default:
      log << MSG::ERROR << "Unknown implementation technology: " 
          << m_condDBImpl << "(code " << m_condDBImplCode << ")" << endreq;
      return StatusCode::FAILURE;
    }
    if ( ! status.isSuccess() ) return status;
    // Get dataAccess and folderMgr handles
    m_condDBDataAccess = m_condDBmgr->getCondDBBasicDataAccess();
    m_condDBFolderMgr  = m_condDBmgr->getCondDBBasicFolderMgr();
        
  } catch (CondDBException &e) {

    log << MSG::ERROR << "Error in CondDB initialization!"         << endreq;
    log << MSG::ERROR << "*** ConditionsDB exception caught:"      << endreq;
    log << MSG::ERROR << "***  error message:" << e.getMessage()   << endreq;
    log << MSG::ERROR << "***  error code:   " << e.getErrorCode() << endreq;
    return StatusCode::FAILURE;

  } catch (...) {

    log << MSG::ERROR << "Error in CondDB initialization!" << endreq;
    log << MSG::ERROR << "***  UNKNOWN exception caught"   << endreq;
    return StatusCode::FAILURE;

  }

  log << MSG::INFO << "Specific initialization completed" << endreq;
  return status;
}

//----------------------------------------------------------------------------

/// Perform the technology-dependent part of DB initialization
StatusCode ConditionsDBGate::i_initDBObjy()
{

  MsgStream log(msgSvc(), "ConditionsDBGate" );
  log << MSG::INFO << "Objy-specific initialization starting" << endreq;

#ifdef __CondDBObjy__
  
  //--- Begin of Objy implementation-specific code    
  m_condDBmgr = CondDBObjyDBMgrFactory::createCondDBMgr();
  
  // Initialise the database
  std::string condDBInfo;
  StatusCode status = i_buildCondDBInfo( condDBInfo );
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
  return status;
  
#else

  log << MSG::ERROR << "CondDB Oracle implementation not linked" << endreq;
  return StatusCode::FAILURE;
  
#endif

}

//----------------------------------------------------------------------------

/// Perform the technology-dependent part of DB initialization
StatusCode ConditionsDBGate::i_initDBOracle()
{

  MsgStream log(msgSvc(), "ConditionsDBGate" );
  log << MSG::INFO << "Oracle-specific initialization starting" << endreq;

#ifdef __CondDBOracle__

  //--- Begin of Oracle implementation-specific code    
  //  m_condDBmgr = cool::CondDBOracleMgrFactory::createCondDBMgr();
  m_condDBmgr = CondDBOracleMgrFactory::createCondDBMgr();
  
  // Initialise the database
  std::string condDBInfo;
  StatusCode status = i_buildCondDBInfo( condDBInfo );
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
  return status;
  
#else
  
  log << MSG::ERROR << "CondDB Oracle implementation not linked" << endreq;
  return StatusCode::FAILURE;
  
#endif

}

//----------------------------------------------------------------------------

/// Perform the technology-dependent part of DB initialization
StatusCode ConditionsDBGate::i_initDBMySQL()
{

  MsgStream log(msgSvc(), "ConditionsDBGate" );
  log << MSG::INFO << "MySQL-specific initialization starting" << endreq;

#ifdef __CondDBMySQL__

  //--- Begin of MySQL implementation-specific code    
  m_condDBmgr = CondDBMySQLMgrFactory::createCondDBMgr();
  
  // Initialise the database
  std::string condDBInfo;
  StatusCode status = i_buildCondDBInfo( condDBInfo );
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
  
  //--- End of MySQL implementation-specific code
  return status;
  
#else

  log << MSG::ERROR << "CondDB MySQL implementation not linked" << endreq;
  return StatusCode::FAILURE;
  
#endif

}

//----------------------------------------------------------------------------

/// Stop the service.
StatusCode ConditionsDBGate::finalize()
{

  MsgStream log(msgSvc(), "ConditionsDBGate" );
  log << MSG::DEBUG << "Finalizing" << endreq;
  if( m_condDBmgr != 0 )  {
    
    switch ( m_condDBImplCode ) {
    case CONDDBOBJY:
#ifdef __CondDBObjy__
      CondDBObjyDBMgrFactory::destroyCondDBMgr( m_condDBmgr );
#else
      log << MSG::ERROR << "CondDB Objy implementation not linked" << endreq;
      return StatusCode::FAILURE;
#endif
      break;
      
    case CONDDBORACLE:
#ifdef __CondDBOracle__
      //      cool::CondDBOracleMgrFactory::destroyCondDBMgr( m_condDBmgr );
      CondDBOracleMgrFactory::destroyCondDBMgr( m_condDBmgr );
#else
      log << MSG::ERROR << "CondDB Oracle implementation not linked" << endreq;
      return StatusCode::FAILURE;
#endif
      break;
      
    case CONDDBMYSQL:
#ifdef __CondDBMySQL__
      CondDBMySQLMgrFactory::destroyCondDBMgr( m_condDBmgr );
#else
      log << MSG::ERROR << "CondDB MySQL implementation not linked" << endreq;
      return StatusCode::FAILURE;
#endif
      break;
      
    default:
      log << MSG::ERROR << "Unknown implementation technology: " 
          << m_condDBImpl << "(code " << m_condDBImplCode << ")" << endreq;
      return StatusCode::FAILURE;
    }
  }

  return Service::finalize();

}

//----------------------------------------------------------------------------

/// Query interface
StatusCode ConditionsDBGate::queryInterface(const InterfaceID& riid, 
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
  log << MSG::DEBUG << "AbsoluteTime=" << time.absoluteTime() 
      << "(0x" << std::hex 
      << time.absoluteTime() 
      << std::dec << ")" 
      << endreq; 

  // Convert ITime to CondDBKey
  CondDBKey key;
  StatusCode status = i_convertToKey ( key, time );
  if ( !status.isSuccess() ) return status;
  log << MSG::DEBUG << "Key=" << key 
      << "(0x" << std::hex 
      << key
      << std::dec << ")"
      << endreq; 

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
      << "(0x" << std::hex 
      << refValidSince.absoluteTime()
      << std::dec << ")" 
      << ","   << refValidTill.absoluteTime()
      << "(0x" << std::hex 
      << refValidTill.absoluteTime()
      << std::dec << ")" 
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
	<< "(0x" << std::hex 
	<< refpCobject->validSince()
	<< std::dec << ")" 
	<< " , "   << refpCobject->validTill()
	<< "(0x" << std::hex 
	<< refpCobject->validTill()
	<< std::dec << ")" 
	<< " ] : '" << data << "'" << endreq;
    refpCobject = 0;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Build the technology-specific init string to access the database
StatusCode 
ConditionsDBGate::i_buildCondDBInfoObjy( std::string& condDBInfo )
{  

  /// Build the init string to access the Objy database
  MsgStream log(msgSvc(), "ConditionsDBGate" );
  log << MSG::VERBOSE 
      << "Building the full Objy path to the ConditionsDB boot file" << endreq;

#ifdef __CondDBObjy__

  // Try to retrieve the host for the boot file from the jobOptions
  // Assume it is on current host if no host is specified:
  // retrieve it via gethostname (MAXHOSTNAMELEN is defined in sys/param.h)
  if ( m_condDBHost != "" ) {
    log << MSG::VERBOSE
	<< "Using condDBHost from jobOptions:" << endreq; 
    log << MSG::VERBOSE << m_condDBHost << endreq;    
  } else {
    log << MSG::VERBOSE << "Using current host as condDBHost:" << endreq; 
    char* hostname = new char[MAXHOSTNAMELEN];
    if ( gethostname(hostname, MAXHOSTNAMELEN) != 0 ) {
      log << MSG::ERROR << "System method gethostname failed" << endreq;
      return StatusCode::FAILURE;
    }
    m_condDBHost = hostname;
    log << MSG::VERBOSE << m_condDBHost << endreq;    
    delete[] hostname;
  }
  
  // Try to retrieve the boot file name from the jobOptions
  if ( m_condDBName != "" ) {
    log << MSG::VERBOSE 
	<< "Using condDBName from jobOptions:" << endreq; 
    log << MSG::VERBOSE << m_condDBName << endreq;    
  } else {
    log << MSG::ERROR 
	<< "Property condDBName not set in jobOptions" << endreq; 
    return StatusCode::FAILURE;
  }

  // Create the full path to the boot file
  condDBInfo = m_condDBHost + "::" + m_condDBName;
  log << MSG::INFO << "CondDB Objy boot path is " << condDBInfo << endreq;

  return StatusCode::SUCCESS;

#else

  log << MSG::ERROR 
      << "CondDB Objy implementation has not been linked!" << endreq;
  condDBInfo = "";
  return StatusCode::FAILURE;

#endif

}

//----------------------------------------------------------------------------

/// Build the technology-specific init string to access the database
StatusCode 
ConditionsDBGate::i_buildCondDBInfoOracle( std::string& condDBInfo )
{  

  /// Build the init string to access the Oracle CondDB
  MsgStream log(msgSvc(), "ConditionsDBGate" );
  log << MSG::VERBOSE 
      << "Building the Oracle init string for the ConditionsDB" << endreq;

#ifdef __CondDBOracle__

  // Print out NLS_LANG and ORA_NLS33 to help debug ORA-12705
  log << MSG::VERBOSE << "Environment variable NLS_LANG is ";
  if ( 0 == getenv( "NLS_LANG" ) ) {
    log << "(undefined)" << endreq;
  } else {
    log << getenv("NLS_LANG") << endreq;
  }
  log << MSG::VERBOSE << "Environment variable ORA_NLS33 is ";
  if ( 0 == getenv( "ORA_NLS33" ) ) {
    log << "(undefined)" << endreq;
  } else {
    log << getenv("ORA_NLS33") << endreq;
  }

  // Try to retrieve the user for the Oracle database from the jobOptions
  if ( m_condDBUser != "" ) {
    log << MSG::VERBOSE
	<< "Using condDBUser from jobOptions:" << endreq; 
    log << MSG::VERBOSE << m_condDBUser << endreq;    
  } else {
    log << MSG::ERROR 
	<< "Property condDBUser not set in jobOptions" << endreq; 
    return StatusCode::FAILURE;
  }
  
  // Try to retrieve the password for the Oracle database from the jobOptions
  if ( m_condDBPswd != "" ) {
    log << MSG::VERBOSE
	<< "Using condDBPswd from jobOptions:" << endreq; 
    if ( !m_showCondDBPswd ) {
      log << MSG::VERBOSE << "********" << endreq;    
    } else {
      log << MSG::VERBOSE << m_condDBPswd << endreq;    
    }
  } else {
    log << MSG::ERROR 
	<< "Property condDBPswd not set in jobOptions" << endreq; 
    return StatusCode::FAILURE;
  }
  
  // Try to retrieve the host for the Oracle database from the jobOptions
  if ( m_condDBHost != "" ) {
    log << MSG::VERBOSE
	<< "Using condDBHost from jobOptions:" << endreq; 
    log << MSG::VERBOSE << m_condDBHost << endreq;    
  } else {
    log << MSG::ERROR 
	<< "Property condDBHost not set in jobOptions" << endreq; 
    return StatusCode::FAILURE;
  }
  
  // Try to retrieve the ConditionsDB name from the jobOptions
  if ( m_condDBName != "" ) {
    log << MSG::VERBOSE 
	<< "Using condDBName from jobOptions:" << endreq; 
    log << MSG::VERBOSE << m_condDBName << endreq;    
  } else {
    log << MSG::ERROR 
	<< "Property condDBName not set in jobOptions" << endreq; 
    return StatusCode::FAILURE;
  }

  if ( !m_showCondDBPswd ) {
    // Create a printable Oracle init string (without the password)
    condDBInfo  = "user="     + m_condDBUser; // Oracle user
    condDBInfo += ",passwd=********";         // Oracle password
    condDBInfo += ",db="      + m_condDBHost; // Oracle DB (tnsnames.ora)
    condDBInfo += ",cond_db=" + m_condDBName; // User-defined CondDB name
    log << MSG::INFO << "CondDB Oracle init string is " 
	<< condDBInfo << endreq;
  }

  // Create the actual Oracle init string
  condDBInfo  = "user="     + m_condDBUser; // Oracle user
  condDBInfo += ",passwd="  + m_condDBPswd; // Oracle password
  condDBInfo += ",db="      + m_condDBHost; // Oracle DB (tnsnames.ora)
  condDBInfo += ",cond_db=" + m_condDBName; // User-defined CondDB name
  if ( m_showCondDBPswd ) {
    log << MSG::INFO << "CondDB Oracle init string is " 
	<< condDBInfo << endreq;
  }

  return StatusCode::SUCCESS;

#else

  log << MSG::ERROR 
      << "CondDB Oracle implementation has not been linked!" << endreq;
  condDBInfo = "";
  return StatusCode::FAILURE;

#endif

}

//----------------------------------------------------------------------------

/// Build the technology-specific init string to access the database
StatusCode 
ConditionsDBGate::i_buildCondDBInfoMySQL( std::string& condDBInfo )
{  

  MsgStream log(msgSvc(), "ConditionsDBGate" );
  /// Build the init string to access the MySQL CondDB
  log << MSG::VERBOSE 
      << "Building the MySQL init string for the ConditionsDB" << endreq;

#ifdef __CondDBMySQL__

  // Try to retrieve the user for the MySQL database from the jobOptions
  log << MSG::VERBOSE
      << "Using condDBUser:" << endreq; 
  log << MSG::VERBOSE << m_condDBUser << endreq;    
  
  // Try to retrieve the password for the MySQL database from the jobOptions
  log << MSG::VERBOSE
      << "Using condDBPswd:" << endreq; 
  if ( !m_showCondDBPswd ) {
    log << MSG::VERBOSE << "********" << endreq;    
  } else {
    log << MSG::VERBOSE << m_condDBPswd << endreq;    
  }
  
  // Try to retrieve the host for the MySQL database from the jobOptions
  log << MSG::VERBOSE
      << "Using condDBHost:" << endreq; 
  log << MSG::VERBOSE << m_condDBHost << endreq;    
  
  // Try to retrieve the ConditionsDB name from the jobOptions
  log << MSG::VERBOSE 
      << "Using condDBName:" << endreq; 
  log << MSG::VERBOSE << m_condDBName << endreq;    

  if ( !m_showCondDBPswd ) {
    // Create a printable MySQL init string (without the password)
    condDBInfo  = m_condDBHost;       // MySQL server host
    condDBInfo += ":" + m_condDBName; // MySQL DB name
    condDBInfo += ":" + m_condDBUser; // MySQL user
    condDBInfo += ":********";        // MySQL password
    log << MSG::INFO << "CondDB MySQL init string is " 
	<< condDBInfo << endreq;
  }

  // Create the actual MySQL init string
  condDBInfo  =       m_condDBHost; // MySQL server host
  condDBInfo += ":" + m_condDBName; // MySQL DB name
  condDBInfo += ":" + m_condDBUser; // MySQL user
  condDBInfo += ":" + m_condDBPswd; // MySQL password
  if ( m_showCondDBPswd ) {
    log << MSG::INFO << "CondDB MySQL init string is " 
	<< condDBInfo << endreq;
  }

  return StatusCode::SUCCESS;

#else

  log << MSG::ERROR 
      << "CondDB MySQL implementation has not been linked!" << endreq;
  condDBInfo = "";
  return StatusCode::FAILURE;

#endif

}

//----------------------------------------------------------------------------

/// Build the technology-specific init string to access the database
StatusCode 
ConditionsDBGate::i_buildCondDBInfo( std::string& condDBInfo )
{  

  MsgStream log(msgSvc(), "ConditionsDBGate" );
  log << MSG::VERBOSE 
      << "Building the technology-specific init string to the ConditionsDB" 
      << endreq;

  switch ( m_condDBImplCode ) {

  case CONDDBOBJY:
    return i_buildCondDBInfoObjy( condDBInfo );
    break;

  case CONDDBORACLE:
    return i_buildCondDBInfoOracle( condDBInfo );
    break;

  case CONDDBMYSQL:
    return i_buildCondDBInfoMySQL( condDBInfo );
    break;

  default:
    log << MSG::ERROR << "Unknown implementation technology: " 
        << m_condDBImpl << "(code " << m_condDBImplCode << ")" << endreq;
    return StatusCode::FAILURE;

  } 

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
const std::string & ConditionsDBGate::implementation() const{
  return m_condDBImpl;
}
short ConditionsDBGate::implementationCode() const {
  return m_condDBImplCode;
}

