#include "Event/LumiCounters.h"

#include "DumpLumiEvents.h"

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <sstream>

DECLARE_ALGORITHM_FACTORY(DumpLumiEvents)

inline bool hasEnding(const std::string& s, const std::string& ending);
inline double round(double x, double factor);
inline bool comparePairs(const KeyLocationPair& x, const KeyLocationPair& y);

DumpLumiEvents::DumpLumiEvents(const std::string& name, ISvcLocator* pSvcLocator) :
    GaudiAlgorithm(name, pSvcLocator)
    , m_intermediateStream(NULL)
    , m_outputStream(NULL)
{
  declareProperty("Sort", m_sort=true);
  declareProperty("OutputFileName", m_outputFileName);
  declareProperty("RecVerticesLocation", m_recVerticesLocation);
  declareProperty("IntermediateStreamFileName", m_intermediateStreamFileName);
  declareProperty("NEventsHint", m_nEventsHint=4096000);
}

DumpLumiEvents::~DumpLumiEvents() {
}

void DumpLumiEvents::outputOdin(std::ostream& s, LHCb::ODIN* odin) {
  s << odin->version() << ","
      << odin->runNumber() << ","
      << odin->calibrationStep() << ","
      << odin->eventType() << ","
      << odin->orbitNumber() << ","
      << odin->eventNumber() << ","
      << odin->gpsTime() << ","
      << odin->errorBits() << ","
      << odin->detectorStatus() << ","
      << odin->bunchCurrent() << ","
      << (int) (odin->bunchCrossingType()) << ","
      << odin->forceBit() << ","
      << (int) (odin->calibrationType()) << ","
      << (int) (odin->triggerType()) << ","
      << odin->timeAlignmentEventWindow() << ","
      << odin->bunchId() << ","
      << ";";
}

void DumpLumiEvents::outputLumiSummary(std::ostream& s, LHCb::HltLumiSummary* hltLumiSummary) {
  LHCb::HltLumiSummary::ExtraInfo summaryInfo = hltLumiSummary->extraInfo();
  LHCb::HltLumiSummary::ExtraInfo::const_iterator it;
  for (it = summaryInfo.begin(); it != summaryInfo.end(); ++it) {
    int key = it->first;
    int value = it->second;
    s << key << "," << value << ",";
  }
  s << ";";
}

void DumpLumiEvents::outputVertices(std::ostream& s, LHCb::RecVertices* recVertices) {
  LHCb::RecVertices::const_iterator it;
  for (it = recVertices->begin(); it != recVertices->end(); ++it) {
    LHCb::RecVertex* vx = *it;
    s << round(vx->position().x(), 1e5) << ","
        << round(vx->position().y(), 1e5) << ","
        << round(vx->position().z(), 1e5) << ","
        << vx->tracks().size() << ","
        << round(vx->chi2(), 1e2) << ',';
  }
  s << ";";
}

StatusCode DumpLumiEvents::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if (sc.isFailure())
    return sc; // error printed already by GaudiAlgorithm
  if (msgLevel(MSG::DEBUG))
    debug() << "==> Initialize" << endmsg;

  m_dumpVertices = !m_recVerticesLocation.empty();
  m_compressed = hasEnding(m_outputFileName, ".bz2");

  // Open output file
  std::ios::openmode mode = std::ios::out | std::ios::trunc;
  if (m_compressed) mode |= std::ios::binary;
  m_outputFile.open(m_outputFileName.c_str(), mode);
  if (m_outputFile.fail())
    return Error("Could not open output file", StatusCode::FAILURE);

  // Create filtering stream with or without compression
  boost::iostreams::filtering_ostream* fs = new boost::iostreams::filtering_ostream();
  if (m_compressed)
    fs->push(boost::iostreams::bzip2_compressor());
  fs->push(m_outputFile);
  m_outputStream = fs;

  // Setup intermediate stream
  if (m_sort) {
    if (m_intermediateStreamFileName.empty())
      m_intermediateStream = new std::stringstream();
    else {
      m_intermediateStream = new std::fstream(m_intermediateStreamFileName.c_str(),
          std::fstream::in | std::fstream::out | std::fstream::trunc);
      if (m_intermediateStream->fail())
        return Error("Could not open file for the intermediate stream.", StatusCode::FAILURE);
    }

    m_keyLocationPairs.reserve(m_nEventsHint);
    info() << "Reserved capacity for " << m_nEventsHint << " elements in key-location pairs vector." << endmsg;
  }
  else {
    m_intermediateStream = m_outputStream;
  }

  return StatusCode::SUCCESS;
}

StatusCode DumpLumiEvents::execute() {
  if (msgLevel(MSG::DEBUG))
    debug() << "==> Execute" << endmsg;

  if (!exist<LHCb::ODIN>(LHCb::ODINLocation::Default))
    return Error("ODIN cannot be loaded", StatusCode::FAILURE);
  if (!exist<LHCb::HltLumiSummary>(LHCb::HltLumiSummaryLocation::Default))
    return Error("LumiSummary cannot be loaded", StatusCode::FAILURE);
  if (m_dumpVertices && !exist<LHCb::RecVertices>(m_recVerticesLocation))
    return Error("RecVertices cannot be loaded", StatusCode::FAILURE);

  LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);
  LHCb::HltLumiSummary* hltLumiSummary = get<LHCb::HltLumiSummary>(LHCb::HltLumiSummaryLocation::Default);
  LHCb::RecVertices* recVertices = m_dumpVertices?get<LHCb::RecVertices>(m_recVerticesLocation):NULL;

  // interested only in events with random counter different from 0
  unsigned int random = hltLumiSummary->info(LHCb::LumiCounters::Random, 0);
  if (random != 0) {
    if (LIKELY(m_sort))
      m_keyLocationPairs.push_back(KeyLocationPair(odin->gpsTime(), m_intermediateStream->tellp()));

    outputOdin(*m_intermediateStream, odin);
    outputLumiSummary(*m_intermediateStream, hltLumiSummary);
    if (m_dumpVertices)
      outputVertices(*m_intermediateStream, recVertices);
    (*m_intermediateStream) << '\n';

    if (UNLIKELY(msgLevel(MSG::DEBUG))) {
      std::ostringstream oss;
      outputOdin(oss, odin);
      outputLumiSummary(oss, hltLumiSummary);
      if (m_dumpVertices)
        outputVertices(oss, recVertices);
      debug() << oss.str() << endmsg;
    }
  }

  setFilterPassed(random != 0);
  return StatusCode::SUCCESS;
}

StatusCode DumpLumiEvents::finalize() {
  if (msgLevel(MSG::DEBUG))
    debug() << "==> Finalize" << endmsg;

  if (m_sort) {
    std::sort(m_keyLocationPairs.begin(), m_keyLocationPairs.end(), comparePairs);
    // insertion sort is good for almost sorted data,
    // but in our case performance is similar to std::sort()
    //insertionSort();

    std::istream* is = dynamic_cast<std::iostream*>(m_intermediateStream);

    std::vector<KeyLocationPair>::const_iterator it;
    for (it = m_keyLocationPairs.begin(); it != m_keyLocationPairs.end(); ++it) {
      is->seekg(it->location);
      is->get(*(m_outputStream->rdbuf()));
      m_outputStream->put('\n');
      if (UNLIKELY(msgLevel(MSG::DEBUG))) {
        is->seekg(it->location);
        is->get(*(debug().stream().rdbuf()));
        debug() << endmsg;
      }
    }

    delete m_intermediateStream;
  }

  delete m_outputStream; // closes m_outputFile too

  return GaudiAlgorithm::finalize(); // must be called after all other actions
}

void DumpLumiEvents::insertionSort() {
  ulonglong tkey;
  int n = m_keyLocationPairs.size();
  int j;
  KeyLocationPair tpair(0,0);
  for (int i = 1; i < n; i++) {
    tpair = m_keyLocationPairs[i];
    tkey = tpair.key;
    for (j = i-1; j >= 0; j--) {
      if (tkey < m_keyLocationPairs[j].key)
        m_keyLocationPairs[j+1] = m_keyLocationPairs[j];
      else
        break;
    }
    m_keyLocationPairs[j+1] = tpair;
  }
}

inline bool hasEnding(const std::string& s, const std::string& ending) {
  if (s.length() >= ending.length())
    return (0 == s.compare(s.size() - ending.size(), ending.size(), ending));
  else
    return false;
}

inline double round(double x, double factor) {
  return int(x * factor) / factor;
}

inline bool comparePairs(const KeyLocationPair& x, const KeyLocationPair& y) {
  return (x.key < y.key);
}
