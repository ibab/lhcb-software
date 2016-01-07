#include "DQFilter.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "DetDesc/Condition.h"

DQFilter::DQFilter(const std::string & name, ISvcLocator *pSvcLocator):
  base_class(name, pSvcLocator), m_acceptTool(0)
{
  declareProperty("UseBeginEvent",
                  m_beginEvent = true,
                  "If set to true, the filtering is done at the level of the "
                  "BeginEvent incident.");
  declareProperty("AcceptTool",
                  m_acceptToolName = "DQAcceptTool",
                  "IAccept Tool to filter the events.");
}


StatusCode DQFilter::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  // Instantiate the public tool.
  // It is better to get it soon because it should require the UMS, and it must
  // be registered to the incident svc before us.
  m_acceptTool = tool<IAccept>(m_acceptToolName);

  // We must ensure that the UMS is up *before* registering to IncidentSvc to
  // have the correct order of calls.
  if (m_beginEvent) {
    updMgrSvc();
    m_incSvc = service("IncidentSvc");
    if (!m_incSvc) {
      error() << "Failed to load IncidentSvc" << endmsg;
      return StatusCode::FAILURE;
    }

    m_incSvc->addListener(this, IncidentType::BeginEvent);
  }

  if (msgLevel(MSG::DEBUG)) {
    debug() << "DQFilter/" << name() << " initialized:" << endmsg;
    debug() << "  filtering on " << ((m_beginEvent) ? "BeginEvent" : "execute") << endmsg;
    debug() << "  using " << m_acceptToolName << endmsg;
  }
  return sc;
}

StatusCode DQFilter::execute()
{
  const bool accepted = m_acceptTool->accept();
  // Print the message only if we do not use the BeginEvent incident
  // otherwise it gets printed twice for good events.
  if ((!m_beginEvent) && (msgLevel() <= MSG::VERBOSE)) {
    verbose() << "Filter event: "
              << ((accepted) ? "good" : "bad") << " event" << endmsg;
  }
  setFilterPassed(accepted);
  return StatusCode::SUCCESS;
}

StatusCode DQFilter::finalize()
{
  if (m_incSvc) {
    m_incSvc->removeListener(this, IncidentType::BeginEvent);
    m_incSvc.reset();
  }
  if (m_acceptTool) {
    releaseTool(m_acceptTool).ignore();
    m_acceptTool = 0;
  }

  return GaudiAlgorithm::finalize();
}

void DQFilter::handle(const Incident&) {
  const bool accepted = m_acceptTool->accept();
  if (msgLevel() <= MSG::VERBOSE) {
    verbose() << "Handling incident: "
              << ((accepted) ? "good" : "bad") << " event" << endmsg;
  }
  if (!accepted) {
    m_incSvc->fireIncident(Incident(name(), IncidentType::AbortEvent));
  }
}

DECLARE_ALGORITHM_FACTORY( DQFilter )
