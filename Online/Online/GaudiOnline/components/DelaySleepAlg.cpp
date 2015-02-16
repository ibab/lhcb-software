// $Id: DelaySleepAlg.cpp,v 1.2 2006-11-22 16:33:26 frankb Exp $
// Include files from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "RTL/rtl.h"

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

  /// Online helper algorithm to create programmable time delays
  /**
  *  Online helper algorithm to create artificial time delays during 
  *  the various transitions of a Gaudi based process.
  *  Trivial, but extremely helpful.
  *
  *  The delays can be set for all transitions using job options.
  *  Delay values are expressed in MILLI-SECONDS!
  *
  *
  *  \author  Markus Frank
  *  \date    2005-10-13
  *  \version 1.1
  */
  class DelaySleepAlg : public Algorithm {
    int m_delay, m_muDelay;
    int m_initDelay, m_finiDelay, m_stopDelay, m_startDelay;
  public: 
    /// Standard constructor
    DelaySleepAlg(const std::string& nam,ISvcLocator* pSvc) : Algorithm(nam,pSvc)
    {
      /// Delay times during ramping up/down:
      declareProperty("InitializeDelay",  m_initDelay=0);
      declareProperty("StartDelay",       m_startDelay=0);
      declareProperty("StopDelay",        m_stopDelay=0);
      declareProperty("FinalizeDelay",    m_finiDelay=0);

      /// Delay times for event processing:
      declareProperty("DelayTime",      m_delay=1);
      declareProperty("MicroDelayTime", m_muDelay=0);          
    }
    /// Destructor
    virtual ~DelaySleepAlg()  {
    } 

    StatusCode _halt(int micro_seconds)    {
      if ( micro_seconds )
	::lib_rtl_usleep(micro_seconds);
      return StatusCode::SUCCESS;
    }

    /// Initialization
    virtual StatusCode initialize()  {  return _halt(m_initDelay*1000);        }
    /// Start
    virtual StatusCode start()       {  return _halt(m_startDelay*1000);       }
    /// Stop
    virtual StatusCode stop()        {  return _halt(m_stopDelay*1000);        }
    /// Finalization
    virtual StatusCode finalize()    {  return _halt(m_finiDelay*1000);        }
    /// Main execution
    virtual StatusCode execute()     {  return _halt(m_delay*1000+m_muDelay);  }
  };
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,DelaySleepAlg)
