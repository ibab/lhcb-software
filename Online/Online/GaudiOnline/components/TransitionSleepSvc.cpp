// $Id: PauseAlg.cpp,v 1.1 2009-12-03 19:05:31 frankb Exp $

// Include files from Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiOnline/OnlineService.h" 
#include "RTL/rtl.h"

/// LHCb namespace declaration
namespace LHCb  {

  /// Service to perform strategic sleeps during transitions.....
  /*
   *  \author Markus Frank
   *  \date   2015-10-13
   */
  class TransitionSleepSvc : public OnlineService  {
    /// Timeout in seconds
    int   m_initSleep;
    int   m_startSleep;
    int   m_stopSleep;
    int   m_finiSleep;
    int   m_termSleep;

    int   m_pauseSleep;
    int   m_cancelSleep;
  public: 

    /// Standard constructor
    TransitionSleepSvc(const std::string& nam,ISvcLocator* pSvc) 
      : OnlineService(nam,pSvc)
    {
      declareProperty("InitializeSleep", m_initSleep=0);
      declareProperty("StartSleep",      m_startSleep=0);
      declareProperty("StopSleep",       m_stopSleep=0);
      declareProperty("FinializeSleep",  m_finiSleep=0);
      declareProperty("TerminateSleep",  m_termSleep=0);
      declareProperty("PauseSleep",      m_pauseSleep=0);
      declareProperty("CancelSleep",     m_cancelSleep=0);
    }

    /// Destructor
    virtual ~TransitionSleepSvc()  {
      if ( m_termSleep>0 ) ::lib_rtl_sleep(m_termSleep*1000);
    }

    /// Incident handler implemenentation: Inform that a new incident has occured
    virtual void handle(const Incident& inc)    {
      info("Got incident:"+inc.source()+" of type "+inc.type());
      if ( inc.type() == "DAQ_PAUSE" )  {
        if ( m_pauseSleep>0 ) ::lib_rtl_sleep(m_pauseSleep*1000);
      }
      else if ( inc.type() == "DAQ_CANCEL" )  {
        if ( m_cancelSleep>0 ) ::lib_rtl_sleep(m_cancelSleep*1000);
      }
    }

    /// Initialize the service
    virtual StatusCode initialize()  {
      StatusCode sc = OnlineService::initialize();
      if ( m_initSleep>0 ) ::lib_rtl_sleep(m_initSleep*1000);
      return sc;
    }

    /// Start the service
    virtual StatusCode start()  {
      if ( m_startSleep>0 ) ::lib_rtl_sleep(m_startSleep*1000);
      return OnlineService::start();
    }

    /// Pause the service
    virtual StatusCode pause()  {
      if ( m_pauseSleep>0 ) ::lib_rtl_sleep(m_pauseSleep*1000);
      return StatusCode::SUCCESS;
    }

    /// Stop the service
    virtual StatusCode stop()  {
      if ( m_stopSleep>0 ) ::lib_rtl_sleep(m_stopSleep*1000);
      return OnlineService::stop();
    }

    /// Finalize the service
    virtual StatusCode finalize()  {
      if ( m_finiSleep>0 ) ::lib_rtl_sleep(m_finiSleep*1000);
      return OnlineService::finalize();
    }
  };

}

#include "GaudiKernel/SvcFactory.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,TransitionSleepSvc)
