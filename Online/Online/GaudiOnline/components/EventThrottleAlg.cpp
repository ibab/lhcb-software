// $Id: TimeoutAlg.cpp,v 1.7 2010-03-03 13:16:49 frankb Exp $
// Include files from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/MsgStream.h"
#include "RTL/time.h"
#include "RTL/rtl.h"

using namespace std;

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

  /// Throttle the consumption of events in a Gaudi algorithms
  /** 
   *
   *  @author Markus Frank
   *  @date   2005-10-13
   */
  class EventThrottleAlg : public Algorithm {
    int m_minTime;
    struct timeval m_last;
  public:
    /// Standard constructor
    EventThrottleAlg(const string& nam,ISvcLocator* pSvc) 
      : Algorithm(nam,pSvc)
    {
      ::gettimeofday(&m_last,0);
      // Minimal time per event in milli seconds
      declareProperty("DelayTime", m_minTime=1000);
    }

    /// Destructor
    virtual ~EventThrottleAlg()  {
    }

    /// Main execution
    virtual StatusCode execute()  {
      typedef long long int llint;
      MsgStream log(msgSvc(), name());
      struct timeval now;
      ::gettimeofday(&now,0);
      llint diff   = llint((now.tv_sec - m_last.tv_sec )*1000000) + llint(now.tv_usec-m_last.tv_usec);
      llint min_tm = m_minTime;
      min_tm *= 1000;
      log << MSG::INFO << "Diff:" << diff << " min:" << min_tm 
	  << " --> Need to wait for " << (min_tm-diff) << "musec" << endmsg;
      if ( diff < min_tm )  {
        ::lib_rtl_usleep(min_tm-diff);
      }
      ::gettimeofday(&m_last,0);
      setFilterPassed(true);
      return StatusCode::SUCCESS;
    }
  };
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,EventThrottleAlg)
