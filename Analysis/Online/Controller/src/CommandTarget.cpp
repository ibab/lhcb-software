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
#include "FiniteStateMachine/CommandTarget.h"
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

/// Constructor
CommandTarget::CommandTarget(const std::string& nam) : DimCommand(nam.c_str(), (char*)"C"), TypedObject(0,nam)  { 
  string svcname = nam+"/status";
  m_monitor.targetState = m_monitor.state = NOT_READY;
  m_monitor.lastCmd     = m_monitor.doneCmd = (int)::time(0);
  m_monitor.metaState   = SUCCESS_ACTION;
  m_monitor.pid         = ::lib_rtl_pid();
  m_monitor.partitionID = -1;
  m_monitor.pad         = 0;
  m_service    = new DimService(svcname.c_str(),(char*)m_stateName.c_str());
  svcname      = nam+"/fsm_status";
  m_fsmService = new DimService(svcname.c_str(),(char*)"L:2;I:1;C",&m_monitor,sizeof(m_monitor));
  DimServer::start(nam.c_str());
}

/// Standard destructor
CommandTarget::~CommandTarget()   {
}

/// Declare process state to DIM service
FSM::ErrCond CommandTarget::declareState(const std::string& new_state)  {
  std::string old_state = m_stateName;
  m_prevStateName = m_stateName;
  m_stateName = new_state;
  display(ALWAYS,"%s> Declare state:%s",c_name(),m_stateName.c_str());
  m_service->updateService((char*)m_stateName.c_str());
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
  m_fsmService->updateService(&m_monitor,sizeof(m_monitor));
  return FSM::SUCCESS;
}

/// Run the object
CommandTarget& CommandTarget::run()  {
  IocSensor::instance().run();
  return *this;
}

/// Constructor
DAQCommandTarget::DAQCommandTarget(const std::string& nam)  
  : CommandTarget(nam)
{
  declareState(ST_NAME_OFFLINE);
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
  display(NOLOG,"%s> Received transition request:%s",c_name(),cmd.c_str());
  if ( cmd == "load"  )
    setTargetState(NOT_READY);
  else if ( cmd == "configure")
    setTargetState(READY);
  else if ( cmd == "start"    )
    setTargetState(RUNNING);
  else if ( cmd == "stop"     )
    setTargetState(STOPPED);
  else if ( cmd == "reset"    )
    setTargetState(NOT_READY);
  else if ( cmd == "unload"   )
    setTargetState(UNKNOWN);
  else if ( cmd == "pause"    )
    setTargetState(PAUSED);
  else if ( cmd == "continue" )
    setTargetState(RUNNING);
  else if ( cmd == "recover" )
    setTargetState(OFFLINE);
  else if ( cmd == "RESET" )
    setTargetState(OFFLINE);
  else   {
    setTargetState(ERROR);
    return;
  }
  declareSubState(EXEC_ACTION);
}

/// Interrupt handling routine
void DAQCommandTarget::handle(const Event& ev)   {
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
