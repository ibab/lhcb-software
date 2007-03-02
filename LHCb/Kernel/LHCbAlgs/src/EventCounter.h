// $Id: EventCounter.h,v 1.1 2007-03-02 08:49:26 cattanem Exp $
#ifndef EVENTCOUNTER_H 
#define EVENTCOUNTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "IEventCounter.h"

//static const InterfaceID IID_EventCounter ( "EventCounter", 1, 0 );

/** @class EventCounter EventCounter.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-06-15
 */
class EventCounter : public GaudiTool,
                     virtual public IEventCounter,
                     virtual public IIncidentListener {
public: 

  // Return the interface ID
  //  static const InterfaceID& interfaceID() { return IID_EventCounter; }

  /// Standard constructor
  EventCounter( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~EventCounter( ){}; ///< Destructor

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
#endif // EVENTCOUNTER_H
