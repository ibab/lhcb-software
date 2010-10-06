// $Id: FarmStatDisplay.cpp,v 1.1 2010-10-06 21:55:24 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/FarmStatDisplay.cpp,v 1.1 2010-10-06 21:55:24 frankb Exp $

// Framework include files
#include "ROMon/HelpDisplay.h"
#include "ROMon/FarmStatClusterDisplay.h"
#include "SCR/ScrDisplay.h"

// C++ include files
#include <memory>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {


  /**@class FarmStatDisplay ROMon.h GaudiOnline/FarmStatDisplay.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class FarmStatDisplay : public InternalDisplay  {
  protected:
    typedef std::map<std::string, FarmStatClusterLine*> Clusters;
    Clusters                           m_clusters;

    std::auto_ptr<HelpDisplay>         m_helpDisplay;
    std::auto_ptr<FarmStatClusterDisplay>  m_clusterDisplay;

    /// vector with all farm displays
    int                                m_height;
    int                                m_width;
    /// Cursor position in sub display array
    size_t                             m_posCursor;
    int                                m_anchorX, m_anchorY;
    const FarmStatClusterLine*         m_currLine;
    SCR::Display*                      m_header;

    /// Keyboard rearm action
    static int key_rearm (unsigned int fac, void* param);
    /// Keyboard action
    static int key_action(unsigned int fac, void* param);

public:
    /// Standard constructor
    FarmStatDisplay(int argc, char** argv);
    /// Standard destructor
    virtual ~FarmStatDisplay();
    /// Handle keyboard interrupts
    int handleKeyboard(int key);

    const FarmStatClusterLine* currentLine() const       { return m_currLine; }
    void setCurrentLine(const FarmStatClusterLine* line) { m_currLine = line; }
    /// Set cursor to position
    virtual void set_cursor(InternalDisplay* disp);
    /// Set cursor to line position
    virtual void set_cursor(const FarmStatClusterLine* line);
    /// Interactor overload: Display callback handler
    virtual void handle(const Event& ev);
    /// DIM command service callback
    virtual void update(const void* data);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
    /// Show context dependent help window
    int showHelpWindow();
    /// Show window with the boot information of one cluster
    int showClusterWindow(const FarmStatClusterLine* line);
    /// Display cluster information in line form
    void display(const FarmStatClusterLine* line);
    /// DIM command service callback
    static void feedData(void* tag, void* address, int* size);
  };
}

#include "SCR/MouseSensor.h"
#include "CPP/TimeSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/Lock.h"
#include "RTL/strdef.h"
#include "SCR/scr.h"
#include "WT/wtdef.h"
#include "ROMon/Constants.h"
#include "ROMon/FarmStatClusterDisplay.h"

#include "ROMonDefs.h"

extern "C" {
#include "dic.h"
}

// C++ include files
#include <set>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <iostream>
#include <algorithm>

using namespace ROMon;
using namespace SCR;
using namespace std;

typedef vector<string>               StringV;

// Max. 15 seconds without update allowed
#define UPDATE_TIME_MAX    15
#define BOOTLINE_START      3
#define BOOTLINE_FIRSTPOS   9
#define MAX_BOOT_TIME     600

typedef set<string> _SETV;

static FarmStatDisplay* s_fd = 0;

namespace {

  float _cnvItem(const char* p, float*) {  return ::atof(p); }
  int   _cnvItem(const char* p, int*)   {  return ::atol(p); }

  template <class T> void _cnv(string buffer,std::map<string,map<string,vector<T> > >& cnt) {
    char* data = (char*)buffer.c_str();
    char *e, *c, *q, *p, *node, *node_end, *sub_item;
    if ( (q=::strchr(data,':')) && (p=::strchr(data,'(')) )  {
      *q = 0;
      if ( *(p+1) != ')' )  {
	for(node=++p,node_end=node+1; node && *node == '[' && (q=::strchr(node,':')) && (node_end=::strchr(++node,']')); node=::strchr(node_end+1,'[') ) {
	  *node_end = *q = 0;
	  map<string,vector<T> >& node_info = cnt[node];
	  for(; *++q == '{' && (e=::strchr(q,'}')) && (q=strchr(sub_item=++q,':')); q=e ) {
	    *e = *(c=q) = 0;
	    vector<T>& vals = node_info[sub_item];
	    while( (c=::strchr(p=++c,'#')) )  {
	      *c=0; vals.push_back(_cnvItem(p,(T*)0));
	    }
	    vals.push_back(_cnvItem(p,(T*)0));
	    q = e;
	  }
	  cout << endl;
	}
      }
    }
  }
    
  void help() {
    printf("  romon_boot_display -option [-option]\n"
	   "       -an[chor]=+<x-pos>+<ypos>    Set anchor for sub displays\n");
  }

  struct FarmStatLineByPosition {
    int pos;
    FarmStatLineByPosition(int p) : pos(p) {}
    bool operator()(const pair<string,FarmStatClusterLine*>& o) 
    {    return o.second->position() == pos;  }
  };

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
}


// Standard constructor
FarmStatClusterLine::FarmStatClusterLine(FarmStatDisplay* p, int pos, const std::string& n)
  : m_mbm(0), m_cpu(0), m_position(pos), m_parent(p), m_name(n)
{
  string svc = "/FarmStats/"+strlower(n)+"/MBM";
  m_mbm = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,mbmHandler,(long)this,0,0);
  svc = "/FarmStats/"+strlower(n)+"/CPU";
  m_cpu = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,cpuHandler,(long)this,0,0);
}

FarmStatClusterLine::~FarmStatClusterLine() {
  if ( m_mbm ) {
    ::dic_release_service(m_mbm);
    m_mbm = 0;
  }
  if ( m_cpu ) {
    ::dic_release_service(m_cpu);
    m_cpu = 0;
  }
}

/// DIM command service callback
void FarmStatClusterLine::cpuHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    FarmStatClusterLine* l = *(FarmStatClusterLine**)tag;
    l->m_cpuData.clear();
    _cnv((const char*)address, l->m_cpuData);
    l->m_parent->display(l);
  }
}

/// DIM command service callback
void FarmStatClusterLine::mbmHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    FarmStatClusterLine* l = *(FarmStatClusterLine**)tag;
    l->m_mbmData.clear();
    _cnv((const char*)address, l->m_mbmData);
    l->m_parent->display(l);
  }
}

/// Standard constructor
FarmStatDisplay::FarmStatDisplay(int argc, char** argv)
  : InternalDisplay(0,""), m_posCursor(BOOTLINE_FIRSTPOS), m_anchorX(10), m_anchorY(10), m_currLine(0)
{
  char txt[128];
  string anchor;
  RTL::CLI cli(argc,argv,help);
  m_name = "ALL";
  int level = LIB_RTL_INFO;
  cli.getopt("printlevel", 2, level);
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
  //::lib_rtl_install_printer(ro_rtl_print,(void*)level);
  if ( cli.getopt("debug",2) != 0 ) {
    ::lib_rtl_output(LIB_RTL_ALWAYS,"Attach debugger: gdb --pid %d",::lib_rtl_pid());
    ::lib_rtl_sleep(10000);
  }
  s_fd = this;
  m_title = " MBM/CPU statistics display";
  ::scrc_create_pasteboard (&m_pasteboard, 0, &m_height, &m_width);
  ScrDisplay::setPasteboard(m_pasteboard);
  ScrDisplay::setBorder(BLUE|INVERSE);
  m_width -= 2;
  m_height -= 2;
  ::scrc_create_display (&m_display, m_height, m_width, NORMAL, ON, m_title.c_str());
  show(2,2);
  ::scrc_put_chars(m_display,txt,NORMAL,1,BOOTLINE_START,1);
  ::scrc_put_chars(m_display,"",NORMAL,1,BOOTLINE_START+1,1);

  ::scrc_put_chars(m_display,"Color Legend:",NORMAL,2,3,1);
  ::scrc_put_chars(m_display,"X",GREEN|INVERSE,3,3,0);
  ::scrc_put_chars(m_display,"Finished, FMC started",NORMAL,3,7,0);
  ::scrc_put_chars(m_display,"X",GREEN|BOLD,  3,30,0);
  ::scrc_put_chars(m_display,"...TCP started",NORMAL|BOLD,3,33,0);
  ::scrc_put_chars(m_display,"X",BLUE|INVERSE,3,60,0);
  ::scrc_put_chars(m_display,"...ETH1 started",NORMAL,3,63,0);
  ::scrc_put_chars(m_display,"X",BLUE|BOLD,3,90,0);
  ::scrc_put_chars(m_display,"...ETH0 started",NORMAL|BOLD,3,93,0);

  ::scrc_put_chars(m_display,"X",CYAN|INVERSE,4,3,0);
  ::scrc_put_chars(m_display,"...PCI started",NORMAL,4,7,0);
  ::scrc_put_chars(m_display,"X",CYAN|BOLD,4,30,0);
  ::scrc_put_chars(m_display,"...CPU(s) started",NORMAL|BOLD,4,33,0);
  ::scrc_put_chars(m_display,"X",MAGENTA|BOLD,4,60,0);
  ::scrc_put_chars(m_display,"...Mounting disks",NORMAL|BOLD,4,63,0);

  ::scrc_put_chars(m_display,"X",MAGENTA|INVERSE,4,90,0);
  ::scrc_put_chars(m_display,"DHCP request seen",NORMAL,4,93,0);
  ::scrc_put_chars(m_display,"X",RED|INVERSE|BOLD,5,3,0);
  ::scrc_put_chars(m_display,"...NOT BOOTED",BOLD,5,7,0);

  ::scrc_create_display(&m_header,1, m_width-3, NORMAL, ON, m_title.c_str());
  ::scrc_put_chars(m_header,"Cluster",BOLD|INVERSE,1,BOOTLINE_START-1,1);
  ::scrc_put_chars(m_header,"Ctrl-PC",BOLD|INVERSE,1,BOOTLINE_START+10,0);
  ::scrc_put_chars(m_header,"Cluster members",BOLD|INVERSE,1,BOOTLINE_START+19,1);
  ::scrc_set_border(m_header,"",NORMAL);
  ::scrc_paste_display(m_header, m_pasteboard,8,BOOTLINE_START);

  ::scrc_end_pasteboard_update (m_pasteboard);
  ::scrc_fflush(m_pasteboard);
  ::scrc_set_cursor(m_display, 2, BOOTLINE_START);
  ::scrc_cursor_off(m_pasteboard);
  ::wtc_remove(WT_FACILITY_SCR);
  ::wtc_subscribe(WT_FACILITY_SCR, key_rearm, key_action, m_pasteboard);
  MouseSensor::instance().start(pasteboard());
  MouseSensor::instance().add(this,this->InternalDisplay::display());
  m_svc = ::dic_info_service((char*)"/FarmStats/SERVICE_LIST",MONITORED,0,0,0,dataHandler,(long)this,0,0);
}

/// Standard destructor
FarmStatDisplay::~FarmStatDisplay()  {  
  ::scrc_unpaste_display(m_header,m_pasteboard);
  ::scrc_delete_display(m_header);
  MouseSensor::instance().stop();
  ::wtc_remove(WT_FACILITY_SCR);
  disconnect();
  ::scrc_begin_pasteboard_update(m_pasteboard);
  close();
  ::scrc_end_pasteboard_update (m_pasteboard);
  ::scrc_delete_pasteboard(m_pasteboard);
  m_pasteboard = 0;
  ::scrc_resetANSI();
  ::lib_rtl_output(LIB_RTL_ALWAYS,"Farm display deleted and resources freed......\n");
}

// Display cluster information in line form
void FarmStatDisplay::display(const FarmStatClusterLine* line) {
  typedef FarmStatClusterLine::_CI _CI;
  size_t pos = line->position();
  int col = NORMAL;
  Pasteboard* pb = pasteboard();
  const char *nam = line->name().c_str();

  RTL::Lock lock(screenLock());
  ::scrc_begin_pasteboard_update (pb);
  ::scrc_cursor_off(pb);
  ::scrc_put_chars(m_display,nam,(pos==m_posCursor?BLUE|INVERSE:NORMAL)|BOLD,pos,BOOTLINE_START,0);
  int xp = 11+BOOTLINE_START;
  for(_CI::const_iterator ci=line->cpuData().begin(); ci != line->cpuData().end(); ++ci)  {
    char txt[64];
    const char* n = (*ci).first.c_str();
    ::sprintf(txt," %s ",n+(::strncmp(n,nam,::strlen(nam)+2) ? ::strlen(n)-2 : 0));
    ::scrc_put_chars(m_display,txt,col,pos,xp,0);
    xp += ::strlen(txt)+1;
  }
  ::scrc_put_chars(m_display," ",NORMAL,pos,xp,1);
  if ( pos == m_posCursor ) {
    ::scrc_set_cursor(m_display,pos,2);
  }
  if ( m_clusterDisplay.get() && m_clusterDisplay->name() == nam )  {
    m_clusterDisplay->update(line);
  }
  ::scrc_cursor_on(pb);
  ::scrc_end_pasteboard_update(pb);
}

/// Keyboard rearm action
int FarmStatDisplay::key_rearm (unsigned int /* fac */, void* param)  {
  Pasteboard* pb = (Pasteboard*)param;
  return ::scrc_fflush(pb);
}

/// Keyboard action
int FarmStatDisplay::key_action(unsigned int /* fac */, void* /* param */)  {
  int key = ::scrc_read_keyboard(0,0);
  if (!key) return WT_SUCCESS;
  return s_fd->handleKeyboard(key);
}

/// Set cursor to position
void FarmStatDisplay::set_cursor(InternalDisplay* d) {
  ::scrc_set_cursor(d->display(),2,2);
}

/// Set cursor to position
void FarmStatDisplay::set_cursor(const FarmStatClusterLine* line) {
  const FarmStatClusterLine* curr = currentLine();
  if ( curr )   {
    ::scrc_put_chars(m_display,curr->name().c_str(),NORMAL|BOLD,curr->position(),BOOTLINE_START,0);
  }
  setCurrentLine(line);
  if ( line ) {
    ::scrc_put_chars(m_display,line->name().c_str(),BLUE|BOLD|INVERSE,line->position(),BOOTLINE_START,0);
    ::scrc_set_cursor(m_display,line->position(),2);
  }
}

/// Show context dependent help window
int FarmStatDisplay::showHelpWindow() {
  DisplayUpdate update(this,true);
  if ( m_helpDisplay.get() ) {
    MouseSensor::instance().remove(this,m_helpDisplay->display());
    m_helpDisplay = auto_ptr<HelpDisplay>(0);
  }
  else {
    string input = ::getenv("ROMONROOT") != 0 ? ::getenv("ROMONROOT") : "..";
    string fin = input+"/doc/farmStats.hlp";
    string tag = m_clusterDisplay.get() ? "stats-subfarm" : "stats-display";
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window",tag,fin));
    if ( m_helpDisplay.get() ) {
      m_helpDisplay->show(m_anchorY,m_anchorX);
      MouseSensor::instance().add(this,m_helpDisplay->display());
    }
  }
  return WT_SUCCESS;
}

/// Show window with the boot information of one cluster
int FarmStatDisplay::showClusterWindow(const FarmStatClusterLine* line) {
  DisplayUpdate update(this,true);
  if ( m_clusterDisplay.get() ) {
    MouseSensor::instance().remove(this,m_clusterDisplay->display());
    m_clusterDisplay = auto_ptr<FarmStatClusterDisplay>(0);
  }
  else {
    m_clusterDisplay = auto_ptr<FarmStatClusterDisplay>(new FarmStatClusterDisplay(this,line->name()));
    if ( m_clusterDisplay.get() ) {
      m_clusterDisplay->show(m_anchorY,m_anchorX);
      m_clusterDisplay->update(line);
      MouseSensor::instance().add(this,m_clusterDisplay->display());
    }
  }
  return WT_SUCCESS;
}

/// Handle keyboard interrupts
int FarmStatDisplay::handleKeyboard(int key)    {
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
      IocSensor::instance().send(this,CMD_SHOWHELP,m_posCursor);
      return WT_SUCCESS;
    case RETURN_KEY:
    case ENTER:
      IocSensor::instance().send(this,CMD_SHOWSUBFARM,m_posCursor);
      return WT_SUCCESS;
    case MOVE_UP:
      if( m_posCursor>BOOTLINE_FIRSTPOS ) --m_posCursor;
      IocSensor::instance().send(this,CMD_POSCURSOR,m_posCursor);
      break;
    case MOVE_DOWN:
      if( m_posCursor < m_clusters.size()+BOOTLINE_FIRSTPOS ) ++m_posCursor;
      IocSensor::instance().send(this,CMD_POSCURSOR,m_posCursor);
      break;
    default:
      return WT_SUCCESS;
    }
  }
  catch(...) {
  }
  return WT_SUCCESS;
}

/// Interactor overload: Display callback handler
void FarmStatDisplay::handle(const Event& ev) {
  const MouseEvent* m = 0;

  RTL::Lock lock(screenLock());
  switch(ev.eventtype) {
  case ScrMouseEvent:
    m = ev.get<MouseEvent>();
    if ( m->button == 2 ) {
      IocSensor::instance().send(this,CMD_SHOWHELP,this);
    }
    else if ( m->msec != (unsigned int)-1 ) {
      if ( m_helpDisplay.get() ) {
        IocSensor::instance().send(this,CMD_SHOWHELP,this);
      }
      else {
        IocSensor::instance().send(this,CMD_SHOWSUBFARM,m->y-1);
      }
    }
    return;
  case TimeEvent:
    return;
  case IocEvent:
    switch(ev.type) {
    case CMD_CONNECT: {
      char txt[256];
      Clusters copy = m_clusters;
      Clusters::iterator j;
      auto_ptr<_SETV> nodes((_SETV*)ev.data);
      dim_lock();
      m_clusters.clear();
      for(_SETV::const_iterator i=nodes->begin(); i!=nodes->end(); ++i) {
	const string& n = *i;
	if ( (j=copy.find(n)) == copy.end() ) {
	  FarmStatClusterLine* l = new FarmStatClusterLine(this,m_posCursor=m_clusters.size()+BOOTLINE_FIRSTPOS,n);
	  if ( !m_currLine ) m_currLine = l;
	  m_clusters.insert(make_pair(n,l));
	}
	else {
	  m_clusters.insert(make_pair(n,(*j).second));
	  copy.erase(j);
	}
      }
      for(j=copy.begin(); j != copy.end(); ++j)
	delete (*j).second;
      ::sprintf(txt,"Total number of boot clusters:%d %50s",
		int(m_clusters.size()),"<CTRL-H for Help>, <CTRL-E to exit>");
      ::scrc_begin_pasteboard_update(m_pasteboard);
      ::scrc_put_chars(m_display,txt,NORMAL,1,BOOTLINE_START,1);
      ::scrc_end_pasteboard_update(m_pasteboard);
      dim_unlock();
      break;
    }

    case CMD_UPDATE: {
      unsigned char* ptr = (unsigned char*)ev.data;
      this->update(ptr + sizeof(int));
      break;
    }
    case CMD_HANDLE_KEY:
      handleKeyboard(int((long)ev.data));
      break;
    case CMD_SHOWHELP:
      showHelpWindow();
      break;
    case CMD_SHOWSUBFARM:
      if ( !m_helpDisplay.get() ) {
	m_posCursor = (long)ev.data;
	Clusters::iterator i=find_if(m_clusters.begin(),m_clusters.end(),FarmStatLineByPosition(m_posCursor));
	if ( i != m_clusters.end() ) {
	  showClusterWindow((*i).second);
	  IocSensor::instance().send(this,CMD_POSCURSOR,m_posCursor);
	}
      }
      break;

    case CMD_POSCURSOR:
      if ( !m_helpDisplay.get() ) {
	m_posCursor = (long)ev.data;
	Clusters::iterator i=find_if(m_clusters.begin(),m_clusters.end(),FarmStatLineByPosition(m_posCursor));
	if ( i != m_clusters.end() ) {
	  DisplayUpdate update(this);
	  if ( m_clusterDisplay.get() )  {
	    m_clusterDisplay->update((*i).second);
	    ::scrc_set_cursor(m_clusterDisplay->display(),2,2);
	  }
	  else {
	    set_cursor((*i).second);
	  }
	}
      }
      break;
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


/// DIM command service callback
void FarmStatDisplay::update(const void* address) {
  if ( address ) {
    char *msg = (char*)address;
    char *p=msg, *at=msg, *last=msg, *typ, *node;
    if ( *(int*)msg != *(int*)"DEAD" )  {
      auto_ptr<_SETV> nodes(new _SETV);
      while ( last != 0 && at != 0 )  {
	last = ::strchr(at,'|');
	if ( last ) *last = 0;
	if ( (typ=::strstr(at,"/MBM")) || (typ=::strstr(at,"/CPU")) ) {
	  node = strchr(at+1,'/');
	  *(p=strchr(node+1,'/')) = 0;
	  nodes->insert(node+1);
	  //break; // TO REMOVE!!
	}
	at = strchr(last+1,'/');
      }
      if ( !nodes->empty() ) {
	static bool ignore = false;
	if ( ignore ) return;
	ignore = true;
	IocSensor::instance().send(this,CMD_CONNECT,nodes.release());
      }
    }
  }
}

// Main entry routine to start the display application
extern "C" int romon_farmstat_display(int argc,char** argv) {
  FarmStatDisplay* disp = new FarmStatDisplay(argc,argv);
  IocSensor::instance().run();
  delete disp;
  return 1;
}

