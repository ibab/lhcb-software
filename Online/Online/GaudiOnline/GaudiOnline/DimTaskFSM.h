#ifndef ONLINEKERNEL_DIMTASKFSM_H
#define ONLINEKERNEL_DIMTASKFSM_H

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ObjectFactory.h"
#include "CPP/Interactor.h"
#include <string>
class DimService;
class DimCommand;
class PropertyMgr;

/*
 * LHCb namespace declaration
 */
namespace LHCb  {
  /** @class DimTaskFSM DimTaskFSM.h GaudiOnline/DimTaskFSM.h
    *
    * The DimTaskFSM is an object to encapsulate applications,
    * which execute the basic FSM state machinery given by
    * the FSMs in the HLT farm.
    *  
    * These FSMs look like this:
    *
    *                     +----------+                        
    *                     |  UNKNOWN |                      
    *                     +----------+                         
    *                         |    ^                  
    *            DimTaskFSM() |    | unload()
    *                         v    |                  
    *                     +----------+       +---------+
    *                     | NOT_READY|<------+  ERROR  |<------+
    *                     +----------+       +---------+       |
    *                         |  ^                             |
    *                         |  |                             |
    *              configure()|  |reset()              error() |
    *                         V  |                             |
    *                     +----------+                         |
    *                     | READY    |                         |
    *                     +----------+                         |
    *                         |  ^                             |
    *                 start() |  | stop()                      |
    *                         V  |                             |
    *                     +-----------+                        |
    *                     | RUNNING   |------------------------+
    *                     +-----------+
    *
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class DimTaskFSM : public Interactor, 
                     virtual public IRunable,
                     virtual public IAppMgrUI
  {
  public:
    enum Transitions { 
      CONFIGURE=1, 
      INITIALIZE, 
      ENABLE, 
      NEXTEVENT, 
      DISABLE, 
      PRESTOP, 
      FINALIZE,
      TERMINATE,
      UNLOAD, 
      ERROR,
      STARTUP_DONE
    };
    enum State  {
      ST_UNKNOWN,
      ST_LOADED,
      ST_NOT_READY,
      ST_READY,
      ST_RUNNING,
      ST_STOPPED,
      ST_ERROR
    };
    enum SubState  {
      SUCCESS_ACTION,
      EXEC_ACTION,
      FAILED_ACTION,
      UNKNOWN_ACTION
    };
  protected:
    /// Variable to contain object name (==constant)
    std::string   m_name;
    /// Variable to contain the state name
    std::string   m_stateName;
    /// Variable to contain the process name
    std::string   m_procName;
    /// Sub state
    std::string   m_subStateName;
    /// Pointer to dim command to treceive transition changes
    DimCommand*   m_command;
    /// Pointer to the dim service publishing the state
    DimService*   m_service;
    /// Pointer to the dim service publishing the sub-state
    DimService*   m_subStateService;
    /// Gaudi property manager
    PropertyMgr*  m_propertyMgr;
    /// Flag to indicate the event loop is "ON"
    bool          m_continue;
    /// Flag to indicate the event loop should be executed and rearmed.
    bool          m_haveEventLoop;
    /// Object reference count
    unsigned long m_refCount;

  public:
    /// Function to rearm the event loop
    StatusCode rearm();
    /// Print error message (returns FAILURE)
    StatusCode printErr(int flag, const char* fmt, ...);
    /// Declare FSM state
    StatusCode declareState(const std::string& new_state);
    /// Declare FSM state
    StatusCode declareState(State state);
    /// Declare FSM sub-state
    StatusCode declareSubState(SubState state);
    /// Declare FSM state
    StatusCode declareSubState(const std::string& new_state);
    /// Accessor to property manager
    PropertyMgr& propertyMgr()   { return *m_propertyMgr; }

  public:
    /// Initializing constructor                       (OFFLINE     -> Inactive)
    DimTaskFSM(IInterface*);
    /// Standard destructor
    virtual ~DimTaskFSM();
    /// Print overload
    virtual void output(int level, const char* s);
    /// Interactor overload: handle Sensor stimuli
    virtual void handle(const Event& ev);
    /// Cancel IO request
    virtual StatusCode cancel();
    /// Implmentation of IInterface::addRef
    virtual unsigned long addRef();
    /// Implmentation of IInterface::release
    virtual unsigned long release();
    /// Implementation of IInterface::queryInterface
    virtual StatusCode queryInterface(const InterfaceID& iid, void** pinterface);
    /// IRunable overload: Run the dim command and listen to stimuli
    virtual StatusCode run();
    /// IAppMgrUI overload: object name
    virtual const std::string& name() const        {    return m_name;      }
    /// IAppMgrUI overload: state name
    virtual const std::string& stateName() const   {    return m_stateName; }
    /// Startup done                                    (Offline    -> Inactive)
    virtual StatusCode startupDone();
    /// Configure application                           (Inactive   -> Configured)
    virtual StatusCode configure();
    /// Initialize the application                      (Configured -> Ready)
    virtual StatusCode initialize();
    /// Enable the event loop and event processing      (Ready      -> Running)
    virtual StatusCode enable();
    /// Process single event                            (Running    -> Running)
    virtual StatusCode nextEvent(int num_event);
    /// Disable the event loop                          (Running    -> Ready)
    virtual StatusCode disable();
    /// Finialize the application                       (Ready      -> Finalized)
    virtual StatusCode finalize();
    /// Terminate application, but do not exit process  (Finalized  -> Inactive)
    virtual StatusCode terminate(); 
    /// Disconnect process and exit                     (Inactive   -> OFFLINE )
    virtual StatusCode unload();
    /// Invoke transition to error state                ( ****      -> Error   )
    virtual StatusCode error();
  };
}
#endif // ONLINEKERNEL_DIMTASKFSM_H
