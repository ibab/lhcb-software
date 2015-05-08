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
#include "Controller/NativeDimSlave.h"
#include "FiniteStateMachine/Machine.h"

// C/C++ include files
#include <cerrno>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace FiniteStateMachine;
using namespace std;

typedef FSM::ErrCond ErrCond;

/// Class Constructor
NativeDimSlave::NativeDimSlave(const Type* typ, const string& nam, Machine* machine, bool internal) 
  : DimSlave(typ,nam,machine,internal), m_pid(0)
{
}

/// Standatrd destructor
NativeDimSlave::~NativeDimSlave() {
}

/// Start slave process
FSM::ErrCond NativeDimSlave::start()  {
  int ret = 0;
  vector<char*> argv, envp;
  string utgid = "UTGID="+name();

  for(size_t i=0; i<m_argv.size();++i)   {
    argv.push_back((char*)m_argv[i].c_str());
  }
  argv.push_back(0);
  for(size_t i=0; i<m_envp.size();++i) {
    if ( m_envp[i].substr(0,5)=="UTGID" ) continue;
    envp.push_back((char*)m_envp[i].c_str());
  }
  envp.push_back((char*)utgid.c_str());
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

/// Force slave killing
FSM::ErrCond NativeDimSlave::forceKill() {
  if ( m_pid ) {
    int status = 0;
    if ( 0 == ::kill(m_pid,SIGKILL) ) {
      pid_t pid = ::waitpid(-1,&status,0);
      if ( pid < 0 )    {
	display(ERROR,c_name(),"FAILED to kill slave. Curr State:%s",metaStateName());	  
      }
      m_pid = 0;
    }
    else if ( errno == ESRCH ) {    // Non-existing process
      m_pid = 0;
    }
    else if ( errno == EINVAL )  {  // Bad signal number ???!!!!
      return FSM::FAIL;             // Controller probably fucked
    }
    else if ( errno == EPERM )  {   // Cannot be my process
      m_pid = 0;
      return FSM::FAIL;
    }
  }
  return FSM::SUCCESS;
}

/// Handle timeout on unload transition according to timer ID
void NativeDimSlave::handleUnloadTimeout()  {
  int status = 0;
  if ( m_timerID.second == SLAVE_UNLOAD_TIMEOUT ) {
    display(ERROR,c_name(),"unload command unsuccessful. Send SIGTERM. State:%s",metaStateName());	  
    if ( 0 == ::kill(m_pid,SIGTERM) ) {
      pid_t pid = ::waitpid(-1,&status,WNOHANG);
      if ( pid < 0 ) startTimer(SLAVE_TERMINATE_TIMEOUT);
      else m_pid = 0;
    }
    else if ( errno == ESRCH ) {    // Non-existing process
      m_pid = 0;
    }
  }
  else if ( m_timerID.second == SLAVE_TERMINATE_TIMEOUT ) {
    display(ERROR,c_name(),"unload command unsuccessful. Send SIGKILL. State:%s",metaStateName());	  
    m_timerID = TimerID(0,0);
    forceKill();
  }
}

/// Handle state updates for a particular slave
void NativeDimSlave::handleState(const string& msg)  {
  int          st = currentState();
  bool   starting = st == SLAVE_LIMBO || st == SLAVE_STARTING;
  DimSlave::handleState(msg);
  if ( msg.empty() && !starting )  {
    int status = 0;
    pid_t pid = ::waitpid(m_pid,&status,WNOHANG);
    if ( pid > 0 )   {
      m_pid = 0;
    }
  }
}
