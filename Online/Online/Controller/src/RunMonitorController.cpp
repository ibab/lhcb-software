/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**      FOR HLT2 monitoring purposes
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/

// Framework include files
#include "Controller/RunMonitor.h"
#include "FiniteStateMachine/FSMTypes.h"
#include "CPP/TimeSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"

using namespace std;
using namespace FiniteStateMachine;
using namespace FiniteStateMachine::DAQ;

#define LINE_MARKS "==========================================================================================================="

/// Constructor
RunMonitorController::RunMonitorController(CPP::Interactor* master, Machine* mach, const RunMonitorConfig* config)
  : CPP::Interactor(), TypedObject(0,mach->name()), m_master(master), m_errorState(0),
    m_machine(mach), m_current(0), m_slave(0), m_config(config)
{
  const Type* typ = m_machine->type();
  m_errorState = typ->state(ST_NAME_ERROR);
  mach->setFailAction(Callback::make(this,&RunMonitorController::fail));
  mach->setCompletionAction(Callback::make(this,&RunMonitorController::publish));

  /// Upwards transitions
  mach->setCompletionAction(typ->transition(ST_NAME_OFFLINE,ST_NAME_NOT_READY), Callback::make(this,&RunMonitorController::configure));
  mach->setCompletionAction(typ->transition(ST_NAME_NOT_READY,ST_NAME_READY),   Callback::make(this,&RunMonitorController::start));
  mach->setCompletionAction(typ->transition(ST_NAME_READY,ST_NAME_RUNNING),     Callback::make(this,&RunMonitorController::publish));
  mach->setCompletionAction(typ->transition(ST_NAME_RUNNING,ST_NAME_PAUSED),    Callback::make(this,&RunMonitorController::paused));

  /// Downwards transitions:
  mach->setCompletionAction(typ->transition(ST_NAME_PAUSED,ST_NAME_READY),      Callback::make(this,&RunMonitorController::reset));
  mach->setCompletionAction(typ->transition(ST_NAME_READY,ST_NAME_NOT_READY),   Callback::make(this,&RunMonitorController::unload));
  mach->setInAction(typ->state(ST_NAME_ERROR),   Callback::make(this,&RunMonitorController::daq_err));
  mach->setInAction(typ->state(ST_NAME_OFFLINE), Callback::make(this,&RunMonitorController::done));
}

/// Standard destructor
RunMonitorController::~RunMonitorController() {
  if ( m_machine ) delete m_machine;
  m_machine = 0;
}

/// Prepare FSM machine and slave task to work on a new work item
void RunMonitorController::prepareTask(RunMonitorItem* w)   {
  m_work = w;
  if ( m_work )   {
    string nam = m_work->utgid(m_config->name);
    m_machine->setName(nam);
    printf("Machine:%s\n",m_machine->c_name());
    m_name = nam;
  }
  else  {
    char nam[1024];
    ::snprintf(nam,sizeof(nam),"%s_%s_IDLE",
	       RTL::nodeNameShort().c_str(),
	       m_config->name.c_str());
    m_machine->setName(nam);
    m_name = nam;
  }
  if ( m_slave )  {
    m_machine->removeSlave(m_slave);
    delete m_slave;
    m_slave = 0;
  }
  if ( m_work )  {
    m_slave = (*m_config->createSlave)(this);
    m_machine->addSlave(m_slave);
  }
}

/// Move FSM machine to the requested state by name
FSM::ErrCond RunMonitorController::go_to_state(const string& state)  {
  IocSensor::instance().send(this,CMD_EXECUTE,(void*)m_machine->type()->state(state));
  return FSM::SUCCESS; 
}

/// Invoke a transition from the current state (if exists) by name
FSM::ErrCond RunMonitorController::invoke_transition(const string& transition)  {
  IocSensor::instance().send(this,CMD_TRANSITION,(void*)new string(transition));
  return FSM::SUCCESS; 
}

/// FSM machine callback on the transition to ERROR
FSM::ErrCond RunMonitorController::daq_err()     {
  display(ALWAYS,c_name(),"%s> DAQ_ERROR. state %s",m_machine->c_name(),m_machine->c_state());
  if ( m_work ) m_work->properties["FSM-Slave:DAQ_ERROR"] = fsm_slave_state_string(machine(),true);
  IocSensor::instance().send(m_master,CMD_FAILED,this);
  return FSM::SUCCESS;
}

/// FSM callback on enter the state OFFLINE ie. when the slave finished work or got killed.
FSM::ErrCond RunMonitorController::done()    {
  Machine* m = m_machine;
  string   mstate = m->state()->name();
  const Transition* tr = m->currTrans();
  display(ALWAYS,c_name(),"%s> DONE. state %s",m->c_name(),m->c_state());
  display(ALWAYS,c_name(),"%s> DONE %s",m->c_name(),fsm_slave_state_string(machine()).c_str());
  display(ALWAYS,c_name(),"%s> DONE Tr: %s",m->c_name(),tr ? tr->c_name() : "UNKNOWN");
  if ( m_work ) m_work->properties["FSM-Slave:DONE"] = fsm_slave_state_string(machine(),true);
  if ( mstate == "RUNNING" || mstate == "PAUSED" || mstate == "READY" )   {
    IocSensor::instance().send(m_master,CMD_DONE,this);
    return daq_err();
  }
  IocSensor::instance().send(m_master,CMD_FINISHED,this);
  return FSM::SUCCESS;
}

/// FSM machine callback on the transition 'configure' from NOT_READY to READY
FSM::ErrCond RunMonitorController::configure() {
  if ( m_work ) m_work->properties["FSM-Slave:CONFIGURE"] = fsm_slave_state_string(machine(),true);
  return invoke_transition("configure");
}

/// FSM machine callback on the transition 'start' from READY to RUNNING
FSM::ErrCond RunMonitorController::start()     {
  if ( m_work ) m_work->properties["FSM-Slave:READY"] = fsm_slave_state_string(machine(),true);
  return invoke_transition("start");
}

/// FSM machine callback on the transition 'stop' from RUNNING/PAUSE to READY
FSM::ErrCond RunMonitorController::stop()      {
  if ( m_work ) m_work->properties["FSM-Slave:RUN-ENDED"] = fsm_slave_state_string(machine(),true);
  return invoke_transition("stop");
}

/// FSM machine callback on reach the NOT_READY/OFFLINE state
FSM::ErrCond RunMonitorController::reset()     {
  if ( m_work ) m_work->properties["FSM-Slave:RESET"] = fsm_slave_state_string(machine(),true);
  return invoke_transition("reset");
}

/// Callback to load slave task and 'start' FSM round-trip
FSM::ErrCond RunMonitorController::load()      {
  if ( m_work ) m_work->properties["FSM-Slave:OFFLINE"] = fsm_slave_state_string(machine(),true);
  return invoke_transition("load");
}

/// FSM machine callback on the transition 'unload' from NOT_READY to OFFLINE
FSM::ErrCond RunMonitorController::unload()    {
  if ( m_work ) m_work->properties["FSM-Slave:UNLOAD"] = fsm_slave_state_string(machine(),true);
  return invoke_transition("unload");
}

/// FSM machine callback on entering the state 'PAUSED'
FSM::ErrCond RunMonitorController::paused()    {
  if ( m_work ) m_work->properties["FSM-Slave:PAUSED"] = fsm_slave_state_string(machine(),true);
  return invoke_transition("stop");
}

/// Generic FAILURE callback of the FSM machine
FSM::ErrCond RunMonitorController::fail()    {
  const Machine::Slaves& slaves = machine()->slaves();
  display(ALWAYS,c_name(),"++++ Controller %s FAILED....................",machine()->c_name());
  publish();
  printSlaves();
  for(Machine::Slaves::const_iterator i=slaves.begin(); i!= slaves.end(); ++i)  {
    if ( fsm_slave_anomaly(*i) )  {
      IocSensor::instance().send(m_master,CMD_FAILED,this);
      if ( m_work ) m_work->properties["FSM-Slave:FAILURE"] = fsm_slave_state_string(machine(),true);
      break;
    }
  }
  display(ALWAYS,c_name(),"++++ %s",LINE_MARKS);
  return FSM::SUCCESS;
}

/// Print slaves, which are in a annomal status
void RunMonitorController::printSlaves(bool annomal_only)  const  {
  const Machine::Slaves& slaves = machine()->slaves();
  for(Machine::Slaves::const_iterator i=slaves.begin(); i!= slaves.end(); ++i)  {
    const Slave* s = *i;
    if ( !annomal_only || fsm_slave_anomaly(s) )  {
      display(ALWAYS,c_name(),"Controlled slave %s in state %s/%s - %s Alive:%-3s Limbo:%-3s Answered:%-3s ",
	      s->c_name(), s->c_state(), s->metaStateName(), s->statusName(),
	      yes_no(s->isAlive()), yes_no(s->isLimbo()), yes_no(s->answered()));
    }
  }
}

/// Print outgoing transitions for the machine in the requested state
void RunMonitorController::printOutgoingTransitions(const State* st)   const  {
  if ( st )  {
    for(State::Transitions::const_iterator i=st->outgoing().begin(); i!=st->outgoing().end(); ++i)    {
      const Transition* tr = *i;
      display(ALWAYS,c_name(),"Controller: Machine State:%s transitionByName %s -> %s",
	      machine()->c_state(), tr->c_name(), tr->to()->c_name());
      
    }
  }
}

/// Print outgoing transitions for the machine in the current state
void RunMonitorController::printOutgoingTransitions()   const  {
  printOutgoingTransitions(machine()->state());
}

/// Interrupt handling routine
void RunMonitorController::handle(const CPP::Event& event) {
  switch(event.eventtype) {
  case TimeEvent:
    break;
  case IocEvent:
    switch(event.type)   {
    case CMD_LOAD:
      load();
      break;

    case CMD_FAILED:
      IocSensor::instance().send(this, CMD_TRANSITION, event.iocPtr<void>());
      break;

    case CMD_TRANSITION:   {
      try {
	string* transition = event.iocPtr<string>();
	ErrCond ret = machine()->invokeTransitionByName(*transition);
	display(ALWAYS,c_name(),"Controller: State:%s %s invoke transitionByName %s",
		m_machine->c_state(), ret == FSM::SUCCESS ? "" : "FAILED to", transition->c_str());
	return;
      }
      catch(const exception& e)  {
	display(ALWAYS,c_name(),"Controller: Exception during invokation of transition: %s",e.what());
      }
      break;
    }

    case CMD_EXECUTE:   {
      const State* s = event.iocPtr<State>();
      display(ALWAYS,c_name(),"Controller: Invoke transition from %s to state:%s",
	      m_machine->c_state(),s ? s->c_name() : "Bad-State");
      m_machine->invokeTransition(s);
      break;
    }

    case Slave::SLAVE_TRANSITION:
      go_to_state(ST_NAME_ERROR);
      break;

    case Slave::SLAVE_FAILED:
      go_to_state(ST_NAME_ERROR);
      break;

    default:
      break;
    }
    break;
  default:
    break;
  }
}

/// Publish state information when transition is completed
FSM::ErrCond RunMonitorController::publish()  {
  string state = m_machine->c_state();
  display(INFO,c_name(),"%s> Controller PUBLISH state %s",m_machine->c_name(),state.c_str());
  return FSM::SUCCESS;
}
