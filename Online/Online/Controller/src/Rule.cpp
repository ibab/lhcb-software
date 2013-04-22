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
  nam += state_current ? state_current->c_name() : DAQ::ST_NAME_ANY;
  nam += "->";
  nam += state_target ? state_target->c_name() : DAQ::ST_NAME_ANY;
  return nam;
}

/// Class Constructor
Rule::Rule(const Type *typ, const State* state_current, const State* state_target, Direction dir)
  : TypedObject(typ,makeName(typ,state_current,state_target)), 
    m_currState(state_current), m_targetState(state_target), m_direction(dir)
{
}

/// Standatrd destructor  
Rule::~Rule()    {
}

/// Check if a rule applies to a given slave state
bool Rule::operator()(const State* slave_state, Direction direction_flag)  const  {
  if ( slave_state->type() == m_type && m_direction == direction_flag )  {
    // If required slave state check it!
    const Transition* tr = slave_state->findTrans(m_targetState);
    if ( tr )   {
      if ( !m_currState || (m_currState && m_currState == tr->from()) )  {
	display(NOLOG,"+++RULE: %s> Apply rule on slave: tr:%s [%s] --  %s -> %s",
		c_name(),tr->c_name(),m_currState ? m_currState->c_name() : "----",
		slave_state->c_name(),m_targetState->c_name());
	return true;
      }
      display(NOLOG,"+++RULE: %s> Ignore rule on slave:  %s -> %s  (%s , %s) [STATE_MISMATCH]",
	      c_name(),slave_state->c_name(),m_targetState->c_name(),
	      m_currState ? m_currState->c_name() : "----",
	      slave_state ? slave_state->c_name() : "----");
      return false;
      }
    display(NOLOG,"+++RULE: %s> Ignore rule on slave:  %s -> %s  [NO_TRANSITION]",
	    c_name(),slave_state->c_name(),m_targetState->c_name());
    return false;
  }
  display(NOLOG,"+++RULE: %s> Ignore rule on slave:  %s -> %s",
	  c_name(),slave_state->c_name(),m_targetState->c_name());
  return false;
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
bool Predicate::operator()(const State* slave_state)  const   {
  return m_allowed.find(slave_state) != m_allowed.end();
}
