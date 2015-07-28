#include "ROMon/FarmDisplay.h"

namespace ROMon {

  /**@class RecFarmSubDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display summarizing one single subfarm. Showed as an array on the
   *   main display.
   *
   *   @author M.Frank
   */
  class RecFarmSubDisplay : public InternalDisplay  {
    long int          m_evtRecv;
    long int          m_evtReco;
    long int          m_evtSent;
    long int          m_totRecv;
    long int          m_totReco;
    long int          m_totSent;
    int               m_height;
    bool              m_hasProblems;
  public:
    /// Initializing constructor
    RecFarmSubDisplay(InternalDisplay* parent, const std::string& title, int height, bool bad=false);
    /// Standard destructor
    virtual ~RecFarmSubDisplay();
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

  InternalDisplay* createRecFarmSubDisplay(InternalDisplay* parent, const std::string& title, int height) {
    return new RecFarmSubDisplay(parent,title, height);
  }
}
#include <set>
#include <limits>
#include "RTL/strdef.h"
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
typedef MBMBuffer::Clients           Clients;
typedef Node::Buffers                Buffers;
typedef Nodeset::Nodes               Nodes;
static const long  LNG_max = numeric_limits<int>::max();
static const float FLT_max = numeric_limits<float>::max();

// Max. 15 seconds without update allowed
#define UPDATE_TIME_MAX 15

/// Initializing constructor
RecFarmSubDisplay::RecFarmSubDisplay(InternalDisplay* parent, const string& title, int height, bool bad) 
  : InternalDisplay(parent, title)
{
  m_evtSent = m_totSent = 0;
  m_evtReco = m_totReco = 0;
  m_evtRecv = m_totRecv = 0;
  m_lastUpdate = time(0);
  m_height = height;
  ::scrc_create_display(&m_display,m_height,48,NORMAL,ON,m_title.c_str());
  init(bad);
  string svc = svcPrefix()+strlower(title)+"/ROpublish";
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
  ::scrc_put_chars(m_display," ",col,m_height,1,1);
  ::scrc_put_chars(m_display,"Tot:",BOLD,2,1,1);
  ::scrc_put_chars(m_display,"Min:",BOLD,3,1,1);
  ::scrc_put_chars(m_display,"Max:",BOLD,4,1,1);
  ::scrc_put_chars(m_display,"  ",col,2,46,0);
  ::scrc_put_chars(m_display,"  ",col,3,45,0);
  ::scrc_put_chars(m_display,"  ",col,m_height,44,0);
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
  ::scrc_put_chars(m_display," ",NORMAL,2,1,1);
  ::scrc_put_chars(m_display," No update information available",BOLD|RED|INVERSE,3,1,1);
  ::scrc_put_chars(m_display," ",NORMAL,3,1,1);
  ::scrc_put_chars(m_display," ",NORMAL,m_height,1,1);
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
  long long int evt_prod[2]={0,0}, min_prod[2]  = {LNG_max,LNG_max};
  long int free_space[2]  = {0,0}, min_space[2] = {LNG_max,LNG_max};
  long int used_slots[2]  = {0,0}, min_slots[2] = {LNG_max,LNG_max};
  long int buf_clients[2] = {0,0};
  float fspace[2]    = {FLT_max,FLT_max};
  float fslots[2]    = {FLT_max,FLT_max};
  long int evt_sent  = LNG_max;
  long int evt_reco  = LNG_max;
  long int evt_recv  = LNG_max;
  bool inuse         = false;
  int numNodes       = 0;
  int numBuffs       = 0;
  int numClients     = 0;
  set<string> bad_nodes;

  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const Buffers& buffs = *(*n).buffers();
    numNodes++;
    long int node_evt_recv = 0;
    long int node_evt_sent = LNG_max;
    long int node_evt_reco = LNG_max;
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      int idx = 0;
      char b = (*ib).name[0];
      const MBMBuffer::Control& ctrl = (*ib).ctrl;
      numBuffs++;
      switch(b) {
      case INPUT_BUFFER:        idx = 0; break;
      case OUTPUT_BUFFER:       idx = 1; break;
      default:                        continue;
      }
      inuse = true;
      fspace[idx]       = ro_min(fspace[idx],float(ctrl.i_space)/float(ctrl.bm_size)); 
      fslots[idx]       = ro_min(fslots[idx],float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax));
      min_space[idx]    = ro_min(min_space[idx],(ctrl.i_space*ctrl.bytes_p_Bit)/1024/1024);
      min_slots[idx]    = ro_min(min_slots[idx],ctrl.p_emax-ctrl.i_events);
      min_prod[idx]     = ro_min(min_prod[idx],ctrl.tot_produced);
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
        const char* p = ::strchr((*ic).name,'_');
        if ( p ) p = ::strchr(++p,'_');
        if ( p ) {
          if ( b==INPUT_BUFFER && p[4] == REC_TASK )  {
            node_evt_reco = ro_min(node_evt_reco,(*ic).events);
          }
          else if ( b==INPUT_BUFFER && p[8] == REC_RECEIVER_TASK )  {
            node_evt_recv += (*ic).events;
          }
          else if ( b==OUTPUT_BUFFER && p[8] == REC_SENDER_TASK )  {
            node_evt_sent = ro_min(node_evt_sent,(*ic).events);
          }
        }
      }
    }
    evt_reco = ro_min(evt_reco,node_evt_reco);
    evt_recv = ro_min(evt_recv,node_evt_recv);
    evt_sent = ro_min(evt_sent,node_evt_sent);
  }
  char b1[64];
  TimeStamp frst=ns.firstUpdate();
  time_t t1 = numNodes == 0 ? time(0) : frst.first, now = time(0);
  ::strftime(b1,sizeof(b1),"%H:%M:%S",::localtime(&t1));
  ::sprintf(text," %s %s [%d nodes %d buffers %d clients] ",
            m_name.c_str(),b1,numNodes,numBuffs,numClients);
  m_title = text;
  if ( numNodes != 0 ) {
    m_lastUpdate = t1;
  }
  m_hasProblems = true;

  if ( buf_clients[0] != 0 )
    ::sprintf(txt,"%9lld%4ld%6ld   %9lld%4ld%6ld",
              evt_prod[0],buf_clients[0],used_slots[0],
              evt_prod[1],buf_clients[1],used_slots[1]);
  else
    ::sprintf(txt,"%9s%4s%6s   %9s%4s%6s","--","","--","--","","--");
  ::scrc_put_chars(m_display,txt,NORMAL,2,5,1);
  if ( buf_clients[0] != 0 )
    ::sprintf(txt,"%9lld%4s%6ld   %9lld%4s%6ld",min_prod[0],"",min_slots[0],min_prod[1],"",min_slots[1]);
  else
    ::sprintf(txt,"%9s%4s%6s   %9s%4s%6s","--","","--","--","","--");
  ::scrc_put_chars(m_display,txt,NORMAL,3,5,1);

  if ( numNodes == 0 ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::scrc_put_chars(m_display," No nodes found in this subfarm!",BOLD|RED|INVERSE,m_height,1,1);
  }
  else if ( now-m_lastUpdate > UPDATE_TIME_MAX ) {
    setTimeoutError();
  }
  else if ( !inuse ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," Subfarm not in used by any partition....",NORMAL|INVERSE|GREEN,m_height,1,1);
  }
  else if ( fslots[0] < SLOTS_MIN || fslots[1] < SLOTS_MIN ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::sprintf(txt," SLOTS at limit:");
    if ( fslots[0] < SLOTS_MIN ) ::strcat(txt,"INPUT ");
    if ( fslots[1] < SLOTS_MIN ) ::strcat(txt,"OUTPUT ");
    ::sprintf(text,"[%d nodes]",int(bad_nodes.size()));
    ::strcat(txt,text);
    ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,m_height,1,1);
  }
  else if ( fspace[0] < SPACE_MIN || fspace[1] < SPACE_MIN ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::sprintf(txt," SPACE at limit:");
    if ( fspace[0] < SPACE_MIN ) ::strcat(txt,"INPUT ");
    if ( fspace[1] < SPACE_MIN ) ::strcat(txt,"OUTPUT ");
    ::sprintf(text,"[%d nodes]",int(bad_nodes.size()));
    ::strcat(txt,text);
    ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,m_height,1,1);
  }
  else if ( inuse && evt_recv <= m_evtRecv && evt_prod[0] == m_totRecv ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No reconstruction activity visible.",BOLD|RED,m_height,1,1);
  }
  else if ( evt_reco <= m_evtReco && evt_prod[1] > m_totReco ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::scrc_put_chars(m_display," Some RECO(s) stuck.",BOLD|RED|INVERSE,m_height,1,1);
  }
  else if ( inuse && evt_reco <= m_evtReco && evt_prod[1] == m_totReco ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No activity visible.",BOLD|RED,m_height,1,1);
  }
  else if ( evt_sent <= m_evtSent && evt_prod[1] > m_totSent ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::scrc_put_chars(m_display," Some Sender(s) stuck.",BOLD|RED|INVERSE,m_height,1,1);
  }
  else if ( inuse && evt_sent <= m_evtSent && evt_prod[1] == m_totSent ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No STORAGE activity visible.",BOLD|RED,m_height,1,1);
  }
  else {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display,"",NORMAL,m_height,1,1);
    ::scrc_put_chars(m_display," No obvious Errors detected....",NORMAL|INVERSE|GREEN,m_height,1,1);
    m_hasProblems = false;
  }
  m_evtRecv = evt_recv;
  m_evtReco = evt_reco;
  m_evtSent = evt_sent;
  m_totRecv = evt_prod[0];
  m_totReco = evt_prod[1];
  m_totSent = evt_prod[1];

  IocSensor::instance().send(m_parent,CMD_CHECK,this);
}
