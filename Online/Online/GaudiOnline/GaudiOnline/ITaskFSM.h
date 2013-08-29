#ifndef ONLINEKERNEL_ITASKFSM_H
#define ONLINEKERNEL_ITASKFSM_H

#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/IAppMgrUI.h"

// Forward declarations
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
  class ITaskFSM : virtual public IRunable, virtual public IAppMgrUI   {
  public:
    enum Transitions { 
      CONFIGURE=1, 
      INITIALIZE, 
      START,
      ENABLE, 
      NEXTEVENT, 
      DISABLE, 
      PRESTOP, 
      STOP,
      FINALIZE,
      TERMINATE,
      UNLOAD, 
      PAUSE,
      CONTINUE,
      ERROR,
      STARTUP_DONE,
      CONNECT_DIM=1000
    };
    enum State  {
      ST_UNKNOWN   = 'U',
      ST_NOT_READY = 'N',
      ST_READY     = 'r',
      ST_RUNNING   = 'R',
      ST_STOPPED   = 'S',
      ST_PAUSED    = 'P',
      ST_ERROR     = 'E',
      TR_ERROR     = ERROR
    };
    enum SubState  {
      SUCCESS_ACTION = 'S',
      EXEC_ACTION    = 'E',
      FAILED_ACTION  = 'F',
      UNKNOWN_ACTION = 'U'
    };

  public:
    /// Print overload
    virtual void output(int level, const char* s) = 0;

    /// Clear current state name
    virtual void clearState() = 0;
    /// IAppMgrUI overload: state name
    virtual const std::string& stateName() const = 0;

    /// Declare FSM state
    virtual StatusCode _declareState(const std::string& new_state) = 0;
    /// Declare FSM state
    virtual StatusCode declareState(State state) = 0;
    /// Declare FSM sub-state
    virtual StatusCode declareSubState(SubState state) = 0;
    /// Set transition target state
    virtual void setTargetState(State target) = 0;
    /// (Re)connect DIM services
    virtual StatusCode connectDIM(DimCommand* cmd=0) = 0;
    /// Disconnect DIM services
    virtual StatusCode disconnectDIM() = 0;

    /// Cancel IO request
    virtual StatusCode cancel() = 0;
    /// Function to rearm the event loop
    virtual StatusCode rearm()  = 0;

    /// Enable the event loop and event processing      (Ready      -> Running)
    virtual StatusCode enable() = 0;
    /// Disconnect process and exit                     (Inactive   -> OFFLINE )
    virtual StatusCode unload() = 0;
    /// Invoke transition to error state                ( ****      -> Error   )
    virtual StatusCode error()  = 0;
  };
}
#endif // ONLINEKERNEL_ITASKFSM_H
