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
#include "Controller/TaskManager.h"
#include "RTL/strdef.h"
#include "RTL/rtl.h"
extern "C" {
#include "dic.h"
#include "dim.h"
}

// C/C++ include files
#include <cstdarg>
#include <map>

using namespace FiniteStateMachine;
using namespace std;

namespace {
  int _execDim(const string& datapoint, const char* fmt, ...)   {
    va_list args;
    va_start( args, fmt);
    char str[4096];
    size_t len = ::vsnprintf(str,sizeof(str),fmt,args);
    va_end (args);
    str[len]=0;
    int ret = ::dic_cmnd_service((char*)datapoint.c_str(),str,len+1);
    if ( ret == 1 )  {
      return 1;
    }
    return 0;
  }
}

/// Standard constructor
TaskManager::TaskManager(const string& node)   {
  string n = RTL::str_upper(node);
  m_node     = n;
  m_start = "/FMC/"+n+"/task_manager/start";
  m_stop  = "/FMC/"+n+"/task_manager/stop";
  m_kill  = "/FMC/"+n+"/task_manager/kill";
}

/// Standard destructor
TaskManager::~TaskManager()  {
}

/// Copy constructor
TaskManager::TaskManager(const TaskManager& c) 
  : m_node(c.m_node), m_start(c.m_start), m_stop(c.m_stop), m_kill(c.m_kill)
{
}

/// Assignment operator
TaskManager& TaskManager::operator=(const TaskManager& c)  {
  if ( &c != this ) {
    m_node = c.m_node;
    m_start = c.m_start;
    m_stop = c.m_stop;
    m_kill = c.m_kill;
  }
  return *this;
}

/// Instance accessor
TaskManager& TaskManager::instance(const string& node)  {
  static map<string,TaskManager*> m;
  map<string,TaskManager*>::iterator i=m.find(node);
  if ( i == m.end() ) {
    i = m.insert(make_pair(node,new TaskManager(node))).first;
  }
  return *((*i).second);
}

/// Start a process
int TaskManager::start(const std::string& utgid, const string& fmc_args, const string& cmd, const string& args)  const {
  return _execDim(m_start,"-u %s %s %s %s",utgid.c_str(),fmc_args.c_str(),cmd.c_str(),args.c_str());
}
/// Kill a process
int TaskManager::stop(const std::string& utgid, int sig_num, int wait_before_kill)  const {
  return _execDim(m_stop,"-s %d -d %d %s",sig_num,wait_before_kill,utgid.c_str());
}

/// Kill a process
int TaskManager::kill(const std::string& utgid, int sig_num)  const {
  return _execDim(m_stop,"-s %d %s",sig_num,utgid.c_str());
}
