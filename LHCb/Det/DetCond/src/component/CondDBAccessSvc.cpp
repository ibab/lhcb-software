// Include files
#include <sstream>
//#include <cstdlib>
//#include <ctime>


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
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/RecordSpecification.h"
#include "CoolKernel/FolderSpecification.h"
#include "CoolKernel/StorageType.h"
#include "CoolKernel/Record.h"

#include "CoralBase/AttributeList.h"
#include "CoralBase/Exception.h"
// FIXME: Needed because of COOL bug #38422
#include "CoralBase/AttributeException.h"

#include "DetCond/ICOOLConfSvc.h"

// local
#include "CondDBAccessSvc.h"
#include "CondDBCache.h"

#include "CondDBCommon.h"
#include "IOVListHelpers.h"

// Factory implementation
DECLARE_SERVICE_FACTORY(CondDBAccessSvc)

// Utility function
namespace {
  template <class EXC>
  inline void report_exception(MsgStream &log, const std::string &msg, const EXC& e){
    log << MSG::ERROR << msg << endmsg;
    log << MSG::ERROR << System::typeinfoName(typeid(e)) << ": "
                      << e.what() << endmsg;
  }
}

#include "GaudiKernel/Sleep.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBAccessSvc
//
// 2005-01-11 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBAccessSvc::CondDBAccessSvc(const std::string& name, ISvcLocator* svcloc):
  base_class(name,svcloc),
  m_coolConfSvc(0),
  m_cache(0),
  m_rndmSvc(0),
  m_latestHeartBeat(0)
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
  declareProperty("ReadOnly",         m_readonly         = true  );

  declareProperty("ConnectionTimeOut", m_connectionTimeOutProp = 120 );

  declareProperty("LazyConnect",      m_lazyConnect      = true  );
  declareProperty("EnableXMLDirectMapping", m_xmlDirectMapping = true,
                  "Allow direct mapping from CondDB structure to"
                  " transient store.");
  declareProperty("HeartBeatCondition", m_heartBeatCondition = "");

  declareProperty("QueryGranularity", m_queryGranularity = 0,
                  "Granularity of the query in the database (in time units), "
                  "to allow bulk retrievals.");
}

//=============================================================================
// Destructor
//=============================================================================
CondDBAccessSvc::~CondDBAccessSvc() {}

//=============================================================================
// initialize
//=============================================================================
StatusCode CondDBAccessSvc::initialize(){
  StatusCode sc = base_class::initialize();
  if (sc.isFailure()) return sc;

  MsgStream log(msgSvc(), name() );
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Initialize" << endmsg;

  if (m_connectionTimeOutProp) {
    m_connectionTimeOut = boost::posix_time::seconds(m_connectionTimeOutProp);
  } else {
    m_connectionTimeOut = boost::posix_time::pos_infin;
  }

  if ( m_noDB && !m_useCache ) {
    log << MSG::ERROR << "Database access disabled and cache off: I cannot work like that. Ciao!" << endmsg;
    return StatusCode::FAILURE;
  }

  if ( !m_noDB ) {
    if ( connectionString() == "" ) {
      // we need a connection string to connect to the DB
      log << MSG::ERROR << "Connection to database requested and no connection string provided." << endmsg;
      log << MSG::ERROR << "Set the option \"" << name() << ".ConnectionString\"." << endmsg;
      return StatusCode::FAILURE;
    }

    if ( ! m_lazyConnect ) {
      sc = i_initializeConnection();
      if (!sc.isSuccess()) return sc;
    }

  }
  else {
    log << MSG::INFO << "Database not requested: I'm not trying to connect" << endmsg;
  }

  // set up cache if needed
  if (m_useCache) {
    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG << "Initialize CondDB cache." << endmsg;
    m_cache = new CondDBCache(MsgStream(msgSvc(), name() + ".Cache"),
                              m_cacheHL, m_cacheLL);
    if (m_cache == NULL) {
      log << MSG::ERROR << "Unable to initialize CondDB cache." << endmsg;
      return StatusCode::FAILURE;
    }
    // when we do bulk retrievals it is normal to have overlaps when inserting objects
    // into the cache
    m_cache->setSilentConflicts(m_queryGranularity > 0);

  } else {
    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG << "CondDB cache not needed" << endmsg;
    m_cache = NULL;
  }
  if ( m_xmlDirectMapping && ! m_useCache ) {
    // @todo: make it possible to use the direct mapping without cache
    log << MSG::FATAL << "Cannot use direct XML mapping without cache (YET)" << endmsg;
    return StatusCode::FAILURE;
  }

  if( UNLIKELY( log.level() <= MSG::DEBUG ) ) {
    if (!m_heartBeatCondition.empty()) {
      log << MSG::DEBUG << "Using heart beat condition \"" << m_heartBeatCondition << '"' << endmsg;
    }
  }

  return sc;
}

//=============================================================================
// finalize
//=============================================================================
StatusCode CondDBAccessSvc::finalize(){
  if( UNLIKELY( m_outputLevel <= MSG::DEBUG ) ) {
    MsgStream log(msgSvc(), name() );
    log << MSG::DEBUG << "Finalize" << endmsg;
  }

  // stop TimeOut thread
  i_stopTimeoutChecker();

  // release the database
  m_db.reset();
  if (m_useCache) {
    // dump the content of the cache
    m_cache->dump();
    // dispose of the cache manager
    delete m_cache;
  }
  if ( m_rndmSvc ) m_rndmSvc->release();

  return base_class::finalize();
}

//=============================================================================
// Connect to the database
//=============================================================================
StatusCode CondDBAccessSvc::i_initializeConnection(){
  if( UNLIKELY( m_outputLevel <= MSG::DEBUG ) ) {
    MsgStream log(msgSvc(), name() );
    log << MSG::DEBUG << "Connection string = \"" << connectionString() << "\"" << endmsg;
  }

  StatusCode sc = i_openConnection();
  if (!sc.isSuccess()) return sc;

  // start TimeOut thread
  i_startTimeoutChecker();

  return i_validateDefaultTag();
}

//=============================================================================
// Connect to the database
//=============================================================================
StatusCode CondDBAccessSvc::i_openConnection(){
  MsgStream log(msgSvc(), name() );

  try {
    if (! m_db) { // The database is not yet opened

      if ( !m_coolConfSvc ) {
        StatusCode sc = service("COOLConfSvc",m_coolConfSvc,true);
        if (sc.isFailure()){
          log << MSG::ERROR << "Cannot get COOLConfSvc" << endmsg;
          return sc;
        }
      }

      if( UNLIKELY( log.level() <= MSG::DEBUG ) )
        log << MSG::DEBUG << "Get cool::DatabaseSvc" << endmsg;
      cool::IDatabaseSvc &dbSvc = m_coolConfSvc->databaseSvc();
      if( UNLIKELY( log.level() <= MSG::DEBUG ) ) {
        log << MSG::DEBUG << "cool::DatabaseSvc got" << endmsg;
        log << MSG::DEBUG << "Opening connection" << endmsg;
      }
      m_db = dbSvc.openDatabase(connectionString(),m_readonly);

    }
    else {
      if( UNLIKELY( log.level() <= MSG::VERBOSE ) )
        log << MSG::VERBOSE << "Database connection already established!" << endmsg;
    }
    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG << "Retrieve the root folderset." << endmsg;
    m_rootFolderSet = m_db->getFolderSet("/");
  }
  //  catch ( cool::DatabaseDoesNotExist &e ) {
  catch ( coral::Exception &e ) {
    report_exception(log,"Problems opening database",e);
    m_db.reset();
    return StatusCode::FAILURE;
  }
  catch ( cool::Exception &e ) {
    report_exception(log,"Problems opening database",e);
    m_db.reset();
    return StatusCode::FAILURE;
  }

  touchLastAccess();
  log << MSG::INFO << "Connected to database \"" << connectionString() << "\"" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode CondDBAccessSvc::i_validateDefaultTag() {
  MsgStream log(msgSvc(), name() );

  // Check the existence of the provided tag.
  StatusCode sc = i_checkTag();

  // Try again if requested
  int trials_to_go = m_checkTagTrials - 1; // take into account the trial just done
  while (!sc.isSuccess() && (trials_to_go > 0)){
    log << MSG::INFO << "TAG \"" << tag() << "\" not ready, I try again in " << m_checkTagTimeOut << "s. "
        << trials_to_go << " trials left." << endmsg;
    Gaudi::Sleep(m_checkTagTimeOut);
    sc = i_checkTag();
    --trials_to_go;
  }

  // Fail if the tag is not found
  if (!sc.isSuccess()){
    log << MSG::ERROR << "Bad TAG given: \"" << tag() << "\" not in the database" << endmsg;
    return sc;
  }
  log << MSG::INFO << "Using TAG \"" << tag() << "\"" << endmsg;
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
    // the cache must be cleared if the tag is changed
    clearCache();
    MsgStream log(msgSvc(), name() );
    log << MSG::WARNING << "TAG changed to \"" << _tag << "\"" << endmsg;
  } else {
    MsgStream log(msgSvc(), name() );
    log << MSG::WARNING << "Unable to set TAG \"" << _tag
        << "\": not in the DB. (Still using \"" << tag() << "\")" << endmsg;
  }
  return sc;
}
StatusCode CondDBAccessSvc::i_checkTag(const std::string &tag) const {

  MsgStream log(msgSvc(), name() );
  if ( !m_db ) {
    log << MSG::ERROR << "Check tag \"" << tag
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }

  DataBaseOperationLock dbLock(this);

  if( UNLIKELY( log.level() <= MSG::VERBOSE ) )
    log << MSG::VERBOSE << "Check availability of tag \"" << tag << "\"" << endmsg;
  /// @TODO: check all sub-nodes to validate the tag and not only the root
  if (m_rootFolderSet) {
    // HEAD tags are always good
    //if ( (tag.empty()) || (tag == "HEAD") ) return StatusCode::SUCCESS;
    if ( cool::IHvsNode::isHeadTag(tag) ) {
      if( UNLIKELY( log.level() <= MSG::VERBOSE ) )
        log << MSG::VERBOSE << "\"" << tag << "\" is a HEAD tag: OK" << endmsg;
      return StatusCode::SUCCESS;
    }
    // try to resolve the tag (it cannot be checked)
    try {
      try {
        m_rootFolderSet->resolveTag(tag);
      } catch (cool::NodeRelationNotFound) {
        // to be ignored: it means that the tag exists, but somewhere else.
      } catch (coral::AttributeException) { // FIXME: COOL bug #38422
        // to be ignored: it means that the tag exists, but somewhere else.
      }
      if( UNLIKELY( log.level() <= MSG::VERBOSE ) )
        log << MSG::VERBOSE << "\"" << tag << "\" found: OK" << endmsg;
      return StatusCode::SUCCESS;
    } catch (cool::TagNotFound &) {
      if( UNLIKELY( log.level() <= MSG::VERBOSE ) )
        log << MSG::VERBOSE << "\"" << tag << "\" NOT found" << endmsg;
      return StatusCode::FAILURE;
    }
    catch (cool::TagRelationNotFound &e) {
      log << MSG::ERROR << "got a cool::TagRelationNotFound : " << e.what() << endmsg;
      return StatusCode::FAILURE;
    }
    catch (std::exception &e) {
      report_exception(log,"got exception",e);
      return StatusCode::FAILURE;
    }

  }
  return StatusCode::FAILURE;
}


//=============================================================================
// Return the connection string used to connect to the database.
//=============================================================================
const std::string &CondDBAccessSvc::connectionString() const{
  return m_connectionString;
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
        cool::FolderSpecification spec((vers == SINGLE)
                                       ?cool::FolderVersioning::SINGLE_VERSION
                                       :cool::FolderVersioning::MULTI_VERSION,
                                       CondDB::getXMLStorageSpec());

        // append to the description the storage type
        std::ostringstream _descr;
        _descr << descr << " <storage_type=" << std::dec << XML_StorageType << ">";
        m_db->createFolder(path,
                           spec,
                           _descr.str(),
                           true);
      }
      break;
    default:
      MsgStream log(msgSvc(), name() );
      log << MSG::ERROR << "Unable to create the folder \"" << path
          << "\": unknown StorageType" << endmsg;
      return StatusCode::FAILURE;
    }
  } catch(cool::NodeExists &){
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
        cool::RecordSpecification recSpec;
        for (std::set<std::string>::const_iterator f = fields.begin(); f != fields.end(); ++f ){
          recSpec.extend(*f, cool::StorageType::String16M);
        }
        cool::FolderSpecification spec((vers == SINGLE)
                                       ?cool::FolderVersioning::SINGLE_VERSION
                                       :cool::FolderVersioning::MULTI_VERSION,
                                       recSpec);

        // append to the description the storage type
        std::ostringstream _descr;
        _descr << descr << " <storage_type=" << std::dec << XML_StorageType << ">";
        m_db->createFolder(path,
                           spec,
                           _descr.str(),
                           true);
      }
      break;
    default:
      MsgStream log(msgSvc(), name() );
      log << MSG::ERROR << "Unable to create the folder \"" << path
          << "\": unknown StorageType" << endmsg;
      return StatusCode::FAILURE;
    }
  } catch(cool::NodeExists &){
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
    cool::Record payload(folder->payloadSpecification());
    payload["data"].setValue<cool::String16M>(data);
    folder->storeObject(timeToValKey(since),timeToValKey(until),payload,channel);

  } catch (cool::FolderNotFound &) {

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
    cool::Record payload(folder->payloadSpecification());
    for (std::map<std::string,std::string>::const_iterator d = data.begin(); d != data.end(); ++d ){
      payload[d->first].setValue<cool::String16M>(d->second);
    }

    folder->storeObject(timeToValKey(since),timeToValKey(until),payload,channel);

  } catch (cool::FolderNotFound &) {

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
  // while time.ns() is a positive signed Int64! (the same thing)
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
    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG << "entering tagLeafNode: \"" << path << '"' << endmsg;

    cool::IFolderPtr folder = m_db->getFolder(path);
    if (folder->versioningMode() == cool::FolderVersioning::SINGLE_VERSION){
      log << MSG::WARNING << "Not tagging leaf node \"" << path << "\": single-version" << endmsg;
    } else {
      if( UNLIKELY( log.level() <= MSG::DEBUG ) )
        log << MSG::DEBUG << "Tagging leaf node \"" << path << "\": " << tagName << endmsg;
      folder->tagCurrentHead(tagName, description);
    }

  } catch (cool::FolderNotFound &) {

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

  if ( !m_db->isOpen() ) {
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
  catch (cool::FolderSetNotFound &) {
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
                                      DataPtr &data,
                                      std::string &descr, Gaudi::Time &since, Gaudi::Time &until,
                                      cool::ChannelId channel){
  return i_getObject(path, when, data, descr, since, until,
                     true, channel, "");
}

StatusCode CondDBAccessSvc::getObject(const std::string &path, const Gaudi::Time &when,
                                      DataPtr &data,
                                      std::string &descr, Gaudi::Time &since, Gaudi::Time &until,
                                      const std::string &channel){
  return i_getObject(path, when, data, descr, since, until,
                     false, 0, channel);
}
/*
namespace {
  ICondDBReader::IOVList getHoles(const ICondDBReader::IOV& iov, const ICondDBReader::IOVList& data){
    typedef ICondDBReader::IOVList IOVList;
    typedef ICondDBReader::IOV IOV;
    IOVList result;

    Gaudi::Time last = iov.since; // keep track of the end of coverage
    // loop over covering interval
    for (IOVList::const_iterator covered = data.begin(); covered != data.end(); ++covered) {
      if (covered->since > last) { // hole between the end of coverage and begin of next IOV
        result.push_back(IOV(last, covered->since));
      }
      last = covered->until; // prepare to look for the next hole
    }
    if (last < iov.until) {
      // we didn't get anything to cover until the end of the requested IOV
      result.push_back(IOV(last, iov.until));
    }

    return result;
  }
}
*/

ICondDBReader::IOVList CondDBAccessSvc::i_getIOVsFromDB(const std::string & path, const IOV &iov, cool::ChannelId channel) {
  ICondDBReader::IOVList result;
  try {
    DataBaseOperationLock dbLock(this);
    // we want a folder, so go to the database to get it
    cool::IFolderPtr folder = database()->getFolder(path);
    cool::IObjectIteratorPtr objects;

    // FIXME: we need to considered the query granularity
    // Note: IFolder::browseObject returns the objects valid up to the 'until'
    //       included, which means that asking for [1,10] will return also the
    //       object starting at 10, so, to exclude it we need to ask for [1,9].
    if (folder->versioningMode() == cool::FolderVersioning::SINGLE_VERSION
        || tag().empty() || tag() == "HEAD" ){
      objects = folder->browseObjects(iov.since.ns(), iov.until.ns() - 1, channel);
    } else {
      objects = folder->browseObjects(iov.since.ns(), iov.until.ns() - 1, channel, folder->resolveTag(tag()));
    }

    if (!objects->isEmpty()) {// check if we managed to find anything
      while (objects->goToNext()) {
        // add data to the cache while filling the list of IOVs
        const cool::IObject &obj = objects->currentRef();
        m_cache->insert(folder, obj, channel);
        result.push_back(ICondDBReader::IOV(Gaudi::Time(obj.since()), Gaudi::Time(obj.until())));
      }
    }
  } catch(cool::FolderNotFound &/*e*/) {
    // ignore
  } catch (cool::TagRelationNotFound &/*e*/) {
    // ignore
  } catch (cool::NodeRelationNotFound &) {
    // to be ignored: it means that the tag exists, but it is not in the
    // node '/'.
  } catch (coral::AttributeException &) { // FIXME: COOL bug #38422
    // to be ignored: it means that the tag exists, but it is not in the
    // node '/'.
  }
  return result;
}

ICondDBReader::IOVList CondDBAccessSvc::getIOVs(const std::string & path, const IOV &iov, cool::ChannelId channel)
{
  typedef ICondDBReader::IOVList IOVList;
  IOVList result;
  if (m_useCache){
    /// Look for holes in the timeline of the cache
    result = m_cache->getIOVs(path, iov, channel);
    IOVList holes = IOVListHelpers::find_holes(result, iov);
    for(IOVList::iterator hole = holes.begin(); hole != holes.end(); ++hole) {
      const IOVList cover = i_getIOVsFromDB(path, *hole, channel);
      result.insert(result.end(), cover.begin(), cover.end());
    }
    std::sort(result.begin(), result.end());
  } else {
    result = i_getIOVsFromDB(path, iov, channel);
  }
  return result;
}

ICondDBReader::IOVList CondDBAccessSvc::getIOVs(const std::string & path, const IOV &iov, const std::string & channel)
{
  cool::ChannelId id;
  if (m_useCache){
    // Check if the cache knows about the path
    if (m_cache->hasPath(path)) {
      // the folder is in the cache
      if (m_cache->getChannelId(path, channel, id)) {
        return getIOVs(path, iov, id); // we know about the folder and the channel
      } else {
        return ICondDBReader::IOVList(); // we know about the folder, but not about the channel
      }
    }
  }

  // the folder is not in the cache or we do not use the cache, so we have to
  // get the channel id from the DB
  try {
    DataBaseOperationLock dbLock(this);
    cool::IFolderPtr folder = database()->getFolder(path);
    id = folder->channelId(channel);
  } catch(cool::FolderNotFound &/*e*/) {
    return ICondDBReader::IOVList(); // unknown folder
  } catch(cool::InvalidChannelName &/*e*/) {
    return ICondDBReader::IOVList(); // unknown channel
  }
  return getIOVs(path, iov, id);
}

StatusCode CondDBAccessSvc::i_getObjectFromDB(const std::string &path, const cool::ValidityKey &when,
                                              DataPtr &data,
                                              std::string &descr, cool::ValidityKey &since, cool::ValidityKey &until,
                                              bool use_numeric_chid, cool::ChannelId channel, const std::string &channelstr){
  try {

    bool existsFolderSet = false;
    {
      DataBaseOperationLock dbLock(this);
      existsFolderSet = database()->existsFolderSet(path);
    }
    // Check if the user asked for a folderset
    if (existsFolderSet) {
      if ( !m_xmlDirectMapping ) {
        // with FolderSets, I put an empty entry and clear the shared_ptr
        if (m_useCache) m_cache->addFolderSet(path,"");
        data.reset();
      } else {
        // Using XML direct mapping, foldersets are replaced in the cache
        // with the XML equivalent (a catalog).
        i_generateXMLCatalogFromFolderset(path);
        // now get the data from the cache
        m_cache->get(path,when,channel,since,until,descr,data);
      }

      return StatusCode::SUCCESS;
    }
    else {
      // Special retrieval procedure if we use "query granularity" (make sense
      // only when using the cache).
      if (m_useCache && m_queryGranularity > 0){
        // modify the range rounding it to the requested granularity (if needed)
        // Range used for the query
        cool::ValidityKey sinceWhen = when, untilWhen = when;

        sinceWhen -= when % m_queryGranularity;
        untilWhen = sinceWhen + m_queryGranularity;

        if( UNLIKELY( m_outputLevel <= MSG::DEBUG ) ) {
          MsgStream log(msgSvc(),name());
          log << MSG::DEBUG << "Retrieving conditions in range "
              << sinceWhen << " - " << untilWhen << endmsg;
        }

        DataBaseOperationLock dbLock(this);
        // we want a folder, so go to the database to get it
        cool::IFolderPtr folder = database()->getFolder(path);
        cool::IObjectIteratorPtr objects;
        if ( !use_numeric_chid ) { // we need to convert from name to id
          channel = folder->channelId(channelstr);
        }

        if (folder->versioningMode() == cool::FolderVersioning::SINGLE_VERSION
            || tag().empty() || tag() == "HEAD" ){
          objects = folder->browseObjects(sinceWhen, untilWhen, channel);
        } else {
          objects = folder->browseObjects(sinceWhen, untilWhen, channel, folder->resolveTag(tag()));
        }

        if (objects->isEmpty()) // check if we managed to find anything
          return StatusCode::FAILURE;

        while (objects->goToNext()) {
          m_cache->insert(folder, objects->currentRef(), channel);
        }
        // now get the data from the cache
        m_cache->get(path, when, channel, since, until, descr, data);

      } else { // no-cache or no granularity are quite similar

        DataBaseOperationLock dbLock(this);
        // we want a folder, so go to the database to get it
        cool::IFolderPtr folder = database()->getFolder(path);
        cool::IObjectPtr object;
        if ( !use_numeric_chid ) { // we need to convert from name to id
          channel = folder->channelId(channelstr);
        }

        if (folder->versioningMode() == cool::FolderVersioning::SINGLE_VERSION
            || tag().empty() || tag() == "HEAD" ){
          object = folder->findObject(when, channel);
        } else {
          object = folder->findObject(when, channel, folder->resolveTag(tag()));
        }

        if (m_useCache) {
          // add the object to the cache
          m_cache->insert(folder, *object, channel);
          // and get the data back
          m_cache->get(path, when, channel, since, until, descr, data);
        } else {
          data = DataPtr(new cool::Record(object->payload()));
          descr = folder->description();
          since = object->since();
          until = object->until();
        }
      }
    }
  } catch ( cool::FolderNotFound &/*e*/) {
    //log << MSG::ERROR << e << endmsg;
    return StatusCode::FAILURE;
  } catch (cool::TagRelationNotFound &/*e*/) {
    return StatusCode::FAILURE;
  } catch (cool::ObjectNotFound &/*e*/) {
    //log << MSG::ERROR << "Object not found in \"" << path <<
    //  "\" for tag \"" << (*accSvc)->tag() << "\" ("<< now << ')' << endmsg;
    //log << MSG::DEBUG << e << endmsg;
    return StatusCode::FAILURE;
  } catch (cool::NodeRelationNotFound &) {
    // to be ignored: it means that the tag exists, but it is not in the
    // node '/'.
    return StatusCode::FAILURE;
  } catch (coral::AttributeException &) { // FIXME: COOL bug #38422
    // to be ignored: it means that the tag exists, but it is not in the
    // node '/'.
    return StatusCode::FAILURE;
  } catch (coral::Exception &e) {
    MsgStream log(msgSvc(),name());
    report_exception(log,"got CORAL exception",e);
  }
  return StatusCode::SUCCESS;
}

StatusCode CondDBAccessSvc::i_getObject(const std::string &path, const Gaudi::Time &when,
                                        DataPtr &data,
                                        std::string &descr, Gaudi::Time &since, Gaudi::Time &until,
                                        bool use_numeric_chid, cool::ChannelId channel, const std::string &channelstr){

  cool::ValidityKey vk_when = timeToValKey(when);
  cool::ValidityKey vk_since = 0, vk_until = 0;

  // This is not in i_getObjectFromDB because I need to ensure that m_latestHeartBeat
  // is correctly set even when using the cache.
  if (vk_when >= i_latestHeartBeat()) {
    MsgStream log(msgSvc(), name());
    const Gaudi::Time hb = valKeyToTime(i_latestHeartBeat());
    log << MSG::ERROR << "Database not up-to-date. Latest known update is at "
        << hb.format(false, "%Y-%m-%d %H:%M:%S") << "." << hb.nanoformat()
        << " UTC, event time is "
        << when.format(false, "%Y-%m-%d %H:%M:%S") << "." << when.nanoformat()
        << " UTC" << endmsg;
    return StatusCode::FAILURE;
  }

  if (m_useCache) {

    // Check if the cache knows about the path
    if ( m_cache->hasPath(path) ) {

      // the folder is in the cache
      if ( !use_numeric_chid ) { // we need to convert from name to id
        if (!m_cache->getChannelId(path,channelstr,channel)) {
          // the channel name cannot be found in the cached folder
          return StatusCode::FAILURE;
        }
      }

      if ( m_cache->get(path,vk_when,channel,vk_since,vk_until,descr,data) ) {
        since = valKeyToTime(vk_since);
        /// Artificially cutting the end of validity of the retrieved object to
        /// the latest know heart beat guarantees that we will have to go back
        /// to the database when the event time exceeds it.
        /// Note that we are not calling i_latestHeartBeat() on purpose:
        /// it returns +inf if called during initialize, but it sets
        /// correctly the variable m_latestHeartBeat.
        until = valKeyToTime(std::min(vk_until, m_latestHeartBeat));
        return StatusCode::SUCCESS;
      }
    }

  }
  // If we get here, either we do not know about the folder, we didn't
  // find the object, or we are not using the cache, so let's try the DB
  if (m_noDB) {
    // oops... we are not using the db: no way of getting the object from it
    return StatusCode::FAILURE;
  }

  StatusCode sc = i_getObjectFromDB(path,vk_when,data,descr,vk_since,vk_until,use_numeric_chid,channel,channelstr);
  since = valKeyToTime(vk_since);
  /// Artificially cutting the end of validity of the retrieved object to
  /// the latest know heart beat guarantees that we will have to go back
  /// to the database when the event time exceeds it.
  /// Note that we are not calling i_latestHeartBeat() on purpose:
  /// it returns +inf if called during initialize, but it sets
  /// correctly the variable m_latestHeartBeat.
  until = valKeyToTime(std::min(vk_until, m_latestHeartBeat));
  return sc;
}

namespace {
  // Small helper function to reduce duplications
  inline void cannotGetHeartBeatError(CondDBAccessSvc *self, const std::string&path) {
    MsgStream log(self->msgSvc(), self->name());
    log << MSG::ERROR << "Cannot get latest heart beat (" << path
        << ") in the database" << endmsg;
  }
}
const cool::ValidityKey& CondDBAccessSvc::i_latestHeartBeat()
{
  if (m_latestHeartBeat == 0) {
    if (m_heartBeatCondition.empty() ||
        m_noDB) { // it doesn't make sense to ask for a heart beat if we do not use the DB
      // no heart beat condition: the database is always valid
      m_latestHeartBeat = cool::ValidityKeyMax;
    } else {
      if( UNLIKELY( m_outputLevel <= MSG::DEBUG ) ) {
        MsgStream log(msgSvc(),name());
        log << MSG::DEBUG << "Retrieving heart beat condition \"" << m_heartBeatCondition << '"' << endmsg;
      }
      // we do not use the normal functions to retrieve the object because
      // we want to by-pass the cache
      try {
        DataBaseOperationLock dbLock(this);
        cool::IFolderPtr folder = database()->getFolder(m_heartBeatCondition);
        cool::IObjectPtr obj = folder->findObject(cool::ValidityKeyMax-1, 0);
        m_latestHeartBeat = obj->since();
      }
      catch (cool::Exception &) {
        cannotGetHeartBeatError(this, m_heartBeatCondition);
        m_latestHeartBeat = 1; // not set to 0 to avoid another search in the database
      }
      catch (coral::Exception &) {
        cannotGetHeartBeatError(this, m_heartBeatCondition);
        m_latestHeartBeat = 1; // not set to 0 to avoid another search in the database
      }
      if( UNLIKELY( m_outputLevel <= MSG::DEBUG ) ) {
        MsgStream log(msgSvc(),name());
        log << MSG::DEBUG << "Latest heart beat at " << m_latestHeartBeat << endmsg;
      }
    }
  }
  if (FSMState() != Gaudi::StateMachine::RUNNING) {
    // Temporarily consider the database valid if not running
    // (e.g. during initialize).
    // Note that the retrieve is done (and must be done) anyway,
    // because it is needed by i_getObject().
    return cool::ValidityKeyMax;
  }
  return m_latestHeartBeat;
}


//=========================================================================
//
//=========================================================================
StatusCode CondDBAccessSvc::getChildNodes (const std::string &path,
                                           std::vector<std::string> &folders,
                                           std::vector<std::string> &foldersets) {
  MsgStream log(msgSvc(),name());
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Entering \"getChildNodes\"" << endmsg;

  folders.clear();
  foldersets.clear();

  try {

    if (!m_noDB) { // If I have the DB I always use it!
      DataBaseOperationLock dbLock(this);
      if (database()->existsFolderSet(path)) {
        if( UNLIKELY( log.level() <= MSG::DEBUG ) )
          log << MSG::DEBUG << "FolderSet \"" << path  << "\" exists" << endmsg;

        cool::IFolderSetPtr folderSet = database()->getFolderSet(path);

        std::vector<std::string> fldr_names = folderSet->listFolders();
        std::vector<std::string> fldrset_names = folderSet->listFolderSets();

        for ( std::vector<std::string>::iterator f = fldr_names.begin(); f != fldr_names.end(); ++f ) {
          if( UNLIKELY( log.level() <= MSG::DEBUG ) )
            log << MSG::DEBUG << *f << endmsg;
          // Check if folder is tagged with a tag set to load db with.
          cool::IFolderPtr folder = database()->getFolder(*f);
          if (folder->versioningMode() == cool::FolderVersioning::MULTI_VERSION){
            try{
              folder->resolveTag(tag());
              folders.push_back(f->substr(f->rfind('/')+1));
            } catch (cool::TagRelationNotFound &) {
              if( UNLIKELY( log.level() <= MSG::DEBUG ) )
                log << MSG::DEBUG << "Tag '" << tag()
                    << "' relation was not found for ': "<< *f << "' folder" << endmsg;
            } catch (cool::ReservedHeadTag &) {
              //to be ignored: 'HEAD' tag is in every node
              folders.push_back(f->substr(f->rfind('/')+1));
            } catch (cool::NodeRelationNotFound &) {
              //to be ignored: it means that the tag exists, but it is not in the node '/'.
            } catch (coral::AttributeException &) { // FIXME: COOL bug #38422. To be ignored:
              //it means that the tag exists, but it is not in the node '/'.
            } catch (coral::Exception &e) {
              report_exception(log,"got CORAL exception",e);
              folders.push_back(f->substr(f->rfind('/')+1));
            }
          } else { //add folder if it is single version without tag verification
            folders.push_back(f->substr(f->rfind('/')+1));
          }
        }

        for ( std::vector<std::string>::iterator f = fldrset_names.begin(); f != fldrset_names.end(); ++f ) {
          if( UNLIKELY( log.level() <= MSG::DEBUG ) )
            log << MSG::DEBUG << *f << endmsg;
          foldersets.push_back(f->substr(f->rfind('/')+1));
        }

        if( UNLIKELY( log.level() <= MSG::DEBUG ) ) {
          log << MSG::DEBUG << "got " << folders.size() << " sub folders" << endmsg;
          log << MSG::DEBUG << "got " << foldersets.size() << " sub foldersets" << endmsg;
        }

      } else {
        // cannot get the sub-nodes of a folder!
        return StatusCode::FAILURE;
      }
    } else if (m_useCache) {
      // if no db, but cache, let's assume we know everything is in there
      m_cache->getSubNodes(path,folders,foldersets);
    } else {
      // no cache and no db
      return StatusCode::FAILURE;
    }
  } catch ( cool::FolderNotFound &/*e*/) {
    //log << MSG::ERROR << e << endmsg;
    return StatusCode::FAILURE;
  } catch (coral::Exception &e) {
    report_exception(log,"got CORAL exception",e);
  }
  return StatusCode::SUCCESS;


}

//=========================================================================
//
//=========================================================================
StatusCode CondDBAccessSvc::getChildNodes (const std::string &path, std::vector<std::string> &node_names) {

  std::vector<std::string> temp;
  StatusCode sc = getChildNodes(path, node_names, temp);
  if (sc.isSuccess())
    node_names.insert(node_names.end(), temp.begin(), temp.end());
  return sc;

}

//=========================================================================
// Tells if the path is available in the database.
//=========================================================================
bool CondDBAccessSvc::exists(const std::string &path) {

  try {

    if (!m_noDB) { // If I have the DB I always use it!
      DataBaseOperationLock dbLock(this);
      return database()->existsFolderSet(path) || database()->existsFolder(path);
    } else if (m_useCache) {
      // if no db, but cache, let's assume we know everything is in there
      return m_cache->hasPath(path);
    }

  } catch (coral::Exception &e) {
    MsgStream log(msgSvc(),name());
    report_exception(log,"got CORAL exception",e);
  }
  // if we do not have neither DB nor cache, or we got an exception
  return false;
}

//=========================================================================
// Tells if the path (if it exists) is a folder.
//=========================================================================
bool CondDBAccessSvc::isFolder(const std::string &path) {

  try {

    if (!m_noDB) { // If I have the DB I always use it!
      DataBaseOperationLock dbLock(this);
      return database()->existsFolder(path);
    } else if (m_useCache) {
      // if no db, but cache, let's assume we know everything is in there
      return m_cache->isFolder(path);
    }

  } catch (coral::Exception &e) {
    MsgStream log(msgSvc(),name());
    report_exception(log,"got CORAL exception",e);
  }
  // if we do not have neither DB nor cache, or we got an exception
  return false;
}

//=========================================================================
// Tells if the path (if it exists) is a folderset.
//=========================================================================
bool CondDBAccessSvc::isFolderSet(const std::string &path) {

  try {

    if (!m_noDB) { // If I have the DB I always use it!
      DataBaseOperationLock dbLock(this);
      return database()->existsFolderSet(path);
    } else if (m_useCache) {
      // if no db, but cache, let's assume we know everything is in there
      return m_cache->isFolderSet(path);
    }

  } catch (coral::Exception &e) {
    MsgStream log(msgSvc(),name());
    report_exception(log,"got CORAL exception",e);
  }
  // if we do not have neither DB nor cache, or we got an exception
  return false;
}

//=========================================================================
// Disconnect from the database.
//=========================================================================
void CondDBAccessSvc::disconnect() {
  boost::mutex::scoped_lock busy_lock(m_busy);
  if ( database() && database()->isOpen() ) {
    if ( UNLIKELY( msgLevel() <= MSG::DEBUG ) ) {
      debug() << "Forced disconnect from database (will reconnect automatically)" << endmsg;
    }
    database()->closeDatabase();
  } else if ( UNLIKELY( msgLevel() <= MSG::DEBUG ) ) {
    debug() << "Database already disconnected" << endmsg;
  }
  i_stopTimeoutChecker();
}

//=========================================================================
// Add database name and TAG to the passed vector.
//=========================================================================
void CondDBAccessSvc::defaultTags ( std::vector<LHCb::CondDBNameTagPair>& tags ) const {
  /// @todo This should be something like
  /// <quote>
  /// tags.push_back(LHCb::CondDBNameTagPair(database()->dbName(),tag()));
  /// </quote>
  /// but COOL API does not provide that function yet. (Available since 2.2.0)

  std::string dbName;
  // Parsing of COOL connection string to find database name
  // - first type: <tech>://<server>;schema=<schema>;dbname=<dbname>
  std::string::size_type pos = connectionString().find("dbname=");
  if ( std::string::npos != pos ) {
    pos += 7;
    std::string::size_type pos2 = connectionString().find(';',pos);
    if ( std::string::npos != pos2 )
      dbName = connectionString().substr(pos,pos2-pos);
    else
      dbName = connectionString().substr(pos);
  } else {
    // - second type: <alias>/<dbname>
    pos = connectionString().find_last_of('/');
    if ( std::string::npos != pos ) {
      dbName = connectionString().substr(pos+1);
    } else {
      throw GaudiException("Cannot understand COOL connection string",
                           "CondDBAccessSvc::defaultTags",StatusCode::FAILURE);
    }
  }
  // If the tag is a "HEAD" tag, I want to show "HEAD"
  std::string tagName = tag();
  if (cool::IHvsNode::isHeadTag(tagName)) {
    tagName = "HEAD";
  }

  tags.push_back(LHCb::CondDBNameTagPair(dbName,tagName));

}

//=========================================================================
//
//=========================================================================
StatusCode CondDBAccessSvc::cacheAddFolder(const std::string &path, const std::string &descr,
                                           const cool::IRecordSpecification& spec) {
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
  return cacheAddFolder(path,_descr.str(),CondDB::getXMLStorageSpec());
}

//=========================================================================
//
//=========================================================================
StatusCode CondDBAccessSvc::cacheAddXMLFolder(const std::string &path, const std::set<std::string> &fields) {
  std::ostringstream _descr;
  _descr << " <storage_type=" << std::dec << XML_StorageType << ">";
  cool::RecordSpecification spec;
  for (std::set<std::string>::const_iterator f = fields.begin(); f != fields.end(); ++f ){
    spec.extend(*f, cool::StorageType::String16M);
  }
  return cacheAddFolder(path,_descr.str(),spec);
}

//=========================================================================
//
//=========================================================================
StatusCode CondDBAccessSvc::cacheAddObject(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                           const cool::IRecord& payload, cool::ChannelId channel) {
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
  cool::Record payload(CondDB::getXMLStorageSpec());
  payload["data"].setValue<cool::String16M>(data);
  return cacheAddObject(path,since,until,payload,channel);
}

//=========================================================================
//
//=========================================================================
StatusCode CondDBAccessSvc::cacheAddXMLData(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                            const std::map<std::string,std::string> &data, cool::ChannelId channel) {
  cool::RecordSpecification spec;
  for (std::map<std::string,std::string>::const_iterator d = data.begin(); d != data.end(); ++d ){
    spec.extend(d->first,cool::StorageType::String16M);
  }

  cool::Record payload(spec);

  for (std::map<std::string,std::string>::const_iterator d = data.begin(); d != data.end(); ++d ){
    payload[d->first].setValue<cool::String16M>(d->second);
  }
  return cacheAddObject(path,since,until,payload,channel);
}

//=========================================================================
//
//=========================================================================
void CondDBAccessSvc::clearCache()
{
  if (m_useCache) {
    m_cache->clear();
  }
}

//=========================================================================
//
//=========================================================================
void CondDBAccessSvc::dumpCache() const {
  if (m_useCache) m_cache->dump();
}

//=========================================================================
//
//=========================================================================
void CondDBAccessSvc::i_generateXMLCatalogFromFolderset(const std::string &path){
  // Use the name of the folderset as catalog name.
  std::string::size_type pos = path.rfind('/');
  if ( std::string::npos == pos ) {
    pos = 0; // if we cannot find '/' let's take the whole string
  } else {
    ++pos;
  }
  std::string folderset_name = path.substr(pos);

  // Get the names of sub-folders and sub-foldersets
  std::vector<std::string> fldr_names, fldrset_names;
  getChildNodes(path, fldr_names, fldrset_names).ignore();

  std::string xml;
  CondDB::generateXMLCatalog(folderset_name,fldr_names,fldrset_names,xml);

  // Put the data in the cache
  if ( ! m_cache->hasPath(path) )
    cacheAddXMLFolder(path);

  // This is needed because we cannot add objects valid for the current event
  // to the cache using the ICondDBAccessSvc API.
  bool check_enabled = m_cache->setIOVCheck(false);
  cacheAddXMLData(path,Gaudi::Time::epoch(),Gaudi::Time::max(),xml,0).ignore();
  m_cache->setIOVCheck(check_enabled);

}
