// Include files
#include "GaudiKernel/Map.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "DetDesc/Condition.h"

#include <vector>

namespace DetCondTest {

/** @class TestConditionAlg TestConditionAlg.h component/TestConditionAlg.h
 *
 *  Simple algorithm that prints the requested conditions at every event.
 *
 *  @author Marco CLEMENCIC
 *  @date   2008-06-27
 */
class TestConditionAlg : public GaudiAlgorithm {
public:
  /// Standard constructor
  TestConditionAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestConditionAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void i_dump();

  /// Names of the conditions to print
  std::vector<std::string> m_condPaths;

  /// Flag to decide if the conditions have to be loaded also during the
  /// initialize step.
  bool m_loadAtInit;

  /// Container of the conditions to print
  std::vector<std::string> m_conditionDeps;

  /// Container of the conditions to print
  GaudiUtils::Map<std::string,Condition*> m_conditions;
};

//-----------------------------------------------------------------------------
// Implementation file for class : TestConditionAlg
//
// 2008-06-27 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TestConditionAlg::TestConditionAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("Conditions", m_condPaths,
      "list of paths to conditions in the detector transient store");
  declareProperty("LoadDuringInitialize", m_loadAtInit = false,
      "load the requested conditions already during the initialization");
  declareProperty("ConditionsDependencies", m_conditionDeps,
      "declare dependencies between objects as a list of strings 'A -> B') "
      "to indicate that the condition A depends on B.");
}
//=============================================================================
// Destructor
//=============================================================================
TestConditionAlg::~TestConditionAlg() {}

namespace {
  void printDepsError(MsgStream& stream,
      size_t lineNo, const std::string& msg,
      const std::string& line) {
    stream << "Syntax error in item " << lineNo
        << " of ConditionsDependencies: " << msg << endmsg;
    stream << "  '" << line << "'" << endmsg;
  }
}
//=============================================================================
// Initialization
//=============================================================================
StatusCode TestConditionAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  std::vector<std::string>::const_iterator path;
  for (path = m_condPaths.begin(); path != m_condPaths.end(); ++path){
    registerCondition<TestConditionAlg>(*path,m_conditions[*path],NULL);
  }

  std::vector<std::string>::const_iterator deps;
  for (deps = m_conditionDeps.begin(); deps != m_conditionDeps.end(); ++deps) {
    std::string::size_type pos = deps->find("->");
    if (pos == std::string::npos) {
      printDepsError(error(), deps - m_conditionDeps.begin(), "missing '->'", *deps);
      return StatusCode::FAILURE;
    }
    std::string::size_type p0, p1;
    p0 = deps->find_first_not_of(" \n\r\t");
    p1 = deps->find_last_not_of(" \n\r\t", pos - 1) + 1;
    std::string first(*deps, p0, p1 - p0);
    if (first.empty()) {
      printDepsError(error(), deps - m_conditionDeps.begin(), "missing first argument", *deps);
      return StatusCode::FAILURE;
    }
    p0 = deps->find_first_not_of(" \n\r\t", pos + 2);
    p1 = deps->find_last_not_of(" \n\r\t") + 1;
    std::string second(*deps, p0, p1 - p0);
    if (second.empty()) {
      printDepsError(error(), deps - m_conditionDeps.begin(), "missing second argument", *deps);
      return StatusCode::FAILURE;
    }
    info() << "Declaring dependency of '" << first << "' on '" << second << "'" << endmsg;
    updMgrSvc()->registerCondition(getDet<Condition>(first), second);
  }

  if (m_loadAtInit) {
    sc = updMgrSvc()->update(this);
    info() << "Conditions loaded at initialize" << endmsg;
    if (sc.isSuccess()){
      i_dump();
    }
    else return sc;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestConditionAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  i_dump();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TestConditionAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  updMgrSvc()->unregister(this);
  m_conditions.clear();
  m_condPaths.clear();

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
//  Print the conditions
//=============================================================================
void TestConditionAlg::i_dump()
{
  info() << "Requested Conditions:\n";
  GaudiUtils::Map<std::string,Condition*>::iterator it;
  for (it = m_conditions.begin(); it != m_conditions.end(); ++it) {
    info() << "--- " << it->first << "\n" << *(it->second) << "\n";
  }
  info() << endmsg;
}

//=============================================================================


/** Small algorithm that runs a fake event loop during finalize to scan the
 *  values of the conditions (see bug #74255).
 *
 *  @author Marco CLEMENCIC
 *  @date   2010-10-25
 */
class FinalizationEvtLoop: public GaudiAlgorithm {
public:
  /// Standard constructor
  FinalizationEvtLoop(const std::string& name, ISvcLocator* pSvcLocator):
    GaudiAlgorithm(name, pSvcLocator)
  {
    declareProperty("Conditions", m_condPaths,
                    "list of paths to conditions in the detector transient store");
    declareProperty("InitialTime", m_initTime = 0, // 1970-01-01 00:00:00UTC
                    "First event time of the fake event loop");
    declareProperty("FinalTime", m_finalTime = m_initTime + 10000000000LL, // init + 10s
                    "Final time of the loop");
    declareProperty("Step", m_step = 1000000000LL, // 1s
                    "Step of the loop");
  }

  virtual ~FinalizationEvtLoop() {}

  virtual StatusCode initialize() {
    StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

    if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

    std::vector<std::string>::const_iterator path;
    for (path = m_condPaths.begin(); path != m_condPaths.end(); ++path){
      registerCondition<FinalizationEvtLoop>(*path, m_conditions[*path], NULL);
    }

    return StatusCode::SUCCESS;
  }

  virtual StatusCode execute   () {
    return StatusCode::SUCCESS;
  }

  virtual StatusCode finalize  () {
    Gaudi::Time t(m_initTime);
    const Gaudi::Time fin(m_finalTime);
    const Gaudi::TimeSpan step(m_step);

    SmartIF<IDetDataSvc> dds(detSvc());
    for ( ; t < fin; t += step) {
      dds->setEventTime(t);
      info() << "Update for event time " << t << endmsg;
      if (updMgrSvc()->newEvent().isSuccess()) {
        info() << "Requested Conditions:\n";
        GaudiUtils::Map<std::string,Condition*>::iterator it;
        for (it = m_conditions.begin(); it != m_conditions.end(); ++it) {
          info() << "--- " << it->first << "\n" << *(it->second) << "\n";
        }
        info() << endmsg;
      }
      else {
        error() << "Failure updating" << endmsg;
        return StatusCode::FAILURE;
      }
    }

    return StatusCode::SUCCESS;
  }

private:
  /// First event time of the fake event loop
  Gaudi::Time::ValueType m_initTime;
  /// First event time of the fake event loop
  Gaudi::Time::ValueType m_finalTime;
  /// First event time of the fake event loop
  Gaudi::Time::ValueType m_step;

  /// Names of the conditions to print
  std::vector<std::string> m_condPaths;

  /// Container of the conditions to print
  GaudiUtils::Map<std::string,Condition*> m_conditions;
};

/** Test algorithm that triggers the bug #80076
 *  https://savannah.cern.ch/bugs/?80076
 */
class bug_80076: public TestConditionAlg {
public:
  /// Constructor.
  bug_80076(const std::string& name, ISvcLocator* pSvcLocator):
    TestConditionAlg(name, pSvcLocator) {}

  /// Override the initialize to ensure that the conditions are already loaded
  /// during the initialize.
  StatusCode initialize() {
    StatusCode sc = TestConditionAlg::initialize();
    if (sc.isFailure()) return sc;

    std::vector<std::string>::const_iterator path;
    for (path = m_condPaths.begin(); path != m_condPaths.end(); ++path){
      // this ensures that the objects are loaded in the transient store
      exist<DataObject>(detSvc(), *path);
    }

    return sc;
  }
};

}

// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY( DetCondTest, TestConditionAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( DetCondTest, FinalizationEvtLoop )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( DetCondTest, bug_80076 )
