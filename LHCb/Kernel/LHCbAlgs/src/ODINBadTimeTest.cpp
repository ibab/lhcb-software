// Include files

// From Gaudi
#include "GaudiKernel/IEventTimeDecoder.h"

// From LHCb
#include "Event/ODIN.h"

// local
#include "ODINBadTimeTest.h"

// ----------------------------------------------------------------------------
// Implementation file for class: ODINBadTimeTest
//
// 18/10/2011: Marco Clemencic
// ----------------------------------------------------------------------------
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCbAlgsTests, ODINBadTimeTest)

namespace LHCbAlgsTests {
// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
ODINBadTimeTest::ODINBadTimeTest(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
  , m_evtTimeTool(0)
{
}

// ============================================================================
// Destructor
// ============================================================================
ODINBadTimeTest::~ODINBadTimeTest() {}

// ============================================================================
// Initialization
// ============================================================================
StatusCode ODINBadTimeTest::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_evtTimeTool = tool<IEventTimeDecoder>("OdinTimeDecoder", this, true);

  return StatusCode::SUCCESS;
}

// ============================================================================
// Main execution
// ============================================================================
StatusCode ODINBadTimeTest::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  static bool called = false;
  LHCb::ODIN* odin = new LHCb::ODIN();
  if (!called) {
    // valid GPS time on the first call
    odin->setGpsTime(1318944600000000ULL); // 18/10/2011 15:30
  } else {
    // invalid GPS time on the following calls
    odin->setGpsTime(9239754600000000ULL); // 18/10/2262 15:30
  }
  put(evtSvc(), odin, LHCb::ODINLocation::Default);

  info() << "Set ODIN time to " << odin->gpsTime() / 1000000
         << " (" << (called ? "invalid" : "valid") << ")" << endmsg;
  // will not try to decode the ODIN bank
  info() << "ODIN time -> " << m_evtTimeTool->getTime() << endmsg;

  called = true;

  return StatusCode::SUCCESS;
}

// ============================================================================
// Finalize
// ============================================================================
StatusCode ODINBadTimeTest::finalize() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  releaseTool(m_evtTimeTool);
  m_evtTimeTool = 0;

  return GaudiAlgorithm::finalize(); // must be called after all other actions
}

// ============================================================================
}
