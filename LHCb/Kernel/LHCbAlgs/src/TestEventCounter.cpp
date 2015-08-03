// Include files

// local
#include "TestEventCounter.h"

using namespace LHCbAlgsTests;

// ----------------------------------------------------------------------------
// Implementation file for class: TestEventCounter
//
// 11/06/2012: Marco Clemencic
// ----------------------------------------------------------------------------
DECLARE_ALGORITHM_FACTORY(TestEventCounter)

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
TestEventCounter::TestEventCounter(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator), m_eventCounter(NULL)
{
  declareProperty("EvtCounter", m_eventCounterName = "EvtCounter",
                  "Type/Name of the (public) event counter to use.");
}

// ============================================================================
// Destructor
// ============================================================================
TestEventCounter::~TestEventCounter() {}

// ============================================================================
// Initialization
// ============================================================================
StatusCode TestEventCounter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_eventCounter = tool<IEventCounter>(m_eventCounterName);

  return StatusCode::SUCCESS;
}

// ============================================================================
// Main execution
// ============================================================================
StatusCode TestEventCounter::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  info() << "Event count = " << m_eventCounter->getEventCounter() << endmsg;

  return StatusCode::SUCCESS;
}

// ============================================================================
// Finalize
// ============================================================================
StatusCode TestEventCounter::finalize() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  release(m_eventCounter);
  m_eventCounter = NULL;

  return GaudiAlgorithm::finalize(); // must be called after all other actions
}

// ============================================================================
