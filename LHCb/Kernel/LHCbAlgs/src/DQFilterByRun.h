/*
 * DQFilterByRun.h
 *
 *  Created on: Jan 31, 2012
 *      Author: marcocle
 */

#ifndef DQFILTERBYRUN_H_
#define DQFILTERBYRUN_H_

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "Kernel/IDQFilter.h"
#include "Kernel/IDQScanner.h"

class Condition;
class IIncidentSvc;

/** @class DQFilterByRun
 *  Small algorithm to filter events according to the Data Quality flags stored
 *  in the conditions database on a run-by-run basis.
 *
 *  When initialized, the algorithm register itself as user of the DQ Flags
 *  conditions and, by default, it listens for BeginEvent incidents.
 *
 *  @author Marco Clemencic
 *  @date   Jan 31, 2012
 */
class DQFilterByRun: public extends1<GaudiAlgorithm, IIncidentListener> {
public:
  /// Standard constructor
  DQFilterByRun( const std::string& name, ISvcLocator* pSvcLocator );

  /// Initialize the algorithm.
  /// Register to the UpdateManagerSvc as user of the Data Quality flags condition
  /// and, if not requested otherwise, register as BeginEvent listener.
  virtual StatusCode initialize();
  /// By default it does nothing, but if requested not to use the BeginEvent,
  /// it sets the
  virtual StatusCode execute();
  /// If registered as BeginEvent listener, unregister.
  virtual StatusCode finalize();

  virtual void handle(const Incident&);

private:
  /// Path to the condition object mapped to the run.
  std::string m_condPath;

  /// Flag to state if we have to filter at the level of the Begin Event
  /// incident or during the execute.
  bool m_beginEvent;

  ToolHandle<IDQFilter> m_filter;
  ToolHandle<IDQScanner> m_scanner;

  /// Call-back function passed to the UpdateManagerSvc to update the current
  /// filtering status (good or bad).
  StatusCode i_checkFlags();

  /// Transient flag updated every time the run condition changes to state
  /// if the currently processed event is good or bad.
  bool m_bad;

  /// Pointer to the run condition (filled by the UpdateManagerSvc).
  Condition *m_run;

  /// Pointer to the IncidentSvc.
  SmartIF<IIncidentSvc> m_incSvc;
};

#endif // DQFILTERBYRUN_H_
