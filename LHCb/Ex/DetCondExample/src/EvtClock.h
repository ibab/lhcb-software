//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/src/EvtClock.h,v 1.1.1.1 2001-09-14 15:45:03 andreav Exp $
#ifndef DETCONDEXAMPLE_EVTCLOCK_H
#define DETCONDEXAMPLE_EVTCLOCK_H 1

// Base class
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IConditionDataSvc;

///---------------------------------------------------------------------------
/** @class EvtClock EvtClock.h DetCondExample/EvtClock.h

    Simple algorithm to set fake event times on the ConditionDataSvc.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class EvtClock : public Algorithm {

 public:

  /// Constructor
  EvtClock ( const std::string& name, ISvcLocator* pSvcLocator ); 
  
  // Algorithm standard methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:

  /// Absolute time of current event
  longlong i_evtTime();

 private:

  /// Current event number
  long m_eventNumber;

  /// Absolute time of first event
  long m_startTime;

  /// Absolute time spacing between events
  long m_delayTime;

  /// Handle to the IConditionDataSvc
  IConditionDataSvc* m_conditionDataSvc;
  
};

#endif    // DETCONDEXAMPLE_EVTCLOCK_H


