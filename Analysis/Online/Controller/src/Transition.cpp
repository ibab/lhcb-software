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
// Framework include files
#include "FiniteStateMachine/Type.h"
#include "FiniteStateMachine/State.h"
#include "FiniteStateMachine/FSMTypes.h"
#include "FiniteStateMachine/Functors.h"
#include "FiniteStateMachine/Transition.h"

// C/C++ include files
#include <cstdio>
#include <stdexcept>
#include <algorithm>

using namespace FiniteStateMachine;
using namespace std;

/// Default Constructor
TransitionActions::TransitionActions() : m_pre(), m_action(), m_fail()
{
}

/// Class Constructor
TransitionActions::TransitionActions(const Callback& pre, const Callback& act, const Callback& fail)
  : m_pre(pre), m_action(act), m_fail(fail)
{
}

/// Class Destructor
TransitionActions::~TransitionActions()   {
}

/// Update callback structures for stat enter actions. 
TransitionActions& TransitionActions::setAction(const Callback& cb)   {
  m_action = cb;
  return *this;
}

/// Update callback structures for stat enter actions. 
TransitionActions& TransitionActions::setPreAction(const Callback& cb)   {
  m_pre = cb;
  return *this;
}

/// Update callback structures for stat leave actions. 
TransitionActions& TransitionActions::setFailAction(const Callback& cb)   {
  m_fail = cb;
  return *this;
}

/// Update callback structures for stat leave actions. 
TransitionActions& TransitionActions::setCompletionAction(const Callback& cb)   {
  m_complete = cb;
  return *this;
}

/// Class Constructor
Transition::Transition (const Type* typ, const string& nam, 
			const State* from, const State* to, int flags)
  : TypedObject(typ,nam), m_from(from), m_to(to), m_create(false), m_checkLimbo(false)
{
  m_kill = (flags&KILL)==KILL;
  m_create = (flags&CREATE)==CREATE;
  m_checkLimbo = (flags&CHECK)==CHECK;
}

/// Standatrd destructor  
Transition::~Transition()    {
}

/// Add a new rule to a transition
const Rule* Transition::addRule(const Type* target_type, const string& curr_state, 
				const string& target_state, Rule::Direction direction)
{
  if ( target_type )  {
    bool any = curr_state==DAQ::ST_NAME_ANY;
    string nam = target_type->name()+"::"+curr_state+"->"+target_state;
    const State* t_state = target_type->state(target_state);
    const State* c_state = any ? 0 : target_type->state(curr_state);
    if ( !any && !c_state )  {
      throw runtime_error("Transition::addRule> invalid object state "+curr_state+
			  " for rule in transition:"+name()+" of type "+type()->name());
    }
    if ( t_state )   {
      Rule* rule = new Rule(target_type,c_state,t_state,direction);
      m_rules.insert(rule);
      return rule;
    }
    throw runtime_error("Transition::addRule> invalid target state "+target_state+
			" for rule in transition:"+name()+" of type "+type()->name());
  }
  throw runtime_error("Transition::addRule> invalid target type for rule in transition:"+name()+" of type "+type()->name());
}

/// Add a new predicate to a transition
const Predicate* Transition::addPredicate(const Type* target_type, const Predicate::States& allowed)  {
  if ( target_type )  {
    Predicate* predicate = new Predicate(target_type,allowed);
    m_predicates.insert(predicate);
    return predicate;
  }
  throw runtime_error("Transition::addPredicate> invalid target type for predicate in transition:"+name()+" of type "+type()->name());
}
