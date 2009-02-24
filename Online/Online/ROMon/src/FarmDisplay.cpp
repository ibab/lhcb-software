// $Id: FarmDisplay.cpp,v 1.37 2009-02-24 10:38:20 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/FarmDisplay.cpp,v 1.37 2009-02-24 10:38:20 frankb Exp $

#include "ROMon/CtrlSubfarmDisplay.h"
#include "ROMon/RecSubfarmDisplay.h"
#include "ROMon/MonitoringDisplay.h"
#include "ROMon/SubfarmDisplay.h"
#include "ROMon/StorageDisplay.h"
#include "ROMon/FarmDisplay.h"
#include "ROMon/CPUMon.h"
#include "SCR/MouseSensor.h"
#include "CPP/TimeSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/Lock.h"
#include "RTL/strdef.h"
#include "SCR/scr.h"
#include "WT/wtdef.h"
extern "C" {
#include "dic.h"
}

// C++ include files
#include <set>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <fstream>
#include <iostream>
#include <algorithm>

#ifdef _WIN32
#define strcasecmp  _stricmp
#define strncasecmp _strnicmp
#define vsnprintf   _vsnprintf
#endif
using namespace ROMon;
using namespace SCR;
using namespace std;

typedef Nodeset::Nodes               Nodes;
typedef Node::Buffers                Buffers;
typedef MBMBuffer::Clients           Clients;
typedef Node::Tasks                  Tasks;

// Max. 15 seconds without update allowed
#define UPDATE_TIME_MAX 15

typedef vector<string> StringV;

static FarmDisplay* s_fd = 0;
static const char *sstat[17] = {" nl", "   ", "*SL","*EV","*SP","WSL","WEV","WSP","wsl","wev","wsp"," ps"," ac", "SPR", "WER", "   "};
//static const int   INT_min = numeric_limits<int>::min();
static const int   INT_max = numeric_limits<int>::max();
static const float FLT_max = numeric_limits<float>::max();


static void help() {
  cout << "  romon_farm -option [-option]" << endl
            << "       -all                         Show all subfarms." << endl
            << "       -p[artition]=<name>          Partition name providing monitoring information." << endl
            << "       -an[chor]=+<x-pos>+<ypos>    Set anchor for sub displays" << endl
            << endl;
}

static string strUpper(const string& src) {
  string r=src;
  for(size_t i=0;i<r.length();++i)r[i]=::toupper(r[i]);
  return r;
}

namespace ROMon {
  InternalDisplay* createFarmSubDisplay(FarmDisplay* parent, const string& title);
  InternalDisplay* createRecFarmSubDisplay(FarmDisplay* parent, const string& title);
  InternalDisplay* createCtrlFarmSubDisplay(FarmDisplay* parent, const string& title);
  InternalDisplay* createMonitoringSubDisplay(FarmDisplay* parent, const string& title);
  InternalDisplay* createStorageSubDisplay(FarmDisplay* parent, const string& title);
}
namespace {
  struct DisplayUpdate {
    Pasteboard* m_pb;
    bool m_flush;
    DisplayUpdate(InternalDisplay* d, bool flush=false) : m_pb(d->pasteboard()), m_flush(flush) {
      ::scrc_begin_pasteboard_update(m_pb);
    }
    ~DisplayUpdate() {
      ::scrc_end_pasteboard_update (m_pb);
      if ( m_flush ) ::scrc_fflush(m_pb);
    }
  };
  /// Extract node/service name from DNS info
  void getServiceNode(char* s, string& svc, string& node) {
    char* at = strchr(s,'@');
    *at = 0;
    svc = s;
    node = at+1;
  }
}

HelpDisplay::HelpDisplay(FarmDisplay* parent, const string& title, const string& tag) 
: InternalDisplay(parent,title)
{
  bool use = false, isHeader=false;
  string s, input = ::getenv("ROMONROOT") != 0 ? ::getenv("ROMONROOT") : "..";
  string start="<"+tag+">", stop = "</"+tag+">";
  string fin = input+"/doc/farmMon.hlp";
  string head = m_title + ": " + fin;
  ifstream in(fin.c_str());

  ::scrc_create_display(&m_display,55,132,NORMAL,ON,head.c_str());
  ::scrc_put_chars(m_display,"Hit CTRL-H to hide the display",BOLD,2,2,1);
  for(int line=3; in.good(); ) {
    getline(in,s);
    if ( !use && (s.find(start) != string::npos || s.find("<common>") != string::npos) ) {
      isHeader = true;
      use = true;
      continue;
    }
    if ( use && (s.find(stop) != string::npos || s.find("</common>") != string::npos) ) {
      use = false;
    }
    if ( use ) {
      ::scrc_put_chars(m_display,s.c_str(),isHeader ? BOLD: NORMAL,++line,2,1);
      isHeader = false;
    }
  }
  ::scrc_set_border(m_display,head.c_str(),INVERSE|BLUE);
}


BufferDisplay::BufferDisplay(FarmDisplay* parent, const string& title) 
  : InternalDisplay(parent,title), m_node(0)
{
  ::scrc_create_display(&m_display,55,130,MAGENTA,ON,"MBM Monitor display for node:");
}

void BufferDisplay::update(const void* data) {
  const Nodeset* ns = (const Nodeset*)data;
  if ( 0 != ns ) {
    string key;
    map<string,string> entries;
    StringV lines;
    string nam;
    int line = 0, node = 0;
    char txt[1024], name[128];
    char *p, *bnam, *cnam;
    Nodes::const_iterator n;

    for (n=ns->nodes.begin(), line=1; n!=ns->nodes.end(); n=ns->nodes.next(n), ++node)  {
      if ( node == m_node ) {
        time_t tim = (*n).time;
        const Buffers& buffs = *(*n).buffers();
        ::strftime(name,sizeof(name),"%H:%M:%S",::localtime(&tim));
        ::sprintf(txt,"MBM Monitor display for node:%s  [%s]",(*n).name,name);
        ::scrc_set_border(m_display,txt,BLUE|INVERSE);
        for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
          const Buffers::value_type::Control& c = (*ib).ctrl;
          bnam = (char*)(*ib).name;
          //if ( ::strlen(bnam)>10 ) {
          //  p = strchr(bnam,'_');
          //  if ( p ) *p = 0;
          //}
          ::sprintf(name," Buffer \"%s\"",bnam);
          ::sprintf(txt,"%-26s  Events: Produced:%d Actual:%d Seen:%d Pending:%d Max:%d",
                    name, c.tot_produced, c.tot_actual, c.tot_seen, c.i_events, c.p_emax);
          ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
          ::sprintf(txt,"%-26s  Space(kB):[Tot:%d Free:%d] Users:[Tot:%d Max:%d]",
                    "",(c.bm_size*c.bytes_p_Bit)/1024, (c.i_space*c.bytes_p_Bit)/1024, 
                    c.i_users, c.p_umax);
          ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
          ::scrc_put_chars(m_display,"  Occupancy [Events]:",NORMAL,++line,1,1);
          draw_bar(29,line,float(c.i_events)/float(c.p_emax),95);
          ::scrc_put_chars(m_display,"            [Space]: ",NORMAL,++line,1,1);
          draw_bar(29,line,float(c.bm_size-c.i_space)/float(c.bm_size),95);
          ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
        }
        break;
      }
    }
    if ( line > 1 ) {
      ::sprintf(txt,"%-20s%5s%6s  %6s%12s %-4s %s","   Name","Part","PID","State","Seen/Prod","REQ","Buffer");
      ::scrc_put_chars(m_display,txt,INVERSE,++line,1,0);
      ::scrc_put_chars(m_display,txt,INVERSE,line,3+m_display->cols/2,1);
    }
    else {
      time_t t = ::time(0);
      ::scrc_put_chars(m_display,"   Unknown Node. No buffers found.",INVERSE|BOLD,++line,1,1);
      ::strftime(txt,sizeof(txt),"           %H:%M:%S",::localtime(&t));
      ::scrc_put_chars(m_display,txt,INVERSE|BOLD,++line,1,1);
      ::scrc_set_border(m_display,"Unknown Node. No buffers found.",INVERSE|RED|BOLD);
    }
    for (n=ns->nodes.begin(), node=0; n!=ns->nodes.end(); n=ns->nodes.next(n), ++node)  {
      if ( node == m_node ) {
        const Buffers& buffs = *(*n).buffers();
        for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
          const Clients& clients = (*ib).clients;
          char* bnam = (char*)(*ib).name;
	  if ( ::strncmp(bnam,"Events_",7)==0 ) bnam += 7;
          for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
            Clients::const_reference c = (*ic);
            cnam = (char*)c.name;
            if ( ::strlen(cnam)>19 ) {
              p = ::strchr(cnam,'_');
              if ( p ) cnam = ++p;
              if ( ::strlen(cnam)>19 ) {
                p = ::strchr(cnam,'_');
                if ( p ) cnam = ++p;
              }
            }
            if ( c.type == 'C' )
              ::sprintf(txt,"%-20s%5X%6d C%6s%12d %c%c%c%c %s",cnam,c.partitionID,c.processID,
                        sstat[(size_t)c.state],c.events,c.reqs[0],c.reqs[1],c.reqs[2],c.reqs[3],bnam);
            else if ( c.type == 'P' )
              ::sprintf(txt,"%-20s%5X%6d P%6s%12d %4s %s",cnam,c.partitionID,c.processID,
                        sstat[(size_t)c.state],c.events,"",bnam);
            else
              ::sprintf(txt,"%-20s%5X%6d ?%6s%12s %4s %s",cnam,c.partitionID,c.processID,"","","",bnam);
            key = bnam;
            key += cnam;
            entries[key] = txt;
          }
        }
        break;
      }
    }
    lines.clear();
    for(map<string,string>::const_iterator m=entries.begin();m!=entries.end();++m) {
      lines.push_back((*m).second);
    }
    
    for(size_t i=0,len=lines.size(),cnt=len/2+(len%2),j=cnt; i<cnt; ++i, ++j)  {
      if ( j<len ) {
        ::sprintf(name,"%%-%ds  %%-%ds",m_display->cols/2,m_display->cols/2);
        ::sprintf(txt,name,lines[i].c_str(),lines[j].c_str());
      }
      else {
        ::sprintf(name,"%%-%ds",m_display->cols);
        ::sprintf(txt,name,lines[i].c_str());
      }
      ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
    }
    ::memset(txt,' ',m_display->cols);
    txt[m_display->cols-1]=0;
    while(line<m_display->rows)
      ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
  }
}

CtrlDisplay::CtrlDisplay(FarmDisplay* parent, const string& title) 
  : InternalDisplay(parent,title), m_node(0)
{
  ::scrc_create_display(&m_display,55,130,MAGENTA,ON,"Node Control display for node:");
}

void CtrlDisplay::update(const void* data) {
  char txt[255], name[255];
  time_t tim = ::time(0);
  int node=0, cnt=0, line=1;
  Cluster* c = (Cluster*)data;
  if ( c ) {
    const Cluster::Nodes& n = c->nodes;
    //const char* fmt = "  %-24s %12s %17zd / %-5zd     %17zd / %-5zd      %s";

    ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
    ::sprintf(txt, "  Cluster:%s  status:%12s  last update:%s",c->name.c_str(),c->status.c_str(),c->time.c_str());
    ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
    for(Cluster::Nodes::const_iterator i=n.begin(); i!=n.end();++i, ++node) {
      if ( node == m_node ) {
        const Cluster::Node& n = (*i).second;
        Cluster::Node::Tasks::const_iterator j;
        ::strftime(name,sizeof(name),"%H:%M:%S",::localtime(&tim));
        ::sprintf(txt,"Task Control display for node:%s  [%s]",n.name.c_str(),name);
        ::scrc_set_border(m_display,txt,BLUE|INVERSE);

        ::sprintf(txt, "  Node:%s  last update:%s",n.name.c_str(),n.time.c_str());
        ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);

        ::sprintf(txt," %-12s %8s    Tasks       Connections  %6s %6s %6s %6s %3s %3s",
                  "","","RSS","Stack","Data","VSize","CPU","MEM");
        ::scrc_put_chars(m_display,txt,INVERSE,++line,1,1);
        ::sprintf(txt," %-12s %8s found/missing found/missing %6s %6s %6s %6s %3s %3s %s",
                  "Node","Status","[MB]","[MB]","[MB]","[MB]","[%]","[%]","Boot time");
        ::scrc_put_chars(m_display,txt,INVERSE,++line,1,1);
        ::sprintf(txt," %-12s %8s %5zd/%-7zd %5zd/%-7zd %6d %6d %6d %6d %3.0f %3.0f %s",
                  n.name.c_str(),n.status.c_str(),
                  n.taskCount,n.missTaskCount,n.connCount,n.missConnCount,
                  int(n.rss/1024),int(n.stack/1024),int(n.data/1024),int(n.vsize/1024),
                  n.perc_cpu, n.perc_mem, n.boot.c_str());
        ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);

        //::sprintf(txt,"  %-24s %12s %17s / Missing   %17s / Missing    %s","Node","Status","Tasks found","Connections found","Timestamp");
        //::scrc_put_chars(m_display,txt,INVERSE,++line,1,1);
        //::sprintf(txt,fmt,n.name.c_str(),n.status.c_str(),n.taskCount,n.missTaskCount,n.connCount,n.missConnCount,n.time.c_str());
        //::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
        if ( n.status == "DEAD" ) {
          ::scrc_put_chars(m_display,"",INVERSE|RED|BOLD,++line,1,1);
          ::scrc_put_chars(m_display,"                        -------------------------------------",INVERSE|RED|BOLD,++line,1,1);
          ::scrc_put_chars(m_display,"                        Node is DEAD. Severe problem present.",INVERSE|RED|BOLD,++line,1,1);
          ::scrc_put_chars(m_display,"                        -------------------------------------",INVERSE|RED|BOLD,++line,1,1);
          ::scrc_put_chars(m_display,"",INVERSE|RED|BOLD,++line,1,1);
        }
        else {
          typedef map<string,bool> _O;
          int l, x;
          _O ord;
          _O::iterator k;
          ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
          if ( n.taskCount > 0 ) {
            ::scrc_put_chars(m_display,"  Task Summary:",n.missTaskCount > 0 ? INVERSE|BOLD|RED : INVERSE|GREEN,++line,1,1);
            for(j=n.tasks.begin(); j != n.tasks.end(); ++j) ord[(*j).first]=(*j).second;
            for(k=ord.begin(),cnt=0,l=line,x=1; k !=ord.end(); ++k,++cnt) {
              bool ok = (*k).second;
              if ( cnt==int((ord.size()+1)/2) ) l = line, x=61;
              ::sprintf(txt,"   [%03d]  %-32s %-10s",cnt,(*k).first.c_str(),ok ? "OK" : "MISSING");
              ::scrc_put_chars(m_display,txt,ok ? NORMAL : INVERSE|RED,++l,x,1);
            }
            line += (ord.size()+3)/2;
            ord.clear();
          }
          else {
            ::scrc_put_chars(m_display,"  No tasks found",INVERSE|BOLD|RED,++line,1,1);
          }
          if ( n.projects.size() > 0 ) {
            cnt=0;
            ::sprintf(txt,"%-15s %-16s %-10s %-10s %-10s %-10s %-10s",
                      "PVSS Summary:","Project name","Event Mgr","Data Mgr","Dist Mgr","FSM Server","Dev Handler");
            ::scrc_put_chars(m_display,txt,INVERSE|GREEN,++line,1,1);
            for(Cluster::Projects::const_iterator q=n.projects.begin(); q != n.projects.end(); ++q,++cnt)  {
              const Cluster::PVSSProject& p = *q;
              bool ok = p.eventMgr && p.dataMgr && p.distMgr;
              ::sprintf(txt,"   [%03d]        %-16s %-10s %-10s %-10s %-10s %-10s",cnt,p.name.c_str(), 
                        p.eventMgr ? "RUNNING" : "DEAD",
                        p.dataMgr  ? "RUNNING" : "DEAD",
                        p.distMgr  ? "RUNNING" : "DEAD",
                        p.fsmSrv   ? "RUNNING" : "DEAD",
                        p.devHdlr  ? "RUNNING" : "DEAD");
              ::scrc_put_chars(m_display,txt,ok ? NORMAL : INVERSE|RED,++line,1,1);
            }
            ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
          }
          if ( n.connCount > 0 ) {
            ::scrc_put_chars(m_display,"  Connection Summary:",n.missConnCount > 0 ? INVERSE|BOLD|RED : INVERSE|GREEN,++line,1,1);
            for(j=n.conns.begin(); j != n.conns.end(); ++j) ord[(*j).first]=(*j).second;
            for(k=ord.begin(),cnt=0,l=line,x=1; k !=ord.end(); ++k,++cnt) {
              bool ok = (*k).second;
              if ( cnt==int((ord.size()+1)/2) ) l = line, x=61;
              ::sprintf(txt,"   [%03d]  %-32s %-10s",cnt,(*k).first.c_str(),ok ? "OK" : "MISSING");
              ::scrc_put_chars(m_display,txt,ok ? NORMAL : INVERSE|RED,++l,x,1);
            }
            line += (ord.size()+3)/2;
            ord.clear();
          }
          else {
            ::scrc_put_chars(m_display,"  No connections found",INVERSE|BOLD|RED,++line,1,1);
          }
          ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
          if ( n.missTaskCount == 0 && n.missConnCount == 0 )
            ::scrc_put_chars(m_display,"  Node looks fine. No missing tasks/network connections found.",INVERSE|GREEN,++line,1,1);
          else if ( n.missTaskCount == 0 )
            ::scrc_put_chars(m_display,"  No missing tasks found, but missing network connections .....",INVERSE|RED,++line,1,1);
          else if ( n.missConnCount == 0 )
            ::scrc_put_chars(m_display,"  No missing network connections found, but missing tasks .....",INVERSE|RED,++line,1,1);
          ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
        }
      }
    }
  }
  else {
    ::scrc_put_chars(m_display,"   Unknown Node. No data found.",INVERSE|BOLD,++line,1,1);
    ::strftime(txt,sizeof(txt),"           %H:%M:%S",::localtime(&tim));
    ::scrc_put_chars(m_display,txt,INVERSE|BOLD,++line,1,1);
    ::scrc_set_border(m_display,"Unknown Node. No data found.",INVERSE|RED|BOLD);
  }
  ::memset(txt,' ',m_display->cols);
  txt[m_display->cols-1]=0;
  while(line<m_display->rows)
    ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
}

ProcessDisplay::ProcessDisplay(FarmDisplay* parent, const string& title, const string& cluster, int flag, int height, int width)
  : InternalDisplay(parent, title), m_flag(flag)
{
  size_t i;
  string svc = "/";
  m_name = "";
  for(i=0; i<title.length() && title[i]!='.';++i)
    m_name += ::tolower(title[i]);
  for(i=0; i<cluster.length() && cluster[i]!='.';++i)
    svc += ::tolower(cluster[i]);
  svc += (flag ? "/ROpublish/Tasks" : "/ROpublish/ROTasks");
  m_title = "Process monitor on "+m_title+" Service:"+svc;
  ::scrc_create_display(&m_display,height,width,NORMAL,ON,m_title.c_str());
  ::scrc_put_chars(m_display,".....waiting for data from DIM service.....",BOLD,2,10,1);
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
}

ProcessDisplay::~ProcessDisplay() {
}

/// DIM command service callback
void ProcessDisplay::update(const void* address) {
  const ProcFarm* pf = (const ProcFarm*)address;
  if ( pf->type == ProcFarm::TYPE ) {
    updateContent(*pf);
  }
}

static inline bool __procmatch(const char* nam, const char* pattern) {
  return ::strncmp(nam,pattern,strlen(pattern))==0;
}
static bool is_driver(const char* cmd) {
  return
    __procmatch(cmd,"aio/") ||
    __procmatch(cmd,"ata/") ||
    __procmatch(cmd,"events/") ||
    __procmatch(cmd,"lockd") ||
    __procmatch(cmd,"khubd") ||
    __procmatch(cmd,"kacpid") ||
    __procmatch(cmd,"kjournald") ||
    __procmatch(cmd,"kauditd") ||
    __procmatch(cmd,"kblockd/") ||
    __procmatch(cmd,"khelper") ||
    __procmatch(cmd,"kswapd") ||
    __procmatch(cmd,"kseriod") ||
    __procmatch(cmd,"krfcommd") ||
    __procmatch(cmd,"migration/") ||
    __procmatch(cmd,"ata_aux") ||
    __procmatch(cmd,"scsi_eh_") ||
    __procmatch(cmd,"md0_raid") ||
    __procmatch(cmd,"md1_raid") ||
    __procmatch(cmd,"md2_raid") ||
    __procmatch(cmd,"pdflush") ||
    __procmatch(cmd,"rpciod") ||
    __procmatch(cmd,"ksoftirqd/");
}
static inline bool is_sys_task(const char* cmd) { 
  return
    __procmatch(cmd,"init") ||
    __procmatch(cmd,"gpm ") ||
    __procmatch(cmd,"xfs ") ||
    __procmatch(cmd,"nfsd") ||
    __procmatch(cmd,"ntpd") ||
    __procmatch(cmd,"rpc.") ||
    __procmatch(cmd,"crond") ||
    __procmatch(cmd,"dhcpd") ||
    __procmatch(cmd,"klogd") ||
    __procmatch(cmd,"cupsd") ||
    __procmatch(cmd,"mdadm") ||
    __procmatch(cmd,"acpid") ||
    __procmatch(cmd,"xinetd") ||
    __procmatch(cmd,"lm_ip ") ||
    __procmatch(cmd,"portmap") ||
    __procmatch(cmd,"syslogd") ||
    __procmatch(cmd,"irqbalance") ||
    __procmatch(cmd,"sendmail:") ||
    __procmatch(cmd,"dbus-daemon") ||
    __procmatch(cmd,"clientmqueue") ||
    __procmatch(cmd,"mingetty") ||
    __procmatch(cmd,"smartd") ||
    __procmatch(cmd,"dhclient");
}
static inline bool is_pvss_task(const char* cmd) { 
  return __procmatch(cmd,"PVSS00") || __procmatch(cmd,"smiSM ");
}

/// Update display content
void ProcessDisplay::updateContent(const ProcFarm& pf) {
  typedef Procset::Processes _P;
  typedef ProcFarm::Nodes _N;
  char state, txt[255], text[64], tmb[32];
  int cnt = 0;
  int line = 1;
  time_t t1;
  for(_N::const_iterator i=pf.nodes.begin(); i!=pf.nodes.end(); i=pf.nodes.next(i)) {
    const Procset& ps = (*i);
    char* dot = strchr(ps.name,'.');
    if (0 == ::strncmp(ps.name,m_name.c_str(),3+(dot ? dot-ps.name : m_name.length()))) {
      const _P& procs = ps.processes;
      t1 = ps.time;
      ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
      ::strftime(text,sizeof(text),"%H:%M:%S",::localtime(&t1));
      ::sprintf(txt,"Node:%s last update:%s [%d processes]",ps.name,text,procs.size());
      ::scrc_put_chars(m_display,txt,BOLD,++line,10,1);
      if ( m_flag == 0 )
	::scrc_put_chars(m_display,"The display shows tasks with a UTGID",NORMAL,++line,10,1);
      else if ( m_flag == 1 )
	::scrc_put_chars(m_display,"The display shows NON-system, NON-UTGID, NON-PVSS tasks",NORMAL,++line,10,1);
      else if ( m_flag == 2 )
	::scrc_put_chars(m_display,"The display shows SYSTEM tasks",NORMAL,++line,10,1);
      else if ( m_flag == 3 )
	::scrc_put_chars(m_display,"The display shows PVSS tasks",NORMAL,++line,10,1);
      else if ( m_flag == 4 )
	::scrc_put_chars(m_display,"The display shows DRIVER tasks",NORMAL,++line,10,1);
      ::sprintf(txt,"      %-32s %-6s %-5s %5s %5s %6s %6s %7s %7s %6s %3s   %s",
		"UTGID","Owner","State","PID","PPID","Mem[%]","VM[kB]","RSS[kB]","Stk[kB]","CPU[%]","Thr","Started");
      ::scrc_put_chars(m_display,txt,INVERSE,++line,1,1);
      for(_P::const_iterator ip=procs.begin(); ip!=procs.end(); ip=procs.next(ip)) {
	const Process& p = *ip;
	switch(m_flag) {
	case 0:
	  if ( ::strncmp(p.utgid,"N/A",3)==0 ) continue;
	  break;
	case 1:  // Fall-through option if nothing else catches....
	  if ( ::strncmp(p.utgid,"N/A",3)!=0 ) continue;
	  if ( is_pvss_task(p.cmd) ) continue;
	  if ( is_sys_task(p.cmd) ) continue;
	  if ( is_driver(p.cmd) ) continue;
	  break;
	case 2:
	  if ( !is_sys_task(p.cmd) ) continue;
	  break;
	case 3:
	  if ( !is_pvss_task(p.cmd) ) continue;
	  break;
	case 4:
	  if ( !is_driver(p.cmd) ) continue;
	  break;
	default:
	  break;
	}
	t1 = p.start;
	::strftime(tmb,sizeof(tmb),"%b %d %H:%M",::localtime(&t1));
	state = p.state == 0 ? '?' : p.state;
	if ( ::strncmp(p.utgid,"N/A",3)==0 ) {
	  ::strncpy(text,p.cmd,sizeof(text));
	  text[27] = 0;
	  text[26]=text[25]=text[24]='.';
	  ::sprintf(txt,"%3d: %3s:%-28s %-10s %c %5d %5d %6.3f %6.0f %7.0f %7.0f %6.2f %3d   %s",
		    ++cnt, p.utgid,text,p.owner,state,p.pid,p.ppid,p.mem,p.vsize,p.rss,p.stack,p.cpu,p.threads,tmb);
	}
	else {
	  ::sprintf(txt,"%3d: %-32s %-10s %c %5d %5d %6.3f %6.0f %7.0f %7.0f %6.2f %3d   %s",
		    ++cnt, p.utgid,p.owner,state,p.pid,p.ppid,p.mem,p.vsize,p.rss,p.stack,p.cpu,p.threads,tmb);
	}
	::scrc_put_chars(m_display,txt,NORMAL,++line,2,1);
      }
      return;
    }
  }
  t1 = ::time(0);
  ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
  ::sprintf(txt,"   Unknown Node '%s'. No corresponding process information found.",m_name.c_str());
  ::scrc_put_chars(m_display,txt, INVERSE|BOLD,++line,5,1);
  ::strftime(txt,sizeof(txt),"         %H:%M:%S",::localtime(&t1));
  ::scrc_put_chars(m_display,txt,INVERSE|BOLD,++line,5,1);
  ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED|BOLD);
}

CPUDisplay::CPUDisplay(FarmDisplay* parent, const string& title, const string& node, int height, int width)
  : InternalDisplay(parent, title), m_node(node)
{
  string svc = "/";
  for(size_t i=0; i<title.length() && title[i]!='.';++i) svc += ::tolower(title[i]);
  svc += "/ROpublish/CPU";
  m_title = "CPU monitor on "+m_title+" Service:"+svc;
  ::scrc_create_display(&m_display,height,width,NORMAL,ON,m_title.c_str());
  ::scrc_put_chars(m_display,".....waiting for data from DIM service.....",BOLD,2,10,1);
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
}

CPUDisplay::~CPUDisplay() {
}

/// DIM command service callback
void CPUDisplay::update(const void* address) {
  const CPUfarm* f = (const CPUfarm*)address;
  if ( f->type == CPUfarm::TYPE ) {
    updateContent(*f);
  }
}

/// Update display content
void CPUDisplay::updateContent(const CPUfarm& f) {
  typedef CPUset::Cores _C;
  typedef CPUfarm::Nodes _N;
  char txt[255], text[64];
  int cnt = 0;
  int line = 1;
  time_t t1 = f.time;
  ::strftime(text,sizeof(text),"%H:%M:%S",::localtime(&t1));
  ::sprintf(txt,"      CPU farm:%s %s  [%d nodes]",f.name,text,f.nodes.size());
  ::scrc_put_chars(m_display,txt,BOLD,++line,3,1);
  ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
  for(_N::const_iterator i=f.nodes.begin(); i!=f.nodes.end(); i=f.nodes.next(i)) {
    const CPUset& cs = (*i);
    const _C& cores = cs.cores;
    const CPU::Stat& avg = cs.averages;
    t1 = cs.time;
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
    ::strftime(text,sizeof(text),"%H:%M:%S",::localtime(&t1));
    ::sprintf(txt,"      Node:%-8s  Family: %s last update:%s [%d cores] Context switch rate:%9.0f Hz",
              cs.name,cs.family,text,cores.size(),cs.ctxtRate);
    ::scrc_put_chars(m_display,txt,INVERSE,++line,3,1);
    ::sprintf(txt,"      Average values: %9s %9.3f %9.3f %9.3f %10.3f %9.3f %9.3f %9.0f",
              "",avg.user,avg.system,avg.idle,avg.iowait,avg.IRQ,avg.softIRQ,avg.nice);
    ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);
    if ( strcasecmp(m_node.c_str(),cs.name) != 0 ) continue;
    ::sprintf(txt,"        %9s %5s %9s %9s %9s %9s %10s %9s %9s %9s",
              "Clock","Cache","Mips","User[%]","System[%]","Idle[%]","IO wait[%]","IRQ","SoftIRQ","Nice");
    ::scrc_put_chars(m_display,txt,BOLD,++line,1,1);
    for(_C::const_iterator ic=cores.begin(); ic!=cores.end(); ic=cores.next(ic)) {
      const CPU& c = *ic;
      ::sprintf(txt,"Core %3d:%6.0f %5d %9.0f %9.3f %9.3f %9.3f %10.3f %9.3f %9.3f %9.0f",
                ++cnt, c.clock,c.cache,c.bogomips,c.stats.user,c.stats.system,c.stats.idle,
                c.stats.iowait,c.stats.IRQ,c.stats.softIRQ,c.stats.nice);
      ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);
    }
    ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
  }
  if ( 0 == f.nodes.size() ) {
    t1 = ::time(0);
    ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
    ::strftime(txt,sizeof(txt),"   No CPU information found.         %H:%M:%S",::localtime(&t1));
    ::scrc_put_chars(m_display,txt,INVERSE|BOLD,++line,5,1);
    ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED|BOLD);
  }
}

/// Standard constructor
FarmDisplay::FarmDisplay(int argc, char** argv)
  : InternalDisplay(0,""), m_subfarmDisplay(0), m_nodeSelector(0), 
    m_posCursor(0), m_subPosCursor(0), m_anchorX(10), m_anchorY(20), m_mode(HLT_MODE)
{
  bool all = false;
  char txt[128];
  string anchor;
  RTL::CLI cli(argc,argv,help);
  cli.getopt("partition",   2, m_name = "ALL");
  cli.getopt("match",       2, m_match = "*");
  if ( cli.getopt("anchor",2,anchor) != 0 ) {
    int x, y;
    if ( 2 == ::sscanf(anchor.c_str(),"+%d+%d",&x,&y) ) {
      m_anchorX = x;
      m_anchorY = y;
    }
    else if ( 2 == ::sscanf(anchor.c_str(),"%dx%d",&x,&y) ) {
      m_anchorX = x;
      m_anchorY = y;
    }
    else {
      ::printf("No valid anchor position given.\n");
    }
  }
  m_dense = 0 != cli.getopt("dense",2);
  all = 0 != cli.getopt("all",2);
  m_mode = cli.getopt("reconstruction",2) == 0 ? HLT_MODE : RECO_MODE;
  if ( cli.getopt("taskmonitor",2) != 0 ) m_mode = CTRL_MODE;
  s_fd = this;
  if ( m_mode == RECO_MODE && all && m_match=="*" )
    ::sprintf(txt," Reconstruction farm display of all known subfarms ");
  else if ( m_mode == RECO_MODE && all )
    ::sprintf(txt," Reconstruction farm display of all known subfarms with name '%s'",m_match.c_str());
  else if ( m_mode == RECO_MODE )
    ::sprintf(txt," Reconstruction farm display of partition %s ",m_name.c_str());
  else if ( m_mode == CTRL_MODE && all && m_match=="*" )
    ::sprintf(txt," Task Control farm display of all known subfarms ");
  else if ( m_mode == CTRL_MODE && all )
    ::sprintf(txt," Task Control farm display of all known subfarms with name '%s'",m_match.c_str());
  else if ( m_mode == CTRL_MODE )
    ::sprintf(txt," Task Control farm display of partition %s ",m_name.c_str());
  else if ( m_match == "*" && all )
    ::sprintf(txt," HLT Farm display of all known subfarms ");
  else if ( all )
    ::sprintf(txt," HLT Farm display of all known subfarms with the name '%s'",m_match.c_str());
  else
    ::sprintf(txt," HLT Farm display of partition %s ",m_name.c_str());
  m_title = txt;
  ::scrc_create_pasteboard (&m_pasteboard, 0, &m_height, &m_width);
  ScrDisplay::setPasteboard(m_pasteboard);
  ScrDisplay::setBorder(BLUE|INVERSE);
  m_width -= 2;
  m_height -= 2;
  ::scrc_create_display (&m_display, m_height, m_width, BOLD, ON, m_title.c_str());
  show(2,2);
  ::scrc_put_chars(m_display,txt,NORMAL,1,2,1);
  ::scrc_end_pasteboard_update (m_pasteboard);
  ::scrc_fflush(m_pasteboard);
  ::scrc_set_cursor(m_display, 2, 10);
  ::scrc_cursor_off(m_pasteboard);
  ::wtc_remove(WT_FACILITY_SCR);
  ::wtc_subscribe(WT_FACILITY_SCR, key_rearm, key_action, m_pasteboard);
  MouseSensor::instance().start(pasteboard());
  if ( all ) {
    m_svc = ::dic_info_service((char*)"DIS_DNS/SERVER_LIST",MONITORED,0,0,0,dnsDataHandler,(long)this,0,0);
  }
  else {
    m_listener = auto_ptr<PartitionListener>(new PartitionListener(this,m_name));
  }
}

/// Standard destructor
FarmDisplay::~FarmDisplay()  {  
  MouseSensor::instance().stop();
  ::wtc_remove(WT_FACILITY_SCR);
  disconnect();
  m_listener = auto_ptr<PartitionListener>(0);
  ::scrc_begin_pasteboard_update(m_pasteboard);
  m_ctrlDisplay = auto_ptr<CtrlDisplay>(0);
  m_mbmDisplay = auto_ptr<BufferDisplay>(0);
  if ( m_subfarmDisplay ) {
    MouseSensor::instance().remove(m_subfarmDisplay->display());
    if ( m_nodeSelector ) {
      MouseSensor::instance().remove(m_nodeSelector->display());
    }
    m_subfarmDisplay->finalize();
    delete m_subfarmDisplay;
    m_subfarmDisplay = 0;
  }
  subDisplays().clear();
  close();
  ::scrc_end_pasteboard_update (m_pasteboard);
  ::scrc_delete_pasteboard(m_pasteboard);
  m_pasteboard = 0;
  ::scrc_resetANSI();
  ::printf("Farm display deleted and resources freed......\n");
}

/// DIM command service callback
void FarmDisplay::dnsDataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    FarmDisplay* disp = *(FarmDisplay**)tag;
    disp->update(address);
  }
}

/// Keyboard rearm action
int FarmDisplay::key_rearm (unsigned int /* fac */, void* param)  {
  Pasteboard* pb = (Pasteboard*)param;
  return ::scrc_fflush(pb);
}

/// Keyboard action
int FarmDisplay::key_action(unsigned int /* fac */, void* /* param */)  {
  int key = ::scrc_read_keyboard(0,0);
  if (!key) return WT_SUCCESS;
  return s_fd->handleKeyboard(key);
}

/// Set cursor to position
void FarmDisplay::set_cursor(InternalDisplay* /* updater */) {
  set_cursor();
}

/// Set cursor to position
void FarmDisplay::set_cursor() {
  if ( 0 == m_subfarmDisplay ) {
    InternalDisplay* d = currentDisplay();
    if ( d ) ::scrc_set_cursor(d->display(),2, 2);
  }
  else {
    Display* d1 = m_subfarmDisplay->display();
    if ( d1 ) ::scrc_set_cursor(d1, m_subPosCursor, 2);
  }
}

/// Get farm display from cursor position
InternalDisplay* FarmDisplay::currentDisplay() {
  size_t cnt;
  SubDisplays::iterator k;
  SubDisplays& sd = subDisplays();
  for(k=sd.begin(), cnt = 0; k != sd.end(); ++k, ++cnt) {
    if ( cnt == m_posCursor ) {
      return (*k).second;
    }
  }
  return 0;
}

/// Show subfarm display
int FarmDisplay::showSubfarm()    {
  InternalDisplay* d = 0;
  if ( m_subfarmDisplay ) {
    DisplayUpdate update(this,true);
    MouseSensor::instance().remove(m_subfarmDisplay->display());
    if ( m_nodeSelector ) {
      MouseSensor::instance().remove(m_nodeSelector->display());
    }
    m_subfarmDisplay->finalize();
    delete m_subfarmDisplay;
    m_subfarmDisplay = 0;
    m_cpuDisplay = auto_ptr<CPUDisplay>(0);
    m_mbmDisplay = auto_ptr<BufferDisplay>(0);
    m_ctrlDisplay = auto_ptr<CtrlDisplay>(0);
    m_procDisplay = auto_ptr<ProcessDisplay>(0);
    m_subPosCursor = 8;
    m_nodeSelector = 0;
  }
  else if ( (d=currentDisplay()) != 0 ) {
    string dnam = d->name();
    string title = "Sub farm info:" + dnam;
    string svc = "-servicename=/"+dnam+"/ROpublish";
    string part= "-partition="+m_name;
    if ( m_mode == CTRL_MODE ) {
      svc = "-servicename=/"+strUpper(dnam)+"/TaskSupervisor/Status";
      const char* argv[] = {"",svc.c_str(), "-delay=300"};
      m_subfarmDisplay = new CtrlSubfarmDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX,m_anchorY,3,(char**)argv);
    }
    else if ( strncasecmp(dnam.c_str(),"storectl01",10)==0 && m_name != "ALL" ) {
      const char* argv[] = {"",svc.c_str(), part.c_str(), "-delay=300"};
      m_subfarmDisplay = new StorageDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX,m_anchorY,4,(char**)argv);
    }
    else if ( strncasecmp(dnam.c_str(),"mona08",6)==0 && m_name != "ALL" ) {
      string relay = "-namerelay="+dnam+"01";
      const char* argv[] = {"",svc.c_str(), part.c_str(), "-delay=300", "-relayheight=12", "-nodeheight=12", relay.c_str()};
      m_subfarmDisplay = new MonitoringDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX,m_anchorY,7,(char**)argv);
    }
    else if ( strncasecmp(dnam.c_str(),"mona09",6)==0 && m_name != "ALL" ) {
      string relay = "-namerelay="+dnam+"01";
      const char* argv[] = {"",svc.c_str(), part.c_str(), "-delay=300", "-relayheight=14", "-nodeheight=22", relay.c_str()};
      m_subfarmDisplay = new MonitoringDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX,m_anchorY,7,(char**)argv);
    }
    else if ( m_mode == RECO_MODE ) {
      const char* argv[] = {"",svc.c_str(), "-delay=300"};
      m_subfarmDisplay = new RecSubfarmDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX,m_anchorY,3,(char**)argv);
    }
    else if ( m_mode == HLT_MODE ) {
      const char* argv[] = {"",svc.c_str(), "-delay=300"};
      m_subfarmDisplay = new SubfarmDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX,m_anchorY,3,(char**)argv);
    }
    else {
      m_nodeSelector = 0;
      m_subfarmDisplay = 0;
      return WT_SUCCESS;
    }
    m_nodeSelector = dynamic_cast<ScrDisplay*>(m_subfarmDisplay->nodeDisplay());
    m_subfarmDisplay->initialize();
    ::lib_rtl_sleep(200);
    MouseSensor::instance().add(this,m_subfarmDisplay->display());
    if ( m_nodeSelector )   {
      MouseSensor::instance().add(this,m_nodeSelector->display());
    }
    IocSensor::instance().send(this,CMD_UPDATE,m_subfarmDisplay);
    m_subPosCursor = SUBFARM_NODE_OFFSET;
  }
  return WT_SUCCESS;
}

/// Show context dependent help window
int FarmDisplay::showHelpWindow() {
  DisplayUpdate update(this,true);
  if ( m_helpDisplay.get() ) {
    MouseSensor::instance().remove(this,m_helpDisplay->display());
    m_helpDisplay = auto_ptr<HelpDisplay>(0);
  }
  else if ( m_mbmDisplay.get() ) 
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","mbm"));
  else if ( m_ctrlDisplay.get() ) 
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","ctrl"));
  else if ( m_procDisplay.get() ) 
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","procs"));
  else if ( m_cpuDisplay.get() ) 
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","cpu"));
  else if ( m_subfarmDisplay && m_mode == CTRL_MODE )
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","subfarm_ctrl"));
  else if ( m_subfarmDisplay && m_mode == RECO_MODE )
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","subfarm_reco"));
  else if ( m_subfarmDisplay )
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","subfarm"));
  else
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","farm"));
  if ( m_helpDisplay.get() ) {
    m_helpDisplay->show(m_anchorY,m_anchorX);
    MouseSensor::instance().add(this,m_helpDisplay->display());
  }
  return WT_SUCCESS;
}

/// Show window with processes on a given node
int FarmDisplay::showProcessWindow(int flag) {
  if ( m_procDisplay.get() ) {
    if ( m_helpDisplay.get() ) showHelpWindow();
    MouseSensor::instance().remove(this,m_procDisplay->display());
    m_procDisplay = auto_ptr<ProcessDisplay>(0);
  }
  else if ( m_subfarmDisplay ) {
    string cluster = m_subfarmDisplay->clusterName();
    string node = m_subfarmDisplay->nodeName(m_subPosCursor-SUBFARM_NODE_OFFSET);
    if ( !node.empty() ) {
      m_procDisplay = auto_ptr<ProcessDisplay>(new ProcessDisplay(this,node,cluster,flag));
      m_procDisplay->show(m_anchorY+5,m_anchorX+12);
      MouseSensor::instance().add(this,m_procDisplay->display());
      return WT_SUCCESS;
    }
  }
  return WT_SUCCESS;
}

/// Show window with buffer information of a given node
int FarmDisplay::showMbmWindow() {
  DisplayUpdate update(this,true);
  if ( m_mbmDisplay.get() ) {
    if ( m_helpDisplay.get() ) showHelpWindow();
    MouseSensor::instance().remove(this,m_mbmDisplay->display());
    m_mbmDisplay = auto_ptr<BufferDisplay>(0);
  }
  else if ( m_subfarmDisplay ) {
    m_mbmDisplay = auto_ptr<BufferDisplay>(new BufferDisplay(this,"MBM Monitor display"));
    m_mbmDisplay->setNode(m_subPosCursor-SUBFARM_NODE_OFFSET);
    m_mbmDisplay->update(m_subfarmDisplay->data().pointer);
    m_mbmDisplay->show(m_anchorY+5,m_anchorX+12);
    MouseSensor::instance().add(this,m_mbmDisplay->display());
  }
  return WT_SUCCESS;
}

/// Show window with buffer information of a given node
int FarmDisplay::showCtrlWindow() {
  DisplayUpdate update(this,true);
  if ( m_ctrlDisplay.get() ) {
    if ( m_helpDisplay.get() ) showHelpWindow();
    MouseSensor::instance().remove(this,m_ctrlDisplay->display());
    m_ctrlDisplay = auto_ptr<CtrlDisplay>(0);
  }
  else if ( m_subfarmDisplay ) {
    m_ctrlDisplay = auto_ptr<CtrlDisplay>(new CtrlDisplay(this,"CTRL Monitor display"));
    m_ctrlDisplay->setNode(m_subPosCursor-SUBFARM_NODE_OFFSET);
    m_ctrlDisplay->update(m_subfarmDisplay->data().pointer);
    m_ctrlDisplay->show(m_anchorY+5,m_anchorX+12);
    MouseSensor::instance().add(this,m_ctrlDisplay->display());
  }
  return WT_SUCCESS;
}

/// Show window with CPU information of a given subfarm
int FarmDisplay::showCpuWindow() {
  DisplayUpdate update(this,true);
  if ( m_cpuDisplay.get() ) {
    if ( m_helpDisplay.get() ) showHelpWindow();
    MouseSensor::instance().remove(this,m_cpuDisplay->display());
    m_cpuDisplay = auto_ptr<CPUDisplay>(0);
  }
  else if ( m_subfarmDisplay ) {
    string cluster_name = m_subfarmDisplay->clusterName();
    string node_name = m_subfarmDisplay->nodeName(m_subPosCursor-SUBFARM_NODE_OFFSET);
    if ( !node_name.empty() ) {
      m_cpuDisplay = auto_ptr<CPUDisplay>(new CPUDisplay(this,cluster_name,node_name));
      m_cpuDisplay->show(m_anchorY+5,m_anchorX+12);
      MouseSensor::instance().add(this,m_cpuDisplay->display());
      return WT_SUCCESS;
    }
  }
  return WT_SUCCESS;
}

/// DIM command service callback
void FarmDisplay::update(const void* address) {
  char c, *msg = (char*)address;
  string svc, node;
  size_t idx, idq;
  switch(c=msg[0]) {
  case '+':
    getServiceNode(++msg,svc,node);
    idx = svc.find("/ROpublish");
    idq = svc.find("/hlt");
    if ( idq == string::npos ) idq = svc.find("/mona");
    if ( idq == string::npos ) idq = svc.find("/store");
    if ( idx != string::npos && idq == 0 ) {
      string f = svc.substr(1,idx-1);
      if ( ::strcase_match_wild(f.c_str(),m_match.c_str()) ) {
        IocSensor::instance().send(this,CMD_ADD,new string(f));
      }
    }
    break;
  case '-':
    break;
  case '!':
    //getServiceNode(++msg,svc,node);
    //log() << "Service " << msg << " in ERROR." << endl;
    break;
  default:
    if ( *(int*)msg != *(int*)"DEAD" )  {
      char *at, *p = msg, *last = msg;
      auto_ptr<Farms> farms(new Farms);
      while ( last != 0 && (at=strchr(p,'@')) != 0 )  {
        last = strchr(at,'|');
        if ( last ) *last = 0;
        getServiceNode(p,svc,node);
        idx = svc.find("/ROpublish");
        idq = svc.find("/hlt");
        if ( idq == string::npos ) idq = svc.find("/mona");
        if ( idq == string::npos ) idq = svc.find("/store");
        if ( idx != string::npos && idq == 0 ) {
          string f = svc.substr(1,idx-1);
          if ( ::strcase_match_wild(f.c_str(),m_match.c_str()) ) {
            farms->push_back(f);
          }
        }
        p = last+1;
      }
      if ( !farms->empty() )
        IocSensor::instance().send(this,CMD_CONNECT,farms.release());
    }
    break;
  }
}

/// Handle keyboard interrupts
int FarmDisplay::handleKeyboard(int key)    {
  int cnt, col, row;
  InternalDisplay* d = 0;
  Display* d1;
  SubDisplays& sd = subDisplays();
  RTL::Lock lock(screenLock());
  try {
    switch (key)    {
    case CTRL_W:
      ::scrc_repaint_screen (m_pasteboard);
      break;
    case 'E':
    case CTRL_E:
      delete this;
      ::exit(0);
      return WT_SUCCESS;
    case 'h':
    case 'H':
    case CTRL_H:
      return showHelpWindow();
    case 'c':
    case 'C':
      return showCpuWindow();
    case 'k':
    case 'K':
      IocSensor::instance().send(this,CMD_SHOWCTRL,this);
      return WT_SUCCESS;
    case 'm':
    case 'M':
    case '.':
    case KPD_PERIOD:
      IocSensor::instance().send(this,CMD_SHOWMBM,this);
      return WT_SUCCESS;
    case 'p':
    case 'P':
      return showProcessWindow(0);
    case CTRL_B:
      return showProcessWindow(1);
    case CTRL_V:
      return showProcessWindow(2);
    case CTRL_P:
      return showProcessWindow(3);
    case CTRL_N:
      return showProcessWindow(4);
    case CTRL_M:
      return WT_SUCCESS;      

    case RETURN_KEY:
    case ENTER:
      IocSensor::instance().send(this,CMD_SHOWSUBFARM,this);
      return WT_SUCCESS;
    case MOVE_LEFT:
      if ( 0 == m_subfarmDisplay && (d=currentDisplay()) ) {
        SubDisplays::reverse_iterator k;
        col = d ? d->display()->col : 2;
        row = d ? d->display()->row : 2;
        for(k=sd.rbegin(), cnt=sd.size(); k != sd.rend(); ++k, --cnt) {
          d1 = (*k).second->display();
          if ( d1->col < col ) {
            if ( d1->row == row ) {
              m_posCursor = --cnt;
              set_cursor();
              return WT_SUCCESS;
            }
          }
        }
      }
      break;
    case MOVE_RIGHT:
      if ( 0 == m_subfarmDisplay && (d=currentDisplay()) ) {
        SubDisplays::iterator k;
        col = d ? d->display()->col : 2;
        row = d ? d->display()->row : 2;
        for(k=sd.begin(), cnt=0; k != sd.end(); ++k, ++cnt) {
          d1 = (*k).second->display();
          if ( d1->col > col ) {
            if ( d1->row == row ) {
              m_posCursor = cnt;
              set_cursor();
              return WT_SUCCESS;
            }
          }
        }
      }
      break;
    case MOVE_UP:
      if ( 0 == m_subfarmDisplay && m_posCursor > 0 )
        --m_posCursor;
      else if ( 0 != m_subfarmDisplay && m_subPosCursor > SUBFARM_NODE_OFFSET )
        --m_subPosCursor;
      break;
    case MOVE_DOWN:
      if ( 0 == m_subfarmDisplay && m_posCursor < subDisplays().size()-1 )
        ++m_posCursor;
      else if ( 0 != m_subfarmDisplay && m_subfarmDisplay->numNodes()>(m_subPosCursor-SUBFARM_NODE_OFFSET) )
        ++m_subPosCursor;
      break;
    case PAGE_UP:
      if ( 0 == m_subfarmDisplay && (d=currentDisplay()) ) {
        SubDisplays::reverse_iterator k;
        col = d ? d->display()->col : 2;
        row = d ? d->display()->row : 2;
        for(k=sd.rbegin(), cnt=sd.size(); k != sd.rend(); ++k, --cnt) {
          d1 = (*k).second->display();
          if ( d1->col == col ) {
            if ( d1->row == min(d1->row,row) ) {
              row = d1->row;
              m_posCursor = cnt;
            }
          }
        }
      }
      break;
    case PAGE_DOWN:
      if ( 0 == m_subfarmDisplay && (d=currentDisplay()) ) {
        SubDisplays::iterator k;
        col = d ? d->display()->col : 2;
        row = d ? d->display()->row : 0;
        for(k=sd.begin(), cnt=0; k != sd.end(); ++k, ++cnt) {
          d1 = (*k).second->display();
          if ( d1->col == col ) {
            if ( d1->row == max(d1->row,row) ) {
              row = d1->row;
              m_posCursor = cnt;
            }
          }
        }
      }
      break;
    default:
      return WT_SUCCESS;
    }
  }
  catch(...) {
  }
  set_cursor();
  return WT_SUCCESS;
}

/// Interactor overload: Display callback handler
void FarmDisplay::handle(const Event& ev) {
  int cnt = 0;
  time_t now = time(0);
  Farms::iterator i;
  SubDisplays::iterator k;
  const MouseEvent* m = 0;

  RTL::Lock lock(screenLock());
  switch(ev.eventtype) {
  case ScrMouseEvent:
    m = ev.get<MouseEvent>();
    if ( m->button == 2 ) {
      IocSensor::instance().send(this,CMD_SHOWHELP,this);
    }
    else if ( m->msec != (unsigned int)-1 ) {
      if ( m_helpDisplay.get() )//&& m_helpDisplay->display() == m->display )
        IocSensor::instance().send(this,CMD_SHOWHELP,this);
      else if ( m_ctrlDisplay.get() )//&& m_ctrlDisplay->display() == m->display )
        IocSensor::instance().send(this,CMD_SHOWCTRL,this);
      else if ( m_mbmDisplay.get() )//&& m_mbmDisplay->display() == m->display )
        IocSensor::instance().send(this,CMD_SHOWMBM,this);
      else if ( m_procDisplay.get() )//&& m_procDisplay->display() == m->display )
        IocSensor::instance().send(this,CMD_SHOWPROCS,this);
      else if ( m_cpuDisplay.get() )//&& m_cpuDisplay->display() == m->display )
        IocSensor::instance().send(this,CMD_SHOWCPU,this);
      else
        IocSensor::instance().send(this,CMD_SHOWSUBFARM,this);
    }
    else if ( m_subfarmDisplay && m->msec == (unsigned int)-1 ) {
      Display* disp = m_nodeSelector->display();
      if ( m->display == disp ) {
        size_t pos = m->y - disp->row - 2;
        if ( m_subfarmDisplay->numNodes()>pos ) {
          RTL::Lock unlock(screenLock(),true);
          m_subPosCursor = pos + SUBFARM_NODE_OFFSET;
          handleKeyboard(m->button == 0 ? KPD_PERIOD : CTRL_P);
        }
      }
    }
    return;
  case TimeEvent:
    if (ev.timer_data == m_subfarmDisplay ) {
      IocSensor::instance().send(this,CMD_UPDATE,this);
    }
    return;
  case IocEvent:
    switch(ev.type) {
    case CMD_SETCURSOR:
      set_cursor();
      break;
    case CMD_HANDLE_KEY:
      handleKeyboard(int((long)ev.data));
      break;
    case CMD_SHOWSUBFARM: {
      RTL::Lock lock(screenLock(),true);
      showSubfarm();
      return;
    }
    case CMD_SHOWCPU:
      showCpuWindow();
      return;
    case CMD_SHOWCTRL:
    case CMD_SHOWMBM:
      if ( m_mode == CTRL_MODE ) showCtrlWindow();
      else showMbmWindow();
      return;
    case CMD_SHOWPROCS:
      showProcessWindow(0);
      return;
    case CMD_SHOWHELP:
      showHelpWindow();
      return;
    case CMD_SHOW:
      for(k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k, ++cnt) {
        InternalDisplay* d = (*k).second;
        if ( d == ev.data )  {
          RTL::Lock unlock(screenLock(),true);
          m_posCursor = cnt;
          showSubfarm();
          return;
        }
      }
      break;
    case CMD_POSCURSOR:
      for(k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k, ++cnt) {
        InternalDisplay* d = (*k).second;
        if ( d == ev.data )  {
          m_posCursor = cnt;
          set_cursor();
          return;
        }
      }
      break;
    case CMD_UPDATE:
      if ( m_subfarmDisplay )   {
        //m_subfarmDisplay->update();
        IocSensor::instance().send(m_subfarmDisplay,ROMonDisplay::CMD_UPDATEDISPLAY,this);
      }
      if ( m_mbmDisplay.get() )  {
        const void* data = m_subfarmDisplay->data().pointer;
        m_mbmDisplay->setNode(m_subPosCursor-SUBFARM_NODE_OFFSET);
        m_mbmDisplay->update(data);
      }
      else if ( m_ctrlDisplay.get() ) {
        const void* data = m_subfarmDisplay->data().pointer;
        m_ctrlDisplay->setNode(m_subPosCursor-SUBFARM_NODE_OFFSET);
        m_ctrlDisplay->update(data);
      }
      TimeSensor::instance().add(this,1,m_subfarmDisplay);
      break;
    case CMD_ADD:
      i = find(m_farms.begin(),m_farms.end(),*(string*)ev.data);
      if ( i == m_farms.end() ) {
        m_farms.push_back(*(string*)ev.data);
        connect(m_farms);
      }
      delete (string*)ev.data;
      return;
    case CMD_CONNECT: {
      DisplayUpdate update(this,false);
      m_farms = *(StringV*)ev.data;
      connect(m_farms);
      delete (StringV*)ev.data;
      return;
    }
    case CMD_CHECK: {
      DisplayUpdate update(this,true);
      for(k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k) {
        InternalDisplay* d = (*k).second;
        if ( d != ev.data ) d->check(now);
      }
      break;
    }
    case CMD_DELETE:
      delete this;
      ::lib_rtl_sleep(200);
      ::exit(0);
      return;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void FarmDisplay::connect(const vector<string>& farms) {
  SubDisplays::iterator k;
  SubDisplays copy;
  char txt[128];

  ::sprintf(txt,"Total number of subfarms:%d %50s",int(farms.size()),"<CTRL-H for Help>, <CTRL-E to exit>");
  ::scrc_put_chars(m_display,txt,NORMAL,1,2,1);

  for( k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k)
    (*k).second->hide();
  for (Farms::const_iterator i=farms.begin(); i != farms.end(); ++i) {
    k = m_farmDisplays.find(*i);
    if ( k == m_farmDisplays.end() ) {
      if ( m_mode == RECO_MODE )
        copy.insert(make_pair(*i,createRecFarmSubDisplay(this,*i)));
      else if ( m_mode == CTRL_MODE )
        copy.insert(make_pair(*i,createCtrlFarmSubDisplay(this,*i)));
      else if ( ::strncasecmp((*i).c_str(),"mona0",5)==0 )
        copy.insert(make_pair(*i,createMonitoringSubDisplay(this,*i)));
      else if ( ::strncasecmp((*i).c_str(),"storectl",8)==0 )
        copy.insert(make_pair(*i,createStorageSubDisplay(this,*i)));
      else
        copy.insert(make_pair(*i,createFarmSubDisplay(this,*i)));
    }
    else {
      copy.insert(*k);
      m_farmDisplays.erase(k);
    }
  }
  for (k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k)
    delete (*k).second;
  m_farmDisplays = copy;

  int subdisp_height = m_dense ? SUBFARM_DISP_HEIGHT_DENSE : SUBFARM_DISP_HEIGHT_NORMAL;
  int subdisp_width  = SUBFARM_DISP_WIDTH;
  int total_height   = SUBFARM_DISP_TOP;
  int total_width    = SUBFARM_DISP_LEFT;
  for (k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k) {
    int h = total_height+subdisp_height;
    int w = total_width+subdisp_width+1;
    if ( h > m_height+2 ) {
      total_height = SUBFARM_DISP_TOP;
      total_width  = w+1;
      if ( w+subdisp_width > m_width+2 ) break;
    }
    (*k).second->show(total_height, total_width);
    total_height += subdisp_height+1;
  }
}

static size_t do_output(void*,int,const char* fmt, va_list args) {
  char buffer[1024];
  size_t len = ::vsnprintf(buffer, sizeof(buffer), fmt, args);
  //::upic_write_message(buffer,"");
  return len;
}

extern "C" int romon_farm(int argc,char** argv) {
  FarmDisplay* disp = new FarmDisplay(argc,argv);
  ::lib_rtl_install_printer(do_output,0);
  IocSensor::instance().run();
  delete disp;
  return 1;
}
