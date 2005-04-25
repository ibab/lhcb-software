// $Id: CondDBAccessSvc.cpp,v 1.4 2005-04-25 10:38:35 marcocle Exp $
// Include files 
#include <sstream>
#include <unistd.h>

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/TimePoint.h"

#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"

#include "RelationalCool/RalDatabaseSvcFactory.h"
#include "RelationalCool/RelationalException.h"

#include "AttributeList/AttributeList.h"

// local
#include "CondDBAccessSvc.h"

// Factory implementation
static SvcFactory<CondDBAccessSvc> s_factory;
const ISvcFactory &CondDBAccessSvcFactory = s_factory;

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBAccessSvc
//
// 2005-01-11 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

// ==== Static data members
pool::AttributeListSpecification *CondDBAccessSvc::s_XMLstorageAttListSpec = NULL;
unsigned long long CondDBAccessSvc::s_instances = 0;

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
  declareProperty("TAG",          m_dbTAG);
  declareProperty("BackEnd",      m_dbBackEnd);
  declareProperty("RecreateDB",   m_recreateDB);
  declareProperty("RunTest",      m_test);

  // default for properties:
  m_hidePasswd = true;
  m_dbBackEnd  = "mysql";
  m_recreateDB = false;
  m_test       = false;
  m_dbTAG      = "";
  
  if (s_XMLstorageAttListSpec == NULL){
    // attribute list spec template
    s_XMLstorageAttListSpec = new pool::AttributeListSpecification();
    s_XMLstorageAttListSpec->push_back<std::string>("data");
  }
  ++s_instances;
}
//=============================================================================
// Destructor
//=============================================================================
CondDBAccessSvc::~CondDBAccessSvc() {
  // check how many instances are still around.
  // if it is the last one, delete the attribute list (if still there).
  if (--s_instances == 0 && s_XMLstorageAttListSpec != NULL) {
    delete s_XMLstorageAttListSpec;
    s_XMLstorageAttListSpec = NULL;
  }
}

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

  MsgStream log(msgSvc(), name() );

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

  sc = i_openConnention();
  if (sc.isFailure()) return sc;

  /* TODO: this piece of code must be reintroduced when HVS is functional
     sc = i_checkTag();
     if (sc.isFailure()){
     log << MSG::ERROR << "Bad TAG given: \"" << tag() << "\" not in the database" << endmsg;
     return sc;
     }
  */
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
                           cool::FolderVersioning::SINGLE_VERSION,
                           true);
      pool::AttributeList data(BasicStringALSpec);
      data["type"].setValue<int>(1);
      data["str"].setValue<std::string>(std::string("Here is the data for ")
                                        +m_test_path);
      data.print(std::cout);
      std::cout << std::endl;
    
    
      folder->storeObject(cool::ValidityKeyMin,cool::ValidityKeyMax,data);
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
                          cool::FolderVersioning::SINGLE_VERSION, true );
      m_db->createFolder( rootName+"/scLHCb", attListSpec,
                          "this is a test folder",
                          cool::FolderVersioning::SINGLE_VERSION, true );
      m_db->createFolder( rootName+"/SlowControl/LHCb/scLHCb", attListSpec,
                          "this is a test folder",
                          cool::FolderVersioning::SINGLE_VERSION, true );
      m_db->createFolder( rootName+"/SlowControl/Hcal/scHcal", attListSpec,
                          "this is a test folder",
                          cool::FolderVersioning::SINGLE_VERSION, true );
      m_db->createFolder( rootName+"/Geometry/LHCb", attListSpec,
                          "this is a test folder",
                          cool::FolderVersioning::SINGLE_VERSION, true );
      m_db->createFolder( rootName+"/Geometry2/LHCb", attListSpec,
                          "this is a test folder",
                          cool::FolderVersioning::SINGLE_VERSION, true );
      m_db->createFolder( rootName+"/Geometry2/lvLHCb", attListSpec,
                          "this is a test folder",
                          cool::FolderVersioning::SINGLE_VERSION, true );
      m_db->createFolder( rootName+"/Alignment/Ecal/alEcal", attListSpec,
                          "this is a test folder",
                          cool::FolderVersioning::SINGLE_VERSION, true );
    }
    

    try {
      cool::IFolderPtr folder = m_db->getFolder(m_test_path);
      cool::IObjectPtr object = folder->findObject(2000);
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
  MsgStream log(msgSvc(), name() );
  log << MSG::DEBUG << "Finalize" << endmsg;
  // release the database
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
  MsgStream log(msgSvc(), name() );

  try {
    if (! m_db) { // The database is not yet opened
      log << MSG::DEBUG << "Get DatabaseSvc" << endmsg;
      cool::IDatabaseSvc &dbSvc = cool::RalDatabaseSvcFactory::databaseService();
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
// TAG handling
//=============================================================================
const std::string &CondDBAccessSvc::tag() const{ return m_dbTAG; }
StatusCode CondDBAccessSvc::setTag(const std::string &_tag){
  StatusCode sc = i_checkTag(_tag);
  if ( sc.isSuccess() ) {
    m_dbTAG = _tag;
  } else {
    MsgStream log(msgSvc(), name() );
    log << MSG::WARNING << "Unable to set TAG \"" << _tag
        << "\": not in the DB. (Still using \"" << tag() << "\")" << endmsg;
  }
  return sc;
}
StatusCode CondDBAccessSvc::i_checkTag(const std::string &tag) const {
  if (m_db) {
    if ( (tag == "") || (tag == "HEAD") || m_db->existsTag(tag) ) return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}


//=============================================================================
// Utilities
//=============================================================================

StatusCode CondDBAccessSvc::createFolder(const std::string &path,
                                         const std::string &descr,
                                         StorageType storage,
                                         VersionMode vers) const {
  if ( !m_db ) {
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to create the folder \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }
  
  try {
    switch (storage) {
    case FOLDERSET:
      m_db->createFolderSet(path,descr,true);
      break;
    case XML:
      {
        // append to the description the storage type
        std::ostringstream _descr;
        _descr << descr << " <storage_type=" << std::dec << XML_StorageType << ">";
        m_db->createFolder(path,
                           *s_XMLstorageAttListSpec,
                           _descr.str(),
                           (vers == SINGLE)
                           ?cool::FolderVersioning::SINGLE_VERSION
                           :cool::FolderVersioning::MULTI_VERSION,
                           true);
      }
      break;
    default:
      MsgStream log(msgSvc(), name() );
      log << MSG::ERROR << "Unable to create the folder \"" << path
          << "\": unknown StorageType" << endmsg;
      return StatusCode::FAILURE;
    }
  } catch(cool::RelationalNodeExists){
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to create the folder \"" << path
        << "\": the node already exists" << endmsg;
    return StatusCode::FAILURE;
  } catch(cool::Exception &e){
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to create the folder \"" << path
        << "\" (cool::Exception): " << e << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode CondDBAccessSvc::storeXMLString(const std::string &path, const std::string &data,
                                           const ITime &since, const ITime &till) const {
  try {
    // retrieve folder pointer
    cool::IFolderPtr folder = m_db->getFolder(path);
    if (!folder->isLeaf()) {
      MsgStream log(msgSvc(), name() );
      log << MSG::ERROR << "Trying to store data into the non-leaf folder \"" <<
        path << '\"' << endmsg;
      return StatusCode::FAILURE;
    }
    pool::AttributeList payload(*s_XMLstorageAttListSpec);
    payload["data"].setValue<std::string>(data);
    folder->storeObject(timeToValKey(since),timeToValKey(till),payload);
  } catch (cool::Exception &e){
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to store the XML string into \"" << path
        << "\" (cool::Exception): " << e << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

cool::ValidityKey CondDBAccessSvc::timeToValKey(const TimePoint &time) const {
  // TODO: ValidityKey is an uInt64 of which only 63 bits used (0 -> 9223372036854775807)

  // ValidityKey is uInt64 while time.absoluteTime() is a signed Int64!
  // I cannot compare them directly (
  if (time.absoluteTime() < 0) return cool::ValidityKeyMin;
  if (time.absoluteTime() == time_absolutefuture.absoluteTime()) return cool::ValidityKeyMax; 
  return time.absoluteTime();
}

TimePoint CondDBAccessSvc::valKeyToTime(const cool::ValidityKey &key) const {
  TimePoint t(key);
  return t;
}

StatusCode CondDBAccessSvc::tagFolder(const std::string &path, const std::string &tagName,
                                      const std::string &description){
  try {
    MsgStream log(msgSvc(),name());
    log << MSG::DEBUG << "entering tagFolder: \"" << path << '"' << endmsg;
    // retrieve folder pointer
    StatusCode sc;
    cool::IFolderPtr folder = m_db->getFolder(path);
    if (!folder->isLeaf()) { // FolderSet
      log << MSG::DEBUG << "it is a folderset" << endmsg;
      std::string sub_path(path);
      if ( sub_path != "/" ) { // root folderset does not need an extra "/"
      	sub_path += "/";
      }
      // discover child folders
      std::vector<std::string> fldr_names = m_db->listFolders();
      std::vector<std::string>::iterator i;
      for ( i = fldr_names.begin(); i != fldr_names.end(); ++i ){
        if ( *i != sub_path // avoid infinite recursion on folderset "/". TODO: refine
             && i->find(sub_path) == 0 // (*i) starts with path
             && i->find('/',sub_path.size()+1) == i->npos) // and does not contain any other '/'
          sc = tagFolder(*i,tagName,description); // recursion!
      }
    } else {
      if (folder->versioningMode() == cool::FolderVersioning::SINGLE_VERSION){
        log << MSG::WARNING << "not tagging folder \"" << path << "\": single-version" << endmsg;
      } else {
        log << MSG::DEBUG << "tagging folder \"" << path << "\": " << tagName << endmsg;
        folder->tag(folder->fullPath() + "-" + tagName,description);
      }
    }
  } catch (cool::RelationalFolderNotFound &e) {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Folder \"" << path << "\" not found!" << endmsg;
    log << e << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
