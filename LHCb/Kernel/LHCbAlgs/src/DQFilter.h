/*
 * DQFilter.h
 *
 *  Created on: Jan 31, 2012
 *      Author: marcocle
 */

#ifndef DQFILTER_H_
#define DQFILTER_H_

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "Kernel/IAccept.h"

class Condition;
class IIncidentSvc;

/** @class DQFilter
 *  Small algorithm to filter events according to the Data Quality flags stored
 *  in the conditions database on a run-by-run basis.
 *
 *  When initialized, the algorithm register itself as user of the DQ Flags
 *  conditions and, by default, it listens for BeginEvent incidents.
 *
 *  @author Marco Clemencic
 *  @date   Jan 31, 2012
 */
class DQFilter: public extends1<GaudiAlgorithm, IIncidentListener> {
public:
  /// Standard constructor
  DQFilter( const std::string& name, ISvcLocator* pSvcLocator );

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
  /// Flag to state if we have to filter at the level of the Begin Event
  /// incident or during the execute.
  bool m_beginEvent;

  /// Type/Name of the (public) IAccept tool used to choose if the event has to
  /// be accepted or not (default: DQAcceptTool).
  std::string m_acceptToolName;

  /// Pointer to the IAccept tool.
  IAccept *m_acceptTool;

  /// Pointer to the IncidentSvc.
  SmartIF<IIncidentSvc> m_incSvc;
};

#endif // DQFILTERBYRUN_H_
