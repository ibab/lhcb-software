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
#include "FiniteStateMachine/Transition.h"

// C/C++ include files
#include <stdexcept>

using namespace FiniteStateMachine;
using namespace std;

/// Class Constructor
Type::Type(const string& nam) : m_name(nam)
{
}

/// Standatrd destructor  
Type::~Type()    {
}

    /// Accessor: Set initial state
Type& Type::setInitialState(const State* state)   {
  m_initState = state;
  return *this;
}
    /// Access state by its name. Throws exception if state does not exist
const State* Type::state(const std::string& nam)  const   {
  States::const_iterator i = m_states.find(nam);
  if ( i != m_states.end() )  return (*i).second;
  throw runtime_error("Invalid state name:"+nam);
}

/// Access transition by its name. Throws exception if transition does not exist
const Transition* Type::transition(const std::string& nam)  const   {
  Transitions::const_iterator i = m_transitions.find(nam);
  if ( i != m_transitions.end() )  return (*i).second;
  throw runtime_error("Invalid state name:"+nam);
}

/// Access transition by its source and target state name. Throws exception if transition does not exist
const Transition* Type::transition(const std::string& from, const std::string& to)  const   {
  return transition(from+"->"+to);
}

/// Access transition by its source and target state name. Throws exception if transition does not exist
Type::ConstTransitions Type::transitionsByName(const std::string& nam)  const  {
  ConstTransitions tr;
  for(Transitions::const_iterator i=m_transitions.begin(); i != m_transitions.end(); ++i) {
    const Transition* t = (*i).second;
    if ( t->name() == nam ) tr.insert(make_pair((*i).first,t));
  }
  return tr;
}

/// Add a new state to the FSM type
const State* Type::addState(const std::string& nam)  {
  if ( !nam.empty() )  {
    States::const_iterator i = m_states.find(nam);
    if ( i == m_states.end() )  {
      return m_states[nam] = new State(this,nam);
    }
    throw runtime_error("Type::addState> Already existing state:"+nam);
  }
  throw runtime_error("Type::addState> Invalid state name: <empty>");  
}

/// Add a new Transition to the FSM type
Transition* Type::addTransition(const State* from, const State* to, unsigned int flags)    {
  if ( from )  {
    if ( to )  {
      string nam = from->name()+"->"+to->name();
      Transition* tr = new Transition(this,nam,from,to,flags);
      const_cast<State*>(from)->outgoing().insert(tr);
      const_cast<State*>(to)->incoming().insert(tr);
      m_transitions[nam] = tr;
      return tr;
    }
    throw runtime_error("Type::addTransition> Invalid target state to add transition to type "+name());  
  }
  throw runtime_error("Type::addTransition> Invalid initial state to add transition to type "+name());  
}

/// Add a new Transition to the FSM type
Transition* Type::addTransition(const std::string& cmd, const State* from, const State* to, unsigned int flags)    {
  if ( !cmd.empty() )  {
    if ( from )  {
      if ( to )  {
	string nam = from->name()+"->"+to->name();
	Transition* tr = new Transition(this,cmd,from,to,flags);
	const_cast<State*>(from)->outgoing().insert(tr);
	const_cast<State*>(to)->incoming().insert(tr);
	m_transitions[nam] = tr;
	return tr;
      }
      throw runtime_error("Type::addTransition> Invalid target state to add transition "+cmd+" to type "+name());  
    }
    throw runtime_error("Type::addTransition> Invalid initial state to add transition "+cmd+" to type "+name());  
  }
  throw runtime_error("Type::addTransition> Invalid state name: <empty> to add transition to "+name());  
}
