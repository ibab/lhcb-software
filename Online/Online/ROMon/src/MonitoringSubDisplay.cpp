#include "ROMon/FarmDisplay.h"

namespace ROMon {

  /**@class MonitoringSubDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display summarizing one single subfarm. Showed as an array on the
   *   main display.
   *
   *   @author M.Frank
   */
  class MonitoringSubDisplay : public InternalDisplay  {
    int               m_evtRelay;
    int               m_evtWorker;
    int               m_totRelay;
    int               m_totWorker;
    int               m_numUpdate;
    int               m_height;
    bool              m_hasProblems;
    bool              m_inUse;
    std::string       m_partition;
    std::string       m_relayNode;

  public:
    /// Initializing constructor
    MonitoringSubDisplay(InternalDisplay* parent, const std::string& title, int height, bool bad=false);
    /// Standard destructor
    virtual ~MonitoringSubDisplay();
    /// Initialize default display text
    void init(bool bad);
    /// Access to problem flag
    bool hasProblems() const { return m_hasProblems; }
    /// Set timeout error
    void setTimeoutError();
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
    /// Update display content
    virtual void updateContent(const Nodeset& ns);
    /// Update display content
    virtual void update(const void* data, size_t len)
    { this->InternalDisplay::update(data,len); }
  };

  InternalDisplay* createMonitoringSubDisplay(InternalDisplay* parent, const std::string& title, int height) {
    return new MonitoringSubDisplay(parent,title,height);
  }
}
#include <set>
#include <limits>
#include "CPP/IocSensor.h"
#include "SCR/MouseSensor.h"
#include "SCR/scr.h"
#include "ROMonDefs.h"
extern "C" {
#include "dic.h"
}

using namespace ROMon;
using namespace SCR;
using namespace std;
// Max. 15 seconds without update allowed
#define UPDATE_TIME_MAX 15

/// Initializing constructor
MonitoringSubDisplay::MonitoringSubDisplay(InternalDisplay* parent, const string& title, int height, bool bad) 
  : InternalDisplay(parent, title)
{
  m_numUpdate = 0;
  m_evtRelay  = m_totRelay = 0;
  m_evtWorker = m_totWorker = 0;
  m_partition = parent->name();
  m_relayNode = m_name+"01";
  m_lastUpdate = time(0);
  m_height     = height;
  ::scrc_create_display(&m_display,m_height,48,NORMAL,ON,m_title.c_str());
  init(bad);
  string svc = svcPrefix()+strlower(title)+"/ROpublish";
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
  m_hasProblems = false;
  MouseSensor::instance().add(this,m_display);
}

/// Standard destructor
MonitoringSubDisplay::~MonitoringSubDisplay() {
  MouseSensor::instance().remove(m_display);
}

/// Initialize default display text
void MonitoringSubDisplay::init(bool bad) {
  int col = bad ? INVERSE|RED : NORMAL;
  char txt[128];
  ::sprintf(txt,"%-7s%9s%4s%5s%9s%4s%4s",
            "","Events","Cl","Sl","Output","Cl","Sl");
  ::scrc_put_chars(m_display,txt,col|INVERSE,1,1,1);
  ::scrc_put_chars(m_display," ",col,2,1,1);
  ::scrc_put_chars(m_display," ",col,3,1,1);
  ::scrc_put_chars(m_display," ",col,m_height,1,1);
  ::scrc_put_chars(m_display,"Worker:",BOLD,2,1,1);
  ::scrc_put_chars(m_display,"  ",col,2,46,0);
  ::scrc_put_chars(m_display,"  ",col,3,45,0);
  ::scrc_put_chars(m_display,"  ",col,m_height,45,0);
  ::scrc_set_border(m_display,m_title.c_str(),col|BOLD);
}

/// DIM command service callback
void MonitoringSubDisplay::update(const void* address) {
  const Nodeset* ns = (const Nodeset*)address;
  if ( ns->type == Nodeset::TYPE ) {
    updateContent(*ns);
  }
}

/// Set timeout error
void MonitoringSubDisplay::setTimeoutError() {
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
  ::scrc_put_chars(m_display," No update information available",BOLD|RED|INVERSE,4,1,1);
}

/// Check display for errors
void MonitoringSubDisplay::check(time_t now) {
  if ( hasProblems() ) {
    if ( now - lastUpdate() > UPDATE_TIME_MAX ) {
      setTimeoutError();
    }
  }
}

/// Set the focus to this display
void MonitoringSubDisplay::setFocus() {
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
}

/// Release the focus of this display
void MonitoringSubDisplay::releaseFocus() {
}

/// Interactor overload: Display callback handler
void MonitoringSubDisplay::handle(const Event& ev) {
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
void MonitoringSubDisplay::updateContent(const Nodeset& ns) {
  typedef Node::Buffers                Buffers;
  typedef Nodeset::Nodes               Nodes;
  char txt[128], text[128];
  bool inuse = false;
  set<string> bad_nodes;
  static const int   INT_max = numeric_limits<int>::max();
  static const float FLT_max = numeric_limits<float>::max();

  string evt_buff = std::string("Events_"+m_partition);
  string out_buff = std::string("Output_"+m_partition);
  int numNodes = 0, numBuffs = 0, numClients = 0;
  int num_cl[3] = {0,0,0}, num_sl[3] = {0,0,0};
  long long int tot_prod[3] = {0,0,0}, min_prod[3] = {INT_max,INT_max,INT_max};

  float fsp, fspace[3] = {FLT_max,FLT_max,FLT_max};
  float fsl, fslots[3] = {FLT_max,FLT_max,FLT_max};

  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    bool relay = ::strncasecmp((*n).name,m_relayNode.c_str(),m_relayNode.length()) == 0;
    const Buffers& buffs = *(*n).buffers();
    ++numNodes;
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      const char* bnam = (*ib).name;
      if ( ::strstr(bnam,m_partition.c_str()) != 0 ) {
        int idx = -1;
        if ( relay && evt_buff == bnam )
          idx = 0;
        else if ( evt_buff == bnam )
          idx = 1;
        else if ( out_buff == bnam )
          idx = 2;
        if ( idx >= 0 ) {
          const MBMBuffer::Control& ctrl = (*ib).ctrl;
          int ncl = (*ib).clients.size();
          numClients += ncl;
          ++numBuffs;
          tot_prod[idx] += ctrl.tot_produced;
          num_cl[idx]   += ncl;
          num_sl[idx]   += ctrl.p_emax - ctrl.i_events;

          min_prod[idx] = min(min_prod[idx],ctrl.tot_produced);
          fsl = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
          fsp = float(ctrl.i_space)/float(ctrl.bm_size);
          fspace[idx]   = min(fspace[idx],fsp);
          fslots[idx]   = min(fslots[idx],fsl);
          if ( fsl < SLOTS_MIN || fsp < SPACE_MIN ) {
            bad_nodes.insert((*n).name);
          }
          inuse = true;
        }
      }
    }
  }
  char b1[64];
  TimeStamp frst=ns.firstUpdate();
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

  if ( tot_prod[1] != 0 && tot_prod[2] != 0 ) {
    ::sprintf(txt,"%-7s%9s%4s%5s%9s%4s%4s","","Events","Cl","Sl","Output","Cl","Sl");
    ::scrc_put_chars(m_display,txt,INVERSE,1,1,1);
    ::sprintf(txt,"%9lld%4d%5d%9lld%4d%4d",
              tot_prod[1],num_cl[1],num_sl[1],
              tot_prod[2],num_cl[2],num_sl[2]);
  }
  else if ( tot_prod[1] != 0 ) {
    ::sprintf(txt,"%-7s%9s%4s%5s%17s","","Events","Cl","Sl","");
    ::scrc_put_chars(m_display,txt,INVERSE,1,1,1);
    ::sprintf(txt,"%9lld%4d%5d%17s",tot_prod[1],num_cl[1],num_sl[1],"  No Output streams");
  }
  else {
    ::sprintf(txt,"%-7s%9s%4s%5s%17s","","Events","Cl","Sl","");
    ::scrc_put_chars(m_display,txt,INVERSE,1,1,1);
    ::sprintf(txt,"%9s%4s%5s%17s","--","--","--","");
  }
  ::scrc_put_chars(m_display,"Worker:",BOLD,2,1,1);
  ::scrc_put_chars(m_display,txt,NORMAL,2,8,1);

  if ( tot_prod[0] != 0 )
    ::sprintf(txt,"%9lld%4d%5d%17s",tot_prod[0],num_cl[0],num_sl[0],"");
  else
    ::sprintf(txt,"%9s%4s%5s%17s","--","--","--","");
  ::scrc_put_chars(m_display,"Relay: ",BOLD,3,1,1);
  ::scrc_put_chars(m_display,txt,NORMAL,3,8,1);


  if ( now-m_lastUpdate > UPDATE_TIME_MAX ) {
    setTimeoutError();
  }
  else if ( numNodes == 0 ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::scrc_put_chars(m_display," No nodes found in this cluster!",BOLD|RED|INVERSE,m_height,1,1);
  }
  else if ( !inuse ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," Monitoring cluster not used yet....",NORMAL|INVERSE|GREEN,m_height,1,1);
  }
  else if ( fslots[0] < SLOTS_MIN || fslots[1] < SLOTS_MIN || fslots[2] < SLOTS_MIN ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::sprintf(txt," SLOTS at limit:");
    if ( fslots[0] < SLOTS_MIN ) ::strcat(txt,"Relay ");
    if ( fslots[1] < SLOTS_MIN ) ::strcat(txt,"Events ");
    if ( fslots[2] < SLOTS_MIN ) ::strcat(txt,"Output ");
    ::sprintf(text,"[%d nodes]",int(bad_nodes.size()));
    ::strcat(txt,text);
    ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,m_height,1,1);
  }
  else if ( fspace[0] < SPACE_MIN || fspace[1] < SPACE_MIN || fspace[2] < SPACE_MIN  ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::sprintf(txt," SPACE at limit:");
    if ( fspace[0] < SPACE_MIN ) ::strcat(txt,"Relay ");
    if ( fspace[1] < SPACE_MIN ) ::strcat(txt,"Events ");
    if ( fspace[2] < SPACE_MIN ) ::strcat(txt,"Output ");
    ::sprintf(text,"[%d nodes]",int(bad_nodes.size()));
    ::strcat(txt,text);
    ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,m_height,1,1);
  }
  else if ( tot_prod[0]>0 && tot_prod[0] <= m_totRelay ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No RELAY activity visible.",BOLD|RED,m_height,1,1);
  }
  else if ( tot_prod[1]>0 && tot_prod[1] <= m_totWorker ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No Worker activity visible.",BOLD|RED,m_height,1,1);
  }
  else if ( min_prod[0] != INT_max && min_prod[0]>0 && min_prod[0] <= m_evtRelay ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," Relay node looks idle.",BOLD|RED,m_height,1,1);
  }
  else if ( min_prod[1] != INT_max && min_prod[1]>0 && min_prod[1] <= m_evtWorker ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," Some workers idle around.",BOLD|RED,m_height,1,1);
  }
  else if ( m_height>3 )  {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No obvious Errors detected....",NORMAL|INVERSE|GREEN,m_height,1,1);
  }
  else {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL|INVERSE|GREEN);
  }

  m_evtRelay  = min_prod[0];
  m_totRelay  = tot_prod[0];
  m_evtWorker = min_prod[1];
  m_totWorker = tot_prod[1];

  IocSensor::instance().send(m_parent,CMD_CHECK,this);
}
