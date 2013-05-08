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
  class Transition;

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
      NO_DIRECTION = 0,
      MASTER2SLAVE = 1,
      SLAVE2MASTER = 2
    };
  protected:
    /// FSM current state of the target type the rule applies to
    const State* m_currState;
    /// FSM target state of the target type the rule applies to
    const State* m_targetState;
    /// FSM transition to be executed if specified
    const Transition* m_transition;
    /// Rule direction: Master->Task or Task->Master
    Direction    m_direction;

  public:
    /// Initializing class Constructor
    Rule(const Type *typ, const State* state_current, const State* state_target, Direction direction);
    /// Initializing class Constructor
    Rule(const Type *typ, const Transition* tr, Direction direction);
    /// Standatrd destructor  
    virtual ~Rule();    
    /// Retrieve pointer to the target state object
    const State* targetState ()  const;
    /// Retrieve pointer to the current state object
    const State* currState ()  const;
    /// Access the FSM transition if it is specified
    const Transition* transition() const  { return m_transition;       }
    /// Rule direction
    Direction direction() const           { return m_direction;        }
    /// Is the rule directed towards the master
    bool toMaster() const        { return m_direction == SLAVE2MASTER; }
    /// Is the rule directed towards the master
    bool toSlave() const         { return m_direction == MASTER2SLAVE; }
    /// Check if a rule applies to a given slave state
    bool applies(const State* slave_state, Direction direction)  const;
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
    bool hasState(const State* slave_state)  const;
  };   //  End class Predicate

  /**@class When  When.h FiniteStateMachine/When.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class When : public Predicate   {
  public:
    enum Multiplicity { 
      NONE             = 0, 
      ANY_IN_STATE     = 1<<0,
      ANY_NOT_IN_STATE = 1<<1,
      ALL_IN_STATE     = 1<<2,
      ALL_NOT_IN_STATE = 1<<3
    };
    typedef std::pair<const State*,Rule::Direction> Result;

  protected:
    /// The Rule to be executed for this when clause
    Rule          m_rule;
    /// Multiplicity enum
    Multiplicity  m_mult;
  public:
    /// Class Constructor. The rule applies to all objects independent of their currrent state
    When(const Type *typ, 
	 const State* current, 
	 Multiplicity m, 
	 const States& allowed, 
	 const State* target,
	 Rule::Direction direction);
    /// Standatrd destructor  
    virtual ~When();
    const Rule&  rule()  const        {  return m_rule;   }
    Multiplicity multiplicity() const {  return m_mult;   }
    /// Check if a slave with a given state satisfies the predicate
    Result fires(const States& slave_states)  const;
  };   //  End class When

  std::pair<When::Multiplicity,When::States> 
    allChildrenInState(const State* s0,   const State* s1=0, const State* s2=0, const State* s3=0,
		       const State* s4=0, const State* s5=0, const State* s6=0, const State* s7=0);
  std::pair<When::Multiplicity,When::States> 
    allChildrenNotInState(const State* s0,   const State* s1=0, const State* s2=0, const State* s3=0,
			  const State* s4=0, const State* s5=0, const State* s6=0, const State* s7=0);
  std::pair<When::Multiplicity,When::States> 
    anyChildInState(const State* s0,   const State* s1=0, const State* s2=0, const State* s3=0,
		    const State* s4=0, const State* s5=0, const State* s6=0, const State* s7=0);
  std::pair<When::Multiplicity,When::States> 
    anyChildNotInState(const State* s0,   const State* s1=0, const State* s2=0, const State* s3=0,
		       const State* s4=0, const State* s5=0, const State* s6=0, const State* s7=0);
  const State* moveTo(const State* s);

}      //  End namespace FiniteStateMachine
#endif //  ONLINE_FINITESTATEMACHINE_RULE_H

