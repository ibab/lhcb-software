// Include files
#ifdef WIN32 // Hacks to compile on Windows...
#define NOMSG
#define NOGDI
#define max max
#endif

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Time.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IDetDataSvc.h"

// local
#include "CondDBTimeSwitchSvc.h"
#include "CondDBCommon.h"


// Factory implementation
DECLARE_SERVICE_FACTORY(CondDBTimeSwitchSvc)

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBTimeSwitchSvc
//
// 2006-07-10 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

// This is needed otherwise the implementation of std::map does
// not find operator<(Gaudi::Time,Gaudi::Time).
namespace Gaudi { using ::operator<; }
//=============================================================================
// Code copied from GaudiKernel Parsers, to have a parser for
// pair<long long,long long>.
// ============================================================================
// GaudiKernel
// ============================================================================
// 2011-08-26 : alexander.mazurov@gmail.com
#include "GaudiKernel/ParsersFactory.h"
namespace {
  StatusCode parse(std::pair<long long,long long>& result,
      const std::string& input){
    return Gaudi::Parsers::parse_(result, input);
  }
}
//=============================================================================

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBTimeSwitchSvc::CondDBTimeSwitchSvc( const std::string& name, ISvcLocator* svcloc ):
  base_class(name,svcloc),
  m_readersDeclatations(),
  m_readers(),
  m_latestReaderRequested(0),
  m_dds(0)
{

  // "'CondDBReader':(since, until)", with since and until doubles
  declareProperty("Readers", m_readersDeclatations);

  declareProperty("EnableXMLDirectMapping", m_xmlDirectMapping = true,
                  "Allow direct mapping from CondDB structure to"
                  " transient store.");
}

//=============================================================================
// Destructor
//=============================================================================
CondDBTimeSwitchSvc::~CondDBTimeSwitchSvc() {}

//=============================================================================
// initialize
//=============================================================================
StatusCode CondDBTimeSwitchSvc::initialize(){
  StatusCode sc = base_class::initialize();
  if (sc.isFailure()) return sc;

  MsgStream log(msgSvc(), name() );
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Initialize" << endmsg;

  if (m_readersDeclatations.empty()) {
    log << MSG::ERROR << "No CondDBReader has been specified"
                         " (property 'Readers')." << endmsg;
    return StatusCode::FAILURE;
  }

  // decoding the property "Readers"
  std::string reader_name, reader_siov;
  std::pair<long long,long long> reader_iov;
  for (ReadersDeclatationsType::iterator rd = m_readersDeclatations.begin();
       rd != m_readersDeclatations.end(); ++rd){
    // first step of parsing (split "'name':value" -> "name","value")
    sc = Gaudi::Parsers::parse(reader_name,reader_siov,*rd);
    if (sc.isSuccess()) {
      // second step (only if first passed)
      sc = ::parse(reader_iov,reader_siov);
    }
    if (sc.isFailure()){
      log << MSG::ERROR << "Cannot decode string '" << *rd << "'" << endmsg;
      return sc;
    }
    // Check for overlaps
    const bool quiet = true;
    ReaderInfo *old = readerFor(reader_iov.first,quiet);
    if (!old) old = readerFor(reader_iov.second,quiet);
    if (old) {
      log << MSG::ERROR << "Conflicting IOVs between '" << old->name  << "':("
          << old->since.ns() << "," << old->until.ns() << ") and "
          << *rd << endmsg;
      return StatusCode::FAILURE;
    }
    // use "until" as key to be able to search with "upper_bound"
    ReaderInfo ri(reader_name, reader_iov.first, reader_iov.second);
    m_readers.insert(std::make_pair(ri.until,ri));
  }
  if( UNLIKELY( outputLevel() <= MSG::DEBUG ) ) {
    log << MSG::DEBUG << "Configured CondDBReaders:" << endmsg;
    ReadersType::iterator r;
    for (r = m_readers.begin(); r != m_readers.end(); ++r) {
      log << MSG::DEBUG << " " << r->second.since << " - " << r->second.until
          << ": " << r->second.name << endmsg;
    }
  }
  // we need to reset it because it got corrupted during the
  // check for overlaps
  m_latestReaderRequested = 0;

  return sc;
}

//=============================================================================
// finalize
//=============================================================================
StatusCode CondDBTimeSwitchSvc::finalize(){
  if( UNLIKELY( outputLevel() <= MSG::DEBUG ) ) {
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "Finalize" << endmsg;
  }

  // release all the loaded CondDBReader services
  m_readers.clear();
  if(m_dds) {
    m_dds->release();
    m_dds = 0;
  }
  m_readersDeclatations.clear();
  m_latestReaderRequested = 0;

  return base_class::finalize();
}

//=========================================================================
//  find the appropriate reader
//=========================================================================
CondDBTimeSwitchSvc::ReaderInfo *CondDBTimeSwitchSvc::readerFor(const Gaudi::Time &when, bool quiet) {
  MsgStream log(msgSvc(), name());

  if (!quiet) log << MSG::VERBOSE << "Get CondDBReader for event time " << when << endmsg;

  // TODO: (MCl) if we change service, we may clear the cache of the one
  //       that is not needed.
  if ((!m_latestReaderRequested) || !m_latestReaderRequested->isValidAt(when)){
    // service not valid: search for the correct one

    // Find the element with key ("until") greater that the requested one
    ReadersType::iterator reader = m_readers.upper_bound(when);
    if (reader != m_readers.end() && reader->second.isValidAt(when)){
      m_latestReaderRequested = &(reader->second);
    } else {
      m_latestReaderRequested = 0; // reader not found
      if (!quiet) log << MSG::WARNING << "No reader configured for requested event time" << endmsg;
    }
  }

  return m_latestReaderRequested;
}

//=========================================================================
//  get the current time
//=========================================================================
Gaudi::Time CondDBTimeSwitchSvc::getTime() {
  if (!m_dds) {
    StatusCode sc = service("DetectorDataSvc",m_dds,false);
    if (sc.isFailure()) {
      MsgStream log(msgSvc(), name());
      log << MSG::WARNING << "Cannot find the DetectorDataSvc,"
                             " using a default event time (0)" << endmsg;
      return Gaudi::Time();
    }
  }
  return m_dds->eventTime();
}

//=========================================================================
//  get the current reader
//=========================================================================
CondDBTimeSwitchSvc::ReaderInfo *CondDBTimeSwitchSvc::currentReader() {
  if (!m_latestReaderRequested) {
    // Let's stay on the safe side if we don't find a reader
    if (readerFor(getTime()) == 0) {
      throw GaudiException("No reader configured for current event time",
                           "CondDBTimeSwitchSvc::currentReader",StatusCode::FAILURE);
    }
  }
  return m_latestReaderRequested;
}

//=========================================================================
//  retrieve an object
//=========================================================================
StatusCode CondDBTimeSwitchSvc::getObject (const std::string &path,
                                           const Gaudi::Time &when,
                                           DataPtr &data,
                                           std::string &descr,
                                           Gaudi::Time &since,
                                           Gaudi::Time &until,
                                           cool::ChannelId channel) {
  // get the reader for the requested time
  ReaderInfo *ri = readerFor(when);
  if (!ri) return StatusCode::FAILURE;

  StatusCode sc;
  if (m_xmlDirectMapping && isFolderSet(path)) {
    descr = "Catalog generated automatically by " + name();
    since = Gaudi::Time::epoch();
    until = Gaudi::Time::max();
    sc = CondDB::generateXMLCatalog(this,path,data);
  } else {
    sc = ri->reader(serviceLocator())->getObject(path,when,data,descr,since,until,channel);
    if (sc.isSuccess()) ri->cutIOV(since,until);
  }
  return sc;
}
StatusCode CondDBTimeSwitchSvc::getObject (const std::string &path,
                                           const Gaudi::Time &when,
                                           DataPtr &data,
                                           std::string &descr,
                                           Gaudi::Time &since,
                                           Gaudi::Time &until,
                                           const std::string &channel) {
  // get the reader for the requested time
  ReaderInfo *ri = readerFor(when);
  if (!ri) return StatusCode::FAILURE;

  StatusCode sc;
  if (m_xmlDirectMapping && isFolderSet(path)) {
    descr = "Catalog generated automatically by " + name();
    since = Gaudi::Time::epoch();
    until = Gaudi::Time::max();
    sc = CondDB::generateXMLCatalog(this,path,data);
  } else {
    sc = ri->reader(serviceLocator())->getObject(path,when,data,descr,since,until,channel);
    if (sc.isSuccess()) ri->cutIOV(since,until);
  }
  return sc;

}

template <typename Channel>
ICondDBReader::IOVList CondDBTimeSwitchSvc::i_getIOVs(const std::string & path, const IOV &iov, const Channel &channel)
{
  IOVList iovs;

  IOV tmp;

  // get the list of readers valid in the given time IOV
  ReadersType::iterator r;
  for(r = m_readers.begin(); r != m_readers.end(); ++r) {
    if (r->second.until <= iov.since) continue; // ignore readers before...
    if (r->second.since >= iov.until) break; // ...and after the request

    tmp.since = std::max(iov.since, r->second.since);
    tmp.until = std::min(iov.until, r->second.until);

    IOVList new_iovs = r->second.reader(serviceLocator())->getIOVs(path, tmp, channel);
    if (!new_iovs.empty()) {
      // trim the IOVs found
      new_iovs.front().since = std::max(tmp.since, new_iovs.front().since);
      new_iovs.back().until = std::min(tmp.until, new_iovs.back().until);

      iovs.insert(iovs.end(), new_iovs.begin(), new_iovs.end());
    }
  }
  return iovs;
}

ICondDBReader::IOVList CondDBTimeSwitchSvc::getIOVs(const std::string & path, const IOV &iov, cool::ChannelId channel)
{
  return i_getIOVs(path, iov, channel);
}

ICondDBReader::IOVList CondDBTimeSwitchSvc::getIOVs(const std::string & path, const IOV &iov, const std::string & channel)
{
  return i_getIOVs(path, iov, channel);
}

//=========================================================================
//  get the list of child nodes of a folderset
//=========================================================================
StatusCode CondDBTimeSwitchSvc::getChildNodes (const std::string &path, std::vector<std::string> &node_names) {
  return getChildNodes(path,node_names,node_names);
}

//=========================================================================
//  get the list of child nodes of a folderset
//=========================================================================
StatusCode CondDBTimeSwitchSvc::getChildNodes (const std::string &path,
                                               std::vector<std::string> &folders,
                                               std::vector<std::string> &foldersets) {
  // clear the destination vectors
  folders.clear();
  foldersets.clear();

  // delegate to the current Reader (hoping that is good enough)
  return currentReader()->reader(serviceLocator())->getChildNodes(path,folders,foldersets);
}

//=========================================================================
// Tells if the path is available in the database.
//=========================================================================
bool CondDBTimeSwitchSvc::exists(const std::string &path) {
  return currentReader()->reader(serviceLocator())->exists(path);
}

//=========================================================================
// Tells if the path (if it exists) is a folder.
//=========================================================================
bool CondDBTimeSwitchSvc::isFolder(const std::string &path) {
  return currentReader()->reader(serviceLocator())->isFolder(path);
}

//=========================================================================
// Tells if the path (if it exists) is a folderset.
//=========================================================================
bool CondDBTimeSwitchSvc::isFolderSet(const std::string &path) {
  return currentReader()->reader(serviceLocator())->isFolderSet(path);
}

//=========================================================================
// Force disconnection from database.
//=========================================================================
void CondDBTimeSwitchSvc::disconnect() {
  // loop over all readers
  ReadersType::const_iterator reader;
  for ( reader = m_readers.begin(); reader != m_readers.end(); ++reader ) {
    if (reader->second.loaded())
      reader->second.reader(serviceLocator())->disconnect();
  }
}

//=========================================================================
// Collect the list of used tags and databases
//=========================================================================
void CondDBTimeSwitchSvc::defaultTags ( std::vector<LHCb::CondDBNameTagPair>& tags ) const {
  // loop over all readers
  ReadersType::const_iterator reader;
  for ( reader = m_readers.begin(); reader != m_readers.end(); ++reader ) {
    reader->second.reader(serviceLocator())->defaultTags(tags);
  }
}

//=============================================================================
