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
#include "FiniteStateMachine/Transition.h"

// C/C++ include files
#include <cstdio>
#include <stdexcept>

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
const Rule* Transition::adoptRule(Rule* rule)   {
  if ( rule ) {
    m_rules.push_back(rule);
    return rule;
  }
  throw runtime_error("Transition::addRule> invalid rule for transition:"+name()+" of type "+type()->name());
}

/// Add a new rules to a transition
void Transition::adoptRule(Rules rules) {
  for(Rules::iterator i=rules.begin(); i!=rules.end(); ++i)
    adoptRule(*i);
}

/// Add a new predicate to a transition
const Predicate* Transition::addPredicate(const Predicate::States& allowed)  {
  Predicate* predicate = new Predicate(type(),allowed);
  m_predicates.insert(predicate);
  return predicate;
}

/// Helper function to define a whole set of rules to a transition depending on explicit transitions
AllChildrenOfType::Rules AllChildrenOfType::execTransition(const Transitions& transitions)  const {
  Rules rules;
  for(Type::ConstTransitions::const_iterator i=transitions.begin(); i!=transitions.end(); ++i) {
    Rule* r = new Rule(m_type,(*i).second,Rule::MASTER2SLAVE);
    rules.push_back(r);
  }
  return rules;
}

/// Add a new rule to a transition
Rule* AllChildrenOfType::execTransition(const string& curr_state, const string& target_state, Rule::Direction direction)  const {
  bool   any = curr_state == ST_NAME_ANY;
  string nam = m_type->name()+"::"+curr_state+"->"+target_state;
  const State* t_state = m_type->state(target_state);
  const State* c_state = any ? 0 : m_type->state(curr_state);
  if ( !any && !c_state )  {
    throw runtime_error("adoptRule> invalid object state "+curr_state+" in type "+m_type->name());
  }
  if ( t_state )   {
    return execTransition(curr_state,target_state,direction);
  }
  throw runtime_error("adoptRule> invalid target state "+target_state+" in type "+m_type->name());
}

/// Add a new rule to a transition
Rule* AllChildrenOfType::execTransition(const State* curr_state, const State* target_state, Rule::Direction direction)  const {
  if ( target_state )   {
    Rule* rule = new Rule(m_type,curr_state,target_state,direction);
    return rule;
  }
  throw runtime_error("adoptRule> invalid target state in type "+m_type->name());
}

/// Helper function to add ANY rules
Rule* AllChildrenOfType::moveTo(const string& target_state) const    {
  return execTransition(ST_NAME_ANY,target_state,Rule::MASTER2SLAVE);
}

/// Helper function to add ANY rules
Rule* AllChildrenOfType::moveTo(const State* target_state) const    {
  return execTransition(0,target_state,Rule::MASTER2SLAVE);
}

/// Helper function to add predicates
Predicate::States AllChildrenOfType::inState(const State* s1, const State* s2,
					     const State* s3, const State* s4, 
					     const State* s5) const
{
  Predicate::States allowed;
  if ( s1 ) allowed.insert(s1);
  if ( s2 ) allowed.insert(s2);
  if ( s3 ) allowed.insert(s3);
  if ( s4 ) allowed.insert(s4);
  if ( s5 ) allowed.insert(s5);
  return allowed;
}

/// Add a new predicate to a transition
Predicate::States AllChildrenOfType::inState(const string& s1, const string& s2,
					     const string& s3, const string& s4, 
					     const string& s5)  const
{
  string sname="";
  const State* st1 = m_type->state(s1);
  const State* st2 = s2.empty() ? 0 : m_type->state(s2);
  const State* st3 = s3.empty() ? 0 : m_type->state(s3);
  const State* st4 = s4.empty() ? 0 : m_type->state(s4);
  const State* st5 = s5.empty() ? 0 : m_type->state(s5);
  if ( !st1 ) sname = s1;
  else if ( !s2.empty() && !st2 ) sname = s2;
  else if ( !s3.empty() && !st3 ) sname = s3;
  else if ( !s4.empty() && !st4 ) sname = s4;
  else if ( !s5.empty() && !st5 ) sname = s5;
  else  {
    return inState(st1,st2,st3,st4,st5);
  }
  throw runtime_error("Type::addPredicate> invalid allowed state "+sname+" for predicate of type "+m_type->name());
}

