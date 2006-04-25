// $Id: CondDBAccessSvc.cpp,v 1.18 2006-04-25 17:20:20 marcocle Exp $
// Include files 
#include <sstream>

// needed to sleep (ORA-01466 work-around)
#include "SealBase/TimeInfo.h"

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Time.h"

#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IFolderSet.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/ExtendedAttributeListSpecification.h"
#include "CoolKernel/PredefinedStorageHints.h"

#include "CoolApplication/DatabaseSvcFactory.h"

#include "CoralBase/AttributeList.h"

// local
#include "CondDBAccessSvc.h"
#include "CondDBCache.h"

// Factory implementation
static SvcFactory<CondDBAccessSvc> s_factory;
const ISvcFactory &CondDBAccessSvcFactory = s_factory;

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBAccessSvc
//
// 2005-01-11 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

// ==== Static data members
cool::ExtendedAttributeListSpecification *CondDBAccessSvc::s_XMLstorageAttListSpec = NULL;
unsigned long long CondDBAccessSvc::s_instances = 0;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBAccessSvc::CondDBAccessSvc(const std::string& name, ISvcLocator* svcloc):
  Service(name,svcloc)
{

  declareProperty("ConnectionString", m_connectionString);
  declareProperty("HostName",         m_dbHostName);
  declareProperty("User",             m_dbUser);
  declareProperty("Password",         m_dbPassword);
  declareProperty("HidePassword",     m_hidePasswd);
  declareProperty("Database",         m_dbName);
  declareProperty("Schema",           m_dbSchema);
  declareProperty("TAG",              m_dbTAG);
  declareProperty("BackEnd",          m_dbBackEnd);
  declareProperty("RecreateDB",       m_recreateDB);
  declareProperty("RunTest",          m_test);
  declareProperty("UseCache",         m_useCache);
  declareProperty("CacheLowLevel",    m_cacheLL=10);
  declareProperty("CacheHighLevel",   m_cacheHL=100);
  //declareProperty("CachePreload",     m_cachePreload=3600*1E9); // ns
  declareProperty("NoDB",             m_noDB);

  // default for properties:
  m_connectionString = "";
  m_dbHostName = "";
  m_dbUser     = "";
  m_dbPassword = "";
  m_hidePasswd = true;
  m_dbName     = "";
  m_dbSchema   = "";
  m_dbTAG      = "";
  m_dbBackEnd  = "mysql";
  m_recreateDB = false;
  m_test       = false;
  m_useCache   = false;
  m_noDB       = false;
  
  if (s_XMLstorageAttListSpec == NULL){
    // attribute list spec template
    s_XMLstorageAttListSpec = new cool::ExtendedAttributeListSpecification();
    s_XMLstorageAttListSpec->push_back("data", "string", cool::PredefinedStorageHints::STRING_MAXSIZE_16M);
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
  
  if ( m_noDB && !m_useCache ) {
    log << MSG::ERROR << "Database access disabled and cache off: I cannot work like that. Ciao!" << endmsg;
    return StatusCode::FAILURE;
  }
  
  if ( !m_noDB ) {
    // user, name and password can be specified via authentication.xml
    if (m_connectionString == ""){
      if (m_dbHostName == "" ||
	  // m_dbUser == "" ||
	  m_dbName == "" ||
	  m_dbSchema == "" ||
	  m_dbBackEnd == "" ){
	log << MSG::ERROR << "An information needed to connect to the CondDB is missing." << endmsg;
	log << MSG::ERROR << "Check that the option 'ConnectionString' is set." << endmsg;
	return StatusCode::FAILURE;
      }
      else{
	log << MSG::WARNING << "Properties 'HostName', 'Schema', 'Database' and 'BackEnd' are deprecated." << endmsg;
	log << MSG::WARNING << "Please use 'ConnectionString' instead." << endmsg;
	m_connectionString = m_dbBackEnd + 
	                     "://"        + m_dbHostName + 
	                     ";schema="   + m_dbSchema + 
                             ";user="     + m_dbUser + 
                             ";password=" + m_dbPassword + 
	                     ";dbname="   + m_dbName;
      }
    }
    log << MSG::DEBUG << "Connection string = \"" ;
    log << m_connectionString << "\"" << endmsg;

    sc = i_openConnection();
    if (!sc.isSuccess()) return sc;
  } 
  else {
    log << MSG::INFO << "Database not requested: I'm not trying to connect" << endmsg;
  }
  
  // set up cache if needed
  if (m_useCache) {
    log << MSG::DEBUG << "Initialize CondDB cache." << endmsg;
    m_cache = new CondDBCache(MsgStream(msgSvc(), name() + ".Cache"),m_cacheHL,m_cacheLL);
    if (m_cache == NULL) {
      log << MSG::ERROR << "Unable to initialize CondDB cache." << endmsg;
      return StatusCode::FAILURE;
    }
  } else {
    log << MSG::DEBUG << "CondDB cache not needed" << endmsg;
    m_cache = NULL;
  }

  /* TODO: this piece of code must be reintroduced when HVS is functional
     sc = i_checkTag();
     if (sc.isFailure()){
     log << MSG::ERROR << "Bad TAG given: \"" << tag() << "\" not in the database" << endmsg;
     return sc;
     }
  */
  if ( !m_test ) {
    return sc;
  } else  if ( !m_noDB ) { // do the test
    log << MSG::DEBUG << "Entering Test" << endmsg;

    cool::ExtendedAttributeListSpecification BasicStringALSpec;
    BasicStringALSpec.push_back("type", "int", cool::PredefinedStorageHints::STRING_MAXSIZE_16M);
    BasicStringALSpec.push_back("str", "string", cool::PredefinedStorageHints::STRING_MAXSIZE_16M);

    std::string m_test_path = "/this/is/a/new/test/Folder";
    
    {
      log << MSG::DEBUG << "Create Folder \"" << m_test_path << "\"" <<endmsg;
      cool::IFolderPtr folder =
        m_db->createFolder(m_test_path,BasicStringALSpec,
				   "this is a test folder",
				   cool::FolderVersioning::SINGLE_VERSION,
				   true);
      coral::AttributeList data;
      data.extend("type","int");
      data.extend("str","string");
      
      data["type"].data<int>() = 1;
      data["str"].data<std::string>() = std::string("Here is the data for ")+m_test_path;
      data.toOutputStream(std::cout);
      std::cout << std::endl;
    
      folder->storeObject(cool::ValidityKeyMin,cool::ValidityKeyMax,data);
    }
    
    {
      log << MSG::DEBUG << "*** Second Test ***" << endmsg;

      cool::ExtendedAttributeListSpecification attListSpec;
      attListSpec.push_back("storage_type", "long", cool::PredefinedStorageHints::STRING_MAXSIZE_16M);
      attListSpec.push_back("data", "string", cool::PredefinedStorageHints::STRING_MAXSIZE_16M);

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
      object->payload().toOutputStream(std::cout);
    } catch (cool::FolderNotFound &e) {
      log << MSG::ERROR << "Folder \"" << m_test_path << "\" not found!" << endmsg;
      log << MSG::ERROR << e.what() << endmsg;
      return StatusCode::FAILURE;
    } catch (cool::ObjectNotFound &e) {
      log << MSG::ERROR << "Object \"" << m_test_path << "\" not found!" << endmsg;
      log << MSG::ERROR << e.what() << endmsg;
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
  if (m_useCache) {
    // dump the content of the cache
    m_cache->dump();
    // dispose of the cache manager
    delete m_cache;
  }
  
  return Service::finalize();
}

//=============================================================================
// Connect to the database
//=============================================================================
StatusCode CondDBAccessSvc::i_openConnection(){
  MsgStream log(msgSvc(), name() );

  try {
    if (! m_db) { // The database is not yet opened
      log << MSG::DEBUG << "Get DatabaseSvc" << endmsg;
      cool::IDatabaseSvc &dbSvc = cool::DatabaseSvcFactory::databaseService();
      if ( ! &dbSvc ) {
        log << MSG::ERROR << "unable to get the Database Service" << endmsg;
        return StatusCode::FAILURE;
      }
      log << MSG::DEBUG << "got service" << endmsg;
      
      if (m_recreateDB) { // Recreate the DB if requested
        log << MSG::INFO << "Recreating Database" << endmsg;
        
        log << MSG::DEBUG << "drop the database \"";
        log << m_connectionString << "\"" << endmsg;
        dbSvc.dropDatabase(m_connectionString, false);
        log << MSG::DEBUG << "done" << endmsg;
        
        log << MSG::DEBUG << "create empty the database " << endmsg;
        m_db = dbSvc.createDatabase(m_connectionString);
        
        if (m_dbBackEnd == "oracle"){
          // ORA-01466: work-around
          log << MSG::INFO << "ORA-01466: work-around" << endmsg;
          seal::TimeInfo::sleep(1);
        }
      } else { // if !recreate => just open
        log << MSG::DEBUG << "Opening connection" << endmsg;
	m_db = dbSvc.openDatabase(m_connectionString);
      }
    } else {
      log << MSG::VERBOSE << "Database connection already established!" << endmsg;
    }
  }
  //  catch ( cool::DatabaseDoesNotExist &e ) {
  catch ( cool::Exception &e ) {
    log << MSG::ERROR << "Problems opening database" << endmsg;
    log << MSG::ERROR << e.what() << endmsg;
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

  if (m_dbTAG == _tag) return StatusCode::SUCCESS; // no need to change

  StatusCode sc = i_checkTag(_tag);
  if ( sc.isSuccess() ) {
    m_dbTAG = _tag;
    if (m_useCache) {
      // the cache must be cleared if the tag is changed
      m_cache->clear();
    }
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
  } catch(cool::NodeExists){
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to create the folder \"" << path
        << "\": the node already exists" << endmsg;
    return StatusCode::FAILURE;
  } catch(cool::Exception &e){
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to create the folder \"" << path
        << "\" (cool::Exception): " << e.what() << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode CondDBAccessSvc::storeXMLString(const std::string &path, const std::string &data,
                                           const Gaudi::Time &since, const Gaudi::Time &until, cool::ChannelId channel) const {
  if ( !m_db ) {
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to store the object \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }
  
  try {
    // retrieve folder pointer
    cool::IFolderPtr folder = m_db->getFolder(path);
    if (!folder->isLeaf()) {
      MsgStream log(msgSvc(), name() );
      log << MSG::ERROR << "Trying to store data into the non-leaf folder \"" <<
        path << '\"' << endmsg;
      return StatusCode::FAILURE;
    }
    coral::AttributeList payload;
    payload.extend("data","string");
    payload["data"].data<std::string>() = data;
    folder->storeObject(timeToValKey(since),timeToValKey(until),payload,channel);
  } catch (cool::Exception &e){
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to store the XML string into \"" << path
        << "\" (cool::Exception): " << e.what() << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
StatusCode CondDBAccessSvc::storeXMLString(const std::string &path, const std::string &data,
                                           const double since_s, const double until_s, cool::ChannelId channel) const {
  return storeXMLString(path,data,
                        Gaudi::Time((long long int)(since_s * 1e9)),Gaudi::Time((long long int)(until_s * 1e9)),
                        channel);
}

cool::ValidityKey CondDBAccessSvc::timeToValKey(const Gaudi::Time &time) const {
  // ValidityKey is an uInt64 of which only 63 bits used (0 -> 9223372036854775807),
  // while time.nd() is a positive signed Int64! (the same thing)
  return time.ns();
}

Gaudi::Time CondDBAccessSvc::valKeyToTime(const cool::ValidityKey &key) const {
  return Gaudi::Time(key);
}

StatusCode CondDBAccessSvc::tagFolder(const std::string &path, const std::string &tagName,
                                      const std::string &description, cool::ChannelId /* channel */){
  MsgStream log(msgSvc(),name());

  if ( !m_db ) {
    log << MSG::ERROR << "Unable to tag the folder \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }

  try {
    log << MSG::DEBUG << "entering tagFolder: \"" << path << '"' << endmsg;
    // retrieve folder pointer
    StatusCode sc;
    //
    if (m_db->existsFolderSet(path)) { // FolderSet
      cool::IFolderSetPtr folderSet = m_db->getFolderSet(path);
      log << MSG::DEBUG << "it is a folderset" << endmsg;
      // discover child folders
      std::vector<std::string> folderSet_names = folderSet->listFolderSets();
      std::vector<std::string> folder_names    = folderSet->listFolders();
      std::vector<std::string>::iterator i;
      for ( i = folderSet_names.begin(); i != folderSet_names.end(); ++i ){
        sc = tagFolder(*i, tagName, description); // recursion!
      }
      for ( i = folder_names.begin(); i != folder_names.end(); ++i ){
        sc = tagFolder(*i, tagName, description); // recursion again. Might be merged with above...
      }
    } else {
      if (m_db->existsFolder(path)) {
        cool::IFolderPtr folder = m_db->getFolder(path);
        log << MSG::DEBUG << "it is a folder" << endmsg;
        if (folder->versioningMode() == cool::FolderVersioning::SINGLE_VERSION){
          log << MSG::WARNING << "Not tagging folder \"" << path << "\": single-version" << endmsg;
        } else {
          log << MSG::DEBUG << "Tagging folder \"" << path << "\": " << tagName << endmsg;
          folder->tagCurrentHead(folder->fullPath() + "-" + tagName, description);
        }
      } else {
        MsgStream log(msgSvc(),name());
        log << MSG::ERROR << "Folder \"" << path << "\" not found!" << endmsg;
        return StatusCode::FAILURE;
      }
    }
  } catch (cool::FolderNotFound &e) {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Folder \"" << path << "\" not found!" << endmsg;
    log << e.what() << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode CondDBAccessSvc::getObject(const std::string &path, const Gaudi::Time &when,
                                      boost::shared_ptr<coral::AttributeList> &data,
                                      std::string &descr, Gaudi::Time &since, Gaudi::Time &until, cool::ChannelId channel){

  try {
    if (m_useCache) {
      cool::ValidityKey vk_when = timeToValKey(when);
      cool::ValidityKey vk_since, vk_until;
      if (!m_cache->get(path,vk_when,channel,vk_since,vk_until,descr,data)) {
        // not found
        if (!m_noDB) {
          if (database()->existsFolderSet(path)) {
            // with FolderSets, I put an empty entry and clear the shared_ptr
            m_cache->addFolderSet(path,"");
            data.reset();
            return StatusCode::SUCCESS;
          }
          // go to the database
          cool::IFolderPtr folder = database()->getFolder(path);
          cool::IObjectPtr obj;
          if (tag() == "HEAD" || tag() == ""){
            obj = folder->findObject(vk_when,channel);
          } else {
            obj = folder->findObject(vk_when,channel,folder->fullPath()+"-"+tag());
          }
          m_cache->insert(folder,obj,channel);
          // now the object is in the cache
          m_cache->get(path,vk_when,channel,vk_since,vk_until,descr,data);
        } else {
          // we are not using the db: no way of getting the object from it
          return StatusCode::FAILURE;
        }
      }
      since = valKeyToTime(vk_since);
      until = valKeyToTime(vk_until);
    } else if (!m_noDB){
      
      if (database()->existsFolderSet(path)) {
        // with FolderSets, I clear the shared_ptr (it's the folderset signature)
        data.reset();
        return StatusCode::SUCCESS;
      }

      cool::IFolderPtr folder = database()->getFolder(path);
      descr = folder->description();

      cool::IObjectPtr obj;
      if (tag() == "HEAD" || tag() == ""){
        obj = folder->findObject(timeToValKey(when),channel);
      } else {
        obj = folder->findObject(timeToValKey(when),channel,folder->fullPath()+"-"+tag());
      }
    
      // deep copy of the attr. list
      data = boost::shared_ptr<coral::AttributeList>(new coral::AttributeList(obj->payload()));
   
      since = valKeyToTime(obj->since());
      until = valKeyToTime(obj->until());
      
    } else {
      //log << MSG::ERROR << "Object not found in cache and database is off" << endmsg;
      return StatusCode::FAILURE;
    }
    
  } catch ( cool::FolderNotFound /*&e*/) {
    //log << MSG::ERROR << e << endmsg;
    return StatusCode::FAILURE;
  } catch (cool::ObjectNotFound /*&e*/) {
    //log << MSG::ERROR << "Object not found in \"" << path <<
    //  "\" for tag \"" << (*accSvc)->tag() << "\" ("<< now << ')' << endmsg;
    //log << MSG::DEBUG << e << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
//  
//=========================================================================
StatusCode CondDBAccessSvc::getChildNodes (const std::string &path, std::vector<std::string> &node_names) {

  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "Entering \"getChildNodes\"" << endmsg;

  node_names.clear();

  try {
    
    if (!m_noDB) { // If I have the DB I always use it!
      if (database()->existsFolderSet(path)) {
        log << MSG::DEBUG << "FolderSet \"" << path  << "\" exists" << endmsg;
        
        cool::IFolderSetPtr folderSet = database()->getFolderSet(path);

        std::vector<std::string> fldr_names = folderSet->listFolders();
        std::vector<std::string> fldrset_names = folderSet->listFolderSets();

        for ( std::vector<std::string>::iterator f = fldr_names.begin(); f != fldr_names.end(); ++f ) {
          log << MSG::DEBUG << *f << endmsg;
          node_names.push_back(f->substr(f->rfind('/')));
        }
        for ( std::vector<std::string>::iterator f = fldrset_names.begin(); f != fldrset_names.end(); ++f ) {
          log << MSG::DEBUG << *f << endmsg;
          node_names.push_back(f->substr(f->rfind('/')));
        }
        
        log << MSG::DEBUG << "got " << node_names.size() << " sub folders" << endmsg;
      } else {
        // cannot get the sub-nodes of a folder!
        return StatusCode::FAILURE;
      }
    } else if (m_useCache) {
      // if no db, but cache, let's assume we know everything is in there
      m_cache->getSubNodes(path,node_names);
    } else {
      // no cache and no db
      return StatusCode::FAILURE;
    }
  } catch ( cool::FolderNotFound /*&e*/) {
    //log << MSG::ERROR << e << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;

}
//=========================================================================
//  
//=========================================================================
StatusCode CondDBAccessSvc::cacheAddFolder(const std::string &path, const std::string &descr,
                                           const cool::ExtendedAttributeListSpecification& spec) {
  if (!m_useCache) {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Cache not in use: I cannot add a folder to it." << endmsg;
    return StatusCode::FAILURE;
  }
  return m_cache->addFolder(path,descr,spec) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//=========================================================================
//  
//=========================================================================
StatusCode CondDBAccessSvc::cacheAddFolderSet(const std::string &path, const std::string &descr) {
  if (!m_useCache) {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Cache not in use: I cannot add a folder-set to it." << endmsg;
    return StatusCode::FAILURE;
  }
  return m_cache->addFolderSet(path,descr) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//=========================================================================
//  
//=========================================================================
StatusCode CondDBAccessSvc::cacheAddXMLFolder(const std::string &path) {
  std::ostringstream _descr;
  _descr << " <storage_type=" << std::dec << XML_StorageType << ">";
  return cacheAddFolder(path,_descr.str(),*s_XMLstorageAttListSpec);
}

//=========================================================================
//  
//=========================================================================
StatusCode CondDBAccessSvc::cacheAddObject(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                           const coral::AttributeList& payload, cool::ChannelId channel) {
  if (!m_useCache) {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Cache not in use: I cannot add an object to it." << endmsg;
    return StatusCode::FAILURE;
  }
  return m_cache->addObject(path,timeToValKey(since),timeToValKey(until),payload,channel)
    ? StatusCode::SUCCESS
    : StatusCode::FAILURE;
}

//=========================================================================
//  
//=========================================================================
StatusCode CondDBAccessSvc::cacheAddXMLObject(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                              const std::string &data, cool::ChannelId channel) {
  /// @todo this is affected by the evolution in COOL API
  coral::AttributeList payload;
  payload.extend("data","string");
  payload["data"].data<std::string>() = data;
  return cacheAddObject(path,since,until,payload,channel);
}

//=========================================================================
//  
//=========================================================================
void CondDBAccessSvc::dumpCache() const {
  if (m_useCache) m_cache->dump();
}
