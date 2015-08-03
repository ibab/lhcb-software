// $Id: RunChangeTest.cpp,v 1.1 2009-02-03 18:31:03 marcocle Exp $
// Include files

// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IEventTimeDecoder.h"

#include "Event/ODIN.h"

// local
#include "RunChangeTest.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RunChangeTest
//
// 2008-07-24 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCbAlgsTests,RunChangeTest)

using namespace LHCb;
using namespace LHCbAlgsTests;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RunChangeTest::RunChangeTest( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
  m_counter(0),
  m_eventTimeDecoder(NULL),
  m_incSvc(NULL)
{

}
//=============================================================================
// Destructor
//=============================================================================
RunChangeTest::~RunChangeTest() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RunChangeTest::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  m_eventTimeDecoder = tool<IEventTimeDecoder>("OdinTimeDecoder",this,true);

  m_incSvc = svc<IIncidentSvc>("IncidentSvc", true);
  m_incSvc->addListener(this, "RunChange");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RunChangeTest::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  std::vector<std::string> test_cases {
    "Define initial condition.",
    "Test: same run, flagging -> filtering",               // 1->2
    "Test: new run,  always filtering",                    // 2->3
    "Test: new run,  filtering -> flagging",               // 3->4
    "Test: new run,  always flagging",                     // 4->5
    "Test: new run,  flagging -> filtering",               // 5->6
    "Test: same run, filtering -> flagging (no trigger)",  // 6->7 FIXME: correct?
    "Test: same run, filtering (stable, no trigger)",      // 7->
  };

  ODIN* odin = new ODIN();
  info() << test_cases[std::min(m_counter, test_cases.size()-1)] << endmsg;
  switch (++m_counter) {
  case 1: // run 1, flagging
    odin->setRunNumber(1);
    odin->setEventType(ODIN::FlaggingModeMask);
    break;
  case 2: // run 1, filtering
    odin->setRunNumber(1);
    odin->setEventType(0x0000);
    break;
  case 3: // run 2, filtering
    odin->setRunNumber(2);
    odin->setEventType(0x0000);
    break;
  case 4: // run 3, flagging
    odin->setRunNumber(3);
    odin->setEventType(ODIN::FlaggingModeMask);
    break;
  case 5: // run 4, flagging
    odin->setRunNumber(4);
    odin->setEventType(ODIN::FlaggingModeMask);
    break;
  case 6: // run 5, filtering
    odin->setRunNumber(5);
    odin->setEventType(0x0000);
    break;
  case 7: // run 5, flagging
    odin->setRunNumber(5);
    odin->setEventType(ODIN::FlaggingModeMask);
    break;
  default: // run 5, filtering
    odin->setRunNumber(5);
    odin->setEventType(0x0000); // FIXME
    break;
  }

  put(evtSvc(), odin, ODINLocation::Default);

  // will not try to decode the ODIN bank, but issue a ChangeRun
  m_eventTimeDecoder->getTime();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RunChangeTest::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  m_eventTimeDecoder = 0; // released in the base class
  if (m_incSvc){
    m_incSvc->removeListener(this, "RunChange");
  }

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
//  Incident handling
//=============================================================================
void RunChangeTest::handle(const Incident& incident) {
  info() << incident.type() << " incident received from " << incident.source()
         << endmsg;
  ODIN* odin = get<ODIN>(ODINLocation::Default);
  info() << "Run " << odin->runNumber() << ", "
         << (odin->isFlagging() ? "flagging" : "filtering") << endmsg;
}

//=============================================================================
