// $Id: DelaySleepAlg.cpp,v 1.1 2006-10-02 14:48:09 frankb Exp $
// Include files from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "RTL/rtl.h"

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

  /** @class DelaySleepAlg DelaySleepAlg.h tests/DelaySleepAlg.h
  *  Creates and fills dummy RawEvent  
  *
  *  @author Markus Frank
  *  @date   2005-10-13
  */
  class DelaySleepAlg : public Algorithm {
    int m_delay;
  public: 
    /// Standard constructor
    DelaySleepAlg(const std::string& nam,ISvcLocator* pSvc) : Algorithm(nam,pSvc)
    {  declareProperty("DelayTime",m_delay=1);          }
    /// Destructor
    virtual ~DelaySleepAlg()  {} 
    /// Main execution
    virtual StatusCode execute()  {
      if ( m_delay > 0 ) lib_rtl_sleep(m_delay);
      return StatusCode::SUCCESS;
    }
  };
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,DelaySleepAlg)
