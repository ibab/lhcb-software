// $Id: FarmDisplay.cpp,v 1.51 2010-10-21 06:04:22 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/FarmDisplay.cpp,v 1.51 2010-10-21 06:04:22 frankb Exp $

// Framework include files
#include "ROMon/ClusterDisplay.h"
#include "ROMon/FarmDisplay.h"
#include "SCR/MouseSensor.h"
#include "CPP/TimeSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/Lock.h"
#include "RTL/strdef.h"
#include "SCR/scr.h"
#include "WT/wtdef.h"
#include "ROMonDefs.h"
extern "C" {
#include "dic.h"
}

// C++ include files
#include <set>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <algorithm>

using namespace ROMon;
using namespace SCR;
using namespace std;

typedef Nodeset::Nodes               Nodes;
typedef Node::Buffers                Buffers;
typedef MBMBuffer::Clients           Clients;
typedef Node::Tasks                  Tasks;
typedef vector<string>               StringV;

// Max. 15 seconds without update allowed
#define SUBFARMDISPLAY_OFFSET 8

static FarmDisplay* s_fd = 0;

static void help() {
  cout << "  romon_farm -option [-option]" << endl
       << "       -all                         Show all subfarms." << endl
       << "       -p[artition]=<name>          Partition name providing monitoring information." << endl
       << "       -an[chor]=+<x-pos>+<ypos>    Set anchor for sub displays" << endl
       << endl;
}

namespace ROMon {
  InternalDisplay* createFarmSubDisplay(InternalDisplay* parent,
                                        const string& title, 
                                        const string& partition,
                                        int height);
  InternalDisplay* createRecFarmSubDisplay(InternalDisplay* parent, const string& title, int height);
  InternalDisplay* createCtrlFarmSubDisplay(InternalDisplay* parent, const string& title, int height);
  InternalDisplay* createMonitoringSubDisplay(InternalDisplay* parent, const string& title, int height);
  InternalDisplay* createStorageSubDisplay(InternalDisplay* parent, const string& title, int height);
}


namespace {
#if 0
  ScrDisplay* swapMouseSelector(Interactor* ia, ClusterDisplay* from, ClusterDisplay* to) {
    if ( from ) {
      MouseSensor::instance().remove(from->display());
      ScrDisplay* d = dynamic_cast<ScrDisplay*>(from->nodeDisplay());
      if ( d ) MouseSensor::instance().remove(d->display());
    }
    if ( to ) {
      ScrDisplay* d = dynamic_cast<ScrDisplay*>(to->nodeDisplay());
      MouseSensor::instance().add(ia,to->display());
      if ( d )  MouseSensor::instance().add(ia,d->display());
      return d;
    }
    return 0;
  }
#endif

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

/// Standard constructor
FarmDisplay::FarmDisplay(int argc, char** argv)
  : FarmDisplayBase()
{
  char txt[128];
  string anchor, prefix;
  RTL::CLI cli(argc,argv,help);
  bool all = 0 != cli.getopt("all",2);
  bool xml = 0 != cli.getopt("xml",2);
  m_subDisplayHeight = 3;
  cli.getopt("partition",   2, m_name = "ALL");
  cli.getopt("match",       2, m_match = "*");
  cli.getopt("prefix",      2, prefix);
  cli.getopt("sdh",         2, m_subDisplayHeight);
  cli.getopt("node-height", 7, m_subDisplayHeight);

  m_dense   = 0 != cli.getopt("dense",2);
  m_reverse = 0 != cli.getopt("reverse",3);
  m_mode  = cli.getopt("reconstruction",3) == 0 ? HLT_MODE : RECO_MODE;
  if ( cli.getopt("taskmonitor",2) != 0 ) m_mode = CTRL_MODE;
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
  if ( m_name!="*" && m_name!="ALL" ) m_partition = m_name;
  if ( !prefix.empty() ) InternalDisplay::setSvcPrefix(prefix);
  if ( m_reverse       ) InternalDisplay::setCreateFlags(INVERSE);
  s_fd = this;
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
  ::scrc_create_display(&m_display,m_height,m_width,NORMAL|(m_reverse?INVERSE:NORMAL),ON,m_title.c_str());
  show(2,2);
  if ( m_mode == CTRL_MODE ) {
    ::scrc_put_chars(m_display,txt,NORMAL|BOLD,1,2,0);
    ::scrc_put_chars(m_display,"<CTRL-H for Help>, <CTRL-E to exit>",NORMAL|BOLD,1,40,0);
    ::scrc_put_chars(m_display,"nn",GREEN|INVERSE,1,80,0);
    ::scrc_put_chars(m_display,": OK",NORMAL,1,82,0);
    ::scrc_put_chars(m_display,"nn",RED|INVERSE,1,90,0);
    ::scrc_put_chars(m_display,": Not OK",NORMAL,1,92,0);
    ::scrc_put_chars(m_display,"nn",BLUE|INVERSE,1,110,0);
    ::scrc_put_chars(m_display,": OK/Excluded",NORMAL,1,112,0);
    ::scrc_put_chars(m_display,"nn",MAGENTA|INVERSE,1,130,0);
    ::scrc_put_chars(m_display,": Not OK/Excluded",NORMAL,1,132,1);
  }
  else if ( m_mode == TORRENT_MODE ) {
    ::scrc_put_chars(m_display,txt,NORMAL|BOLD,1,2,0);
    ::scrc_put_chars(m_display,"<CTRL-H for Help>, <CTRL-E to exit>",NORMAL|BOLD,2,40,0);
    ::scrc_put_chars(m_display," nn ",GREEN|INVERSE,1,80,0);
    ::scrc_put_chars(m_display,": OK, TorrentInfo present",NORMAL,1,85,0);
    ::scrc_put_chars(m_display," nn ",BLUE|INVERSE,2,80,0);
    ::scrc_put_chars(m_display,": Excluded from HLT",NORMAL,2,85,0);
    ::scrc_put_chars(m_display," nn ",RED|INVERSE|BOLD,1,120,0);
    ::scrc_put_chars(m_display,": Not OK, No torrent info",NORMAL,1,125,0);
    ::scrc_put_chars(m_display," nn ",BG_BLACK|FG_YELLOW|BOLD,2,120,0);
    ::scrc_put_chars(m_display,": Executing",NORMAL,2,125,0);
  }
  ::scrc_end_pasteboard_update (m_pasteboard);
  ::scrc_fflush(m_pasteboard);
  ::scrc_set_cursor(m_display, 2, 10);
  ::scrc_cursor_off(m_pasteboard);
  ::wtc_remove(WT_FACILITY_SCR);
  ::wtc_subscribe(WT_FACILITY_SCR, key_rearm, key_action, m_pasteboard);
  MouseSensor::instance().start(pasteboard());
  if ( xml )
    m_listener = auto_ptr<PartitionListener>(new PartitionListener(this,m_name,m_match,xml));
  else
    m_listener = auto_ptr<PartitionListener>(new PartitionListener(this,m_name,m_match));
}

/// Standard destructor
FarmDisplay::~FarmDisplay()  {  
  MouseSensor::instance().stop();
  ::wtc_remove(WT_FACILITY_SCR);
  disconnect();
  m_listener = auto_ptr<PartitionListener>(0);
  ::scrc_begin_pasteboard_update(m_pasteboard);
  m_ctrlDisplay = auto_ptr<CtrlNodeDisplay>(0);
  m_mbmDisplay = auto_ptr<BufferDisplay>(0);
  if ( m_nodeSelector ) {
    MouseSensor::instance().remove(m_nodeSelector->display());
    m_nodeSelector = 0;
  }
  if ( m_subfarmDisplay ) {
    MouseSensor::instance().remove(m_subfarmDisplay->display());
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

/// Get the name of the currently selected cluster
string FarmDisplay::selectedCluster() const {
  if ( m_sysDisplay.get() )
    return m_sysDisplay->clusterName();
  else if ( m_subfarmDisplay )
    return m_subfarmDisplay->clusterName();
  else if ( currentDisplay() )
    return currentDisplay()->name();
  return "";
}

/// Get the name of the currently selected cluster and node
pair<string,string> FarmDisplay::selectedNode() const {
  string node_name, cl = selectedCluster();
  if ( !cl.empty() ) {
    if ( m_sysDisplay.get() )
      node_name = m_sysDisplay->nodeName(m_subPosCursor);
    else if ( m_subfarmDisplay )
      node_name = m_subfarmDisplay->nodeName(m_subPosCursor);
  }
  return make_pair(cl,node_name);
}

/// Number of sub-nodes in a cluster
size_t FarmDisplay::selectedClusterSize() const {
  if ( m_sysDisplay.get() )
    return m_sysDisplay->numNodes();
  else if ( m_roDisplay.get() )
    return m_roDisplay->numNodes();
  else if ( m_torrentDisplay.get() )
    return m_torrentDisplay->numNodes();
  else if ( m_subfarmDisplay )
    return m_subfarmDisplay->numNodes();
  return 0;
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
  RTL::Lock lock(screenLock());
  return s_fd->handleKeyboard(key);
}

/// Set cursor to position
void FarmDisplay::set_cursor(InternalDisplay* /* updater */) {
  set_cursor();
}

/// Set cursor to position
void FarmDisplay::set_cursor() {
  if ( 0 != m_sysDisplay.get() ) {
    Display* d1 = m_sysDisplay->display();
    if ( d1 ) ::scrc_set_cursor(d1, m_subPosCursor+SUBFARMDISPLAY_OFFSET, 2);
  }
  else if ( 0 != m_roDisplay.get() ) {
    Display* d1 = m_roDisplay->display();
    if ( d1 ) ::scrc_set_cursor(d1, m_subPosCursor+SUBFARMDISPLAY_OFFSET, 2);
  }
  else if ( 0 != m_torrentDisplay.get() ) {
    Display* d1 = m_torrentDisplay->display();
    if ( d1 ) ::scrc_set_cursor(d1, m_subPosCursor+SUBFARMDISPLAY_OFFSET, 2);
  }
  else if ( 0 != m_subfarmDisplay ) {
    Display* d1 = m_subfarmDisplay->display();
    if ( d1 ) ::scrc_set_cursor(d1, m_subPosCursor+SUBFARMDISPLAY_OFFSET, 2);
  }
  else {
    InternalDisplay* d = currentDisplay();
    if ( d ) ::scrc_set_cursor(d->display(),2, 2);
  }
}

/// Get farm display from cursor position
InternalDisplay* FarmDisplay::currentDisplay()  const {
  size_t cnt;
  SubDisplays::const_iterator k;
  for(k=m_farmDisplays.begin(), cnt=0; k != m_farmDisplays.end(); ++k, ++cnt) {
    if ( cnt == m_posCursor ) {
      return (*k).second;
    }
  }
  return 0;
}

/// Get farm display name from cursor position
string FarmDisplay::currentDisplayName()  const {
  InternalDisplay* d = currentDisplay();
  return d ? d->name() : string("");
}

/// Get farm <partition>/<display name> from cursor position
std::string FarmDisplay::currentCluster()  const {
  InternalDisplay* d = currentDisplay();
  if ( d ) return m_name +"/" + d->name();
  return "";
}

/// Handle keyboard interrupts
int FarmDisplay::handleKeyboard(int key)    {
  if ( FarmDisplayBase::handleKeyboard(key) == WT_SUCCESS ) {
    return WT_SUCCESS;
  }
  int cnt, col, row;
  InternalDisplay* d = 0;
  Display* d1;
  SubDisplays& sd = subDisplays();
  try {
    switch (key)    {
    case MOVE_UP:
      if( 0 == m_nodeSelector && m_posCursor > 0 )
        --m_posCursor;
      else if( m_nodeSelector && m_subPosCursor > 0 )
        --m_subPosCursor;
      break;
    case MOVE_DOWN:
      if( 0 == m_nodeSelector && m_posCursor < subDisplays().size()-1 )
        ++m_posCursor;
      else if( m_nodeSelector && selectedClusterSize() > m_subPosCursor )
        ++m_subPosCursor;
      break;
    case MOVE_LEFT:
      if( 0 == m_subfarmDisplay && 0 == m_sysDisplay.get() && (d=currentDisplay()) ) {
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
      if( 0 == m_subfarmDisplay && 0 == m_sysDisplay.get() && (d=currentDisplay()) ) {
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
    case PAGE_UP:
      if ( 0 == m_nodeSelector && (d=currentDisplay()) ) {
        SubDisplays::reverse_iterator k;
        col = d ? d->display()->col : 2;
        row = d ? d->display()->row : 2;
        for(k=sd.rbegin(), cnt=sd.size(); k != sd.rend(); ++k, --cnt) {
          d1 = (*k).second->display();
          if ( d1->col == col ) {
            if ( d1->row == min(d1->row,row) ) {
              row = d1->row;
              m_posCursor = cnt;
              return WT_SUCCESS;
            }
          }
        }
      }
      break;
    case PAGE_DOWN:
      if ( 0 == m_nodeSelector && (d=currentDisplay()) ) {
        SubDisplays::iterator k;
        col = d ? d->display()->col : 2;
        row = d ? d->display()->row : 0;
        for(k=sd.begin(), cnt=0; k != sd.end(); ++k, ++cnt) {
          d1 = (*k).second->display();
          if ( d1->col == col ) {
            if ( d1->row == max(d1->row,row) ) {
              row = d1->row;
              m_posCursor = cnt;
              return WT_SUCCESS;
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
  InternalDisplay* d = 0;
  Farms::iterator i;
  SubDisplays::iterator k;
  const MouseEvent* m = 0;

  RTL::Lock lock(screenLock());
  switch(ev.eventtype) {
  case ScrMouseEvent:
    m = ev.get<MouseEvent>();
    if ( handleMouseEvent(m) ) {
    }
    else if ( m_nodeSelector && m->msec == (unsigned int)-1 ) {
      Display* disp = m_nodeSelector->display();
      if ( m->display == disp ) {
        size_t pos = m->y - disp->row - 2;
        if ( selectedClusterSize()>pos ) {
          int cmd = m->button==0 ? m_sysDisplay.get() ? CMD_SHOWCTRL : CMD_SHOWMBM : CMD_SHOWPROCS;
          m_subPosCursor = pos;
          IocSensor::instance().send(this,cmd,this);
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
    if ( handleIocEvent(ev) ) {
      return;
    }
    switch(ev.type) {
    case CMD_SHOW:
      for(k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k, ++cnt) {
        if ( (d=(*k).second) == ev.data )  {
          m_posCursor = cnt;
          IocSensor::instance().send(this,CMD_SHOWSUBFARM,this);
          return;
        }
      }
      break;
    case CMD_POSCURSOR:
      for(k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k, ++cnt) {
        if ( (d=(*k).second) == ev.data )  {
          m_posCursor = cnt;
          set_cursor();
          return;
        }
      }
      break;
    case CMD_UPDATE:
      if ( m_subfarmDisplay )   {
        IocSensor::instance().send(m_subfarmDisplay,  ROMonDisplay::CMD_UPDATEDISPLAY,this);
      }
      if ( m_sysDisplay.get() )   {
        IocSensor::instance().send(m_sysDisplay.get(),ROMonDisplay::CMD_UPDATEDISPLAY,this);
      }
      if ( m_roDisplay.get() )   {
        IocSensor::instance().send(m_roDisplay.get(),ROMonDisplay::CMD_UPDATEDISPLAY,this);
      }
      if ( m_torrentDisplay.get() )   {
        IocSensor::instance().send(m_torrentDisplay.get(),ROMonDisplay::CMD_UPDATEDISPLAY,this);
      }
      if ( m_mbmDisplay.get() )  {
        const void* data = m_subfarmDisplay->data().pointer;
        m_mbmDisplay->setNode(m_subPosCursor);
        m_mbmDisplay->update(data);
      }
      else if ( m_mode == CTRL_MODE && m_ctrlDisplay.get() ) {
        const void* data = m_subfarmDisplay->data().pointer;
        m_ctrlDisplay->setNode(m_subPosCursor);
        m_ctrlDisplay->update(data);
      }
      TimeSensor::instance().add(this,1,m_subfarmDisplay);
      break;
    case CMD_ADD: {
      StringV farms;
      for(k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k) {
        if ( (*k).second->name() == *ev.iocPtr<string>() ) {
          delete ev.iocPtr<string>();
          return;
        }
        farms.push_back((*k).first);
      }
      farms.push_back(*ev.iocPtr<string>());
      connect(m_name,farms);
      delete ev.iocPtr<string>();
      return;
    }
    case CMD_CONNECT: {
      StringV farms;
      farms.clear();
      farms.assign(ev.iocPtr<StringV>()->begin()+1,ev.iocPtr<StringV>()->end());
      connect(*ev.iocPtr<StringV>()->begin(),farms);
      delete ev.iocPtr<vector<string> >();
      return;
    }
    case CMD_CHECK: {
      DisplayUpdate update(this,true);
      for(k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k)
        if ( (d=(*k).second) != ev.data ) d->check(now);
      if ( m_sysDisplay.get() ) m_sysDisplay->update();
      break;
    }
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void FarmDisplay::connect(const std::string& /* section */, const vector<string>& farms) {
  SubDisplays::iterator k;
  SubDisplays copy;
  char txt[128];
  DisplayUpdate update(this,false);

  ::sprintf(txt,"Total number of subfarms:%d    ",int(farms.size()));
  ::scrc_put_chars(m_display,txt,NORMAL|BOLD,1,2,0);

  for( k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k)
    (*k).second->hide();
  for (Farms::const_iterator i=farms.begin(); i != farms.end(); ++i) {
    k = m_farmDisplays.find(*i);
    if ( k == m_farmDisplays.end() ) {
      if ( m_mode == RECO_MODE )
        copy.insert(make_pair(*i,createRecFarmSubDisplay(this,*i, m_subDisplayHeight)));
      else if ( m_mode == CTRL_MODE )
        copy.insert(make_pair(*i,createCtrlFarmSubDisplay(this,*i, m_subDisplayHeight)));
      else if ( ::strncasecmp((*i).c_str(),"mona0",5)==0 )
        copy.insert(make_pair(*i,createMonitoringSubDisplay(this,*i, m_subDisplayHeight)));
      else if ( ::strncasecmp((*i).c_str(),"storectl",8)==0 )
        copy.insert(make_pair(*i,createStorageSubDisplay(this,*i, m_subDisplayHeight)));
      else
        copy.insert(make_pair(*i,createFarmSubDisplay(this,*i,m_partition,m_subDisplayHeight)));
    }
    else {
      copy.insert(*k);
      m_farmDisplays.erase(k);
    }
  }
  for (k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k)
    delete (*k).second;
  m_farmDisplays = copy;

  int subdisp_height = m_subDisplayHeight + (m_dense ? 0 : 1);
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
  return len;
}

extern "C" int romon_farm(int argc,char** argv) {
  FarmDisplay* disp = new FarmDisplay(argc,argv);
  ::lib_rtl_install_printer(do_output,0);
  IocSensor::instance().run();
  delete disp;
  return 1;
}
