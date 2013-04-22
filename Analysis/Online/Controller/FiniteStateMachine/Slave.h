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
#ifndef ONLINE_FINITESTATEMACHINE_SLAVE_H
#define ONLINE_FINITESTATEMACHINE_SLAVE_H

// Framework include files
#include "CPP/Interactor.h"
#include "FiniteStateMachine/State.h"
#include "FiniteStateMachine/Transition.h"

// C/C++ include files
#include <set>

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine {

  // Forward declarations
  class Transition;
  class Machine;
  class State;

  /**@class Slave  Slave.h Ctrl/Slave.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class Slave : public TypedObject, public Interactor    {
  public:
    typedef FSM::ErrCond ErrCond;
    enum SlaveState  {
      SLAVE_LIMBO              = 3101,
      SLAVE_STARTING           = 3102,
      SLAVE_ALIVE              = 3103,
      SLAVE_EXECUTING          = 3104,
      SLAVE_FAILED             = 3105,
      SLAVE_KILLED             = 3106,
      // Handling of answers from physical slaves
      SLAVE_DEAD               = 3108,
      SLAVE_TRANSITION         = 3109,
      SLAVE_FINISHED           = 3110,
      SLAVE_START_TIMEOUT      = 3900,
      SLAVE_TRANSITION_TIMEOUT = 3901
    };

  protected:
    SlaveState        m_meta;
    Machine*          m_machine;
    const State*      m_state;
    const Rule*       m_rule;
  public:
    /** Class Constructor
     *
     * @arg typ     [string, read-only] FSM type reference
     * @arg nam     [string, read-only] Slave name
     * @arg machine [string, read-only] FSM machine reference
     */
    Slave(const Type* typ, const std::string& nam, Machine* machine);
    /// Standatrd destructor
    virtual ~Slave();    
    /// Internal meta-state of the slave
    SlaveState currentState() const                       {  return m_meta;                 }
    /// Access meta state as string
    const char* metaStateName() const;
    /// Access current state of the slave
    const State* state() const                            {  return m_state;                }
    /// Access the current rule to be applied
    const Rule*  rule() const                             {  return m_rule;                 }
    /// Retrieve reference to current State structure name
    const char* c_state ()  const                         {  return m_state->c_name();      }
    /// Check if the slave is limbo
    bool isLimbo() const           {  return m_meta==SLAVE_LIMBO || m_meta==SLAVE_STARTING; }

    /// Invoke transition on slave. Noop if slave is already in target state
    ErrCond apply(const Rule* rule);

    /// Send IOC interrupt to slave
    ErrCond send(int code, const State* state=0)   const;

    /// External interrupt handler
    virtual void handle(const Event& event);
    /// IOC interrupt handle handler
    virtual void handleIoc(const Event& event);
    /// Send notification to machine object about slave's state changes
    virtual ErrCond notifyMachine(int meta_state);

    /// Send transition request to the slave
    virtual ErrCond sendRequest(const Transition* tr) = 0;
    /// Start slave process
    virtual ErrCond start() = 0;
    /** Kill slave process. 
     *  Function MUST return FSM::SUCCESS if slave is truely dead 
     *  or FSM::WAIT_ACTION if slave is going to die.
     */
    virtual ErrCond kill() = 0;

    /// Callback on alive signs of slave process: Invoke using IOC SLAVE_ALIVE
    virtual ErrCond iamHere();
    /// Callback on sudden death of slave process: Invoke using IOC SLAVE_LIMBO
    virtual ErrCond iamDead();
    /// Callback, when transition was executed successfully (called on receipt of answer message). Invoke using IOC SLAVE_FINISHED_TRANS
    virtual ErrCond transitionDone(const State* state);
    /// Callback, when the slave invoked a transition himself (called on receipt of answer message). Invoke using IOC SLAVE_FINISHED_TRANS
    virtual ErrCond transitionSlave(const State* state);
    /// Callback, when transition failed (called on receipt of answer message). Invoke using IOC SLAVE_FAILED
    virtual ErrCond transitionFailed(const State* state);

  };   //  End class State
}      //  End namespace 
#endif //  ONLINE_FINITESTATEMACHINE_SLAVE
