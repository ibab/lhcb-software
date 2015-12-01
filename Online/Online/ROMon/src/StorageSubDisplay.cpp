#include "ROMon/FarmDisplay.h"

namespace ROMon {

  /**@class StorageSubDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display summarizing one single subfarm. Showed as an array on the
   *   main display.
   *
   *   @author M.Frank
   */
  class StorageSubDisplay : public InternalDisplay  {
    int               m_evtRecv;
    int               m_evtSent;
    int               m_totRecv;
    int               m_totSent;
    int               m_numUpdate;
    int               m_height;
    bool              m_hasProblems;
    bool              m_inUse;
    std::string       m_partition;
    std::string       m_relayNode;

  public:
    /// Initializing constructor
    StorageSubDisplay(InternalDisplay* parent, const std::string& title, int height, bool bad=false);
    /// Standard destructor
    virtual ~StorageSubDisplay();
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

  InternalDisplay* createStorageSubDisplay(InternalDisplay* parent, const std::string& title, int height) {
    return new StorageSubDisplay(parent,title,height);
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
StorageSubDisplay::StorageSubDisplay(InternalDisplay* parent, const string& title, int height, bool bad) 
  : InternalDisplay(parent, title)
{
  m_numUpdate = 0;
  m_height  = height;
  m_evtSent = m_totSent = 0;
  m_evtRecv = m_totRecv = 0;
  m_partition = parent->name();
  m_relayNode = "storerecv";
  m_lastUpdate = time(0);
  ::scrc_create_display(&m_display,m_height,48,NORMAL,ON,m_title.c_str());
  init(bad);
  string svc = svcPrefix()+strlower(title)+"/ROpublish";
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
  m_hasProblems = false;
  MouseSensor::instance().add(this,m_display);
}

/// Standard destructor
StorageSubDisplay::~StorageSubDisplay() {
  MouseSensor::instance().remove(m_display);
}

/// Initialize default display text
void StorageSubDisplay::init(bool bad) {
  int col = bad ? INVERSE|RED : NORMAL;
  char txt[128];
  ::sprintf(txt,"%-13s%9s%4s%5s%9s%4s%4s",
            "","Events","Cl","Sl","","","");
  ::scrc_put_chars(m_display,txt,col|INVERSE,1,1,1);
  ::scrc_put_chars(m_display," ",col,2,1,1);
  ::scrc_put_chars(m_display," ",col,3,1,1);
  ::scrc_put_chars(m_display," ",col,m_height,1,1);
  ::scrc_put_chars(m_display,"Recv Layer:",BOLD,2,1,1);
  ::scrc_put_chars(m_display,"Stream Layer:",BOLD,3,1,1);
  ::scrc_put_chars(m_display,"  ",col,2,46,0);
  ::scrc_put_chars(m_display,"  ",col,3,45,0);
  ::scrc_put_chars(m_display,"  ",col,m_height,44,0);
  ::scrc_set_border(m_display,m_title.c_str(),col|BOLD);
}

/// DIM command service callback
void StorageSubDisplay::update(const void* address) {
  const Nodeset* ns = (const Nodeset*)address;
  if ( ns->type == Nodeset::TYPE ) {
    updateContent(*ns);
  }
}

/// Set timeout error
void StorageSubDisplay::setTimeoutError() {
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
  ::scrc_put_chars(m_display,"No update information available",BOLD|RED|INVERSE,m_height,1,1);
}

/// Check display for errors
void StorageSubDisplay::check(time_t now) {
  if ( hasProblems() ) {
    if ( now - lastUpdate() > UPDATE_TIME_MAX ) {
      setTimeoutError();
    }
  }
}

/// Set the focus to this display
void StorageSubDisplay::setFocus() {
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
}

/// Release the focus of this display
void StorageSubDisplay::releaseFocus() {
}

/// Interactor overload: Display callback handler
void StorageSubDisplay::handle(const CPP::Event& ev) {
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
void StorageSubDisplay::updateContent(const Nodeset& ns) {
  typedef Node::Buffers                Buffers;
  typedef Nodeset::Nodes               Nodes;
  char txt[128], text[128];
  bool inuse = false;
  set<string> bad_nodes;
  static const int   INT_max = numeric_limits<int>::max();
  static const float FLT_max = numeric_limits<float>::max();

  string evt_buff = std::string("Events_"+m_partition);
  int numNodes = 0, numBuffs = 0, numClients = 0;
  float fsp, fsl, fspace[3] = {FLT_max,FLT_max,FLT_max}, fslots[3] = {FLT_max,FLT_max,FLT_max};
  long long int tot_prod[3] = {0,0,0}, min_prod[3] = {INT_max,INT_max,INT_max};
  int   num_cl[3] = {0,0,0}, num_sl[3] = {0,0,0};

  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    bool recv_node = ::strncasecmp((*n).name,"storerecv",8) == 0;
    const Buffers& buffs = *(*n).buffers();
    ++numNodes;
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      const char* bnam = (*ib).name;
      if ( ::strstr(bnam,m_partition.c_str()) != 0 ) {
        int idx = (recv_node && evt_buff == bnam) ? 0 : (evt_buff == bnam) ? 1 : -1;
        if ( idx >= 0 )   {
          const MBMBuffer::Control& ctrl = (*ib).ctrl;
          ++numBuffs;
          tot_prod[idx] += ctrl.tot_produced;
          numClients    += (*ib).clients.size();
          num_cl[idx]   += (*ib).clients.size();
          num_sl[idx]   += ctrl.p_emax - ctrl.i_events;
          min_prod[idx]  = min(min_prod[idx],ctrl.tot_produced);
          fsp = float(ctrl.i_space)/float(ctrl.bm_size);
          fsl = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
          fspace[idx]    = min(fspace[idx],fsp);
          fslots[idx]    = min(fslots[idx],fsl);
          if ( fsl < SLOTS_MIN || fsp < SPACE_MIN ) bad_nodes.insert((*n).name);
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

  if ( tot_prod[0] != 0 )
    ::sprintf(txt,"%9lld%4d%5d%17s",tot_prod[0],num_cl[0],num_sl[0],"");
  else
    ::sprintf(txt,"%9s%4s%5s%17s","--","--","--","");
  ::scrc_put_chars(m_display,"Recv Layer:",BOLD,2,1,1);
  ::scrc_put_chars(m_display,txt,NORMAL,2,14,1);

  if ( tot_prod[1] != 0 )
    ::sprintf(txt,"%9lld%4d%5d%17s",tot_prod[1],num_cl[1],num_sl[1],"");
  else
    ::sprintf(txt,"%9s%4s%5s%17s","--","--","--","");
  ::scrc_put_chars(m_display,"Stream Layer:",BOLD,3,1,1);
  ::scrc_put_chars(m_display,txt,NORMAL,3,14,1);

  m_hasProblems = true;
  if ( now-m_lastUpdate > UPDATE_TIME_MAX ) {
    setTimeoutError();
  }
  else if ( numNodes == 0 ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::scrc_put_chars(m_display," No nodes found!",BOLD|RED|INVERSE,m_height,1,1);
  }
  else if ( !inuse ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display," Storage not used yet....",NORMAL|INVERSE|GREEN,m_height,1,1);
  }
  else if ( fslots[0] < SLOTS_MIN || fslots[1] < SLOTS_MIN ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::sprintf(txt,"SLOTS at limit:");
    if ( fslots[0] < SLOTS_MIN ) ::strcat(txt,"Recv ");
    if ( fslots[1] < SLOTS_MIN ) ::strcat(txt,"Stream ");
    ::sprintf(text,"[%d nodes]",int(bad_nodes.size()));
    ::strcat(txt,text);
    ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,m_height,1,1);
  }
  else if ( fspace[0] < SPACE_MIN || fspace[1] < SPACE_MIN ) {
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED);
    ::sprintf(txt,"SPACE at limit:");
    if ( fspace[0] < SPACE_MIN ) ::strcat(txt,"Recv ");
    if ( fspace[1] < SPACE_MIN ) ::strcat(txt,"Stream ");
    ::sprintf(text,"[%d nodes]",int(bad_nodes.size()));
    ::strcat(txt,text);
    ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,m_height,1,1);
  }
  else if ( min_prod[0] != INT_max && min_prod[0]>0 && min_prod[0] <= m_evtRecv ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display,"Receiving layer idle.",RED|BOLD,m_height,1,1);
  }
  else if ( min_prod[1] != INT_max && min_prod[1]>0 && min_prod[1] <= m_evtSent ) {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display,"Streaming layer idle.",RED|BOLD,m_height,1,1);
  }
  else if ( m_height>3 )   {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL);
    ::scrc_put_chars(m_display,"No obvious Errors detected....",NORMAL|INVERSE|GREEN,m_height,1,1);
  }
  else {
    ::scrc_set_border(m_display,m_title.c_str(),NORMAL|INVERSE|GREEN);
  }

  m_evtRecv = min_prod[0];
  m_evtSent = min_prod[1];
  m_totRecv = tot_prod[0];
  m_totSent = tot_prod[1];

  IocSensor::instance().send(m_parent,CMD_CHECK,this);
}
