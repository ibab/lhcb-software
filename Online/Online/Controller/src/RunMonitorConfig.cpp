/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**      FOR HLT2 monitoring purposes
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/
#include "Controller/RunMonitor.h"
#include "RTL/rtl.h"
#include <climits>
#include <sys/stat.h>
#include <sys/types.h>


using namespace std;
using namespace FiniteStateMachine;

/// Standard constructor
RunMonitorConfig::RunMonitorConfig()  {
  // Set default values
  char buff[PATH_MAX];
  const char* cw_dir = ::getcwd(buff,sizeof(buff));
  string cwd = cw_dir ? cw_dir : ".";
  todo                  =  cwd + "/ToDo";
  done                  =  cwd + "/Done";
  failed                =  cwd + "/Failed";
  running               =  cwd + "/Running";
  archive               =  cwd + "/Archive";
  slaveType             =  "FmcSlave";
  taskType              =  "";
  dataDirectory         =  "/daqarea/lhcb/data/2015/RAW/FULL";

  debugMessenger        =  false;
  debugScanner          =  false;
  debugCtrl             =  false;
  internalOnly          =  false;
  scanInterval          = -1;
  numWorkers            =  1;
  numEventsPerRun       =  60000UL;
  avgEventSize          =  100 * 1024;             // 100 kB average event size
  minEventsPerFile      =  1024UL;                 // Min 1000 events per file
  minEventsPerRun       =  60000UL;                // Min 60 k events per run before execution starts
  createSlave           =  0;
}

/// Standard destructor
RunMonitorConfig::~RunMonitorConfig()  {
}

/// Recalculate depending values
RunMonitorConfig& RunMonitorConfig::config()  {
  return *this;
}

static bool make_dir(const std::string& nam)   {
  string sub_dir;
  for(size_t idx = nam.find("/",1); sub_dir != nam; idx = nam.find("/",idx+1) )  {
    struct stat stat_buff;
    sub_dir = nam.substr(0,idx);
    if ( 0 == ::stat(sub_dir.c_str(),&stat_buff) )
      continue;
    int ret = ::mkdir(sub_dir.c_str(),S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
    if ( 0 != ret )  {
      printf("FAILURE: Cannot create directory:%s : %s\n", sub_dir.c_str(), RTL::errorString());
      return false;
    }
  }
  ::printf("+++ Checked/created working directory:%s\n",sub_dir.c_str());
  return true;
}

/// Creata all directories used by the application: todo, done, failed, running, etc.
bool RunMonitorConfig::make_dirs()  {
  if ( make_dir(todo) && make_dir(done) && make_dir(running) && make_dir(failed) && make_dir(archive) )  {
    if ( make_dir(archive+"/Done") && make_dir(archive+"/Failed") )
      return true;
  }
  throw runtime_error("Failed to create working directories.");
}
