// $Id: NodeStatsCollector.cpp,v 1.5 2008-11-20 15:43:59 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/NodeStatsCollector.cpp,v 1.5 2008-11-20 15:43:59 frankb Exp $
#define MBM_IMPLEMENTATION
#include "ROMon/NodeStatsCollector.h"
#include "ROMon/CPUMonOstream.h"
#include "ROMon/ROMonOstream.h"
#include "ROMon/SysInfo.h"
#include "ROMonDefs.h"
#include "RTL/Lock.h"
#include "dis.hxx"
#include <stdexcept>

using namespace std;
using namespace ROMon;

static void check_dbg(RTL::CLI& cli) {
  bool dbg = cli.getopt("debug",3) != 0;
  while(dbg) {
    ::lib_rtl_sleep(100);
    if ( !dbg ) break;
  }
}

/// Standard constructor
NodeStatsCollector::NodeStatsCollector(int argc, char** argv)
  : m_sys(0), m_print(0), m_verbose(false),
    m_mbmDelay(500), m_mbmSvc(0), m_mbmSize(100), m_mbmBuffer(0),
    m_statDelay(4000), m_statSvc(0), m_statSize(100), m_statBuffer(0),
    m_mbm(0), m_fsm()
{
  RTL::CLI cli(argc, argv, NodeStatsCollector::help);
  std::string svc, nam;
  check_dbg(cli);
  cli.getopt("publish",   2, svc);
  cli.getopt("statSize",  5, m_statSize);
  cli.getopt("mbmSize",   4, m_mbmSize);
  cli.getopt("statDelay", 5, m_statDelay);
  cli.getopt("mbmDelay",  4, m_mbmDelay);
  m_print   = cli.getopt("print",2) != 0;
  m_verbose = cli.getopt("verbose",1) != 0;
  m_statSize   *= 1024;
  m_statBuffer  = new char[m_statSize];
  m_mbmSize    *= 1024;
  m_mbmBuffer   = new char[m_mbmSize];
  CPUMonData cpu(m_statBuffer);
  cpu.node->reset();
  ROMonData mbm(m_mbmBuffer);
  mbm.node->reset();
  nam = svc;
  if ( !svc.empty() )  {
    std::string dns = ::getenv("DIM_DNS_NODE") ? ::getenv("DIM_DNS_NODE") : "None";
    bool has_mbm = RTL::nodeNameShort() != dns;
    nam = svc + "/Statistics";
    m_statSvc = ::dis_add_service((char*)nam.c_str(),(char*)"C",0,0,feedStats,(long)this);
    nam = svc + "/Readout";
    if ( has_mbm ) m_mbmSvc = ::dis_add_service((char*)nam.c_str(),(char*)"C",0,0,feedMBM,(long)this);
  }
  else  {
    log() << "Unknown data type -- cannot be published." << std::endl;
    throw std::runtime_error("Unknown data type and unknwon service name -- cannot be published.");
  }
  m_fsm.setVerbose(m_verbose);
  m_fsm.start();
  DimServer::start(svc.c_str());
}

/// Default destructor
NodeStatsCollector::~NodeStatsCollector() {
  ::dis_remove_service(m_statSvc);
  if ( 0 != m_mbmSvc ) ::dis_remove_service(m_mbmSvc);
}

/// Help printout in case of -h /? or wrong arguments
void NodeStatsCollector::help() {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"romon_syscollect -opt [-opt]\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -s[ize]=<number>  Size of the global section in kBytes\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -d[elay]=<number> Delay in milliseconds between two updates of the information\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -p[rint]=<number> Make printout to stdout\n");  
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -v[erbose]        Make printouts to stdout.\n");  
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -p[ublish]        DIM Servive name.\n");  
}

/// Feed data to DIS when updating data
void NodeStatsCollector::feedStats(void* tag, void** buf, int* size, int* first) {
  static const char* empty = "";
  NodeStatsCollector* h = *(NodeStatsCollector**)tag;
  if ( !(*first) ) {
    CPUMonData cpu(h->m_statBuffer);
    *buf = h->m_statBuffer;
    *size  = cpu.node->length();
    if ( h->m_verbose ) {
      log() << "[NodeStatsCollector] Published " << *size 
            << " Bytes of Stat data @" << *buf << std::endl;
    }
    return;
  }
  *size = 0;
  *buf = (void*)empty;
}

/// Feed data to DIS when updating data
void NodeStatsCollector::feedMBM(void* tag, void** buf, int* size, int* first) {
  static const char* empty = "";
  NodeStatsCollector* h = *(NodeStatsCollector**)tag;
  if ( !(*first) ) {
    ROMonData ro(h->m_mbmBuffer);
    *buf = h->m_mbmBuffer;
    *size  = ro.node->length();
    if ( h->m_verbose ) {
      log() << "[NodeStatsCollector] Published " << *size 
            << " Bytes of data MBM @" << *buf << std::endl;
    }
    return;
  }
  *size = 0;
  *buf = (void*)empty;
}

/// Monitor Node statistics information
int NodeStatsCollector::monitorStats() {
  CPUMonData buf(m_statBuffer);
  buf.node->type = NodeStats::TYPE;
  m_sys->update();
  if ( m_print ) {
    log() << "SysInfo: ========================" 
          << ::lib_rtl_timestr() 
          << "========================" << endl
          << *buf.node << endl;
    log() << "------ Data size:" << m_sys->statistics()->length() << endl;
  }
  return 1;
}

/// Monitor task and MBM information
int NodeStatsCollector::monitorTasks() {
  ROMonData task_data(m_mbmBuffer);
  typedef RODimFSMListener::Clients Clients;
  m_mbm.handle(task_data.node, m_mbmSize);
  Node::Tasks* tasks = task_data.node->tasks();
  Node::Tasks::iterator it = tasks->reset();
  const Clients& cl = m_fsm.clients();
  task_data.node->type = Node::TYPE;
  for(Clients::const_iterator ic = cl.begin(); ic != cl.end(); ++ic) {
    const FSMTask* t = (*ic).second->data<FSMTask>();
    if ( ((char*)it) > task_data.str+m_mbmSize ) {
      log() << "Task Buffer section memory too small.....exiting" << endl;
      return 0;
    }
    if ( t->processID != -1 ) {
      ::memcpy(it,t,sizeof(FSMTask));
      it = tasks->add(it);
    }
  }
  task_data.node->fixup();
  if ( m_print ) {
    log() << "========================" << ::lib_rtl_timestr() 
          << "========================" << endl
          << *task_data.node << endl;
  }
  return 1;
}

/// Start monitoring activity
int NodeStatsCollector::monitor() {
  std::string node = RTL::nodeNameShort();
  CPUMonData buf(m_statBuffer);
  bool exec = true;
  SysInfo sys(buf.node,m_statSize,2);
  sys.init();
  m_sys = &sys;
  int stat_delay = m_statDelay;
  while(exec)    {
    stat_delay -= m_mbmDelay;
    ::dim_lock();
    try {
      if ( 0 != m_mbmSvc )  {
	monitorTasks();
      }
      if ( stat_delay<=0 ) {
        monitorStats();
      }
    }
    catch(const std::exception& e) {
      log() << "Exception while reading information:" << e.what() << endl;
    }
    catch(...) {
      log() << "Unknown exception while task information:" << endl;
    }
    ::dim_unlock();
    if ( 0 != m_mbmSvc ) ::dis_update_service(m_mbmSvc);
    if ( stat_delay<=0 ) {
      ::dis_update_service(m_statSvc);
      stat_delay = m_statDelay;
    }
    ::lib_rtl_sleep(m_mbmDelay);
  }
  return 1;
}

extern "C" int romon_syscollect(int argc, char** argv) {
  NodeStatsCollector romon(argc,argv);
  print_startup("System information COLLECTOR");
  return romon.monitor();
}
