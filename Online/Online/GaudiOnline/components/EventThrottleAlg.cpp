// $Id: TimeoutAlg.cpp,v 1.7 2010-03-03 13:16:49 frankb Exp $
// Include files from Gaudi
#include "GaudiKernel/Algorithm.h" 
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
      declareProperty("MinTime", m_minTime=1000);
    }

    /// Destructor
    virtual ~EventThrottleAlg()  {
    }

    /// Main execution
    virtual StatusCode execute()  {
      typedef long long int llint;
      struct timeval now;
      ::gettimeofday(&now,0);
      llint diff = llint((now.tv_sec - m_last.tv_sec )*1000000) + llint(now.tv_usec-m_last.tv_usec);
      if ( diff < m_minTime*1000 )  {
        ::lib_rtl_usleep(m_minTime*1000-diff);
      }
      m_last = now;
      return StatusCode::SUCCESS;
    }
  };
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,EventThrottleAlg)
