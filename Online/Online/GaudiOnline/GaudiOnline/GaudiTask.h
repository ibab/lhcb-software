#ifndef ONLINEKERNEL_GAUDITASK_H
#define ONLINEKERNEL_GAUDITASK_H

// Framework includes
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiOnline/DimTaskFSM.h"
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
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class GaudiTask : public DimTaskFSM, virtual public IIncidentListener  {
  protected:
    /// Property: name of runable object
    std::string   m_runable;
    /// Property: name of event loop object
    std::string   m_evtloop;
    /// Property: name of message service object
    std::string   m_msgsvcType;
    /// Property: main jonb options
    std::string   m_mainOptions;
    /// Property: optional options
    std::string   m_optOptions;
    /// Main appliation manager object
    IAppMgrUI*    m_appMgr;
    /// Sub application manager object
    IAppMgrUI*    m_subMgr;
    /// Reference to incident service
    IIncidentSvc* m_incidentSvc;
    /// Pointer to message service
    IMessageSvc*  m_msgSvc;

    /// Access to message service object
    IMessageSvc* msgSvc();
    /// Print overload
    virtual void output(const char* s);
    /// Set properties of application manager instance
    StatusCode setInstanceProperties(IAppMgrUI* inst);
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
    /// Callback on configure transition
    virtual StatusCode configure();
    /// Callback on start transition
    virtual StatusCode initialize();
    /// Callback to process event
    virtual StatusCode nextEvent(int num_event);
    /// Callback on stop transition
    virtual StatusCode finalize();
    /// Callback on reset transition
    virtual StatusCode terminate();
  };
}
#endif // ONLINEKERNEL_GAUDITASK_H
