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
#include "FiniteStateMachine/Machine.h"
#include "FiniteStateMachine/FSMTypes.h"
#include "Controller/XmlTaskConfiguration.h"
#include "Controller/Controller.h"
#include "RTL/rtl.h"
#include "dis.hxx"

// C/C++ include files
#include <cstdio>
#include <cerrno>
#include <cstdarg>

using namespace std;
using namespace FiniteStateMachine;

static void help_ctrl() {
  ::fprintf(stdout,
	    " fsm_ctrl -arg=<value> [ -arg=<value> ... ]                                 \n"
	    "          -help                     Print this help message.                \n"
	    "          -print=[number]           Print level (0 for debug).              \n"
	    "          -partition=[string]       Partition name.                         \n"
	    "          -runinfo=[string]         Path to file with python formatted      \n"
	    "                                    information from RunInfo datapoint.     \n"
	    "          -mode=[string]            Run configuration.                      \n"
	    "                                    One of [NORMAL,FORKING,CHECKPOINTING].  \n"
	    "          -taskconfig=[string]      Path to xml file with slave information.\n"
	    "          -count=[number]           Number of Moore processes to be forked. \n");
  ::exit(EINVAL);
}

static void invalid_arg(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  ::vfprintf(stderr,fmt,args);
  va_end (args);
  help_ctrl();
}

/// Test routine
extern "C" int fsm_ctrl(int argc, char** argv)  {
  string utgid = RTL::processName(), runinfo, taskdefs, mode, partition, type="FmcSlave";
  int    print = 0, count=-1;
  RTL::CLI cli(argc, argv, help_ctrl);
  cli.getopt("mode",2,mode);
  cli.getopt("type",2,type);
  cli.getopt("print",2,print);
  cli.getopt("count",2,count);
  cli.getopt("runinfo",2,runinfo);
  cli.getopt("partition",2,partition);
  cli.getopt("taskconfig",2,taskdefs);

  if      ( utgid.substr(0,3)=="P00" ) invalid_arg("Invalid UTGID environment='%s'\n",utgid.c_str());
  else if ( partition.empty() ) invalid_arg("Invalid argument -partition='%s'\n",partition.c_str());
  else if ( taskdefs.empty() ) invalid_arg("Invalid argument -taskcondig='%s'\n",taskdefs.c_str());
  else if ( runinfo.empty() ) invalid_arg("Invalid argument -runinfo='%s'\n",runinfo.c_str());
  else if ( mode.empty()   ) invalid_arg("Invalid argument -mode='%s'\n",mode.c_str());
  else if ( count < 0     ) invalid_arg("Invalid argument -count='%d'\n",count);

  TypedObject::setPrintLevel(print);
  Machine              mach(fsm_type("DAQ"),utgid+"::daq");
  Controller           ctrl(utgid,&mach);
  XmlTaskConfiguration cfg(partition,taskdefs,runinfo,mode,count);
  
  ctrl.display(ctrl.INFO,"%s> Selected running mode is:%s",utgid.c_str(),mode.c_str());
  if ( !cfg.attachTasks(mach,type) )  {
    ::fprintf(stderr,"Failed to interprete XML tasklist.\n");
    ::exit(EINVAL);
  }
  ctrl.display(ctrl.INFO,"Controller task started...");
  ctrl.run();
  return 1;
}
