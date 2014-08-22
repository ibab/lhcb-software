// $Id: RTLThreadTestAlg.cpp,v 1.2 2006-11-22 16:33:26 frankb Exp $
// Include files from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "RTL/rtl.h"

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

  /** @class RTLThreadTestAlg RTLThreadTestAlg.h tests/RTLThreadTestAlg.h
  *  Creates and fills dummy RawEvent  
  *
  *  @author Markus Frank
  *  @date   2005-10-13
  */
  class RTLThreadTestAlg : public Algorithm {
    typedef std::vector<lib_rtl_thread_t> Threads;
    typedef std::pair<RTLThreadTestAlg*,int*> Arg;
    int m_thrAtInit, m_thrAtStart, m_init, m_start;
    Threads m_threadsInit, m_threadsStart;

  public: 
    /// Standard constructor
    RTLThreadTestAlg(const std::string& nam,ISvcLocator* pSvc) : Algorithm(nam,pSvc)    {
      declareProperty("ThreadsAtInit",m_thrAtInit=0);
      declareProperty("ThreadsAtStart",m_thrAtStart=0);
    }
    /// Destructor
    virtual ~RTLThreadTestAlg()  {} 

    static int _run(void* arg)   {
      Arg* a = (Arg*)arg;
      for(long cnt = 0; *(a->second); ++cnt) 
	::lib_rtl_sleep(10);
      delete a;
      return 1;
    }

    StatusCode startThreads(Threads& c, int& flag, int cnt)  {
      flag = 1;
      for(int i=0; i<cnt; ++i)   {
	lib_rtl_thread_t h = 0;
	::lib_rtl_start_thread(this->_run,new Arg(this,&flag),&h);
	c.push_back(h);
      }
      return StatusCode::SUCCESS;
    }
    StatusCode stopThreads(Threads& c, int& flag, int cnt)  {
      flag = 0;
      for(int i=0; i<cnt; ++i) ::lib_rtl_delete_thread(c[i]);
      c.clear();
      return StatusCode::SUCCESS;
    }

    /// Main execution
    virtual StatusCode initialize() {  return startThreads(m_threadsInit,m_init,m_thrAtInit);   }
    /// Main execution
    virtual StatusCode finalize()   {  return stopThreads(m_threadsInit,m_init,m_thrAtInit);    }
    /// Main execution
    virtual StatusCode start()      {  return startThreads(m_threadsStart,m_start,m_thrAtStart);  }
    /// Main execution
    virtual StatusCode stop()       {  return stopThreads(m_threadsStart,m_start,m_thrAtStart);   }
    /// Main execution
    virtual StatusCode execute()    {  return StatusCode::SUCCESS;    }
  };
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,RTLThreadTestAlg)
