#ifndef ONLINEKERNEL_DIMTASKFSM_H
#define ONLINEKERNEL_DIMTASKFSM_H

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ObjectFactory.h"
#include "GaudiOnline/ITaskFSM.h"
#include "CPP/Interactor.h"

// Forward declarations
class DimService;
class DimCommand;

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
    *
    *                     +----------+                        
    *                     |  UNKNOWN |                      
    *                     +----------+                         
    *                         |  ^                  
    *            DimTaskFSM() |  | unload()
    *                         |  |        
    *                         v  |                  
    *                     +----------+       +---------+
    *                     | NOT_READY|<------+  ERROR  |<------+
    *                     +----------+       +---------+       |
    *                         |  ^                             |
    *                         |  |                             |
    *              configure()|  |reset()              error() |
    *                         |  |                             |
    *                         |  |                             |
    *                         V  |                             |
    *        activate()   +----------+                         |
    *      +--------------| READY    |                         |
    *      |              +----------+                         |
    *      |                  |  ^                             |
    *      V                  |  |                             |
    *  +---------+            |  |                             |
    *  | ACTIVE  |            |  |                             |
    *  +---------+            |  |                             |
    *      |                  |  |                             |
    *      |          start() |  | stop()                      |
    *      |                  V  |                             |
    *      | go()         +-----------+                        |
    *      +------------->| RUNNING   |------------------------+
    *                     +-----------+
    *                         |  ^                             
    *                         |  |                             
    *               pause()   |  |  continue()                             
    *                         |  |                             
    *                         V  |                             
    *                     +-----------+
    *                     | PAUSED    |
    *                     +-----------+
    *
    *
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class DimTaskFSM : public Interactor, virtual public ITaskFSM  {
  public:
    struct FSMMonitoring {
      unsigned long lastCmd, doneCmd;
      int pid;
      char targetState, state, metaState, pad;
      int partitionID;
    } m_monitor;

  protected:
    /// Variable to contain object name (==constant)
    std::string   m_name;
    /// Variable to contain the state name
    std::string   m_stateName;
    /// Variable to contain the previous state name
    std::string   m_prevStateName;
    /// Pointer to dim command to treceive transition changes
    DimCommand*   m_command;
    /// Pointer to the dim service publishing the state
    DimService*   m_service;
    /// Pointer to the dim service publishing the sub-state
    DimService*   m_fsmService;
    /// Gaudi property manager
    PropertyMgr*  m_propertyMgr;
    /// Flag to indicate the event loop is "ON"
    bool          m_continue;
    /// Flag to indicate the event loop should be executed and rearmed.
    bool          m_haveEventLoop;
    /// Object reference count
    unsigned long m_refCount;

  public:
    /// Print error message (returns FAILURE)
    StatusCode printErr(int flag, const char* fmt, ...);
    /// Declare FSM state
    virtual StatusCode _declareState(const std::string& new_state);
    /// Accessor to property manager
    PropertyMgr& propertyMgr()   { return *m_propertyMgr; }
    /// Translate integer state to string name
    static std::string stateName(int state);

    ///  ITaskFSM overload: Clear current state name
    virtual void clearState() { m_stateName = ""; } 

  public:
    /// Initializing constructor                       (OFFLINE     -> Inactive)
    DimTaskFSM(IInterface*, bool connect_dim=true);
    /// Standard destructor
    virtual ~DimTaskFSM();
    /// Print overload
    virtual void output(int level, const char* s);

    /// Interactor overload: handle Sensor stimuli
    virtual void handle(const Event& ev);

    /// ITaskFSM overload: Cancel IO request/Event loop
    virtual StatusCode cancel();
    /// ITaskFSM overload: Function to rearm the event loop
    virtual StatusCode rearm();

    /// ITaskFSM overload: Declare FSM state
    virtual StatusCode declareState(State state);
    /// ITaskFSM overload: Declare FSM sub-state
    virtual StatusCode declareSubState(SubState state);
    /// ITaskFSM overload: Set transition target state
    virtual void setTargetState(State target) { m_monitor.targetState = char(target); }

    /// ITaskFSM overload: (Re)connect DIM services
    virtual StatusCode connectDIM(DimCommand* cmd=0);
    /// ITaskFSM overload: Disconnect DIM services
    virtual StatusCode disconnectDIM();

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
    /// Reinitialize the application
    virtual StatusCode reinitialize();

    /// Start the application
    virtual StatusCode start();
    /// Callback on "activate" command
    virtual StatusCode activate();
    /// Callback on "go" command
    virtual StatusCode go();
    /// Restart the application
    virtual StatusCode restart();
    /// Stop the application
    virtual StatusCode stop();

    /// Pause the application  ( RUNNING -> PAUSED )
    virtual StatusCode pause();    
    /// Wok call on Pause  ( RUNNING -> PAUSED )
    virtual StatusCode pauseProcessing();    
    /// Continue the application  ( PAUSED -> RUNNING )
    virtual StatusCode continuing();
    /// Continue the application  ( PAUSED -> RUNNING )
    virtual StatusCode continueProcessing();

    /// Access FSM state
    virtual Gaudi::StateMachine::State FSMState() const;

    /// ITaskFSM overload: Enable the event loop and event processing (Ready -> Running)
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

    /// Fire an incident from network interrupt
    virtual StatusCode fireIncident(const std::string& type);
  };
}
#endif // ONLINEKERNEL_DIMTASKFSM_H
