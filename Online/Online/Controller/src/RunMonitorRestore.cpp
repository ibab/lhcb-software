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

// Framework include files
#include "Controller/RunMonitor.h"
#include "CPP/TimeSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"

#include "RTL/readdir.h"
#include "RTL/rtl.h"
#include "dis.hxx"
#include "dic.hxx"
#include <memory>
#include <libgen.h>

using namespace std;
using namespace FiniteStateMachine;

/// Standard constructor
RunMonitorRestore::RunMonitorRestore(RunMonitorCoordinator* coord, const RunMonitorConfig* cfg)
  : CPP::Interactor(), TypedObject(0,"Restore_"+cfg->name), m_coordinator(coord), m_config(cfg)
{
}

/// Standard destructor
RunMonitorRestore::~RunMonitorRestore()   {
}

/// Interactor callback to handle external interrupts
void RunMonitorRestore::handle(const CPP::Event& event)   {
  try {
    switch(event.eventtype) {
    case TimeEvent:
      break;    
    case NetEvent:
      break;
    case IocEvent:
      handleIoc(event);
      break;
    default:
      break;
    }
  }
  catch(...)  {
  }
}

/// Interactor callback to handle IocSensor interrupts
void RunMonitorRestore::handleIoc(const CPP::Event& event)   {
  switch(event.type)  {
  case CMD_EXIT:
    ::exit(event.iocData<long>());
    break;

  case CMD_EXECUTE:
    scan();
    break;
  default:
    break;
  }
}

static void unlink_dir(const std::string& f)   {
  DIR* dir = ::opendir(f.c_str());
  if ( dir )  {
    for(dirent* dp = readdir(dir); dp; dp = readdir(dir))
      if ( ::isalnum(*dp->d_name) && -1 == ::remove(((f+"/")+dp->d_name).c_str()) ) perror(dp->d_name);
    ::closedir(dir);
    if ( -1 == ::remove(f.c_str()) ) perror(f.c_str());
  }
}

/// Scan the work directory and re-establish work items
void RunMonitorRestore::scan()   {
  DIR* dir = ::opendir(m_config->running.c_str());
  try  {
    for(dirent* dp = dir ? readdir(dir) : 0; dp; dp = readdir(dir))  {
      string n = dp->d_name;
      if ( n[0] && ::isdigit(n[0]) )    {
	string todo_dir = m_config->todo + "/" + n;
	string runn_dir = m_config->running + "/" + n;
	string dp, fname = runn_dir + "/" +n + ".run";
	auto_ptr<RunMonitorItem> item(RunMonitorItem::fromXMLFile(n,fname));
	if ( item.get() )  {
	  dp = item->utgid(m_config->name)+"/EXIT";
	  display(ALWAYS,"RUNNING","Kill OLD task: %s....",dp.c_str());
	  ::dic_cmnd_service(dp.c_str(),(void*)"1",2);
	  display(ALWAYS,"RUNNING","Rename work item %s to %s.",runn_dir.c_str(), todo_dir.c_str());
	  unlink_dir(todo_dir);
	  int ret = ::rename(runn_dir.c_str(), todo_dir.c_str());
	  if ( 0 != ret )   {
	    display(ALWAYS,"RUNNING","Failed to rename work item from %s to %s. [%s]",
		    runn_dir.c_str(), todo_dir.c_str(), RTL::errorString());
	  }
	  if ( m_coordinator )  {
	    IocSensor::instance().send(m_coordinator,CMD_ADD,(void*)item.release());
	  }
	}
      }
    }
  }
  catch(...)  {
    display(ALWAYS,"Restore","Error scanning todo directory:%s",m_config->todo.c_str());
  }
  if ( dir )   {
    ::closedir(dir);
  }
}

static void help_restore() {
  ::fprintf(stdout,
	    " runmonitor_restore_test -arg=<value> [ -arg=<value> ... ]                  \n"
	    "          -help                     Print this help message.                \n"
	    "          -print=[number]           Print level (0 for debug).              \n"
	    "          -sleep=[number]           Sleep a number of seconds at startup    \n"
	    "                                    to enable debugging.                    \n");
  ::exit(EINVAL);
}

extern "C" int runmonitor_restore_test(int argc, char** argv)  {
  RunMonitorConfig cfg;
  int    print = 0, secs_sleep=0;
  RTL::CLI cli(argc, argv, help_restore);
  cfg.name = "Test";
  cli.getopt("todo",   1,cfg.todo = "./ToDo");
  cli.getopt("running",1,cfg.running = "./Running");
  cli.getopt("print",  2,print);
  cli.getopt("sleep",  2,secs_sleep);

  if ( secs_sleep > 0 ) {
    for(secs_sleep *= 1000; secs_sleep >= 0; secs_sleep -= 100)
      ::lib_rtl_sleep(100);
  }
  TypedObject::setPrintLevel(print);
  RunMonitorRestore restore(0, &cfg);
  IocSensor::instance().send(&restore,CMD_EXECUTE,nullptr);
  IocSensor::instance().send(&restore,CMD_EXIT,nullptr);
  IocSensor::instance().run();
  return 1;
}
