// $Id: CondDBAccessSvc.cpp,v 1.2 2005-02-09 08:49:29 marcocle Exp $
// Include files 
#include "CondDBAccessSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"

#include <sstream>

#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IValidityKey.h"
#include "RelationalCool/RalDatabaseSvcFactory.h"
#include "RelationalCool/RelationalException.h"

#include <unistd.h>
// local

// Factory implementation
static SvcFactory<CondDBAccessSvc> s_factory;
const ISvcFactory &CondDBAccessSvcFactory = s_factory;

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBAccessSvc
//
// 2005-01-11 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBAccessSvc::CondDBAccessSvc(const std::string& name, ISvcLocator* svcloc):
  Service(name,svcloc)
{

  declareProperty("HostName",     m_dbHostName);
  declareProperty("User",         m_dbUser);
  declareProperty("Password",     m_dbPassword);
  declareProperty("HidePassword", m_hidePasswd);
  declareProperty("Database",     m_dbName);
  declareProperty("Schema",       m_dbSchema);
  declareProperty("DefaultTAG",   m_dbTAG);
  declareProperty("BackEnd",      m_dbBackEnd);
  declareProperty("RecreateDB",   m_recreateDB);
  declareProperty("RunTest",      m_test);

  // default for properties:
  m_hidePasswd = true;
  m_dbBackEnd  = "mysql";
  m_recreateDB = false;
  m_test       = false;
  
}
//=============================================================================
// Destructor
//=============================================================================
CondDBAccessSvc::~CondDBAccessSvc() {}; 

//=============================================================================
// queryInterface
//=============================================================================
StatusCode CondDBAccessSvc::queryInterface(const InterfaceID& riid,
                                           void** ppvUnknown){
  if ( IID_ICondDBAccessSvc.versionMatch(riid) )   {
    *ppvUnknown = (ICondDBAccessSvc*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface(riid,ppvUnknown);
}

//=============================================================================
// initialize
//=============================================================================
StatusCode CondDBAccessSvc::initialize(){
  StatusCode sc = Service::initialize();
  if (sc.isFailure()) return sc;

  MsgStream log(msgSvc(), "CondDBAccessSvc" );

  log << MSG::DEBUG << "Initialize" << endmsg;
  
  if ( m_dbHostName == "" ||
       m_dbUser == "" ||
       m_dbName == "" ||
       m_dbSchema == "" ||
       m_dbBackEnd == "" ){
    return StatusCode::FAILURE;
  }

  cool::DatabaseId uri = i_connection_uri();
  
  log << MSG::DEBUG << "Connection string = \"" ;
  if (! m_hidePasswd){
    log << uri;
  } else {
    log << m_dbBackEnd << "://" << m_dbHostName
        << ";schema="   << m_dbSchema
        << ";user="     << m_dbUser
        << ";password=" << "**hidden**"
        << ";dbname="   << m_dbName;
  }
  log << "\"" << endmsg;

  /*
  log << MSG::DEBUG << "Get DatabaseSvc" << endmsg;
  cool::IDatabaseSvc &dbSvc = cool::RalDatabaseSvcFactory::getDatabaseService();
  if ( ! &dbSvc ) {
    log << MSG::ERROR << "unable to get the Database Service" << endmsg;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG << "got service" << endmsg;

  if (m_recreateDB) {
    log << MSG::INFO << "Recreating Database" << endmsg;

    log << MSG::DEBUG << "drop the database " << uri << endmsg;
    dbSvc.dropDatabase(uri,false);
    log << MSG::DEBUG << "done" << endmsg;

    log << MSG::DEBUG << "create empty the database " << endmsg;
    cool::IDatabasePtr tmpPtr = dbSvc.createDatabase(uri);
    tmpPtr.reset(); // close the database (resetting the shared pointer)

    if (m_dbBackEnd == "oracle"){
      // ORA-01466: work-around
      log << MSG::INFO << "ORA-01466: work-around" << endmsg;
      sleep(1);
    }
    
  }
  */
  
  sc = i_openConnention();
  if (sc.isFailure()) return sc;

  if ( !m_test ) {
    return sc;
  } else { // do the test
    log << MSG::DEBUG << "Entering Test" << endmsg;

    pool::AttributeListSpecification BasicStringALSpec;
    BasicStringALSpec.push_back<int>("type");
    BasicStringALSpec.push_back<std::string>("str");

    std::string m_test_path = "/this/is/a/new/test/Folder";
    
    {
      log << MSG::DEBUG << "Create Folder \"" << m_test_path << "\"" <<endmsg;
      cool::IFolderPtr folder =
        m_db->createFolder(m_test_path,BasicStringALSpec,
                           "this is a test folder",
                           cool::FolderVersioning::ONLINE,
                           true);
      pool::AttributeList data(BasicStringALSpec);
      data["type"].setValue<int>(1);
      data["str"].setValue<std::string>(std::string("Here is the data for ")
                                        +m_test_path);
      data.print(std::cout);
      std::cout << std::endl;
    
    
      folder->storeObject(cool::IValidityKeyMin,cool::IValidityKeyMax,data);
      //folder->storeObject(cool::IValidityKeyMin,1000,data);
      //dispose of the used folder
      //folder.reset();
    }
    
    {
      log << MSG::DEBUG << "*** Second Test ***" << endmsg;

      pool::AttributeListSpecification attListSpec;
      attListSpec.push_back<long>("storage_type");
      attListSpec.push_back<std::string>("data");

      std::string rootName = "/CONDDB";

      m_db->createFolderSet( rootName+"/SlowControl",
                             "this is a test folderset", true );
      m_db->createFolderSet( rootName+"/SlowControl/LHCb",
                             "this is a test folderset", true ); 
      m_db->createFolderSet( rootName+"/SlowControl/Hcal",
                             "this is a test folderset", true ); 
      m_db->createFolderSet( rootName+"/Geometry",
                             "this is a test folderset", true ); 
      m_db->createFolderSet( rootName+"/Geometry2",
                             "this is a test folderset", true ); 
      m_db->createFolderSet( rootName+"/Alignment",
                             "this is a test folderset", true );
      m_db->createFolderSet( rootName+"/Alignment/Ecal",
                             "this is a test folderset", true );
      
      m_db->createFolder( rootName+"/pippo", attListSpec,
                          "this is a test folder",
                          cool::FolderVersioning::ONLINE, true );
      m_db->createFolder( rootName+"/scLHCb", attListSpec,
                          "this is a test folder",
                          cool::FolderVersioning::ONLINE, true );
      m_db->createFolder( rootName+"/SlowControl/LHCb/scLHCb", attListSpec,
                          "this is a test folder",
                          cool::FolderVersioning::ONLINE, true );
      m_db->createFolder( rootName+"/SlowControl/Hcal/scHcal", attListSpec,
                          "this is a test folder",
                          cool::FolderVersioning::ONLINE, true );
      m_db->createFolder( rootName+"/Geometry/LHCb", attListSpec,
                          "this is a test folder",
                          cool::FolderVersioning::ONLINE, true );
      m_db->createFolder( rootName+"/Geometry2/LHCb", attListSpec,
                          "this is a test folder",
                          cool::FolderVersioning::ONLINE, true );
      m_db->createFolder( rootName+"/Geometry2/lvLHCb", attListSpec,
                          "this is a test folder",
                          cool::FolderVersioning::ONLINE, true );
      m_db->createFolder( rootName+"/Alignment/Ecal/alEcal", attListSpec,
                          "this is a test folder",
                          cool::FolderVersioning::ONLINE, true );
    }
    

    try {
      cool::IFolderPtr folder =
        m_db->getFolder(m_test_path);
      //pool::AttributeList retieved_data(folder->getPayloadSpecification());
      cool::IObjectPtr object =
        folder->findObject(2000);
      object->payload().print(std::cout);
    } catch (cool::RelationalFolderNotFound &e) {
      log << MSG::ERROR << "Folder \"" << m_test_path << "\" not found!" << endmsg;
      log << MSG::ERROR << e << endmsg;
      return StatusCode::FAILURE;
    } catch (cool::RelationalObjectNotFound &e) {
      log << MSG::ERROR << "Object \"" << m_test_path << "\" not found!" << endmsg;
      log << MSG::ERROR << e << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// finalize
//=============================================================================
StatusCode CondDBAccessSvc::finalize(){
  MsgStream log(msgSvc(), "CondDBAccessSvc" );
  log << MSG::DEBUG << "Finalize" << endmsg;
  // release the database
  //cool::IDatabasePtr nullPtr;
  //m_db = nullPtr;
  m_db.reset();
  return Service::finalize();
}

//=============================================================================
// prepare the string used to connect to the DB
//=============================================================================
std::string CondDBAccessSvc::i_connection_uri() const {
  std::ostringstream result;
  
  result << m_dbBackEnd << "://" << m_dbHostName
         << ";schema="   << m_dbSchema
         << ";user="     << m_dbUser
         << ";password=" << m_dbPassword
         << ";dbname="   << m_dbName;

  return result.str();
}

//=============================================================================
// Connect to the database
//=============================================================================
StatusCode CondDBAccessSvc::i_openConnention(){
  MsgStream log(msgSvc(), "CondDBAccessSvc" );

  try {
    if (! m_db) { // The database is not yet opened
      log << MSG::DEBUG << "Get DatabaseSvc" << endmsg;
      cool::IDatabaseSvc &dbSvc = cool::RalDatabaseSvcFactory::getDatabaseService();
      if ( ! &dbSvc ) {
        log << MSG::ERROR << "unable to get the Database Service" << endmsg;
        return StatusCode::FAILURE;
      }
      log << MSG::DEBUG << "got service" << endmsg;
      
      if (m_recreateDB) { // Recreate the DB if requested
        log << MSG::INFO << "Recreating Database" << endmsg;
        
        std::string uri = i_connection_uri();
        log << MSG::DEBUG << "drop the database " << uri << endmsg;
        dbSvc.dropDatabase(uri,false);
        log << MSG::DEBUG << "done" << endmsg;
        
        log << MSG::DEBUG << "create empty the database " << endmsg;
        m_db = dbSvc.createDatabase(uri);
        
        if (m_dbBackEnd == "oracle"){
          // ORA-01466: work-around
          log << MSG::INFO << "ORA-01466: work-around" << endmsg;
          sleep(1);
        }
      } else { // if !recreate => just open
        log << MSG::DEBUG << "Opening connection" << endmsg;
        m_db = dbSvc.openDatabase(i_connection_uri());
      }
    } else {
      log << MSG::VERBOSE << "Database connection already established!" << endmsg;
    }
  }
  //  catch ( cool::RelationalDatabaseDoesNotExist &e ) {
  catch ( cool::RelationalException &e ) {
    log << MSG::ERROR << "Problems opening database" << endmsg;
    log << MSG::ERROR << e << endmsg;
    m_db.reset();
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// returns a shared pointer to a CondDBObject
//=============================================================================
/*
cool::IObjectPtr CondDBAccessSvc::getCondDBObject(const std::string &path,
                                                  const cool::IValidityKey &timePoint,
                                                  const cool::IChannelId &channelId){
  try {
    cool::IFolderPtr folder = database()->getFolder(path);
    cool::IObjectPtr obj(folder->findObject(timePoint,channelId));
    return obj;
  }
  catch (cool::RelationalFolderNotFound) {
    MsgStream log(msgSvc(), "CondDBAccessSvc" );
    log << MSG::ERROR << "Folder \"" << path << "\" not found!" << endmsg;
    cool::IObjectPtr obj;
    return obj;
  }
}
*/


//=============================================================================
