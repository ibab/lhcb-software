// Include files

// From Gaudi
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/Time.h"

// local
#include "PrintEventTime.h"

// ----------------------------------------------------------------------------
// Implementation file for class: PrintEventTime
//
// 03/10/2011: Marco Clemencic
// ----------------------------------------------------------------------------
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCbAlgsTest, PrintEventTime)

namespace LHCbAlgsTest {

  // ============================================================================
// Standard constructor, initializes variables
// ============================================================================
PrintEventTime::PrintEventTime(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
{

}

// ============================================================================
// Destructor
// ============================================================================
PrintEventTime::~PrintEventTime() {}

// ============================================================================
// Initialization
// ============================================================================
StatusCode PrintEventTime::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_detDataSvc = detSvc();

  return StatusCode::SUCCESS;
}

// ============================================================================
// Main execution
// ============================================================================
StatusCode PrintEventTime::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  info() << "Current event time: " << m_detDataSvc->eventTime() << endmsg;

  return StatusCode::SUCCESS;
}

// ============================================================================
// Finalize
// ============================================================================
StatusCode PrintEventTime::finalize() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  m_detDataSvc.reset();

  return GaudiAlgorithm::finalize(); // must be called after all other actions
}

}
// ============================================================================
