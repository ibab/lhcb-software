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
#include "FiniteStateMachine/Rule.h"
#include "FiniteStateMachine/State.h"
#include "FiniteStateMachine/FSMTypes.h"

// C/C++ include files
#include <cstdio>

using namespace FiniteStateMachine;
using namespace std;

static string makeName(const Type *type, const State* state_current, const State* state_target)  {
  string nam = type->name()+"::";
  nam += state_current ? state_current->c_name() : ST_NAME_ANY;
  nam += "->";
  nam += state_target ? state_target->c_name() : ST_NAME_ANY;
  return nam;
}

/// Class Constructor
Rule::Rule(const Type* typ, const State* state_current, const State* state_target, Direction dir)
  : TypedObject(typ,makeName(typ,state_current,state_target)), 
    m_currState(state_current), m_targetState(state_target), m_transition(0), m_direction(dir)
{
}

/// Class Constructor
Rule::Rule(const Type* typ, const Transition* tr, Direction dir)
  : TypedObject(typ,makeName(tr->type(),tr->from(),tr->to())), 
    m_currState(0), m_targetState(0), m_transition(tr), m_direction(dir)
{
}

/// Standatrd destructor  
Rule::~Rule()    {
}

/// Retrieve pointer to the target state object
const State* Rule::targetState ()  const    {
  return m_transition ? m_transition->to() : m_targetState;
}

/// Retrieve pointer to the current state object
const State* Rule::currState ()  const      {
  return m_transition ? m_transition->from() : m_currState;
}

#if 0
/// Check if a rule applies to a given slave state
const Transition* Rule::applies(const Transition* transition, const State* slave_state, Direction direction_flag)  const  {
  const Transition* tr = applies(slave_state,direction_flag);
  if ( tr == transition ) return tr;
  return 0;
}
#endif

/// Check if a rule applies to a given slave state
const Transition* Rule::applies(const State* slave_state, Direction direction_flag)  const  {
  const State* to = targetState();
  if ( slave_state->type() == m_type && m_direction == direction_flag )  {
    // If required slave state check it!
    const Transition* tr = m_transition;
    if ( tr )  {
      if ( tr->from() == slave_state )  {
	display(NOLOG,"+++RULE: %s> Apply rule on slave: tr:%s --  %s -> %s",
		c_name(),tr->c_name(),tr->from()->c_name(),to->c_name());
	return tr;
      }
      display(NOLOG,"+++RULE: %s> Ignore rule on slave:  %s -> %s  [NO_TRANSITION]",
	      c_name(),slave_state->c_name(),to->c_name());
      return 0;
    }
    else if ( 0 != (tr=slave_state->findTrans(to)) )   {
      if ( !m_currState || (m_currState && m_currState == tr->from()) )  {
	display(NOLOG,"+++RULE: %s> Apply rule on slave: tr:%s [%s] --  %s -> %s",
		c_name(),tr->c_name(),m_currState ? m_currState->c_name() : "----",
		slave_state->c_name(),to->c_name());
	return tr;
      }
      display(NOLOG,"+++RULE: %s> Ignore rule on slave:  %s -> %s  (%s , %s) [STATE_MISMATCH]",
	      c_name(),slave_state->c_name(),to->c_name(),
	      m_currState ? m_currState->c_name() : "----",
	      slave_state ? slave_state->c_name() : "----");
      return 0;
    }
    display(NOLOG,"+++RULE: %s> Ignore rule on slave:  %s -> %s  [NO_TRANSITION]",
	    c_name(),slave_state->c_name(),to->c_name());
    return 0;
  }
  display(NOLOG,"+++RULE: %s> Ignore rule on slave:  %s -> %s",
	  c_name(),slave_state->c_name(),to->c_name());
  return 0;
}

/// Class Constructor
Predicate::Predicate(const Type *typ, const States& allowed)
  : TypedObject(typ,makeName(typ,*allowed.begin(),0)), m_allowed(allowed)
{
}

/// Standatrd destructor  
Predicate::~Predicate()    {
}

/// Check if a slave with a given state satisfies the predicate
bool Predicate::hasState(const State* slave_state)  const   {
  return m_allowed.find(slave_state) != m_allowed.end();
}

/// Class Constructor
When::When(const Type *typ, const State* current, Multiplicity m, const States& allowed, 
	   const State* target, Rule::Direction direction)
  : Predicate(typ,allowed), m_rule(typ,current,target,direction), m_mult(m)
{
  m_name = makeName(typ,current,target);
}

/// Standatrd destructor  
When::~When()  {
}
#include <iostream>
/// Check if a slave with a given state satisfies the predicate
When::Result When::fires(const States& states)  const   {
  const State* to = m_rule.targetState();
  States::const_iterator i;
  if ( m_mult == ALL_IN_STATE )   {
    for(i=states.begin(); i!=states.end(); ++i)
      if ( m_allowed.find(*i) == m_allowed.end() ) {
	//cout << (*i)->name() << " Not found! " << to->name() << endl;
	return Result(0,Rule::NO_DIRECTION);
      }
    return Result(to,m_rule.direction());
  }
  else if ( m_mult == ALL_NOT_IN_STATE )   {
    for(i=states.begin(); i!=states.end(); ++i)
      if ( m_allowed.find(*i) != m_allowed.end() ) 
	return Result(0,Rule::NO_DIRECTION);
    return Result(to,m_rule.direction());
  }
  else if ( m_mult == ANY_IN_STATE )   {
    for(i=states.begin(); i!=states.end(); ++i)
      if ( m_allowed.find(*i) != m_allowed.end() ) 
	return Result(to,m_rule.direction());
    return Result(0,Rule::NO_DIRECTION);
  }
  else if ( m_mult == ANY_NOT_IN_STATE )   {
    for(i=states.begin(); i!=states.end(); ++i)
      if ( m_allowed.find((*i)) == m_allowed.end() )
	return Result(to,m_rule.direction());
    return Result(0,Rule::NO_DIRECTION);
  }
  return Result(0,Rule::NO_DIRECTION);
}

static pair<When::Multiplicity,When::States> 
makeWhenParam(When::Multiplicity p, const State* s0, const State* s1, const State* s2, const State* s3,
				    const State* s4, const State* s5, const State* s6, const State* s7)
{
  pair<When::Multiplicity,When::States> r(p,When::States());
  r.second.insert(s0);
  if ( s1 ) r.second.insert(s1);
  if ( s2 ) r.second.insert(s2);
  if ( s3 ) r.second.insert(s3);
  if ( s4 ) r.second.insert(s4);
  if ( s5 ) r.second.insert(s5);
  if ( s6 ) r.second.insert(s6);
  if ( s7 ) r.second.insert(s7);
  return r;
}

const State* FiniteStateMachine::moveTo(const State* s)    {
  return s; 
}

pair<When::Multiplicity,When::States> 
FiniteStateMachine::allChildrenInState  (const State* s0, const State* s1, const State* s2, const State* s3,
					 const State* s4, const State* s5, const State* s6, const State* s7)
{
  return makeWhenParam(When::ALL_IN_STATE, s0, s1, s2, s3, s4, s5, s6, s7);
}

pair<When::Multiplicity,When::States> 
FiniteStateMachine::allChildrenNotInState(const State* s0, const State* s1, const State* s2, const State* s3,
					  const State* s4, const State* s5, const State* s6, const State* s7)
{
  return makeWhenParam(When::ALL_NOT_IN_STATE, s0, s1, s2, s3, s4, s5, s6, s7);
}

pair<When::Multiplicity,When::States> 
FiniteStateMachine::anyChildInState(const State* s0, const State* s1, const State* s2, const State* s3,
				    const State* s4, const State* s5, const State* s6, const State* s7)
{
  return makeWhenParam(When::ANY_IN_STATE, s0, s1, s2, s3, s4, s5, s6, s7);
}

pair<When::Multiplicity,When::States> 
FiniteStateMachine::anyChildNotInState(const State* s0, const State* s1, const State* s2, const State* s3,
				       const State* s4, const State* s5, const State* s6, const State* s7)
{
  return makeWhenParam(When::ANY_NOT_IN_STATE, s0, s1, s2, s3, s4, s5, s6, s7);
}
