#ifndef ONLINEKERNEL_GAUDITASK_H
#define ONLINEKERNEL_GAUDITASK_H

// Framework includes
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Service.h"
#include "GaudiOnline/DimTaskFSM.h"
#include "RTL/rtl.h"

// C++ include files
#include <memory>

// Forward declarations:
class IIncidentSvc;
class IMessageSvc;
class IAppMgrUI;

/*
 * LHCb namespace declaration
 */
namespace LHCb  {
  /** @class GaudiTask GaudiTask.h GaudiOnline/GaudiTask.h
    *
    * Basic object implementing the functionality of a task executing
    * the gaudi data processing environment.
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class GaudiTask : public DimTaskFSM, virtual public IIncidentListener  {
  protected:
    /// Python interpreter wrapper
    struct PythonInterpreter {
    private:
      /// State context
      void* m_state;
    public:
      /// Default constructor
      explicit PythonInterpreter();
      /// Default destructor
      ~PythonInterpreter();
      /// Special callback after fork
      static void afterFork();
    };

    /// Wrapper class for global python state
    struct PythonGlobalState {
    private:
      /// State context
      int m_state;
    public:
      /// Default constructor
      explicit PythonGlobalState();
      /// Default destructor
      ~PythonGlobalState();
    };

    /// Handle to python interpreter
    std::auto_ptr<PythonInterpreter> m_python;
    /// Handle to second layer execution thread
    lib_rtl_thread_t  m_handle;
    /// Property: name of runable object
    std::string       m_runable;
    /// Property: name of event loop object
    std::string       m_evtloop;
    /// Property: name of message service object
    std::string       m_msgsvcType;
    /// Property: main jonb options
    std::string       m_mainOptions;
    /// Property: optional options
    std::string       m_optOptions;
    /// Property: Auto start flag
    int               m_autostart;
    /// Main appliation manager object
    IAppMgrUI*        m_appMgr;
    /// Sub application manager object
    IAppMgrUI*        m_subMgr;
    /// Reference to incident service
    IIncidentSvc*     m_incidentSvc;
    /// Pointer to message service
    IMessageSvc*      m_msgSvc;
    /// Counter for consecutive errors during event processing
    int               m_nerr;
    /// Flag to indicate if runable has stopped executing
    bool              m_eventThread;
    /// Flag to ignore incidents on PAUSE/CONTINUE
    bool              m_ignoreIncident;

    /// Property: Flag to not declare state on DAQ_PAUSE incident (default: true)
    bool              m_declarePAUSE;
    /// Property: Flag to not declare state on DAQ_CONTINUE incident (default: true)
    bool              m_declareCONTINUE;
    /// Property: Flag to indicate that finalize/terminate should be(not be) called (default: true)
    bool              m_execFinalize;

    /// Static thread routine to execute a Gaudi runable
    static int execRunable(void* arg);

    /// Access to message service object
    IMessageSvc* msgSvc();
    /// Print overload
    virtual void output(int level, const char* s);
    /// Print overload
    virtual void output(int level, const std::string& s);
    /// Set properties of application manager instance
    StatusCode setInstanceProperties(IAppMgrUI* inst);
    /// Start 2nd layer runable
    StatusCode startRunable(IRunable* runable);

    /// Configure standard C++ second level application manager
    StatusCode configSubManager();
    /// Configure Python based second level application manager
    StatusCode configPythonSubManager();

    /// Wait for runable thread to stop. If it does not, send kill signal
    virtual void stopRunable();
    /// Internal helper: configure  application manager
    virtual int configApplication();
    /// Internal helper: initialize application manager
    virtual int initApplication();
    /// Internal helper: start application manager
    virtual int startApplication();
    /// Start second layer event processing runable for GAUDI Application
    virtual int goApplication();
    /// Internal helper: stop application manager
    virtual int stopApplication();
    /// Internal helper: finalize   application manager
    virtual int finalizeApplication();
    /// Internal helper: terminate  application manager
    virtual int terminateApplication();

  public:
    /// Configuration helper to use Gaudi for setting job options
    class Configuration : public Service    {
    public:
      /// Default constructor
      Configuration(const std::string& nam, ISvcLocator* svc) : Service(nam,svc)      {}
      /// Default destructor
      virtual ~Configuration()      {      }
      /// Initialize (sets object properties on initialize of the lower level instance)
      virtual StatusCode initialize();
    };

  public:
    /// Standard constructor
    GaudiTask(IInterface* svc);
    /// Default destructor
    virtual ~GaudiTask();

    /// Interactor overload: handle Sensor stimuli
    virtual void handle(const Event& ev)  { this->DimTaskFSM::handle(ev); }

    /// Incident handler implemenentation: Inform that a new incident has occured
    virtual void handle(const Incident& incident);

    /// Run the complete job (from intialize to terminate)
    virtual StatusCode run();

    /// Callback on exiting task
    virtual StatusCode unload();

    /// Callback on I/O cancel
    virtual StatusCode cancel();

    /// Enable the event loop and event processing      (Ready      -> Running)
    virtual StatusCode enable();

    /// Pause the application  ( RUNNING -> PAUSED )
    virtual StatusCode pause();    

    /// Continue the application  ( PAUSED -> RUNNING )
    virtual StatusCode continuing();

    /// Callback to process event
    virtual StatusCode nextEvent(int num_event);

    /// Access flag to indicate if runable has stopped executing
    bool eventThread() const { return m_eventThread; }

    /// Set flag to indicate if runable has stopped executing
    void setEventThread(bool val)  { m_eventThread = val; }

    /// Lock task object
    static void lock();
    /// Unlock task object
    static void unlock();

    /// Fire an incident from network interrupt
    virtual StatusCode fireIncident(const std::string& type);

  };
}
#endif // ONLINEKERNEL_GAUDITASK_H
