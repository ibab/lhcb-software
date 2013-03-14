// Include files

// From Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Incident.h"

// From LHCb
#include "Event/RecHeader.h"

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

  // pretend we are opening files
  if (m_eventNumber == 0) {
    info() << "Pretend opening file 'first_file.StripFake.dst'" << endmsg;
    m_incSvc->fireIncident(Incident("first_file.StripFake.dst", IncidentType::BeginInputFile));
  }
  else if (m_eventNumber == 10) {
    info() << "Pretend opening file 'second_file.StripFake.dst'" << endmsg;
    m_incSvc->fireIncident(Incident("second_file.StripFake.dst", IncidentType::BeginInputFile));
  }

  // generate and store a RecHeader
  LHCb::RecHeader *rh = new LHCb::RecHeader();
  rh->setRunNumber(123456);
  rh->setEvtNumber(++m_eventNumber);
  put(rh, LHCb::RecHeaderLocation::Default);
  info() << "Run " << rh->runNumber() << ", Event " << rh->evtNumber() << endmsg;

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
