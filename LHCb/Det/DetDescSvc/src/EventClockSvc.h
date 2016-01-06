// $Id: EventClockSvc.h,v 1.3 2006-09-26 10:45:48 marcocle Exp $
#ifndef EVENTCLOCKSVC_H 
#define EVENTCLOCKSVC_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"

class IEventTimeDecoder;
class IToolSvc;

/** @class EventClockSvc EventClockSvc.h
 *  
 *  Small IncidentListener to set the event time.
 *
 *  @author Marco Clemencic
 *  @date   2005-07-08
 */
class EventClockSvc: public extends<Service, IIncidentListener> {
public: 
  /// Standard constructor
  EventClockSvc(const std::string& name, ISvcLocator* svcloc); 

  virtual ~EventClockSvc( ); ///< Destructor

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
  /// Name of the Data Provider (set by the option DetDataSvc, by default "DetectorDataSvc").
  std::string m_detDataSvcName;
  /// Name of the Event Time Decoder (set by the option EventTimeDecoder, by default "FakeEventTime").
  std::string m_eventTimeDecoderName;
  
  // --- services ---
  /// Pointer to the incident service;
  SmartIF<IIncidentSvc> m_incidentSvc;
  /// Handle to the IDetDataSvc interface (to propagate the event time).
  IDetDataSvc      *m_detDataSvc;
  /// Handle to the Tool Service.
  IToolSvc         *m_toolSvc;

  // --- tools ---
  /// Pointer to the EventTimeDecoder tool
  IEventTimeDecoder *m_eventTimeDecoder;


  // --- misc ---
  /// Time to use before reading the first event  (set by the option InitialTime, by default 0).
  long long m_initialTime;
  
};
#endif // EVENTCLOCKSVC_H
