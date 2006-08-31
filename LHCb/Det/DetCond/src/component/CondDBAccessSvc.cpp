// $Id: CondDBAccessSvc.cpp,v 1.29 2006-08-31 13:53:03 marcocle Exp $
// Include files
#include <sstream>
//#include <cstdlib>
//#include <ctime>


// needed to sleep between retrials
#include "SealBase/TimeInfo.h"

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Time.h"

#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmEngine.h"

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
DECLARE_SERVICE_FACTORY(CondDBAccessSvc)

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
  Service(name,svcloc),
  m_rndmSvc(0),
  m_timeOutCheckerThread(0)
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
  declareProperty("ReadOnly",         m_readonly         = true );
  
  declareProperty("ConnectionTimeOut", m_connectionTimeOut = 0 );
  
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
  } else if ( IID_ICondDBEditor.versionMatch(riid) )   {
    *ppvUnknown = (ICondDBEditor*)this;
    addRef();
    return SUCCESS;
  } else if ( IID_ICondDBReader.versionMatch(riid) )   {
    *ppvUnknown = (ICondDBReader*)this;
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
    if ( m_connectionString == "" ) {
      // we need a connection string to connect to the DB
      log << MSG::ERROR << "Connection to database requested and no connection string provided." << endmsg;
      log << MSG::ERROR << "Set the option \"" << name() << ".ConnectionString\"." << endmsg;
      return StatusCode::FAILURE;
    }
    log << MSG::DEBUG << "Connection string = \""  << m_connectionString << "\"" << endmsg;
    
    sc = i_openConnection();
    if (!sc.isSuccess()) return sc;

    // Check the existence of the provided tag.
    sc = i_checkTag();

    // Try again if requested
    int trials_to_go = m_checkTagTrials - 1; // take into account the trial just done
    while (!sc.isSuccess() && (trials_to_go > 0)){
      log << MSG::INFO << "TAG \"" << tag() << "\" not ready, I try again in " << m_checkTagTimeOut << "s. "
          << trials_to_go << " trials left." << endmsg;
      seal::TimeInfo::sleep(m_checkTagTimeOut);
      sc = i_checkTag();
      --trials_to_go;
    }

    // Fail if the tag is not found
    if (!sc.isSuccess()){
      log << MSG::ERROR << "Bad TAG given: \"" << tag() << "\" not in the database" << endmsg;
      return sc;
    }

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

  // start TimeOut thread
  if (!m_noDB && m_connectionTimeOut) {
    touchLastAccess();
    TimeOutChecker tc(this);
    m_timeOutCheckerThread = std::auto_ptr<boost::thread>(new boost::thread(tc));
  }

  return sc;
}

//=============================================================================
// finalize
//=============================================================================
StatusCode CondDBAccessSvc::finalize(){
  MsgStream log(msgSvc(), name() );
  log << MSG::DEBUG << "Finalize" << endmsg;

  // stop TimeOut thread
  if (m_connectionTimeOut) {
    m_stop.notify_all(); // tell the thread to stop
    m_timeOutCheckerThread->join(); // wait for it
    m_timeOutCheckerThread.reset(); // delete it
  }

  // release the database
  m_db.reset();
  if (m_useCache) {
    // dump the content of the cache
    m_cache->dump();
    // dispose of the cache manager
    delete m_cache;
  }
  if ( m_rndmSvc ) m_rndmSvc->release();

  return Service::finalize();
}

//=============================================================================
// Connect to the database
//=============================================================================
StatusCode CondDBAccessSvc::i_openConnection(){
  MsgStream log(msgSvc(), name() );

  try {
    if (! m_db) { // The database is not yet opened
      log << MSG::DEBUG << "Get cool::DatabaseSvc" << endmsg;
      cool::IDatabaseSvc &dbSvc = cool::DatabaseSvcFactory::databaseService();
      log << MSG::DEBUG << "cool::DatabaseSvc got" << endmsg;

      log << MSG::DEBUG << "Opening connection" << endmsg;
      m_db = dbSvc.openDatabase(m_connectionString,m_readonly);
    
    }
    else {
      log << MSG::VERBOSE << "Database connection already established!" << endmsg;
    }
    log << MSG::DEBUG << "Retrieve the root folderset." << endmsg;
    m_rootFolderSet = m_db->getFolderSet("/");
  }
  //  catch ( cool::DatabaseDoesNotExist &e ) {
  catch ( cool::Exception &e ) {
    log << MSG::ERROR << "Problems opening database" << endmsg;
    log << MSG::ERROR << e.what() << endmsg;
    m_db.reset();
    return StatusCode::FAILURE;
  }

  touchLastAccess();
  return StatusCode::SUCCESS;
}

//=============================================================================
// TAG handling
//=============================================================================
const std::string &CondDBAccessSvc::tag() const { return m_dbTAG; }
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
  DataBaseOperationLock dbLock(this);

  MsgStream log(msgSvc(), name() );
  log << MSG::VERBOSE << "Check availability of tag \"" << tag << "\"" << endmsg;
  if (m_rootFolderSet) {
    // HEAD tags are always good
    //if ( (tag.empty()) || (tag == "HEAD") ) return StatusCode::SUCCESS;
    if ( m_rootFolderSet->isHeadTag(tag) ) {
      log << MSG::VERBOSE << "\"" << tag << "\" is a HEAD tag: OK" << endmsg;
      return StatusCode::SUCCESS;
    }
    // try to resolve the tag (it cannot be checked)
    try {
      m_rootFolderSet->resolveTag(tag);
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
StatusCode CondDBAccessSvc::createNode(const std::string &path,
                                       const std::string &descr,
                                       StorageType storage,
                                       VersionMode vers) const {
  if ( m_readonly ) {
    MsgStream log(msgSvc(), name() );
    log << "Cannot create node in read-only mode" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( !m_db ) {
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to create the folder \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }

  DataBaseOperationLock dbLock(this);
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

StatusCode CondDBAccessSvc::createNode(const std::string &path,
                                       const std::string &descr,
                                       const std::set<std::string> &fields,
                                       StorageType storage,
                                       VersionMode vers) const {
  if ( m_readonly ) {
    MsgStream log(msgSvc(), name() );
    log << "Cannot create node in read-only mode" << endmsg;
    return StatusCode::FAILURE;
  }
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
        cool::ExtendedAttributeListSpecification spec;
        for (std::set<std::string>::const_iterator f = fields.begin(); f != fields.end(); ++f ){
          spec.push_back(*f, "string", cool::PredefinedStorageHints::STRING_MAXSIZE_16M);
        }
        m_db->createFolder(path,
                           spec,
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

StatusCode CondDBAccessSvc::storeXMLData(const std::string &path, const std::string &data,
                                         const Gaudi::Time &since, const Gaudi::Time &until, cool::ChannelId channel) const {
  if ( m_readonly ) {
    MsgStream log(msgSvc(), name() );
    log << "Cannot store in read-only mode" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( !m_db ) {
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to store the object \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }

  DataBaseOperationLock dbLock(this);
  try {
    // retrieve folder pointer
    cool::IFolderPtr folder = m_db->getFolder(path);
    coral::AttributeList payload;
    /// @todo This will probably change with newer COOL
    payload.extend("data","string");
    payload["data"].data<std::string>() = data;
    folder->storeObject(timeToValKey(since),timeToValKey(until),payload,channel);

  } catch (cool::FolderNotFound) {

    MsgStream log(msgSvc(), name() );
    if (m_db->existsFolderSet(path))
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

StatusCode CondDBAccessSvc::storeXMLData(const std::string &path, const std::map<std::string,std::string> &data,
                                         const Gaudi::Time &since, const Gaudi::Time &until, cool::ChannelId channel) const {
  if ( m_readonly ) {
    MsgStream log(msgSvc(), name() );
    log << "Cannot store in read-only mode" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( !m_db ) {
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to store the object \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }
  
  try {
    // retrieve folder pointer
    cool::IFolderPtr folder = m_db->getFolder(path);

    /// @todo This will change with COOL 1.4
    coral::AttributeList payload(folder->payloadSpecification());
    for (std::map<std::string,std::string>::const_iterator d = data.begin(); d != data.end(); ++d ){
      payload[d->first].data<std::string>() = d->second;
    }
    
    folder->storeObject(timeToValKey(since),timeToValKey(until),payload,channel);

  } catch (cool::FolderNotFound) {

    MsgStream log(msgSvc(), name() );
    if (m_db->existsFolderSet(path))
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

  if ( m_readonly ) {
    log << "Cannot tag in read-only mode" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( !m_db ) {
    log << MSG::ERROR << "Unable to tag the leaf node \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }

  DataBaseOperationLock dbLock(this);
  try {
    log << MSG::DEBUG << "entering tagLeafNode: \"" << path << '"' << endmsg;

    cool::IFolderPtr folder = m_db->getFolder(path);
    if (folder->versioningMode() == cool::FolderVersioning::SINGLE_VERSION){
      log << MSG::WARNING << "Not tagging leaf node \"" << path << "\": single-version" << endmsg;
    } else {
      log << MSG::DEBUG << "Tagging leaf node \"" << path << "\": " << tagName << endmsg;
      folder->tagCurrentHead(tagName, description);
    }

  } catch (cool::FolderNotFound) {

    if (m_db->existsFolderSet(path))
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

  if (!m_db->isOpen()) {
    throw GaudiException("Database not open","CondDBAccessSvc::generateUniqueTagName",StatusCode::FAILURE);
  }
  if ( ! m_rndmSvc ) {
    IRndmGenSvc *svc;
    StatusCode sc = service("RndmGenSvc",svc,true);
    const_cast<CondDBAccessSvc*>(this)->m_rndmSvc = svc;
    if ( ! sc.isSuccess() ) {
      throw GaudiException("Cannot get a pointer to RndmGenSvc","CondDBAccessSvc::generateUniqueTagName",sc);
    }
  }
  
  std::string tag = "";
  do {
    // start with the signature
    tag = "_auto_";
    // add the base, if any
    if (!base.empty()) tag += base + "-";
    // append 6 randomly chosen chars in set [0-9A-Za-z]
    for ( int i = 0; i<6; ++i ) {
      char c=(char) ( 62.0 * m_rndmSvc->engine()->rndm() );
      if ( c > 61 ) GaudiException("Generator failure","CondDBAccessSvc::generateUniqueTagName",StatusCode::FAILURE); // c %= 62;
      if ( c >= 36 ) tag += c + 61;
      else if ( c >= 10 ) tag += c + 55;
      else tag += c + 48;
    }
    // check if the random name already exists or is reserved
  } while ( m_db->existsTag(tag) || (reserved.find(tag) != reserved.end()) );

  return tag;
}


StatusCode CondDBAccessSvc::recursiveTag(const std::string &path, const std::string &tagName,
                                         const std::string &description) {
  std::set<std::string> reserved;
  DataBaseOperationLock dbLock(this);
  return i_recursiveTag(path,tagName,description,tagName,reserved);
}

StatusCode CondDBAccessSvc::i_recursiveTag(const std::string &path, const std::string &base,
                                           const std::string &description,
                                           const std::string &tagName,
                                           std::set<std::string> &reserved) {
  MsgStream log(msgSvc(),name());

  if ( m_readonly ) {
    log << MSG::ERROR << "Cannot tag in read-only mode" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( !m_db ) {
    log << MSG::ERROR << "Unable to tag the inner node \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }

  try {
    // start reserving the tag name we want to apply to the current folderset
    reserved.insert(tagName);

    // get the list of child nodes (both types)
    cool::IFolderSetPtr this_folderset = m_db->getFolderSet(path);    
    std::vector<std::string> folders = this_folderset->listFolders();
    std::vector<std::string> foldersets = this_folderset->listFolderSets();

    // loop over leaf nodes and apply the tags
    std::vector<std::string>::iterator f;
    for ( f = folders.begin(); f != folders.end(); ++f ) {

      std::string auto_tag = generateUniqueTagName(base,reserved);
      cool::IFolderPtr child_folder = m_db->getFolder(*f);

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

      cool::IFolderSetPtr child_folderset = m_db->getFolderSet(*f);
      child_folderset->createTagRelation(tagName, auto_tag);

    }
  }
  catch (cool::FolderSetNotFound) {
    if (m_db->existsFolder(path))
      log << MSG::ERROR << "Node \"" << path << "\" is a leaf." << endmsg;
    else
      log << MSG::ERROR << "Cannot find node \"" << path << '\"' << endmsg;
    return StatusCode::FAILURE;
  }
  catch (cool::Exception &e) {
    log << MSG::ERROR << "Problems tagging" << endmsg;
    log << MSG::ERROR << e.what() << endmsg;
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
          DataBaseOperationLock dbLock(this);
          if (database()->existsFolderSet(path)) {
            // with FolderSets, I put an empty entry and clear the shared_ptr
            m_cache->addFolderSet(path,"");
            data.reset();
            return StatusCode::SUCCESS;
          }
          // go to the database
          cool::IFolderPtr folder = database()->getFolder(path);
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
      
      DataBaseOperationLock dbLock(this);
      if (database()->existsFolderSet(path)) {
        // with FolderSets, I clear the shared_ptr (it's the folderset signature)
        data.reset();
        return StatusCode::SUCCESS;
      }

      cool::IFolderPtr folder = database()->getFolder(path);
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
      DataBaseOperationLock dbLock(this);
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
StatusCode CondDBAccessSvc::cacheAddXMLFolder(const std::string &path, const std::set<std::string> &fields) {
  std::ostringstream _descr;
  _descr << " <storage_type=" << std::dec << XML_StorageType << ">";
  cool::ExtendedAttributeListSpecification spec;
  for (std::set<std::string>::const_iterator f = fields.begin(); f != fields.end(); ++f ){
    spec.push_back(*f, "string", cool::PredefinedStorageHints::STRING_MAXSIZE_16M);
  }
  return cacheAddFolder(path,_descr.str(),spec);
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
StatusCode CondDBAccessSvc::cacheAddXMLData(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
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
StatusCode CondDBAccessSvc::cacheAddXMLData(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                            const std::map<std::string,std::string> &data, cool::ChannelId channel) {
  /// @todo this is affected by the evolution in COOL API
  coral::AttributeList payload;
  for (std::map<std::string,std::string>::const_iterator d = data.begin(); d != data.end(); ++d ){
    payload.extend(d->first,"string");
    payload[d->first].data<std::string>() = d->second;
  }
  return cacheAddObject(path,since,until,payload,channel);
}

//=========================================================================
//
//=========================================================================
void CondDBAccessSvc::dumpCache() const {
  if (m_useCache) m_cache->dump();
}
