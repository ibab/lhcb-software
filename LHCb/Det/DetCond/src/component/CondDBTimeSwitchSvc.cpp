// Include files
#ifdef WIN32 // Hacks to compile on Windows...
#define NOMSG
#define NOGDI
#define max max
#endif

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Time.h"
#include "GaudiKernel/Parsers.h"
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
//=============================================================================
// Code copied from GaudiKernel Parsers, to have a parser for
// pair<long long,long long>.

// ============================================================================
// Boost
// ============================================================================
#include "boost/bind.hpp"

#include <boost/version.hpp>
#if BOOST_VERSION >= 103800
#define BOOST_SPIRIT_USE_OLD_NAMESPACE
#include <boost/spirit/include/classic.hpp>
#include "boost/spirit/include/phoenix1.hpp"
#else
#include <boost/spirit.hpp>
#include "boost/spirit/phoenix.hpp"
#endif

// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Parsers.h"
#include "GaudiKernel/Grammars.h"
namespace {
  using namespace std;
  using namespace boost::spirit;
  using namespace Gaudi::Parsers;

  /// the actual type of position iterator
  typedef boost::spirit::position_iterator<string::const_iterator> IteratorT;

  /// create the position iterator from the inptut
  inline IteratorT createIterator(const std::string& input){
    return IteratorT(input.begin(), input.end());
  }
  StatusCode parse(pair<long long,long long>& result, const string& input){
    return parse
    ( createIterator(input),
        IteratorT(),
        PairGrammar < IntGrammar<long long> , IntGrammar <long long> >()[var(result)=arg1],
        SkipperGrammar()).full;
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
  if (outputLevel() <= MSG::DEBUG) {
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
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Finalize" << endmsg;

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
