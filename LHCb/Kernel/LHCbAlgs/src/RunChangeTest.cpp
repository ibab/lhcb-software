// $Id: RunChangeTest.cpp,v 1.1 2009-02-03 18:31:03 marcocle Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
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
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCbAlgsTests,RunChangeTest);

using namespace LHCb;
using namespace LHCbAlgsTests;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RunChangeTest::RunChangeTest( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
  m_runNumber(0),
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

  ODIN* odin = new ODIN();
  odin->setRunNumber(++m_runNumber);
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
  info() << "Run number:" << get<ODIN>(ODINLocation::Default)->runNumber() << endmsg;
}

//=============================================================================
