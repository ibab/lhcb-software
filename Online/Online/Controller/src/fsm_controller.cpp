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
#include "FiniteStateMachine/Controller.h"
#include "FiniteStateMachine/XmlTaskConfiguration.h"
#include "RTL/rtl.h"
#include "dis.hxx"

// C/C++ include files
#include <cstdio>
#include <cerrno>

using namespace std;
using namespace FiniteStateMachine;
using namespace FiniteStateMachine::DAQ;
typedef FSM::ErrCond ErrCond;

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

/// Test routine
extern "C" int fsm_ctrl(int argc, char** argv)  {
  string utgid = RTL::processName(), runinfo, taskdefs, mode, partition;
  int    print = 0, count=-1;
  RTL::CLI cli(argc, argv, help_ctrl);
  cli.getopt("mode",2,mode);
  cli.getopt("print",2,print);
  cli.getopt("count",2,count);
  cli.getopt("runinfo",2,runinfo);
  cli.getopt("partition",2,partition);
  cli.getopt("taskconfig",2,taskdefs);

  if      ( utgid.substr(0,3)=="P00" ) help_ctrl();
  else if ( partition.empty() ) help_ctrl();
  else if ( taskdefs.empty() ) help_ctrl();
  else if ( runinfo.empty() ) help_ctrl();
  else if ( mode.empty() ) help_ctrl();
  else if ( count < 0 ) help_ctrl();

  TypedObject::setPrintLevel(print);
  Machine              mach(fsm_type("DAQ"),utgid+"::daq");
  Controller           ctrl(utgid,&mach);
  XmlTaskConfiguration cfg(taskdefs);

  if ( !cfg.attachTasks(mach,partition,runinfo,mode,count) )  {
    ::exit(EINVAL);
  }
  mach.setHandler(&ctrl);
  ctrl.display(ctrl.ALWAYS,"Controller task started...");
  DimServer::autoStartOn();
  DimServer::start(utgid.c_str());
  ctrl.run();
  return 1;
}
