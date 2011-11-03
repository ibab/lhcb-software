#include "Framework/DataflowMgr.h"
#include "Framework/Monitor.h"
#include "Test/Algorithm.h"
#include "Test/Bits.h"
#include "RTL/rtl.h"
#include <cstdarg>
#include <cstring>
#include <memory>

using namespace std;
using namespace Framework;
using namespace FrameworkTest;

namespace FrameworkTest {
  float time_cutoff = 0.0;
  // float time_cutoff = 50000.0;
  // float time_cutoff = 25000.0;
  // float time_cutoff = 10000.0;

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
  size_t getMultiplicity(size_t multiplicity,const AlgConfig& c) {
    if ( c.name == "DstWriter" ) return 1;
    if ( c.name == "Framework" ) return 1;
    if ( c.time < time_cutoff  ) return 1;
    if ( time_cutoff>0.00001 ) {
      ::printf("Algorithm:%-35s max instances:%d time:%8.0f\n",c.name.c_str(),int(multiplicity),c.time);
    }
    return multiplicity;
  }
}

static size_t output(void* par, int lvl, const char* fmt, va_list args) {
  long level = (long)par;
  if ( long(lvl)>=level ) {
    std::string format = fmt;
    size_t len = ::vprintf((format+"\n").c_str(), args);
    return len;
  }
  return 0;
}

static void help() {
}

namespace {
  struct Params {
    size_t multiplicity;
    size_t num_events;
    size_t total_events;
    size_t thread_count;
    size_t wait;
    float  cut;
    int    print, log;
    Params() { ::memset(this,0,sizeof(Params)); }
  };
}

static void execTest(Monitor* monitor, const Params& par) {
  typedef AlgorithmExecutorFactory<AlgConfig> AlgEntry;
  DataflowMgr mgr(par.log,monitor), *manager=&mgr;
  size_t multiplicity = par.multiplicity;
  const char* line = "====================================================";
  const char* raw_mask[]   = {
    "/Event",
    "/Event/DAQ",
    "/Event/DAQ/RawEvent",
    0};
  ::lib_rtl_output(LIB_RTL_ALWAYS,"%s  START      %s",line,line);
  ::lib_rtl_output(LIB_RTL_ALWAYS,"==  Params: total_events:%ld num_events:%ld  multiplicity:%ld thread_count:%ld cut:%7.0f",
		   par.total_events, par.num_events, par.multiplicity, par.thread_count, par.cut);
  ::lib_rtl_output(LIB_RTL_ALWAYS,"%s=============%s",line,line);
  time_cutoff = par.cut;
  ContextFactory* ctxt = new ContextFactory(mgr.ioMask(raw_mask));
  mgr.adoptContextFactory(ctxt);
#include "code.cpp"
  mgr.resolveInputs();
  mgr.createThreads(par.thread_count);
  mgr.start(par.total_events,par.num_events);
  ::lib_rtl_sleep(par.wait);
  mgr.printStatistics(par.num_events);
  mgr.shutdown(false,true);
  mgr.stop();
  ::lib_rtl_output(LIB_RTL_ALWAYS,"%s  FINISHED   %s",line,line);
}

int single_run(Monitor* monitor, const Params& par) {
  const char* line = "====================================================";
  ::lib_rtl_output(LIB_RTL_INFO,"++ Executing test: %s   run   %s",line,line);
  execTest(monitor,par);
  ::lib_rtl_output(LIB_RTL_INFO,"%s  DONE  %s",line,line);
  return 1;
}

int multi_run(const string& hist, const Params& par) {
  char text[1024];
  const char* line = "====================================================";
  ::lib_rtl_output(LIB_RTL_INFO,"++ Executing test: %s   run   %s",line,line);

  Params p = par;
  bool first = true;
  for(size_t num_events=1; num_events<=par.num_events; ++num_events) {
    p.num_events = num_events;
    //for(size_t multiplicity=1; multiplicity<=par.multiplicity;++multiplicity) 
    {
      p.multiplicity = par.multiplicity;
      for(size_t n_thread=1; n_thread<=par.thread_count; n_thread += (n_thread<5 ? 1 : 2)) {
	p.thread_count = n_thread;
	::sprintf(text,"lbGCDTest.exe -multiplicity=%d -total_events=%d -num_events=%d -thread_count=%d -wait=10 -out=%s -cut=%d %s",
		  int(p.multiplicity),int(p.total_events),int(p.num_events),int(p.thread_count),hist.c_str(),
		  int(par.cut), first ? "" : "-update");
	
	first = false;
	//::lib_rtl_output(LIB_RTL_ALWAYS,"%s\n",text);
	::system(text);
      }
    }
  }

  ::lib_rtl_output(LIB_RTL_INFO,"%s  DONE  %s",line,line);
  return 1;
}

int main(int argc, char** argv) {
  Params par;
  string hist_file;
  RTL::CLI cli(argc,argv,help);
  cli.getopt("log",2,par.log=LIB_RTL_INFO);
  cli.getopt("print",2,par.print=LIB_RTL_WARNING);
  cli.getopt("multiplicity",2,par.multiplicity=1);
  cli.getopt("num_events",  2,par.num_events=10);
  cli.getopt("total_events",2,par.total_events=200);
  cli.getopt("thread_count",2,par.thread_count=32);
  cli.getopt("wait_threads",2,par.wait=1000);
  cli.getopt("output",      2,hist_file);
  cli.getopt("cut",         2,par.cut);
  bool update = cli.getopt("update",2);

  lib_rtl_install_printer(output,(void*)par.print);
  if ( cli.getopt("sweep",2) == 0 ) {
    auto_ptr<Monitor> monitor(hist_file.empty() ? 0 : new Monitor(hist_file,update));
    return single_run(monitor.get(),par);
  }
  return multi_run(hist_file,par);
}
