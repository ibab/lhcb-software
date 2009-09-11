// $Id: EventCountAlg.h,v 1.1 2009-09-11 09:21:43 rlambert Exp $
#ifndef EVENTCOUNTALG_H 
#define EVENTCOUNTALG_H 1

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"


/** @class EventCountAlg EventCountAlg.h
 *  
 *  This is a simple alg with an efficiency counter.
 *  It will presumably replace EventCountHisto for 
 *  most users of the XML summary service.
 *  The property CounterName can be used to set the 
 *  name of the counter at configuration time.
 *
 *  The number of events, and the number of times
 *  this algorithm are called are used to calculate 
 *  the efficiency
 *
 *  @author Robert Lambert
 *  @date   2007-01-08
 */
class EventCountAlg : public GaudiAlgorithm ,
                      virtual public IIncidentListener{
public: 
  /// Standard constructor
  EventCountAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~EventCountAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  /// IListener interface to be triggered at begin of event
  virtual void handle(const Incident&) { m_nHandled++; }
 
protected:

private:
  ///two integers are used, and a StatEntity isn't defined 
  ///until the last moment.
  unsigned long int m_nHandled; ///<total events
  unsigned long int m_nExecuted; ///<number of events reaching this point
  std::string m_counterName; ///<name of counter, set by option CounterName
  IIncidentSvc* m_incSvc; ///<the incident service

};
#endif // EVENTCOUNTHISTO_H
