// $Id: EventClockSvc.h,v 1.2 2006-02-01 19:40:26 marcocle Exp $
#ifndef EVENTCLOCKSVC_H 
#define EVENTCLOCKSVC_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"

/** @class EventClockSvc EventClockSvc.h
 *  
 *  Small IncidentListener to set the event time.
 *
 *  @author Marco Clemencic
 *  @date   2005-07-08
 */
class EventClockSvc: public virtual Service, public virtual IIncidentListener {
public: 
  /// Standard constructor
  EventClockSvc(const std::string& name, ISvcLocator* svcloc); 

  virtual ~EventClockSvc( ); ///< Destructor

  /** Query interfaces (\see{IInterface})
      @param riid       ID of Interface to be retrieved
      @param ppvUnknown Pointer to Location for interface pointer
  */
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

  /// Initialize Service
  virtual StatusCode initialize();
  
  /// Finalize Service
  virtual StatusCode finalize();

  // ---- Implement IIncidentListener interface ----
  /// Handle BeginEvent incident.
  virtual void handle(const Incident &inc);

protected:

private:

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<EventClockSvc>;

  // ---------- data members ----------
  // --- names ---
  /// Name of the Data Provider (set by the option EvtDataSvc, by default "EventDataSvc").
  std::string m_evtDataProviderName;
  /// Name of the Data Provider (set by the option DetDataSvc, by default "DetectorDataSvc").
  std::string m_detDataSvcName;
  
  // --- services ---
  /// Pointer to the incident service;
  IIncidentSvc     *m_incidentSvc;
  /// Handle to the Data Provider interface of EventDataSvc (where to find the time... in the future).
  IDataProviderSvc *m_evtDataProvider;
  /// Handle to the IDetDataSvc interface (to propagate the event time).
  IDetDataSvc      *m_detDataSvc;

  // --- misc ---
  /// First event time (when simluating them).
  /// Set by the option StartTime (default = 0)
  long long m_startTime;
  /// Event time increment (when simluating event times).
  /// Set by the option TimeStep (default = 0, which means no simulation)
  long long m_timeStep;

};
#endif // EVENTCLOCKSVC_H
