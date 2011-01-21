// $Id: EnvironDumper.cpp,v 1.1 2009-12-03 19:05:31 frankb Exp $

// Include files from Gaudi
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/Algorithm.h" 
#include "RTL/rtl.h"

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

  /** @class EnvironDumper EnvironDumper.h GaudiOnline/EnvironDumper.h
  *  Dump environment variables according to job options
  *
  *  @author Markus Frank
  *  @date   2005-10-13
  */
  class EnvironDumper : public Algorithm {
  protected:
    /// Property: Probability to send signal. If < 0, signal is always sent
    std::vector<std::string> m_env;

    /// Dump the environment
    StatusCode dump()  {
      typedef std::vector<std::string> _V;
      MsgStream log(msgSvc(),name());
      log << MSG::ALWAYS;
      for(_V::const_iterator i=m_env.begin(); i!=m_env.end(); ++i) {
	const std::string& e = *i;
	const char* val = ::getenv(e.c_str());
	log << "Environment variable: " << e << "=" 
	    << (const char*)(val ? val : "****Undefined****") << endmsg;
	std::cerr << "Environment variable: " << e << "=" 
		  << (const char*)(val ? val : "****Undefined****") << std::endl;
      }
      return StatusCode::SUCCESS;
    }
  public: 
    /// Standard constructor
    EnvironDumper(const std::string& nam,ISvcLocator* pSvc) : Algorithm(nam,pSvc)  {
      declareProperty("Environment",m_env);
    }
    /// Destructor
    virtual ~EnvironDumper()  {} 
    /// Initialization
    virtual StatusCode initialize() { return dump(); }
    /// Start
    virtual StatusCode start()      { return dump(); }
    /// Start
    virtual StatusCode stop()       { return dump(); }
    /// Start
    virtual StatusCode finalize()   { return dump(); }
    /// Main execution
    virtual StatusCode execute()    {      return StatusCode::SUCCESS;     }
  };
}

#include "GaudiKernel/AlgFactory.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,EnvironDumper)
