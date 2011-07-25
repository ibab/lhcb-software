#include "DQFilter.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "DetDesc/Condition.h"

#include <boost/foreach.hpp>
#include <boost/algorithm/string/case_conv.hpp>
using boost::algorithm::to_lower_copy;

DQFilter::DQFilter(const std::string & name, ISvcLocator *pSvcLocator):
  base_class(name, pSvcLocator), m_bad(false)
{
  declareProperty("ConditionPath",
                  m_condPath = "Conditions/DQ/Flags",
                  "Path in the Detector Transient Store where to find the Data "
                  "Quality condition.");
  declareProperty("UseBeginEvent",
                  m_beginEvent = true,
                  "If set to true, the filtering is done at the level of the "
                  "BeginEvent incident.");
  declareProperty("IgnoredFlags",
                  m_ignoredFlagsProp,
                  "List of flags to ignore when filtering events")
    ->declareUpdateHandler(&DQFilter::i_propUpdate, this);
}


StatusCode DQFilter::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  registerCondition(m_condPath, m_flags, &DQFilter::i_checkFlags);

  if (m_beginEvent) {
    m_incSvc = service("IncidentSvc");
    if (!m_incSvc) {
      error() << "Failed to load IncidentSvc" << endmsg;
      return StatusCode::FAILURE;
    }

    m_incSvc->addListener(this, IncidentType::BeginEvent);
  }

  if (msgLevel() <= MSG::DEBUG) {
    debug() << "DQFilter/" << name() << " initialized:" << endmsg;
    debug() << "  using condition at '" << m_condPath << "'" << endmsg;
    debug() << "  filtering on " << ((m_beginEvent) ? "BeginEvent" : "execute") << endmsg;
    if (!m_ignoredFlagsProp.empty()) {
      debug() << "  ignoring flags:" << endmsg;
      BOOST_FOREACH(const std::string &s, m_ignoredFlagsProp)
      {
        debug() << "    " << s << endmsg;
      }
    }
  }
  return sc;
}

StatusCode DQFilter::execute()
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


StatusCode DQFilter::finalize()
{
  if (m_incSvc) {
    m_incSvc->removeListener(this, IncidentType::BeginEvent);
    m_incSvc.reset();
  }

  return GaudiAlgorithm::finalize();
}

void DQFilter::i_propUpdate(Property&) {
  m_ignoredFlags.clear();
  BOOST_FOREACH(const std::string &s, m_ignoredFlagsProp)
  {
    m_ignoredFlags.insert(to_lower_copy(s));
  }
}

StatusCode DQFilter::i_checkFlags()
{
  if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
    verbose() << "Updating Data Quality flags" << endmsg;
  m_bad = false;
  typedef std::map<std::string, int> flags_t;
  const flags_t &flags = m_flags->param<flags_t>("map");
  for (flags_t::const_iterator f = flags.begin(); (!m_bad) && (f != flags.end()); ++f) {
    // if the flag is not ignored
    if (m_ignoredFlags.find(to_lower_copy(f->first)) == m_ignoredFlags.end())
      m_bad = (f->second != 0); // no need to consider the previous value of m_bad because we do an early exit
    else {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << "Ignoring flag: " << f->first << endmsg;
    }
  }
  // we successfully updated the m_bad state
  return StatusCode::SUCCESS;
}

void DQFilter::handle(const Incident&) {
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
DECLARE_ALGORITHM_FACTORY( DQFilter )
