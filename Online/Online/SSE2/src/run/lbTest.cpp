#include "Framework/DataflowMgr.h"
#include "Framework/Monitor.h"
#include "Test/Algorithm.h"
#include "Test/Bits.h"
#include "RTL/rtl.h"
#include <cstdarg>

using namespace std;
using namespace Framework;
using namespace FrameworkTest;

namespace FrameworkTest {
  struct AlgConfig : public AlgorithmConfig  {
    AlgConfig(const string& n, ClientData d, const IODef& i, const IODef& o, float t, float s)
      : AlgorithmConfig(n,d,i,o,t,s) {}
    AlgConfig(const AlgConfig& c)
      : AlgorithmConfig(c.name,c.param,c.inputs,c.outputs,c.time,c.timeSigma),
	parent(c.parent), type(c.type), order(c.order), level(c.level), 
	flags(c.flags), daughters(c.daughters) 
    {}

    string parent;
    string type;
    int order;
    int level;
    int flags;
    vector<string> daughters;
    void setDaughters(const char* d[]) {  daughters = IODef(d); }
  };
}

static size_t output(void* par, int lvl, const char* fmt, va_list args) {
  long level = (long)par;
  if ( long(lvl)>=level ) {
    std::string format = fmt;
    size_t len = ::vprintf(fmt, args);
    return len;
  }
  return 0;
}

static void help() {
}

int main(int argc, char** argv) {
  RTL::CLI cli(argc,argv,help);
  size_t multiplicity=1, num_events=10, total_events=200, thread_count=32, wait=1000;
  Monitor* monitor = 0;
  string hist_file;
  DataflowMgr mgr, *manager=&mgr;
  const char* raw_mask[]   = {
    "/Event",
    "/Event/DAQ",
    "/Event/DAQ/RawEvent",
    0};
  ContextFactory* ctxt = new ContextFactory(&mgr, raw_mask);
  mgr.adoptContextFactory(ctxt);

  typedef AlgorithmExecutorFactory<AlgConfig> AlgEntry;

  cli.getopt("multiplicity",2,multiplicity);
  cli.getopt("num_events",2,num_events);
  cli.getopt("total_events",2,total_events);
  cli.getopt("thread_count",2,thread_count);
  cli.getopt("wait_threads",2,wait);
  cli.getopt("hist_file",2,hist_file);
  
  lib_rtl_install_printer(output,(void*)LIB_RTL_WARNING);

  if ( !hist_file.empty() ) {
    monitor = new Monitor(hist_file);
    mgr.setMonitor(monitor);
  }
#include "code.cpp"

  const char* line = "====================================================";
  ::lib_rtl_output(LIB_RTL_INFO,"++ Executing test: %s   run   %s\n",line,line);
  mgr.resolveInputs();
  for(int j=1, i=0; i<1; ++i)  {
    ::lib_rtl_output(LIB_RTL_INFO,"%s  %d  %s\n",line,i,line);
    mgr.createThreads(thread_count);
    while(j>0)  {
      ::lib_rtl_output(LIB_RTL_ALWAYS,"%s  START %3d  %s\n",line,j,line);
      mgr.start(total_events,num_events);
      ::lib_rtl_sleep(wait);
      mgr.shutdown(true,true);
      //mgr.restart();
      --j;
    }
    mgr.stop();
  }
  ::lib_rtl_output(LIB_RTL_INFO,"%s  DONE  %s\n",line,line);
  //::lib_rtl_sleep(100000);
  if ( monitor ) delete monitor;
  return 1;
}
