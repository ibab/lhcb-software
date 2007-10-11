// $Id: EvtCounter.h,v 1.1 2007-10-11 08:47:29 cattanem Exp $
#ifndef EVTCOUNTER_H 
#define EVTCOUNTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "IEventCounter.h"

/** @class EvtCounter EvtCounter.h
 *  
 *  @author Patrick KOPPENBURG
 *  @date   2004-06-15
 */
class EvtCounter : public GaudiTool,
                   virtual public IEventCounter,
                   virtual public IIncidentListener {
public: 

  /// Standard constructor
  EvtCounter( const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  virtual ~EvtCounter( ){}; ///< Destructor

  /// IListener interface to be triggered at begin of event
  virtual void handle(const Incident&) { this->incrementEventCounter(); }
  
  /// Initialize method 
  StatusCode initialize();
  
  /// Finalize method 
  StatusCode finalize(){return StatusCode::SUCCESS;};

  /// Get Event counter
  long getEventCounter() { return m_eventCounter; } 

  /// Increment Event counter
  void incrementEventCounter() { ++m_eventCounter; }
  
protected:

private:
  long m_eventCounter; ///< number of events processed
};
#endif // EVTCOUNTER_H
