#include "CPP/Interactor.h"
// C/C++ include files
#include <vector>
#include <map>

class ControllerDump : public Interactor {
public:
  class DimStatus {
  public:
    ControllerDump* actor;
    int             cmd, id;
    DimStatus(ControllerDump* a, int c, int i=0) 
      : actor(a), cmd(c), id(i) {}
    DimStatus() {}
  };
  class DimSlaveData {
  public:
    DimStatus slv;
    DimStatus ctrl;
    std::string slvData, ctrlData;
    DimSlaveData() {}
    ~DimSlaveData() {}
  };
protected:
  std::string m_name;
  std::string m_dns;
  std::string m_state;
  std::string              m_tasks;
  DimStatus                m_stat;
  DimStatus                m_task;
  typedef std::map<std::string,DimSlaveData*> Slaves;
  Slaves m_slaves;
  int m_interval;

public:
  /// Initializing constructor
  ControllerDump(const std::string& dns, const std::string& nam);
  /// Default destructor
  virtual ~ControllerDump();
  /// Start the dump
  void start(int interval = -1);
  /// Dump obtained information
  void show();
  /// Add new slave
  void addSlave(const std::string& slv);
  /// Remove new slave
  void removeSlave(const std::string& slv);

  /// Event handler routine
  virtual void handle(const CPP::Event& event);
  /// Interrupt event handler routine
  int handleIoc(const CPP::Event& event);
  /// Timer event handler routine
  void handleTimer(const CPP::Event& event);
};

// Framework include files
#include "CPP/Event.h"
#include "CPP/IocSensor.h"
#include "CPP/TimeSensor.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <memory>
#include <algorithm>
#include "dic.hxx"

using namespace CPP;
using namespace std;

/// DimInfo overload to process messages
static void stringHandler(void* tag, void* address, int* size) {
  if ( tag && size )  {
    string* s = *(string**)tag;
    if ( *size > 0 )   {
      *s = (const char*)address;
      return;
    }
    *s = "";
  }
}
enum {
  CMD_UPDATE_STATE = 1,
  CMD_UPDATE_TASKS = 2,
  CMD_SLAVE = 3,
  CMD_CTRLSLAVE = 4,
  CMD_UPDATE = 9999
};

namespace {
  template <typename T> int update(ControllerDump* , const T&)  {
    //IocSensor::instance().send(ptr,CMD_UPDATE,ptr);
    return 1;
  }
}

/// Initializing constructor
ControllerDump::ControllerDump(const std::string& dns, const std::string& nam)
  : m_name(nam), m_dns(dns), 
    m_stat(this,CMD_UPDATE_STATE), 
    m_task(this,CMD_UPDATE_TASKS)
{
  ::dic_set_dns_node(m_dns.c_str());
  ::lib_rtl_output(LIB_RTL_ALWAYS,"+++ DNS: %s",m_dns.c_str());
  ::lib_rtl_output(LIB_RTL_ALWAYS,"+++ Controller: %s",m_name.c_str());
  
  m_stat.id = ::dic_info_service((m_name+"/status").c_str(),MONITORED,0,0,0,stringHandler,(long)&m_state,0,0);
  m_task.id = ::dic_info_service((m_name+"/tasks").c_str(), MONITORED,0,0,0,stringHandler,(long)&m_tasks,0,0);
}

/// Default destructor
ControllerDump::~ControllerDump()   {
  if ( m_stat.id ) ::dic_release_service(m_stat.id);
  if ( m_task.id ) ::dic_release_service(m_task.id);
}

/// Start the dump
void ControllerDump::start(int interval)  {
  m_interval = interval;
  TimeSensor::instance().add(this,5);
}

/// Event handler routine
void ControllerDump::handle(const Event& event)   {
  switch(event.eventtype) {
  case TimeEvent:
    handleTimer(event);
    break;
  case IocEvent:
    handleIoc(event);
    break;
  default:
    break;
  }
}

/// Interrupt event handler routine
int ControllerDump::handleIoc(const Event& event)   {
  switch(event.type)  {
  case CMD_UPDATE_STATE:
    break;
  case CMD_UPDATE_TASKS:
    break;
  case CMD_SLAVE:
    break;
  case CMD_CTRLSLAVE:
    break;
  default:
    break;
  }
  return 1;
}

/// Timer event handler routine
void ControllerDump::handleTimer(const Event& event)   {
  if (event.timer_id)  {
    dim_lock();
    show();
    dim_unlock();
    TimeSensor::instance().add(this,m_interval);
  }
}

/// Add new slave
void ControllerDump::addSlave(const string& slv)  {
  if ( slv.find("/") != string::npos )  {
    string sn = slv.substr(0,slv.find("/"));
    DimSlaveData* sd = new DimSlaveData;
    m_slaves[slv] = sd;
    sd->slv.cmd = CMD_SLAVE;
    sd->slv.actor = this;
    sd->slv.id = ::dic_info_service((sn+"/status").c_str(),
				    MONITORED,0,0,0,stringHandler,(long)&sd->slvData,0,0);
    sd->ctrl.cmd = CMD_CTRLSLAVE;
    sd->ctrl.actor = this;
    sd->ctrl.id = ::dic_info_service((m_name+"/Slave/"+sn+"/CtrlState").c_str(),
				     MONITORED,0,0,0,stringHandler,(long)&sd->ctrlData,0,0);
  }
}

/// Remove new slave
void ControllerDump::removeSlave(const string& slv)  {
  Slaves::iterator is=m_slaves.find(slv);
  if ( is != m_slaves.end() )  {
    auto_ptr<DimSlaveData> sd((*is).second);
    if ( sd->slv.id ) ::dic_release_service(sd->slv.id);
    if ( sd->ctrl.id ) ::dic_release_service(sd->ctrl.id);
    m_slaves.erase(is);
    ::lib_rtl_output(LIB_RTL_ALWAYS,"|++       Removed Monitor: %s",slv.c_str());
  }
}


#include <iostream>
vector<string> strsplit(const string& s, char p)  {
  vector<string> buf;
  size_t idx = 0, idq=0;
  for(;(idx=s.find(p,idq)) != string::npos; idq=++idx)
    buf.push_back(s.substr(idq,idx-idq));
  if ( s.length() > idq ) buf.push_back(s.substr(idq));
  return buf;//std::move(buf);
}

/// Dump obtained information
void ControllerDump::show()    {
  vector<string> tasks = strsplit(m_tasks,'|');
  Slaves current;
  ::lib_rtl_output(LIB_RTL_ALWAYS,"|++ DNS: %s",m_dns.c_str());
  ::lib_rtl_output(LIB_RTL_ALWAYS,"|++ Controller: %s  [%s]   %ld children",
		   m_name.c_str(),m_state.c_str(),tasks.size());
  for(size_t i=0; i<tasks.size();++i)   {
    const string& slv = tasks[i];
    Slaves::const_iterator is=m_slaves.find(slv);
    if ( is != m_slaves.end() )  {
      ::lib_rtl_output(LIB_RTL_ALWAYS,"|++>> %3ld : %s  [%s]",i,slv.c_str(),
		       (*is).second->slvData.c_str());
      ::lib_rtl_output(LIB_RTL_ALWAYS,"|++         %s",(*is).second->ctrlData.c_str());
      continue;
    }
    ::lib_rtl_output(LIB_RTL_ALWAYS,"|++   %3ld : %s  [Apparently unknown]",i,slv.c_str());
    addSlave(slv);
  }
  for(Slaves::iterator is=m_slaves.begin(); is != m_slaves.end(); ++is)  {
    if ( find(tasks.begin(), tasks.end(), (*is).first) == tasks.end() ) {
      removeSlave((*is).first);
      is = m_slaves.begin();
    }
  }
  const char* time_str = ::lib_rtl_timestr();
  ::lib_rtl_output(LIB_RTL_ALWAYS,"+- %s -----------------------------------------------------+",time_str);
}


/*============================================================
**
**      Ctrl process management on a HLT worker node
**
**  AUTHORS:
**
**      M.Frank
**
**==========================================================*/
// Framework include files
#include "RTL/rtl.h"
#include "RTL/strdef.h"

// C/C++ include files

using namespace std;

static void help() {
  ::fprintf(stdout,
	    " fsm_ctrl -arg=<value> [ -arg=<value> ... ]                                 \n"
	    "          -help                     Print this help message.                \n"
	    "          -print=[number]           Print level (0 for debug).              \n"
	    "          -fsm=<type>               FSM protocol type (default:DAQ)         \n"
	    "          -partition=[string]       Partition name.                         \n"
	    "          -runinfo=[string]         Path to file with python formatted      \n"
	    "                                    information from RunInfo datapoint.     \n"
	    "          -mode=[string]            Run configuration.                      \n"
	    "                                    One of [NORMAL,FORKING,CHECKPOINTING].  \n"
	    "          -taskconfig=[string]      Path to xml file with slave information.\n"
	    "          -count=[number]           Number of Moore processes to be forked. \n"
	    "          -sleep=[number]           Sleep a number of seconds at startup    \n"
	    "                                    to enable debugging.                    \n");
  ::exit(EINVAL);
}

static void invalid_arg(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  ::vfprintf(stderr,fmt,args);
  va_end (args);
  help();
}

/// Static printer (RTL overload)
static size_t printout(void* , int , const char* fmt, va_list args)  {
  char buffer[2048];
  size_t len = ::vsnprintf(buffer, sizeof(buffer), fmt, args);
  ::printf("%s\n",buffer);  
  return len;
}


/// Test routine
extern "C" int fsm_dump(int argc, char** argv)  {
  RTL::CLI cli(argc, argv, help);
  string utgid, dns;
  cli.getopt("dns",3,dns);
  cli.getopt("utgid",2,utgid);

  ::lib_rtl_install_printer(printout,0);

  if ( dns.empty() && 0 == ::getenv("DIM_DNS_NODE") ) 
    invalid_arg("Invalid dns argument and no DIM_DNS_NODE environment.");
  else if ( dns.empty() )
    dns = ::getenv("DIM_DNS_NODE");
  if ( utgid.empty() ) utgid = RTL::str_upper(dns)+"_Controller";

  ControllerDump dmp(dns, utgid);
  dmp.start(2);
  IocSensor::instance().run();
  return 1;
}
