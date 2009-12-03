// $Id: SignalAlg.cpp,v 1.1 2009-12-03 19:05:31 frankb Exp $

// Include files from Gaudi
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "RTL/rtl.h"

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

  /** @class SignalAlg SignalAlg.h tests/SignalAlg.h
  *  This algorithm send with a given probability 
  *
  *  @author Markus Frank
  *  @date   2005-10-13
  */
  class SignalAlg : public Algorithm {
    /// Event counter
    int   m_evtCount;
    /// Property: Signal number to be sent to event loop thread
    int   m_signo;
    /// Property: Probability to send signal. If < 0, signal is always sent
    float m_probability;
  public: 
    /// Standard constructor
    SignalAlg(const std::string& nam,ISvcLocator* pSvc) : Algorithm(nam,pSvc), m_evtCount(0)    {
      declareProperty("SignalNumber",m_signo=11);       // SIGSEGV
      declareProperty("Probability",m_probability=-1.);
      ::srand(123456);
    }
    /// Destructor
    virtual ~SignalAlg()  {} 
    /// Main execution
    virtual StatusCode execute()  {
      ++m_evtCount;
      if ( m_signo > 0 )   {
	bool do_signal = (m_probability <= 0) || ((float(rand())/float(RAND_MAX))<m_probability);
	if ( do_signal ) {
	  MsgStream log(msgSvc(),name());
	  log << MSG::ALWAYS << "Signal exception with signo:" << m_signo 
	      << " to event loop thread after " << m_evtCount << " events." << endmsg;
	  void* id = lib_rtl_thread_id();
	  ::lib_rtl_kill_thread(id,m_signo);
	}
      }
      return StatusCode::SUCCESS;
    }
  };
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,SignalAlg)
