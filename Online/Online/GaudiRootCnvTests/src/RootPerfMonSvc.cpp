// $Id: RootPerfMonSvc.cpp,v 1.12 2010-09-27 15:43:53 frankb Exp $
//====================================================================
//  RootPerfMonSvc implementation
//--------------------------------------------------------------------
//
//  Description: Implementation of the ROOT data storage
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/DataIncident.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/System.h"
#include "GaudiUtils/IIODataManager.h"
#include "RootPerfMonSvc.h"
#include "RootCnv/RootDataConnection.h"
#include "Sys.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TTreePerfStats.h"

using namespace std;
using namespace Gaudi;
typedef const string& CSTR;

#define S_OK   StatusCode::SUCCESS
#define S_FAIL StatusCode::FAILURE

namespace {
  /// Release an interface pointer
  template<class T> inline void releasePtr(T*& p)  {
    if ( 0 != p )    {
      p->release();
      p = 0;
    }
  }
  /// Delete ponter and set variable to NULL
  template<class T> inline void deletePtr(T*& p)  {
    if ( 0 != p )    {
      delete p;
      p = 0;
    }
  }
}

namespace Gaudi {
  struct PerfMonCounters {
    int    evtCount;
    float  userTime;
    float  sysTime;
    float  sleepAvg;
    float  rss;
    float  vmsize;
    PerfMonCounters();
  };
}

static const char* clean_root_name(const string& n) {
  return n.c_str();
}

PerfMonCounters::PerfMonCounters() {
  ::memset(this,0,sizeof(PerfMonCounters));
}

/// Standard constructor
RootPerfMonSvc::RootPerfMonSvc(CSTR nam, ISvcLocator* svc)
  : Service( nam, svc), m_collectInterval(100),
    m_incidentSvc(0), m_ioMgr(0), m_log(0), m_output(0), m_countersTree(0), m_counters(0), 
    m_treeStatDir(0)
{
  declareProperty("IOPerfStats",     m_ioPerfStats);
  declareProperty("Output",          m_outputName = "RootStats.root");
  declareProperty("CollectInterval", m_collectInterval);
  m_beginEvent = new PerfMonCounters();
  m_endEvent   = new PerfMonCounters();
  m_beginProc  = new PerfMonCounters();
  m_endProc    = new PerfMonCounters();
  m_beginInput = new PerfMonCounters();
  m_endInput   = new PerfMonCounters();
}

/// Standard destructor
RootPerfMonSvc::~RootPerfMonSvc() {
  delete m_beginEvent;
  delete m_endEvent;
  delete m_beginProc;
  delete m_endProc;
  delete m_beginInput;
  delete m_endInput;
}

/// IInterface implementation : queryInterface
StatusCode RootPerfMonSvc::queryInterface(const InterfaceID& riid, void** ppIf)   {
  if ( IIncidentListener::interfaceID().versionMatch(riid) ) {
    *ppIf = (IIncidentListener*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(riid, ppIf);
}

/// Small routine to issue exceptions
StatusCode RootPerfMonSvc::error(CSTR msg)  {
  if ( m_log ) {
    log() << MSG::ERROR << "Error: " << msg << endmsg;
    return S_FAIL;
  }
  MsgStream m(msgSvc(),name());
  m << MSG::ERROR << "Error: " << msg << endmsg;
  return S_FAIL;
}

// Initialize the Db data persistency service
StatusCode RootPerfMonSvc::initialize()  {
  string cname;
  struct timezone tz;
  StatusCode status = Service::initialize();
  if ( !status.isSuccess() )
    return error("Failed to initialize Service base class.");
  m_log = new MsgStream(msgSvc(),name());
  if( !(status=service("IncidentSvc", m_incidentSvc)).isSuccess() )
    return error("Unable to localize interface from service:IncidentSvc");
  if( !(status=service("IIODataManager", m_ioMgr)).isSuccess() )
    return error("Unable to localize interface from service:IIODataManager");


  m_incidentSvc->addListener(this,"BeginEvent");
  m_incidentSvc->addListener(this,"EndEvent");
  m_incidentSvc->addListener(this,"BeginProcessing");
  m_incidentSvc->addListener(this,"EndProcessing");
  m_incidentSvc->addListener(this,"BeginInputFile");
  m_incidentSvc->addListener(this,"EndInputFile");

  // now open the output file for the results:
  TDirectory::TContext dirCtxt(0);
  m_output = new TFile(m_outputName.c_str(),"RECREATE","ROOT I/O statistics for Gaudi",3);
  m_output->cd();
  //m_counters = new TTree(...);
  m_treeStatDir = m_output->mkdir("IOPerfStats");  
  ::gettimeofday(&m_startTime,&tz);
  m_evtCount = 0;
  return S_OK;
}

// Finalize the Db data persistency service
StatusCode RootPerfMonSvc::finalize()    {
  log() << MSG::INFO;
  releasePtr(m_ioMgr);
  m_incidentSvc->removeListener(this);
  releasePtr(m_incidentSvc);
  if ( m_output ) {
    TDirectory::TContext dirCtxt(m_output);
    if ( m_counters )  {
      m_counters->Write();
      m_countersTree->Write();
      delete m_counters;
    }
    for(RootTreeStats::iterator i = m_rdFiles.begin(); i!=m_rdFiles.end();++i)
      saveTreeStatistics((*i).second);
    m_output->Close();
    deletePtr(m_output);
    m_treeStatDir = 0;
  }
  deletePtr(m_log);
  return Service::finalize();
}

/// Save TTree I/O statistics to output file for given dataset
void RootPerfMonSvc::saveTreeStatistics(const string& dataset) {
  RootTreeStats::iterator i = m_rdFiles.find(dataset);
  if ( i != m_rdFiles.end() ) {
    saveTreeStatistics((*i).second);
    return;
  }
  error("Tree statistics for dataset "+dataset+" cannot be saved. [dataset not found]");
}

/// Save TTree I/O statistics to output file
void RootPerfMonSvc::saveTreeStatistics(TTreePerfStats* s) {
  TDirectory::TContext dirCtxt(0);
  s->Finish();
  m_treeStatDir->cd();
  s->Write(s->GetName());
}

/// Create TTree I/O statistics to output file for given dataset
void RootPerfMonSvc::createTreeStatistics(const string& dataset) {
  RootTreeStats::iterator i = m_rdFiles.find(dataset);
  RootDataConnection* c = dynamic_cast<RootDataConnection*>(m_ioMgr->connection(dataset));
  if ( c )   {
    if ( i == m_rdFiles.end() ) {
      TTree* t = c->getSection("Event",false);
      TTreePerfStats* s = new TTreePerfStats(clean_root_name(dataset),t);
      m_rdFiles.insert(make_pair(dataset,s));
      return;
    }
    error("The dataset "+dataset+" is already an input file. No new Tree I/O statistics created.");
    return;
  }
  error("No data connection found for "+dataset+". No new Tree I/O statistics created.");
}

/// Incident handler implemenentation: Inform that a new incident has occured
void RootPerfMonSvc::handle(const Incident& inc)    {
  log() << MSG::INFO << "Got incident:" << inc.source() << " of type " << inc.type() << endmsg;
  const string& s = inc.type();
  switch(s[0]) {
  case 'B':
    switch(s[5]) {
    case 'I':  // BeginInputFile
      createTreeStatistics(inc.source());
    case 'P':  // BeginProcessing
      ++m_evtCount;
      collectCounters(inc,*m_beginInput);
      break;
    case 'E':  // BeginEvent
      collectCounters(inc,*m_beginInput);
      break;
    default:
      break;
    }
    return;
  case 'E':
    switch(s[3]) {
    case 'I':  // EndInputFile
      saveTreeStatistics(inc.source());
    case 'P':  // EndProcessing
    case 'E':  // EndEvent
      collectCounters(inc,*m_endInput);
      break;
    default:
      break;
    }
    return;
  default:
    return;
  }
}

/// Collect system counters following a given incident
void RootPerfMonSvc::collectCounters(const Incident& /* incident */, PerfMonCounters& ) {
  /*
    Write speed (CPU + Wall)   [CPU efficiency]
    Read speed  (CPU + Wall)   [CPU efficiency]
    100%, 10 %, 1 % ­ event access
    100% ­ 1 % object access per event
    Access patterns
    No 2 analysis programs are the same
    - no exactone-fits-all solution
    - only fuzzy approaches
    Memory consumption
    File size
    Compression factors of individual branches
  */
  ROMon::SysProcess    sysProc;
  ROMon::StatusProcess statusProc;
  int pid = getpid();
  if ( sysProc.read(pid)  && statusProc.read(pid) ) {
#if 0
    // Collect here the relvant variables to be traced
    c.rss = sysProc.rss;
    c.vmsize = sysProc.vmsize;
    c.userTime = 0;
    c.sysTime =    0;
    c.sleepAvg = 0;
    c.evtCount = m_evtCount;
#endif
  }
}
