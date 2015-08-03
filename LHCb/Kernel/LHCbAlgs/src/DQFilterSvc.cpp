#include "DQFilterSvc.h"

#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IUpdateManagerSvc.h"


DECLARE_SERVICE_FACTORY(DQFilterSvc)

DQFilterSvc::DQFilterSvc(const std::string & name, ISvcLocator *svc):
  base_class(name, svc), m_acceptTool(0)
{
  declareProperty("AcceptTool",
                  m_acceptToolName = "DQAcceptTool",
                  "IAccept Tool to filter the events.");
}



StatusCode DQFilterSvc::initialize()
{
  StatusCode sc = Service::initialize();
  if (sc.isFailure()) return sc;

  if ( !(m_toolSvc = serviceLocator()->service("ToolSvc")) ) // assignment meant
  {
    error() << "Failed to retrieve ToolSvc" << endmsg;
    return StatusCode::FAILURE;
  }

  // Instantiate the public tool.
  // It is better to get it soon because it should require the UMS, and it must
  // be registered to the incident svc before us.
  sc = m_toolSvc->retrieveTool(m_acceptToolName, m_acceptTool);
  if (sc.isFailure()) return sc;

  // We must ensure that the UMS is up *before* registering to IncidentSvc to
  // have the correct order of calls.
  serviceLocator()->service("UpdateManagerSvc");
  if ( !(m_incSvc = serviceLocator()->service("IncidentSvc")) ) // assignment meant
  {
    error() << "Failed to retrieve Incident service." << endmsg;
    return StatusCode::FAILURE;
  }
  m_incSvc->addListener(this, IncidentType::BeginEvent);

  if (msgLevel(MSG::DEBUG)) {
    debug() << "DQFilterSvc/" << name() << " initialized:" << endmsg;
    debug() << "  filtering using " << m_acceptToolName << endmsg;
  }

  return StatusCode::SUCCESS;
}



StatusCode DQFilterSvc::finalize()
{
  if (m_incSvc) { // we may not have retrieved the IncidentSvc
    m_incSvc->removeListener(this, IncidentType::BeginEvent);
    m_incSvc.reset();
  }

  if (m_toolSvc) { // we may not have retrieved the ToolSvc
    // Do not call releaseTool if the ToolSvc was already finalized.
    if (SmartIF<IStateful>(m_toolSvc)->FSMState() > Gaudi::StateMachine::CONFIGURED) {
      m_toolSvc->releaseTool(m_acceptTool).ignore();
    } else {
      info() << "ToolSvc already finalized: cannot release tools. Check options." << endmsg;
    }
    m_acceptTool = 0;
    m_toolSvc.reset();
  }

  return Service::finalize();
}



void DQFilterSvc::handle(const Incident &)
{
  const bool accepted = m_acceptTool->accept();
  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "Handling incident: "
              << ((accepted) ? "good" : "bad") << " event" << endmsg;
  }
  if (!accepted) {
    m_incSvc->fireIncident(Incident(name(), IncidentType::AbortEvent));
  }
}



DQFilterSvc::~DQFilterSvc()
{
}



