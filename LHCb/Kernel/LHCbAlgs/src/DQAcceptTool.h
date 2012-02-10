#ifndef SRC_DQACCEPTTOOL_H
#define SRC_DQACCEPTTOOL_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "Kernel/IDQFilter.h"
#include "Kernel/IDQScanner.h"

// Implemented interfaces
#include "Kernel/IAccept.h" // IAccept

class Condition;

/** @class DQAcceptTool DQAcceptTool.h src/DQAcceptTool.h
  *
  *
  * @author Marco Clemencic
  * @date 09/02/2012
  */
class DQAcceptTool: public extends1<GaudiTool, IAccept> {
public:
  /// Standard constructor
  DQAcceptTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~DQAcceptTool(); ///< Destructor

  /// Initialize the tool.
  virtual StatusCode initialize();

  /// Finalize the tool.
  virtual StatusCode finalize();

  /// Tells if the current event has to be accepted or not, according to the
  virtual bool accept() const;
protected:
private:
  /// Path to the used condition object.
  /// Depending on the "ByRun" property it defined the condition to get
  /// the run boundaries or the one for the DQ Flags.
  std::string m_condPath;

  /// Tell if the DQ flags have to be used by run or by event.
  bool m_byRun;

  /// Type/Name of the (private) tool to filter the DQ Flags (default: BasicDQFilter).
  std::string m_filterName;
  /// Type/Name of the (private) tool to scan the CondDB for the DQ Flags of a run (default: CondDBDQScanner).
  std::string m_scannerName;

  /// @{
  /// Pointers to the tools.
  IDQFilter *m_filter;
  IDQScanner *m_scanner;
  /// @}

  /// Call-back function passed to the UpdateManagerSvc to update the current
  /// filtering status (good or bad).
  StatusCode i_checkFlagsByRun();
  /// Call-back function passed to the UpdateManagerSvc to update the current
  /// filtering status (good or bad).
  StatusCode i_checkFlagsByEvent();

  /// Transient flag updated every time the run condition changes to state
  /// if the currently processed event is good or bad.
  bool m_accepted;

  /// Pointer to condition to use (filled by the UpdateManagerSvc).
  /// @see m_condPath
  Condition *m_cond;

};

#endif // SRC_DQACCEPTTOOL_H
