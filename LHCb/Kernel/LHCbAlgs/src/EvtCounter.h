#ifndef EVTCOUNTER_H
#define EVTCOUNTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "Kernel/IEventCounter.h"

/** @class EvtCounter EvtCounter.h
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-06-15
 */
class EvtCounter : public extends2<GaudiTool,
                                   IEventCounter,
                                   IIncidentListener> {
public:

  /// Standard constructor
  EvtCounter( const std::string& type,
              const std::string& name,
              const IInterface* parent);

  virtual ~EvtCounter(){} ///< Destructor

  /// IListener interface to be triggered at begin of event
  virtual void handle(const Incident&) { this->incrementEventCounter(); }

  /// Initialize method
  StatusCode initialize();

  /// Get Event counter
  long getEventCounter() const { return m_eventCounter; }

  /// Increment Event counter
  void incrementEventCounter() { ++m_eventCounter; }

  /// Set Event counter
  void setEventCounter(long n) { m_eventCounter = n; }

protected:

private:
  long m_eventCounter; ///< number of events processed
  long m_initialCount; ///< allow to change the number used for the first event (property InitialCount)
  bool m_useIncident; ///< whether to increment of BeginEvent incident or not
};
#endif // EVTCOUNTER_H
