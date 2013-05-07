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
#include "FiniteStateMachine/Machine.h"
#include "FiniteStateMachine/FSMTypes.h"
#include "FiniteStateMachine/Transition.h"
#include "Controller/Controller.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <cstdio>
#include <memory>

using namespace std;
using namespace FiniteStateMachine;
using namespace FiniteStateMachine::DAQ;

/// Constructor
Controller::Controller(const string&  nam, Machine* m)
  : CommandTarget(nam), m_errorState(0), m_machine(m)
{
  const Type*  typ = m_machine->type();
  const State* rdy = typ->state(ST_NAME_READY);
  m_errorState = typ->state(ST_NAME_ERROR);
  m->setFailAction(Callback::make(this,&Controller::fail));
  m->setCompletionAction(Callback::make(this,&Controller::publish));
  m->setInAction(rdy,Callback::make(this,&Controller::ready));
  publish();
}

/// Standard destructor
Controller::~Controller() {
}

/// Publish state information when transition failed. 
FSM::ErrCond Controller::fail()  {
  const Transition* tr = m_machine->currTrans();
  // Nothing to do, since failure will be handled with IOC SLAVE_FAILED
  display(ALWAYS,"%s::%s> Controller: FAILED to invoke transition %s from %s Metastate:%s",
	  c_name(), m_machine->type()->c_name(), tr ? tr->c_name() : "", 
	  m_machine->c_state(), m_machine->currentMetaName());
  for(Machine::Slaves::iterator i=m_machine->slaves().begin(); i!= m_machine->slaves().end(); ++i)  {
    Slave* s = *i;
    if ( s->currentState() == Slave::SLAVE_FAILED ) s->setState(m_errorState);
    display(ALWAYS,"%s> Controller: Slave %s in state %s has meta-state:%s",
	    c_name(), s->c_name(), s->c_state(), s->metaStateName());
  }
  IocSensor::instance().send(this,ERROR_PROCESS,this);
  return FSM::SUCCESS;
}

/// Publish state information when transition is completed
FSM::ErrCond Controller::publish()  {
  string state = m_machine->c_state();
  return declareState(state);
}

/// State enter action for READY: Reset all internal slaves to external ones
FSM::ErrCond Controller::ready()  {
  for(Machine::Slaves::iterator i=m_machine->slaves().begin(); i!= m_machine->slaves().end(); ++i)  {
    Slave* s = *i;
    if ( s->isInternal() )  {
      s->setInternal(false);
    }
  }
  return FSM::SUCCESS;
}

/// Interrupt handling routine
void Controller::handle(const Event& ev)    {
  Slave* slave = ev.iocPtr<Slave>();
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type)  {
    case EXIT_PROCESS:
      ::lib_rtl_sleep(500);
      ::exit(0);
      break;
    case ERROR_PROCESS:
      m_machine->invokeTransition(m_errorState,Rule::SLAVE2MASTER);
      break;
    default:
      display(ALWAYS,"Controller: %s> ERROR: Invoke transition from %s action:%d",
	      c_name(),m_machine->c_state(),ev.type);
      //m_machine->invokeTransition((const State*)ev.data);
      break;
    }
    break;
  default:
    break;
  }
}

/// Set transition target state
void Controller::setTargetState(int target) {
  m_monitor.targetState = char(target);
}

/// Invoke single transition request on machine
FSM::ErrCond Controller::invokeTransition(const string& transition)  {
  const State*      state = m_machine->state();
  const Transition* trans = state->findTransByName(transition);
  ErrCond ret = trans ? m_machine->invokeTransition(trans) : ErrCond(FSM::TRANNOTFOUND);
  if ( ret != FSM::SUCCESS )   {
    //setTargetState(ERROR);
    //ret = m_machine->invokeTransition(ST_NAME_ERROR,Rule::SLAVE2MASTER);
    //display(ALWAYS,"Unknown transition:%s from state %s. Moving to ERROR!",
    //    transition.c_str(),state->c_name());
  }
  return ret;
}

/// DimCommand overload: handle DIM commands
void Controller::commandHandler()   {
  // Decouple as quickly as possible from the DIM command loop !
  std::string cmd = getString();
  display(NOLOG,"%s> Received transition request:%s",c_name(),cmd.c_str());
  if ( !m_machine->isIdle() )  {
    display(ERROR,"%s> Machine is not idle!",c_name());
    m_machine->goIdle();
  }
  if ( cmd == "load"  )  {
    setTargetState(NOT_READY);
    invokeTransition(cmd);
  }
  else if ( cmd == "configure")  {
    setTargetState(READY);
    invokeTransition(cmd);
  }
  else if ( cmd == "start"    )  {
    setTargetState(RUNNING);
    invokeTransition(cmd);
  }
  else if ( cmd == "stop"     )  {
    setTargetState(STOPPED);
    invokeTransition(cmd);
  }
  else if ( cmd == "reset"    )  {
    setTargetState(NOT_READY);
    invokeTransition(cmd);
  }
  else if ( cmd == "pause"    )  {
    setTargetState(PAUSED);
    invokeTransition(cmd);
  }
  else if ( cmd == "continue" )  {
    setTargetState(RUNNING);
    invokeTransition(cmd);
  }
  else if ( cmd == "recover" )  {
    setTargetState(OFFLINE);
    invokeTransition(cmd);
  }
  else if ( cmd == "RESET" )  {
    setTargetState(OFFLINE);
    invokeTransition(cmd);
  }
  else if ( cmd == "unload"   )  {
    setTargetState(OFFLINE);
    invokeTransition(cmd);
  }
  else if ( cmd == "destroy"   )  {
    setTargetState(UNKNOWN);
    IocSensor::instance().send(this,EXIT_PROCESS,this);
    declareState(ST_NAME_UNKNOWN);
    return;
  }
  else   { // Will invoke transition to error!
    invokeTransition("UnknownTransition");
    return;
  }
  declareSubState(EXEC_ACTION);
}
