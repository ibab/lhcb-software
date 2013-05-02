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
#include "Controller/DimSlave.h"
#include "FiniteStateMachine/Type.h"
#include "RTL/rtl.h"
extern "C" {
#include "dic.h"
#include "dim.h"
}

// C/C++ include files
#include <boost/assign/std/vector.hpp>
#include <cstring>

using namespace boost::assign;
using namespace FiniteStateMachine;
using namespace std;

/// Class Constructor
DimSlave::DimSlave(const Type* typ, const string& nam, Machine* machine, bool internal) 
  : Slave(typ,nam,machine,internal), m_dimState(0,0), m_timerID(0,0), m_commandName(), m_killCmd("unload")
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

/// Set the process arguments from single string
DimSlave& DimSlave::setArgs(const string& args)  {
  m_argv.clear();
  return addArgs(args);
}

/// Set the process arguments from single string
DimSlave& DimSlave::addArgs(const string& args)  {
  std::string copy = args;
  for( char* p = (char*)copy.c_str(), *savePtr=0;; p=0)  {
    char* token = ::strtok_r(p," ",&savePtr);
    if ( !token ) break;
    m_argv += token;
  }
  return *this;
}

/// Kill slave process
FSM::ErrCond DimSlave::kill()  {
  stopTimer();
  int ret = ::dic_cmnd_service((char*)m_commandName.c_str(),(char*)m_killCmd.c_str(),m_killCmd.length()+1);
  display(ALWAYS,"%s::%s> %s %s command to slave. Curr State:%s",
	  RTL::processName().c_str(),c_name(),
	  ret != 1 ? "FAILED to send" : "Sent",m_killCmd.c_str(),metaStateName());	  
  startTimer(SLAVE_UNLOAD_TIMEOUT);
  return FSM::WAIT_ACTION;
}

/// Send transition request to the slave
FSM::ErrCond DimSlave::sendRequest(const Transition* tr)  {
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

/// Handle timeout according to timer ID
void DimSlave::handleTimeout()  {
  if ( SLAVE_EXECUTING == currentState() )  {
    if ( m_timerID.second == SLAVE_TRANSITION_TIMEOUT )  {
      send(SLAVE_TRANSITION_TIMEOUT);
    }
    else {
      handleUnloadTimeout();
    }
  }
}

/// Handle timeout on unload transition according to timer ID
void DimSlave::handleUnloadTimeout()  {
  if ( m_timerID.second == SLAVE_UNLOAD_TIMEOUT )   {
    display(ERROR,"%s> unload command unsuccessful. FAILURE - [%s]. State:%s",
	    c_name(),"Insufficient Implementation",metaStateName());	  
  }
}

/// Handle state updates for a particular slave
void DimSlave::handleState(const string& msg)  {
  int          st = currentState();
  bool   starting = st == SLAVE_LIMBO || st == SLAVE_STARTING;
  stopTimer();
  if ( msg.empty() )    {  // No-link ?
    if ( !starting )  {
      display(NOLOG,"%s::%s> Slave DEAD. Curr State:%s",
	      RTL::processName().c_str(),c_name(),metaStateName());
      //send(SLAVE_LIMBO,m_state);
      iamDead();
    }
    return;
  }

  const State* state = type()->state(msg);
  const Transition* transition = state ? m_state->findTrans(state) : 0;
  if ( starting )   {
    //send(SLAVE_ALIVE,type()->initialState());
    iamHere();
    return;
  }
  else if ( transition )  {
    //send(SLAVE_FINISHED,state);
    transitionDone(state);
    return;
  }
  else if ( state )  {
    //send(SLAVE_TRANSITION,state);
    transitionSlave(state);
    return;
  }
  else  {
    //send(SLAVE_FAILED,state);
    transitionFailed();
    return;
  }
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

/// DTQ overload to process timeout(s)
void DimSlave::tmoHandler(void* tag)  {
  if ( tag ) {
    DimSlave* slave = (DimSlave*)tag;
    slave->handleTimeout();
  }
}
