// Include files
#include <algorithm>

// local
#include "ServiceStarter.h"

using namespace LHCbAlgsTest;

// ----------------------------------------------------------------------------
// Implementation file for class: ServiceStarter
//
// 09/01/2012: Marco Clemencic
// ----------------------------------------------------------------------------
DECLARE_ALGORITHM_FACTORY(ServiceStarter)

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
namespace {
  const std::vector<std::string> phases = {"initialize", "start", "execute"};
}
#else
#include "boost/assign/list_of.hpp"
namespace {
  const std::vector<std::string> phases = boost::assign::list_of("initialize")("start")("execute");
}
#endif

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
ServiceStarter::ServiceStarter(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty("Service", m_serviceName,
      "Service to retrieve.");
  declareProperty("Phase", m_phase,
      "When to retrieve the service (initialize, start, execute).");
}

// ============================================================================
// Destructor
// ============================================================================
ServiceStarter::~ServiceStarter() {}

StatusCode ServiceStarter::i_retrieveService(const std::string &currentPhase) {
  if (!m_service && currentPhase == m_phase) {
    info() << "Retrieving " << m_serviceName << endmsg;
    m_service = service(m_serviceName);
    if (!m_service) {
      error() << "Could not get service";
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

// ============================================================================
// Initialization
// ============================================================================
StatusCode ServiceStarter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  if (m_serviceName.empty()) {
    error() << "The property 'Service' must be specified." << endmsg;
    return StatusCode::FAILURE;
  }

  if (m_phase.empty() || std::find(phases.begin(), phases.end(), m_phase) == phases.end()) {
    error() << "The property 'Phase' must be set to one of: ";
    for (std::vector<std::string>::const_iterator i = phases.begin(); i != phases.end(); ++i) {
      if (phases.begin() != i)
        error() << ", ";
      error() << *i;
    }
    error() << endmsg;
    error() << "  current value: " << (m_phase.empty() ? std::string("(empty)") : m_phase) << endmsg;
    return StatusCode::FAILURE;
  }

  return i_retrieveService("initialize");
}

// ============================================================================
// Main execution
// ============================================================================
StatusCode ServiceStarter::start() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Start" << endmsg;

  return i_retrieveService("start");
}

// ============================================================================
// Main execution
// ============================================================================
StatusCode ServiceStarter::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  return i_retrieveService("execute");
}

// ============================================================================
// Finalize
// ============================================================================
StatusCode ServiceStarter::finalize() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  m_service.reset();

  return GaudiAlgorithm::finalize(); // must be called after all other actions
}

// ============================================================================
