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
#include "FiniteStateMachine/Type.h"
#include "FiniteStateMachine/Rule.h"
#include "FiniteStateMachine/Slave.h"
#include "FiniteStateMachine/Machine.h"
#include "FiniteStateMachine/Functors.h"
#include "FiniteStateMachine/Transition.h"

using namespace FiniteStateMachine;
using namespace std;

/// Operator invoked for each predicate to check if it is fulfilled
bool ByName::operator()(const TypedObject* p) const   
{  return p->name() == object->name();                                      }

/// Operator invoked for each predicate to check if it is fulfilled
bool ByName::operator()(const pair<string,TypedObject*>& p) const   
{  return (*this)(p.second);                                                }

/// Operator invoked for each predicate to check if it is fulfilled
bool FindTransitionByName::operator()(const Transition* t) const   
{  return t->name() == object;                                              }

/// Operator invoked for each predicate to check if it is fulfilled
bool FindTransitionByState::operator()(const Transition* t) const   
{  return t->to() == object;                                                }

/// Operator invoked for each predicate to check if it is fulfilled
bool FindTransitionByStateName::operator()(const Transition* t) const   
{  return t->to()->name() == object;                                        }

/// Operator invoked for each slave to be started
void SlaveStarter::operator()(Slave* s)    {
  int st = s->currentState();
  if ( st == Slave::SLAVE_ALIVE || st == Slave::SLAVE_EXECUTING )
    ++count;
  else if ( st == Slave::SLAVE_FAILED )
    ++fail;
  else if ( st == Slave::SLAVE_LIMBO )  {
    FSM::ErrCond sc = s->start();
    if ( sc == FSM::WAIT_ACTION  ) ++dead;
    else if ( sc == FSM::SUCCESS ) ++count;
    else ++fail;
  }
}

/// Operator invoked for each slave to be killed
void SlaveKiller::operator()(Slave* s)    {
  FSM::ErrCond sc = s->kill();
  if ( sc == FSM::SUCCESS     ) ++dead;
  if ( sc == FSM::WAIT_ACTION ) ++count;
  else ++fail;
}

/// Operator invoked for each rule and invoke slave accordingly
void InvokeSlave::operator()(Slave* s)   {
  const State* slState = s->state();
  const Transition::Rules& r = object->rules();
  if ( !object->checkLimbo() && s->isLimbo() ) return; 
  for(Transition::Rules::const_iterator i=r.begin(); i!=r.end();++i)  {
    const Rule* rule = (*i);
    if ( (*rule)(slState,direction) )  {
      FSM::ErrCond sc = s->apply(rule);
      if      ( status == FSM::SUCCESS && sc == FSM::WAIT_ACTION ) status = sc;
      else if ( sc == FSM::TRANNOTFOUND ) { /* Rule does not apply */ }
      else if ( sc != FSM::SUCCESS      ) status = sc;
      return;
    }
  }
}

/// Operator invoked for each slave to be checked
void SlaveLimboCount::operator()(const Slave* s)     {
  if ( s->isLimbo() )  ++count;
}

/// Operator invoked for each rule to check if it is fulfilled
void CheckStateSlave::operator()(const Slave* s)   {  
  const State* slState = s->state();
  const Type*  slType  = s->type();
  const Rule*  slRule  = s->rule();
  const Transition::Rules& r = object->rules();
  if ( s->isLimbo() ) { 
    //s->display(s->ALWAYS,"%s> DEAD Metastate:%s",s->c_name(),s->metaStateName());
    ++dead; 
  }
  else if ( s->currentState() == Slave::SLAVE_FAILED ) { 
    //s->display(s->ALWAYS,"%s> FAILED Metastate:%s",s->c_name(),s->metaStateName());
    ++fail; 
  }
  else if ( s->currentState() == Slave::SLAVE_EXECUTING ) { 
    //s->display(s->ALWAYS,"%s> EXECUTING - ignore Metastate:%s",s->c_name(),s->metaStateName());
  }
  else   {  // Check if all rules for this type are fulfilled
    for(Transition::Rules::const_iterator i=r.begin(); i != r.end(); ++i)  {
      const Rule* rule = (*i);
      if ( rule->type() == slType )  {
	if      ( slRule==rule && slState==rule->targetState() )	++count;  // Slave fulfills state criteria
	else if ( slState == rule->targetState() )  ++count;  // Slave fulfills state criteria
	else if ( slState == rule->currState() )  {}   // Slave fulfills state criteria, has not yet answered
	else if ( !slRule ) ++count; // There was nothing to be done.
	else {++fail;  // What can I do: there was a rule, but the slave did not go to the target state
	  s->display(s->ALWAYS,"%s FAILED Metastate:%s [Rule not fulfilled] State:%s",
		     s->c_name(),s->metaStateName(),slState->c_name());
	}
	break;
      }
    }
  }
}

/// Operator invoked for each predicate to check if it is fulfilled
void PredicateSlave::operator()(const Slave* s)   {
  bool check_limbo = object->checkLimbo();
  // If there are no limbo tasks allowed for this transition, we have to check this before any predicate!
  if ( check_limbo && s->isLimbo() )  {
    status = FSM::WAIT_ACTION;
    return;
  }
  // Now check the predicates.
  const Transition::Predicates& pred = object->predicates();
  for(Transition::Predicates::const_iterator i=pred.begin(); i!=pred.end(); ++i)  {
    const Predicate* p = (*i);
    if ( s->type() == p->type() )  {
      if      ( !check_limbo && s->isLimbo() ) continue;
      else if ( !(*p)(s->state()) ) status = FSM::FAIL;
    }
  }
}

/// Operator invoked for each machine during printing.
void PrintObject::operator()(const Machine* m) const  {
  int flag = TypedObject::ALWAYS;
  const char* pref = prefix.c_str();
  const Type* typ = m->type();
  const Transition* tr = m->currTrans();
  m->display(flag,"%s+-------- FSMmachine: %s of type %s.",pref,m->c_name(),typ->c_name());
  m->display(flag,"%s|  Meta transition:    %s  -> %s",pref,m->previousMetaName(),m->currentMetaName());
  m->display(flag,"%s|  Current state:      %s",pref,m->c_state());
  m->display(flag,"%s|  Current Transition: %s",pref,tr ? tr->c_name()       : "----");
  m->display(flag,"%s|  Target  state:      %s",pref,tr ? tr->to()->c_name() : "----");
  PrintObject slave_printer("++"+prefix);
  for_each(m->slaves().begin(),m->slaves().end(),slave_printer);
  m->display(flag,"%s+----------------------------------------------------------------------",pref);
}

/// Operator invoked for each Machine object printout.
void PrintObject::operator()(const MachineHandle& h) const  {
  const Machine*     m = h.ptr;
  const Type*        t = m->type();
  const Transition* tr = m->currTrans();
  m->display(TypedObject::ALWAYS,"%s+- FSMmachine: %s of type %s State:%-12s [%s] %s %s",
	     prefix.c_str(),m->c_name(),t->c_name(),m->c_state(),m->currentMetaName(),
	     tr ? "Executing transition" : "Idle", tr ? tr->c_name() : "----");
}

/// Operator invoked for each machine during printing.
void PrintObject::operator()(const Slave* s) const  {
  s->display(TypedObject::ALWAYS,"%s+  Slave: %s of type %s: State:%-12s Meta-state:%s managed by %s",
	     prefix.c_str(),s->c_name(),s->type()->c_name(),s->c_state(),
	     s->metaStateName(),s->c_machine());
}

/// Operator invoked for each Transition object printing.
void PrintObject::operator()(const Transition* t) const   {
  int flag = TypedObject::ALWAYS;
  const char* pref = prefix.c_str();
  const Transition::Rules& r=t->rules();
  const Transition::Predicates& p=t->predicates();
  PrintObject rule_printer(prefix+"| ->");

  t->display(flag,"%s+----------------FSMtransition----------------",pref);
  t->display(flag,"%s| TRANSITION:%s #Rules:%d #Predicates:%d Target:%s",pref,
	     t->c_name(), int(r.size()),int(p.size()),t->to()->c_name());
  t->display(flag,"%s|-------------FSMtransition::Rules------------",pref);
  for_each(r.begin(),r.end(),rule_printer);
  t->display(flag,"%s|----------FSMtransition::Predicates----------",pref);
  for_each(p.begin(),p.end(),rule_printer);
  t->display(flag,"%s+----------End of FSMtransition Dump----------",pref);
}

/// Operator invoked for each State object printout.
void PrintObject::operator()(const State* s) const  {
  int flag = TypedObject::ALWAYS;
  const char* pref = prefix.c_str();
  const State::Transitions& in=s->incoming();
  const State::Transitions& out=s->outgoing();
  PrintObject transition_printer(prefix+"| ->");

  s->display(flag,"%s+--------------------- FSMstate -----------+",pref);
  s->display(flag,"%s| Name : %s ",pref,s->c_name());
  s->display(flag,"%s| Transient: %d ",pref,s->isTransient());
  s->display(flag,"%s| Out trans #%d",pref,int(out.size()));
  for_each(out.begin(),out.end(),transition_printer);
  s->display(flag,"%s| In  trans #%d",pref,int(in.size()));
  for_each(in.begin(),in.end(),transition_printer);
  s->display(flag,"%s+------------------------------------------+",pref);
}

/// Operator invoked for each State object printout.
void PrintObject::operator()(const StateHandle& s) const  {
  TypedObject::display(TypedObject::ALWAYS,"%s+--------------------- FSMstate: %s -----------+",
		       prefix.c_str(),s.ptr->c_name());
}

/// Operator invoked for each Predicate object printout.
void PrintObject::operator()(const Predicate* p) const  {
  string states = "";
  const Predicate::States st = p->allowed();
  for(Predicate::States::const_iterator i=st.begin();i!= st.end();++i) states += (*i)->name()+" ";
  TypedObject::display(TypedObject::ALWAYS,"%s| FSMrule/Pred %s: Allowed states:%s\n",
		       prefix.c_str(),p->type()->c_name(),states.c_str());
}

/// Operator invoked for each Rule object printout.
void PrintObject::operator()(const Rule* /*r*/) const  {
#if 0
  int flag = TypedObject::ALWAYS;
  const char* pref = prefix.c_str();
  r->display(flag,"%s| FSMrule/Pred %s==%s [%s]\n",pref,r->type()->c_name(),r->state()->c_name(),
	     r->direction() == Rule::MASTER2SLAVE ? "Master->Task" : "Task->Master");
#endif
}
