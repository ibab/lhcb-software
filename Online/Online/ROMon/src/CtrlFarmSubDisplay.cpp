#include "ROMon/FarmDisplay.h"
#include "SCR/MouseSensor.h"
#include "CPP/IocSensor.h"
#include "RTL/Lock.h"
#include "SCR/scr.h"
#include "ROMonDefs.h"
extern "C" {
#include "dic.h"
}
#include <set>

using namespace ROMon;
using namespace SCR;
using namespace std;

#include "ROMon/TaskSupervisor.h"
#include "ROMon/TaskSupervisorParser.h"

namespace ROMon {

  /**@class CtrlFarmSubDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display summarizing one single subfarm. Showed as an array on the
   *   main display.
   *
   *   @author M.Frank
   */
  class CtrlFarmSubDisplay : public InternalDisplay {
    typedef std::set<std::string> StrSet;
    /// Flag to indicate probles with entity
    bool                m_hasProblems;
    /// Extracted cluster information for all contained nodes
    Cluster             m_cluster;
    /// Set of excluded nodes
    StrSet              m_excluded;
    /// Display height
    int                 m_height;
  public:
    /// Initializing constructor
    CtrlFarmSubDisplay(InternalDisplay* parent, const string& title, int height, bool bad=false);
    /// Standard destructor
    virtual ~CtrlFarmSubDisplay();
    /// Initialize default display text
    void init(bool bad);
    /// Access to problem flag
    bool hasProblems() const { return m_hasProblems; }
    /// Set timeout error
    void setTimeoutError();
    /// Update display content
    virtual void update(const void* data);
    /// Update display content
    virtual void update(const void* data, size_t len)
    { this->InternalDisplay::update(data,len); }
    /// Command service callback after already parsed data
    void updateContent(XML::TaskSupervisorParser& ts);
    /// Check display for errors
    virtual void check(time_t stamp);
    /// Set the focus to this display
    virtual void setFocus();
    /// Release the focus of this display
    virtual void releaseFocus();
    /// Interactor overload: Display callback handler
    virtual void handle(const Event& ev);
  };
  InternalDisplay* createCtrlFarmSubDisplay(InternalDisplay* parent, const string& title, int height) {
    return new CtrlFarmSubDisplay(parent,title,height);
  }
}

#define UPDATE_TIME_MAX 30
#define DISP_WIDTH      48

#define COL_WARNING         (RED|BOLD)
#define COL_ALARM           (RED|BOLD|INVERSE)

/// Initializing constructor
CtrlFarmSubDisplay::CtrlFarmSubDisplay(InternalDisplay* parent, const string& title, int height, bool bad)
  : InternalDisplay(parent, title)
{
  m_height = height;
  m_lastUpdate = time(0);
  ::scrc_create_display(&m_display,m_height,DISP_WIDTH,createFlags(),ON,m_title.c_str());
  init(bad);
  string svc   = "HLT/ExcludedNodes/"+strupper(title);
  m_svc2 = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,excludedHandler,(long)this,0,0);
  svc = svcPrefix()+strupper(title)+"/TaskSupervisor/Status";
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
  ::scrc_put_chars(m_display,"     No information availible",COL_ALARM,m_height,1,1);
  ::scrc_set_border(m_display,m_title.c_str(),COL_WARNING);
}

/// DIM command service callback
void CtrlFarmSubDisplay::update(const void* address) {
  const char* data = (const char*)address;
  try {
    XML::TaskSupervisorParser ts;
    if ( ts.parseBuffer(m_name, data,::strlen(data)+1) ) {
      updateContent(ts);
    }
    else {
      ::scrc_put_chars(m_display," Received invalid XML information",COL_ALARM,1,1,1);
      ::scrc_set_border(m_display,m_title.c_str(),COL_ALARM);
    }
    IocSensor::instance().send(m_parent,CMD_CHECK,this);
  }
  catch(...) {
  }
}

/// DIM command service callback
void CtrlFarmSubDisplay::updateContent(XML::TaskSupervisorParser& ts) {
  bool twoline = false;
  char txt[128];
  string val, border = m_title;
  bool cl_good = true;
  Cluster& c = m_cluster;
  Cluster::Nodes::const_iterator i, e;
  int col = NORMAL, pos = 0, line=2;
  size_t taskCount=0, missTaskCount=0;
  size_t connCount=0, missConnCount=0;
  int pvss_status=0;
  c.nodes.clear();
  ts.getClusterNodes(c);
  ::scrc_put_chars(m_display,"", NORMAL,m_height,1,1);
  for(i=c.nodes.begin(), e=c.nodes.end(), pos=1; i!=e;++i) {
    const Cluster::Node& n = (*i).second;
    bool excl = m_excluded.find(n.name) != m_excluded.end();
    bool good = (n.status == "ALIVE");
    for(Cluster::Projects::const_iterator q=n.projects.begin(); q != n.projects.end(); ++q) {
      bool pvss_ok = (*q).eventMgr && (*q).dataMgr && (*q).distMgr;
      if ( pvss_ok && pvss_status<2 ) pvss_status=1;
      else if ( !pvss_ok ) pvss_status = 2;
      good = good && pvss_ok;
    }
    col = good && n.missTaskCount==0 && n.missConnCount==0 ? (m_height>2 ? GREEN|INVERSE : NORMAL) : COL_ALARM;
    if ( excl )  {
      col = INVERSE|(col==COL_ALARM ? MAGENTA : BLUE);
    }
    else {
      taskCount     += n.taskCount;
      missTaskCount += n.missTaskCount;
      connCount     += n.connCount;
      missConnCount += n.missConnCount;
    }
    val = (n.name == m_name ? n.name : " "+n.name.substr(n.name.length()-2));
    if ( twoline && pos > DISP_WIDTH-4 ) {
      ::scrc_put_chars(m_display,"...",col,line,pos,0);
    }
    else {
      ::scrc_put_chars(m_display,val.c_str(),col,line,pos,0);
      pos += val.length();
    }
    if ( !twoline && pos>DISP_WIDTH-2 )  {
      ::scrc_put_chars(m_display,"",NORMAL,line,pos,1);
      ++line;
      pos=1;
      twoline=true;
    }
    cl_good |= (good || excl);
  }
  ::scrc_put_chars(m_display,"",NORMAL,line,pos,1);
  col = (c.status=="ALIVE" || cl_good) ? NORMAL|BOLD : (c.status=="MIXED") ? COL_WARNING : COL_ALARM;
  size_t nchar = ::sprintf(txt,"%-12s",pvss_status>0 ? pvss_status==1 ? "PVSS Ok" : "PVSS Errors" : "");
  ::scrc_put_chars(m_display,txt,col,1,1,1);
  nchar  = ::sprintf(txt,"%2d Nodes ",int(c.nodes.size()));
  nchar += ::sprintf(txt+nchar,"%3d %sTasks ",
                     int(missTaskCount>0 ? missTaskCount : taskCount),
                     missTaskCount>0 ? "BAD " : "");
  nchar += ::sprintf(txt+nchar,"%3d %sConnections ",
                     int(missConnCount>0 ? missConnCount : connCount),
                     missConnCount>0 ? "BAD " : "");
  ::scrc_put_chars(m_display,txt,col&~BOLD,1,13,1);

  ::sprintf(txt," - %s",c.time.c_str());
  border = m_title + " - " + c.status + " - " + c.time;
  col = NORMAL|BOLD;
  struct tm tm;
  ::strptime(c.time.c_str(),"%Y-%m-%d %H:%M:%S",&tm);
  if ( ::time(0)-(m_lastUpdate=::mktime(&tm)) > UPDATE_TIME_MAX ) {
    setTimeoutError();
  }
  else if ( pvss_status>1 ) {
    ::scrc_put_chars(m_display,"PVSS environment looks funny - Please Check.",COL_ALARM,m_height,1,1);
    ::scrc_set_border(m_display,border.c_str(),COL_WARNING);
  }
  else if ( !cl_good && c.status == "DEAD" ) {
    ::scrc_put_chars(m_display,"",NORMAL,1,1,0);
    ::scrc_put_chars(m_display,"",NORMAL,m_height,1,0);
    ::scrc_put_chars(m_display,"Nodes down - Please check.",COL_WARNING,m_height,1,1);
    ::scrc_set_border(m_display,border.c_str(),COL_ALARM);
  }
  else if ( !cl_good && c.status == "MIXED" ) {
    ::scrc_put_chars(m_display,"Some nodes down - Please check.",BOLD,m_height,1,1);
    ::scrc_set_border(m_display,border.c_str(),m_height<=2 ? NORMAL|INVERSE|RED : col);
  }
  else if ( c.status == "MIXED" ) {
    if ( m_height > 3 ) ::scrc_put_chars(m_display,"Some nodes are down.",BOLD,m_height,1,1);
    ::scrc_set_border(m_display,border.c_str(),m_height<=2 ? NORMAL|INVERSE|RED : col);
  }
  else if ( missTaskCount>0 ) {
    ::scrc_put_chars(m_display,"Tasks missing - Please check.",NORMAL,m_height,1,1);
    ::scrc_set_border(m_display,border.c_str(),col);
  }
  else if ( missConnCount>0 ) {
    ::scrc_put_chars(m_display,"Connectivity bad - Please check.",NORMAL,m_height,1,1);
    ::scrc_set_border(m_display,border.c_str(),col);
  }
  else if ( !twoline || m_height>3 )  {
    ::scrc_put_chars(m_display,"No obvious error detected.",NORMAL|GREEN,m_height,1,1);
    ::scrc_set_border(m_display,border.c_str(),NORMAL|BOLD);
  }
  else if ( m_height<=2 )  {
    ::scrc_set_border(m_display,border.c_str(),NORMAL|INVERSE|GREEN);
  }
  else  {
    ::scrc_set_border(m_display,border.c_str(),NORMAL|BOLD);
  }
}

/// Set timeout error
void CtrlFarmSubDisplay::setTimeoutError() {
  char txt[128];
  ::scrc_put_chars(m_display,"                     UNKNOWN ",COL_WARNING,1,1,1);
  ::sprintf(txt," No update information for > %d seconds",UPDATE_TIME_MAX);
  ::scrc_put_chars(m_display,txt,COL_ALARM,m_height,1,1);
  ::scrc_set_border(m_display,m_title.c_str(),COL_WARNING);
}

/// Check display for errors
void CtrlFarmSubDisplay::check(time_t now) {
  if ( hasProblems() || (now - lastUpdate()) > UPDATE_TIME_MAX )
    setTimeoutError();
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
  const MouseEvent* m;
  switch(ev.eventtype) {
  case ScrMouseEvent:
    m = ev.get<MouseEvent>();
    if ( m->button == 2 ) {
      IocSensor::instance().send(parent(),CMD_SHOWHELP,this);
      return;
    }
    setFocus();
    IocSensor::instance().send(parent(),m->msec == (unsigned int)-1 ? CMD_POSCURSOR : CMD_SHOW,this);
    return;
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
  InternalDisplay::handle(ev);
}
