/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/
#ifndef ONLINE_FINITESTATEMACHINE_MACHINE_H
#define ONLINE_FINITESTATEMACHINE_MACHINE_H

// Framework include files
#include "CPP/Event.h"
#include "CPP/Interactor.h"
#include "FiniteStateMachine/Type.h"
#include "FiniteStateMachine/State.h"
#include "FiniteStateMachine/Transition.h"

// C/C++ include files
#include <string>
#include <vector>
#include <map>
#include <set>

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine {

  // Forward declarations
  class Slave;

  /**@class Machine  Machine.h FiniteStateMachine/Machine.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class Machine : public TypedObject, public CPP::Interactor    {
  public:
    typedef FSM::ErrCond ErrCond;
    /// StateActions container definition
    typedef std::map<const State*,StateActions>            StateActionMap;
    /// TransitionActions container definition
    typedef std::map<const Transition*,TransitionActions>  TransitionActionMap;
    /// Container of slaves States
    typedef std::set<const State*>                         States;
    /// Container of slaves
    typedef std::vector<Slave*>                            Slaves;

    /// Metastates and Action codes of the FSMmachine object
    enum MachineState  {
      MACH_WAIT_NULL  =  1<<1,
      MACH_IDLE       =  1<<2,        /// Machine IDLE
      MACH_OUTACTION  =  1<<3,        /// Executing state leave action
      MACH_ACTIVE     =  1<<4,
      MACH_CHK_SLV    =  1<<5,        /// Machine is checking if all slave tasks completed the transition
      MACH_EXEC_ACT   =  1<<6,        /// Executing Action
      MACH_INACTION   =  1<<7,        /// Executing state enter action
      MACH_FAIL       =  1<<8         /// Code used on failure
    };
  protected:
    /// Internal finite state machine implementation
    FSM                 m_fsm;
    /// Reference to the State structure of the state the machine is in
    const State*        m_currState;
    /// Hold pointer of current Transition
    const Transition*   m_currTrans;
    /// Set of StateActions assigned to this machine  
    StateActionMap      m_stateActions;
    /// Set of TransActions assigned to this machine
    TransitionActionMap m_transActions;
    /// Set of depending slave FSMs
    Slaves              m_slaves;
    /// General callback always called when a transition completes
    Callback            m_completion;
    /// General callback always called when a transition fails
    Callback            m_fail;
    /// General callback always called when a slaves get manipulated
    Callback            m_meta;
    /// Falg to indicate command reversal
    Rule::Direction     m_direction;

  public:
    /** Class Constructor
     *
     * @arg typ     [pointer, read-only]   FSM type
     * @arg nam     [string, read-only]    State name
     */
    Machine(const Type *typ, const std::string& nam);
    
    /// Standatrd destructor  
    virtual ~Machine();

    /// Set new machine name
    void setName(const std::string& nam)          {  m_name = nam;                             }
    /// Retrieve reference to current State structure
    const State* state ()  const                  {  return m_currState;                       }
    /// Retrieve reference to current State structure name
    const char*  c_state ()  const                {  return m_currState->c_name();             }
    /// Update current state
    Machine& setState (const State* state)        {  m_currState = state; return *this;        }
    /// Retrieve reference of current Transition structure
    const Transition* currTrans()  const          {  return m_currTrans;                       }
    /// Store the current transition
    Machine& setTarget (const Transition* trans)  {  m_currTrans = trans; return *this;        }
    /// Access array of slaves (CONST)
    const Slaves& slaves()  const                 {  return m_slaves;                          }
    /// Access array of slaves
    Slaves& slaves()                              {  return m_slaves;                          }
    /// Check if the machine is in idle state
    bool isIdle()  const                          {  return m_fsm.currentState() == MACH_IDLE; }
    /// Meta state name of current meta-state
    const char* currentMetaName()  const;
    /// Meta state name of previous meta-state
    const char* previousMetaName()  const;
    /// Meta state name of target meta-state
    const char* targetMetaName()  const;
    
    /// Set all slave matching the meta state mask to the real FSM state.
    int  setSlaveState(int meta_mask, const State* state);
    /// Collect the states of all slaves
    const States slaveStates() const;
    /// Evaluate the when rules accoding to the slave states and invoke transition if required.
    bool evaluateWhens();

    /// Add a slave to this FSM  machine
    ErrCond addSlave(Slave* slave);
    /// Remove a slave from this FSM  machine
    ErrCond removeSlave(Slave* slave);

    /** Specific Interactor handle to act on sensor interrupts
     *  @arg  ev    [Event,read-only]   Event structure to be handled
     */
    virtual void handle(const CPP::Event& event);
    /** Handle network events for the Machine object
     *  @arg  ev    [Event,read-only]   Event structure to be handled
     */
    virtual void handleNetEvent(const CPP::Event& event);
    /// IOC handler
    virtual void handleIoc(const CPP::Event& event);
    
    /// Invoke meta transition on FSM object
    ErrCond invokeMetaTransition(unsigned int target, const void* param);
    /// Invoke FSM transition
    ErrCond invokeTransition(const Transition* transition, Rule::Direction dir=Rule::MASTER2SLAVE);
    /// Invoke FSM transition to target state
    ErrCond invokeTransitionByName(const std::string& state, Rule::Direction dir=Rule::MASTER2SLAVE);
    /// Invoke FSM transition to target state
    ErrCond invokeTransition(const State* state, Rule::Direction dir=Rule::MASTER2SLAVE);
    /// Invoke FSM transition to target state
    ErrCond invokeTransition(const std::string& state, Rule::Direction dir=Rule::MASTER2SLAVE);
    /// Check predicates and apply rules to slaves
    ErrCond startTransition();
    /// Check slave status. Once all slaves have answered, invoke transition
    ErrCond checkSlaves();
    /// Execute state leave action
    ErrCond finishTransition (const void* user_param);

    /// Callback executed on a failure of a metastate change
    ErrCond metaTransitionFail();
    /// Callback executed if the current transition is not availible
    ErrCond metaTransitionMissing();
    /// Action routine to be called if a transition is ended and the machine is idle.
    ErrCond goIdle();
    /// Finish failed transition and return to the IDLE status
    ErrCond goIdleFromFail();

    /// Null action
    virtual ErrCond nullAction()  const;
    /// Start/kill slaves if required by transition descriptor
    virtual ErrCond handleSlaves();
    /// Start slaves if required by transition descriptor
    virtual ErrCond checkAliveSlaves();
    /// Execute state leave action
    virtual ErrCond doOutAction(const void* user_param);
    /// Execute state enter action at the end of the transition
    virtual ErrCond doInAction (const void* user_param);
    /// Execute transition action
    virtual ErrCond doAction (const void* user_param);
    /// Execute failure action
    virtual ErrCond doFail(const void* user_param);

    /// Finish any lengthy action
    virtual ErrCond finishAnyAction(ErrCond status, int next_state);
    /// Finish lengthy state leave action
    virtual ErrCond finishOutAction(ErrCond status);
    /// Finish lengthy state leave action
    virtual ErrCond finishAction(ErrCond status);
    /// Finish lengthy state leave action
    virtual ErrCond finishInAction(ErrCond status);

    /// Update callback structures always called when a transition fails
    Machine& setFailAction (const Callback& action);
    /// Update callback structures always called when a transition completes
    Machine& setCompletionAction (const Callback& action);
    /// Update callback structures always called when slaves get manipulated
    Machine& setMetaStateAction (const Callback& action);
    /// Update callback structures for state enter action. 
    Machine& setInAction (const State* state, const Callback& action);
    /// Update callback structures for state leave action. 
    Machine& setOutAction (const State* state, const Callback& action);
    /// Update action callback. If there where already action callbacks installed, delete them.
    Machine& setAction(const Transition* transition, const Callback& action);
    /// Update pre-action callback. If there where already action callbacks installed, delete them.
    Machine& setPreAction(const Transition* transition, const Callback& action);
    /// Update fail-action callback. If there where already action callbacks installed, delete them.
    Machine& setFailAction(const Transition* transition, const Callback& action);
    /// Update completion-action callback. If there where already action callbacks installed, delete them.
    Machine& setCompletionAction(const Transition* transition, const Callback& action);

    /** Assign machine specific state enter/leave action
     *  @arg state_name  [string,read-only]    Name of the state the action callbacks should be assigned to
     *  @arg in          [Callback,read-only]  Callback structure for state in action
     *  @arg out         [Callback,read-only]  Callback structure for state out action
     *  @return  Status code indicating success or failure
     */
    Machine& setStateActions (const State* state,const Callback& in,const Callback& out);

    /** Assign machine specific transition actions
     *  @arg Transition  [Transition* read-only] Transition the action callbacks should be assigned to
     *  @arg pre_action  [Callback,read-only]    Callback structure for transition pre-action
     *  @arg action      [Callback,read-only]    Callback structure for transition action
     *  @arg fail_action [Callback,read-only]    Callback structure for transition failure action
     *  @return  Status code indicating success or failure
     */
    Machine& setTransActions (const Transition* transition,const Callback& pre_action,const Callback& action,const Callback& fail_action);
  };   //  End class Machine

  typedef FsmHandle<Machine> MachineHandle;

}      //  End namespace 
#endif //  ONLINE_FINITESTATEMACHINE_STATE_H
