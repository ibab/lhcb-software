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
#include <memory>

using namespace std;
using namespace FiniteStateMachine;

static void help_ctrl() {
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
  help_ctrl();
}

namespace Online {
  struct FMCLogger {
    typedef size_t (*print_function_t)(void*, int, const char*, const char*);
    std::string m_program;
    std::string m_proc;
    std::string m_node;
    int         m_fifoFD;

  protected:
    static size_t print_func(void* arg, int severity, const char* src, const char* str);
  public:
    FMCLogger(const std::string& exe);
    virtual ~FMCLogger();
    print_function_t printer() const { return print_func; }
    size_t print(int typ, const char* src, const char* text);
  };
}

/// Test routine
extern "C" int fsm_ctrl(int argc, char** argv)  {
  auto_ptr<Online::FMCLogger> logger;
  string utgid = RTL::processName(), runinfo, taskdefs, mode, partition;
  string fsm_typ_name = "DAQ", slave_type="FmcSlave";
  int    print = 0, count=-1, secs_sleep=0, bind_cpus=0;
  RTL::CLI cli(argc, argv, help_ctrl);
  cli.getopt("fsm",3,fsm_typ_name);
  cli.getopt("mode",2,mode);
  cli.getopt("type",2,slave_type);
  cli.getopt("print",2,print);
  cli.getopt("count",2,count);
  cli.getopt("runinfo",2,runinfo);
  cli.getopt("partition",2,partition);
  cli.getopt("taskconfig",2,taskdefs);
  cli.getopt("sleep",2,secs_sleep);
  cli.getopt("bindcpus",2,bind_cpus);

  if ( secs_sleep > 0 ) {
    for(secs_sleep *= 1000; secs_sleep >= 0; secs_sleep -= 100)
      ::lib_rtl_sleep(100);
  }
  if ( slave_type == "FmcSlave" ) {
    /// Need to install a proper printer, which directy writes in the 
    /// proper format to the FCM fifo
    logger = auto_ptr<Online::FMCLogger>(new Online::FMCLogger("CTRL"));
    TypedObject::setPrinter(logger.get(),logger->printer());
  }

  if      ( utgid.substr(0,3)=="P00" ) invalid_arg("Invalid UTGID environment='%s'\n",utgid.c_str());
  else if ( partition.empty() ) invalid_arg("Invalid argument -partition='%s'\n",partition.c_str());
  else if ( taskdefs.empty() ) invalid_arg("Invalid argument -taskcondig='%s'\n",taskdefs.c_str());
  else if ( runinfo.empty() ) invalid_arg("Invalid argument -runinfo='%s'\n",runinfo.c_str());
  else if ( mode.empty()   ) invalid_arg("Invalid argument -mode='%s'\n",mode.c_str());
  else if ( count < 0     ) invalid_arg("Invalid argument -count='%d'\n",count);

  TypedObject::setPrintLevel(print);
  Machine              mach(fsm_type(fsm_typ_name),utgid+"::daq");
  Controller           ctrl(utgid,&mach);
  XmlTaskConfiguration cfg(partition,taskdefs,runinfo,mode,count);
  
  ctrl.display(ctrl.INFO,utgid.c_str(),"Selected running mode is:%s",mode.c_str());
  if ( !cfg.attachTasks(mach,slave_type,bind_cpus != 0) )  {
    ::fprintf(stderr,"Failed to interprete XML tasklist.\n");
    ::exit(EINVAL);
  }
  ctrl.display(ctrl.INFO,utgid.c_str(),"Controller task started...");
  ctrl.run();
  return 1;
}

#include "RTL/rtl.h"
#include "RTL/strdef.h"

using namespace Online;

FMCLogger::FMCLogger(const std::string& exe)
  : m_program(exe)
{
  string tmp = RTL::nodeNameShort();
  m_proc     = RTL::processName();
  m_node     = RTL::str_lower(tmp);
  m_fifoFD   = ::fileno(stderr);
}

FMCLogger::~FMCLogger() {
}

size_t FMCLogger::print(int typ, const char* src, const char* text) {
  static const char *sl[8]={"[NIL]  ","[VERB] ","[DEBUG]","[INFO] ","[WARN] ","[ERROR]",
			    "[FATAL]", "[INFO]"};
  char buffer[4096];
  struct tm lNow;
  time_t now = ::time(0);
  ::localtime_r(&now,&lNow);
  size_t len = ::strftime(buffer,sizeof(buffer),"%b%d-%H%M%S",&lNow);
  typ = (typ>int(sizeof(sl)/sizeof(sl[0]))) ? (sizeof(sl)/sizeof(sl[0]))-1 : (typ<0 ? 0 : typ);
  len += ::snprintf(buffer+len,sizeof(buffer)-len,"%s%s: %s(%s): %s: %s\n",
		    sl[typ],m_node.c_str(),m_program.c_str(),m_proc.c_str(),src,text);
  buffer[len] = 0;
  buffer[sizeof(buffer)-1] = 0;
  for(size_t written = 0; written < len; ) {
    int w = ::write(m_fifoFD,buffer+written,len-written);
    if ( errno != EAGAIN ) break;
    written += w;
  }
#if 0
  size_t len = ::fputs(str,stdout);
  ::fflush(stdout);
#endif
  return len;
}

size_t FMCLogger::print_func(void* arg, int severity, const char* src, const char* text) {
  return ((FMCLogger*)arg)->print(severity, src, text);
}
