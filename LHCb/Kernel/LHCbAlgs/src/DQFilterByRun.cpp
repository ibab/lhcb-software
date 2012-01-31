#include "DQFilterByRun.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "DetDesc/Condition.h"

DQFilterByRun::DQFilterByRun(const std::string & name, ISvcLocator *pSvcLocator):
  base_class(name, pSvcLocator), m_filter("BasicDQFilter", this),
  m_scanner("CondDBDQScanner", this), m_bad(false)
{
  declareProperty("ConditionPath",
                  m_condPath = "Conditions/Online/LHCb/RunParameters",
                  "Path in the Detector Transient Store where to find the run "
                  "condition.");
  declareProperty("UseBeginEvent",
                  m_beginEvent = true,
                  "If set to true, the filtering is done at the level of the "
                  "BeginEvent incident.");
  declareProperty("Filter",
                  m_filter,
                  "IDQFilter Tool defining the acceptance rules.");
  declareProperty("Scanner",
                  m_scanner,
                  "IDQScanner Tool defining the acceptance rules.");
}


StatusCode DQFilterByRun::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  registerCondition(m_condPath, m_run, &DQFilterByRun::i_checkFlags);

  // We must register to the UMS *before* registering to IncidentSvc to
  // have the correct order of calls.
  if (m_beginEvent) {
    m_incSvc = service("IncidentSvc");
    if (!m_incSvc) {
      error() << "Failed to load IncidentSvc" << endmsg;
      return StatusCode::FAILURE;
    }

    m_incSvc->addListener(this, IncidentType::BeginEvent);
  }

  if (msgLevel(MSG::DEBUG)) {
    debug() << "DQFilterByRun/" << name() << " initialized:" << endmsg;
    debug() << "  using condition at '" << m_condPath << "'" << endmsg;
    debug() << "  filtering on " << ((m_beginEvent) ? "BeginEvent" : "execute") << endmsg;
    debug() << "  using " << m_filter.typeAndName() << endmsg;
    debug() << "  on the results of " << m_scanner.typeAndName() << endmsg;
  }
  return sc;
}

StatusCode DQFilterByRun::execute()
{
  // Print the message only if we do not use the BeginEvent incident
  // otherwise it gets printed twice for good events.
  if ((!m_beginEvent) && (msgLevel() <= MSG::VERBOSE)) {
    verbose() << "Filter event: "
              << ((m_bad) ? "bad" : "good") << " event" << endmsg;
  }
  setFilterPassed(!m_bad);
  return StatusCode::SUCCESS;
}


StatusCode DQFilterByRun::finalize()
{
  if (m_incSvc) {
    m_incSvc->removeListener(this, IncidentType::BeginEvent);
    m_incSvc.reset();
  }
  m_filter.release().ignore();

  return GaudiAlgorithm::finalize();
}

StatusCode DQFilterByRun::i_checkFlags()
{
  if (UNLIKELY(msgLevel(MSG::VERBOSE)))
    verbose() << "Updating Data Quality flags for run " << m_run->param<int>("RunNumber") << endmsg;

  const IDQFilter::FlagsType& flags = m_scanner->scan(m_run->validSince(), m_run->validTill());
  if (UNLIKELY(msgLevel(MSG::VERBOSE)))
    verbose() << "-> " << flags << endmsg;

  m_bad = !m_filter->accept(flags);
  // we successfully updated the m_bad state
  return StatusCode::SUCCESS;
}

void DQFilterByRun::handle(const Incident&) {
  if (msgLevel() <= MSG::VERBOSE) {
    verbose() << "Handling incident: "
              << ((m_bad) ? "bad" : "good") << " event" << endmsg;
  }
  // note that m_bad is automatically updated when the DQ Flag condition changes
  if (m_bad) {
    m_incSvc->fireIncident(Incident(name(), IncidentType::AbortEvent));
  }
}

#include "GaudiKernel/AlgFactory.h"
DECLARE_ALGORITHM_FACTORY( DQFilterByRun )
