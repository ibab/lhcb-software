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
#include "FiniteStateMachine/State.h"
#include "FiniteStateMachine/Transition.h"
#include "CPP/Interactor.h"

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
  class Slave : public TypedObject, public CPP::Interactor  {
  public:
    typedef FSM::ErrCond ErrCond;
    enum SlaveState  {
      SLAVE_NONE               = 0xFFFFFFFF,
      SLAVE_LIMBO              = 1<<20,
      SLAVE_STARTING           = 1<<21,
      SLAVE_ALIVE              = 1<<22,
      SLAVE_EXECUTING          = 1<<23,
      SLAVE_FAILED             = 1<<24,
      SLAVE_KILLED             = 1<<25,
      // Handling of answers from physical slaves
      SLAVE_DEAD               = 1<<26,
      SLAVE_TRANSITION         = 1<<27,
      SLAVE_FINISHED           = 1<<28,
      // Timeout values
      SLAVE_TIMEOUT            = 1<<29,
      SLAVE_UNLOAD_TIMEOUT,
      SLAVE_TERMINATE_TIMEOUT,
      SLAVE_KILL_TIMEOUT,
      SLAVE_START_TIMEOUT
    };

  protected:
    /// TimerID definition
    typedef std::pair<void*,unsigned long> TimerID;
    /// Definition of the timeout table
    typedef std::map<const void*,int> TimeoutTable; 

    /// Timeout table for various transitions
    TimeoutTable      m_timeouts;
    /// ID of timeout timer
    TimerID           m_timerID;
    /// General transition timeout, if no other is specified.
    int               m_tmo;

    /// The slave's meta-state
    SlaveState        m_meta;
    /// The slave's meta-state
    SlaveState        m_status;
    /// Reference to the managing machine
    Machine*          m_machine;
    /// Current slave state. During transition the original state
    const State*      m_state;
    /// Rule currently applied during this transition
    const Rule*       m_rule;
    /// Instance identifier (default: 0, >0 for forked processes)
    int               m_instanceTag;
    /// Flag indicating (initial) internal transition actions
    bool              m_internal;
    /// Flag indicating if the slave may be started (internal slaves may not!)
    bool              m_mayStart;
    /// Flag to store if slave is alive
    bool              m_alive;
    /// Flag to see if the slave has answered to a request
    bool              m_answered;

  public:
    /** Class Constructor
     *
     * @arg typ     [string, read-only] FSM type reference
     * @arg nam     [string, read-only] Slave name
     * @arg machine [string, read-only] FSM machine reference
     */
    Slave(const Type* typ, const std::string& nam, Machine* machine, bool internal);
    /// Standatrd destructor
    virtual ~Slave();    
    /// Meta state name translation from enumeration to string
    static const char* metaName(SlaveState st);
    /// Internal meta-state of the slave
    SlaveState currentState() const                       {  return m_meta;                 }
    /// Internal meta-state of the slave
    void setCurrentState(SlaveState new_state)            {  m_meta = new_state;            }
    /// Access meta state as string
    const char* metaStateName() const;
    /// Access to error conditions if they occurred during the last sequence
    SlaveState statusState() const                        {  return m_status;               }
    /// Access meta state as string
    const char* statusName() const;
    /// Access instance identifier
    int instanceTag() const                               {  return m_instanceTag;          }
    /// Set instance ID
    void setInstanceTag(int new_tag)                      {  m_instanceTag = new_tag;       }
    /// Check if the slave answered to the current request
    bool answered()  const                                {  return m_answered;             }
    /// Check if the task is alive
    bool isAlive() const                                  {  return m_alive;                }
    /// Check if the slave is limbo
    bool isLimbo() const    {  return !m_alive||((m_meta&SLAVE_LIMBO)==SLAVE_LIMBO)||((m_meta&SLAVE_STARTING)==SLAVE_STARTING);}
    /// Access flag indicating (initial) internal transition actions
    bool isInternal() const                               {  return m_internal;             }
    /// Access flag indicating (initial) internal transition actions
    void setInternal(bool internal_flag)                  {  m_internal = internal_flag;    }
    /// Access flag indicating if slave may be started after death (ie. not forked/internal)
    bool mayStart() const                                 {  return m_mayStart;             }
    /// Access current state of the slave
    const State* state() const                            {  return m_state;                }
    /// Set current state of the slave. Note: be careful modifying the state if it does not correspond to the object 
    void setState(const State* new_state)                 {  m_state = new_state;           }
    /// Access the current rule to be applied
    const Rule*  rule() const                             {  return m_rule;                 }
    /// Retrieve reference to current State structure name
    const char* c_state ()  const;
    /// Retrieve reference to managing machine structure name
    const char* c_machine ()  const;

    /// Set the default timeout value
    void setTimeout(int val)                 {  m_tmo = val;  }
    /// Access the command string 
    int timeout() const                      {  return m_tmo; }
    /// Add entry in transition timeout table (timeout in seconds)
    Slave& addTimeout(const Transition* param, int tmp);
    /// Remove entry in transition timeout table
    Slave& removeTimeout(const Transition* param);
    /// Start the slave's transition timeout
    virtual Slave& startTimer(int reason, const void* param=0) = 0;
    /// Stop the slave's transition timeout
    virtual Slave& stopTimer() = 0;

    /** Specific Interactor handle to act on sensor interrupts
     *  @arg  ev    [Event,read-only]   Event structure to be handled
     */
    virtual void handle(const CPP::Event& event);
    /// Handle timeout according to timer ID
    virtual void handleTimeout();
    /// Handle timeout on unload transition according to timer ID
    virtual void handleUnloadTimeout();

    /// Handle state updates for a particular slave
    virtual void handleState(const std::string& msg);

    /// Invoke transition on slave. Noop if slave is already in target state
    ErrCond apply(const Rule* rule);

    /// Send IOC interrupt to slave - used for internal slave transitions
    virtual ErrCond send(int code, const State* state=0)   const;

    /// Send notification to machine object about slave's state changes
    virtual ErrCond notifyMachine(int meta_state);

    /// Start slave process
    virtual ErrCond startSlave(const Transition* tr);
    /// Start slave process
    virtual ErrCond killSlave();
    /// Force slave killing  (kill -9 or equivalent)
    virtual ErrCond forceKill();

    /// Virtual method -- must be overloaded -- Send transition request to the slave
    virtual ErrCond sendRequest(const Transition* tr);
    /// Inquire slave state. The reply may come later!
    virtual ErrCond inquireState();
    /// Start slave process. Base class implementation will throw an exception
    virtual ErrCond start();
    /** Kill slave process. Base class implementation will throw an exception
     *  Function MUST return FSM::SUCCESS if slave is truely dead 
     *  or FSM::WAIT_ACTION if slave is going to die.
     */
    virtual ErrCond kill();
    /// Publish debug information
    virtual void publishDebugInfo();
    /// Publish tag information
    virtual void publishTag(const std::string& tag);

    /// Callback on alive signs of slave process: Invoke using IOC SLAVE_ALIVE
    virtual ErrCond iamHere();
    /// Callback on sudden death of slave process: Invoke using IOC SLAVE_LIMBO
    virtual ErrCond iamDead();
    /// Callback, when transition was executed successfully (called on receipt of answer message). Invoke using IOC SLAVE_FINISHED_TRANS
    virtual ErrCond transitionDone(const State* state);
    /// Callback, when the slave invoked a transition himself (called on receipt of answer message). Invoke using IOC SLAVE_FINISHED_TRANS
    virtual ErrCond transitionSlave(const State* state);
    /// Callback, when transition failed (called on receipt of answer message). Invoke using IOC SLAVE_FAILED
    virtual ErrCond transitionFailed();

  };   //  End class State
}      //  End namespace 
#endif //  ONLINE_FINITESTATEMACHINE_SLAVE
