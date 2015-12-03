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
#include "FiniteStateMachine/Machine.h"
#include "FiniteStateMachine/Functors.h"
#include "FiniteStateMachine/Transition.h"
#include "CPP/IocSensor.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <cstdarg>
#include <stdexcept>
#include <iostream>
#include <sstream>

using namespace FiniteStateMachine;
using namespace std;

typedef FSM::ErrCond ErrCond;

static IocSensor& ioc() {  return IocSensor::instance();  }

static ErrCond ret_success( Machine* actor, int code)  {
  ioc().send((Machine*)actor,code);
  return FSM::SUCCESS;
}

static ErrCond ret_failure( Machine* actor)  {
  ioc().send((Machine*)actor,Machine::MACH_FAIL);
  return FSM::SUCCESS;
}

static const char* dir(int d) {
  if ( d == Rule::SLAVE2MASTER ) return "Slave->Master";
  else if ( d == Rule::MASTER2SLAVE ) return "Master->Slave";
  return "Unknown";
}

/// Convert meta state to name
static const char* _metaStateName(int state)   {
  switch(state)  {
#define MakeName(x) case Machine::x: return #x
    MakeName(MACH_WAIT_NULL);   
    MakeName(MACH_IDLE);             /// Machine IDLE
    MakeName(MACH_ACTIVE);           /// Machine Active
    MakeName(MACH_OUTACTION);        /// Executing state leave action
    MakeName(MACH_CHK_SLV);          /// Machine is checking if all slave tasks completed the transition
    MakeName(MACH_EXEC_ACT);         /// Executing Action
    MakeName(MACH_INACTION);         /// Executing state enter action
    MakeName(MACH_FAIL);             /// Code used on failure
#undef MakeName
  default:    break;
  }
  return "UNKNOWN";
}

/// Class Constructor
Machine::Machine(const Type *typ, const string& nam)
  : TypedObject(typ,nam), m_fsm(MACH_IDLE), m_currState(0), 
    m_currTrans(0), m_direction(Rule::MASTER2SLAVE)
{
  display(DEBUG,c_name(),"FSMmachine: creating machine %s of type %s",nam.c_str(),typ->c_name());
  setState(m_type->initialState()).setTarget(0);
  Callback cb(this);
  m_fsm.setCurrentState(MACH_IDLE);
  m_fsm.setFailureCallback(cb.make(&Machine::metaTransitionFail));
  m_fsm.setNoTransitionCallback(cb.make(&Machine::metaTransitionMissing));
  m_fsm.addTransition(MACH_IDLE,     MACH_IDLE,     "Idle->Idle",      cb.make(&Machine::goIdle));
  m_fsm.addTransition(MACH_IDLE,     MACH_OUTACTION,"Idle->OutAct",    cb.make(&Machine::doOutAction));

  // Short protocol: If there are no slaves to be checked (SLAVE2MASTER)
  m_fsm.addTransition(MACH_OUTACTION,MACH_EXEC_ACT, "OutAct->WaitAct", cb.make(&Machine::doAction));

  // Full protocol: Full slave handling
  m_fsm.addTransition(MACH_OUTACTION,MACH_ACTIVE,   "OutAct->Active",  cb.make(&Machine::handleSlaves));
  m_fsm.addTransition(MACH_ACTIVE,   MACH_ACTIVE,   "Active->Active",  cb.make(&Machine::checkAliveSlaves));
  m_fsm.addTransition(MACH_ACTIVE,   MACH_CHK_SLV,  "Active->ChkSlv",  cb.make(&Machine::startTransition));
  m_fsm.addTransition(MACH_ACTIVE,   MACH_EXEC_ACT, "ChkSlv->WaitAct", cb.make(&Machine::doAction));

  // Action processing
  m_fsm.addTransition(MACH_EXEC_ACT, MACH_EXEC_ACT, "WaitAct->WaitAct",cb.make(&Machine::nullAction));
  m_fsm.addTransition(MACH_CHK_SLV,  MACH_CHK_SLV,  "ChkSlv->ChkSlv",  cb.make(&Machine::nullAction));
  m_fsm.addTransition(MACH_CHK_SLV,  MACH_EXEC_ACT, "ChkSlv->WaitAct", cb.make(&Machine::doAction));
  m_fsm.addTransition(MACH_EXEC_ACT, MACH_INACTION, "WaitAct->WaitIA", cb.make(&Machine::doInAction));
  m_fsm.addTransition(MACH_INACTION, MACH_IDLE,     "WaitIact->Idle",  cb.make(&Machine::finishTransition));
  // NOT_OK transitions
  m_fsm.addTransition(MACH_IDLE,     MACH_FAIL,     "Idle->Fail",      cb.make(&Machine::doFail));
  m_fsm.addTransition(MACH_ACTIVE,   MACH_FAIL,     "Active->Fail",    cb.make(&Machine::doFail));
  m_fsm.addTransition(MACH_OUTACTION,MACH_FAIL,     "WaitOAct->Fail",  cb.make(&Machine::doFail));
  m_fsm.addTransition(MACH_EXEC_ACT, MACH_FAIL,     "WaitAct->Fail",   cb.make(&Machine::doFail));
  m_fsm.addTransition(MACH_CHK_SLV,  MACH_FAIL,     "CheckSlv->Fail",  cb.make(&Machine::doFail));
  m_fsm.addTransition(MACH_INACTION, MACH_FAIL,     "WaitIAct->Fail",  cb.make(&Machine::doFail));
  m_fsm.addTransition(MACH_FAIL,     MACH_IDLE,     "Fail->Idle",      cb.make(&Machine::goIdleFromFail));
  m_fsm.addTransition(MACH_FAIL,     MACH_FAIL,     "Fail->Fail",      cb.make(&Machine::nullAction));
}

/// Standatrd destructor  
Machine::~Machine()   {
}

/// Meta state name of current meta-state
const char* Machine::currentMetaName()  const   {
  return _metaStateName(m_fsm.currentState());
}

/// Meta state name of previous meta-state
const char* Machine::previousMetaName()  const   {
  return _metaStateName(m_fsm.previousState());
}

/// Meta state name of target meta-state
const char* Machine::targetMetaName()  const   {
  return _metaStateName(m_fsm.targetState());
}

/// IOC and network handler
void Machine::handle(const CPP::Event& event)  {
  switch(event.eventtype) {
  case NetEvent:
    handleNetEvent(event);
    break;
  case IocEvent:
    handleIoc(event);
    break;
  default:
    break;
  }
}

/// Network handler
void Machine::handleNetEvent(const CPP::Event& /* event */)   {
}

/// IOC handler
void Machine::handleIoc(const CPP::Event& event)   {
  ErrCond status = FSM::SUCCESS;
  switch(event.type)  {
  case Slave::SLAVE_ALIVE:
    status = invokeMetaTransition(MACH_ACTIVE,this);
    break;
  case Slave::SLAVE_LIMBO:
    status = invokeMetaTransition(MACH_CHK_SLV,event.data);
    m_meta.execute(this);
    break;
  case Slave::SLAVE_FINISHED:
    status = invokeMetaTransition(MACH_CHK_SLV,event.data);
    m_meta.execute(this);
    break;
  case Slave::SLAVE_TRANSITION:   /// State change by slave. Handle the request.
    display(INFO,c_name(),"Machine got SLAVE_TRANSITION in state:%s/%s - evaluate WHENs. Idle:%s",
            c_state(), currentMetaName(), isIdle() ? "Yes" : "No");
#if 0
    for(Slaves::const_iterator i=m_slaves.begin(); i!= m_slaves.end(); ++i)  {
      const Slave* s = *i;
      display(ALWAYS,c_name(),"Controller Slave %s in state %s/%s",
              s->c_name(), s->c_state(), s->metaStateName());
    }
#endif
    if ( isIdle() ) {
      if ( !evaluateWhens() )   {
        // If no When clause fired, we have to propagate the change of 
        // the slave state to the master for publishing reasons.
        // If a when clause fired, the publishing is done during the transition execution 
        // of the machine.
        m_meta.execute(this);
      }
    }
    break;
  case Slave::SLAVE_FAILED:
    if ( FSM::TRANNOTFOUND == checkSlaves() )  {
      display(WARNING,c_name(),"Machine got SLAVE_FAILED in meta state:%s - evaluate WHENs. Idle:%s",
              currentMetaName(), isIdle() ? "Yes" : "No");
      evaluateWhens();
    }
    m_meta.execute(this);
    break;
  case 0x0FEEDBAB:
    display(WARNING,c_name(),"Machine ****TEST**** in meta state:%s - evaluate WHENs. Idle:%s",
            currentMetaName(), isIdle() ? "Yes" : "No");
    evaluateWhens();
    break;
  case Machine::MACH_EXEC_ACT:
  case Machine::MACH_OUTACTION:
  default:
    status = invokeMetaTransition(event.type,this);
    break;
  }
  if ( status != FSM::SUCCESS )  {
    PrintObject()(this);
  }
}

/// Set all slave matching the meta state mask to the real FSM state.
int Machine::setSlaveState(int meta_mask, const State* state)   {
  SetSlaveState actor = for_each(m_slaves.begin(),m_slaves.end(),SetSlaveState(state,meta_mask));
  return actor.count;
}

/// Collect the states of all slaves
const Machine::States Machine::slaveStates() const   {
  States states;
  for(Slaves::const_iterator i=m_slaves.begin(); i!=m_slaves.end(); ++i)
    states.insert((*i)->state());
  return states;
}

/// Evaluate the when rules accoding to the slave states and invoke transition if required.
bool Machine::evaluateWhens()  {
  const State::Whens& whens = state()->when();
  if ( !whens.empty() )  {
    stringstream state_names;
    const Machine::States states = slaveStates();
    state_names << "Slave states:";
    for(Slaves::const_iterator is=m_slaves.begin(); is!=m_slaves.end(); ++is)
      state_names << (*is)->name() << ":" << (*is)->c_state() << "  ";

    display(INFO,c_name(),"Machine Idle:%s EvaluateWhen: Check %d when clauses.",
            isIdle() ? " YES " : " NO ",int(whens.size()));
    display(DEBUG,c_name(),"%s",state_names.str().c_str());
    for(State::Whens::const_iterator iw=whens.begin(); iw != whens.end(); ++iw)  {
      const When* w = (*iw);
      When::Result res = w->fires(states);
      if ( res.first && res.first != state() )  {
        display(INFO,c_name(),"WHEN clause: %s fired. Invoke tramsition to:%s",
                w->c_name(),res.first->c_name());
        ErrCond cond = invokeTransition(res.first,res.second);
        if ( cond == FSM::SUCCESS ) return true;
      }
      else  {
        display(DEBUG,c_name(),"WHEN clause: %s rejected.",w->c_name());
      }
    }
  }
  return false;
}

/// Callback executed on a failure of a metastate change
ErrCond Machine::metaTransitionFail() {
  display(ALWAYS,c_name(),"FSM> FAILED ACTION from state:%d [%s] to state %d [%s]",
          int(m_fsm.currentState()),_metaStateName(m_fsm.currentState()),
          int(m_fsm.targetState()),_metaStateName(m_fsm.targetState()));
  return FSM::SUCCESS;
}

/// Callback executed if the current transition is not availible
ErrCond Machine::metaTransitionMissing() {
  for(const FSM::MicFSMTransition* tr = m_fsm.head(); tr; tr = tr->next())  {
    display(ALWAYS,c_name(),"FSM> FAILED Transition %s from state:%d [%s] to state %d [%s]",
            tr->condition().c_str(),
            int(tr->from()),_metaStateName(tr->from()),
            int(tr->to()),_metaStateName(tr->to()));
  }
  display(ALWAYS,c_name(),"FSM> UNKNOWN TRANSITION  from state:%d [%s] to state %d [%s]",
          int(m_fsm.currentState()),_metaStateName(m_fsm.currentState()),
          int(m_fsm.targetState()),_metaStateName(m_fsm.targetState()));

  return FSM::SUCCESS;
}

/// Invoke FSM transition
ErrCond Machine::invokeTransition (const Transition* transition, Rule::Direction direction)   {
  if ( transition )  {
    setTarget(transition);
    m_direction = direction;
    m_fsm.setCurrentState(MACH_IDLE);
    for_each(m_slaves.begin(),m_slaves.end(),SlaveReset());
    return ret_success(this,MACH_OUTACTION);
  }
  return FSM::TRANNOTFOUND;
}

/// Invoke FSM transition
ErrCond Machine::invokeTransitionByName(const string& transition, Rule::Direction direction)   {
  if ( !transition.empty() )  {
    const State* st = m_currState;
    if ( st )  {
      for(State::Transitions::const_iterator i=st->outgoing().begin(); i!=st->outgoing().end(); ++i)    {
        const Transition* tr = *i;
        if ( tr->name() == transition )   {
          return invokeTransition(tr,direction);
        }
      }
    }
    const Transition* tr = type()->transition(transition);
    if ( tr ) return invokeTransition(tr,direction);
  }
  return FSM::TRANNOTFOUND;
}

/// Invoke FSM transition to target state
ErrCond Machine::invokeTransition (const State* target_state, Rule::Direction direction)   {
  const Transition* tr = target_state ? state()->findTrans(target_state) : 0;
  return tr ? invokeTransition(tr,direction) : ErrCond(FSM::TRANNOTFOUND);
}

/// Invoke FSM transition to target state
ErrCond Machine::invokeTransition(const std::string& target_state, Rule::Direction direction)   {
  return invokeTransition(type()->state(target_state),direction);
}

/// Invoke meta transition on FSM object
ErrCond Machine::invokeMetaTransition(unsigned int target, const void* param)  {
#if 0
  display(DEBUG,c_name(),"FSM> EXEC META TRANSITION  from state:%d [%s] to state %d [%s]",
          int(m_fsm.currentState()),_metaStateName(m_fsm.currentState()),
          int(target),_metaStateName(target));
#endif
  return m_fsm.invokeTransition(target,param);
}

/// Finish lengthy state leave action
ErrCond Machine::goIdle()  {
  m_fsm.setCurrentState(MACH_IDLE);
  setTarget(0);
  return FSM::SUCCESS;
}

/// Finish failed transition and return to the IDLE status
ErrCond Machine::goIdleFromFail()  {
  ioc().send(this,Slave::SLAVE_TRANSITION,this);
  return goIdle();
}

/// Null action
ErrCond Machine::nullAction()  const {
  return FSM::SUCCESS;
}

/// Start slaves if required by transition descriptor
ErrCond Machine::handleSlaves()  {
  const Transition* tr = currTrans();
  const Slaves&     sl = slaves();
  if ( sl.size() > 0 && tr->create() )  {
    SlaveStarter starter=for_each(sl.begin(),sl.end(),SlaveStarter(tr));
    m_meta.execute(this);
    // Failed to create some of the slaves
    if ( starter.fail >0 ) return ret_failure(this);
    // Slaves are created, wait for the IAMHERE. We can only continue if ALL slaves are alive
    if ( starter.dead>0  ) return FSM::SUCCESS;
    if ( starter.count< sl.size() ) return FSM::SUCCESS;
  }
  else if ( sl.size() > 0 && tr->killActive() )  {
    SlaveKiller killer=for_each(sl.begin(),sl.end(),SlaveKiller(tr));
    m_meta.execute(this);
    // All slaves are dead now
    if ( killer.dead == sl.size() ) return ret_success(this,MACH_ACTIVE);
    // Some slaves are still in the process to die....
    if ( killer.count>0           ) return FSM::SUCCESS;
  }
  return ret_success(this,MACH_ACTIVE);
}

/// Start slaves if required by transition descriptor
ErrCond Machine::checkAliveSlaves()  {
  const Transition* tr = currTrans();
  if ( tr ) {
    const Slaves&     sl = slaves();
    if ( !sl.empty() && tr->checkLimbo() )  {
      SlaveLimboCount limbos=for_each(sl.begin(),sl.end(),SlaveLimboCount());
      m_meta.execute(this);
      if ( limbos.count>0 ) return FSM::SUCCESS;
    }
    return ret_success(this,MACH_CHK_SLV);
  }
  return ret_success(this,Slave::SLAVE_TRANSITION);
}

/// Check slave status. Once all slaves have answered, invoke transition
ErrCond Machine::checkSlaves()   {
  const Slaves&     sl = slaves();
  const Transition* tr = currTrans();
  if ( tr )  {
    // Here we only enter in the presence of slaves!
    CheckStateSlave check = for_each(sl.begin(),sl.end(),CheckStateSlave(tr));
    size_t bad = check.fail+check.dead;
    size_t count = check.count+bad;
    display(DEBUG,c_name(),"Executing '%s'. Count:%d Fail:%d Dead:%d Other:%d Answered:%d Size:%d",
            tr->c_name(), int(check.count), int(check.fail), int(check.dead),
            int(check.other), int(check.answered), int(sl.size()));
    m_meta.execute(this);
    if ( tr->killActive() && check.dead < sl.size() )   {
      display(DEBUG,c_name(),"WAIT_ACTION '%s'. Count:%d Fail:%d Dead:%d Size:%d KillActive:%s",
              tr->c_name(), int(check.count), int(check.fail), int(check.dead),int(sl.size()),
              tr->killActive() ? "YES" : "NO");
      return FSM::WAIT_ACTION;
    }
    else if ( tr->create()     && check.dead > 0    )   {
      display(DEBUG,c_name(),"WAIT_ACTION '%s'. Count:%d Fail:%d Dead:%d Size:%d Create:%s",
              tr->c_name(), int(check.count), int(check.fail), int(check.dead),int(sl.size()),
              tr->create() ? "YES" : "NO");
      return FSM::WAIT_ACTION;
    }
    else if ( tr->checkLimbo() && !tr->create() && check.dead > 0 && count == sl.size() )   {
      display(DEBUG,c_name(),"WAIT_FAIL   '%s'. Count:%d Fail:%d Dead:%d Size:%d CheckLimbo:%s Create:%s",
              tr->c_name(), int(check.count), int(check.fail), int(check.dead),int(sl.size()),
              tr->checkLimbo() ? "YES" : "NO", tr->create() ? "YES" : "NO");
      return ret_failure(this);
    }
    else if ( tr->checkLimbo() && check.dead > 0    )   {
      display(DEBUG,c_name(),"WAIT_ACTION '%s'. Count:%d Fail:%d Dead:%d Size:%d CheckLimbo:%s",
              tr->c_name(), int(check.count), int(check.fail), int(check.dead),int(sl.size()),
              tr->checkLimbo() ? "YES" : "NO");
      return FSM::WAIT_ACTION;
    }
    // Either the transition is finished or all slaves should be dead now....
    if ( check.count == sl.size() )  {
      return ret_success(this,MACH_EXEC_ACT);
    }
    else if ( tr->killActive() && check.dead == sl.size() )  {
      return ret_success(this,MACH_EXEC_ACT);
    } 
    else if ( !tr->checkLimbo() && check.fail==0 && check.dead+check.count==sl.size() )  {
      return ret_success(this,MACH_EXEC_ACT);
    }
    else if ( check.fail>0 && check.count+check.fail == sl.size() )   {
      display(INFO,c_name(),"Executing '%s'. Invoke MACH_FAIL. count:%d fail:%d dead:%d",
              tr->c_name(), int(check.count), int(check.fail), int(check.dead));
      return ret_failure(this);
    }
    else if ( check.fail>0 )   {
      return FSM::FAIL;
    }
    else if ( check.answered == sl.size() && check.count+check.other == sl.size() )  {
      // This may only happen, if a slave changes state again before the transition finished.
      // In this event we have to cancel the transition and work down the slave state change.
      goIdle();
      return ret_success(this,Slave::SLAVE_TRANSITION);
    }
    else if ( check.answered == sl.size() && check.count>0 )  {
      // This may happen, if there is a transition request where 'any' of the children are
      // sufficient to trigger the parent transition.
      goIdle();
      return ret_success(this,Slave::SLAVE_TRANSITION);
    }
    return FSM::FAIL;
  }
  return FSM::TRANNOTFOUND;
}

/// Check predicates and apply rules to slaves
ErrCond Machine::startTransition()  {
  int st = m_fsm.currentState();
  if ( st == MACH_ACTIVE )  {
    const Slaves&     sl = slaves();
    const Transition* tr = currTrans();
    TransitionActionMap::const_iterator i = m_transActions.find(tr);
    if ( i != m_transActions.end() )  {
      int sc = (*i).second.pre().execute(this);
      if ( sc != FSM::SUCCESS )  {
        return ret_failure(this);
      }
    }
    if ( !sl.empty() )  {
      PredicateSlave pred = for_each(sl.begin(),sl.end(),PredicateSlave(tr));
      if ( pred.ok() )   {
        display(DEBUG,c_name(),"Executing %s. Predicates checking finished successfully. Dir:%s #Slaves:%d",
                tr->c_name(),dir(m_direction),sl.size());
        m_meta.execute(this);
        InvokeSlave func = for_each(sl.begin(),sl.end(),InvokeSlave(tr,m_direction));
        if ( func.status == FSM::WAIT_ACTION )  {
          return FSM::SUCCESS;
        }
        else if ( !func.ok() )  {
          display(INFO,c_name(),"Executing %s. Failed to invoke slaves according to rules.",tr->c_name());
          return ret_failure(this);
        }
        return ret_success(this,MACH_EXEC_ACT);
      }
      return ret_failure(this);
    }
    return ret_success(this,MACH_EXEC_ACT);
  }
  else if ( st == MACH_CHK_SLV )  {
    return FSM::SUCCESS;
  }
  return ret_failure(this);
}

/// Finish any lengthy action
ErrCond Machine::finishAnyAction(ErrCond status, int next_state)  {
  if      ( status == FSM::SUCCESS     ) return ret_success(this,next_state);
  else if ( status == FSM::WAIT_ACTION ) return FSM::SUCCESS;  // Noop, self transition
  return ret_failure(this); // BADPREACTION,BADPOSTACTION,BADACTION,FAIL
}

/// Execute state leave action
ErrCond Machine::doOutAction (const void* user_param)  {
  if ( state() )  {
    StateActionMap::const_iterator i = m_stateActions.find(state());
    int sc = (i==m_stateActions.end()) ? (*i).second.out().execute(user_param) : ErrCond(FSM::SUCCESS);
    return finishOutAction(sc);
  }
  return FSM::FAIL;
}

/// Finish lengthy state leave action
ErrCond Machine::finishOutAction(ErrCond status)  {
  return finishAnyAction(status,MACH_ACTIVE);
}

/// Execute state leave action
ErrCond Machine::doAction (const void* user_param)  {
  const Transition* tr = currTrans();
  TransitionActionMap::const_iterator i = m_transActions.find(tr);
  m_meta.execute(this);
  if ( i != m_transActions.end() )  {
    int sc = (*i).second.action().execute(user_param);
    return finishAction(sc);
  }
  return finishAction(FSM::SUCCESS);
}

/// Finish lengthy state leave action
ErrCond Machine::finishAction(ErrCond status)  {
  return finishAnyAction(status,MACH_INACTION);
}

/// Execute state leave action
ErrCond Machine::doInAction (const void* user_param)  {
  const Transition* tr = currTrans();
  if ( tr->to() )  {
    StateActionMap::const_iterator i = m_stateActions.find(tr->to());
    int sc = (i==m_stateActions.end()) ? ErrCond(FSM::SUCCESS) : (*i).second.in().execute(user_param);
    return finishInAction(sc);
  }
  return FSM::FAIL;
}

/// Finish lengthy state leave action
ErrCond Machine::finishInAction(ErrCond status)  {
  return finishAnyAction(status,MACH_IDLE);
}

/// Execute state leave action
ErrCond Machine::finishTransition (const void* user_param)  {
  const Transition* tr = currTrans();
  TransitionActionMap::const_iterator i = m_transActions.find(tr);
  setState(tr->to()).setTarget(0);
  if ( i != m_transActions.end() ) (*i).second.completion().execute(user_param);
  m_completion.execute(user_param);
  display(INFO,c_name(),"Executing %s. Finished transition. Current state:%s",tr->c_name(),tr->to()->c_name());
  m_direction = Rule::MASTER2SLAVE;
  return FSM::SUCCESS;
}

/// Execute state leave action
ErrCond Machine::doFail (const void* user_param)  {
  TransitionActionMap::const_iterator i = m_transActions.find(currTrans());
  ErrCond ret = ret_success(this,MACH_IDLE); // Must be first to allow transition chains
  if ( i != m_transActions.end() )
    (*i).second.fail().execute(user_param);
  m_fail.execute(user_param);
  m_meta.execute(this);
  return ret;
}

/// Update callback structures always called when a transition starts
Machine& Machine::setMetaStateAction (const Callback& action)   {
  m_meta = action;
  return *this;
}

/// Update callback structures always called when a transition fails
Machine& Machine::setFailAction (const Callback& action)  {
  m_fail = action;
  return *this;
}

/// Update callback structures always called when a transition completes
Machine& Machine::setCompletionAction (const Callback& action)   {
  m_completion = action;
  return *this;
}

/// Update callback structures for state enter action. 
Machine& Machine::setInAction (const State* state, const Callback& action)   {
  if ( state ) m_stateActions[state].setInAction(action);
  return *this;
}

/// Update callback structures for state leave action. 
Machine& Machine::setOutAction (const State* state, const Callback& action)  {
  if ( state ) m_stateActions[state].setOutAction(action);
  return *this;
}

/// Update action callback. If there where already action callbacks installed, delete them.
Machine& Machine::setAction(const Transition* transition, const Callback& action)  {
  if ( transition ) m_transActions[transition].setAction(action);
  return *this;
}

/// Update pre-action callback. If there where already action callbacks installed, delete them.
Machine& Machine::setPreAction(const Transition* transition, const Callback& action)  {
  if ( transition ) m_transActions[transition].setPreAction(action);
  return *this;
}

/// Update fail-action callback. If there where already action callbacks installed, delete them.
Machine& Machine::setFailAction(const Transition* transition, const Callback& action)  {
  if ( transition ) m_transActions[transition].setFailAction(action);
  return *this;
}

/// Update completion-action callback. If there where already action callbacks installed, delete them.
Machine& Machine::setCompletionAction(const Transition* transition, const Callback& action)  {
  if ( transition ) m_transActions[transition].setCompletionAction(action);
  return *this;
}

/// Add a slave to this FSM  machine
FSM::ErrCond Machine::addSlave(Slave* slave)   {
  if ( slave )  {
    Slaves::iterator i=find(m_slaves.begin(),m_slaves.end(),slave);
    if ( i == m_slaves.end() ) {
      m_slaves.push_back(slave);
      return FSM::SUCCESS;
    }
  }
  throw runtime_error(name()+"> Attempt to add invalid slave.");
}

/// Remove a slave from this FSM  machine
FSM::ErrCond Machine::removeSlave(Slave* slave)   {
  if ( slave )  {
    Slaves::iterator i=find(m_slaves.begin(),m_slaves.end(),slave);
    if ( i != m_slaves.end() ) {
      m_slaves.erase(i);
      return FSM::SUCCESS;
    }
  }
  throw runtime_error(name()+"> Attempt to add invalid slave.");
}
    
