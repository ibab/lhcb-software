// Include files

// From Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Incident.h"

// From LHCb
#include "Event/RecHeader.h"
#include "Event/RecSummary.h"
#include "Event/HltDecReports.h"

// local
#include "FakeIndexerData.h"

using namespace EventIndexerTest;

// ----------------------------------------------------------------------------
// Implementation file for class: FakeIndexerData
//
// 14/03/2013: Marco Clemencic
// ----------------------------------------------------------------------------
DECLARE_ALGORITHM_FACTORY(FakeIndexerData)

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
FakeIndexerData::FakeIndexerData(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator), m_eventNumber(0)
{
  m_files.push_back("first_file.FakeStream.dst");
  m_files.push_back("second_file.OtherStream.dst");
  declareProperty("FakeInputFiles", m_files,
                  "Names of the input files we pretend we are reading "
                  " (10 events each).");
}

// ============================================================================
// Destructor
// ============================================================================
FakeIndexerData::~FakeIndexerData() {}

// ============================================================================
// Initialization
// ============================================================================
StatusCode FakeIndexerData::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

  m_eventNumber = 0;

  m_incSvc = service("IncidentSvc");
  if (!m_incSvc) return StatusCode::FAILURE;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  return StatusCode::SUCCESS;
}

// ============================================================================
// Main execution
// ============================================================================
StatusCode FakeIndexerData::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  if ((m_eventNumber % 10) == 0) {
    size_t idx = m_eventNumber / 10;
    if (idx < m_files.size()) {
      info() << "Pretend opening file '" << m_files[idx] << "'" << endmsg;
      m_incSvc->fireIncident(Incident(m_files[idx], IncidentType::BeginInputFile));
    }
  }

  // generate and store a RecHeader
  LHCb::RecHeader *rh = new LHCb::RecHeader();
  rh->setRunNumber(123456);
  rh->setEvtNumber(++m_eventNumber);
  put(rh, LHCb::RecHeaderLocation::Default);
  info() << "Run " << rh->runNumber() << ", Event " << rh->evtNumber() << endmsg;

  LHCb::RecSummary *rsummary = new LHCb::RecSummary();
  rsummary->addInfo(LHCb::RecSummary::nITClusters, 1);
  put(rsummary, LHCb::RecSummaryLocation::Default);

  LHCb::HltDecReports *rreports = new LHCb::HltDecReports();
  put(rreports, "Strip/Phys/DecReports");

  return StatusCode::SUCCESS;
}

// ============================================================================
// Finalize
// ============================================================================
StatusCode FakeIndexerData::finalize() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  m_incSvc.reset();

  return GaudiAlgorithm::finalize(); // must be called after all other actions
}

// ============================================================================
