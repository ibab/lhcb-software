// $Id: BootDisplay.cpp,v 1.7 2010-10-14 06:44:04 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/BootDisplay.cpp,v 1.7 2010-10-14 06:44:04 frankb Exp $

// Framework include files
#include "ROMon/BootMon.h"
#include "ROMon/HelpDisplay.h"
#include "ROMon/BootClusterDisplay.h"
#include "SCR/ScrDisplay.h"

#include <memory>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  // Forward declarations
  class BootDisplay;
  class BootNodeStatusset;

  /**@class BootClusterLine ROMon.h GaudiOnline/BootDisplay.h
   *
   *   Display entry for the boot status of one subfarm
   *
   *   @author M.Frank
   */
  class BootClusterLine  {
  protected:
    std::string        m_name;
    int                m_svc;
    size_t             m_position;
    BootDisplay*       m_parent;
    BootNodeStatusset* m_cluster;
    char*              m_ptr;

  public:
    BootClusterLine(BootDisplay* p, int pos, const std::string& n);
    virtual ~BootClusterLine();
    const std::string& name() const           { return m_name;      }
    const BootNodeStatusset* cluster() const  { return m_cluster;   }
    int position() const                      { return m_position;  }
    /// DIM command service callback
    static void dataHandler(void* tag, void* address, int* size);
  };

  /**@class BootDisplay ROMon.h GaudiOnline/BootDisplay.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class BootDisplay : public InternalDisplay  {
  protected:
    typedef std::map<std::string, BootClusterLine*> Clusters;
    Clusters                           m_clusters;

    std::auto_ptr<HelpDisplay>         m_helpDisplay;
    std::auto_ptr<BootClusterDisplay>  m_clusterDisplay;

    /// vector with all farm displays
    int                                m_height;
    int                                m_width;
    /// Cursor position in sub display array
    size_t                             m_posCursor;
    int                                m_anchorX, m_anchorY;
    const BootClusterLine*             m_currLine;
    SCR::Display*                      m_header;

    /// Keyboard rearm action
    static int key_rearm (unsigned int fac, void* param);
    /// Keyboard action
    static int key_action(unsigned int fac, void* param);

  public:
    /// Standard constructor
    BootDisplay(int argc, char** argv);
    /// Standard destructor
    virtual ~BootDisplay();
    /// Handle keyboard interrupts
    int handleKeyboard(int key);

    const BootClusterLine* currentLine() const       { return m_currLine; }
    void setCurrentLine(const BootClusterLine* line) { m_currLine = line; }
    /// Set cursor to line position
    virtual void pos_cursor(const BootClusterLine* line);
    /// Interactor overload: Display callback handler
    virtual void handle(const CPP::Event& ev);
    /// DIM command service callback
    virtual void update(const void* data);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
    /// Show context dependent help window
    int showHelpWindow();
    /// Show window with the boot information of one cluster
    int showClusterWindow(const BootClusterLine* line);
    /// Display cluster information in line form
    void display(const BootClusterLine* line);
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
#include "ROMon/BootClusterDisplay.h"

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
#include <fstream>
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

static BootDisplay* s_fd = 0;

namespace {

  void help() {
    printf("  romon_boot_display -option [-option]\n"
           "       -an[chor]=+<x-pos>+<ypos>    Set anchor for sub displays\n");
  }

  struct BootLineByPosition {
    int pos;
    BootLineByPosition(int p) : pos(p) {}
    bool operator()(const pair<string,BootClusterLine*>& o) 
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
BootClusterLine::BootClusterLine(BootDisplay* p, int pos, const std::string& n)
  : m_name(n), m_svc(0), m_position(pos), m_parent(p), m_ptr(0)
{
  string svc = InternalDisplay::svcPrefix()+strlower(n)+"/BootMonitor";
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
}

BootClusterLine::~BootClusterLine() {
  if ( m_svc ) {
    ::dic_release_service(m_svc);
    m_svc = 0;
  }
}

/// DIM command service callback
void BootClusterLine::dataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    BootClusterLine* l = *(BootClusterLine**)tag;
    char* ptr = new char[*size+sizeof(int)];
    *(int*)ptr = *size;
    ::memcpy(ptr+sizeof(int),address,*size);
    if ( l->m_ptr ) delete [] l->m_ptr;
    l->m_ptr = ptr;
    l->m_cluster = (BootNodeStatusset*)((char*)l->m_ptr + sizeof(int));
    l->m_parent->display(l);
  }
}

/// Standard constructor
BootDisplay::BootDisplay(int argc, char** argv)
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
  if ( cli.getopt("reverse",3) ) setCreateFlags(INVERSE); 
  ::lib_rtl_install_printer(ro_rtl_print,(void*)long(level));
  s_fd = this;
  m_title = " BOOT monitor display";
  ::scrc_create_pasteboard (&m_pasteboard, 0, &m_height, &m_width);
  ScrDisplay::setPasteboard(m_pasteboard);
  ScrDisplay::setBorder(BLUE|INVERSE);
  m_width -= 2;
  m_height -= 2;
  ::scrc_create_display (&m_display, m_height, m_width, createFlags(), ON, m_title.c_str());
  show(2,2);
  ::scrc_begin_pasteboard_update (m_pasteboard);
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

  ::scrc_create_display (&m_header,1, m_width-3, NORMAL, ON, m_title.c_str());
  ::scrc_put_chars(m_header,"Cluster",BOLD|INVERSE,1,BOOTLINE_START-1,1);
  ::scrc_put_chars(m_header,"Ctrl-PC",BOLD|INVERSE,1,BOOTLINE_START+10,0);
  ::scrc_put_chars(m_header,"Cluster members",BOLD|INVERSE,1,BOOTLINE_START+19,1);
  ::scrc_set_border(m_header,"",NORMAL);
  ::scrc_paste_display(m_header, m_pasteboard,8,BOOTLINE_START);

  ::scrc_end_pasteboard_update (m_pasteboard);
  ::scrc_fflush(m_pasteboard);
  ::scrc_cursor_off(m_pasteboard);
  ::wtc_remove(WT_FACILITY_SCR);
  ::wtc_subscribe(WT_FACILITY_SCR, key_rearm, key_action, m_pasteboard);
  m_svc = ::dic_info_service((char*)"/BootMonitor/Clusters",MONITORED,0,0,0,dataHandler,(long)this,0,0);
  MouseSensor::instance().start(pasteboard());
  MouseSensor::instance().add(this,this->InternalDisplay::display());
}

/// Standard destructor
BootDisplay::~BootDisplay()  {  
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
void BootDisplay::display(const BootClusterLine* line) {
  time_t now = ::time(0);
  size_t pos = line->position();
  const BootNodeStatusset* c = line->cluster();
  const BootNodeStatusset::Nodes& nodes = c->nodes;
  BootNodeStatusset::Nodes::const_iterator ci=nodes.begin();

  RTL::Lock lock(screenLock());
  Pasteboard* pb = pasteboard();
  ::scrc_cursor_off(pb);
  ::scrc_begin_pasteboard_update (pb);
  ::scrc_put_chars(m_display,c->name,(pos==m_posCursor?BLUE|INVERSE:NORMAL)|BOLD,pos,BOOTLINE_START,0);
  int xp = 11+BOOTLINE_START;
  for(; ci != nodes.end(); ci = nodes.next(ci) ) {
    char txt[64];
    int col = RED|INVERSE;
    int st  = (*ci).status;
    const char* n = (*ci).name;
    if (      abs(long(now-(*ci).dhcpReq))>MAX_BOOT_TIME && 
              0 != (st&BootNodeStatus::DHCP_REQUESTED) && 
              0 == (st&BootNodeStatus::FMC_STARTED)     ) col = RED|INVERSE|BOLD|FLASH;
    else if ( 0 != (st&BootNodeStatus::FMC_STARTED)     ) col = GREEN|INVERSE;
    else if ( 0 != (st&BootNodeStatus::TCP_STARTED)     ) col = GREEN|BOLD;
    else if ( 0 != (st&BootNodeStatus::ETH1_STARTED)    ) col = BLUE|INVERSE|BOLD;
    else if ( 0 != (st&BootNodeStatus::ETH0_STARTED)    ) col = BLUE|BOLD;
    else if ( 0 != (st&BootNodeStatus::PCI_STARTED)     ) col = CYAN|INVERSE|BOLD;
    else if ( 0 != (st&BootNodeStatus::CPU_STARTED)     ) col = CYAN|BOLD;
    else if ( 0 != (st&BootNodeStatus::MOUNT_REQUESTED) ) col = MAGENTA|BOLD|FLASH;
    else if ( 0 != (st&BootNodeStatus::DHCP_REQUESTED)  ) col = MAGENTA|INVERSE|BOLD|FLASH;
    else                                                  col = RED|INVERSE|BOLD;
    txt[1] = n[0];
    txt[2] = n[1];
    if ( ::strncmp(n,c->name,::strlen(c->name)+2) == 0 )
      ::snprintf(txt,sizeof(txt)," %s ",n);
    else
      ::snprintf(txt,sizeof(txt)," %s ",n+::strlen(n)-2);
    ::scrc_put_chars(m_display,txt,col,pos,xp,0);
    xp += ::strlen(txt)+1;
  }
  ::scrc_put_chars(m_display," ",NORMAL,pos,xp,1);
  if ( m_clusterDisplay.get() && m_clusterDisplay->name() == c->name )  {
    m_clusterDisplay->update(c);
  }
  ::scrc_end_pasteboard_update(pb);
}

/// Keyboard rearm action
int BootDisplay::key_rearm (unsigned int /* fac */, void* param)  {
  Pasteboard* pb = (Pasteboard*)param;
  return ::scrc_fflush(pb);
}

/// Keyboard action
int BootDisplay::key_action(unsigned int /* fac */, void* /* param */)  {
  int key = ::scrc_read_keyboard(0,0);
  if (!key) return WT_SUCCESS;
  RTL::Lock lock(screenLock());
  return s_fd->handleKeyboard(key);
}

/// Set cursor to position
void BootDisplay::pos_cursor(const BootClusterLine* line) {
  const BootClusterLine* curr = currentLine();
  if ( curr )   {
    ::scrc_put_chars(m_display,curr->name().c_str(),NORMAL|BOLD,curr->position(),BOOTLINE_START,0);
  }
  setCurrentLine(line);
  if ( line ) {
    ::scrc_put_chars(m_display,line->name().c_str(),BLUE|BOLD|INVERSE,line->position(),BOOTLINE_START,0);
    if ( m_clusterDisplay.get() ) m_clusterDisplay->update(line->cluster());
  }
}

/// Show context dependent help window
int BootDisplay::showHelpWindow() {
  DisplayUpdate update(this,true);
  if ( m_helpDisplay.get() ) {
    MouseSensor::instance().remove(this,m_helpDisplay->display());
    m_helpDisplay = auto_ptr<HelpDisplay>(0);
  }
  else {
    string input = ::getenv("ROMONROOT") != 0 ? ::getenv("ROMONROOT") : "..";
    string fin = input+"/doc/bootMon.hlp";
    string tag = m_clusterDisplay.get() ? "boot-subfarm" : "boot-display";
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window",tag,fin));
    if ( m_helpDisplay.get() ) {
      m_helpDisplay->show(m_anchorY,m_anchorX);
      MouseSensor::instance().add(this,m_helpDisplay->display());
    }
  }
  return WT_SUCCESS;
}

/// Show window with the boot information of one cluster
int BootDisplay::showClusterWindow(const BootClusterLine* line) {
  DisplayUpdate update(this,true);
  if ( m_clusterDisplay.get() ) {
    MouseSensor::instance().remove(this,m_clusterDisplay->display());
    m_clusterDisplay = auto_ptr<BootClusterDisplay>(0);
  }
  else {
    m_clusterDisplay = auto_ptr<BootClusterDisplay>(new BootClusterDisplay(this,line->name()));
    if ( m_clusterDisplay.get() ) {
      m_clusterDisplay->show(m_anchorY,m_anchorX);
      m_clusterDisplay->update(line->cluster());
      MouseSensor::instance().add(this,m_clusterDisplay->display());
    }
  }
  return WT_SUCCESS;
}

/// Handle keyboard interrupts
int BootDisplay::handleKeyboard(int key)    {
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
void BootDisplay::handle(const CPP::Event& ev) {
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
    case CMD_UPDATE: {
      unsigned char* ptr = (unsigned char*)ev.data;
      update(ptr + sizeof(int), *(int*)ptr);
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
        Clusters::iterator i=find_if(m_clusters.begin(),m_clusters.end(),BootLineByPosition(m_posCursor));
        if ( i != m_clusters.end() ) {
          showClusterWindow((*i).second);
          IocSensor::instance().send(this,CMD_POSCURSOR,m_posCursor);
        }
      }
      break;
    case CMD_POSCURSOR:
      if ( !m_helpDisplay.get() ) {
        Clusters::iterator i=find_if(m_clusters.begin(),m_clusters.end(),BootLineByPosition(m_posCursor));
        if ( i != m_clusters.end() ) {
          DisplayUpdate update(this);
          pos_cursor((*i).second);
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
void BootDisplay::update(const void* address) {
  if ( address ) {
    char txt[256];
    const BootClusterCollection* c = (BootClusterCollection*)address;
    for(BootClusterCollection::const_iterator i=c->begin(); i!=c->end(); i=c->next(i)) {
      const char* n = (*i).name;
      if ( m_clusters.find(n) == m_clusters.end() ) {
        BootClusterLine* l = new BootClusterLine(this,m_posCursor=m_clusters.size()+BOOTLINE_FIRSTPOS,n);
        m_clusters.insert(make_pair(n,l));
      }
    }
    ::snprintf(txt,sizeof(txt),"Total number of boot clusters:%d %50s",
               int(m_clusters.size()),"<CTRL-H for Help>, <CTRL-E to exit>");
    ::scrc_put_chars(m_display,txt,NORMAL,1,BOOTLINE_START,1);
  }
}

// Main entry routine to start the display application
extern "C" int romon_boot_display(int argc,char** argv) {
  auto_ptr<BootDisplay> disp(new BootDisplay(argc,argv));
  IocSensor::instance().run();
  return 1;
}
