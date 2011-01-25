/*
 * DQFilter.h
 *
 *  Created on: Jan 24, 2011
 *      Author: marcocle
 */

#ifndef DQFILTER_H_
#define DQFILTER_H_

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"

class Condition;
class IIncidentSvc;

/** @class DQFilter
 *  Small algorithm to filter events according to the Data Quality flags stored
 *  in the conditions database.
 *
 *  When initialized, the algorithm register itself as user of the DQ Flags
 *  conditions and, by default, it listens for BeginEvent incidents.
 *
 *  @author Marco Clemencic
 *  @date   Jan 24, 2011
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
  /// Path to the condition object containing the Data Quality flags.
  std::string m_condPath;

  /// Flag to state if we have to filter at the level of the Begin Event
  /// incident or during the execute.
  bool m_beginEvent;

  /// List of flag (names) to ignore during the filtering. The matching is
  /// case-insensitive.
  std::vector<std::string> m_ignoredFlagsProp;

  /// Internal storage for ignored flags.
  std::set<std::string> m_ignoredFlags;

  /// Call-back function to update the internal storage of ignored flags.
  void i_propUpdate(Property&);

  /// Call-back function passed to the UpdateManagerSvc to update the current
  /// filtering status (good or bad).
  StatusCode i_checkFlags();

  /// Transient flag updated every time the DQ Flags condition changes to state
  /// if the currently processed event is good or bad.
  bool m_bad;

  /// Pointer to the Data Quality Flags condition (filled by the UpdateManagerSvc).1
  Condition *m_flags;

  /// Pointer to the IncidentSvc.
  SmartIF<IIncidentSvc> m_incSvc;
};

#endif // DQFILTER_H_
