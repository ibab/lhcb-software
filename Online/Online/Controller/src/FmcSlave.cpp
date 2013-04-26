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
#include "FiniteStateMachine/FmcSlave.h"
#include "FiniteStateMachine/Type.h"
#include "FiniteStateMachine/State.h"
#include "FiniteStateMachine/Machine.h"
#include "FiniteStateMachine/TaskManager.h"
#include "CPP/IocSensor.h"
#include "RTL/rtl.h"
extern "C" {
#include "dic.h"
#include "dim.h"
}

// C/C++ include files
#include <boost/assign/std/vector.hpp>
#include <iostream>
#include <cstdio>
#include <cerrno>
#include <memory>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace FiniteStateMachine;
using namespace std;

typedef FSM::ErrCond ErrCond;

/// Class Constructor
FmcSlave::FmcSlave(const Type* typ, const string& nam, Machine* machine, bool internal) 
  : DimSlave(typ,nam,machine), m_internal(internal)
{
}

/// Standatrd destructor
FmcSlave::~FmcSlave() {
}

/// Start slave process
ErrCond FmcSlave::start()  {
  if ( isInternal() )  {
    send(SLAVE_ALIVE,0);
    return FSM::WAIT_ACTION;
  }
  TaskManager::instance(RTL::nodeNameShort()).start(name(),fmc_args,m_cmd,cmd_args);
  return FSM::SUCCESS;
}

/// Kill slave process
ErrCond FmcSlave::kill()  {
  stopTimer();
  if ( m_pid != 0 )  {
    int ret = ::dic_cmnd_service((char*)m_commandName.c_str(),(char*)"unload",7);
    display(ALWAYS,"%s::%s> %s command to slave. Curr State:%s",
	    RTL::processName().c_str(),c_name(),
	    ret != 1 ? "FAILED to send UNLOAD" : "Sent unload",metaStateName());	  
    startTimer(SLAVE_UNLOAD_TIMEOUT);
    return FSM::WAIT_ACTION;
  }
  // Slave is already dead. 
  return FSM::SUCCESS;
}

/// Send transition request to the slave
ErrCond FmcSlave::sendRequest(const Transition* tr)  {
  if ( tr )  {
    return isInternal() ? send(SLAVE_FINISHED,tr->to()) : DimSlave::sendRequest(tr);
  }
  return FSM::FAIL;
}

/// Handle timeout on unload transition according to timer ID
void FmcSlave::handleUnloadTimeout()  {
  TaskManager::instance(RTL::nodeNameShort()).stop(name(),SIGTERM,15);
}
