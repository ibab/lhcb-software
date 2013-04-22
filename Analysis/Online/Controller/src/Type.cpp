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

/// Add a new rule to a transition
const Rule* Type::addRule(Transition* transition, const Type* target_type, 
			  const string& curr_state, const string& target_state, 
			  Rule::Direction direction)  
{
  if ( transition )  {
    return transition->addRule(target_type,curr_state,target_state,direction);
  }
  throw runtime_error("Type::addRule> invalid transition to add rule for type:"+name());
}


/// Add a new predicate to a transition
const Predicate* Type::addPredicate(Transition* transition,const Type* target_type,
				    const std::string& s1, const std::string& s2,
				    const std::string& s3, const std::string& s4, 
				    const std::string& s5)  
{
  if ( transition )  {
    string sname="";
    set<const State*> allowed;

    const State* st1 = target_type->state(s1);
    const State* st2 = s2.empty() ? 0 : target_type->state(s2);
    const State* st3 = s3.empty() ? 0 : target_type->state(s3);
    const State* st4 = s4.empty() ? 0 : target_type->state(s4);
    const State* st5 = s5.empty() ? 0 : target_type->state(s5);
    if ( !st1 ) sname = s1;
    else if ( !s2.empty() && !st2 ) sname = s2;
    else if ( !s3.empty() && !st3 ) sname = s3;
    else if ( !s4.empty() && !st4 ) sname = s4;
    else if ( !s5.empty() && !st5 ) sname = s5;
    else  {
      if ( st1 ) allowed.insert(st1);
      if ( st2 ) allowed.insert(st2);
      if ( st3 ) allowed.insert(st3);
      if ( st4 ) allowed.insert(st4);
      if ( st5 ) allowed.insert(st5);
      return transition->addPredicate(target_type,allowed);
    }
    throw runtime_error("Type::addPredicate> invalid allowed state "+sname+
			" for predicate in transition:"+transition->name()+" of type "+name());
  }
  throw runtime_error("Type::addPredicate> invalid transition to add predicate for type:"+name());
}
