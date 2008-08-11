#ifndef ONLINEKERNEL_GAUDITASK_H
#define ONLINEKERNEL_GAUDITASK_H

// Framework includes
#include "GaudiKernel/IIncidentListener.h"
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
    struct PythonInterpreter {
      PythonInterpreter();
      ~PythonInterpreter();
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
    bool              m_autostart;
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

    /// Access to message service object
    IMessageSvc* msgSvc();
    /// Print overload
    virtual void output(int level, const char* s);
    /// Set properties of application manager instance
    StatusCode setInstanceProperties(IAppMgrUI* inst);
    /// Start 2nd layer runable
    StatusCode startRunable(IRunable* runable);

    /// Configure standard C++ second level application manager
    StatusCode configSubManager();
    /// Configure Python based second level application manager
    StatusCode configPythonSubManager();

    /// Wait for runable thread to stop. If it does not, send kill signal
    void stopRunable();
    /// Internal helper: configure  application manager
    virtual int configApplication();
    /// Internal helper: initialize application manager
    virtual int initApplication();
    /// Internal helper: start application manager
    virtual int startApplication();
    /// Internal helper: stop application manager
    virtual int stopApplication();
    /// Internal helper: finalize   application manager
    virtual int finalizeApplication();
    /// Internal helper: terminate  application manager
    virtual int terminateApplication();

  public:
    /// Standard constructor
    GaudiTask(IInterface* svc);
    /// Default destructor
    virtual ~GaudiTask();
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
  };
}
#endif // ONLINEKERNEL_GAUDITASK_H
