#ifndef ONLINEKERNEL_DIMTASKFSM_H
#define ONLINEKERNEL_DIMTASKFSM_H

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "CPP/Interactor.h"
#include <string>
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
    *                     +----------+                        
    *                     |  OFFLINE |                      
    *                     +----------+                         
    *                         |    ^                  
    *            DimTaskFSM() |    | disconnect()   
    *                         v    |                  
    *                     +----------+       +---------+
    *                     | Inactive |<------+  Error  |<------+
    *              +----->|          |---+   +---------+       |
    *              |      +----------+   |                     |
    *              |  error()            |                     |
    *   terminate()|   ^     configure() |             error() |
    *              |   |                 v                     |
    *       +-------------+        +-------------+             |
    *       |  Finalized  |        |  Configured |-------------+
    *       +-------------+        +-------------+             |
    *              |                     |                     |
    *   finalize() |        initialize() |                     |
    *              |                     |                     |
    *              |      +---------+    |                     |
    *              +------|  Ready  |<---+                     |
    *                     |         |--------------------------+
    *                     +---------+                          |
    *                       |    ^                             |
    *             enable()  |    |  disable()                  |
    *                       v    |                             |
    *                   +-------------+                        |
    *                   |   Running   |------------------------+
    *                   +-------------+
    *
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class DimTaskFSM : public Interactor  {
  public:
    enum Transitions { 
      CONFIGURE=1, 
      INITIALIZE, 
      ENABLE, 
      PROCESS, 
      DISABLE, 
      PRESTOP, 
      FINALIZE,
      TERMINATE,
      DISCONNECT, 
      ERROR
    };

  protected:
    /// Variable to contain the state name
    std::string m_stateName;
    /// Variable to contain the process name
    std::string m_procName;
    /// Pointer to dim command to treceive transition changes
    DimCommand* m_command;
    /// Pointer to the dim service publishing the state
    DimService* m_service;
    /// Flag to indicate the event loop is "ON"
    bool        m_continue;
    /// Flag to indicate the event loop should be executed and rearmed.
    bool        m_haveEventLoop;

  protected:
    /// Function to rearm the event loop
    StatusCode rearm();
    /// Print error message (returns FAILURE)
    StatusCode printErr(int flag, const std::string& msg);
    /// Declare FSM state
    void declareState(const std::string& new_state);
    /// Printout overload
    size_t print(const char* fmt,...);
    /// Print overload
    virtual void output(const char* s);

  public:
    /// Initializing constructor                       (OFFLINE     -> Inactive)
    DimTaskFSM(bool loop = false);
    /// Standard destructor
    virtual ~DimTaskFSM();
    /// Run the dim command and listen to stimuli
    virtual StatusCode run() const;
    /// Interactor overload: handle Sensor stimuli
    virtual void handle(const Event& ev);
    /// Cancel IO request
    virtual StatusCode cancel();
    /// Configure application                           (Inactive   -> Configured)
    virtual StatusCode config();
    /// Initialize the application                      (Configured -> Ready)
    virtual StatusCode init();
    /// Enable the event loop and event processing      (Ready      -> Running)
    virtual StatusCode enable();
    /// Process single event                            (Running    -> Running)
    virtual StatusCode process();
    /// Disable the event loop                          (Running    -> Ready)
    virtual StatusCode disable();
    /// Finialize the application                       (Ready      -> Finalized)
    virtual StatusCode finalize();
    /// Terminate application, but do not exit process  (Finalized  -> Inactive)
    virtual StatusCode terminate(); 
    /// Disconnect process and exit                     (Inactive   -> OFFLINE )
    virtual StatusCode disconnect();
    /// Invoke transition to error state                ( ****      -> Error   )
    virtual StatusCode error();
  };
}
#endif // ONLINEKERNEL_DIMTASKFSM_H
