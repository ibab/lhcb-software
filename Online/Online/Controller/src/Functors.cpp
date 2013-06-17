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
#include <cstdio>

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
    FSM::ErrCond sc = s->startSlave();
    if ( sc == FSM::WAIT_ACTION  ) ++dead;
    else if ( sc == FSM::SUCCESS ) ++count;
    else ++fail;
  }
}

/// Operator invoked for each slave to be killed
void SlaveKiller::operator()(Slave* s)    {
  FSM::ErrCond sc = s->killSlave();
  if ( sc == FSM::SUCCESS     ) ++dead;
  if ( sc == FSM::WAIT_ACTION ) ++count;
  else ++fail;
}

/// Operator invoked for each rule and invoke slave accordingly
void InvokeSlave::operator()(Slave* s)   {
  const Rule*  rule = 0;
  const State* slState = s->state();
  const Transition::Rules& r = object->rules();

  if ( direction != Rule::MASTER2SLAVE ) return;
  if ( !object->checkLimbo() && s->isLimbo() ) return; 
  // We favor identical transitions over artificial ones!
  for(Transition::Rules::const_iterator i=r.begin(); i!=r.end();++i)  {
    rule = (*i);
    const Transition* t = rule->transition();
    if ( t == object && object->from() == slState ) 
      goto Apply;
  }
  for(Transition::Rules::const_iterator i=r.begin(); i!=r.end();++i)  {
    rule = (*i);
    if ( rule->applies(slState,direction) )
      goto Apply;
  }
  return;
 Apply:
  FSM::ErrCond sc = s->apply(rule);
  if      ( status == FSM::SUCCESS && sc == FSM::WAIT_ACTION ) status = sc;
  else if ( sc == FSM::TRANNOTFOUND ) { /* Rule does not apply */ }
  else if ( sc != FSM::SUCCESS      ) status = sc;
}

/// Operator invoked for each slave to be checked
void SetSlaveState::operator()(Slave* s)     {
  if ( (mask&s->currentState()) != 0 )  {
    s->setState(object);
    ++count;
  }
}

/// Operator invoked for each slave to be checked
void SlaveLimboCount::operator()(const Slave* s)     {
  if ( s->isLimbo() )  ++count;
}

/// Operator invoked for each task to reset its state to idle
void SlaveReset::operator()(Slave* s) const {
  s->setCurrentState(s->isAlive() ? Slave::SLAVE_ALIVE : Slave::SLAVE_LIMBO);
}

/// Operator invoked for each rule to check if it is fulfilled
void CheckStateSlave::operator()(const Slave* s)   {  
  const State* slState = s->state();
  const Type*  slType  = s->type();
  const Rule*  slRule  = s->rule();
  const Transition::Rules& r = object->rules();

  if ( s->currentState() == Slave::SLAVE_FAILED ) { 
    ++fail;  // Must be first check: Slave may have failed on start while limbo....
  }
  else if ( s->isLimbo() ) { 
    ++dead;  // Count the number of dead slaves.
  }
  else if ( s->currentState() == Slave::SLAVE_EXECUTING ) { 
    // Nothing to do: Slave has not yet answered
  }
  else if ( slRule && slState && slRule->targetState() == slState )  {
    ++count;  // Slave fulfills state criteria according to rule
  }
  else if ( slState && slState == object->to() ) {
    ++count;  // Slave of same fulfills state criteria
  }
  else if ( slRule && slState && slRule->currState() == slState )  {
    // Nothing to do: Slave has not yet answered
  }
  else if ( slState && object->from() == slState )  {
    // Nothing to do: Slave has not yet answered
  }
  else   {  // Check if all rules for this type are fulfilled
    for(Transition::Rules::const_iterator i=r.begin(); i != r.end(); ++i)  {
      const Rule* rule = (*i);
      if ( rule->type() == slType )  {
	if      ( slRule  == rule && slState == object->to() && slState==rule->targetState() )
	  ++count;  // Slave fulfills state criteria
	else if ( slState == object->to() && slState == rule->targetState() )  
	  ++count;  // Slave fulfills state criteria
	else if ( slState == object->to() )
	  ++count; // There was nothing to be done.
	else {
	  char text1[128], text2[128];
	  ::snprintf(text1,sizeof(text1),"%s",Rule::c_name(rule));
	  ::snprintf(text2,sizeof(text2),"%s",Rule::c_name(slRule));
	  ++fail;  // What can I do: there was a rule, but the slave did not go to the target state
	  s->display(s->ALWAYS,s->c_name(),"FAILED Metastate:%s [Rule not fulfilled] State:%s",
		     s->metaStateName(),State::c_name(slState));
	  s->display(s->ALWAYS,s->c_name(),"Tr:%s from:%s to:%s Rule:%s -- %s",
		     object->c_name(),State::c_name(object->from()),State::c_name(object->to()),text1,text2);
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
    if      ( !check_limbo && s->isLimbo() ) continue;
    else if ( !p->hasState(s->state()) ) status = FSM::FAIL;
  }
}

/// Operator invoked for each machine during printing.
void PrintObject::operator()(const Machine* m) const  {
  int flag = TypedObject::ALWAYS;
  const char* pref = prefix.c_str();
  const Type* typ = m->type();
  const Transition* tr = m->currTrans();
  m->display(flag,pref,"+-------- FSMmachine: %s of type %s.",m->c_name(),typ->c_name());
  m->display(flag,pref,"|  Meta transition:    Prev:%s  Curr:%s Target:%s",
	     m->previousMetaName(),m->currentMetaName(),m->targetMetaName());
  m->display(flag,pref,"|  Current state:      %s",m->c_state());
  m->display(flag,pref,"|  Current Transition: %s",tr ? tr->c_name()       : "----");
  m->display(flag,pref,"|  Target  state:      %s",tr ? tr->to()->c_name() : "----");
  PrintObject slave_printer("|++"+prefix);
  for_each(m->slaves().begin(),m->slaves().end(),slave_printer);
  m->display(flag,pref,"+----------------------------------------------------------------------");
}

/// Operator invoked for each Machine object printout.
void PrintObject::operator()(const MachineHandle& h) const  {
  const Machine*     m = h.ptr;
  const Type*        t = m->type();
  const Transition* tr = m->currTrans();
  m->display(TypedObject::ALWAYS,prefix.c_str(),"+- FSMmachine: %s of type %s State:%-12s [%s] %s %s",
	     m->c_name(),t->c_name(),m->c_state(),m->currentMetaName(),
	     tr ? "Executing transition" : "Idle", tr ? tr->c_name() : "----");
}

/// Operator invoked for each machine during printing.
void PrintObject::operator()(const Slave* s) const  {
  s->display(TypedObject::ALWAYS,prefix.c_str(),
	     "+  Slave: %-32s of type %s: State:%-12s Meta-state:%-14s managed by %s",
	     s->c_name(),s->type()->c_name(),s->c_state(),
	     s->metaStateName(),s->c_machine());
}

/// Operator invoked for each Transition object printing.
void PrintObject::operator()(const Transition* t) const   {
  int flag = TypedObject::ALWAYS;
  const char* pref = prefix.c_str();
  const Transition::Rules& r=t->rules();
  const Transition::Predicates& p=t->predicates();
  PrintObject rule_printer(prefix+"| ->");

  t->display(flag,pref,"+----------------FSMtransition----------------");
  t->display(flag,pref,"| TRANSITION:%s #Rules:%d #Predicates:%d Target:%s",
	     t->c_name(), int(r.size()),int(p.size()),t->to()->c_name());
  t->display(flag,pref,"|-------------FSMtransition::Rules------------");
  for_each(r.begin(),r.end(),rule_printer);
  t->display(flag,pref,"|----------FSMtransition::Predicates----------");
  for_each(p.begin(),p.end(),rule_printer);
  t->display(flag,pref,"+----------End of FSMtransition Dump----------");
}

/// Operator invoked for each State object printout.
void PrintObject::operator()(const State* s) const  {
  int flag = TypedObject::ALWAYS;
  const char* pref = prefix.c_str();
  const State::Transitions& in=s->incoming();
  const State::Transitions& out=s->outgoing();
  PrintObject transition_printer(prefix+"| ->");

  s->display(flag,pref,"+--------------------- FSMstate -----------+");
  s->display(flag,pref,"| Name : %s ",s->c_name());
  s->display(flag,pref,"| Transient: %d ",s->isTransient());
  s->display(flag,pref,"| Out trans #%d",int(out.size()));
  for_each(out.begin(),out.end(),transition_printer);
  s->display(flag,pref,"| In  trans #%d",int(in.size()));
  for_each(in.begin(),in.end(),transition_printer);
  s->display(flag,pref,"+------------------------------------------+");
}

/// Operator invoked for each State object printout.
void PrintObject::operator()(const StateHandle& s) const  {
  TypedObject::display(TypedObject::ALWAYS,prefix.c_str(),
		       "+--------------------- FSMstate: %s -----------+",s.ptr->c_name());
}

/// Operator invoked for each Predicate object printout.
void PrintObject::operator()(const Predicate* p) const  {
  string states = "";
  const Predicate::States st = p->allowed();
  for(Predicate::States::const_iterator i=st.begin();i!= st.end();++i) states += (*i)->name()+" ";
  TypedObject::display(TypedObject::ALWAYS,prefix.c_str(),"| FSMrule/Pred %s: Allowed states:%s\n",
		       p->type()->c_name(),states.c_str());
}

/// Operator invoked for each Rule object printout.
void PrintObject::operator()(const Rule* /*r*/) const  {
#if 0
  int flag = TypedObject::ALWAYS;
  const char* pref = prefix.c_str();
  r->display(flag,pref,"| FSMrule/Pred %s==%s [%s]\n",r->type()->c_name(),r->state()->c_name(),
	     r->direction() == Rule::MASTER2SLAVE ? "Master->Task" : "Task->Master");
#endif
}
