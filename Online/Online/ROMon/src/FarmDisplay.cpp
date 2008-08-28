// $Id: FarmDisplay.cpp,v 1.22 2008-08-28 16:39:14 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/FarmDisplay.cpp,v 1.22 2008-08-28 16:39:14 frankb Exp $

#include "ROMon/CtrlSubfarmDisplay.h"
#include "ROMon/RecSubfarmDisplay.h"
#include "ROMon/SubfarmDisplay.h"
#include "ROMon/FarmDisplay.h"
#include "ROMon/CPUMon.h"
#include "SCR/MouseSensor.h"
#include "CPP/TimeSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"
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
#define vsnprintf _vsnprintf
#endif
using namespace ROMon;
using namespace SCR;

typedef Nodeset::Nodes               Nodes;
typedef Node::Buffers                Buffers;
typedef MBMBuffer::Clients           Clients;
typedef Node::Tasks                  Tasks;
#define BUILDER_TASK                 'M'
#define SENDER_TASK                  'D'
#define MOORE_TASK                   'G'

#define REC_RECEIVER_TASK            'R'
#define REC_SENDER_TASK              'S'
#define REC_TASK                     'B'

#define MEP_BUFFER                   'M'
#define RES_BUFFER                   'R'
#define EVT_BUFFER                   'E'
#define SND_BUFFER                   'S'
#define INPUT_BUFFER                 'I'
#define OUTPUT_BUFFER                'O'

#define SUBFARM_HEIGHT               65
#define SUBFARM_WIDTH               132
#define SUBFARM_NODE_OFFSET           8
#define SUBFARM_DISP_WIDTH           48
#define SUBFARM_DISP_HEIGHT_NORMAL    5
#define SUBFARM_DISP_HEIGHT_DENSE     4
#define SUBFARM_DISP_TOP              4
#define SUBFARM_DISP_LEFT             3
#define NUM_UPDATE_DIFF               5

#define SLOTS_MIN                     .1
#define SPACE_MIN                     .1
// Max. 15 seconds without update allowed
#define UPDATE_TIME_MAX 15

typedef std::vector<std::string> StringV;

static FarmDisplay* s_fd = 0;
static lib_rtl_lock_t    s_lock;
static const char *sstat[17] = {" nl", "   ", "*SL","*EV","*SP","WSL","WEV","WSP","wsl","wev","wsp"," ps"," ac", "SPR", "WER", "   "};
//static const int   INT_min = std::numeric_limits<int>::min();
static const int   INT_max = std::numeric_limits<int>::max();
static const float FLT_max = std::numeric_limits<float>::max();


static void help() {
  std::cout << "  romon_farm -option [-option]" << std::endl
	    << "       -all                         Show all subfarms." << std::endl
	    << "       -p[artition]=<name>          Partition name providing monitoring information." << std::endl
	    << "       -an[chor]=+<x-pos>+<ypos>    Set anchor for sub displays" << std::endl
	    << std::endl;
}

static std::string strUpper(const std::string& src) {
  std::string r=src;
  for(size_t i=0;i<r.length();++i)r[i]=::toupper(r[i]);
  return r;
}

namespace ROMon {
  InternalDisplay* createCtrlFarmSubDisplay(FarmDisplay* parent, const std::string& title);
}
namespace {
  const char* _procNam(const char* nam) {
    char* p;
    if (0 != ::strstr(nam,"MEPRx") ) return nam;
    p = ::strchr(nam,'_');
    if ( 0 != p ) return ++p;
    return "Unknown";
  }

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

/// Update display content
void InternalDisplay::update(const void* data, size_t /* len */) {
  return update(data);
}

/// DIM command service callback
void InternalDisplay::dataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    InternalDisplay* disp = *(InternalDisplay**)tag;
    unsigned char* ptr = new unsigned char[*size+sizeof(int)];
    *(int*)ptr = *size;
    ::memcpy(ptr+sizeof(int),address,*size);
    IocSensor::instance().send(disp,CMD_UPDATE,ptr);
  }
}

/// Interactor overload: Display callback handler
void InternalDisplay::handle(const Event& ev)    {
  switch(ev.eventtype) {
  case IocEvent: {
    switch(ev.type) {
    case CMD_UPDATE: {
      RTL::Lock lock(s_lock);
      Pasteboard* pb = pasteboard();
      unsigned char* ptr = (unsigned char*)ev.data;
      ::scrc_cursor_off(pb);
      ::scrc_begin_pasteboard_update (pb);
      update(ptr + sizeof(int), *(int*)ptr);
      ::scrc_end_pasteboard_update(pb);
      if ( parent() ) parent()->set_cursor();
      ::scrc_cursor_on(pb);
      delete [] ptr;
      break;
    }
    default:
      break;
    }
    break;
  }
  default:
    break;
  }
}

HelpDisplay::HelpDisplay(FarmDisplay* parent, const std::string& title, const std::string& tag) 
: InternalDisplay(parent,title)
{
  bool use = false, isHeader=false;
  std::string s, input = ::getenv("ROMONROOT") != 0 ? ::getenv("ROMONROOT") : "..";
  std::string start="<"+tag+">", stop = "</"+tag+">";
  std::string fin = input+"/doc/farmMon.hlp";
  std::string head = m_title + ": " + fin;
  std::ifstream in(fin.c_str());

  ::scrc_create_display(&m_display,55,132,NORMAL,ON,head.c_str());
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
  ::scrc_set_border(m_display,head.c_str(),INVERSE|BLUE);
}


BufferDisplay::BufferDisplay(FarmDisplay* parent, const std::string& title) 
  : InternalDisplay(parent,title), m_node(0)
{
  ::scrc_create_display(&m_display,55,130,MAGENTA,ON,"MBM Monitor display for node:");
}

void BufferDisplay::update(const void* data) {
  const Nodeset* ns = (const Nodeset*)data;
  if ( 0 != ns ) {
    std::string key;
    std::map<std::string,std::string> entries;
    StringV lines;
    std::string nam;
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
	  if ( ::strlen(bnam)>10 ) {
	    p = strchr(bnam,'_');
	    if ( p ) *p = 0;
	  }
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
    for(std::map<std::string,std::string>::const_iterator m=entries.begin();m!=entries.end();++m) {
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

CtrlDisplay::CtrlDisplay(FarmDisplay* parent, const std::string& title) 
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
    const char* fmt = "  %-24s %12s %17zd %17zd    %s";

    ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
    ::sprintf(txt, "  Cluster:%s  status:%12s  last update:%s",c->name.c_str(),c->status.c_str(),c->time.c_str());
    ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
    ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
    for(Cluster::Nodes::const_iterator i=n.begin(); i!=n.end();++i, ++node) {
      if ( node == m_node ) {
	const Cluster::Node& n = (*i).second;
	Cluster::Node::Tasks::const_iterator j;
	::strftime(name,sizeof(name),"%H:%M:%S",::localtime(&tim));
	::sprintf(txt,"Task Control display for node:%s  [%s]",n.name.c_str(),name);
	::scrc_set_border(m_display,txt,BLUE|INVERSE);
	::sprintf(txt,"  %-24s %12s %17s %17s    %s","Node","Status","Found Tasks","Missing tasks","Timestamp");
	::scrc_put_chars(m_display,txt,INVERSE,++line,1,1);
	::sprintf(txt,fmt,n.name.c_str(),n.status.c_str(),n.taskCount,n.missCount,n.time.c_str());
	::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
	if ( n.status == "DEAD" ) {
	  ::scrc_put_chars(m_display,"",INVERSE|RED|BOLD,++line,1,1);
	  ::scrc_put_chars(m_display,"  -------------------------------------",INVERSE|RED|BOLD,++line,1,1);
	  ::scrc_put_chars(m_display,"  Node is DEAD. Severe problem present.",INVERSE|RED|BOLD,++line,1,1);
	  ::scrc_put_chars(m_display,"  -------------------------------------",INVERSE|RED|BOLD,++line,1,1);
	  ::scrc_put_chars(m_display,"",INVERSE|RED|BOLD,++line,1,1);
	}
	else if ( n.missing.size() > 0 ) {
	  ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
	  ::scrc_put_chars(m_display,"  Missing tasks:",INVERSE|BOLD|RED,++line,1,1);
	  for(j=n.missing.begin(); j != n.missing.end(); ++j,++cnt) {
	    ::sprintf(txt,"   [%d]  %s",cnt,(*j).c_str());
	    ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
	  }
	}
	else {
	  ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
	  ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
	  ::scrc_put_chars(m_display,"  Node looks fine. No missing tasks found.",INVERSE|GREEN,++line,1,1);
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
		"UTGID","PID","PPID","Memory[%]","VSize[kB]","RSS[kB]","CPU[%]","Threads");
      ::scrc_put_chars(m_display,txt,INVERSE,++line,1,1);
      for(_P::const_iterator ip=procs.begin(); ip!=procs.end(); ip=procs.next(ip)) {
	const Process& p = *ip;
	::sprintf(txt,"%3d:  %-32s %5d %5d %9.3f %9.0f %9.0f %6.2f %7d",
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
  if ( 0 == f.nodes.size() ) {
    t1 = ::time(0);
    ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
    ::strftime(txt,sizeof(txt),"   No CPU information found.         %H:%M:%S",::localtime(&t1));
    ::scrc_put_chars(m_display,txt,INVERSE|BOLD,++line,5,1);
    ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED|BOLD);
  }
}

/// Initializing constructor
FarmSubDisplay::FarmSubDisplay(FarmDisplay* parent, const std::string& title, bool bad) 
: InternalDisplay(parent, title)
{
  m_numUpdate = 0;
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
  MouseSensor::instance().add(this,m_display);
}

/// Standard destructor
FarmSubDisplay::~FarmSubDisplay() {
  MouseSensor::instance().remove(m_display);
}

/// Initialize default display text
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

/// Set timeout error
void FarmSubDisplay::setTimeoutError() {
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
  ::scrc_put_chars(m_display," No update information available",BOLD|RED|INVERSE,4,1,1);
}

/// Check display for errors
void FarmSubDisplay::check(time_t now) {
  if ( hasProblems() ) {
    if ( now - lastUpdate() > UPDATE_TIME_MAX ) {
      setTimeoutError();
    }
  }
}

/// Set the focus to this display
void FarmSubDisplay::setFocus() {
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
}

/// Release the focus of this display
void FarmSubDisplay::releaseFocus() {
}

/// Interactor overload: Display callback handler
void FarmSubDisplay::handle(const Event& ev) {
  switch(ev.eventtype) {
  case ScrMouseEvent: {
    const MouseEvent* m = ev.get<MouseEvent>();
    if ( m->button == 2 ) {
      IocSensor::instance().send(parent(),CMD_SHOWHELP,this);
      return;
    }
    setFocus();
    IocSensor::instance().send(parent(),m->msec == (unsigned int)-1 ? CMD_POSCURSOR : CMD_SHOW,this);
    return;
  }
  default:
    break;
  }
  InternalDisplay::handle(ev);
}

/// Update display content
void FarmSubDisplay::updateContent(const Nodeset& ns) {
  char txt[128], text[128];
  int evt_prod[3]    = {0,0,0}, min_prod[3]  = {INT_max,INT_max,INT_max};
  int free_space[3]  = {0,0,0}, min_space[3] = {INT_max,INT_max,INT_max};
  int used_slots[3]  = {0,0,0}, min_slots[3] = {INT_max,INT_max,INT_max};
  int buf_clients[3] = {0,0,0};
  float fspace[3]    = {FLT_max,FLT_max,FLT_max};
  float fslots[3]    = {FLT_max,FLT_max,FLT_max};
  int evt_sent       = INT_max;
  int evt_moore      = INT_max;
  int evt_built      = INT_max;
  bool inuse         = false;
  int numNodes       = 0;
  int numBuffs       = 0;
  int numClients     = 0;
  std::set<std::string> bad_nodes;

  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const Buffers& buffs = *(*n).buffers();
    numNodes++;
    int node_evt_mep = 0;
    int node_evt_sent = INT_max;
    int node_evt_moore = INT_max;
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
	++numClients;
	const char* p = _procNam((*ic).name);
	switch(*p) {
	case BUILDER_TASK:
	  if( b == MEP_BUFFER ) {
	    node_evt_mep += (*ic).events;
	  }
	  break;
	case SENDER_TASK:
	  if( b == RES_BUFFER || b == SND_BUFFER )  {
	    node_evt_sent = std::min(node_evt_sent,(*ic).events);
	  }
	  break;
	case MOORE_TASK:
	  //  Normal  and        TAE event processing
	  if( b == EVT_BUFFER || b == MEP_BUFFER )  {
	    node_evt_moore = std::min(node_evt_moore,(*ic).events);
	  }
	  break;
	default:
	  break;
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
  ::sprintf(text," %s %s [%d nodes %d buffers %d clients] ",
	    m_name.c_str(),b1,numNodes,numBuffs,numClients);
  m_title = text;
  m_inUse = inuse;

  if ( numNodes != 0 ) {
    m_lastUpdate = t1;
  }
  m_hasProblems = true;
  if ( now-m_lastUpdate > UPDATE_TIME_MAX ) {
    setTimeoutError();
  }
  else if ( numNodes == 0 ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::scrc_put_chars(m_display," No nodes found in this subfarm!",BOLD|RED|INVERSE,4,1,1);
  }
  else if ( !inuse ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," Subfarm not used by any partition....",NORMAL|INVERSE|GREEN,4,1,1);
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
  else if ( evt_built <= m_evtBuilt && evt_prod[0]<m_totBuilt ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::sprintf(txt," Some MEPRx(s) stuck.");
    ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,4,1,1);
  }
  else if ( evt_built <= m_evtBuilt && evt_prod[0] == m_totBuilt ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No DAQ activity visible.",BOLD|RED,4,1,1);
  }
  else if ( evt_moore+2 <= m_evtMoore && evt_prod[1] > m_totMoore ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::scrc_put_chars(m_display," Some MOORE(s) stuck.",BOLD|RED|INVERSE,4,1,1);
  }
  else if ( evt_moore <= m_evtMoore && evt_prod[1] == m_totMoore ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No HLT activity visible.",BOLD|RED,4,1,1);
  }
  else if ( evt_sent+2 <= m_evtSent && evt_prod[2] > m_totSent ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::scrc_put_chars(m_display," Some Sender(s) stuck.",BOLD|RED|INVERSE,4,1,1);
  }
  else if ( evt_sent <= m_evtSent && evt_prod[0] == m_totSent ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No STORAGE activity visible.",BOLD|RED,4,1,1);
  }
  else {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No obvious Errors detected....",NORMAL|INVERSE|GREEN,4,1,1);
    m_hasProblems = false;
  }
  m_evtBuilt  = evt_built;
  m_evtMoore  = evt_moore;
  m_evtSent   = evt_sent;
  m_totBuilt  = evt_prod[0];
  m_totMoore  = evt_prod[1];
  m_totSent   = evt_prod[2];

  if ( evt_prod[0] != 0 )
    ::sprintf(txt,"%9d%4d%9d %2d%4d%9d%4d",
	      evt_prod[0],used_slots[0],
	      evt_prod[1],buf_clients[1],used_slots[1],
	      evt_prod[2],used_slots[2]);
  else
    ::sprintf(txt,"%9s%4s%9s  %5s%9s%4s","--","--","--","--","--","--");
  ::scrc_put_chars(m_display,txt,NORMAL,2,5,1);
  if ( min_prod[0] != INT_max )
    ::sprintf(txt,"%9d%4d%9d   %4d%9d%4d",
	      min_prod[0],min_slots[0],
	      min_prod[1],min_slots[1],
	      min_prod[2],min_slots[2]);
  else
    ::sprintf(txt,"%9s%4s%9s  %5s%9s%4s","--","--","--","--","--","--");
  ::scrc_put_chars(m_display,txt,NORMAL,3,5,1);
  IocSensor::instance().send(m_parent,CMD_CHECK,this);
}

/// Initializing constructor
RecFarmSubDisplay::RecFarmSubDisplay(FarmDisplay* parent, const std::string& title, bool bad) 
: InternalDisplay(parent, title)
{
  m_evtSent = m_totSent = 0;
  m_evtReco = m_totReco = 0;
  m_evtRecv = m_totRecv = 0;
  m_lastUpdate = time(0);
  ::scrc_create_display(&m_display,4,48,NORMAL,ON,m_title.c_str());
  init(bad);
  std::string svc = "/";
  for(size_t i=0; i<title.length();++i) svc += ::tolower(title[i]);
  svc += "/ROpublish";
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
  m_hasProblems = false;
  MouseSensor::instance().add(this,m_display);
}

/// Standard destructor
RecFarmSubDisplay::~RecFarmSubDisplay() {
  MouseSensor::instance().remove(m_display);
}

/// Initialize default display text
void RecFarmSubDisplay::init(bool bad) {
  int col = bad ? INVERSE|RED : NORMAL;
  char txt[128];
  ::sprintf(txt,"%-4s%9s%4s%6s   %9s%4s%6s","","INPUT","Cl","Slots","OUTPUT","Cl","Slots");
  ::scrc_put_chars(m_display,txt,col|INVERSE,1,1,1);
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
void RecFarmSubDisplay::update(const void* address) {
  const Nodeset* ns = (const Nodeset*)address;
  if ( ns->type == Nodeset::TYPE ) {
    updateContent(*ns);
  }
}

/// Set timeout error
void RecFarmSubDisplay::setTimeoutError() {
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
  ::scrc_put_chars(m_display," No update information available",BOLD|RED|INVERSE,4,1,1);
}

/// Check display for errors
void RecFarmSubDisplay::check(time_t now) {
  if ( hasProblems() ) {
    if ( now - lastUpdate() > UPDATE_TIME_MAX ) {
      setTimeoutError();
    }
  }
}

/// Set the focus to this display
void RecFarmSubDisplay::setFocus() {
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
}

/// Release the focus of this display
void RecFarmSubDisplay::releaseFocus() {
}

/// Interactor overload: Display callback handler
void RecFarmSubDisplay::handle(const Event& ev) {
  switch(ev.eventtype) {
  case ScrMouseEvent: {
    const MouseEvent* m = ev.get<MouseEvent>();
    setFocus();
    IocSensor::instance().send(parent(),m->msec == (unsigned int)-1 ? CMD_POSCURSOR : CMD_SHOW,this);
    return;
  }
  default:
    break;
  }
  InternalDisplay::handle(ev);
}

/// Update display content
void RecFarmSubDisplay::updateContent(const Nodeset& ns) {
  char txt[128], text[128];
  int evt_prod[2]    = {0,0}, min_prod[2]  = {INT_max,INT_max};
  int free_space[2]  = {0,0}, min_space[2] = {INT_max,INT_max};
  int used_slots[2]  = {0,0}, min_slots[2] = {INT_max,INT_max};
  int buf_clients[2] = {0,0};
  float fspace[2]    = {FLT_max,FLT_max};
  float fslots[2]    = {FLT_max,FLT_max};
  int evt_sent       = INT_max;
  int evt_reco       = INT_max;
  int evt_recv       = INT_max;
  bool inuse         = false;
  int numNodes       = 0;
  int numBuffs       = 0;
  int numClients     = 0;
  std::set<std::string> bad_nodes;

  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const Buffers& buffs = *(*n).buffers();
    numNodes++;
    int node_evt_recv = 0;
    int node_evt_sent = INT_max;
    int node_evt_reco = INT_max;
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      int idx = 0;
      char b = (*ib).name[0];
      const MBMBuffer::Control& ctrl = (*ib).ctrl;
      inuse = true;
      numBuffs++;
      switch(b) {
      case INPUT_BUFFER:	idx = 0; break;
      case OUTPUT_BUFFER:	idx = 1; break;
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
	++numClients;
	char* p = ::strchr((*ic).name,'_');
	if ( p ) p = ::strchr(++p,'_');
	if ( p ) {
	  if ( b==INPUT_BUFFER && p[4] == REC_TASK )  {
	    node_evt_reco = std::min(node_evt_reco,(*ic).events);
	  }
	  else if ( b==INPUT_BUFFER && p[8] == REC_RECEIVER_TASK )  {
	    node_evt_recv += (*ic).events;
	  }
	  else if ( b==OUTPUT_BUFFER && p[8] == REC_SENDER_TASK )  {
	    node_evt_sent = std::min(node_evt_sent,(*ic).events);
	  }
	}
      }
    }
    evt_reco = std::min(evt_reco,node_evt_reco);
    evt_recv = std::min(evt_recv,node_evt_recv);
    evt_sent = std::min(evt_sent,node_evt_sent);
  }
  char b1[64];
  Nodeset::TimeStamp frst=ns.firstUpdate();
  time_t t1 = numNodes == 0 ? time(0) : frst.first, now = time(0);
  ::strftime(b1,sizeof(b1),"%H:%M:%S",::localtime(&t1));
  ::sprintf(text," %s %s [%d nodes %d buffers %d clients] ",
	    m_name.c_str(),b1,numNodes,numBuffs,numClients);
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
  else if ( !inuse ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," Subfarm not in used by any partition....",NORMAL|INVERSE|GREEN,4,1,1);
  }
  else if ( fslots[0] < SLOTS_MIN || fslots[1] < SLOTS_MIN ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::sprintf(txt," SLOTS at limit:");
    if ( fslots[0] < SLOTS_MIN ) ::strcat(txt,"INPUT ");
    if ( fslots[1] < SLOTS_MIN ) ::strcat(txt,"OUTPUT ");
    ::sprintf(text,"[%d nodes]",int(bad_nodes.size()));
    ::strcat(txt,text);
    ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,4,1,1);
  }
  else if ( fspace[0] < SPACE_MIN || fspace[1] < SPACE_MIN ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::sprintf(txt," SPACE at limit:");
    if ( fspace[0] < SPACE_MIN ) ::strcat(txt,"INPUT ");
    if ( fspace[1] < SPACE_MIN ) ::strcat(txt,"OUTPUT ");
    ::sprintf(text,"[%d nodes]",int(bad_nodes.size()));
    ::strcat(txt,text);
    ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,4,1,1);
  }
  else if ( inuse && evt_recv <= m_evtRecv && evt_prod[0] == m_totRecv ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No reconstruction activity visible.",BOLD|RED,4,1,1);
  }
  else if ( evt_reco <= m_evtReco && evt_prod[1] > m_totReco ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::scrc_put_chars(m_display," Some RECO(s) stuck.",BOLD|RED|INVERSE,4,1,1);
  }
  else if ( inuse && evt_reco <= m_evtReco && evt_prod[1] == m_totReco ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No activity visible.",BOLD|RED,4,1,1);
  }
  else if ( evt_sent <= m_evtSent && evt_prod[1] > m_totSent ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::scrc_put_chars(m_display," Some Sender(s) stuck.",BOLD|RED|INVERSE,4,1,1);
  }
  else if ( inuse && evt_sent <= m_evtSent && evt_prod[1] == m_totSent ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No STORAGE activity visible.",BOLD|RED,4,1,1);
  }
  else {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display,"",NORMAL,4,1,1);
    ::scrc_put_chars(m_display," No obvious Errors detected....",NORMAL|INVERSE|GREEN,4,1,1);
    m_hasProblems = false;
  }
  m_evtRecv = evt_recv;
  m_evtReco = evt_reco;
  m_evtSent = evt_sent;
  m_totRecv = evt_prod[0];
  m_totReco = evt_prod[1];
  m_totSent = evt_prod[1];
  if ( buf_clients[0] != 0 )
    ::sprintf(txt,"%9d%4d%6d   %9d%4d%6d",
	      evt_prod[0],buf_clients[0],used_slots[0],
	      evt_prod[1],buf_clients[1],used_slots[1]);
  else
    ::sprintf(txt,"%9s%4s%6s   %9s%4s%6s","--","","--","--","","--");
  ::scrc_put_chars(m_display,txt,NORMAL,2,5,1);
  if ( buf_clients[0] != 0 )
    ::sprintf(txt,"%9d%4s%6d   %9d%4s%6d",min_prod[0],"",min_slots[0],min_prod[1],"",min_slots[1]);
  else
    ::sprintf(txt,"%9s%4s%6s   %9s%4s%6s","--","","--","--","","--");
  ::scrc_put_chars(m_display,txt,NORMAL,3,5,1);
  IocSensor::instance().send(m_parent,CMD_CHECK,this);
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
  if ( h->m_name == "LHCb" ) f->push_back("CALD07");
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
  : InternalDisplay(0,""), m_subfarmDisplay(0), m_nodeSelector(0), 
    m_posCursor(0), m_subPosCursor(0), m_anchorX(10), m_anchorY(20), m_mode(HLT_MODE)
{
  bool all = false;
  char txt[128];
  std::string anchor;
  RTL::CLI cli(argc,argv,help);
  cli.getopt("partition",   2, m_name = "LHCb");
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
  ::lib_rtl_create_lock(0,&s_lock);
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
    m_svc = ::dic_info_service("DIS_DNS/SERVER_LIST",MONITORED,0,0,0,dataHandler,(long)this,0,0);
  }
  else {
    m_listener = std::auto_ptr<PartitionListener>(new PartitionListener(this,m_name));
  }
}

/// Standard destructor
FarmDisplay::~FarmDisplay()  {  
  MouseSensor::instance().stop();
  ::wtc_remove(WT_FACILITY_SCR);
  disconnect();
  m_listener = std::auto_ptr<PartitionListener>(0);
  ::scrc_begin_pasteboard_update(m_pasteboard);
  m_ctrlDisplay = std::auto_ptr<CtrlDisplay>(0);
  m_mbmDisplay = std::auto_ptr<BufferDisplay>(0);
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
    m_cpuDisplay = std::auto_ptr<CPUDisplay>(0);
    m_mbmDisplay = std::auto_ptr<BufferDisplay>(0);
    m_ctrlDisplay = std::auto_ptr<CtrlDisplay>(0);
    m_procDisplay = std::auto_ptr<ProcessDisplay>(0);
    m_subPosCursor = 8;
    m_nodeSelector = 0;
  }
  else if ( (d=currentDisplay()) != 0 ) {
    std::string title = "Sub farm info:" + d->name();
    if ( m_mode == CTRL_MODE ) {
      std::string svc = "-servicename=/"+strUpper(d->name())+"/TaskSupervisor/Status";
      const char* argv[] = {"",svc.c_str(), "-delay=300"};
      m_subfarmDisplay = new CtrlSubfarmDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX,m_anchorY,3,(char**)argv);
      m_nodeSelector = dynamic_cast<ScrDisplay*>(m_subfarmDisplay->nodeDisplay());
    }
    else if ( m_mode == RECO_MODE ) {
      std::string svc = "-servicename=/"+d->name()+"/ROpublish";
      const char* argv[] = {"",svc.c_str(), "-delay=300"};
      m_subfarmDisplay = new RecSubfarmDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX,m_anchorY,3,(char**)argv);
      m_nodeSelector = dynamic_cast<ScrDisplay*>(m_subfarmDisplay->nodeDisplay());
    }
    else if ( m_mode == HLT_MODE ) {
      std::string svc = "-servicename=/"+d->name()+"/ROpublish";
      const char* argv[] = {"",svc.c_str(), "-delay=300"};
      m_subfarmDisplay = new SubfarmDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX,m_anchorY,3,(char**)argv);
      m_nodeSelector = dynamic_cast<ScrDisplay*>(m_subfarmDisplay->nodeDisplay());
    }
    else {
      m_nodeSelector = 0;
      m_subfarmDisplay = 0;
      return WT_SUCCESS;
    }
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
    m_helpDisplay = std::auto_ptr<HelpDisplay>(0);
  }
  else if ( m_mbmDisplay.get() ) 
    m_helpDisplay = std::auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","mbm"));
  else if ( m_ctrlDisplay.get() ) 
    m_helpDisplay = std::auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","ctrl"));
  else if ( m_procDisplay.get() ) 
    m_helpDisplay = std::auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","procs"));
  else if ( m_cpuDisplay.get() ) 
    m_helpDisplay = std::auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","cpu"));
  else if ( m_subfarmDisplay )
    m_helpDisplay = std::auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","subfarm"));
  else
    m_helpDisplay = std::auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","farm"));
  if ( m_helpDisplay.get() ) {
    m_helpDisplay->show(m_anchorY,m_anchorX);
    MouseSensor::instance().add(this,m_helpDisplay->display());
  }
  return WT_SUCCESS;
}

/// Show window with processes on a given node
int FarmDisplay::showProcessWindow() {
  if ( m_procDisplay.get() ) {
    if ( m_helpDisplay.get() ) showHelpWindow();
    MouseSensor::instance().remove(this,m_procDisplay->display());
    m_procDisplay = std::auto_ptr<ProcessDisplay>(0);
  }
  else if ( m_subfarmDisplay ) {
    std::string node_name = m_subfarmDisplay->nodeName(m_subPosCursor-SUBFARM_NODE_OFFSET);
    if ( !node_name.empty() ) {
      m_procDisplay = std::auto_ptr<ProcessDisplay>(new ProcessDisplay(this,node_name));
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
    m_mbmDisplay = std::auto_ptr<BufferDisplay>(0);
  }
  else if ( m_subfarmDisplay ) {
    m_mbmDisplay = std::auto_ptr<BufferDisplay>(new BufferDisplay(this,"MBM Monitor display"));
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
    m_ctrlDisplay = std::auto_ptr<CtrlDisplay>(0);
  }
  else if ( m_subfarmDisplay ) {
    m_ctrlDisplay = std::auto_ptr<CtrlDisplay>(new CtrlDisplay(this,"CTRL Monitor display"));
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
    m_cpuDisplay = std::auto_ptr<CPUDisplay>(0);
  }
  else if ( m_subfarmDisplay ) {
    const Nodeset* ns = (const Nodeset*)m_subfarmDisplay->data().pointer;
    if ( ns ) {
      m_cpuDisplay = std::auto_ptr<CPUDisplay>(new CPUDisplay(this,ns->name));
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
  std::string svc, node;
  size_t idx, idq;
  switch(c=msg[0]) {
  case '+':
    getServiceNode(++msg,svc,node);
    idx = svc.find("/ROpublish");
    idq = svc.find("/hlt");
    if ( idx != std::string::npos && idq == 0 ) {
      std::string f = svc.substr(1,idx-1);
      if ( ::strcase_match_wild(f.c_str(),m_match.c_str()) ) {
	IocSensor::instance().send(this,CMD_ADD,new std::string(f));
      }
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
    case CTRL_P:
      return showProcessWindow();
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

/// Interactor overload: Display callback handler
void FarmDisplay::handle(const Event& ev) {
  int cnt = 0;
  time_t now = time(0);
  Farms::iterator i;
  SubDisplays::iterator k;
  const MouseEvent* m = 0;

  RTL::Lock lock(s_lock);
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
	  RTL::Lock unlock(s_lock,true);
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
      RTL::Lock lock(s_lock,true);
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
      showProcessWindow();
      return;
    case CMD_SHOWHELP:
      showHelpWindow();
      return;
    case CMD_SHOW:
      for(k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k, ++cnt) {
	InternalDisplay* d = (*k).second;
	if ( d == ev.data )  {
	  RTL::Lock unlock(s_lock,true);
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
	InternalDisplay* d = (*k).second;
	if ( d != ev.data )  {
	  d->check(now);
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

  ::sprintf(txt,"Total number of subfarms:%d %50s",int(farms.size()),"<CTRL-H for Help>, <CTRL-E to exit>");
  ::scrc_put_chars(m_display,txt,NORMAL,1,2,1);

  for( k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k)
    (*k).second->hide();
  for (Farms::const_iterator i=farms.begin(); i != farms.end(); ++i) {
    k = m_farmDisplays.find(*i);
    if ( k == m_farmDisplays.end() ) {
      if ( m_mode == RECO_MODE )
	copy.insert(std::make_pair(*i,new RecFarmSubDisplay(this,*i)));
      else if ( m_mode == CTRL_MODE )
	copy.insert(std::make_pair(*i,createCtrlFarmSubDisplay(this,*i)));
      else
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
