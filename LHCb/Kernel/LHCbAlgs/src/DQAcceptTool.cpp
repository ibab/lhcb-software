// Include files

// From Gaudi
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "DetDesc/Condition.h"

// local
#include "DQAcceptTool.h"

// ----------------------------------------------------------------------------
// Implementation file for class: DQAcceptTool
//
// 09/02/2012: Marco Clemencic
// ----------------------------------------------------------------------------
DECLARE_TOOL_FACTORY(DQAcceptTool)

#define ON_DEBUG if (UNLIKELY(msgLevel(MSG::DEBUG)))
#define DEBUG_MSG ON_DEBUG debug()

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
DQAcceptTool::DQAcceptTool(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent), m_filter(0), m_scanner(0), m_accepted(true), m_cond(0)
{
  declareProperty("ConditionPath",
                  m_condPath = "Conditions/Online/LHCb/RunParameters",
                  "Path in the Detector Transient Store where to find the run "
                  "condition.");
  declareProperty("ByRun",
                  m_byRun = true,
                  "If the DQ Flags have to be considered by run or by event.");
  declareProperty("Filter",
                  m_filterName = "BasicDQFilter",
                  "IDQFilter Tool defining the acceptance rules.");
  declareProperty("Scanner",
                  m_scannerName = "CondDBDQScanner",
                  "IDQScanner Tool to collect all the DQ Flags in a run.");
}

StatusCode DQAcceptTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  if (UNLIKELY(m_condPath.empty())) {
    error() << "Cannot work with empty ConditionPath" << endmsg;
    return StatusCode::FAILURE;
  }

  m_filter = tool<IDQFilter>(m_filterName, this);
  m_scanner = tool<IDQScanner>(m_scannerName, this);

  registerCondition(m_condPath, m_cond,
                    (m_byRun)
                        ? &DQAcceptTool::i_checkFlagsByRun
                        : &DQAcceptTool::i_checkFlagsByEvent);

  ON_DEBUG {
    debug() << "DQAcceptTool/" << name() << " initialized:" << endmsg;
    debug() << "  using condition at '" << m_condPath << "'";
    if (m_byRun) {
      debug() << " for run boundaries" << endmsg;
      debug() << "  scanning flags with " << m_scannerName << endmsg;
    } else {
      debug() << " DQ Flags" << endmsg;
    }
    debug() << "  analyze flags with " << m_filterName << endmsg;
  }

  // We do not want to run a scan during initialization, but we may be
  // instantiated during and event. So, if it is the case, we trigger an
  // immediate update.
  SmartIF<IStateful> stateMachine(serviceLocator());
  if (stateMachine->FSMState() >= Gaudi::StateMachine::RUNNING) {
    sc = updMgrSvc()->update(this);
  }

  return sc;
}

StatusCode DQAcceptTool::i_checkFlagsByRun()
{
  if (UNLIKELY(msgLevel(MSG::VERBOSE)))
    verbose() << "Updating Data Quality flags for run " << m_cond->param<int>("RunNumber") << endmsg;

  const IDQFilter::FlagsType& flags = m_scanner->scan(m_cond->validSince(), m_cond->validTill());
  if (UNLIKELY(msgLevel(MSG::VERBOSE)))
    verbose() << "-> " << flags << endmsg;

  m_accepted = m_filter->accept(flags);
  // we successfully updated the m_bad state
  return StatusCode::SUCCESS;
}

StatusCode DQAcceptTool::i_checkFlagsByEvent()
{
  if(UNLIKELY(msgLevel(MSG::VERBOSE)))
    verbose() << "Updating Data Quality flags" << endmsg;

  const IDQFilter::FlagsType& flags = m_cond->param<IDQFilter::FlagsType>("map");
  if (UNLIKELY(msgLevel(MSG::VERBOSE)))
      verbose() << "-> " << flags << endmsg;

  m_accepted = m_filter->accept(flags);
  // we successfully updated the m_bad state
  return StatusCode::SUCCESS;
}

StatusCode DQAcceptTool::finalize()
{
  if (m_filter) {
    releaseTool(m_filter).ignore();
    m_filter = 0;
  }
  if (m_scanner) {
    releaseTool(m_scanner).ignore();
    m_scanner = 0;
  }

  return GaudiTool::finalize();
}

bool DQAcceptTool::accept() const {
  return m_accepted;
}

// ============================================================================
// Destructor
// ============================================================================
DQAcceptTool::~DQAcceptTool() {}

// ============================================================================
