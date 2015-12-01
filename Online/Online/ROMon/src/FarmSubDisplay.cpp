#include "ROMon/FarmDisplay.h"

namespace ROMon {

  /**@class FarmSubDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display summarizing one single subfarm. Showed as an array on the
   *   main display.
   *
   *   @author M.Frank
   */
  class FarmSubDisplay : public InternalDisplay  {
    std::string       m_partition;
    long long int     m_evtBuilt;
    long long int     m_evtMoore;
    long long int     m_evtSent;
    long long int     m_evtOvl;
    long long int     m_totBuilt;
    long long int     m_totMoore;
    long long int     m_totSent;
    long long int     m_totOvl;
    int               m_numUpdate;
    int               m_height;
    bool              m_hasProblems;
    bool              m_inUse;

  public:
    /// Initializing constructor
    FarmSubDisplay(InternalDisplay* parent, 
                   const std::string& title, 
                   const std::string& partition, 
                   int height, bool bad=false);
    /// Standard destructor
    virtual ~FarmSubDisplay();
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
    /// Check display for errors
    virtual void check(time_t stamp);
    /// Set the focus to this display
    virtual void setFocus();
    /// Release the focus of this display
    virtual void releaseFocus();
    /// Interactor overload: Display callback handler
    virtual void handle(const CPP::Event& ev);
    /// Update display content
    virtual void updateContent(const Nodeset& ns);
  };

  InternalDisplay* createFarmSubDisplay(InternalDisplay* parent, 
                                        const std::string& title,
                                        const std::string& partition,
                                        int height) {
    return new FarmSubDisplay(parent,title,partition,height);
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
// Max. 15 seconds without update allowed
#define UPDATE_TIME_MAX 35

namespace {
  const char* _procNam(const char* nam) {
    const char* p;
    if (0 != ::strstr(nam,"MEPRx") ) return nam;
    p = ::strchr(nam,'_');
    if ( 0 != p ) return ++p;
    return "Unknown";
  }
}

/// Initializing constructor
FarmSubDisplay::FarmSubDisplay(InternalDisplay* parent, 
                               const string& title, 
                               const string& part,
                               int height, bool bad) 
  : InternalDisplay(parent, title), m_partition(part)
{
  m_numUpdate = 0;
  m_evtOvl    = m_totOvl = 0;
  m_evtSent   = m_totSent = 0;
  m_evtMoore  = m_totMoore = 0;
  m_evtBuilt  = m_totBuilt = 0;
  m_lastUpdate = time(0);
  m_height = height;
  ::scrc_create_display(&m_display,height,48,NORMAL,ON,m_title.c_str());
  init(bad);
  string svc = svcPrefix()+strlower(title)+"/ROpublish";
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
  ::sprintf(txt,"%-4s%9s%5s%11s%6s%9s%4s",
            "","Overflow","Sl","Events","Sl","Send","Sl");
  ::scrc_put_chars(m_display,txt,col|INVERSE,1,1,1);
  //::scrc_put_chars(m_display,txt,col|BOLD,1,1,1);
  ::scrc_put_chars(m_display," ",col,2,1,1);
  ::scrc_put_chars(m_display," ",col,3,1,1);
  ::scrc_put_chars(m_display," ",col,m_height,1,1);
  ::scrc_put_chars(m_display,"Tot:",BOLD,2,1,1);
  ::scrc_put_chars(m_display,"Min:",BOLD,3,1,1);
  ::scrc_put_chars(m_display,"  ",col,2,46,0);
  ::scrc_put_chars(m_display,"  ",col,3,46,0);
  ::scrc_put_chars(m_display,"  ",col,m_height,44,0);
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
  ::scrc_put_chars(m_display," No update information available",BOLD|RED|INVERSE,m_height,1,1);
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
void FarmSubDisplay::handle(const CPP::Event& ev) {
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
  typedef MBMBuffer::Clients           Clients;
  typedef Node::Buffers                Buffers;
  typedef Nodeset::Nodes               Nodes;
  static const long long LNG_max = numeric_limits<long long>::max();
  static const float     FLT_max = numeric_limits<float>::max();

  char txt[128], text[128];
  long long int evt_prod[4] = {0,0,0,0}, min_prod[4]  = {LNG_max,LNG_max,LNG_max,LNG_max};
  long int free_space[4]    = {0,0,0,0}, min_space[4] = {LNG_max,LNG_max,LNG_max,LNG_max};
  long int free_slots[4]    = {0,0,0,0}, min_slots[4] = {LNG_max,LNG_max,LNG_max,LNG_max};
  int buf_clients[4] = {0,0,0,0};
  float fspace[4]    = {FLT_max,FLT_max,FLT_max,FLT_max};
  float fslots[4]    = {FLT_max,FLT_max,FLT_max,FLT_max};
  float fsl, fsp;
  long int evt_ovl   = LNG_max;
  long int evt_sent  = LNG_max;
  long int evt_moore = LNG_max;
  long int evt_built = LNG_max;
  bool inuse         = false;
  int numNodes       = 0;
  int numBuffs       = 0;
  int numClients     = 0;
  set<string> bad_nodes;
  string part = "_"+m_partition;

  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const Buffers& buffs = *(*n).buffers();
    numNodes++;
    long int node_evt_ovl = 0;
    long int node_evt_mep = 0;
    long int node_evt_sent = LNG_max;
    long int node_evt_moore = LNG_max;
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      int idx = 0;
      const char* bn = (*ib).name;
      char b = bn[0];
      if ( !ro_match_end(m_partition,bn) ) continue;
      const MBMBuffer::Control& ctrl = (*ib).ctrl;
      ++numBuffs;
      switch(b) {
      case MEP_BUFFER:        idx = 0; break;
      case EVT_BUFFER:        idx = 1; break;
      case RES_BUFFER:
      case SND_BUFFER:        idx = 2; break;
      case OVL_BUFFER:        idx = 3; break;
      default:                continue;
      }
      inuse = true;
      fsp               = float(ctrl.i_space)/float(ctrl.bm_size);
      fsl               = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
      fspace[idx]       = ro_min(fspace[idx],fsp); 
      fslots[idx]       = ro_min(fslots[idx],fsl);
      min_space[idx]    = ro_min(min_space[idx],(ctrl.i_space*ctrl.bytes_p_Bit)/1024/1024);
      min_slots[idx]    = ro_min(min_slots[idx],ctrl.p_emax-ctrl.i_events);
      min_prod[idx]     = ro_min(min_prod[idx],ctrl.tot_produced);
      evt_prod[idx]    += ctrl.tot_produced;
      free_space[idx]  += (ctrl.i_space*ctrl.bytes_p_Bit)/1024/1024;
      free_slots[idx]  += (ctrl.p_emax-ctrl.i_events);
      buf_clients[idx] += ctrl.i_users;
      if ( fsl < SLOTS_MIN || fsp < SPACE_MIN ) {
        bad_nodes.insert((*n).name);
      }
      const Clients& clients = (*ib).clients;
      for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
        ++numClients;
        const char* p = _procNam((*ic).name);
        switch(*p) {
        case BUILDER_TASK:
          //  MEP schema         DeferHLT schema
          if( b == MEP_BUFFER || b == EVT_BUFFER ) {
            node_evt_mep += (*ic).events;
          }
          break;
        case SENDER_TASK:
          //  Orig.MEP schema    MEP or DeferHLT schema
          if( b == RES_BUFFER || b == SND_BUFFER )  {
            node_evt_sent = ro_min(node_evt_sent,(*ic).events);
          }
          break;
        case MOORE_TASK:
          //  Normal  and        TAE event processing
          //  MEP or DeferHLT    Orig.MEP schmema
          if( b == EVT_BUFFER || b == MEP_BUFFER )  {
            node_evt_moore = ro_min(node_evt_moore,long((*ic).events));
          }
          break;
        case OVLWR_TASK:
          if( b == OVL_BUFFER )  {
            node_evt_ovl = ro_min(node_evt_ovl,long((*ic).events));
          }
          break;
        default:
          break;
        }
      }
    }
    evt_ovl   = ro_min(evt_ovl,node_evt_ovl);
    evt_moore = ro_min(evt_moore,node_evt_moore);
    evt_built = ro_min(evt_built,node_evt_mep);
    evt_sent  = ro_min(evt_sent,node_evt_sent);
  }
  char b1[64];
  TimeStamp frst=ns.firstUpdate();
  time_t t1 = numNodes == 0 ? time(0) : frst.first, prev_update=m_lastUpdate;
  ::strftime(b1,sizeof(b1),"%H:%M:%S",::localtime(&t1));
  ::sprintf(text," %s %s %d nodes %d buffs %d clts",
            m_name.c_str(),b1,numNodes,numBuffs,numClients);
  m_title = text;
  m_inUse = inuse;

  if ( numNodes != 0 ) {
    m_lastUpdate = t1;
  }
  m_hasProblems = true;

  bool slots_min = fslots[0] < SLOTS_MIN || fslots[1] < SLOTS_MIN || 
    fslots[2] < SLOTS_MIN || fslots[3] < SLOTS_MIN;
  bool space_min = fspace[0] < SPACE_MIN || fspace[1] < SPACE_MIN || 
    fspace[2] < SPACE_MIN || fspace[3] < SPACE_MIN;

  if ( evt_prod[0] || evt_prod[1] )
    ::sprintf(txt,"%9lld%5ld%11lld%6ld%9lld%5ld",
              evt_prod[3],free_slots[3],
              evt_prod[1],free_slots[1],
              evt_prod[2],free_slots[2]);
  else
    ::sprintf(txt,"%9s%5s%10s%7s%9s%5s","--","--","--","--","--","--");
  ::scrc_put_chars(m_display,txt,NORMAL,2,5,1);
  if ( min_prod[0] != LNG_max || min_prod[1] != LNG_max )
    ::sprintf(txt,"%9lld%5ld%11lld%6ld%9lld%5ld",
              min_prod[3],min_slots[3],
              min_prod[1],min_slots[1],
              min_prod[2],min_slots[2]);
  else
    ::sprintf(txt,"%9s%5s%10s%7s%9s%5s","--","--","--","--","--","--");
  ::scrc_put_chars(m_display,"Min:",BOLD,3,1,1);
  ::scrc_put_chars(m_display,txt,NORMAL,3,5,1);

  if ( prev_update-m_lastUpdate > UPDATE_TIME_MAX ) {
    setTimeoutError();
  }
  else if ( numNodes == 0 ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::scrc_put_chars(m_display," No nodes found in this subfarm!",BOLD|RED|INVERSE,m_height,1,1);
  }
  else if ( !inuse ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," Subfarm not used by any partition....",NORMAL|INVERSE|GREEN,m_height,1,1);
  }
  else if ( evt_built <= m_evtBuilt && evt_prod[0]<m_totBuilt ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::sprintf(txt," Some MEPRx(s) stuck.");
    ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,m_height,1,1);
  }
  else if ( evt_built <= m_evtBuilt && evt_prod[0] == m_totBuilt ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No DAQ activity visible.",BOLD|RED,m_height,1,1);
  }
  else if ( evt_moore+2 <= m_evtMoore && evt_prod[1] > m_totMoore ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::scrc_put_chars(m_display," Some MOORE(s) stuck.",BOLD|RED|INVERSE,m_height,1,1);
  }
  else if ( evt_moore <= m_evtMoore && evt_prod[1] == m_totMoore ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No HLT activity visible.",BOLD|RED,m_height,1,1);
  }
  else if ( evt_sent+2 <= m_evtSent && evt_prod[2] > m_totSent ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::scrc_put_chars(m_display," Some Sender(s) stuck.",BOLD|RED|INVERSE,m_height,1,1);
  }
  else if ( evt_sent <= m_evtSent && evt_prod[0] == m_totSent ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No STORAGE activity visible.",BOLD|RED,m_height,1,1);
  }
  else if ( slots_min  ) {
    int nbad = int(bad_nodes.size());
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::sprintf(txt," SLOTS at limit:");
    if ( fslots[0] < SLOTS_MIN ) ::strcat(txt,"MEP ");
    if ( fslots[1] < SLOTS_MIN ) ::strcat(txt,"EVENT ");
    if ( fslots[2] < SLOTS_MIN ) ::strcat(txt,"RES/SEND ");
    if ( fslots[3] < SLOTS_MIN ) ::strcat(txt,"OVL ");
    ::sprintf(text,"[%d nodes]",nbad);
    ::strcat(txt,text);
    // We have 11 slow nodes in a farm: if these are full, this is no error
    ::scrc_put_chars(m_display,txt,INVERSE|(nbad>0 ? GREEN : RED),m_height,1,1);
  }
  else if ( space_min  ) {
    int nbad = int(bad_nodes.size());
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::sprintf(txt," SPACE at limit:");
    if ( fspace[0] < SPACE_MIN ) ::strcat(txt,"MEP ");
    if ( fspace[1] < SPACE_MIN ) ::strcat(txt,"EVENT ");
    if ( fspace[2] < SPACE_MIN ) ::strcat(txt,"RES/SEND ");
    if ( fspace[3] < SPACE_MIN ) ::strcat(txt,"OVL ");
    ::sprintf(text,"[%d nodes]",nbad);
    ::strcat(txt,text);
    // We have 11 slow nodes in a farm: if these are full, this is no error
    ::scrc_put_chars(m_display,txt,INVERSE|(nbad>0 ? GREEN : RED),m_height,1,1);
  }
  else if ( m_height > 3 )  {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," No obvious Errors detected....",NORMAL|INVERSE|GREEN,m_height,1,1);
    m_hasProblems = false;
  }
  else  {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|GREEN);
    m_hasProblems = false;
  }
  m_evtBuilt  = evt_built;
  m_evtMoore  = evt_moore;
  m_evtSent   = evt_sent;
  m_evtOvl    = evt_ovl;
  m_totBuilt  = evt_prod[0];
  m_totMoore  = evt_prod[1];
  m_totSent   = evt_prod[2];
  m_totOvl    = evt_prod[3];

  IocSensor::instance().send(m_parent,CMD_CHECK,this);
}
