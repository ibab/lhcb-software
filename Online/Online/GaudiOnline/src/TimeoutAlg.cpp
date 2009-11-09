// $Id: TimeoutAlg.cpp,v 1.1 2009-11-09 17:47:11 frankb Exp $
// Include files from Gaudi
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/GaudiException.h" 
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/IIncidentListener.h" 
#include "RTL/rtl.h"

#ifndef _WIN32
#include <signal.h>
#include <pthread.h>
#include <execinfo.h>
#include <iomanip>
#endif

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

  /** @class TimeoutAlg TimeoutAlg.h tests/TimeoutAlg.h
  *  Creates and fills dummy RawEvent  
  *
  *  @author Markus Frank
  *  @date   2005-10-13
  */
  class TimeoutAlg : public Algorithm, virtual public IIncidentListener {

    /// Property: Name of the incident used to start the timer
    std::string   m_startIncident;
    /// Property: Optional name of the incident used to stop the timer
    std::string   m_cancelIncident;
    /// Property: Monitoring service type
    std::string   m_monitorSvcType;
    /// Property: Timeout value in milliseconds
    int           m_timeout;
    /// Reference to incident service
    IIncidentSvc* m_incidentSvc;
    /// Timer ID once timer is set
    unsigned long m_timerID;
    /// Timeout counter
    int           m_timeoutCount;
    /// Flag to print backtrace
    bool          m_printTrace;
#ifdef __WIN32
    void startTimer() {}
    void stopTimer() {}
#else
    /// Thread ID of event executing thread
    pthread_t   m_me;
    sigset_t    m_blockMask;
    struct sigaction m_sigact;
    struct sigaction m_oldact;

    static      std::vector<TimeoutAlg*> m_handlers;

    /** @class ExceptionTracer
     */
    struct ExceptionTracer {
      ExceptionTracer(IMessageSvc* svc=0, const std::string& nam="TimeoutAlg")     {
	void * array[25];
	MsgStream err(svc,nam);
	int nSize = backtrace(array, 35);
	char ** symbols = backtrace_symbols(array, nSize);
	for (int i = 0; i < nSize; i++)         {
	  if ( svc )
	    err << MSG::INFO << "===" << std::setw(3) << i << "  " << symbols[i] << endmsg;
	  else
	    std::cout << nam << " [INFO] ===" << std::setw(3) << i << "  " << symbols[i] << std::endl;
	}
	free(symbols);
      }
    };

    /// Static timer callback on timeout
    static int timeoutCall(void* p)   {
      TimeoutAlg* a = (TimeoutAlg*)p;
      a->m_timerID = 0;
      if ( a->m_me ) {
	a->m_handlers.push_back(a);
	//ExceptionTracer tr(a->msgSvc(),a->name());
	if ( 0 != ::pthread_kill(a->m_me,SIGALRM) ) {
	  MsgStream err(a->msgSvc(),a->name());
	  err << MSG::FATAL << "Failed to send SIGALRM to event processing thread!" << endmsg;
	  m_handlers.pop_back();
	  return 1;
	}
      }
      return 1;
    }

    /// In-thread callback to handle timeout signal sent by timeout callback to event loop thread
    static void timeoutHandler(int /* signum */)   {
      char text[256];
      if ( m_handlers.size() > 0 ) {
	TimeoutAlg* a = m_handlers.front();
	m_handlers.erase(m_handlers.begin());
	::pthread_sigmask(SIG_SETMASK,&a->m_blockMask,0);
	::sprintf(text,"TIMEOUT during event processing after %d milliseconds",a->m_timeout);
	++a->m_timeoutCount;
	if ( a->m_printTrace )  {
	  ExceptionTracer tr(a->msgSvc(),a->name());
	}
	throw GaudiException(text,"EVENT_TIMEOUT",StatusCode::FAILURE);
      }
      std::cout << "ERROR << Timeout handler size is NULL!!" << std::endl;
      ExceptionTracer tr(0,"TimeoutAlg");
    }

    /// Start new timer 
    void startTimer() {
      m_me = ::pthread_self();
      ::pthread_sigmask(SIG_BLOCK,0,&m_blockMask);
      ::lib_rtl_set_timer(m_timeout,timeoutCall,this,&m_timerID);
    }
    /// Stop current timer if running
    void stopTimer() {
      if ( m_timerID ) {
	::lib_rtl_kill_timer(m_timerID);
	::pthread_sigmask(SIG_SETMASK,&m_blockMask,0);
	m_timerID = 0;
	m_me = 0;
      }
    }
#endif

  public:
    /// Standard constructor
    TimeoutAlg(const std::string& nam,ISvcLocator* pSvc) 
      : Algorithm(nam,pSvc), m_timeout(10000), m_incidentSvc(0), 
	m_timerID(0), m_timeoutCount(0), m_me(0)
    {
      declareProperty("Timeout",m_timeout);
      declareProperty("PrintTrace",m_printTrace=true);
      declareProperty("StartIncident",m_startIncident=IncidentType::BeginEvent);
      declareProperty("CancelIncident",m_cancelIncident=IncidentType::EndEvent);
    }

    /// Destructor
    virtual ~TimeoutAlg()  {
    }

    /// IInterface implementation: Query interface
    virtual StatusCode queryInterface(const InterfaceID& riid,void** ppIf) {
      if ( riid.versionMatch(IIncidentListener::interfaceID()) )  {
	*ppIf = (IIncidentListener*)this;
	addRef();
	return StatusCode::SUCCESS;
      }
      return Algorithm::queryInterface(riid, ppIf);
    }

    /// Algorithm overload: initialize the algorithm
    virtual StatusCode initialize() {
      MsgStream err(msgSvc(),name());
      StatusCode sc = service("IncidentSvc",m_incidentSvc,true);
      if ( !sc.isSuccess() )  {
	err << MSG::ERROR << "Cannot access incident service." << endmsg;
	return sc;
      }
      m_incidentSvc->addListener(this,m_startIncident);
      m_incidentSvc->addListener(this,m_cancelIncident);
      m_incidentSvc->addListener(this,"EVENT_TIMEOUT");
      declareInfo("TimeoutCount",m_timeoutCount=0,"Number of timeouts processed");
      ::memset(&m_sigact,0,sizeof(m_sigact));
      m_sigact.sa_handler = timeoutHandler;
      ::sigemptyset(&m_sigact.sa_mask);
      ::sigaction(SIGALRM,&m_sigact,&m_oldact);
      return sc;
    }

    /// Algorithm overload: stop the algorithm
    virtual StatusCode start() {
      m_me = 0;
      m_timerID = 0;
      m_timeoutCount = 0;
      return StatusCode::SUCCESS;
    }

    /// Algorithm overload: stop the algorithm
    virtual StatusCode stop() {
      stopTimer();
      return StatusCode::SUCCESS;
    }

    /// Algorithm overload: finalize the algorithm
    virtual StatusCode finalize() {
      if ( monitorSvc() )  {
	monitorSvc()->undeclareAll(this);
      }
      if ( m_incidentSvc ) {
	m_incidentSvc->removeListener(this);
	m_incidentSvc->release();
	m_incidentSvc = 0;
      }
      return StatusCode::SUCCESS;
    }

    /// Main execution
    virtual StatusCode execute()  {
      stopTimer();
      return StatusCode::SUCCESS;
    }

    /// Incident handler implemenentation: Inform that a new incident has occured
    virtual void handle(const Incident& inc)    {
      if ( inc.type() == "EVENT_TIMEOUT" )  {
	MsgStream err(msgSvc(), name());
	err << MSG::FATAL << inc.type() << ": Timeout during event processing." << std::endl;
	++m_timeoutCount;
	stopTimer();
      }
      else if ( inc.type() == m_startIncident )  {
	startTimer();
      }
      else if ( inc.type() == m_cancelIncident ||
		inc.type() == IncidentType::AbortEvent )  {
	stopTimer();
      }
    }
  };
}

std::vector<LHCb::TimeoutAlg*> LHCb::TimeoutAlg::m_handlers;

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,TimeoutAlg)
