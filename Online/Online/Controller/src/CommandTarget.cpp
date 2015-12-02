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
#include "FiniteStateMachine/FSMTypes.h"
#include "FiniteStateMachine/Transition.h"
#include "Controller/CommandTarget.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"
#include "dis.hxx"
#include "dic.hxx"

// C/C++ include files
#include <cstdio>
#include <memory>

using namespace std;
using namespace FiniteStateMachine;
using namespace FiniteStateMachine::DAQ;

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

/// Constructor
CommandTarget::CommandTarget(const std::string& nam) : DimCommand(nam.c_str(), (char*)"C"), TypedObject(0,nam)  { 
  string svcname = nam+"/status";
  m_monitor.targetState = m_monitor.state = NOT_READY;
  m_monitor.lastCmd     = m_monitor.doneCmd = (int)::time(0);
  m_monitor.metaState   = SUCCESS_ACTION;
  m_monitor.pid         = ::lib_rtl_pid();
  m_monitor.partitionID = -1;
  m_monitor.pad         = 0;
  m_service    = ::dis_add_service((char*)svcname.c_str(),(char*)"C",0,0,feed,(long)&m_stateName);
  svcname      = nam+"/fsm_status";
  m_fsmService = ::dis_add_service((char*)svcname.c_str(),(char*)"L:2;I:1;C",&m_monitor,sizeof(m_monitor),0,0);
}

/// Standard destructor
CommandTarget::~CommandTarget()   {
  if ( m_service != 0 ) ::dis_remove_service(m_service);
  if ( m_fsmService != 0 ) ::dis_remove_service(m_fsmService);
}

/// Declare process state to DIM service
FSM::ErrCond CommandTarget::declareState(const std::string& new_state, const std::string& opt)  {
  std::string old_state = m_stateName;
  m_prevStateName = m_stateName;
  ::dim_lock();
  m_stateName = new_state;
  ::dim_unlock();
  display(INFO,c_name(),"Declare state:%s  %s",m_stateName.c_str(),opt.c_str());
  ::dis_update_service(m_service);
  if ( new_state == ST_NAME_ERROR )
    declareSubState(FAILED_ACTION);
  else if ( old_state == new_state )
    declareSubState(FAILED_ACTION);
  else
    declareSubState(SUCCESS_ACTION);
  return FSM::SUCCESS;
}

/// Declare FSM sub-state
FSM::ErrCond CommandTarget::declareSubState(SubState new_state)  {
  m_monitor.metaState = char(new_state);
  switch(new_state)   {
  case SUCCESS_ACTION:
    m_monitor.doneCmd = (int)::time(0);
    break;
  case EXEC_ACTION:
    m_monitor.lastCmd = (int)::time(0);
    break;
  case FAILED_ACTION:
    m_monitor.doneCmd = (int)::time(0);
    break;
  case UNKNOWN_ACTION:
  default:
    m_monitor.doneCmd = (int)::time(0);
    m_monitor.metaState = UNKNOWN;
    break;
  }
  // std::cout << "Update substate..." << std::endl;
  ::dis_update_service(m_fsmService);
  return FSM::SUCCESS;
}

/// Publish state information when transition is completed
FSM::ErrCond CommandTarget::publish()  {
  declareState(m_stateName);
  return FSM::SUCCESS;
}

/// Run the object
CommandTarget& CommandTarget::run()  {
  DimServer::autoStartOn();
  DimServer::start(c_name());
  publish();
  IocSensor::instance().run();
  return *this;
}

/// Constructor
DAQCommandTarget::DAQCommandTarget(const std::string& nam)  
  : CommandTarget(nam), m_timeout(false)
{
  m_stateName = ST_NAME_OFFLINE;
}

/// Standard destructor
DAQCommandTarget::~DAQCommandTarget()
{
}

/// Set transition target state
void DAQCommandTarget::setTargetState(int target) {
  m_monitor.targetState = char(target); 
  IocSensor::instance().send(this,target,(void*)0);
}

/// DimCommand overload: handle DIM commands
void DAQCommandTarget::commandHandler()   {
  // Decouple as quickly as possible from the DIM command loop !
  std::string cmd = getString();
  display(NOLOG,c_name(),"Received transition request:%s",cmd.c_str());
  if ( m_timeout )  {
    display(WARNING,c_name(),"Ignore transition request:%s for TIMEOUT",cmd.c_str());
    m_timeout = false;
    return;
  }
  else if ( cmd == "timeout" )  {
    display(WARNING,c_name(),"Will TIMEOUT next request.");
    m_timeout = true;
    return;
  }
  else if ( cmd == "load"  )
    setTargetState(NOT_READY);
  else if ( cmd == "configure")
    setTargetState(READY);
  else if ( cmd == "start"    )
    setTargetState(RUNNING);
  else if ( cmd == "stop"     )
    setTargetState(STOPPED);
  else if ( cmd == "reset"    )
    setTargetState(NOT_READY);
  else if ( cmd == "pause"    )
    setTargetState(PAUSED);
  else if ( cmd == "continue" )
    setTargetState(RUNNING);
  else if ( cmd == "recover" )
    setTargetState(OFFLINE);
  else if ( cmd == "destroy" )
    setTargetState(OFFLINE);
  else if ( cmd == "error" )
    setTargetState(ERROR);
  else if ( cmd == "unload"   )
    setTargetState(UNKNOWN);
  else if ( cmd == "RESET"   )
    setTargetState(UNKNOWN);
  else   {
    setTargetState(ERROR);
    return;
  }
  declareSubState(EXEC_ACTION);
}

/// Interrupt handling routine
void DAQCommandTarget::handle(const CPP::Event& ev)   {
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type)  {
    case NOT_READY:
      declareState(ST_NAME_NOT_READY);
      break;
    case READY:
      declareState(ST_NAME_READY);
      break;
    case RUNNING:
      declareState(ST_NAME_RUNNING);
      break;
    case PAUSED:
      declareState(ST_NAME_PAUSED);
      break;
    case STOPPED:
      declareState(ST_NAME_READY);
      break;
    case ERROR:
      declareState(ST_NAME_ERROR);
      declareSubState(UNKNOWN_ACTION);
      break;
    case OFFLINE:
    case UNKNOWN:
      declareState(ST_NAME_OFFLINE);
      ::lib_rtl_sleep(500);
      exit(0);
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}
