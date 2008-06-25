// $Id: FarmDisplay.cpp,v 1.6 2008-06-25 22:51:12 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/FarmDisplay.cpp,v 1.6 2008-06-25 22:51:12 frankb Exp $

#include "ROMon/SubfarmDisplay.h"
#include "ROMon/FarmDisplay.h"
#include "ROMon/CPUMon.h"
#include "CPP/TimeSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#include "SCR/scr.h"
#include "WT/wtdef.h"
extern "C" {
#include "dic.h"
}

// C++ include files
#include <set>
#include <limits>
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace ROMon;
using namespace SCR;

typedef Nodeset::Nodes               Nodes;
typedef Node::Buffers                Buffers;
typedef MBMBuffer::Clients           Clients;
typedef Node::Tasks                  Tasks;
#define BUILDER_TASK                 'M'
#define SENDER_TASK                  'D'
#define MOORE_TASK                   'G'
#define MEP_BUFFER                   'M'
#define RES_BUFFER                   'R'
#define EVT_BUFFER                   'E'
#define SND_BUFFER                   'S'
#define SUBFARM_HEIGHT               65
#define SUBFARM_WIDTH               132
#define SUBFARM_NODE_OFFSET           8
#define SUBFARM_DISP_WIDTH           48
#define SUBFARM_DISP_HEIGHT_NORMAL    5
#define SUBFARM_DISP_HEIGHT_DENSE     4
#define SUBFARM_DISP_TOP              4
#define SUBFARM_DISP_LEFT             3

#define SLOTS_MIN                     .1
#define SPACE_MIN                     .1
// Max. 15 seconds without update allowed
#define UPDATE_TIME_MAX 15

typedef std::vector<std::string> StringV;

static FarmDisplay* s_fd = 0;
static lib_rtl_lock_t    s_lock;
static const char *sstat[17] = {" nl", "   ", "*SL","*EV","*SP","WSL","WEV","WSP","wsl","wev","wsp"," ps"," ac", "SPR", "WER", "   "};


static void help() {
  std::cout <<"  romon_farm -option [-option]" << std::endl
	    <<"       -p[artition]=<name>          Partition name providing monitoring information." << std::endl
	    << std::endl;
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
  void getServiceNode(char* s, std::string& svc, std::string& node) {
    char* at = strchr(s,'@');
    *at = 0;
    svc = s;
    node = at+1;
  }
}

InternalDisplay::InternalDisplay(FarmDisplay* parent, const std::string& title) 
: m_pasteboard(0), m_display(0), m_parent(parent), m_name(title), m_title(title), m_svc(0)
{
  m_pasteboard = m_parent ? m_parent->pasteboard() : 0;
  m_lastUpdate = time(0);
}

InternalDisplay::~InternalDisplay() {
  disconnect();
  close();
}

void InternalDisplay::close() {
  if ( m_display ) {
    ::scrc_unpaste_display(m_display,m_pasteboard);
    ::scrc_delete_display(m_display);
    m_display = 0;
  }
}

void InternalDisplay::disconnect() {
  if ( m_svc != 0 ) {
    ::dic_release_service(m_svc);
    m_svc = 0;
  }
}

void InternalDisplay::show(int row, int col) {
  ::scrc_paste_display (m_display, m_pasteboard, row, col);
}

void InternalDisplay::hide() {
  ::scrc_unpaste_display(m_display,m_pasteboard);
}

/// Draw bar to show occupancies
size_t InternalDisplay::draw_bar(int x, int y, float f1, int scale)  {
  size_t l = size_t(float(scale)*f1);
  char txt[1024];
  memset(txt,' ',l);
  txt[l] = 0;
  ::scrc_put_chars(m_display,txt,INVERSE,y,x,0);
  memset(txt,'.',scale-l);
  txt[scale-l] = 0;
  ::scrc_put_chars(m_display,txt,NORMAL,y,l+x,1);
  return 1;
}

/// DIM command service callback
void InternalDisplay::dataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    InternalDisplay* disp = *(InternalDisplay**)tag;
    Pasteboard* pb = disp->pasteboard();
    FarmDisplay* fd = disp->parent();
    RTL::Lock lock(s_lock);
    ::scrc_cursor_off(pb);
    ::scrc_begin_pasteboard_update (pb);
    disp->update(address);
    ::scrc_end_pasteboard_update(pb);
    if ( fd ) fd->set_cursor();
    ::scrc_cursor_on(pb);
  }
}

HelpDisplay::HelpDisplay(FarmDisplay* parent, const std::string& title, const std::string& tag) 
: InternalDisplay(parent,title)
{
  bool use = false, isHeader=false;
  std::string s, input = ::getenv("ROMONROOT") ? ::getenv("ROMONROOT") : "../doc";
  std::string start="<"+tag+">", stop = "</"+tag+">";
  std::ifstream in((input+"/farmMon.hlp").c_str());

  ::scrc_create_display(&m_display,45,130,NORMAL,ON,m_title.c_str());
  ::scrc_put_chars(m_display,"Hit CTRL-H to hide the display",BOLD,2,2,1);
  for(int line=3; in.good(); ) {
    std::getline(in,s);
    if ( !use && (s.find(start) != std::string::npos || s.find("<common>") != std::string::npos) ) {
      isHeader = true;
      use = true;
      continue;
    }
    if ( use && (s.find(stop) != std::string::npos || s.find("</common>") != std::string::npos) ) {
      use = false;
    }
    if ( use ) {
      ::scrc_put_chars(m_display,s.c_str(),isHeader ? BOLD: NORMAL,++line,2,1);
      isHeader = false;
    }
  }
}

ProcessDisplay::ProcessDisplay(FarmDisplay* parent, const std::string& title, int height, int width)
: InternalDisplay(parent, title)
{
  std::string svc = "/";
  m_name = "";
  for(size_t i=0; i<title.length() && title[i]!='.';++i) {
    m_name += ::tolower(title[i]);
    svc += ::tolower(title[i]);
  }
  svc = svc.substr(0,svc.length()-2);
  svc += "/PROCmon";
  m_title = "Process monitor on "+m_title+" Service:"+svc;
  ::scrc_create_display(&m_display,height,width,NORMAL,ON,m_title.c_str());
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

/// Update display content
void ProcessDisplay::updateContent(const ProcFarm& pf) {
  typedef Procset::Processes _P;
  typedef ProcFarm::Nodes _N;
  char txt[255], text[64];
  int cnt = 0;
  int line = 1;
  time_t t1;
  for(_N::const_iterator i=pf.nodes.begin(); i!=pf.nodes.end(); i=pf.nodes.next(i)) {
    const Procset& ps = (*i);
    char* dot = strchr(ps.name,'.');
    if (0 == ::strncmp(ps.name,m_name.c_str(),dot ? dot-ps.name : m_name.length())) {
      const _P& procs = ps.processes;
      t1 = ps.time;
      ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
      ::strftime(text,sizeof(text),"%H:%M:%S",::localtime(&t1));
      ::sprintf(txt,"      Node:%s last update:%s [%d processes]",ps.name,text,procs.size());
      ::scrc_put_chars(m_display,txt,BOLD,++line,3,1);
      ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
      ::sprintf(txt,"        %-32s %5s %5s %9s %9s %9s %6s %8s",
		"UTGID","PID","PPID","Memory[%]","VSize[kB]","RSS[kB]","CPU[%]","#Threads");
      ::scrc_put_chars(m_display,txt,INVERSE,++line,1,1);
      for(_P::const_iterator ip=procs.begin(); ip!=procs.end(); ip=procs.next(ip)) {
	const Process& p = *ip;
	::sprintf(txt,"%3d:  %-32s %5d %5d %9.0f %9.0f %9.0f %6.2f %8d",
		  ++cnt, p.utgid,p.pid,p.ppid,p.mem,p.vsize,p.rss,p.cpu,p.threads);
	::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);
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

CPUDisplay::CPUDisplay(FarmDisplay* parent, const std::string& title, int height, int width)
: InternalDisplay(parent, title)
{
  std::string svc = "/";
  m_name = title.substr(0,title.length()-2);
  for(size_t i=0; i<title.length() && title[i]!='.';++i) svc += ::tolower(title[i]);
  svc += "/CPUmon";
  m_title = "CPU monitor on "+m_title+" Service:"+svc;
  ::scrc_create_display(&m_display,height,width,NORMAL,ON,m_title.c_str());
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
    t1 = cs.time;
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
    ::strftime(text,sizeof(text),"%H:%M:%S",::localtime(&t1));
    ::sprintf(txt,"      Node:%s  Family: %s last update:%s [%d cores] Context switch rate:%9.0f Hz",
	      cs.name,cs.family,text,cores.size(),cs.ctxtRate);
    ::scrc_put_chars(m_display,txt,BOLD,++line,3,1);
    ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
    ::sprintf(txt,"        %9s %5s %9s %9s %9s %9s %10s %9s %9s %9s",
	      "Clock","Cache","Mips","User[%]","System[%]","Idle[%]","IO wait[%]","IRQ","SoftIRQ","Nice");
    ::scrc_put_chars(m_display,txt,INVERSE,++line,1,1);
    for(_C::const_iterator ic=cores.begin(); ic!=cores.end(); ic=cores.next(ic)) {
      const CPU& c = *ic;
      ::sprintf(txt,"%3d:  %9.0f %5d %9.0f %9.3f %9.3f %9.3f %10.3f %9.3f %9.3f %9.0f",
		++cnt, c.clock,c.cache,c.bogomips,c.stats.user,c.stats.system,c.stats.idle,
		c.stats.iowait,c.stats.IRQ,c.stats.softIRQ,c.stats.nice);
      ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);
    }
  }
  if ( line == 1 ) {
    t1 = ::time(0);
    ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
    ::sprintf(txt,"   Unknown Node '%s'. No corresponding CPU information found.",m_name.c_str());
    ::scrc_put_chars(m_display,txt, INVERSE|BOLD,++line,5,1);
    ::strftime(txt,sizeof(txt),"         %H:%M:%S",::localtime(&t1));
    ::scrc_put_chars(m_display,txt,INVERSE|BOLD,++line,5,1);
    ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED|BOLD);
  }
}

FarmSubDisplay::FarmSubDisplay(FarmDisplay* parent, const std::string& title, bool bad) 
: InternalDisplay(parent, title)
{
  m_evtSent  = m_totSent = 0;
  m_evtMoore = m_totMoore = 0;
  m_evtBuilt = m_totBuilt = 0;
  m_lastUpdate = time(0);
  ::scrc_create_display(&m_display,4,48,NORMAL,ON,m_title.c_str());
  init(bad);
  std::string svc = "/";
  for(size_t i=0; i<title.length();++i) svc += ::tolower(title[i]);
  svc += "/ROpublish";
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
  m_hasProblems = false;
}

FarmSubDisplay::~FarmSubDisplay() {
}

void FarmSubDisplay::init(bool bad) {
  int col = bad ? INVERSE|RED : NORMAL;
  char txt[128];
  ::sprintf(txt,"%-4s%9s %3s%9s %2s%4s%9s%4s",
	    "","MEP","Sl","EVENT","Cl","Sl","SEND","Sl");
  ::scrc_put_chars(m_display,txt,col|INVERSE,1,1,1);
  //::scrc_put_chars(m_display,txt,col|BOLD,1,1,1);
  ::scrc_put_chars(m_display," ",col,2,1,1);
  ::scrc_put_chars(m_display," ",col,3,1,1);
  ::scrc_put_chars(m_display," ",col,4,1,1);
  ::scrc_put_chars(m_display,"Tot:",BOLD,2,1,1);
  ::scrc_put_chars(m_display,"Min:",BOLD,3,1,1);
  ::scrc_put_chars(m_display,"Max:",BOLD,4,1,1);
  ::scrc_put_chars(m_display,"  ",col,2,46,0);
  ::scrc_put_chars(m_display,"  ",col,3,45,0);
  ::scrc_put_chars(m_display,"  ",col,4,44,0);
  ::scrc_set_border(m_display,m_title.c_str(),col|BOLD);
}

/// DIM command service callback
void FarmSubDisplay::update(const void* address) {
  const Nodeset* ns = (const Nodeset*)address;
  if ( ns->type == Nodeset::TYPE ) {
    updateContent(*ns);
  }
}

void FarmSubDisplay::setTimeoutError() {
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
  ::scrc_put_chars(m_display," No update information available",BOLD|RED|INVERSE,4,1,1);
}

/// Update display content
void FarmSubDisplay::updateContent(const Nodeset& ns) {
  char txt[128], text[128];
  int evt_prod[3]    = {0,0,0}, min_prod[3]  = {std::numeric_limits<int>::max(),std::numeric_limits<int>::max(),std::numeric_limits<int>::max()};
  int free_space[3]  = {0,0,0}, min_space[3] = {std::numeric_limits<int>::max(),std::numeric_limits<int>::max(),std::numeric_limits<int>::max()};
  int used_slots[3]  = {0,0,0}, min_slots[3] = {std::numeric_limits<int>::max(),std::numeric_limits<int>::max(),std::numeric_limits<int>::max()};
  int buf_clients[3] = {0,0,0};
  float fspace[3]    = {std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),std::numeric_limits<float>::max()};
  float fslots[3]    = {std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),std::numeric_limits<float>::max()};
  int evt_sent       = std::numeric_limits<int>::max();
  int evt_moore      = std::numeric_limits<int>::max();
  int evt_built      = std::numeric_limits<int>::max();
  bool inuse         = false;
  int numNodes       = 0;
  int numBuffs       = 0;
  std::set<std::string> bad_nodes;
  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const Buffers& buffs = *(*n).buffers();
    numNodes++;
    int node_evt_mep = std::numeric_limits<int>::min();
    int node_evt_sent = std::numeric_limits<int>::max();
    int node_evt_moore = std::numeric_limits<int>::max();
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      int idx = 0;
      char b = (*ib).name[0];
      const MBMBuffer::Control& ctrl = (*ib).ctrl;
      inuse = true;
      numBuffs++;
      switch(b) {
      case MEP_BUFFER:	idx = 0; break;
      case EVT_BUFFER:	idx = 1; break;
      case RES_BUFFER:
      case SND_BUFFER:	idx = 2; break;
      default:
	continue;
      }
      fspace[idx]       = std::min(fspace[idx],float(ctrl.i_space)/float(ctrl.bm_size)); 
      fslots[idx]       = std::min(fslots[idx],float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax));
      min_space[idx]    = std::min(min_space[idx],(ctrl.i_space*ctrl.bytes_p_Bit)/1024/1024);
      min_slots[idx]    = std::min(min_slots[idx],ctrl.p_emax-ctrl.i_events);
      min_prod[idx]     = std::min(min_prod[idx],ctrl.tot_produced);
      evt_prod[idx]    += ctrl.tot_produced;
      free_space[idx]  += (ctrl.i_space*ctrl.bytes_p_Bit)/1024/1024;
      used_slots[idx]  += (ctrl.p_emax-ctrl.i_events);
      buf_clients[idx] += ctrl.i_users;
      if ( fslots[idx] < SLOTS_MIN || fspace[idx] < SPACE_MIN ) {
	bad_nodes.insert((*n).name);
      }
      const Clients& clients = (*ib).clients;
      for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
	char* p = strchr((*ic).name,'_');
	if ( p ) {
	  switch(*(++p)) {
	  case BUILDER_TASK:
	    if( b == MEP_BUFFER ) {
	      node_evt_mep = std::max(node_evt_mep,(*ic).events);
	    }
	    break;
	  case SENDER_TASK:
	    if( b == RES_BUFFER || b == SND_BUFFER )  {
	      node_evt_sent = std::min(node_evt_sent,(*ic).events);
	    }
	    break;
	  case MOORE_TASK:
	    if( b == EVT_BUFFER )  {
	      node_evt_moore = std::min(node_evt_moore,(*ic).events);
	    }
	    break;
	  default:
	    break;
	  }
	}
      }
    }
    evt_moore = std::min(evt_moore,node_evt_moore);
    evt_built = std::min(evt_built,node_evt_mep);
    evt_sent  = std::min(evt_sent,node_evt_sent);
  }
  char b1[64];
  Nodeset::TimeStamp frst=ns.firstUpdate();
  time_t t1 = numNodes == 0 ? time(0) : frst.first, now = time(0);
  ::strftime(b1,sizeof(b1),"%H:%M:%S",::localtime(&t1));
  ::sprintf(text,"  %s %s [%d nodes %d buffers] ",m_name.c_str(),b1,numNodes,numBuffs);
  m_title = text;
  if ( numNodes != 0 ) {
    m_lastUpdate = t1;
  }
  m_hasProblems = true;
  if ( numNodes == 0 ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::scrc_put_chars(m_display," No nodes found in this subfarm!",BOLD|RED|INVERSE,4,1,1);
  }
  else if ( now-m_lastUpdate > UPDATE_TIME_MAX ) {
    setTimeoutError();
  }
  else if ( fslots[0] < SLOTS_MIN || fslots[1] < SLOTS_MIN || fslots[2] < SLOTS_MIN ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::sprintf(txt," SLOTS at limit:");
    if ( fslots[0] < SLOTS_MIN ) ::strcat(txt,"MEP ");
    if ( fslots[1] < SLOTS_MIN ) ::strcat(txt,"EVENT ");
    if ( fslots[2] < SLOTS_MIN ) ::strcat(txt,"RES/SEND ");
    ::sprintf(text,"[%d nodes]",int(bad_nodes.size()));
    ::strcat(txt,text);
    ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,4,1,1);
  }
  else if ( fspace[0] < SPACE_MIN || fspace[1] < SPACE_MIN || fspace[2] < SPACE_MIN  ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::sprintf(txt," SPACE at limit:");
    if ( fspace[0] < SPACE_MIN ) ::strcat(txt,"MEP ");
    if ( fspace[1] < SPACE_MIN ) ::strcat(txt,"EVENT ");
    if ( fspace[2] < SPACE_MIN ) ::strcat(txt,"RES/SEND ");
    ::sprintf(text,"[%d nodes]",int(bad_nodes.size()));
    ::strcat(txt,text);
    ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,4,1,1);
  }
  else if ( evt_built <= m_evtBuilt && evt_prod[0] > m_totBuilt ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::sprintf(txt," Some MEPRx(s) stuck.");
    ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,4,1,1);
  }
  else if ( inuse && evt_built <= m_evtBuilt && evt_prod[0] == m_totBuilt ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No DAQ activity visible.",BOLD|RED,4,1,1);
  }
  else if ( evt_moore <= m_evtMoore && evt_prod[1] > m_totMoore ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::scrc_put_chars(m_display," Some MOORE(s) stuck.",BOLD|RED|INVERSE,4,1,1);
  }
  else if ( inuse && evt_moore <= m_evtMoore && evt_prod[1] == m_totMoore ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No HLT activity visible.",BOLD|RED,4,1,1);
  }
  else if ( evt_sent <= m_evtSent && evt_prod[2] > m_totSent ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::scrc_put_chars(m_display," Some Sender(s) stuck.",BOLD|RED|INVERSE,4,1,1);
  }
  else if ( inuse && evt_sent <= m_evtSent && evt_prod[0] == m_totSent ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No STORAGE activity visible.",BOLD|RED,4,1,1);
  }
  else {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display,"",NORMAL,4,1,1);
    ::scrc_put_chars(m_display," No obvious Errors detected....",NORMAL|INVERSE|GREEN,4,1,1);
    m_hasProblems = false;
  }
  m_evtBuilt = evt_built;
  m_evtMoore = evt_moore;
  m_evtSent  = evt_sent;
  m_totBuilt = evt_prod[0];
  m_totMoore = evt_prod[1];
  m_totSent  = evt_prod[2];
  if ( evt_prod[0] != 0 )
    ::sprintf(txt,"%9d%4d%9d %2d%4d%9d%4d",
	      evt_prod[0],used_slots[0],
	      evt_prod[1],buf_clients[1],used_slots[1],
	      evt_prod[2],used_slots[2]);
  else
    ::sprintf(txt,"%9s%4s%9s  %5s%9s%4s","--","--","--","--","--","--");
  ::scrc_put_chars(m_display,txt,NORMAL,2,5,1);
  if ( min_prod[0] != std::numeric_limits<int>::max() )
    ::sprintf(txt,"%9d%4d%9d   %4d%9d%4d",
	      min_prod[0],min_slots[0],
	      min_prod[1],min_slots[1],
	      min_prod[2],min_slots[2]);
  else
    ::sprintf(txt,"%9s%4s%9s  %5s%9s%4s","--","--","--","--","--","--");
  ::scrc_put_chars(m_display,txt,NORMAL,3,5,1);
  IocSensor::instance().send(m_parent,CMD_CHECK,this);
}

BufferDisplay::BufferDisplay(FarmDisplay* parent, const std::string& title) 
  : InternalDisplay(parent,title), m_node(0)
{
  ::scrc_create_display(&m_display,45,130,MAGENTA,ON,"MBM Monitor display for node:");
}

void BufferDisplay::update(const void* data) {
  const Nodeset* ns = (const Nodeset*)data;
  if ( 0 != ns ) {
    StringV lines;
    std::string nam;
    int line = 0, node = 0;
    char txt[1024], name[128];
    Nodes::const_iterator n;

    for (n=ns->nodes.begin(), line=1; n!=ns->nodes.end(); n=ns->nodes.next(n), ++node)  {
      if ( node == m_node ) {
	time_t tim = (*n).time;
	const Buffers& buffs = *(*n).buffers();
	::strftime(name,sizeof(name),"%H:%M:%S",::localtime(&tim));
	::sprintf(txt,"MBM Monitor display for node:%s  [%s]",(*n).name,name);
	::scrc_set_border(m_display,txt,NORMAL|BOLD);
	for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
	  const Buffers::value_type::Control& c = (*ib).ctrl;
	  sprintf(name," Buffer \"%s\"",(*ib).name);
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
      ::sprintf(txt,"Unknown Node. No buffers found.");
      ::scrc_put_chars(m_display,txt,INVERSE|BOLD,++line,1,1);
      ::strftime(txt,sizeof(txt),"           %H:%M:%S",::localtime(&t));
      ::scrc_put_chars(m_display,txt,INVERSE|BOLD,++line,1,1);
      ::scrc_set_border(m_display,txt,INVERSE|RED|BOLD);
    }
    for (n=ns->nodes.begin(), node=0; n!=ns->nodes.end(); n=ns->nodes.next(n), ++node)  {
      if ( node == m_node ) {
	const Buffers& buffs = *(*n).buffers();
	for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
	  const Clients& clients = (*ib).clients;
	  for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
	    Clients::const_reference c = (*ic);
	    if ( c.type == 'C' )
	      ::sprintf(txt,"%-20s%5X%6d C%6s%12d %c%c%c%c %s",c.name,c.partitionID,c.processID,
			sstat[(size_t)c.state],c.events,c.reqs[0],c.reqs[1],c.reqs[2],c.reqs[3],
			(*ib).name);
	    else if ( c.type == 'P' )
	      ::sprintf(txt,"%-20s%5X%6d P%6s%12d %4s %s",c.name,c.partitionID,c.processID,
			sstat[(size_t)c.state],c.events,"",(*ib).name);
	    else
	      ::sprintf(txt,"%-20s%5X%6d ?%6s%12s %4s %s",c.name,c.partitionID,c.processID,
			"","","",(*ib).name);
	    lines.push_back(txt);
	  }
	}
	break;
      }
    }
    for(StringV::const_iterator j,i=lines.begin(); i!=lines.end(); ++i) {
      j = i;
      if ( ++j != lines.end() ) {
	::sprintf(name,"%%-%ds  %%-%ds",m_display->cols/2,m_display->cols/2);
	::sprintf(txt,name,(*i).c_str(),(*j).c_str());
	i = j;
      }
      else {
	::sprintf(name,"%%-%ds",m_display->cols);
	::sprintf(txt,name,(*i).c_str());
      }
      ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
    }
    ::memset(txt,' ',m_display->cols);
    txt[m_display->cols-1]=0;
    while(line<m_display->rows)
      ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
  }
}

/// Standard constructor with object setup through parameters
PartitionListener::PartitionListener(Interactor* parent,const std::string& nam) : m_parent(parent), m_name(nam)
{
  std::string name = "RunInfo/" + m_name + "/HLTsubFarms";
  m_subFarmDP = ::dic_info_service((char*)name.c_str(),MONITORED,0,0,0,subFarmHandler,(long)this,0,0);
}

/// Standard destructor
PartitionListener::~PartitionListener() {
  ::dic_release_service(m_subFarmDP);
}

/// DIM command service callback
void PartitionListener::subFarmHandler(void* tag, void* address, int* size) {
  std::string svc;
  std::auto_ptr<StringV > f(new StringV());
  PartitionListener* h = *(PartitionListener**)tag;
  for(const char* data = (char*)address, *end=data+*size;data<end;data += strlen(data)+1)
    f->push_back(data);
  f->push_back("HLTA08");
  for(StringV::iterator i=f->begin(); i != f->end(); ++i) {
    std::string& s = *i;
    for(size_t j=0; j<s.length(); ++j) {
      s[j] = ::tolower(s[j]);
    }
  }
  IocSensor::instance().send(h->m_parent,CMD_CONNECT,f.release());
}

/// Standard constructor
FarmDisplay::FarmDisplay(int argc, char** argv)
  : InternalDisplay(0,""), m_subfarmDisplay(0), 
    m_posCursor(0), m_subPosCursor(0)
{
  bool all = false;
  char txt[128];
  RTL::CLI cli(argc,argv,help);
  cli.getopt("partition",   1, m_name = "LHCb");
  m_dense = 0 != cli.getopt("dense",1);
  all = 0 != cli.getopt("all",1);
  s_fd = this;
  ::lib_rtl_create_lock(0,&s_lock);
  if ( all ) 
    ::sprintf(txt," Farm display of all known subfarms ");
  else
    ::sprintf(txt," Farm display of partition %s ",m_name.c_str());
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
  if ( all ) {
    m_svc = ::dic_info_service("DIS_DNS/SERVER_LIST",MONITORED,0,0,0,dataHandler,(long)this,0,0);
  }
  else {
    m_listener = std::auto_ptr<PartitionListener>(new PartitionListener(this,m_name));
  }
}

/// Standard destructor
FarmDisplay::~FarmDisplay()  {  
  ::wtc_remove(WT_FACILITY_SCR);
  disconnect();
  m_listener = std::auto_ptr<PartitionListener>(0);
  ::scrc_begin_pasteboard_update(m_pasteboard);
  m_mbmDisplay = std::auto_ptr<BufferDisplay>(0);
  if ( m_subfarmDisplay ) {
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
void FarmDisplay::set_cursor() {
  if ( 0 == m_subfarmDisplay ) {
    FarmSubDisplay* d = currentDisplay();
    if ( d ) ::scrc_set_cursor(d->display(),2, 2);
  }
  else {
    Display* d1 = m_subfarmDisplay->display();
    if ( d1 ) ::scrc_set_cursor(d1, m_subPosCursor, 2);
  }
}

/// Get farm display from cursor position
FarmSubDisplay* FarmDisplay::currentDisplay() {
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
  FarmSubDisplay* d = 0;
  if ( m_subfarmDisplay ) {
    DisplayUpdate update(this,true);
    m_subfarmDisplay->finalize();
    delete m_subfarmDisplay;
    m_subfarmDisplay = 0;
    m_cpuDisplay = std::auto_ptr<CPUDisplay>(0);
    m_mbmDisplay = std::auto_ptr<BufferDisplay>(0);
    m_procDisplay = std::auto_ptr<ProcessDisplay>(0);
    m_subPosCursor = 8;
  }
  else if ( (d=currentDisplay()) != 0 ) {
    std::string title = "Sub farm info:" + d->name();
    std::string svc = "-servicename=/"+d->name()+"/ROpublish";
    const char* argv[] = {"",svc.c_str(), "-delay=300"};
    m_subfarmDisplay = new SubfarmDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,10,20,3,(char**)argv);
    m_subfarmDisplay->initialize();
    ::lib_rtl_sleep(200);
    IocSensor::instance().send(this,CMD_UPDATE,m_subfarmDisplay);
    m_subPosCursor = SUBFARM_NODE_OFFSET;
  }
  return WT_SUCCESS;
}

/// Handle keyboard interrupts
int FarmDisplay::handleKeyboard(int key)    {
  int cnt, col, row;
  FarmSubDisplay* d = 0;
  Display* d1;
  SubDisplays& sd = subDisplays();
  RTL::Lock lock(s_lock);
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
    case CTRL_H: {
      DisplayUpdate update(this,true);
      if ( m_helpDisplay.get() )
	m_helpDisplay = std::auto_ptr<HelpDisplay>(0);
      else if ( m_mbmDisplay.get() ) 
	m_helpDisplay = std::auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","mbm"));
      else if ( m_procDisplay.get() ) 
	m_helpDisplay = std::auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","procs"));
      else if ( m_cpuDisplay.get() ) 
	m_helpDisplay = std::auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","cpu"));
      else if ( m_subfarmDisplay )
	m_helpDisplay = std::auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","subfarm"));
      else
	m_helpDisplay = std::auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","farm"));
      if ( m_helpDisplay.get() ) m_helpDisplay->show(15,25);
      return WT_SUCCESS;
    }
    case 'c':
    case 'C':
      if ( m_subfarmDisplay ) {
	DisplayUpdate update(this,true);
	if ( m_cpuDisplay.get() ) {
	  m_cpuDisplay = std::auto_ptr<CPUDisplay>(0);
	}
	else if ( m_subfarmDisplay ) {
	  const Nodeset* ns = (const Nodeset*)m_subfarmDisplay->data().pointer;
	  if ( ns ) {
	    m_cpuDisplay = std::auto_ptr<CPUDisplay>(new CPUDisplay(this,ns->name));
	    m_cpuDisplay->show(30,22);
	    return WT_SUCCESS;
	  }
	}
      }
      return WT_SUCCESS;
    case 'm':
    case 'M':
    case '.':
    case KPD_PERIOD:
      if ( m_subfarmDisplay ) {
	DisplayUpdate update(this,true);
	if ( m_mbmDisplay.get() ) {
	  m_mbmDisplay = std::auto_ptr<BufferDisplay>(0);
	}
	else if ( m_subfarmDisplay ) {
	  m_mbmDisplay = std::auto_ptr<BufferDisplay>(new BufferDisplay(this,"MBM Monitor display"));
	  m_mbmDisplay->setNode(m_subPosCursor-SUBFARM_NODE_OFFSET);
	  m_mbmDisplay->update(m_subfarmDisplay->data().pointer);
	  m_mbmDisplay->show(25,21);
	}
      }
      return WT_SUCCESS;
    case 'p':
    case 'P':
    case CTRL_P: {
      if ( m_procDisplay.get() ) {
	m_procDisplay = std::auto_ptr<ProcessDisplay>(0);
      }
      else if ( m_subfarmDisplay ) {
	const Nodeset* ns = (const Nodeset*)m_subfarmDisplay->data().pointer;
	if ( ns ) {
	  size_t cnt;
	  Nodes::const_iterator n;
	  const Nodes& nodes = ns->nodes;
	  for (n=nodes.begin(), cnt=0; n!=nodes.end(); n=nodes.next(n), ++cnt)  {
	    if ( cnt == m_subPosCursor-SUBFARM_NODE_OFFSET ) {
	      m_procDisplay = std::auto_ptr<ProcessDisplay>(new ProcessDisplay(this, (*n).name));
	      m_procDisplay->show(30,22);
	      return WT_SUCCESS;
	    }
	  }
	}
      }
      return WT_SUCCESS;
    }
    case RETURN_KEY:
    case ENTER: {
      RTL::Lock lock(s_lock,true);
      showSubfarm();
      return WT_SUCCESS;
    }
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
	    if ( d1->row == std::min(d1->row,row) ) {
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
	    if ( d1->row == std::max(d1->row,row) ) {
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

/// DIM command service callback
void FarmDisplay::update(const void* address) {
  char c, *msg = (char*)address;
  std::string svc, node;
  size_t idx, idq;
  switch(c=msg[0]) {
  case '+':
    getServiceNode(++msg,svc,node);
    idx = svc.find("/ROpublish");
    idq = svc.find("/hlt");
    if ( idx != std::string::npos && idq == 0 ) {
      std::string f = svc.substr(1,idx-1);
      IocSensor::instance().send(this,CMD_ADD,new std::string(f));
    }
    break;
  case '-':
    break;
  case '!':
    //getServiceNode(++msg,svc,node);
    //log() << "Service " << msg << " in ERROR." << std::endl;
    break;
  default:
    if ( *(int*)msg != *(int*)"DEAD" )  {
      char *at, *p = msg, *last = msg;
      std::auto_ptr<Farms> farms(new Farms);
      while ( last != 0 && (at=strchr(p,'@')) != 0 )  {
	last = strchr(at,'|');
	if ( last ) *last = 0;
	getServiceNode(p,svc,node);
	idx = svc.find("/ROpublish");
	idq = svc.find("/hlt");
	if ( idx != std::string::npos && idq == 0 ) {
	  std::string f = svc.substr(1,idx-1);
	  farms->push_back(f);
	}
	p = last+1;
      }
      if ( !farms->empty() )
	IocSensor::instance().send(this,CMD_CONNECT,farms.release());
    }
    break;
  }
}

/// Interactor overload: Display callback handler
void FarmDisplay::handle(const Event& ev) {
  time_t now = time(0);
  Farms::iterator i;
  SubDisplays::iterator k;
  RTL::Lock lock(s_lock);
  switch(ev.eventtype) {
  case TimeEvent:
    if (ev.timer_data == m_subfarmDisplay ) {
      m_subfarmDisplay->update();
      if ( m_mbmDisplay.get() ) {
	const void* data = m_subfarmDisplay->data().pointer;
	m_mbmDisplay->setNode(m_subPosCursor-SUBFARM_NODE_OFFSET);
	m_mbmDisplay->update(data);
      }
      TimeSensor::instance().add(this,1,m_subfarmDisplay);
    }
    return;
  case IocEvent:
    switch(ev.type) {
    case CMD_SETCURSOR:
      set_cursor();
      break;
    case CMD_UPDATE:
      if ( m_subfarmDisplay ) m_subfarmDisplay->update();
      if ( m_mbmDisplay.get() )  {
	const void* data = m_subfarmDisplay->data().pointer;
	m_mbmDisplay->setNode(m_subPosCursor-SUBFARM_NODE_OFFSET);
	m_mbmDisplay->update(data);
      }
      TimeSensor::instance().add(this,1,m_subfarmDisplay);
      break;
    case CMD_ADD:
      i = std::find(m_farms.begin(),m_farms.end(),*(std::string*)ev.data);
      if ( i == m_farms.end() ) {
	m_farms.push_back(*(std::string*)ev.data);
	connect(m_farms);
      }
      delete (std::string*)ev.data;
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
	FarmSubDisplay* d = (*k).second;
	if ( d != ev.data && !d->hasProblems() ) {
	  if ( now - d->lastUpdate() > UPDATE_TIME_MAX ) {
	    d->setTimeoutError();
	  }
	}
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

void FarmDisplay::connect(const std::vector<std::string>& farms) {
  SubDisplays::iterator k;
  SubDisplays copy;
  char txt[128];

  ::sprintf(txt,"Total number of subfarms:%d %100s",int(farms.size()),"<CTRL-H for Help>, <CTRL-E to exit>");
  ::scrc_put_chars(m_display,txt,NORMAL,1,2,1);

  for( k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k)
    (*k).second->hide();
  for (Farms::const_iterator i=farms.begin(); i != farms.end(); ++i) {
    k = m_farmDisplays.find(*i);
    if ( k == m_farmDisplays.end() ) {
      copy.insert(std::make_pair(*i,new FarmSubDisplay(this,*i)));
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
