#include "GaudiKernel/AlgFactory.h"
#include "Event/LumiCounters.h"

#include "DumpLumiEvents.h"

#include <sstream>

DECLARE_ALGORITHM_FACTORY(DumpLumiEvents);

DumpLumiEvents::DumpLumiEvents(const std::string& name,
    ISvcLocator* pSvcLocator) :
    GaudiAlgorithm(name, pSvcLocator) {
  declareProperty("OutputFileName", m_outputFileName);
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

void DumpLumiEvents::outputLumiSummary(std::ostream& s,
    LHCb::HltLumiSummary* hltLumiSummary) {
  LHCb::HltLumiSummary::ExtraInfo summaryInfo = hltLumiSummary->extraInfo();
  LHCb::HltLumiSummary::ExtraInfo::iterator summaryIter;
  for (summaryIter = summaryInfo.begin(); summaryIter != summaryInfo.end();
      ++summaryIter) {
    int key = summaryIter->first;
    int value = summaryIter->second;
    s << key << "," << value << ",";
  }
  s << ";";
}

StatusCode DumpLumiEvents::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if (sc.isFailure())
    return sc; // error printed already by GaudiAlgorithm
  if (msgLevel(MSG::DEBUG))
    debug() << "==> Initialize" << endmsg;

  m_output.open(m_outputFileName.c_str());
  if (m_output.fail())
    return Error("Could not open output file", StatusCode::FAILURE);

  return StatusCode::SUCCESS;
}

StatusCode DumpLumiEvents::execute() {
  if (msgLevel(MSG::DEBUG))
    debug() << "==> Execute" << endmsg;

  if (!exist<LHCb::ODIN>(LHCb::ODINLocation::Default))
    return Error("ODIN cannot be loaded", StatusCode::FAILURE);

  if (!exist<LHCb::HltLumiSummary>(LHCb::HltLumiSummaryLocation::Default))
    return Error("LumiSummary cannot be loaded", StatusCode::FAILURE);

  LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);
  LHCb::HltLumiSummary* hltLumiSummary = get<LHCb::HltLumiSummary>(
      LHCb::HltLumiSummaryLocation::Default);

  // interested only in events with random counter different from 0
  unsigned int random = hltLumiSummary->info(LHCb::LumiCounters::Random, 0);
  if (random != 0) {
    std::ostringstream oss;
    outputOdin(oss, odin);
    outputLumiSummary(oss, hltLumiSummary);
    m_output << oss.str() << '\n';
    if (msgLevel(MSG::DEBUG))
      debug() << oss.str() << endmsg;
  }

  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

StatusCode DumpLumiEvents::finalize() {
  if (msgLevel(MSG::DEBUG))
    debug() << "==> Finalize" << endmsg;

  m_output.close();

  return GaudiAlgorithm::finalize(); // must be called after all other actions
}
