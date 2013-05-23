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
#include "FiniteStateMachine/Machine.h"
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

enum Timeouts {
  SLAVETIMEOUT = 1000
};

/// Class Constructor
DimSlave::DimSlave(const Type* typ, const string& nam, Machine* machine, bool internal) 
  : Slave(typ,nam,machine,internal), m_dimState(0,0), m_commandName(), m_killCmd("unload")
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
  std::string copy = "--";
  copy = args.c_str();
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
      return FSM::WAIT_ACTION;
    }
  }
  return FSM::FAIL;
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
