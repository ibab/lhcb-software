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
#include <sstream>

using namespace std;
using namespace FiniteStateMachine;
using namespace FiniteStateMachine::DAQ;

typedef Machine::Slaves Slaves;
#define ERROR_CONTROL (-9999)

/// Feed data to DIS when updating data
static void feed(void* tag, void** buff, int* size, int* /* first */) {
  static const char* data = "";
  string* s = *(string**)tag;
  if ( !s->empty() )  {
    *buff = (void*)s->c_str();
    *size = s->length()+1;
    return;
  }
  *buff = (void*)data;
  *size = 1;
}

static void error_call(int severity, int error_code, char* message) {
  static int enabled = 1;
  if ( severity == ERROR_CONTROL ) {
    enabled = error_code;
    return;
  }
  if ( enabled ) {
    switch(severity) { 
    case DIM_INFO:
      TypedObject::display(TypedObject::INFO,"DIM",message);
      break;
    case DIM_WARNING:
      TypedObject::display(TypedObject::WARNING,"DIM",message);
      break;
    case DIM_ERROR:
      TypedObject::display(TypedObject::ERROR,"DIM",message);
      break;
    case DIM_FATAL:
      TypedObject::display(TypedObject::FATAL,"DIM",message);
      break;
    default:
      TypedObject::display(TypedObject::FATAL,"DIM",message);
      break;
    }
  }
}

/// Constructor
Controller::Controller(const string&  nam, Machine* m)
  : CommandTarget(nam), m_errorState(0), m_machine(m)
{
  const Type* typ = m_machine->type();
  const Transition* start = typ->transition(ST_NAME_READY,ST_NAME_RUNNING);
  m_errorState = typ->state(ST_NAME_ERROR);
  m->setFailAction(Callback::make(this,&Controller::fail));
  m->setCompletionAction(Callback::make(this,&Controller::publish));
  m->setMetaStateAction(Callback::make(this,&Controller::publishSlaves));
  m->setPreAction(start,Callback::make(this,&Controller::start));
  m_fsmTasks = ::dis_add_service((char*)(nam+"/tasks").c_str(),(char*)"C",0,0,feed,(long)&m_taskInfo);
  error_call(ERROR_CONTROL,0,(char*)this);
  ::dis_add_error_handler(error_call);
  ::dic_add_error_handler(error_call);
}

/// Standard destructor
Controller::~Controller() {
  if ( m_fsmTasks != 0 ) ::dis_remove_service(m_fsmTasks);
}

/// Publish state information when transition failed. 
FSM::ErrCond Controller::fail()  {
  typedef map<Slave::SlaveState,vector<const Slave*> > SlaveMap;
  const Transition* tr = m_machine->currTrans();
  const Slaves      sl = m_machine->slaves();
  error_call(ERROR_CONTROL,1,(char*)"Enable DIM error printing");
  display(ALWAYS,c_name(),"%s> Controller: FAILED to invoke transition %s from %s.",
          m_machine->type()->c_name(), tr ? tr->c_name() : "??Unknown??",m_machine->c_state());
  m_machine->setSlaveState(Slave::SLAVE_FAILED,m_errorState);
  SlaveMap sl_map;
  stringstream msg;
  msg << "Summary:";
  for(Slaves::const_iterator i=sl.begin(); i!= sl.end(); ++i)  {
    const Slave* s = *i;
    display(DEBUG,c_name(),"Controller: Slave %s in state %s has meta-state:%s",
            s->c_name(), s->c_state(), s->metaStateName());
    sl_map[s->statusState()].push_back(s);
  }
  for(SlaveMap::const_iterator i=sl_map.begin(); i!=sl_map.end(); ++i)  {
    Slave::SlaveState slv = (*i).first;
    msg << (Slave::metaName(slv)+6) << ":" << (*i).second.size() << " ";
    if ( slv == Slave::SLAVE_FAILED || slv == Slave::SLAVE_TIMEOUT || slv ==  Slave::SLAVE_DEAD )  {
      for(SlaveMap::mapped_type::const_iterator j=(*i).second.begin(); j!=(*i).second.end(); ++j)  {
        string n = (*j)->name(), m = (*j)->name();
        if ( n.find('_') != string::npos ) { m = n.substr(n.find('_')+1); n = m; }
        if ( n.find('_') != string::npos ) { m = n.substr(n.find('_')+1); n = m; }
        msg << m << " ";
      }
    }
  }
  // Invoke special actions for certain transitions
  if ( tr ) {
    string m = msg.str().substr(0,100);
    m += "....";
    display(ERROR,tr->c_name(),"FAILED %s.",m.c_str());
    //display(ERROR,c_name(),"Controller: Special action on failed transition %s needed.",tr->c_name());
    if ( tr->name() == "recover" )  {
    }
  }
  return publishSlaves();
}

/// Publish state information of the slaves
FSM::ErrCond Controller::publishSlaves()  {
  stringstream info;
  const Slaves slaves = m_machine->slaves();
  const Transition* trans = m_machine->currTrans();
  if ( m_machine->isIdle() )  {
    error_call(ERROR_CONTROL,1,(char*)"Enable DIM error printing");
  }
  else if ( trans && trans->name() == "start" ) {
    error_call(ERROR_CONTROL,0,(char*)"Disable DIM error printing");
  }
  else if ( trans && trans->name() == "unload" ) {
    error_call(ERROR_CONTROL,0,(char*)"Disable DIM error printing");
  }
  else {
    error_call(ERROR_CONTROL,1,(char*)"Enable DIM error printing");
  }
  for(Slaves::const_iterator i=slaves.begin(); i!= slaves.end(); )  {
    Slave* s = *i;
    info << s->name() << "/" << s->c_state() << "/" << string(s->metaStateName());
    if ( ++i == slaves.end() ) break;
    info << "|";
  }
  if ( m_taskInfo != info.str() )  {
    ::dim_lock();
    m_taskInfo = info.str();
    ::dim_unlock();
    ::dis_update_service(m_fsmTasks);
  }
  return FSM::SUCCESS;
}

/// Publish state information when transition is completed
FSM::ErrCond Controller::publish()  {
  string state = m_machine->c_state();
  display(INFO,c_name(),"%s> Controller PUBLISH state %s",m_machine->c_name(),state.c_str());
  if ( state == "ERROR" )  {
    const Slaves slaves = m_machine->slaves();
    error_call(ERROR_CONTROL,1,(char*)"Enable DIM error printing");
    for(Slaves::const_iterator i=slaves.begin(); i!= slaves.end(); ++i)  {
      const Slave* s = *i;
      display(INFO,c_name(),"Controller: Slave %s in state %s has meta-state:%s",
              s->c_name(), s->c_state(), s->metaStateName());
    }
  }
  publishSlaves();
  const Transition* tr = m_machine->currTrans();
  display(INFO,c_name(),"Declare new state:          %s %s %s",state.c_str(),
          tr ? "after transition" : "", tr ? tr->c_name() : "");
  return declareState(state);
}

/// Transition pre-action for start: Reset all internal slaves to external ones
FSM::ErrCond Controller::start()  {
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
      display(ALWAYS,c_name(),"Controller: ERROR: Invoke transition from %s action:%d",
              m_machine->c_state(),ev.type);
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
    //display(ALWAYS,c_name(),"Unknown transition:%s from state %s. Moving to ERROR!",
    //    transition.c_str(),state->c_name());
  }
  return ret;
}

/// DimCommand overload: handle DIM commands
void Controller::commandHandler()   {
  // Decouple as quickly as possible from the DIM command loop !
  ErrCond status = FSM::SUCCESS;
  string cmd = getString();
  display(INFO,c_name(),"Received transition request:%s",cmd.c_str());
  if ( !m_machine->isIdle() )  {
    display(WARNING,c_name(),"Machine is not idle! Force idle state manually.");
    m_machine->goIdle();
  }
  if ( cmd == "load"  )  {
    setTargetState(NOT_READY);
    status = invokeTransition(cmd);
  }
  else if ( cmd == "configure")  {
    setTargetState(READY);
    status = invokeTransition(cmd);
  }
  else if ( cmd == "start"    )  {
    setTargetState(RUNNING);
    status = invokeTransition(cmd);
  }
  else if ( cmd == "stop"     )  {
    setTargetState(STOPPED);
    status = invokeTransition(cmd);
  }
  else if ( cmd == "reset"    )  {
    setTargetState(NOT_READY);
    status = invokeTransition(cmd);
  }
  else if ( cmd == "pause"    )  {
    setTargetState(PAUSED);
    status = invokeTransition(cmd);
  }
  else if ( cmd == "continue" )  {
    setTargetState(RUNNING);
    status = invokeTransition(cmd);
  }
  else if ( cmd == "recover" )  {
    setTargetState(OFFLINE);
    status = invokeTransition(cmd);
  }
  else if ( cmd == "RESET" )  {
    setTargetState(OFFLINE);
    status = invokeTransition(cmd);
  }
  else if ( cmd == "unload"   )  {
    setTargetState(OFFLINE);
    status = invokeTransition(cmd);
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
  if ( status == FSM::TRANNOTFOUND ) {
    m_machine->goIdle();
    if ( !m_machine->evaluateWhens() ) {
      publish();
    }
    return;
  }
  declareSubState(EXEC_ACTION);
}
