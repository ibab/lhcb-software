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
#include "FiniteStateMachine/Functors.h"
#include "FiniteStateMachine/Transition.h"

// C/C++ include files
#include <cstdio>
#include <stdexcept>

using namespace FiniteStateMachine;
using namespace std;

/// Default Constructor
StateActions::StateActions() : m_out(), m_in()
{
}

/// Class Constructor
StateActions::StateActions(const Callback& in, const Callback& out) : m_out(out), m_in(in)
{
}

/// Class Destructor
StateActions::~StateActions()	{
}

/** Update callback structures for stat enter actions. 
 *  If a callback is already present, replace the old structure
 *  with the current staructure and delete the callback object.
 *
 *  @arg  in  [Callback,read-only]  Reference to state incoming callback structure
 *  @arg  out [Callback,read-only]  Reference to state incoming callback structure
 */
StateActions& StateActions::setInAction(const Callback& cb)   {
  m_in = cb;
  return *this;
}

/** Update callback structures for stat leave actions. 
 *  If a callback is already present, replace the old structure
 *  with the current staructure and delete the callback object.
 *
 *  @arg  in  [Callback,read-only]  Reference to state incoming callback structure
 *  @arg  out [Callback,read-only]  Reference to state incoming callback structure
 */
StateActions& StateActions::setOutAction(const Callback& cb)   {
  m_out = cb;
  return *this;
}

/// Class Constructor
State::State(const Type *typ, const string& nam)
  : TypedObject(typ,nam), m_transient(0)
{
}

/// Standatrd destructor  
State::~State()    {
  for_each(m_when.begin(),m_when.end(),Delete<When>());
  m_when.end();
}

/// Return Pointer to FSM transition object leading to the requested target state
const Transition* State::findTrans (const State* state)  const {
  Transitions::const_iterator i = m_outgoing.end();
  if ( state ) i = find_if(m_outgoing.begin(),m_outgoing.end(),FindTransitionByState(state));
  return i == m_outgoing.end() ? 0 : (*i);
}

/// Return Pointer to FSM transition object leading to the requested target state
const Transition* State::findTrans (const string& state)  const {
  Transitions::const_iterator i = find_if(m_outgoing.begin(),m_outgoing.end(),FindTransitionByStateName(state));
  return i == m_outgoing.end() ? 0 : (*i);
}

/// Return Pointer to FSM transition object leading to the requested target state
const Transition* State::findTransByName (const string& transition)  const {
  Transitions::const_iterator i = find_if(m_outgoing.begin(),m_outgoing.end(),FindTransitionByName(transition));
  return i == m_outgoing.end() ? 0 : (*i);
}

/// Add when clause to state object
const When* State::when(const pair<When::Multiplicity,When::States>& p1, const State* target, Rule::Direction direction)  const {
  if ( target )  {
    if ( target->type() == type() )  {
      When*  wh = new When(type(), this, p1.first, p1.second, target, direction);
      State* me = const_cast<State*>(this);
      me->m_when.push_back(wh);
      return wh;
    }
    throw runtime_error("Type:"+type()->name()+"> When of state:"+name()+". Target state "+
			target->name()+" has invalid type!");
  }
  throw runtime_error("Type:"+type()->name()+"> When of state:"+name()+". Invalid target state for WHEN clause.");
}
