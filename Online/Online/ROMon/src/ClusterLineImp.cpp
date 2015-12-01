// $Id: FarmDisplayBase.cpp,v 1.51 2010-10-21 06:04:22 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/FarmDisplayBase.cpp,v 1.51 2010-10-21 06:04:22 frankb Exp $

// Framework include files
#include "ROMon/ClusterLine.h"
#include "CPP/Interactor.h"

// C++ include files
#include <set>
#include <limits>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {


  /**@class FarmClusterLine ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display summarizing one single subfarm. Showed as an array on the
   *   main display.
   *
   *   @author M.Frank
   */
  class FarmClusterLine : public ClusterLine  {
    struct Info {
      long free_slots, buf_clients, task_evt_tot, task_evt_min;
      float min_free_slots, min_free_space;
      Info()   {
        free_slots=buf_clients=task_evt_tot=0;
        task_evt_min   = std::numeric_limits<long>::max();
        min_free_slots = std::numeric_limits<float>::max();
        min_free_space = std::numeric_limits<float>::max();
      }
      Info(const Info& c) {
        free_slots     = c.free_slots;
        min_free_slots = c.min_free_slots;
        buf_clients    = c.buf_clients;
        task_evt_tot   = c.task_evt_tot;
        task_evt_min   = c.task_evt_min;
        min_free_space = c.min_free_space;
        min_free_slots = c.min_free_slots;
      }
      Info& operator=(const Info& c) {
        if ( &c != this )  {
          free_slots     = c.free_slots;
          min_free_slots = c.min_free_slots;
          buf_clients    = c.buf_clients;
          task_evt_tot   = c.task_evt_tot;
          task_evt_min   = c.task_evt_min;
          min_free_space = c.min_free_space;
          min_free_slots = c.min_free_slots;
        }
        return *this;
      }
    };
    std::vector<Info> m_history;

    int               m_numUpdate;
    /// Flag to indicate probles with entity
    bool              m_hasProblems;
    /// Flag to indicate if the cluster is in use
    bool              m_inUse;
    /// Timestamp with last update
    time_t            m_lastUpdate;
  public:
    /// Initializing constructor
    FarmClusterLine(FarmLineDisplay* p, const std::string& partition, const std::string& n);
    /// Default destructor
    virtual ~FarmClusterLine() {}
    /// Display function drawing on pasteboard of current display
    void display();
  };

  /**@class HltDeferLine ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display summarizing one single subfarm. Showed as an array on the
   *   main display.
   *
   *   @author M.Frank
   */
  class HltDeferLine : public ClusterLine, public CPP::Interactor  {
    typedef std::set<std::string> StrSet;
    /// Number of nodes in this subfarm
    int                 m_numNodes;
    /// Number of runs in the subfarm
    int                 m_numRuns;
    /// Number of files deferred
    int                 m_numFiles;
    /// Number of updates received
    int                 m_numUpdate;
    /// Flag to indicate probles with entity
    bool                m_hasProblems;
    /// Timestamp with last update
    time_t              m_lastUpdate;
    /// Service ID for excluded nodes
    int                 m_exclID;
    /// Set of excluded nodes
    StrSet              m_excluded;

    /// DIM command service callback
    static void excludedHandler(void* tag, void* address, int* size);
  public:
    /// Initializing constructor
    HltDeferLine(FarmLineDisplay* p, const std::string& partition, const std::string& n);
    /// Default destructor
    virtual ~HltDeferLine();
    /// Display function drawing on pasteboard of current display
    virtual void display();
    /// Collect summary data
    virtual void collect(Summary& summary)  const;
    /// Interactor overload: Display callback handler
    void handle(const CPP::Event& ev);
  };

  /**@class MonitoringClusterLine ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   @author M.Frank
   */
  class MonitoringClusterLine : public ClusterLine  {
    long long int     m_evtRelay;
    long long int     m_evtWorker;
    long long int     m_totRelay;
    long long int     m_totWorker;
    int               m_numUpdate;
    /// Flag to indicate probles with entity
    bool              m_hasProblems;
    /// Flag to indicate if the cluster is in use
    bool              m_inUse;
    /// Name of the relay node withing storage cluster
    std::string       m_relayNode;
    /// Timestamp with last update
    time_t            m_lastUpdate;
  public:
    /// Initializing constructor
    MonitoringClusterLine(FarmLineDisplay* p, const std::string& partition, const std::string& n);
    /// Default destructor
    virtual ~MonitoringClusterLine() {}
    /// Display function drawing on pasteboard of current display
    void display();
  };

  /**@class StorageClusterLine ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   @author M.Frank
   */
  class StorageClusterLine : public ClusterLine   {
    long long int     m_evtRecv;
    long long int     m_evtSent;
    long long int     m_totRecv;
    long long int     m_totSent;
    int               m_numUpdate;
    /// Flag to indicate probles with entity
    bool              m_hasProblems;
    /// Flag to indicate if the cluster is in use
    bool              m_inUse;
    /// Timestamp with last update
    time_t            m_lastUpdate;
  public:
    /// Initializing constructor
    StorageClusterLine(FarmLineDisplay* p, const std::string& partition, const std::string& n);
    /// Default destructor
    virtual ~StorageClusterLine() {}
    /// Display function drawing on pasteboard of current display
    void display();
  };
}

#include "ROMon/TaskSupervisor.h"
#include "ROMon/TaskSupervisorParser.h"

namespace ROMon {

  /**@class CtrlFarmClusterLine ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   @author M.Frank
   */
  class CtrlFarmClusterLine : public ClusterLine, public CPP::Interactor   {
    typedef std::set<std::string> StrSet;
    /// Flag to indicate probles with entity
    bool                m_hasProblems;
    /// Service ID for excluded nodes
    int                 m_exclID;
    /// Extracted cluster information for all contained nodes
    Cluster             m_cluster;
    /// Set of excluded nodes
    StrSet              m_excluded;
    /// Timestamp with last update
    time_t              m_lastUpdate;

    /// DIM command service callback
    void updateContent(XML::TaskSupervisorParser& ts);
    /// DIM command service callback
    static void excludedHandler(void* tag, void* address, int* size);

  public:
    /// Initializing constructor
    CtrlFarmClusterLine(FarmLineDisplay* p, const std::string& partition, const std::string& n);
    /// Default destructor
    virtual ~CtrlFarmClusterLine();
    /// Display function drawing on pasteboard of current display
    void display();
    /// Interactor overload: Display callback handler
    void handle(const CPP::Event& ev);
  };
}

#include "ROMon/FarmLineDisplay.h"
#include "CPP/IocSensor.h"
#include "RTL/Lock.h"
#include "RTL/strdef.h"
#include "WT/wtdef.h"
#include "SCR/scr.h"
#include "ROMonDefs.h"
#include <limits>
extern "C" {
#include "dic.h"
}


using namespace ROMon;
using namespace SCR;
using namespace std;
typedef vector<string>               StringV;

#define TIME_FORMAT            "%Y-%m-%d %H:%M:%S"
#define UPDATE_TIME_MAX        15
#define CLUSTERLINE_START      2
#define COL_ATTENTION       (RED)
#define COL_WARNING         (RED|BOLD)
#define COL_ALARM           (RED|BOLD|INVERSE)
#define COL_OK              (GREEN|INVERSE)
#define INT_max  numeric_limits<long>::max()
#define FLT_max  numeric_limits<float>::max()
namespace ROMon {
  typedef MBMBuffer::Clients           Clients;
  typedef Node::Buffers                Buffers;
  typedef Nodeset::Nodes               Nodes;
}

namespace {
  ClusterLineFactory<MonitoringClusterLine> s_monitoringLineFactory("Monitoring");
  ClusterLineFactory<StorageClusterLine>    s_storageLineFactory("Storage");
  ClusterLineFactory<CtrlFarmClusterLine>   s_ctrlFarmLineFactory("CtrlFarm");
  ClusterLineFactory<FarmClusterLine>       s_farmLineFactory("Farm");
  ClusterLineFactory<HltDeferLine>          s_hltDeferFactory("HLT");
#if 0
  const char* _procNam(const char* nam) {
    const char* p;
    if (0 != ::strstr(nam,"MEPRx") ) return nam;
    p = ::strchr(nam,'_');
    if ( 0 != p ) {
      const char* q = ::strchr(++p,'_');
      if ( 0 != q ) return ++q;
      return p;
    }
    return "Unknown";
  }
#endif
}

/// Standard constructor
CtrlFarmClusterLine::CtrlFarmClusterLine(FarmLineDisplay* p, const string& partition, const std::string& n)
  : ClusterLine(p,partition,n), m_exclID(0)
{
  m_lastUpdate = time(0);
  m_hasProblems = false;
  connect(strupper(m_name)+"/TaskSupervisor/Status");
  string svc   = "HLT/ExcludedNodes/"+strupper(m_name);
  m_exclID = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,excludedHandler,(long)this,0,0);
}

/// Standard destructor
CtrlFarmClusterLine::~CtrlFarmClusterLine() {
  if ( m_exclID ) ::dic_release_service(m_exclID);
  m_exclID = 0;
}

void CtrlFarmClusterLine::display() {
  const char* p = data<char>();
  try {
    m_lastUpdate = time(0);
    XML::TaskSupervisorParser ts;
    if ( ts.parseBuffer(m_name, p,::strlen(p)+1) ) {
      updateContent(ts);
      return;
    }
  }
  catch(...) {
  }
  size_t         pos = position();
  Pasteboard*     pb = m_parent->pasteboard();
  Display*       dis = m_parent->display();
  RTL::Lock lock(InternalDisplay::screenLock());
  ::scrc_begin_pasteboard_update (pb);
  ::scrc_put_chars(dis," Received invalid XML information",COL_ALARM,pos,1,1);
  ::scrc_end_pasteboard_update(pb);
}

/// DIM command service callback
void CtrlFarmClusterLine::updateContent(XML::TaskSupervisorParser& ts) {
  char txt[256];
  string val, err, tim;
  bool cl_good = true;
  Cluster& c = m_cluster;
  Display*       dis = m_parent->display();
  int col = NORMAL, pos = 0, line=position();
  size_t taskCount=0, missTaskCount=0;
  size_t connCount=0, missConnCount=0;
  int pvss_status=0;
  c.nodes.clear();
  ts.getClusterNodes(c);
  tim = c.time.substr(0,c.time.rfind(' '));
  RTL::Lock lock(InternalDisplay::screenLock());

  ::snprintf(txt,sizeof(txt),"%8s %6d ",tim.c_str()+11,int(c.nodes.size()));
  begin_update(txt);
  pos = 87+CLUSTERLINE_START;
  for(Cluster::Nodes::const_iterator i=c.nodes.begin(), e=c.nodes.end(); i!=e;++i) {
    const Cluster::Node& n = (*i).second;
    bool excl = m_excluded.find(n.name) != m_excluded.end();
    bool good = (n.status == "ALIVE");
    for(Cluster::Projects::const_iterator q=n.projects.begin(); q != n.projects.end(); ++q) {
      bool pvss_ok = (*q).eventMgr && (*q).dataMgr && (*q).distMgr;
      if ( pvss_ok && pvss_status<2 ) pvss_status=1;
      else if ( !pvss_ok ) pvss_status = 2;
      good = good && pvss_ok;
    }
    col = good && n.missTaskCount==0 && n.missConnCount==0 ? GREEN|INVERSE : COL_ALARM;
    if ( excl )  {
      col = INVERSE|(col==COL_ALARM ? MAGENTA : BLUE);
    }
    taskCount     += n.taskCount;
    missTaskCount += n.missTaskCount;
    connCount     += n.connCount;
    missConnCount += n.missConnCount;
    val = (n.name == m_name ? " "+n.name : " "+n.name.substr(n.name.length()-2));
    if ( ::strncasecmp(n.name.c_str(),"store",4)==0 ) val = " " + n.name;
    ::scrc_put_chars(dis,val.c_str(),col,line,pos,0);
    pos += val.length();
    cl_good |= (good || excl);
  }
  ::scrc_put_chars(dis," ",GREEN|INVERSE,line,pos,1);

  col = (c.status=="ALIVE" || cl_good) ? NORMAL : (c.status=="MIXED") ? COL_WARNING : COL_ALARM;
  if ( missTaskCount>0 ) {
    ::snprintf(txt,sizeof(txt),"%4d/BAD ",int(missTaskCount));
    ::scrc_put_chars(dis,txt,COL_ALARM,line,28,0);
    ::scrc_put_chars(dis," ",NORMAL,line,37,0);
  }
  else {
    ::snprintf(txt,sizeof(txt),"%4d ",int(taskCount));
    ::scrc_put_chars(dis,txt,GREEN|INVERSE,line,28,0);
    ::scrc_put_chars(dis,"     ",NORMAL,line,33,0);
  }
  if ( missConnCount>0 ) {
    ::snprintf(txt,sizeof(txt),"%4d/BAD ",int(missConnCount));
    ::scrc_put_chars(dis,txt,COL_ALARM,line,37,0);
    ::scrc_put_chars(dis," ",NORMAL,line,46,0);
  }
  else {
    ::snprintf(txt,sizeof(txt),"%4d ",int(connCount));
    ::scrc_put_chars(dis,txt,GREEN|INVERSE,line,37,0);
    ::scrc_put_chars(dis,"     ",NORMAL,line,42,0);
  }
  ::snprintf(txt,sizeof(txt),"  %-6s  ",c.status.c_str());
  ::scrc_put_chars(dis,txt,col|BOLD,line,45,0);
  //bg_black(pb);
  ::scrc_put_chars(dis,pvss_status>1?"ERROR":"   OK",pvss_status>1?COL_ALARM:GREEN,line,53,0);

  col = NORMAL|BOLD;
  struct tm tm1, tm2;
  time_t now = time(0);
  now = ::mktime(localtime_r(&now,&tm2));
  ::strptime(c.time.c_str(),TIME_FORMAT,&tm1);
  if ( now-::mktime(&tm1) > UPDATE_TIME_MAX )
    err = " No update information present ", col = COL_ALARM;
  else if ( pvss_status>1 )
    err = " PVSS environment looks funny.", col = COL_ALARM;
  else if ( !cl_good && c.status == "DEAD" )
    err = " Nodes down - Check.", col = COL_WARNING;
  else if ( c.status == "MIXED" )
    err = " Some nodes down - Check.", col = COL_WARNING;
  else if ( missTaskCount>0 )
    err = " Tasks missing - Check.", col = COL_WARNING;
  else if ( missConnCount>0 )
    err = " Connectivity bad - Check.", col = COL_WARNING;
  else
    err = " No obvious error detected.", col = NORMAL|BOLD|GREEN;
  err = err + "                                ";
  ::scrc_put_chars(dis,err.substr(0,30).c_str(),col,line,57+CLUSTERLINE_START,0);

  end_update();
}

/// Interactor overload: Display callback handler
void CtrlFarmClusterLine::handle(const CPP::Event& ev) {
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
    case CMD_EXCLUDE:
      m_excluded = *auto_ptr<StrSet>(ev.iocPtr<StrSet>()).get();
      return;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

/// DIM command service callback
void CtrlFarmClusterLine::excludedHandler(void* tag, void* address, int* size) {
  if ( tag ) {
    CtrlFarmClusterLine* l = *(CtrlFarmClusterLine**)tag;
    set<string> nodes;
    if ( address && *size > 0 ) {
      char *p = (char*)address, *end = p+*size;
      while(p<end) {
        nodes.insert(strlower(p));
        p += (::strlen(p)+1);
      }
    }
    IocSensor::instance().send(l,CMD_EXCLUDE,new set<string>(nodes));
  }
}

/// Standard constructor
StorageClusterLine::StorageClusterLine(FarmLineDisplay* p, const string& partition, const std::string& n)
  : ClusterLine(p,partition,n)
{
  m_lastUpdate = time(0);
  m_hasProblems = false;
  m_numUpdate = 0;
  connect(strlower(m_name)+"/ROpublish");
}

void StorageClusterLine::display() {
  char txt[256];
  string err = "";
  int col = NORMAL;
  set<string> bad_nodes;
  size_t         pos = position();
  Display*       dis = m_parent->display();
  const Nodeset*   c = data<Nodeset>();

  string evt_buff = std::string("Events_"+m_partition);
  int numNodes = 0, numBuffs = 0, numClients = 0;
  float fsp, fsl, fspace[3] = {FLT_max,FLT_max,FLT_max}, fslots[3] = {FLT_max,FLT_max,FLT_max};
  long long tot_prod[3] = {0,0,0}, min_prod[3] = {INT_max,INT_max,INT_max};
  int       num_cl[3] = {0,0,0}, num_sl[3] = {0,0,0};

  m_inUse = false;
  ++m_numUpdate;
  for (Nodes::const_iterator n=c->nodes.begin(); n!=c->nodes.end(); n=c->nodes.next(n))  {
    bool recv_node = ::strncasecmp((*n).name,"storerecv",8) == 0;
    const Buffers& buffs = *(*n).buffers();
    ++numNodes;
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      const char* bnam = (*ib).name;
      if ( ::strstr(bnam,m_partition.c_str()) != 0 ) {
        int idx = (recv_node && evt_buff == bnam) ? 0 : (evt_buff == bnam) ? 1 : -1;
        if ( idx >= 0 )   {
          const MBMBuffer::Control& ctrl = (*ib).ctrl;
          ++numBuffs;
          tot_prod[idx] += ctrl.tot_produced;
          numClients    += (*ib).clients.size();
          num_cl[idx]   += (*ib).clients.size();
          num_sl[idx]   += ctrl.p_emax - ctrl.i_events;
          min_prod[idx]  = ro_min(min_prod[idx],ctrl.tot_produced);
          fsp = float(ctrl.i_space)/float(ctrl.bm_size);
          fsl = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
          fspace[idx]    = ro_min(fspace[idx],fsp);
          fslots[idx]    = ro_min(fslots[idx],fsl);
          if ( fsl < SLOTS_MIN || fsp < SPACE_MIN ) bad_nodes.insert((*n).name);
          m_inUse = true;
        }
      }
    }
  }

  RTL::Lock lock(InternalDisplay::screenLock());
  time_t now = ::time(0), t1 = numNodes == 0 ? now : c->firstUpdate().first;
  ::strftime(txt,sizeof(txt)," %H:%M:%S ",::localtime(&t1));
  ::snprintf(txt+strlen(txt),sizeof(txt)-strlen(txt),"%6d %6d %6d   ",numNodes,numBuffs,numClients);
  begin_update(txt);

  if ( numNodes != 0 ) {
    m_lastUpdate = t1;
  }

  if ( tot_prod[0] != 0 )
    ::snprintf(txt,sizeof(txt),"%13lld%6d  %8s%4d%17s",tot_prod[0],num_sl[0],"Clients:",num_cl[0],"");
  else
    ::snprintf(txt,sizeof(txt),"%13s%6s  %8s%4s%17s","--","--","","","");
  ::scrc_put_chars(dis," Recv:   ",BOLD,pos,77+CLUSTERLINE_START,0);
  ::scrc_put_chars(dis,txt,NORMAL,pos,77+9+CLUSTERLINE_START,0);

  if ( tot_prod[1] != 0 )
    ::snprintf(txt,sizeof(txt),"%13lld%6d  %8s%4d%17s",tot_prod[1],num_sl[1],"Clients:",num_cl[1],"");
  else
    ::snprintf(txt,sizeof(txt),"%14s%8s  %8s%4s%17s","--","--","","","");
  ::scrc_put_chars(dis,"  Stream:  ",BOLD,pos,77+57+CLUSTERLINE_START,0);
  ::scrc_put_chars(dis,txt,NORMAL,pos,77+67+CLUSTERLINE_START,0);

  m_hasProblems = true;
  if ( now-m_lastUpdate > UPDATE_TIME_MAX ) {
    err = " No update information available", col = RED|INVERSE|BOLD;
  }
  else if ( numNodes == 0 ) {
    err = " No nodes found!", col = BOLD|RED|INVERSE;
  }
  else if ( !m_inUse ) {
    err = " Storage not used yet....", col = NORMAL|INVERSE|GREEN;
  }
  else if ( fslots[0] < SLOTS_MIN || fslots[1] < SLOTS_MIN ) {
    ::snprintf(txt,sizeof(txt)," SLOTS at limit:");
    if ( fslots[0] < SLOTS_MIN ) ::strncat(txt,"Recv ",sizeof(txt)-strlen(txt)-1);
    if ( fslots[1] < SLOTS_MIN ) ::strncat(txt,"Stream ",sizeof(txt)-strlen(txt)-1);
    ::snprintf(txt+strlen(txt),sizeof(txt)-strlen(txt),"[%d nodes]",int(bad_nodes.size()));
    err = txt, col = BOLD|RED|INVERSE;
  }
  else if ( fspace[0] < SPACE_MIN || fspace[1] < SPACE_MIN ) {
    ::strncpy(txt," SPACE at limit:",sizeof(txt));
    if ( fspace[0] < SPACE_MIN ) ::strncat(txt,"Recv ",sizeof(txt)-strlen(txt)-1);
    if ( fspace[1] < SPACE_MIN ) ::strncat(txt,"Stream ",sizeof(txt)-strlen(txt)-1);
    ::snprintf(txt+strlen(txt),sizeof(txt)-strlen(txt),"[%d nodes]",int(bad_nodes.size()));
    err = txt, col = BOLD|RED|INVERSE;
  }
  else if ( min_prod[0] != INT_max && min_prod[0]>0 && min_prod[0] <= m_evtRecv ) {
    err = " Receiving layer idle.", col = RED|BOLD;
  }
  else if ( min_prod[1] != INT_max && min_prod[1]>0 && min_prod[1] <= m_evtSent ) {
    err = " Streaming layer idle.", col = RED|BOLD;
  }
  else {
    err = " No obvious Errors detected....", col = NORMAL|INVERSE|GREEN;
  }

  err = err + "                                                                 ";
  ::scrc_put_chars(dis,err.substr(0,35).c_str(),col,pos,42+CLUSTERLINE_START,0);
  m_evtRecv = min_prod[0];
  m_evtSent = min_prod[1];
  m_totRecv = tot_prod[0];
  m_totSent = tot_prod[1];
  end_update();
}


/// Standard constructor
MonitoringClusterLine::MonitoringClusterLine(FarmLineDisplay* p, const string& partition, const std::string& n)
  : ClusterLine(p,partition,n)
{
  m_numUpdate = 0;
  m_evtRelay  = m_totRelay = 0;
  m_evtWorker = m_totWorker = 0;
  m_relayNode = m_name+"01";
  m_lastUpdate = time(0);
  m_hasProblems = false;
  connect(strlower(m_name)+"/ROpublish");
}


void MonitoringClusterLine::display() {
  char txt[256];
  string err = "";
  set<string> bad_nodes;
  size_t         pos = position();
  Display*       dis = m_parent->display();
  const Nodeset*   c = data<Nodeset>();
  const Nodes& nodes = c->nodes;
  string evt_buff = "Events_"+m_partition;
  string out_buff = "Output_"+m_partition;
  int numNodes = 0, numBuffs = 0, numClients = 0;
  int  num_cl[3] = {0,0,0}, num_sl[3] = {0,0,0};
  long long tot_prod[3] = {0,0,0}, min_prod[3] = {INT_max,INT_max,INT_max};
  float fsp, fspace[3] = {FLT_max,FLT_max,FLT_max};
  float fsl, fslots[3] = {FLT_max,FLT_max,FLT_max};

  m_inUse = false;
  for (Nodes::const_iterator n=nodes.begin(); n!=nodes.end(); n=nodes.next(n))  {
    bool relay = ::strncasecmp((*n).name,m_relayNode.c_str(),m_relayNode.length()) == 0;
    const Buffers& buffs = *(*n).buffers();
    ++numNodes;
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      const char* bnam = (*ib).name;
      if ( ::strstr(bnam,m_partition.c_str()) != 0 ) {
        int idx = -1;
        if ( relay && evt_buff == bnam )
          idx = 0;
        else if ( !relay && evt_buff == bnam )
          idx = 1;
        else if ( !relay && out_buff == bnam )
          idx = 2;
        if ( idx >= 0 ) {
          const MBMBuffer::Control& ctrl = (*ib).ctrl;
          int ncl = (*ib).clients.size();
          numClients += ncl;
          ++numBuffs;
          tot_prod[idx] += ctrl.tot_produced;
          num_cl[idx]   += ncl;
          num_sl[idx]   += ctrl.p_emax - ctrl.i_events;
          min_prod[idx]  = ro_min(min_prod[idx],ctrl.tot_produced);
          fsl            = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
          fsp            = float(ctrl.i_space)/float(ctrl.bm_size);
          fspace[idx]    = ro_min(fspace[idx],fsp);
          fslots[idx]    = ro_min(fslots[idx],fsl);
          if ( fsl < SLOTS_MIN || fsp < SPACE_MIN ) {
            bad_nodes.insert((*n).name);
          }
          m_inUse = true;
        }
      }
    }
  }

  RTL::Lock lock(InternalDisplay::screenLock());
  time_t now = time(0), t1 = numNodes == 0 ? time(0) : c->firstUpdate().first;
  ::strftime(txt,sizeof(txt)," %H:%M:%S ",::localtime(&t1));
  ::snprintf(txt+::strlen(txt),sizeof(txt)-strlen(txt),"%6d %6d %6d   ",numNodes,numBuffs,numClients);
  begin_update(txt);

  if ( numNodes != 0 ) {
    m_lastUpdate = t1;
  }
  m_hasProblems = true;

  if ( tot_prod[0] != 0 )
    ::snprintf(txt,sizeof(txt),"%13lld%6d  %8s%4d %64s",tot_prod[0],num_sl[0],"Clients:",num_cl[0],"");
  else
    ::snprintf(txt,sizeof(txt),"%13s%6s  %8s%4s %64s","--","--","","","");
  ::scrc_put_chars(dis," Relay: ",BOLD,pos,77+CLUSTERLINE_START,0);
  ::scrc_put_chars(dis,txt,NORMAL,pos,77+9+CLUSTERLINE_START,0);

  if ( tot_prod[1] != 0 && tot_prod[2] != 0 )
    ::snprintf(txt,sizeof(txt),"%13lld%6d  %8s%4d %30s %10lld%6d %8s%4d %64s",
               tot_prod[1],num_sl[1],"Clients:",num_cl[1],"",
               tot_prod[2],num_sl[2],"Clients:",num_cl[2],"");
  else if ( tot_prod[1] != 0 )
    ::snprintf(txt,sizeof(txt),"%13lld%6d  %8s%4d  %10s %-62s",tot_prod[1],num_sl[1],
               "Clients:",num_cl[1],"","No Output streams");
  else
    ::snprintf(txt,sizeof(txt),"%13s%6s  %8s%4s%64s","--","--","","","");
  ::scrc_put_chars(dis,"  Worker:  ",BOLD,pos,77+57+CLUSTERLINE_START,0);
  ::scrc_put_chars(dis,txt,NORMAL,pos,77+67+CLUSTERLINE_START,0);

  int col = NORMAL;
  if ( now-m_lastUpdate > UPDATE_TIME_MAX ) {
    err = " No update information available", col = RED|INVERSE|BOLD;
  }
  else if ( numNodes == 0 ) {
    err = " No nodes found in this cluster!", col = BOLD|RED|INVERSE;
  }
  else if ( !m_inUse ) {
    err = " Monitoring cluster not used yet....", col = NORMAL|INVERSE|GREEN;
  }
  else if ( fslots[0] < SLOTS_MIN || fslots[1] < SLOTS_MIN || fslots[2] < SLOTS_MIN ) {
    ::snprintf(txt,sizeof(txt)," SLOTS at limit:");
    if ( fslots[0] < SLOTS_MIN ) ::strncat(txt,"Relay ", sizeof(txt)-strlen(txt)-1);
    if ( fslots[1] < SLOTS_MIN ) ::strncat(txt,"Events ",sizeof(txt)-strlen(txt)-1);
    if ( fslots[2] < SLOTS_MIN ) ::strncat(txt,"Output ",sizeof(txt)-strlen(txt)-1);
    ::snprintf(txt+strlen(txt),sizeof(txt)-strlen(txt),"[%d nodes]",int(bad_nodes.size()));
    err = txt, col = BOLD|RED|INVERSE;
  }
  else if ( fspace[0] < SPACE_MIN || fspace[1] < SPACE_MIN || fspace[2] < SPACE_MIN  ) {
    ::snprintf(txt,sizeof(txt)," SPACE at limit:");
    if ( fspace[0] < SPACE_MIN ) ::strncat(txt,"Relay ", sizeof(txt)-strlen(txt)-1);
    if ( fspace[1] < SPACE_MIN ) ::strncat(txt,"Events ",sizeof(txt)-strlen(txt)-1);
    if ( fspace[2] < SPACE_MIN ) ::strncat(txt,"Output ",sizeof(txt)-strlen(txt)-1);
    ::snprintf(txt+strlen(txt),sizeof(txt)-strlen(txt),"[%d nodes]",int(bad_nodes.size()));
    err = txt, col = BOLD|RED|INVERSE;
  }
  else if ( tot_prod[0]>0 && tot_prod[0] <= m_totRelay ) {
    err = " No RELAY activity visible.", col = BOLD|RED;
  }
  else if ( tot_prod[1]>0 && tot_prod[1] <= m_totWorker ) {
    err = " No Worker activity visible.", col = BOLD|RED;
  }
  else if ( min_prod[0] != INT_max && min_prod[0]>0 && min_prod[0] <= m_evtRelay ) {
    err = " Relay node looks idle.", col = BOLD|RED;
  }
  else if ( min_prod[1] != INT_max && min_prod[1]>0 && min_prod[1] <= m_evtWorker ) {
    err = " Some workers idle around.", col = BOLD|RED;
  }
  else  {
    err = " No obvious Errors detected....", col = NORMAL|INVERSE|GREEN;
  }
  err = err + "                                                                 ";
  ::scrc_put_chars(dis,err.substr(0,35).c_str(),col,pos,42+CLUSTERLINE_START,0);

  m_evtRelay  = min_prod[0];
  m_totRelay  = tot_prod[0];
  m_evtWorker = min_prod[1];
  m_totWorker = tot_prod[1];

  end_update();
}


namespace {
}

FarmClusterLine::FarmClusterLine(FarmLineDisplay* p, const string& partition, const std::string& n)
  : ClusterLine(p,partition,n)
{
  m_numUpdate  = 0;
  m_lastUpdate = time(0);
  m_hasProblems = false;
  connect(strlower(m_name)+"/ROpublish");
}

void FarmClusterLine::display() {
  char txt[256];
  size_t pos = position();
  Display*       dis = m_parent->display();
  const Nodeset*   c = data<Nodeset>();
  const Nodes& nodes = c->nodes;
  int numNodes=0, numBuffs=0, numClients=0, col=NORMAL;
  bool slots_min = false, space_min = false;
  set<string> bad_nodes;
  map<string,Info> vals;
  Info totals;
  bool partitioned = !(m_partition.empty() || m_partition=="*" || RTL::str_upper(m_partition)=="ALL");

  m_inUse = false;
  for (Nodes::const_iterator n=nodes.begin(); n!=nodes.end(); n=nodes.next(n))  {
    const Buffers& buffs = *(*n).buffers();
    const char* nn = (*n).name;
    Info* info;
    txt[1] = nn[0];
    txt[2] = nn[1];
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      if ( !(!partitioned || ro_match_end(m_partition,(*ib).name)) ) continue;
      string b = (*ib).name, bnam = b.substr(0,partitioned ? b.find('_') : string::npos);
      const MBMBuffer::Control& ctrl = (*ib).ctrl;
      float fsp = float(ctrl.i_space)/float(ctrl.bm_size);
      float fsl = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
      long min_task  = numeric_limits<long>::max();
      map<string,Info>::iterator  i_info = vals.find(bnam);
      if ( i_info == vals.end() ) i_info = vals.insert(make_pair(bnam,Info())).first;

      m_inUse = true;
      info = &((*i_info).second);
      info->free_slots     += (ctrl.p_emax-ctrl.i_events);
      info->buf_clients    += ctrl.i_users;
      info->min_free_slots  = min(info->min_free_slots,fsl);
      info->min_free_space  = min(info->min_free_slots,fsp);
      totals.free_slots    += (ctrl.p_emax-ctrl.i_events);
      totals.buf_clients   += ctrl.i_users;
      totals.min_free_slots = min(totals.min_free_slots,fsl);
      totals.min_free_space = min(totals.min_free_slots,fsp);
      if ( fsl < SLOTS_MIN || fsp < SPACE_MIN ) {
        bad_nodes.insert(nn);
      }
      if ( fsl < SLOTS_MIN ) slots_min |= (fsl<SLOTS_MIN);
      if ( fsp < SPACE_MIN ) space_min |= (fsp<SPACE_MIN);
      const Clients& clients = (*ib).clients;
      for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
        if ( 0 == ::strncasecmp((*ic).name,"MEPRx",5) ) continue;
        info->task_evt_tot   += (*ic).events;
        info->task_evt_min    = ro_min(info->task_evt_min,(*ic).events);
        totals.task_evt_tot  += (*ic).events;
        totals.task_evt_min   = ro_min(totals.task_evt_min,(*ic).events);
        if ( (*ic).events < min_task ) min_task = (*ic).events;
        ++numClients;
      }
#if 0
      if ( (m_measure%5)==0 ) {
        info->stuck = min_task <= info->task_evt_min ? 1 : 0;
        (*ibuf).second.first  = min_task;
      }
#endif
      ++numBuffs;
    }
    ++numNodes;
  }

  RTL::Lock lock(InternalDisplay::screenLock());
  time_t t1 = numNodes == 0 ? time(0) : c->firstUpdate().first, prev_update=m_lastUpdate;
  ::strftime(txt,sizeof(txt)," %H:%M:%S ",::localtime(&t1));
  ::snprintf(txt+::strlen(txt),sizeof(txt)-strlen(txt),"%6d %6d %6d   ",numNodes,numBuffs,numClients);
  begin_update(txt);

  if ( numNodes != 0 ) {
    m_lastUpdate = t1;
  }
  m_hasProblems = true;
  while( m_history.size()<5 ) m_history.push_back(totals);

  col = NORMAL;
  string err = "", tag = "";
  const Info& history = m_history[4];
  if ( prev_update-m_lastUpdate > UPDATE_TIME_MAX ) {
    err = " No update information available", col = RED|INVERSE|BOLD;
  }
  else if ( numNodes == 0 ) {
    err = " No nodes found in this subfarm!", col =  RED|INVERSE|BOLD;
  }
  else if ( !m_inUse ) {
    err = " Subfarm not used by any partition....", col = NORMAL|INVERSE|GREEN;
  }
  else if ( totals.task_evt_min <= history.task_evt_min && totals.task_evt_tot == history.task_evt_tot )  {
    err = " No DAQ activity visible.", col = BOLD|RED;
    tag = "IDLE";
  }
  else if ( totals.task_evt_min <= history.task_evt_min ) {
    err = " Some Task(s) stuck.", col = BOLD|RED|INVERSE;
    tag = "STUCK";
  }
  else if ( slots_min  ) {
    int nbad = int(bad_nodes.size());
    ::snprintf(txt,sizeof(txt)," SLOTS at limit [%d nodes]",nbad);
    err = txt, col = INVERSE|(nbad>0 ? GREEN : RED);
  }
  else if ( space_min  ) {
    int nbad = int(bad_nodes.size());
    ::snprintf(txt,sizeof(txt)," SPACE at limit [%d nodes]",nbad);
    err = txt, col = INVERSE|(nbad>0 ? GREEN : RED);
  }
  else  {
    err = " No obvious Errors detected....", col=NORMAL|INVERSE|GREEN;
    m_hasProblems = false;
  }
  err = err + "                                                                 ";
  ::scrc_put_chars(dis,err.substr(0,35).c_str(),col,pos,42+CLUSTERLINE_START,0);
  txt[0] = 0;
  int sp = 77+CLUSTERLINE_START, nam_len = partitioned ? 10 : 15;
  for(map<string,Info>::const_iterator i=vals.begin(); i!=vals.end(); ++i) {
    const Info& info = (*i).second;
    ::snprintf(txt,sizeof(txt)," %-30s",((*i).first+":").c_str());
    ::scrc_put_chars(dis,txt,NORMAL|BOLD,pos,sp,0);
    sp += nam_len;
    if ( info.min_free_slots < SLOTS_MIN )
      col=BOLD|RED, ::snprintf(txt,sizeof(txt),"%12ld %-20s ",info.task_evt_tot,"SLOTS");
    else if ( info.min_free_space < SPACE_MIN )
      col=BOLD|RED, ::snprintf(txt,sizeof(txt),"%12ld %-20sss ",info.task_evt_tot,"SPACE");
    else if ( info.task_evt_min <= history.task_evt_min && totals.task_evt_tot == history.task_evt_tot )
      col=NORMAL,   ::snprintf(txt,sizeof(txt),"%12ld %5ld %15s ",info.task_evt_tot,info.free_slots,"");
    else if ( info.task_evt_min <= history.task_evt_min )
      col=BOLD|RED, ::snprintf(txt,sizeof(txt),"%12ld %-20s ",info.task_evt_tot,tag.empty() ? "IDLE" : tag.c_str());
    else if ( info.task_evt_tot > 0 )
      col=NORMAL,   ::snprintf(txt,sizeof(txt),"%12ld %5ld %15s",info.task_evt_tot,info.free_slots,"");
    else
      col=NORMAL,   ::snprintf(txt,sizeof(txt),"%12s %5s %15s","--","--","");

    ::scrc_put_chars(dis,txt,col,pos,sp,0);
    sp += 19;
  }
  ::scrc_put_chars(dis,"",NORMAL,pos,sp,1);
#if 0
  if ( min_prod[0] != INT_max || min_prod[1] != INT_max ) {
    if ( min_prod[3]  == INT_max ) min_prod[3] = 0;  // if not existing....
    if ( min_slots[3] == INT_max ) min_slots[3] = 0; // if not existing....
    ::snprintf(txt,sizeof(txt),"%10lld%5ld%11lld%6ld%10lld%5ld",
               min_prod[3],min_slots[3],
               min_prod[1],min_slots[1],
               min_prod[2],min_slots[2]);
  }
  else {
    ::snprintf(txt,sizeof(txt),"%10s%5s%10s%7s%10s%5s","--","--","--","--","--","--");
  }
  ::scrc_put_chars(dis,txt,NORMAL,pos,77+47+CLUSTERLINE_START,0);
#endif
  m_history[4] = m_history[3];
  m_history[3] = m_history[2];
  m_history[2] = m_history[1];
  m_history[1] = m_history[0];
  m_history[0] = totals;
  end_update();
}

#include "ROMon/CPUMon.h"

HltDeferLine::HltDeferLine(FarmLineDisplay* p, const string& partition, const std::string& n)
  : ClusterLine(p,partition,n)
{
  m_numUpdate   = 0;
  m_numFiles    = 0;
  m_numRuns     = 0;
  m_numNodes    = 0;
  m_lastUpdate  = time(0);
  m_hasProblems = false;
  connect(strlower(m_name)+"/ROpublish/HLT1");
  //connect(strlower(m_name)+"/ROpublish/HLTDefer");
  string svc = "HLT/ExcludedNodes/"+strupper(m_name);
  m_exclID   = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,excludedHandler,(long)this,0,0);
}

HltDeferLine::~HltDeferLine() {
  if ( m_exclID ) ::dic_release_service(m_exclID);
  m_exclID = 0;
}

/// Interactor overload: Display callback handler
void HltDeferLine::handle(const CPP::Event& ev) {
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
    case CMD_EXCLUDE:
      m_excluded = *auto_ptr<StrSet>(ev.iocPtr<StrSet>()).get();
      if ( 0 == m_numUpdate ) {
        RTL::Lock lock(InternalDisplay::screenLock());
        begin_update("          -- No Subfarm information retrieved yet --");
        end_update();
      }
      return;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

/// DIM command service callback
void HltDeferLine::excludedHandler(void* tag, void* address, int* size) {
  if ( tag ) {
    HltDeferLine* l = *(HltDeferLine**)tag;
    set<string> nodes;
    if ( address && *size > 0 ) {
      char *p = (char*)address, *end = p+*size;
      while(p<end) {
        nodes.insert(strlower(p));
        p += (::strlen(p)+1);
      }
    }
    IocSensor::instance().send(l,CMD_EXCLUDE,new set<string>(nodes));
  }
}

/// Collect summary data
void HltDeferLine::collect(Summary& summary)  const {
  if ( summary.size() == 0 ) {
    summary.push_back(make_pair("Nodes",0));
    summary.push_back(make_pair("Runs", 0));
    summary.push_back(make_pair("Files",0));
  }
  summary[0].second += m_numNodes;
  summary[2].second += m_numFiles;
  if ( m_numRuns > summary[1].second )
    summary[1].second = m_numRuns;
}

void HltDeferLine::display() {
  typedef DeferredHLTSubfarmStats HLTStats;
  typedef HLTStats::Nodes Nodes;
  typedef HLTStats::Runs Runs;
  char txt[256];
  size_t pos = 66, last_pos = pos, numNodes = 0, numRuns = 0, numFiles = 0;
  Display*       dis = m_parent->display();
  const HLTStats*  s = data<HLTStats>();
  const Runs&   runs = s->runs;
  const Nodes* nodes = s->nodes();
  int col = NORMAL, line = position();
  string val;

  ++m_numUpdate;
  for (Runs::const_iterator ri=runs.begin(); ri!=runs.end(); ri=runs.next(ri))  {
    numFiles += (*ri).second;
    ++numRuns;
  }
  for (Nodes::const_iterator ni=nodes->begin(); ni!=nodes->end(); ni=nodes->next(ni))  {
    ++numNodes;
  }
  m_numNodes = numNodes;
  m_numRuns  = numRuns;
  m_numFiles = numFiles;
  RTL::Lock lock(InternalDisplay::screenLock());
  time_t t1 = numNodes == 0 ? time(0) : s->firstUpdate().first;
  ::strftime(txt,sizeof(txt)," %H:%M:%S ",::localtime(&t1));
  ::snprintf(txt+strlen(txt),sizeof(txt)-strlen(txt),"%6ld %6ld %6ld ",long(numNodes),long(numRuns),long(numFiles));
  begin_update(txt);

  if ( numNodes != 0 ) m_lastUpdate = t1;
  bool valid_data = (time(0) - t1) < 60;
  if ( !valid_data )
    ::scrc_put_chars(dis,"  No update > 60s ",COL_ALARM  ,line,46,0);
  else if ( numRuns>0 )
    ::scrc_put_chars(dis,"  Processing HLT  ",COL_WARNING,line,46,0);
  else
    ::scrc_put_chars(dis,"       DONE       ",COL_OK,line,46,0);
  val = " "+string(s->name)+" ";
  ::scrc_put_chars(dis,val.c_str(),numRuns>0 ? COL_WARNING : COL_OK,line,pos,0);
  pos += val.length();
  ::scrc_put_chars(dis,"  ",NORMAL,line,pos,0);
  pos += 2;
  ::scrc_put_chars(dis," ",INVERSE|BOLD|YELLOW,line,pos,1);
  last_pos = pos;
  if ( valid_data && nodes->size() > 0 ) {
    for (Nodes::const_iterator ni=nodes->begin(); ni!=nodes->end(); ni=nodes->next(ni))  {
      const DeferredHLTStats& n = *ni;
      string nn = n.name;
      if ( nn.length() > 0 && nn != s->name ) {
        const Runs& nr = n.runs;
        bool excl = m_excluded.find(nn) != m_excluded.end();
        col = nr.size()==0 ? COL_OK : COL_ATTENTION;
        if ( excl )
          col = INVERSE|(col != COL_OK ? MAGENTA : BLUE);
        else if ( n.overflowState == 'Y' && nr.size()>0 )
          col = COL_WARNING;
        else if ( n.overflowState == 'Y' )
          col = COL_OK|BOLD;
        nn = nn.substr(nn.length()-2);
        int n_pos = ::atoi(nn.c_str())-1;
        val = " "+nn;
        ::scrc_put_chars(dis,val.c_str(),col,line,pos+n_pos*3,0);
        if ( last_pos < pos+(1+n_pos)*3 ) last_pos = pos+(1+n_pos)*3;
      }
    }
  }
  ::scrc_put_chars(dis," ",COL_OK,line,last_pos,1);
  end_update();
  IocSensor::instance().send(m_parent,CMD_SHOWDEFERREDRUNS,this);
}

