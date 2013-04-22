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
#include "FiniteStateMachine/Controller.h"
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
 : CommandTarget(nam), m_machine(m)
{
  m->setFailAction(Callback::make(this,&Controller::publish));
  m->setCompletionAction(Callback::make(this,&Controller::publish));
  m->setInAction(m->type()->state(ST_NAME_OFFLINE),Callback::make(this,&Controller::invokeDeath));
  publish();
}

/// Standard destructor
Controller::~Controller() {
}

/// Entering stte OFFLINE. Send death signal to self.
FSM::ErrCond Controller::invokeDeath()  {
  IocSensor::instance().send(this,Slave::SLAVE_DEAD,this);
  return publish();
}

/// Publish state information when transition is completed
FSM::ErrCond Controller::publish()  {
  string state = m_machine->c_state();
  return declareState(state);
}

/// Interrupt handling routine
void Controller::handle(const Event& ev)    {
  Slave* slave = ev.iocPtr<Slave>();
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type)  {
    case Slave::SLAVE_DEAD:
      ::lib_rtl_sleep(500);
      ::exit(0);
      break;
    case Slave::SLAVE_TRANSITION:  // Forwarded from Machine
#if 0
      display(ALWAYS,"%s> Invoke transition from %s to %s --- %p",
	      c_name(),m_machine->c_state(),slave->c_state(),
	      m_machine->state()->findTrans(slave->c_state()));
#endif
      if ( m_machine->state() != slave->state() ) 
	m_machine->invokeTransition(slave->c_state(),Rule::SLAVE2MASTER);
      break;
    case Machine::MACH_EXEC_ACT:  // Forwarded from Machine
      m_machine->invokeTransition((const State*)ev.data);
      break;
    default:
      display(ALWAYS,"%s> ERROR: Invoke transition from %s",c_name(),m_machine->c_state());
      m_machine->invokeTransition((const State*)ev.data);
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
    setTargetState(ERROR);
    ret = m_machine->invokeTransition(ST_NAME_ERROR,Rule::SLAVE2MASTER);
    display(ALWAYS,"Unknown transition:%s Moving to ERROR!",transition.c_str());
  }
  return ret;
}

/// DimCommand overload: handle DIM commands
void Controller::commandHandler()   {
  // Decouple as quickly as possible from the DIM command loop !
  std::string cmd = getString();
  display(NOLOG,"%s> Received transition request:%s",c_name(),cmd.c_str());
  if ( !m_machine->isIdle() )  {
    display(ERROR,"Machine is not idle!");
    return;
  }
  else if ( cmd == "load"  )  {
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
  else if ( cmd == "unload"   )  {
    setTargetState(UNKNOWN);
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
  else   { // Will invoke transition to error!
    invokeTransition("UnknownTransition");
    return;
  }
  declareSubState(EXEC_ACTION);
}
