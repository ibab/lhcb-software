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
#include "CPP/Interactor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"

#include "RTL/readdir.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <memory>
#include <sstream>

using namespace std;
using namespace FiniteStateMachine;

/// Standard constructor
RunMonitorToDoScan::RunMonitorToDoScan(const RunMonitorConfig* cfg)
  : CPP::Interactor(), TypedObject(0, "Scanner_"+cfg->name), m_config(cfg)
{
}

/// Standard destructor
RunMonitorToDoScan::~RunMonitorToDoScan()   {
}

/// Interactor callback to handle external interrupts
void RunMonitorToDoScan::handle(const CPP::Event& event)   {
  switch(event.eventtype) {
  case IocEvent:
    switch(event.type)  {
    case CMD_EXIT:
      ::exit(event.iocData<long>());
      break;
    case CMD_SCAN:
    case CMD_EXECUTE:
      display(ALWAYS,"CMD_CLEAR","Re-scanning todo directory %s.",m_config->todo.c_str());
      scan(event.iocPtr<CPP::Interactor>());
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

/// Scan the work directory and send the work to the master
void RunMonitorToDoScan::scan(CPP::Interactor* requestor)  {
  DIR* dir = ::opendir(m_config->todo.c_str());
  try  {
    for(dirent* dp = dir ? readdir(dir) : 0; dp; dp = readdir(dir))  {
      const char* fname = dp->d_name;
      if ( *fname && ::isdigit(*fname) )    {
	string path = m_config->todo + string("/") + fname + string("/") + fname + ".run";
	display(ALWAYS,"build_item","Next work item:%s",fname);
	auto_ptr<RunMonitorItem> item(RunMonitorItem::fromXMLFile(fname,path));
	if ( item.get() )  {
	  if ( m_config->debugScanner ) item->dump();
	  if ( requestor )  {
	    IocSensor::instance().send(requestor, CMD_ADD, item.release());
	  }
	}
      }
    }
  }
  catch(...)  {
    display(ALWAYS,"build_item","Error scanning todo directory:%s",m_config->todo.c_str());
  }
  if ( dir )   {
    ::closedir(dir);
  }
}

static void help_scan() {
  ::fprintf(stdout,
	    " fsm_ctrl -arg=<value> [ -arg=<value> ... ]                                 \n"
	    "          -help                     Print this help message.                \n"
	    "          -d<irectory>=[file]       Relative path to ToDo directory.        \n"
	    "          -print=[number]           Print level (0 for debug).              \n"
	    "          -sleep=[number]           Sleep a number of seconds at startup    \n"
	    "                                    to enable debugging.                    \n");
  ::exit(EINVAL);
}

extern "C" int runmonitor_scan_test(int argc, char** argv)  {
  string todo = "./ToDo";
  int    print = 0, secs_sleep=0;
  RTL::CLI cli(argc, argv, help_scan);
  cli.getopt("todo",1,todo);
  cli.getopt("print",2,print);
  cli.getopt("sleep",2,secs_sleep);

  if ( secs_sleep > 0 ) {
    for(secs_sleep *= 1000; secs_sleep >= 0; secs_sleep -= 100)
      ::lib_rtl_sleep(100);
  }
  TypedObject::setPrintLevel(print);
  RunMonitorConfig cfg;
  cfg.todo = todo;
  cfg.debugScanner = true;
  RunMonitorToDoScan scan(&cfg);
  IocSensor::instance().send(&scan,CMD_EXECUTE,nullptr);
  IocSensor::instance().send(&scan,CMD_EXIT,nullptr);
  IocSensor::instance().run();
  return 1;
}
