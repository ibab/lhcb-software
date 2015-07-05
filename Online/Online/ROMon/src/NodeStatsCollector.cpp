// $Id: NodeStatsCollector.cpp,v 1.8 2010-10-14 13:30:09 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/NodeStatsCollector.cpp,v 1.8 2010-10-14 13:30:09 frankb Exp $
#define MBM_IMPLEMENTATION
#include "ROMon/NodeStatsCollector.h"
#include "ROMon/CPUMonOstream.h"
#include "ROMon/ROMonOstream.h"
#include "ROMon/SysInfo.h"
#include "ROMonDefs.h"
#include "RTL/Lock.h"
#include "RTL/readdir.h"
#include "dis.hxx"
#include <stdexcept>
#include <map>

using namespace std;
using namespace ROMon;

static void check_dbg(RTL::CLI& cli) {
  volatile bool dbg = cli.getopt("debug",3) != 0;
  while(dbg) {
    ::lib_rtl_sleep(100);
    if ( !dbg ) break;
  }
}

/// Standard constructor
NodeStatsCollector::NodeStatsCollector(int argc, char** argv)
  : m_sys(0), m_print(0), m_verbose(false),
    m_mbmDelay(500), m_mbmSvc(0), m_mbmSize(500), m_mbmBuffer(0),
    m_statDelay(4000), m_statSvc(0), m_statSize(500), m_statBuffer(0),
    m_hltSize(100), m_hlt_1_Svc(0), m_hlt_2_Svc(0), 
    m_hlt_1_Buffer(0), m_hlt_2_Buffer(0),
    m_mbm(0), m_fsm()
{
  RTL::CLI cli(argc, argv, NodeStatsCollector::help);
  std::string svc, nam;
  check_dbg(cli);
  cli.getopt("publish",   2, svc);
  cli.getopt("statSize",  5, m_statSize);
  cli.getopt("mbmSize",   4, m_mbmSize);
  cli.getopt("filesSize", 4, m_hltSize);
  cli.getopt("statDelay", 5, m_statDelay);
  cli.getopt("mbmDelay",  4, m_mbmDelay);
  m_print   = cli.getopt("print",2) != 0;
  m_verbose = cli.getopt("verbose",1) != 0;
  m_statSize   *= 1024;
  m_statBuffer  = new char[m_statSize];
  m_mbmSize    *= 1024;
  m_mbmBuffer   = new char[m_mbmSize];
  m_hltSize    *= 1024;
  m_hlt_1_Buffer   = new char[m_hltSize];
  m_hlt_2_Buffer   = new char[m_hltSize];
  nam = strupper(RTL::nodeNameShort())+"_MEPRx_01/OverflowStatus";
  //log() << "MEPRx service:" << nam << endl;
  m_overflow    = '?';
  m_overflowTime= ::time(0);
  m_overflowSvc = ::dic_info_service((char*)nam.c_str(),MONITORED,5,0,0,overflowHandler,(long)this,0,0);
  CPUMonData cpu(m_statBuffer);
  cpu.node->reset();
  ROMonData mbm(m_mbmBuffer);
  mbm.node->reset();
  new(m_hlt_1_Buffer) DeferredHLTStats(RTL::nodeNameShort());
  new(m_hlt_2_Buffer) DeferredHLTStats(RTL::nodeNameShort());

  nam = svc;
  if ( !svc.empty() )  {
    std::string dns = ::getenv("DIM_DNS_NODE") ? ::getenv("DIM_DNS_NODE") : "None";
    bool has_mbm = RTL::nodeNameShort() != dns;
    nam = svc + "/Statistics";
    m_statSvc = ::dis_add_service((char*)nam.c_str(),(char*)"C",0,0,feedStats,(long)this);
    nam = svc + "/Readout";
    if ( has_mbm ) m_mbmSvc = ::dis_add_service((char*)nam.c_str(),(char*)"C",0,0,feedMBM,(long)this);
    nam = svc + "/Hlt1";
    if ( has_mbm ) m_hlt_1_Svc = ::dis_add_service((char*)nam.c_str(),(char*)"C",0,0,feedHLT_1,(long)this);
    nam = svc + "/HltDefer";
    if ( has_mbm ) m_hlt_2_Svc = ::dis_add_service((char*)nam.c_str(),(char*)"C",0,0,feedHLT_2,(long)this);
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
  ::dic_release_service(m_overflowSvc);
  ::dis_remove_service(m_statSvc);
  if ( 0 != m_mbmSvc ) ::dis_remove_service(m_mbmSvc);
  if ( 0 != m_hlt_1_Svc ) ::dis_remove_service(m_hlt_1_Svc);
  if ( 0 != m_hlt_2_Svc ) ::dis_remove_service(m_hlt_2_Svc);
}

/// Dim callback to retrieve the overflow status
void NodeStatsCollector::overflowHandler(void* tag, void* address, int* size) {
  if ( tag ) {
    NodeStatsCollector* it = *(NodeStatsCollector**)tag;
    try {
      it->m_overflow = '?';
      it->m_overflowTime = ::time(0);
      if ( address && size && *size>0 ) {
        if ( ::strncmp((char*)address,"Overflow disabled",16)==0 )
          it->m_overflow = 'N';
        else if ( ::strncmp((char*)address,"Overflow enabled",16)==0 )
          it->m_overflow = 'Y';
        else if ( ::strncmp((char*)address,"Overflow active",15)==0 )
          it->m_overflow = 'Y';
      }
    }
    catch(const exception& e) {
      cout << "Exception in DIM callback processing:" << e.what() << endl;
    }
    catch(...) {
      cout << "UNKNOWN exception in DIM callback processing." << endl;
    }
  }
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
void NodeStatsCollector::feedStats(void* tag, void** buff, int* size, int* ) {
  static const char* empty = "";
  NodeStatsCollector* h = *(NodeStatsCollector**)tag;
  if ( h ) {
    if ( h->m_sys ) {
      CPUMonData cpu(h->m_statBuffer);
      if ( h->m_statBuffer && cpu.node ) {
        *buff = h->m_statBuffer;
        *size = cpu.node->length();
        if ( h->m_verbose ) {
          log() << "[NodeStatsCollector] Published " << *size 
                << " Bytes of Stat data @" << *buff << std::endl;
        }
        return;
      }
    }
  }
  *size = 0;
  *buff = (void*)empty;
}

/// Feed data to DIS when updating data
void NodeStatsCollector::feedMBM(void* tag, void** buff, int* size, int* ) {
  static const char* empty = "";
  NodeStatsCollector* h = *(NodeStatsCollector**)tag;
  if ( h && h->m_mbmBuffer ) {
    ROMonData ro(h->m_mbmBuffer);
    if ( ro.node ) {
      *buff = h->m_mbmBuffer;
      *size  = ro.node->length();
      if ( h->m_verbose ) {
        log() << "[NodeStatsCollector] Published " << *size 
              << " Bytes of data MBM @" << *buff << std::endl;
      }
      return;
    }
  }
  *size = 0;
  *buff = (void*)empty;
}

/// Feed data to DIS when updating data
void NodeStatsCollector::feedHLT_1(void* tag, void** buf, int* size, int* first) {
  NodeStatsCollector* h = *(NodeStatsCollector**)tag;
  h->feedHLT(h->m_hlt_1_Buffer,buf,size,first);
}
/// Feed data to DIS when updating data
void NodeStatsCollector::feedHLT_2(void* tag, void** buf, int* size, int* first) {
  NodeStatsCollector* h = *(NodeStatsCollector**)tag;
  h->feedHLT(h->m_hlt_2_Buffer,buf,size,first);
}

/// Feed data to DIS when updating data
void NodeStatsCollector::feedHLT(char* data_buff, void** buff, int* size, int* ) {
  static const char* empty = "";
  if ( data_buff ) {
    CPUMonData hlt(data_buff);
    if ( hlt.hlt ) {
      *buff = data_buff;
      *size = hlt.hlt->length();
      if ( m_verbose ) {
        log() << "[NodeStatsCollector] Published " << *size 
              << " Bytes of data HLTMon @" << *buff << std::endl;
      }
      return;
    }
  }
  *size = 0;
  *buff = (void*)empty;
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

/// Monitor Deferred HLT statistics information
int NodeStatsCollector::monitorHLT(char* buffer, const std::string& dir_name) {
  int count = 0;
  map<int,int> files;
  unsigned long long blk_size=0,total_blk=0,availible_blk=0;
  DeferredHLTStats* h = new(buffer) DeferredHLTStats(RTL::nodeNameShort());

  /// Now load data into object
  ro_gettime(&h->time,&h->millitm);
  h->overflowState = ((::time(0) - m_overflowTime) > 30) ? '?' : m_overflow;

  DIR* dir = ::opendir(dir_name.c_str());
  if ( dir ) {
    struct dirent *entry;
    ::lib_rtl_diskspace(dir_name.c_str(),&blk_size,&total_blk,&availible_blk);
    h->localdisk.blockSize  = blk_size;
    h->localdisk.numBlocks  = total_blk;
    h->localdisk.freeBlocks = availible_blk;
    while ( (entry=::readdir(dir)) != 0 ) {
      int run=0,date;
      int ret = ::sscanf(entry->d_name,"Run_%07d_%8d-",&run,&date);
      if ( ret == 2 ) {
        map<int,int>::iterator i=files.find(run);
        if ( i==files.end() ) files[run]=1;
        else ++((*i).second);
        ++count;
      }
      else if( !(0==::strcmp(entry->d_name,".") || 0==::strcmp(entry->d_name,"..")) ) {
        // log() << "Strange file name for HLT deferred processing:" << entry->d_name << endl;
      }
    }
    ::closedir(dir);
  }
  else { // If the overflow directory does not exist, the disk-space is void!
    h->localdisk.blockSize  = 4096;
    h->localdisk.freeBlocks = 0;
    h->localdisk.numBlocks  = 0;
  }
  DeferredHLTStats::Runs::iterator ir = h->runs.reset();
  map<int,int>::iterator           i  = files.begin();
  for(; i!=files.end(); ++i) {
    *ir = *i;
    ir = h->runs.add(ir);
  }
  if ( m_print ) {
    log() << "SysInfo: ========================" 
          << ::lib_rtl_timestr() 
          << "========================" << endl
          << *h << endl;
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
  try  {
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
  }
  catch(const exception& e)  {
    log() << "Exception collecting task data:" << e.what() << endl;
    tasks->reset();
  }
  catch (...)  {
    tasks->reset();
  }
  

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
  ::dim_lock();
  SysInfo sys(buf.node,m_statSize,2);
  sys.init();
  m_sys = &sys;
  ::dim_unlock();
  int nclients, stat_delay = m_statDelay;
  while(exec)    {
    stat_delay -= m_mbmDelay;
    ::dim_lock();
    try {
      if ( 0 != m_mbmSvc )  {
        monitorTasks();
      }
      if ( stat_delay<=0 ) {
        monitorStats();
        monitorHLT(m_hlt_1_Buffer,"/localdisk/hlt1");
        monitorHLT(m_hlt_2_Buffer,"/localdisk/overflow");
      }
    }
    catch(const std::exception& e) {
      log() << "Exception while reading information:" << e.what() << endl;
    }
    catch(...) {
      log() << "Unknown exception while task information:" << endl;
    }
    if ( 0 != m_mbmSvc ) {
      if ( (nclients=::dis_update_service(m_mbmSvc)) == 0 ) {
        if ( m_print )   {
          //log() << "No client was listening to my MBM information......" << std::endl;
        }
      }
    }
    if ( stat_delay<=0 ) {
      if ( (nclients=::dis_update_service(m_statSvc)) == 0 ) {
        if ( m_print )   {
          log() << "No client was listening to my node statistics information." << std::endl;
        }
      }
      if ( 0 != m_hlt_1_Svc ) {
        if ( (nclients=::dis_update_service(m_hlt_1_Svc)) == 0 ) {
          if ( m_print )  {
            log() << "No client was listening to the deferred HLT information." << std::endl;
          }
        }
      }
      if ( 0 != m_hlt_2_Svc ) {
        if ( (nclients=::dis_update_service(m_hlt_2_Svc)) == 0 ) {
          if ( m_print )  {
            log() << "No client was listening to the deferred HLT information." << std::endl;
          }
        }
      }
      stat_delay = m_statDelay;
    }
    ::dim_unlock();
    //::lib_rtl_sleep(m_mbmDelay);
    ::dim_usleep(1000*m_mbmDelay);
  }
  return 1;
}

extern "C" int romon_syscollect(int argc, char** argv) {
  NodeStatsCollector romon(argc,argv);
  print_startup("System information COLLECTOR");
  return romon.monitor();
}
