// $Id: EventServerRunable.h,v 1.7 2008-10-14 08:37:21 frankb Exp $
//====================================================================
//  MooreTestSvc
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Description: Runable to run Moore standalone on a single node.

//  Author     : M.Frank
//====================================================================
// $Header: $
#ifndef ONLINE_GAUDIONLINE_MOORETESTSVC_H
#define ONLINE_GAUDIONLINE_MOORETESTSVC_H

// Framework includes
#include "Internals.h"
#include "GaudiOnline/OnlineService.h"
#include "GaudiKernel/IRunable.h"
#include "CPP/Interactor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <map>
#include <ctime>

namespace MBM { class Manager; }

/*
 *   LHCb namespace declaration
 */
namespace LHCb {

  class MooreFileMonitor : public OnlineService  {
  protected:
    /// File descriptor to output file (if set)
    FILE* m_file;
    std::string m_output;
  public:
    typedef MooreTest::UserStats UserStats;
    typedef MooreTest::Statistics Statistics;
    MooreFileMonitor(const std::string& n, ISvcLocator* svc) 
      : OnlineService(n,svc), m_file(stdout) 
    {
      declareProperty("Output",m_output="");
    }
    virtual ~MooreFileMonitor();
    StatusCode initialize();
    StatusCode finalize();
    /// Inform that a new incident has occurred
    void handle(const Incident& incident);
  };

  /** @class MooreTestSvc MooreTestSvc.h MooresStandalone/MooreTestSvc....h
   *
   *   Class definition of MooreTestSvc.
   *
   *
   *  @author Markus Frank
   *  @version 1.0
   */
  class MooreTestSvc : public OnlineService, public Interactor, virtual public IRunable   {
  protected:
    typedef MooreTest::UserStats UserStats;
    typedef MooreTest::Statistics Statistics;

    /// Name of the I/O fifo used by the FmcMessageSvc
    std::string m_fifoName;
    /// Property: Moore version to be used (passed as argument to the 'procname' tasks
    std::string m_mooreVsn;
    /// Property: Assumed partition name
    std::string m_partition;
    /// Property: Script file name for HLT process
    std::string m_script;
    /// Property: Name of the input buffer of the process class to be sampled
    std::string m_input;
    /// Property: Name of the output buffer of the process class to be sampled
    std::string m_output;
    /// Property: The task name to be sampled
    std::string m_procName;
    /// Property: Filename of the online "main" property options
    std::string m_mainOpts;
    /// Property: Options for the buffer manager setup
    std::string m_mbmOpts;
    /// Property: Options for the file reader setup
    std::string m_rdrOpts;
    /// Property: factory name of the result dumper. If empty result goes to stdout
    std::vector<std::string> m_monitorTypes;
    /// Property: Time delay in seconds before measurements start
    int m_startDelay;
    /// Property: Time intervall in seconds between 2 measuements
    int m_intervall;
    /// Property: Total time of the measurement in seconds.
    int m_duration;
    /// Property: Number of slaves to be forked by the  'procname' tasks
    int m_numSlaves;
    /// Property: Partition identifier used by all processes
    int m_partitionID;
    /// Property: indicate if the buffers are partitioned (ie include the paqrtition name)
    bool m_partitionBuffers;
    /// Property: enable automatic running with finalization
    int  m_autoRun;

    /// The file descriptor for the communication fifo
    int m_fifo;
    /// The GBL descriptor for the global MBM section
    lib_rtl_gbl_t    m_bmAll;
    /// Thread descriptor of the message pump
    lib_rtl_thread_t m_messagePump;
    /// Buffer manager descriptor to collect statistics of the input buffer
    MBM::Manager*    m_inputBM;
    /// Buffer manager descriptor to collect statistics of the output buffer
    MBM::Manager*    m_outputBM;
    /// Master to reply to
    Interactor*      m_master;
    /// Extracted statistics
    Statistics       m_stat;

  public:
    /// Standard Constructor
    MooreTestSvc(const std::string& nam, ISvcLocator* svcLoc);
    /// Standard Destructor
    virtual ~MooreTestSvc();

    // IInterface implementation : queryInterface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
    /// IService implementation: initialize the service
    virtual StatusCode initialize();
    /// IService implementation: finalize the service
    virtual StatusCode finalize();
    /// IRunable implementation : Run the class implementation
    virtual StatusCode run();

    /// Interactor callback to handle interrupts
    virtual void handle (const Event& event);

    /// Initialize the statistics
    void init_statistics();
    /// Update the statistics
    void update_statistics();
    /// Print statistics record
    void print_statistics();
    /// Start the logger thread to serve the output
    int startLogger(const std::string&  fifoName);
    /// Map the MBM buffers to extract all necessary information
    StatusCode mapBuffers();
    /// Map single buffer by name if possible
    MBM::Manager* mapBuffer(const std::string& bm_name);
    /// Map the MBM processes to extract all necessary information
    StatusCode mapProcesses();
    /// Static thread callback to collect the messages from the fifo
    static int readMessages(void* param);
  };
}      // End namespace LHCb

#endif /* ONLINE_GAUDIONLINE_MOORETESTSVC_H  */

#include "MBM/Manager.h"
#include "CPP/IocSensor.h"
#include "RTL/ProcessGroup.h"
#include "RTL/strdef.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/DataIncident.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <signal.h>
#include <fcntl.h>
#include <ctime>

#include <iostream>
#include "MBM/bmstruct.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MooreTestSvc)
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MooreFileMonitor)

using namespace std;
using namespace RTL;
using namespace LHCb;
using namespace LHCb::MooreTest;

namespace {
  static std::string command() {
    std::string cmd = ::getenv("GaudiOnlineExe");
    return cmd;
  }

  struct ManagerImp : public MBM::Manager {
    virtual int  optparse (const char*) { return 1; }
    ManagerImp()                        {  }
    virtual ~ManagerImp()               {  }
  };
}

UserStats& UserStats::operator+=(const UserStats& us)  {
  start.time      = us.start.time;
  start.evt_seen += us.start.evt_seen;
  start.evt_prod += us.start.evt_prod;
  
  last.time       = us.last.time;
  last.evt_seen  += us.last.evt_seen;
  last.evt_prod  += us.last.evt_prod;
  
  now.time        = us.now.time;
  now.evt_seen   += us.now.evt_seen;
  now.evt_prod   += us.now.evt_prod;
  return *this;
}

MooreFileMonitor::~MooreFileMonitor()   {
  if ( m_file && m_file != stdout && m_file != stderr ) ::fclose(m_file);
  m_file = 0;
}

StatusCode MooreFileMonitor::initialize()   {
  StatusCode sc = OnlineService::initialize();
  MsgStream msg(msgSvc(),name());
  if ( !sc.isSuccess() )   {
    msg << MSG::ERROR << "Failed to initialize OnlineService  base class." << endmsg;
    return sc;
  }
  if ( !m_output.empty() )  {
    FILE* f = ::fopen(m_output.c_str(),"w");
    if ( 0 == f )  {
      string err = RTL::errorString();
      msg << MSG::ERROR << "Failed to open statitsics output file:" << m_output
	  << "  [" << err << "]" << endmsg;
      return StatusCode::FAILURE;
    }
    m_file = f;
  }
  incidentSvc()->addListener(this,"MOORE_MONITOR");
  return StatusCode::SUCCESS;
}

StatusCode MooreFileMonitor::finalize()   {
  if ( m_file && m_file != stdout && m_file != stderr ) ::fclose(m_file);
  m_file = 0;
  incidentSvc()->removeListener(this);
  return OnlineService::finalize();
}

/// Inform that a new incident has occurred
void MooreFileMonitor::handle(const Incident& incident)   {
  ContextIncident<Statistics*>* inc = (ContextIncident<Statistics*>*)&incident;
  const Statistics& stat = *(inc->tag());

  ::fprintf(m_file,"%-16s %16s %-38s   %-19s   %-38s   %-19s\n",
	    "---------------","-- Time [msec] -",
	    "----------- Events consumed ----------"," --- Rates [Hz] ---",
	    "----------- Events produced ----------"," --- Rates [Hz] ---");
  ::fprintf(m_file,"%-16s %8s %7s %-38s   %-19s   %-38s   %-19s\n",
	    "Process name","Total","Delta",
	    "       Start     Previous          Now"," Instant Cumulative",
	    "       Start     Previous          Now"," Instant Cumulative");
  for(Statistics::const_iterator i=stat.begin(); i!=stat.end(); ++i)    {
    const std::string& name = (*i).first;
    const UserStats& us = (*i).second;
    double inst_time_diff = (double(us.now.time.tv_sec-us.last.time.tv_sec)*1e9  + double(us.now.time.tv_nsec-us.last.time.tv_nsec))/1e9;
    double inst_seen_rate = inst_time_diff < numeric_limits<double>::epsilon() ? 0e0 : double(us.now.evt_seen-us.last.evt_seen)/inst_time_diff;
    double inst_prod_rate = inst_time_diff < numeric_limits<double>::epsilon() ? 0e0 : double(us.now.evt_prod-us.last.evt_prod)/inst_time_diff;
  
    double glob_time_diff = (double(us.now.time.tv_sec-us.start.time.tv_sec)*1e9 + double(us.now.time.tv_nsec-us.start.time.tv_nsec))/1e9;
    double glob_seen_rate = glob_time_diff < numeric_limits<double>::epsilon() ? 0e0 : double(us.now.evt_seen-us.start.evt_seen)/glob_time_diff;
    double glob_prod_rate = glob_time_diff < numeric_limits<double>::epsilon() ? 0e0 : double(us.now.evt_prod-us.start.evt_prod)/glob_time_diff;
    //cout << "Diff:" << glob_time_diff << " " << inst_time_diff << endl;

    const char* p = strchr_safe(name.c_str(),'_');
    const char* q = p ? strchr_safe(p+1,'_') : name.c_str();
    const char* n = q ? q+1 : p ? p+1 : name.c_str();
    ::fprintf(m_file,"%-16s %8ld %7ld %12ld %12ld %12ld %10.0f %10.0f   %12ld %12ld %12ld %10.0f %10.0f\n",n,
	      long(glob_time_diff*1e3),long(inst_time_diff*1e3),
	      us.start.evt_seen, us.last.evt_seen, us.now.evt_seen, inst_seen_rate, glob_seen_rate,
	      us.start.evt_prod, us.last.evt_prod, us.now.evt_prod, inst_prod_rate, glob_prod_rate);
  }
}

/// Standard Constructor
MooreTestSvc::MooreTestSvc(const std::string& nam, ISvcLocator* svcLoc)   
  : OnlineService(nam,svcLoc), m_fifo(-1), m_messagePump(0), m_inputBM(0), m_outputBM(0), m_master(0)
{
  declareProperty("FifoPath",                   m_fifoName    = "/tmp/logSrv.fifo");
  declareProperty("PartitionID",                m_partitionID = 333);
  declareProperty("PartitionName",              m_partition   = "LHCb");
  declareProperty("NumberOfSlaves",             m_numSlaves   = 10);
  declareProperty("PartitionBuffers",           m_partitionBuffers=true);
  declareProperty("Input",                      m_input       = "Events");
  declareProperty("Output",                     m_output      = "Output");
  declareProperty("MonitorProcess",             m_procName    = "_xxxxx_");
  declareProperty("MonitorScript",              m_script      = "");
  declareProperty("MeasuementStartDelay",       m_startDelay  = 15);
  declareProperty("MeasurementIntervall",       m_intervall   = 5);
  declareProperty("MeasurementDuration",        m_duration    = 300);
  declareProperty("MooreOnlineVersion",         m_mooreVsn    = "");
  declareProperty("Auto",                       m_autoRun     = 1);
  declareProperty("MainOptions",                m_mainOpts    = "../options/Main.opts");
  declareProperty("MBMOptions",                 m_mbmOpts     = "../options/Buffers.opts");
  declareProperty("ReaderOptions",              m_rdrOpts     = "../options/Reader.opts");
}

/// Standard Destructor
MooreTestSvc::~MooreTestSvc()   {
}

/// IInterface implementation : queryInterface
StatusCode MooreTestSvc::queryInterface(const InterfaceID& riid, void** ppIf)   {
  if ( IRunable::interfaceID().versionMatch(riid) )  {
    *ppIf = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineService::queryInterface(riid, ppIf);
}

/// IService implementation: initialize the service
StatusCode MooreTestSvc::initialize()   {
  StatusCode sc = OnlineService::initialize();
  if ( !sc.isSuccess() )     {
    return error("+++++ Failed to initialize service base class.");
  }
  m_fifo = startLogger(m_fifoName);
  if ( m_fifo < 0 )  {
    return error("+++++ Failed to open the message fifo:"+m_fifoName);
  }
  info("+++++ Opened logger fifo: %s",m_fifoName.c_str());
  int ret = ::lib_rtl_start_thread(readMessages,this,&m_messagePump);
  if ( !lib_rtl_is_success(ret) ) {
    string err = RTL::errorString();
    return error("Failed to start message pump....  ["+err+"]");
  }
  if ( m_partitionBuffers )  {
    m_input  += "_"+m_partition;
    m_output += "_"+m_partition;
  }
  return StatusCode::SUCCESS;
}

// IService implementation: finalize the service
StatusCode MooreTestSvc::finalize()     {
  if ( m_bmAll )  {
    ::mbmsrv_unmap_global_buffer_info(m_bmAll,false);
    info("+++++ MBM summary unmapped.");
    m_bmAll = 0;
  }
  if ( m_inputBM )  {
    m_inputBM->unmapSections();
    m_inputBM = 0;
  }
  if ( m_outputBM )  {
    m_outputBM->unmapSections();
    m_outputBM = 0;
  }
  if ( m_messagePump )  {
    ::lib_rtl_delete_thread(m_messagePump);
    m_messagePump = 0;
  }
  return OnlineService::finalize();
}

/// Map single buffer by name if possible
MBM::Manager* MooreTestSvc::mapBuffer(const std::string& bm_name)  {
  ManagerImp* bm = new ManagerImp();
  bm->setup(bm_name.c_str());
  int status = bm->mapMonitorSections();
  if ( !lib_rtl_is_success(status) )  {
    delete bm;
    bm = 0;
    return 0;
  }
  return bm;
}

void MooreTestSvc::handle (const Event& e)   {
  switch ( e.eventtype )    {
  case IocEvent:
    switch(e.type)  {
    case MooreTest::CMD_MOORE_EXIT:
      m_autoRun = 2;
      m_master = (Interactor*)e.data;
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

/// Map the MBM buffers to extract all necessary information
StatusCode MooreTestSvc::mapBuffers()   {
  int seconds = 0;
  while ( 1 )  {
    int status = ::mbmsrv_map_global_buffer_info(&m_bmAll,false);
    if(!lib_rtl_is_success(status))   {
      error("+++++ Waiting for Global MBM buffer section....");
      ::lib_rtl_sleep(500);
      continue;
    }
    info("+++++ Global MBM buffer section sucessfully mapped.");
    BUFFERS* buffs = (BUFFERS*)m_bmAll->address;
    try {
      typedef map<string,int> Buffs;
      while (1)   {
	for (int i = 0; i < buffs->p_bmax; ++i)  {
	  if ( buffs->buffers[i].used == 1 )  {
	    const char* bm_name = buffs->buffers[i].name;
	    if ( 0 == m_inputBM && m_input == bm_name )
	      m_inputBM = mapBuffer(m_input);
	    else if ( 0 == m_outputBM && m_output == bm_name )
	      m_outputBM = mapBuffer(m_output);

	    if ( m_inputBM && m_outputBM ) {
	      info("+++++ All buffers are now mapped....continue....");	
	      return StatusCode::SUCCESS;
	    }
	  }
	}
	info("+++++ Waiting for MBM Event buffers being ready.... [%d seconds]",++seconds);
	::lib_rtl_sleep(1000);
      }
    }
    catch(...) {
      info("+++++ Exception during buffer monitoring.");
    }
  }
  return StatusCode::FAILURE;
}

static int countClients(BufferMemory* dsc, const char* match)  {
  int j, count;
  CONTROL *ctr = dsc->ctrl;
  USER *us = dsc->user, *utst=(USER*)~0x0;
  for (j=0, count=0; j<ctr->p_umax; j++,us++ )    {
    if ( us == utst || us == 0         ) break;
    if ( us->block_id != MBM::BID_USER ) continue;
    if ( us->busy     == 0             ) continue;
    if ( ::strstr(us->name,match) )   {
      ++count;
    }
  }
  return count;
}

/// Map the MBM processes to extract all necessary information
StatusCode MooreTestSvc::mapProcesses()   {
  int seconds = 0;
  while(1)  {
    int inputCount  = countClients(m_inputBM->bmid(),m_procName.c_str());
    int outputCount = countClients(m_outputBM->bmid(),m_procName.c_str());
    if ( inputCount == m_numSlaves && outputCount == m_numSlaves )  {
      info("+++++ All Clients of type '"+m_procName+"' are now mapped....continue....");	
      return StatusCode::SUCCESS;
    }
    info("+++++ Waiting for MBM clients of type '%s' being ready.... [%d seconds] Input:%d Output:%d",m_procName.c_str(),++seconds,inputCount,outputCount);
    ::lib_rtl_sleep(1000);
  }
  return StatusCode::FAILURE;
}

/// Initialize the statistics
void MooreTestSvc::init_statistics()    {
  BufferMemory* dsc = m_inputBM->bmid();
  const char* match = m_procName.c_str();
  CONTROL *ctr = dsc->ctrl;
  USER *us = dsc->user, *utst=(USER*)~0x0;
  UserStats empty;
  ::memset(&empty,0,sizeof(empty));
  ::clock_gettime(CLOCK_REALTIME,&empty.start.time);
  empty.now.time = empty.last.time = empty.start.time;

  for (int j=0; j<ctr->p_umax; j++,us++ )    {
    if ( us == utst || us == 0         ) break;
    if ( us->block_id != MBM::BID_USER ) continue;
    if ( us->busy     == 0             ) continue;
    if ( ::strstr(us->name,match) )   {
      bool present = m_stat.find(us->name) != m_stat.end();
      UserStats& ustat = m_stat[us->name];
      if ( !present ) ustat = empty;
      ustat.start.evt_seen = us->ev_seen;
      ustat.last.evt_seen = ustat.start.evt_seen;
      ustat.now.evt_seen = ustat.start.evt_seen;
    }
  }
  dsc = m_outputBM->bmid();
  ctr = dsc->ctrl;
  us = dsc->user;
  for (int j=0; j<ctr->p_umax; j++,us++ )    {
    if ( us == utst || us == 0         ) break;
    if ( us->block_id != MBM::BID_USER ) continue;
    if ( us->busy     == 0             ) continue;
    if ( ::strstr(us->name,match) )   {
      bool present = m_stat.find(us->name) != m_stat.end();
      UserStats& ustat = m_stat[us->name];
      if ( !present ) ustat = empty;
      ustat.start.evt_prod = us->ev_produced;
      ustat.last.evt_prod = ustat.start.evt_prod;
      ustat.now.evt_prod = ustat.start.evt_prod;
    }
  }
}

/// Update the statistics
void MooreTestSvc::update_statistics()   {
  for(Statistics::iterator i=m_stat.begin(); i!=m_stat.end(); ++i)  {
    UserStats& us = (*i).second;
    us.last = us.now;
  }

  BufferMemory* dsc = m_inputBM->bmid();
  const char* match = m_procName.c_str();
  CONTROL *ctr = dsc->ctrl;
  USER *us = dsc->user, *utst=(USER*)~0x0;
  timespec tp;

  ::clock_gettime(CLOCK_REALTIME,&tp);
  for (int j=0; j<ctr->p_umax; j++,us++ )    {
    if ( us == utst || us == 0         ) break;
    if ( us->block_id != MBM::BID_USER ) continue;
    if ( us->busy     == 0             ) continue;
    if ( ::strstr(us->name,match) )   {
      UserStats& ustat = m_stat[us->name];
      ustat.now.evt_seen = us->ev_seen;
      ustat.now.time = tp;
    }
  }
  dsc = m_outputBM->bmid();
  ctr = dsc->ctrl;
  us = dsc->user;
  for (int j=0; j<ctr->p_umax; j++,us++ )    {
    if ( us == utst || us == 0         ) break;
    if ( us->block_id != MBM::BID_USER ) continue;
    if ( us->busy     == 0             ) continue;
    if ( ::strstr(us->name,match) )   {
      UserStats& ustat = m_stat[us->name];
      ustat.now.evt_prod = us->ev_produced;
      ustat.now.time = tp;
    }
  }
}

/// Print statistics record
void MooreTestSvc::print_statistics()    {
  if ( m_stat.size()>0 )  {
    UserStats& sum = m_stat["+++ Grand Total"];
    ::memset(&sum,0,sizeof(sum));
    for(Statistics::iterator i=m_stat.begin(); i!=m_stat.end(); ++i)  {
      UserStats& us = (*i).second;
      sum += us;
    }
    ContextIncident<Statistics*> inc(name(),"MOORE_MONITOR",&m_stat);
    incidentSvc()->fireIncident(inc);
  }
}

/// Start the logger thread to serve the output
int MooreTestSvc::startLogger(const string& fifoName) {
  int fifo = -1;
  if( ::mkfifo(fifoName.c_str(),0666)  ) {
    if ( errno != EEXIST ) { // It is not an error if the file already exists....
      ::lib_rtl_signal_message(LIB_RTL_OS,"Unable to create the fifo: %s.\n",fifoName.c_str());
      return -1;
    }
  }
  ::chmod(fifoName.c_str(),0666);
  if( -1 == (fifo=::open(fifoName.c_str(),O_RDWR|O_NONBLOCK)) ) {
    ::lib_rtl_signal_message(LIB_RTL_OS,"Unable to open the fifo: %s\n",fifoName.c_str());
    return -1;
  }
  if ( -1 == (fcntl(fifo,F_SETFL,fcntl(fifo,F_GETFL)|O_NONBLOCK)) ) {
    ::lib_rtl_signal_message(LIB_RTL_OS,"Unable to set fifo: %s non-blocking\n",fifoName.c_str());
    return -1;
  }
  return fifo;
}

/// Static thread callback to collect the messages from the fifo
int MooreTestSvc::readMessages(void* param) {
  MooreTestSvc* test = (MooreTestSvc*)param;
  char buffer[1024];
  int fd = test->m_fifo;
  test->info("Logger: Starting message pump....");
  while(1) {
    ::lib_rtl_sleep(1);
    int sc = ::read(fd,buffer,sizeof(buffer));
    if ( sc >  0 ) {
      buffer[sc-1] = 0;
      MsgStream msg(test->msgSvc(),"Children");
      msg << MSG::ALWAYS << buffer << endmsg;
    }
    else if ( sc <= 0 && errno == EINTR  ) continue;
    else if ( sc <= 0 && errno == EAGAIN ) continue;
    else {
      test->error("Recv message FAIL");
      return -1;
    }
  }
}

/// IRunable implementation : Run the class implementation
StatusCode MooreTestSvc::run()   {
  string host = RTL::str_upper(RTL::nodeNameShort());
  string dns  = ::getenv("DIM_DNS_NODE") ? ::getenv("DIM_DNS_NODE") : host.c_str();
  string out  = m_fifoName;
  string proc = m_partition+"_"+host+m_procName+"00";
  string main_opts = "-main="+m_mainOpts;
  string mbm_opts  = "-opt="+m_mbmOpts;
  string rdr_opts  = "-opt="+m_rdrOpts;
  char num_slaves[32];
  ::snprintf(num_slaves,sizeof(num_slaves),"%d",m_numSlaves);
  const char *mepinit[] = {"libGaudiOnline.so", "OnlineTask",
			   "-tasktype=LHCb::Class1Task",
			   "-msgsvc=LHCb::FmcMessageSvc",
			   "-auto",
			   main_opts.c_str(),
			   mbm_opts.c_str(),
			   0};
  const char *moore[]   = {m_script.c_str(), 
			   proc.c_str(),
			   m_mooreVsn.c_str(), 
			   dns.c_str(), 
			   m_partition.c_str(), 
			   num_slaves,0};
  const char *aprod[]   = {"libGaudiOnline.so", "OnlineTask", 
			   "-tasktype=LHCb::Class1Task",
			   "-msgsvc=LHCb::FmcMessageSvc",
			   "-auto",
			   main_opts.c_str(),
			   rdr_opts.c_str(),
			   0};

  Process::setDebug(true);  
  Process* mepTask = new Process("MEPInit_0",  command(),mepinit,out.c_str());
  mepTask->start(true);

  /// Wait until the buffers are mapped, then continue
  mapBuffers().ignore();

  /// Start the Moore process(es) using a bash script
  ::lib_rtl_sleep(1000);
  Process* monTask = new Process("Moore_0",   "/bin/bash",moore,out.c_str());
  monTask->start(true);

  /// Wait until all Moore processes are mapped. Then continue
  mapProcesses().ignore();

  info("+++++ Starting producer ...... ");
  Process* prodTask=new Process("Prod_0",command(),aprod,out.c_str());
  prodTask->start(true);

  int i;
  time_t start = time(0);
  if ( m_startDelay > 0 )  {
    for(i=0; i<m_startDelay; ++i)    {
      for ( int j = 0; j<100; ++j )  {
	if ( 2 == m_autoRun ) goto Done;
	::lib_rtl_sleep(10);
      }
      if ( i>0 && (i%10) == 0 )  {
	info("+++++ Warming up .....  %d seconds now.....",i);
      }
    }
  }
  info("+++++ Warming up phase of %d seconds finished. "
       "Starting the measurement cycle for %d seconds.",m_startDelay,m_duration);
  start = time(0);   // Restart timer
  init_statistics();
  for(i = 0; m_autoRun != 2 && i < numeric_limits<int>::max(); ++i)  {
    for ( int j = 0; j<100; ++j )  {
      if ( 2 == m_autoRun ) goto Done;
      ::lib_rtl_sleep(m_intervall*10);
    }
    update_statistics();
    if ( i > 0 )   {
      print_statistics();
    }
    if ( m_autoRun == 1 ) continue;
    else if ( int(time(0) - start) > m_duration ) break;
  }
 Done:
  info("+++++ The measurement time of %d seconds is reached.",int(time(0) - start));
  info("+++++ We are exiting now the measurements cycle.",i);
  prodTask->killall();
  monTask->killall();

  prodTask->wait(Process::WAIT_BLOCK);
  monTask->wait(Process::WAIT_BLOCK);

  /// Wait a bit before kill the buffer manager. Allow to close connections
  ::lib_rtl_sleep(2000);
  mepTask->stopall();
  mepTask->wait(Process::WAIT_BLOCK);

  delete prodTask;
  delete monTask;
  delete mepTask;
  ::lib_rtl_sleep(10);

  system(("rm -f /dev/shm/bm_*"+m_partition).c_str());
  //system((string("rm -f /dev/shm/bm_buffers")).c_str());
  system(("rm -f /tmp/bm_*"+m_partition+"_*").c_str());
  //::kill(0,SIGKILL);
  if ( m_master ) IocSensor::instance().send(m_master,CMD_MOORE_EXIT);
  return StatusCode::SUCCESS;
}
