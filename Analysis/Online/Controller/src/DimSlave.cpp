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
#include "FiniteStateMachine/DimSlave.h"
#include "FiniteStateMachine/Type.h"
#include "FiniteStateMachine/State.h"
#include "FiniteStateMachine/Machine.h"
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
DimSlave::DimSlave(const Type* typ, const string& nam, Machine* machine) 
  : Slave(typ,nam,machine), m_dimState(0,0), m_pid(0), m_timerID(0,0), m_commandName()
{
  m_commandName = nam;
  m_tmo = 3;
  display(ALWAYS,"%s::%s> State data point: %s",RTL::processName().c_str(),c_name(),(nam+"/status").c_str());
  m_dimState.first  = ::dic_info_service((char*)(nam+"/status").c_str(),MONITORED,0,0,0,stateHandler,(long)this,0,0);
  m_dimState.second = ::dic_info_service((char*)(nam+"/fsm_status").c_str(),MONITORED,0,0,0,infoHandler,(long)this,0,0);
}

/// Standatrd destructor
DimSlave::~DimSlave() {
  if ( m_dimState.first )  ::dic_release_service(m_dimState.first);
  if ( m_dimState.second ) ::dic_release_service(m_dimState.second);
  m_dimState = make_pair(0,0);
  m_argv.clear();
  m_envp.clear();
}

/// Clone the environment of the existing process
DimSlave& DimSlave::cloneEnv()  {
  m_envp.clear();
  for(char** e=environ; *e; ++e) m_envp.push_back(*e);
  return *this;
}

/// Start slave process
ErrCond DimSlave::start()  {
  int ret = 0;
  vector<char*> argv, envp;
  for(size_t i=0; i<m_argv.size();++i) argv.push_back((char*)m_argv[i].c_str());
  for(size_t i=0; i<m_envp.size();++i) envp.push_back((char*)m_envp[i].c_str());
  argv.push_back(0);
  envp.push_back(0);
  if ( m_pid == 0 )  {
    switch((m_pid = ::fork()))  {
    case -1:  // Error
      return FSM::FAIL;
    case 0:   // Child
      ret = ::execve(m_cmd.c_str(),&argv[0],&envp[0]);
      if ( ret != 0 )  {
      }
      return FSM::SUCCESS;
    default:  // Parent
      return FSM::WAIT_ACTION;
    }
  }
  return FSM::SUCCESS;
}

/// Kill slave process
ErrCond DimSlave::kill()  {
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
ErrCond DimSlave::sendRequest(const Transition* tr)  {
  if ( tr )  {
    const char* data = tr->name().c_str();
    int len = tr->name().length();
    display(NOLOG,"%s::%s> Sending request %s to %s [%s]",
	    RTL::processName().c_str(),c_name(),data,m_commandName.c_str(),c_state());
    int ret = ::dic_cmnd_service((char*)m_commandName.c_str(),(char*)data,len+1);
    if ( ret == 1 )  {
      startTimer(SLAVE_TRANSITION_TIMEOUT,tr);
      return FSM::WAIT_ACTION;
    }
  }
  return FSM::FAIL;
}

/// Add entry in transition timeout table (timeout in seconds)
DimSlave& DimSlave::addTimeout(const Transition* param, int value)  {
  m_timeouts[param] = value;
  return *this;
}

/// Remove entry in transition timeout table
DimSlave& DimSlave::removeTimeout(const Transition* param)  {
  TimeoutTable::iterator i=m_timeouts.find(param);
  if ( i != m_timeouts.end() ) m_timeouts.erase(i);
  return *this;
}

/// Start the slave's transition timeout
DimSlave& DimSlave::startTimer(int reason, const void* param)   {
  TimeoutTable::const_iterator i=m_timeouts.find(param);
  int tmo = i==m_timeouts.end() ? m_tmo : (*i).second;

  if ( m_timerID.first ) ::dtq_stop_timer(m_timerID.first);
  m_timerID = TimerID(this,reason);
  ::dtq_start_timer(tmo, tmoHandler, m_timerID.first);
  return *this;
}

/// Stop the slave's transition timeout
DimSlave& DimSlave::stopTimer()  {
  if ( m_timerID.first ) ::dtq_stop_timer(m_timerID.first);
  m_timerID = TimerID(0,0);
  return *this;
}

/// DTQ overload to process timeout(s)
void DimSlave::tmoHandler(void* tag)  {
  if ( tag ) {
    DimSlave* s = *(DimSlave**)tag;
    s->handleTimeout();
  }
}

/// Handle timeout according to timer ID
void DimSlave::handleTimeout()  {
  int status;
  if ( SLAVE_EXECUTING == currentState() )  {
    if ( m_timerID.second == SLAVE_TRANSITION_TIMEOUT )  {
      send(SLAVE_TRANSITION_TIMEOUT);
    }
    else if ( m_timerID.second == SLAVE_UNLOAD_TIMEOUT ) {
      display(ERROR,"%s> unload command unsuccessful. Send SIGTERM. State:%s",c_name(),metaStateName());	  
      ::kill(m_pid,SIGTERM);
      pid_t pid = ::waitpid(-1,&status,WNOHANG);
      if ( pid < 0 ) startTimer(SLAVE_TERMINATE_TIMEOUT);
      else m_pid = 0;
    }
    else if ( m_timerID.second == SLAVE_TERMINATE_TIMEOUT ) {
      display(ERROR,"%s> unload command unsuccessful. Send SIGKILL. State:%s",c_name(),metaStateName());	  
      m_timerID = TimerID(0,0);
      ::kill(m_pid,SIGKILL);
      pid_t pid = ::waitpid(-1,&status,0);
      if ( pid < 0 )    {
	display(ERROR,"%s> FAILED to kill slave. Curr State:%s",c_name(),metaStateName());	  
      }
      m_pid = 0;
    }
  }
}

/// Handle state updates for a particular slave
void DimSlave::handleState(const string& msg)  {
  int          st = currentState();
  bool   starting = st == SLAVE_LIMBO || st == SLAVE_STARTING;
  stopTimer();
  if ( msg.empty() )    {  // No-link ?
    if ( !starting )  {
      int status = 0;
      display(NOLOG,"%s::%s> Slave DEAD. Curr State:%s",
	      RTL::processName().c_str(),c_name(),metaStateName());
      pid_t pid = ::waitpid(m_pid,&status,WNOHANG);
      if ( pid > 0 )   {
	send(SLAVE_LIMBO,m_state);
	m_pid = 0;
      }
      else  {
	display(NOLOG,"%s::%s> Looks like fake slave DEATH. Curr State:%s",
		RTL::processName().c_str(),c_name(),metaStateName());
      }
    }
    return;
  }

  const State* state = type()->state(msg);
  const Transition* transition = state ? m_state->findTrans(state) : 0;
  if ( starting )
    send(SLAVE_ALIVE,type()->initialState());
  else if ( transition )
    send(SLAVE_FINISHED,state);
  else if ( state )
    send(SLAVE_TRANSITION,state);
  else
    send(SLAVE_FAILED,state);

  display(ALWAYS,"%s::%s> Slave state:%s Meta-state:%s",
	  RTL::processName().c_str(),c_name(),msg.c_str(),metaStateName());
}

/// DimInfo overload to process messages
void DimSlave::infoHandler(void* tag, void* address, int* size) {
  if ( tag && size ) {
    DimSlave* s = *(DimSlave**)tag;
    int len = *size;
    if ( address && len > 0 )   {
      ::memcpy(&s->m_monitor,address,sizeof(FSMMonitoring));
    }
  }
}

/// DimInfo overload to process messages
void DimSlave::stateHandler(void* tag, void* address, int* size) {
  if ( tag ) {
    int         len = size ? *size : 0;
    DimSlave*     s = *(DimSlave**)tag;
    const char* msg = (const char*)address;
    s->handleState(len > 0 ? string(msg) : string(""));
  }
}
