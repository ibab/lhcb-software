// $Id: CondDBAccessSvc.cpp,v 1.22 2006-07-07 16:40:49 marcocle Exp $
// Include files
#include <sstream>
#include <cstdlib>
#include <ctime>

#include <pcre.h>

// needed to sleep between retrials
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

  declareProperty("ConnectionString", m_connectionString = ""    );
  declareProperty("DefaultTAG",       m_dbTAG            = ""    );
  declareProperty("NoDB",             m_noDB             = false );
  declareProperty("UseCache",         m_useCache         = true  );
  declareProperty("CacheLowLevel",    m_cacheLL          = 10    );
  declareProperty("CacheHighLevel",   m_cacheHL          = 100   );
  //declareProperty("CachePreload",     m_cachePreload=3600*1E9); // ns
  declareProperty("CheckTAGTrials",   m_checkTagTrials   = 1     );
  declareProperty("CheckTAGTimeOut",  m_checkTagTimeOut  = 60    );

  declareProperty("AlternativeDBs",   m_alternatives             );

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

    // Default instance -------------
    log << MSG::DEBUG << "Initialize default COOL instance" << endmsg;
    if ( m_connectionString == "" ) {
      // we need a connection string to connect to the DB
      log << MSG::ERROR << "Connection to database requested and no connection string provided." << endmsg;
      log << MSG::ERROR << "Set the option \"" << name() << ".ConnectionString\"." << endmsg;
      return StatusCode::FAILURE;
    }
    log << MSG::DEBUG << "Connect to \""  << m_connectionString << "\"" << endmsg;

    sc = i_connect(m_connectionString,m_mainDB);
    if ( !sc.isSuccess() ){
      return sc;
    }
      
    m_mainDB.tag = m_dbTAG;
    if ( !i_checkTagLoop(m_mainDB) ) return StatusCode::FAILURE;

    // alternative instances -------------
    log << MSG::DEBUG << "Initialize alternative COOL instances" << endmsg;
    // prepare the regular expression
    pcre *re;
    const char *errormsg;
    int erroffset;
    // the result of the matched pattern contains:
    // $1 = the path
    // $2 = [tag]
    // $3 = tag
    // $4 = connection string
    re = pcre_compile("^(/[^=]*)=(\\[([^]]*)\\])?(.+)$", // regular expression
                      0,                                 // default options
                      &errormsg,                         // for error message
                      &erroffset,                        // for error offset
                      NULL);                             // use default character tables
    if ( ! re ) {
      log << MSG::ERROR  << "Cannot compile regular expression: " << errormsg << " @ " << erroffset << endmsg;
      return StatusCode::FAILURE;
    }

    // prepare result set
    int rc;
    int ovector[30];

    std::vector<std::string>::iterator alt;
    for ( alt = m_alternatives.begin() ; alt != m_alternatives.end() ; ++alt ){
      rc = pcre_exec(re,             // result of pcre_compile()
                     NULL,           // we didn't study the pattern
                     alt->c_str(),   // the subject string
                     alt->size(),    // the length of the subject string
                     0,              // start at offset 0 in the subject
                     0,              // default options
                     ovector,        // vector for substring information
                     30);            // number of elements in the vector
      if ( rc < 0 ) {
        if ( rc == PCRE_ERROR_NOMATCH ) {
          log << MSG::ERROR  << "The string '" << *alt << "' does not define a database alternative" << endmsg;
        }
        else {
          log << MSG::ERROR  << "Unhandled PCRE error: " << rc << endmsg;
        }
        pcre_free(re);
        return StatusCode::FAILURE;
      }
      std::string path(*alt,ovector[0],ovector[1]-ovector[0]);
      std::string conn(*alt,ovector[8],ovector[9]-ovector[8]);

      log << MSG::DEBUG << "Connect to \""  << conn << "\" for path \"" << path << "\"" << endmsg;

      sc = i_connect(conn,m_altDBs[path]);
      if ( !sc.isSuccess() ){
        return sc;
      }

      if ( ovector[6] > 0 ) { // the tag part has been matched
        m_altDBs[path].tag = alt->substr(ovector[6],ovector[7]-ovector[6]);
      }
      if (!i_checkTagLoop(m_altDBs[path])) {
        pcre_free(re);
        return StatusCode::FAILURE;
      }
    }
    // I do not need anymore the RegExp pattern.
    pcre_free(re);
    
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

  return sc;
}

//=============================================================================
// finalize
//=============================================================================
StatusCode CondDBAccessSvc::finalize(){
  MsgStream log(msgSvc(), name() );
  log << MSG::DEBUG << "Finalize" << endmsg;

  // release the main database
  m_mainDB.root.reset();
  m_mainDB.db.reset();

  // release alternatives
  std::map<std::string,DBInstance>::iterator alt;
  for ( alt = m_altDBs.begin(); alt != m_altDBs.end(); ++alt ) {
    alt->second.root.reset();
    alt->second.db.reset();
  }

  if (m_useCache) {
    // dump the content of the cache
    m_cache->dump();
    // dispose of the cache manager
    delete m_cache;
  }

  return Service::finalize();
}

//=============================================================================
// 
//=============================================================================
bool CondDBAccessSvc::i_checkTagLoop(const DBInstance &dbi) const
{
  MsgStream log(msgSvc(), name() );

  // Check the existence of the provided tag.
  bool is_ok = i_checkTag(dbi).isSuccess();
  
  // Try again if requested
  int trials_to_go = m_checkTagTrials - 1; // take into account the trial just done
  while (!is_ok && (trials_to_go > 0)){
    log << MSG::INFO << "TAG \"" << tag() << "\" not ready in " << dbi.db->databaseId()
        << ", I try again in " << m_checkTagTimeOut << "s. "
        << trials_to_go << " trials left." << endmsg;
    seal::TimeInfo::sleep(m_checkTagTimeOut);
    is_ok = i_checkTag(dbi).isSuccess();
    --trials_to_go;
  }

  if (!is_ok) {
    log << MSG::ERROR << "Bad TAG given: \"" << tag() << "\" not in the database " << dbi.db->databaseId() << endmsg;
  }
  
  return is_ok;
}

//=============================================================================
// Connect to the database
//=============================================================================
StatusCode CondDBAccessSvc::i_connect(const std::string &connStr, DBInstance &dbi){
  MsgStream log( msgSvc(), name() );

  try {
    if (! dbi.db) { // The database is not yet opened
      log << MSG::DEBUG << "Get cool::DatabaseSvc" << endmsg;
      cool::IDatabaseSvc &dbSvc = cool::DatabaseSvcFactory::databaseService();
      log << MSG::DEBUG << "cool::DatabaseSvc got" << endmsg;

      log << MSG::DEBUG << "Opening connection" << endmsg;
      dbi.db = dbSvc.openDatabase(connStr);

    }
    else {
      log << MSG::VERBOSE << "Database connection already established!" << endmsg;
    }
    log << MSG::DEBUG << "Retrieve the root folderset." << endmsg;
    dbi.root = dbi.db->getFolderSet("/");
  }
  //  catch ( cool::DatabaseDoesNotExist &e ) {
  catch ( cool::Exception &e ) {
    log << MSG::ERROR << "Problems opening database" << endmsg;
    log << MSG::ERROR << e.what() << endmsg;
    dbi.db.reset();
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// TAG handling
//=============================================================================
const std::string &CondDBAccessSvc::tag() const { return m_mainDB.tag; }
StatusCode CondDBAccessSvc::setTag(const std::string &_tag){

  if (tag() == _tag) return StatusCode::SUCCESS; // no need to change

  StatusCode sc = i_checkTag(_tag,m_mainDB);
  if ( sc.isSuccess() ) {
    m_mainDB.tag = _tag;
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
StatusCode CondDBAccessSvc::i_checkTag(const std::string &tag, const DBInstance &dbi) const {
  MsgStream log( msgSvc(), name() );
  log << MSG::VERBOSE << "Check availability of tag \"" << tag << "\"" << endmsg;
  if (dbi.root) {
    // HEAD tags are always good
    //if ( (tag.empty()) || (tag == "HEAD") ) return StatusCode::SUCCESS;
    if ( dbi.root->isHeadTag(tag) ) {
      log << MSG::VERBOSE << "\"" << tag << "\" is a HEAD tag: OK" << endmsg;
      return StatusCode::SUCCESS;
    }
    // try to resolve the tag (it cannot be checked)
    try {
      dbi.root->resolveTag(tag);
      log << MSG::VERBOSE << "\"" << tag << "\" found: OK" << endmsg;
      return StatusCode::SUCCESS;
    } catch (cool::TagNotFound) {
      log << MSG::VERBOSE << "\"" << tag << "\" NOT found" << endmsg;
      return StatusCode::FAILURE;
    }
    catch (cool::TagRelationNotFound &e) {
      log << MSG::ERROR << "got a cool::TagRelationNotFound : " << e.what() << endmsg;
      return StatusCode::FAILURE;
    }
    catch (std::exception &e) {
      log << MSG::ERROR << "got a std::exception : " << e.what() << endmsg;
      return StatusCode::FAILURE;
    }

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
  return  createNode(path,descr,storage,vers);
}

StatusCode CondDBAccessSvc::createNode(const std::string &path,
                                       const std::string &descr,
                                       StorageType storage,
                                       VersionMode vers) const {
  if ( !m_mainDB.db ) {
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to create the folder \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }

  try {
    switch (storage) {
    case FOLDERSET:
      m_mainDB.db->createFolderSet(path,descr,true);
      break;
    case XML:
      {
        // append to the description the storage type
        std::ostringstream _descr;
        _descr << descr << " <storage_type=" << std::dec << XML_StorageType << ">";
        m_mainDB.db->createFolder(path,
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
  if ( !m_mainDB.db ) {
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to store the object \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }

  try {
    // retrieve folder pointer
    cool::IFolderPtr folder = m_mainDB.db->getFolder(path);
    coral::AttributeList payload;
    /// @todo This will probably change with newer COOL
    payload.extend("data","string");
    payload["data"].data<std::string>() = data;
    folder->storeObject(timeToValKey(since),timeToValKey(until),payload,channel);

  } catch (cool::FolderNotFound &e) {

    MsgStream log(msgSvc(), name() );
    if (m_mainDB.db->existsFolderSet(path))
      log << MSG::ERROR << "Trying to store data into the non-leaf folder \"" <<
        path << '\"' << endmsg;
    else
      log << MSG::ERROR << "Cannot find folder \"" << path << '\"' << endmsg;
    return StatusCode::FAILURE;

  } catch (cool::Exception &e){

    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to store the XML string into \"" << path
        << "\" (cool::Exception): " << e.what() << endmsg;
    return StatusCode::FAILURE;

  }
  return StatusCode::SUCCESS;
}

cool::ValidityKey CondDBAccessSvc::timeToValKey(const Gaudi::Time &time) const {
  // ValidityKey is an uInt64 of which only 63 bits used (0 -> 9223372036854775807),
  // while time.nd() is a positive signed Int64! (the same thing)
  return time.ns();
}

Gaudi::Time CondDBAccessSvc::valKeyToTime(const cool::ValidityKey &key) const {
  return Gaudi::Time(key);
}

StatusCode CondDBAccessSvc::tagLeafNode(const std::string &path, const std::string &tagName,
                                        const std::string &description) {
  MsgStream log(msgSvc(),name());

  if ( !m_mainDB.db ) {
    log << MSG::ERROR << "Unable to tag the leaf node \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }

  try {
    log << MSG::DEBUG << "entering tagLeafNode: \"" << path << '"' << endmsg;

    cool::IFolderPtr folder = m_mainDB.db->getFolder(path);
    if (folder->versioningMode() == cool::FolderVersioning::SINGLE_VERSION){
      log << MSG::WARNING << "Not tagging leaf node \"" << path << "\": single-version" << endmsg;
    } else {
      log << MSG::DEBUG << "Tagging leaf node \"" << path << "\": " << tagName << endmsg;
      folder->tagCurrentHead(tagName, description);
    }

  } catch (cool::FolderNotFound &e) {

    if (m_mainDB.db->existsFolderSet(path))
      log << MSG::ERROR << "Node \"" << path << "\" is not leaf." << endmsg;
    else
      log << MSG::ERROR << "Cannot find node \"" << path << '\"' << endmsg;
    return StatusCode::FAILURE;

  } catch (cool::Exception &e){

    log << MSG::ERROR << "Unable tag leaf node \"" << path
        << "\" (cool::Exception): " << e.what() << endmsg;
    return StatusCode::FAILURE;

  }

  return StatusCode::SUCCESS;
}

std::string CondDBAccessSvc::generateUniqueTagName(const std::string &base,
                                                   const std::set<std::string> &reserved) const {
  static bool first_time = true;
  if (first_time) {
    // initialize the random generator
    srandom(std::time(NULL));
    first_time = false;
  }

  std::string tag = "";
  do {
    // start with the signature
    tag = "_auto_";
    // add the base, if any
    if (!base.empty()) tag += base + "-";
    // append 6 randomly chosen chars in set [0-9A-Za-z]
    for ( int i = 0; i<6; ++i ) {
      char c=(char) (62.0*random()/(RAND_MAX+1.0));
      // if ( c > 61 ) c %= 62;
      if ( c >= 36 ) tag += c + 61;
      else if ( c >= 10 ) tag += c + 55;
      else tag += c + 48;
    }
    // check if the random name already exists or is reserved
  } while ( m_mainDB.db->existsTag(tag) || (reserved.find(tag) != reserved.end()) );

  return tag;
}


StatusCode CondDBAccessSvc::recursiveTag(const std::string &path, const std::string &tagName,
                                         const std::string &description) {
  std::set<std::string> reserved;
  return i_recursiveTag(path,tagName,description,tagName,reserved);
}

StatusCode CondDBAccessSvc::i_recursiveTag(const std::string &path, const std::string &base,
                                           const std::string &description,
                                           const std::string &tagName,
                                           std::set<std::string> &reserved) {
  MsgStream log(msgSvc(),name());

  if ( !m_mainDB.db ) {
    log << MSG::ERROR << "Unable to tag the inner node \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }

  try {
    // start reserving the tag name we want to apply to the current folderset
    reserved.insert(tagName);

    // get the list of child nodes (both types)
    cool::IFolderSetPtr this_folderset = m_mainDB.db->getFolderSet(path);
    std::vector<std::string> folders = this_folderset->listFolders();
    std::vector<std::string> foldersets = this_folderset->listFolderSets();

    // loop over leaf nodes and apply the tags
    std::vector<std::string>::iterator f;
    for ( f = folders.begin(); f != folders.end(); ++f ) {

      std::string auto_tag = generateUniqueTagName(base,reserved);
      cool::IFolderPtr child_folder = m_mainDB.db->getFolder(*f);

      if (child_folder->versioningMode() == cool::FolderVersioning::MULTI_VERSION) {
        // only multi-version folders can be tagged
        child_folder->tagCurrentHead(auto_tag, description);
        // associate the child folder tag with the parent one
        child_folder->createTagRelation(tagName, auto_tag);
      }

    }

    // loop over inner nodes and recurse
    for ( f = foldersets.begin(); f != foldersets.end(); ++f ) {

      std::string auto_tag = generateUniqueTagName(base,reserved);
      StatusCode sc = i_recursiveTag(*f,base,description,auto_tag,reserved);
      if (!sc.isSuccess()) return sc;

      cool::IFolderSetPtr child_folderset = m_mainDB.db->getFolderSet(*f);
      child_folderset->createTagRelation(tagName, auto_tag);

    }
  }
  catch (cool::FolderSetNotFound &e) {
    if (m_mainDB.db->existsFolder(path))
      log << MSG::ERROR << "Node \"" << path << "\" is a leaf." << endmsg;
    else
      log << MSG::ERROR << "Cannot find node \"" << path << '\"' << endmsg;
    return StatusCode::FAILURE;
  }
  catch (cool::Exception &e) {

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

          // find alternative for path
          cool::IDatabasePtr db = alternativeFor(path).db;

          if (db->existsFolderSet(path)) {
            // with FolderSets, I put an empty entry and clear the shared_ptr
            m_cache->addFolderSet(path,"");
            data.reset();
            return StatusCode::SUCCESS;
          }
          // go to the database
          cool::IFolderPtr folder = db->getFolder(path);
          cool::IObjectPtr obj;
          if (folder->versioningMode() == cool::FolderVersioning::SINGLE_VERSION
              || tag().empty() || tag() == "HEAD" ){
            obj = folder->findObject(vk_when,channel);
          } else {
            obj = folder->findObject(vk_when,channel,folder->resolveTag(tag()));
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

      // find alternative for path
      cool::IDatabasePtr db = alternativeFor(path).db;

      if (db->existsFolderSet(path)) {
        // with FolderSets, I clear the shared_ptr (it's the folderset signature)
        data.reset();
        return StatusCode::SUCCESS;
      }

      cool::IFolderPtr folder = db->getFolder(path);
      descr = folder->description();

      cool::IObjectPtr obj;
      if (folder->versioningMode() == cool::FolderVersioning::SINGLE_VERSION
          || tag().empty() || tag() == "HEAD"){
        obj = folder->findObject(timeToValKey(when),channel);
      } else {
        obj = folder->findObject(timeToValKey(when),channel,folder->resolveTag(tag()));
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
      
      // find alternative for path
      cool::IDatabasePtr db = alternativeFor(path).db;

      if (db->existsFolderSet(path)) {
        log << MSG::DEBUG << "FolderSet \"" << path  << "\" exists" << endmsg;

        cool::IFolderSetPtr folderSet = db->getFolderSet(path);

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

//=========================================================================
// Find the alternative
//=========================================================================
const CondDBAccessSvc::DBInstance &CondDBAccessSvc::alternativeFor(const std::string &path) const
{
  if ( path.empty() || (path == "/") ) return m_mainDB;
  
  // loop over alternatives
  std::map<std::string,DBInstance>::const_reverse_iterator alt;
  for ( alt = m_altDBs.rbegin(); alt != m_altDBs.rend(); ++alt ) {
    if ( ( path.size() < alt->first.size() ) &&
         ( path == alt->first.substr(0,path.size()) ) )
      return alt->second;
  }

  return m_mainDB;
}

