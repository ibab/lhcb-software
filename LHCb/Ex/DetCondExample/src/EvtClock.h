// $Id: EvtClock.h,v 1.4 2005-06-23 09:33:37 marcocle Exp $
#ifndef EVTCLOCK_H 
#define EVTCLOCK_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/TimePoint.h"

// Forward declarations
class IDetDataSvc;

/** @class EvtClock EvtClock.h
 *  
 *  Simple algorithm to set fake event times on the detector data service.
 *
 *  @author Andrea Valassi
 *  @date   August 2001
 *  @author Marco Clemencic
 *  @date   2005-06-23
 */
class EvtClock : public GaudiAlgorithm {
public: 
  /// Standard constructor
  EvtClock( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~EvtClock( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  /// Absolute time of current event
  TimePoint i_evtTime();

private:

  /// Current event number
  long m_eventNumber;

  /// Absolute time of first event
  long m_startTime;
  //  ITime::AbsoluteTime m_startTime;

  /// Absolute time spacing between events
  long m_delayTime;
  //  ITime::AbsoluteTime m_delayTime;

  /// Name of the detector data service (option EvtClock.DetDataSvc)
  std::string  m_detDataSvcName;

  /// Handle to the IDetDataSvc interface of the DetectorDataSvc
  IDetDataSvc* m_detDataSvc;
  
};
#endif // EVTCLOCK_H
