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
#ifndef ONLINE_FINITESTATEMACHINE_RULE_H
#define ONLINE_FINITESTATEMACHINE_RULE_H

// Framework include files
#include "FiniteStateMachine/TypedObject.h"

// C/C++ include files
#include <set>

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine   {

  // Forward declarations
  class State;

  /**@class Rule  Rule.h FiniteStateMachine/Rule.h
   *    A FSM rule consists out of three components:
   *    o The FSM protocol type of the machine, which applies the rule
   *    o The FSM protocol type of the machine, which is supposed to follow
   *      the rule (TARGET-TYPE)
   *    o The FSM state, of which the TARGET-TYPE is supposed to go to.
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class Rule : public TypedObject   {
  public:
    enum Direction {
      MASTER2SLAVE = 1,
      SLAVE2MASTER = 2
    };
  protected:
    /// FSM current state of the target type the rule applies to
    const State* m_currState;
    /// FSM target state of the target type the rule applies to
    const State* m_targetState;
    /// Rule direction: Master->Task or Task->Master
    Direction    m_direction;

  public:
    /** Class Constructor
     *
     * @arg typ           [pointer, read-only]  FSM type
     * @arg state_current [pointer, read-only]  Current state of the object the rule applies to
     * @arg state_target  [pointer, read-only]  Target state of the object the rule applies to
     */
    Rule(const Type *typ, const State* state_current, const State* state_target, Direction direction);
    /// Standatrd destructor  
    virtual ~Rule();    
    /// Retrieve pointer to the target state object
    const State* targetState ()  const    { return m_targetState;      }
    /// Retrieve pointer to the current state object
    const State* currState ()  const      { return m_currState;        }
    /// Rule direction
    int direction() const                 { return m_direction;        }
    /// Check if a rule applies to a given slave state
    bool operator()(const State* slave_state, Direction direction)  const;
  };   //  End class Rule


  /**@class Predicate  Predicate.h FiniteStateMachine/Predicate.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class Predicate : public TypedObject   {
  public:
    /// State container definition
    typedef std::set<const State*> States;

  protected:
    /// Container with allowed states for this predicate
    States m_allowed;
  public:
    /** Class Constructor. The rule applies to all objects independent of their currrent state
     *
     * @arg typ    [pointer,   read-only]  FSM type
     * @arg states [container, read-only]  Container of allowed states for this predicate.
     */
    Predicate(const Type *typ, const States& allowed);
    /// Standatrd destructor  
    virtual ~Predicate();
    /// Access container with allowed states
    const States& allowed() const { return m_allowed; }
    /// Check if a slave with a given state satisfies the predicate
    bool operator()(const State* slave_state)  const;
  };   //  End class Predicate
}      //  End namespace 
#endif //  ONLINE_FINITESTATEMACHINE_RULE_H

