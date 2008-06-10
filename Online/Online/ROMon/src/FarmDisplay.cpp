// $Id: FarmDisplay.cpp,v 1.1 2008-06-10 18:20:19 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/FarmDisplay.cpp,v 1.1 2008-06-10 18:20:19 frankb Exp $

#include "ROMon/FarmDisplay.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "SCR/scr.h"
extern "C" {
#include "dic.h"
}

// C++ include files
#include <set>
#include <limits>
#include <cstdlib>
#include <iostream>

using namespace ROMon;
using namespace SCR;

typedef Nodeset::Nodes               Nodes;
typedef Node::Buffers                Buffers;
typedef MBMBuffer::Clients           Clients;
typedef Node::Tasks                  Tasks;
#define BUILDER_TASK 'M'
#define SENDER_TASK  'D'
#define MOORE_TASK   'G'
#define MEP_BUFFER   'M'
#define RES_BUFFER   'R'
#define EVT_BUFFER   'E'
#define SND_BUFFER   'S'

#define SLOTS_MIN  .1
#define SPACE_MIN  .1
// Max. 15 seconds without update allowed
#define UPDATE_TIME_MAX 15

static void help() {
  std::cout <<"  romon_farm -option [-option]" << std::endl
	    <<"       -p[artition]=<name>          Partition name providing monitoring information." << std::endl
	    << std::endl;
}

FarmSubDisplay::FarmSubDisplay(Pasteboard* paste, const std::string& title, bool bad) 
: m_pasteboard(paste), m_title(title)
{
  m_evtSent  = m_totSent = 0;
  m_evtMoore = m_totMoore = 0;
  m_evtBuilt = m_totBuilt = 0;
  ::scrc_create_display(&m_display,4,48,NORMAL,ON,m_title.c_str());
  init(bad);
  std::string svc = "/";
  for(size_t i=0; i<title.length();++i) svc += ::tolower(title[i]);
  svc += "/ROpublish";
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
}

FarmSubDisplay::~FarmSubDisplay() {
  if ( m_svc != 0 ) {
    ::dic_release_service(m_svc);
    m_svc = 0;
  }
  ::scrc_delete_display(m_display);
  m_display = 0;
}

void FarmSubDisplay::init(bool bad) {
  int col = bad ? INVERSE|RED : NORMAL;
  char txt[128];
  ::sprintf(txt,"%-4s%9s %3s%8s %2s%4s%6s%4s",
	    "","MEP","Sl","EVENT","Cl","Sl","SEND","Sl");
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

void FarmSubDisplay::show(int row, int col) {
  ::scrc_paste_display (m_display, m_pasteboard, row, col);
}

void FarmSubDisplay::hide() {
  ::scrc_unpaste_display(m_display,m_pasteboard);
}

/// DIM command service callback
void FarmSubDisplay::dataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    FarmSubDisplay* disp = *(FarmSubDisplay**)tag;
    const Nodeset* ns = (Nodeset*)address;
    if ( ns->type == Nodeset::TYPE ) {
      dim_lock();
      //RTL::Lock lock(m_lock);
      disp->update(*ns);
      dim_unlock();
    }
  }
}

/// Update display content
void FarmSubDisplay::update(const Nodeset& ns) {
  char txt[128], text[128], title[128];
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

  std::set<std::string> bad_nodes;
  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const Buffers& buffs = *(*n).buffers();
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      int idx = 0;
      char b = (*ib).name[0];
      const MBMBuffer::Control& ctrl = (*ib).ctrl;
      inuse = true;
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
	      evt_built = std::min(evt_built,(*ic).events);
	    }
	    break;
	  case SENDER_TASK:
	    if( b == RES_BUFFER || b == SND_BUFFER )  {
	      evt_sent = std::min(evt_sent,(*ic).events);
	    }
	    break;
	  case MOORE_TASK:
	    if( b == EVT_BUFFER )  {
	      evt_moore = std::min(evt_moore,(*ic).events);
	    }
	    break;
	  default:
	    break;
	  }
	}
      }
    }
  }
  ::scrc_begin_pasteboard_update (m_pasteboard);
  char b1[64];
  Nodeset::TimeStamp frst=ns.firstUpdate();
  time_t t1 = frst.first;
  ::strftime(b1,sizeof(b1),"%H:%M:%S",::localtime(&t1));
  ::sprintf(title,"  %s %s  ",m_title.c_str(),b1);
  if ( time(0)-t1 > UPDATE_TIME_MAX ) {
    ::scrc_set_border(m_display,title,BOLD|INVERSE|RED);
    ::sprintf(txt," No update information available");
    ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,4,1,1);
  }
  else if ( fslots[0] < SLOTS_MIN || fslots[1] < SLOTS_MIN || fslots[2] < SLOTS_MIN ) {
    ::scrc_set_border(m_display,title,BOLD|INVERSE|RED);
    ::sprintf(txt," SLOTS at limit:");
    if ( fslots[0] < SLOTS_MIN ) ::strcat(txt,"MEP ");
    if ( fslots[1] < SLOTS_MIN ) ::strcat(txt,"EVENT ");
    if ( fslots[2] < SLOTS_MIN ) ::strcat(txt,"RES/SEND ");
    ::sprintf(text,"[%d nodes]",bad_nodes.size());
    ::strcat(txt,text);
    ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,4,1,1);
  }
  else if ( fspace[0] < SPACE_MIN || fspace[1] < SPACE_MIN || fspace[2] < SPACE_MIN  ) {
    ::scrc_set_border(m_display,title,BOLD|INVERSE|RED);
    ::sprintf(txt," SPACE at limit:");
    if ( fspace[0] < SPACE_MIN ) ::strcat(txt,"MEP ");
    if ( fspace[1] < SPACE_MIN ) ::strcat(txt,"EVENT ");
    if ( fspace[2] < SPACE_MIN ) ::strcat(txt,"RES/SEND ");
    ::sprintf(text,"[%d nodes]",bad_nodes.size());
    ::strcat(txt,text);
    ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,4,1,1);
  }
  else if ( evt_built <= m_evtBuilt && evt_prod[0] > m_totBuilt ) {
    ::scrc_set_border(m_display,title,BOLD|INVERSE|RED);
    ::scrc_put_chars(m_display," Some MEPRx(s) stuck.",BOLD|RED|INVERSE,4,1,1);
  }
  else if ( inuse && evt_built <= m_evtBuilt && evt_prod[0] == m_totBuilt ) {
    ::scrc_set_border(m_display,title,BOLD);
    ::scrc_put_chars(m_display," No DAQ activity visible.",BOLD|RED,4,1,1);
  }
  else if ( evt_moore <= m_evtMoore && evt_prod[1] > m_totMoore ) {
    ::scrc_set_border(m_display,title,BOLD|INVERSE|RED);
    ::scrc_put_chars(m_display," Some MOORE(s) stuck.",BOLD|RED|INVERSE,4,1,1);
  }
  else if ( inuse && evt_moore <= m_evtMoore && evt_prod[1] == m_totMoore ) {
    ::scrc_set_border(m_display,title,BOLD);
    ::scrc_put_chars(m_display," No HLT activity visible.",BOLD|RED,4,1,1);
  }
  else if ( evt_sent <= m_evtSent && evt_prod[2] > m_totSent ) {
    ::scrc_set_border(m_display,title,BOLD|INVERSE|RED);
    ::scrc_put_chars(m_display," Some Sender(s) stuck.",BOLD|RED|INVERSE,4,1,1);
  }
  else if ( inuse && evt_sent <= m_evtSent && evt_prod[0] == m_totSent ) {
    ::scrc_set_border(m_display,title,BOLD);
    ::scrc_put_chars(m_display," No STORAGE activity visible.",BOLD|RED,4,1,1);
  }
  else {
    ::scrc_put_chars(m_display,"",NORMAL,4,1,1);
    ::scrc_put_chars(m_display,"No Errors",NORMAL,4,20,1);
    ::scrc_set_border(m_display,title,BOLD);
  }
  m_evtBuilt = evt_built;
  m_evtMoore = evt_moore;
  m_evtSent  = evt_sent;
  m_totBuilt = evt_prod[0];
  m_totMoore = evt_prod[1];
  m_totSent  = evt_prod[2];
  if ( evt_prod[0] != 0 )
    ::sprintf(txt,"%9d%4d%8d %2d%4d%6d%4d",
	      evt_prod[0],used_slots[0],
	      evt_prod[1],buf_clients[1],used_slots[1],
	      evt_prod[2],used_slots[2]);
  else
    ::sprintf(txt,"%9s%4s%8s  %5s%6s%4s","--","--","--","--","--","--");
  ::scrc_put_chars(m_display,txt,NORMAL,2,5,1);
  if ( min_prod[0] != std::numeric_limits<int>::max() )
    ::sprintf(txt,"%9d%4d%8d   %4d%6d%4d",
	      min_prod[0],min_slots[0],
	      min_prod[1],min_slots[1],
	      min_prod[2],min_slots[2]);
  else
    ::sprintf(txt,"%9s%4s%8s  %5s%6s%4s","--","--","--","--","--","--");
  ::scrc_put_chars(m_display,txt,NORMAL,3,5,1);

  ::scrc_cursor_off(m_pasteboard);
  ::scrc_end_pasteboard_update (m_pasteboard);
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
  std::auto_ptr<std::vector<std::string> > f(new std::vector<std::string>());
  PartitionListener* h = *(PartitionListener**)tag;
  for(const char* data = (char*)address, *end=data+*size;data<end;data += strlen(data)+1)
    f->push_back(data);
  f->push_back("HLTA08");
  IocSensor::instance().send(h->m_parent,CMD_CONNECT,f.release());
}

/// Standard constructor
FarmDisplay::FarmDisplay(int argc, char** argv)
  : m_pasteboard(0), m_display(0), m_listener(0), m_info(0)
{
  bool all = false;
  char txt[128];
  RTL::CLI cli(argc,argv,help);
  cli.getopt("partition",   1, m_partition = "LHCb");
  all = 0 != cli.getopt("all",1);
  if ( all ) 
    ::sprintf(txt," Farm display of all known subfarms ");
  else
    ::sprintf(txt," Farm display of partition %s ",m_partition.c_str());
  ::scrc_create_pasteboard (&m_pasteboard, 0, &m_height, &m_width);
  m_width -= 2;
  m_height -= 2;
  ::scrc_create_display (&m_display, m_height, m_width, BOLD, ON, txt);
  ::scrc_paste_display (m_display, m_pasteboard, 2, 2);
  ::scrc_put_chars(m_display,txt,NORMAL,1,2,1);
  ::scrc_end_pasteboard_update (m_pasteboard);
  ::scrc_fflush(m_pasteboard);
  if ( all ) {
    m_info = ::dic_info_service("DIS_DNS/SERVER_LIST",MONITORED,0,0,0,infoHandler,(long)this,0,0);
  }
  else {
    m_listener = new PartitionListener(this,m_partition);
  }
}

/// Standard destructor
FarmDisplay::~FarmDisplay()  {
  if ( m_info ) {
    ::dic_release_service(m_info);
    m_info = 0;
  }
  if ( m_listener ) {
    delete m_listener;
    m_listener = 0;
  }
}
/// Extract node/service name from DNS info
static void getServiceNode(char* s, std::string& svc, std::string& node) {
  char* at = strchr(s,'@');
  *at = 0;
  svc = s;
  node = at+1;
}

/// DIM command service callback
void FarmDisplay::infoHandler(void* tag, void* address, int* size) {
  if ( address && size && *size>0 ) {
    FarmDisplay* h = *(FarmDisplay**)tag;
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
	IocSensor::instance().send(h,CMD_ADD,new std::string(f));
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
	  IocSensor::instance().send(h,CMD_CONNECT,farms.release());
      }
      break;
    }
  }
}

/// Interactor overload: Display callback handler
void FarmDisplay::handle(const Event& ev) {
  // IocSensor& ioc = IocSensor::instance();
  Farms::iterator i;
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
    case CMD_ADD:
      i = std::find(m_farms.begin(),m_farms.end(),*(std::string*)ev.data);
      if ( i == m_farms.end() ) {
	m_farms.push_back(*(std::string*)ev.data);
	connect(m_farms);
      }
      delete (std::string*)ev.data;
      return;
    case CMD_CONNECT:
      m_farms = *(std::vector<std::string>*)ev.data;
      connect(m_farms);
      delete (std::vector<std::string>*)ev.data;
      return;
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
  ::scrc_begin_pasteboard_update (m_pasteboard);
  ::sprintf(txt,"Total number of subfarms:%d",farms.size());
  ::scrc_put_chars(m_display,txt,NORMAL,2,2,1);

  for( k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k)
    (*k).second->hide();
  for (Farms::const_iterator i=farms.begin(); i != farms.end(); ++i) {
    k = m_farmDisplays.find(*i);
    if ( k == m_farmDisplays.end() ) {
      copy.insert(std::make_pair(*i,new FarmSubDisplay(m_pasteboard,*i)));
    }
    else {
      copy.insert(*k);
      m_farmDisplays.erase(k);
    }
  }
  for (k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k)
    delete (*k).second;
  m_farmDisplays = copy;

  int subdisp_height = 4;
  int subdisp_width  = 48;
  int total_height   = 5;
  int total_width    = 3;
  for (k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k) {
    int h = total_height+subdisp_height+1;
    int w = total_width+subdisp_width+1;
    if ( h > m_height+2 ) {
      total_height = 5;
      total_width  = w+1;
      if ( w+subdisp_width+1 > m_width+2 ) break;
    }
    (*k).second->show(total_height, total_width);
    total_height += subdisp_height+1;
  }
  ::scrc_end_pasteboard_update (m_pasteboard);
}

/// Update header information
void FarmDisplay::showHeader(const Nodeset& ns)   {
  char b1[64], b2[64], txt[256];
  Nodeset::TimeStamp frst=ns.firstUpdate(), last=ns.lastUpdate();
  time_t t1 = frst.first, t2 = last.first;
  ::strftime(b1,sizeof(b1),"%H:%M:%S",::localtime(&t1));
  ::strftime(b2,sizeof(b1),"%H:%M:%S",::localtime(&t2));
  ::sprintf(txt,"         HLT monitoring on %s   [%s]", ns.name, ::lib_rtl_timestr());    
  ::scrc_put_chars(m_display,txt,BOLD,1,20,1);
}

extern "C" int romon_farm(int argc,char** argv) {
  FarmDisplay disp(argc,argv);
  IocSensor::instance().run();
  return 1;
}
