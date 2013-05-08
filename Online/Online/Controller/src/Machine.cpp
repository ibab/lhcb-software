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

// C/C++ include files
#include <cstdarg>
#include <stdexcept>

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
  default: return "UNKNOWN";
  }
  return "UNKNOWN";
}

/// Class Constructor
Machine::Machine(const Type *typ, const string& nam)
  : TypedObject(typ,nam), m_fsm(MACH_IDLE), m_currState(0), 
    m_currTrans(0), m_direction(Rule::MASTER2SLAVE)
{
  display(DEBUG,"FSMmachine: creating machine %s of type %s",c_name(),typ->c_name());
  setState(m_type->initialState()).setTarget(0);
  Callback cb(this);
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
}

/// Standatrd destructor  
Machine::~Machine()   {
}

/// Meta state name of current meta-state
const char* Machine::currentMetaName()  const   {
  return _metaStateName(m_fsm.currentState());
}

/// Meta state name of target meta-state
const char* Machine::previousMetaName()  const   {
  return _metaStateName(m_fsm.previousState());
}

/// IOC and network handler
void Machine::handle(const Event& event)  {
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
void Machine::handleNetEvent(const Event& /* event */)   {
}

/// IOC handler
void Machine::handleIoc(const Event& event)   {
  ErrCond status = FSM::SUCCESS;
  switch(event.type)  {
  case Slave::SLAVE_ALIVE:
    status = m_fsm.invokeTransition(MACH_ACTIVE,this);
    break;
  case Slave::SLAVE_LIMBO:
    status = m_fsm.invokeTransition(MACH_CHK_SLV,event.data);
    break;
  case Slave::SLAVE_FINISHED:
    status = m_fsm.invokeTransition(MACH_CHK_SLV,event.data);
    break;
  case Slave::SLAVE_TRANSITION:   /// State change by slave. Handle the request.
    evaluateWhens();
    break;
  case Slave::SLAVE_FAILED:
    checkSlaves();
    break;
  case Machine::MACH_EXEC_ACT:
    status = m_fsm.invokeTransition(MACH_EXEC_ACT,this);
    break;
  default:
    status = m_fsm.invokeTransition(event.type,this);
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
void Machine::evaluateWhens()  {
  const State::Whens& whens = state()->when();
  if ( !whens.empty() )  {
    const Machine::States states = slaveStates();
    display(INFO,"%s> Machine Idle:%s EvaluateWhen: Check %d when clauses.",
	    c_name(),isIdle() ? " YES " : " NO ",int(whens.size()));
    for(State::Whens::const_iterator iw=whens.begin(); iw != whens.end(); ++iw)  {
      const When* w = (*iw);
      When::Result res = w->fires(states);
      if ( res.first )  {
	display(INFO,"%s> WHEN clause: %s fired. Invoke tramsition to:%s",
		c_name(),w->c_name(),res.first->c_name());
	invokeTransition(res.first,res.second);
	return;
      }
      else  {
	display(INFO,"%s> WHEN clause: %s rejected.",c_name(),w->c_name());
      }
    }
  }
}

/// Invoke FSM transition
ErrCond Machine::invokeTransition (const Transition* transition, Rule::Direction direction)   {
  if ( transition )  {
    setTarget(transition);
    m_direction = direction;
    m_fsm.setCurrentState(MACH_IDLE);
    for_each(m_slaves.begin(),m_slaves.end(),SlaveReset());
    return m_fsm.invokeTransition(MACH_OUTACTION,this);
  }
  return FSM::TRANNOTFOUND;
}

/// Invoke FSM transition
ErrCond Machine::invokeTransitionByName(const string& transition, Rule::Direction direction)   {
  if ( !transition.empty() )  {
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

/// Finish lengthy state leave action
ErrCond Machine::goIdle()  {
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
  const Slaves&     sl = slaves();
  const Transition* tr = currTrans();
  if ( !sl.empty() && tr->checkLimbo() )  {
    SlaveLimboCount limbos=for_each(sl.begin(),sl.end(),SlaveLimboCount());
    m_meta.execute(this);
    if ( limbos.count>0 ) return FSM::SUCCESS;
  }
  return ret_success(this,MACH_CHK_SLV);
}

/// Check slave status. Once all slaves have answered, invoke transition
ErrCond Machine::checkSlaves()   {
  const Slaves&     sl = slaves();
  const Transition* tr = currTrans();
  if ( tr )  {
    // Here we only enter in the presence of slaves!
    CheckStateSlave check = for_each(sl.begin(),sl.end(),CheckStateSlave(tr));
    if ( tr->killActive() && check.dead < sl.size() )
      return FSM::WAIT_ACTION;
    else if ( tr->create()     && check.dead > 0    )
      return FSM::WAIT_ACTION;
    else if ( tr->checkLimbo() && check.dead > 0    )
      return FSM::WAIT_ACTION;
    // Either the transition is finished or all slaves should be dead now....
    if ( check.count == sl.size() )
      return ret_success(this,MACH_EXEC_ACT);
    else if ( tr->killActive() && check.dead == sl.size() )
      return ret_success(this,MACH_EXEC_ACT);
    else if ( !tr->checkLimbo() && check.fail==0 && check.dead+check.count==sl.size() )
      return ret_success(this,MACH_EXEC_ACT);
    else if ( check.fail>0 && check.count+check.fail == sl.size() )   {
      display(INFO,"%s> Executing %s. Invoke MACH_FAIL. count:%d fail:%d dead:%d",
	      c_name(),tr->c_name(), int(check.count), int(check.fail), int(check.dead));
      return ret_failure(this);
    }
    else if ( check.fail>0 ) 
      return FSM::FAIL;
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
    if ( !sl.empty() )  {
      PredicateSlave pred = for_each(sl.begin(),sl.end(),PredicateSlave(tr));
      if ( pred.ok() )   {
	display(DEBUG,"%s> Executing %s. Predicates checking finished successfully.",c_name(),tr->c_name());
	InvokeSlave func = for_each(sl.begin(),sl.end(),InvokeSlave(tr,m_direction));
	m_meta.execute(this);
	if ( func.status == FSM::WAIT_ACTION )  {
	  return FSM::SUCCESS;
	}
	else if ( !func.ok() )  {
	  display(INFO,"%s> Executing %s. Failed to invoke slaves according to rules.",c_name(),tr->c_name());
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
    int sc = (*i).second.pre().execute(user_param);
    if ( sc == FSM::SUCCESS )  {
      sc = (*i).second.action().execute(user_param);
      return finishAction(sc);
    }
    return ret_failure(this);
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
  display(INFO,"%s> Executing %s. Finished transition. Current state:%s",c_name(),tr->c_name(),tr->to()->c_name());
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
    
