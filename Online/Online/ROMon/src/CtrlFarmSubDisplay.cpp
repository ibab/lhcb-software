#include "ROMon/FarmDisplay.h"
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

#include "ROMon/TaskSupervisor.h"
#include "TaskSupervisorParser.h"

namespace ROMon {

  /**@class CtrlFarmSubDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display summarizing one single subfarm. Showed as an array on the
   *   main display.
   *
   *   @author M.Frank
   */
  class CtrlFarmSubDisplay : public InternalDisplay, public Interactor  {
    bool                m_hasProblems;
    /// Extracted cluster information for all contained nodes
    Cluster             m_cluster;
  public:
    /// Access to problem flag
    bool hasProblems() const { return m_hasProblems; }
    /// Set timeout error
    void setTimeoutError();
    /// Initializing constructor
    CtrlFarmSubDisplay(FarmDisplay* parent, const std::string& title, bool bad=false);
    /// Standard destructor
    virtual ~CtrlFarmSubDisplay();
    /// Initialize default display text
    void init(bool bad);
    /// Update display content
    virtual void update(const void* data);
    /// Check display for errors
    virtual void check(time_t stamp);
    /// Set the focus to this display
    virtual void setFocus();
    /// Release the focus of this display
    virtual void releaseFocus();
    /// Interactor overload: Display callback handler
    virtual void handle(const Event& ev);
  };
  InternalDisplay* createCtrlFarmSubDisplay(FarmDisplay* parent, const std::string& title) {
    return new CtrlFarmSubDisplay(parent,title);
  }
}


#define UPDATE_TIME_MAX 30
#define DISP_WIDTH      48

#define COL_WARNING         (RED|BOLD)
#define COL_ALARM           (RED|BOLD|INVERSE)

/// Initializing constructor
CtrlFarmSubDisplay::CtrlFarmSubDisplay(FarmDisplay* parent, const std::string& title, bool bad) 
: InternalDisplay(parent, title)
{
  m_lastUpdate = time(0);
  ::scrc_create_display(&m_display,4,DISP_WIDTH,NORMAL,ON,m_title.c_str());
  init(bad);
  std::string svc = "/";
  for(size_t i=0; i<title.length();++i) svc += ::toupper(title[i]);
  svc += "/TaskSupervisor/Status";
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
  m_hasProblems = false;
  MouseSensor::instance().add(this,m_display);
}

/// Standard destructor
CtrlFarmSubDisplay::~CtrlFarmSubDisplay() {
  MouseSensor::instance().remove(m_display);
}

/// Initialize default display text
void CtrlFarmSubDisplay::init(bool) {
  ::scrc_put_chars(m_display,"                     UNKNOWN ",COL_WARNING,1,1,1);
  ::scrc_put_chars(m_display,"     No information availible",COL_ALARM,2,1,1);
  ::scrc_put_chars(m_display,"                             ",COL_ALARM,3,1,1);
  ::scrc_put_chars(m_display,"                             ",COL_ALARM,4,1,1);
  ::scrc_set_border(m_display,m_title.c_str(),COL_WARNING);
}

/// DIM command service callback
void CtrlFarmSubDisplay::update(const void* address) {
  const char* data = (const char*)address;
  m_lastUpdate = time(0);
  XML::TaskSupervisorParser ts;
  if ( ts.parseBuffer(m_name, data,::strlen(data)+1) ) {
    char txt[128];
    std::string val;
    Cluster& c = m_cluster;
    Cluster::Nodes::const_iterator i;
    int col = NORMAL, pos = 0, line=3;
    size_t taskCount=0, missCount=0;
    c.nodes.clear();
    ts.getClusterNodes(c);
    ::scrc_put_chars(m_display,"Nodes  ", NORMAL,3,1,1);
    ::scrc_put_chars(m_display,"       ", NORMAL,4,1,1);
    for(i=c.nodes.begin(), pos=7; i!=c.nodes.end();++i) {
      const Cluster::Node& n = (*i).second;
      bool good = n.status == "ALIVE";
      col = good && n.missCount==0 ? GREEN|BOLD|INVERSE : COL_ALARM;
      taskCount += n.taskCount;
      missCount += n.missCount;
      val = " "+(n.name == m_name ? n.name : n.name.substr(n.name.length()-2))+" ";
      ::scrc_put_chars(m_display,val.c_str(),col,line,pos,0);
      pos += val.length();
      if ( pos>DISP_WIDTH-5 ) ++line, pos=7;
    }
    col = (c.status=="ALIVE") ? NORMAL|BOLD : (c.status=="MIXED") ? COL_WARNING : COL_ALARM;
    ::sprintf(txt,"%-40s",c.time.c_str());
    ::scrc_put_chars(m_display,txt,NORMAL,1,1,0);
    ::sprintf(txt,"%s",c.status.c_str());
    ::scrc_put_chars(m_display,c.status.c_str(),col,1,22,1);
    ::sprintf(txt,"%2zd Nodes %3zd Tasks running, %2zd missing",c.nodes.size(),taskCount,missCount);
    ::scrc_put_chars(m_display,txt,col&~BOLD,2,1,1);
    if ( c.status == "DEAD" ) {
      ::scrc_put_chars(m_display,"Nodes down - Please check.",RED|BOLD,4,1,1);    
      //::scrc_set_border(m_display,m_title.c_str(),col);
    }
    else if ( c.status == "MIXED" ) {
      ::scrc_put_chars(m_display,"Some nodes down - Please check.",BOLD,4,1,1);    
      //::scrc_set_border(m_display,m_title.c_str(),col);
    }
    else if ( missCount>0 ) {
      ::scrc_put_chars(m_display,"Tasks missing - Please check.",NORMAL,4,1,1);    
      //::scrc_set_border(m_display,m_title.c_str(),col);
    }
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL|BOLD);
    IocSensor::instance().send(m_parent,CMD_CHECK,this);
    return;
  }
  ::scrc_put_chars(m_display," Received invalid XML information",COL_ALARM,1,1,1);    
  ::scrc_set_border(m_display,m_title.c_str(),COL_ALARM);
  IocSensor::instance().send(m_parent,CMD_CHECK,this);
}

/// Set timeout error
void CtrlFarmSubDisplay::setTimeoutError() {
  char txt[128];
  ::sprintf(txt," No update information for > %d seconds",UPDATE_TIME_MAX);
  ::scrc_set_border(m_display,m_title.c_str(),COL_ALARM);
  ::scrc_put_chars(m_display,txt,COL_ALARM,1,1,1);
}

/// Check display for errors
void CtrlFarmSubDisplay::check(time_t now) {
  if ( hasProblems() ) {
    if ( now - lastUpdate() > UPDATE_TIME_MAX ) {
      setTimeoutError();
    }
  }
}

/// Set the focus to this display
void CtrlFarmSubDisplay::setFocus() {
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
}

/// Release the focus of this display
void CtrlFarmSubDisplay::releaseFocus() {
}

/// Interactor overload: Display callback handler
void CtrlFarmSubDisplay::handle(const Event& ev) {
  switch(ev.eventtype) {
  case ScrMouseEvent: {
    const MouseEvent* m = ev.get<MouseEvent>();
    setFocus();
    IocSensor::instance().send(parent(),m->msec == (unsigned int)-1 ? CMD_POSCURSOR : CMD_SHOW,this);
    break;
  }
  default:
    break;
  }
}
