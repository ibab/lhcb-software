// $Id: FarmDisplayBase.cpp,v 1.51 2010-10-21 06:04:22 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/FarmDisplayBase.cpp,v 1.51 2010-10-21 06:04:22 frankb Exp $

// Framework include files
#include "ROMon/FarmLineDisplay.h"
#include "ROMon/HltSummaryDisplay.h"
#include "ROMon/ClusterLine.h"
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
#include <algorithm>

using namespace ROMon;
using namespace SCR;
using namespace std;

#define CLUSTERLINE_FIRSTPOS   6
static FarmLineDisplay* s_fd = 0;

static void help() {
  cout << "  romon_farm -option [-option]" << endl
            << "       -all                         Show all subfarms." << endl
            << "       -p[artition]=<name>          Partition name providing monitoring information." << endl
            << "       -an[chor]=+<x-pos>+<ypos>    Set anchor for sub displays" << endl
            << endl;
}

namespace {
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
FarmLineDisplay::FarmLineDisplay(int argc, char** argv)
  : FarmDisplayBase(), m_currentLine(0)
{
  char txt[128];
  string anchor, prefix;
  RTL::CLI cli(argc,argv,help);
  bool all = 0 != cli.getopt("all",2);
  bool xml = 0 != cli.getopt("xml",2);
  cli.getopt("partition",   2, m_name = "ALL");
  cli.getopt("match",       2, m_match = "*");
  cli.getopt("prefix",      2, prefix);
  cli.getopt("sdh",         2, m_subDisplayHeight);
  cli.getopt("node-height", 7, m_subDisplayHeight);

  m_dense = 0 != cli.getopt("dense",3);
  m_reverse = 0 != cli.getopt("reverse",3);
  m_mode = HLT_MODE;
  if ( cli.getopt("reconstruction",3) ) m_mode = RECO_MODE;
  if ( cli.getopt("taskmonitor",3)    != 0 ) m_mode = CTRL_MODE;
  if ( cli.getopt("torrentmonitor",3) != 0 ) m_mode = TORRENT_MODE;
  if ( cli.getopt("deferred",3)       != 0 ) m_mode = HLTDEFER_MODE;
  if ( cli.getopt("anchor",3,anchor)  != 0 ) {
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
  if ( cli.getopt("debug",5) != 0 ) {
    ::printf("\n\ngdb --pid %d\n\n",::lib_rtl_pid());
    ::lib_rtl_sleep(10000);
  }

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
  else if ( m_mode == TORRENT_MODE )
    ::sprintf(txt," Torrent status display of partition %s ",m_name.c_str());
  else if ( m_mode == HLTDEFER_MODE )
    ::sprintf(txt," HLT work display of partition %s match='%s'",m_name.c_str(),m_match.c_str());
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
  ::scrc_create_display (&m_display, m_height, m_width,createFlags(), ON, m_title.c_str());
  show(2,2);
  ::scrc_begin_pasteboard_update (m_pasteboard);
  if ( m_mode == CTRL_MODE ) {
    ::scrc_put_chars(m_display,txt,NORMAL|BOLD,1,2,0);
    ::scrc_put_chars(m_display,"<CTRL-H for Help>, <CTRL-E to exit>",NORMAL|BOLD,2,40,0);
    ::scrc_put_chars(m_display,"nn",GREEN|INVERSE,1,80,0);
    ::scrc_put_chars(m_display,": OK",NORMAL,1,82,0);
    ::scrc_put_chars(m_display,"nn",RED|INVERSE|BOLD,1,90,0);
    ::scrc_put_chars(m_display,": Not OK",NORMAL,1,92,0);
    ::scrc_put_chars(m_display,"nn",BLUE|INVERSE,1,110,0);
    ::scrc_put_chars(m_display,": OK/Excluded",NORMAL,1,112,0);
    ::scrc_put_chars(m_display,"nn",MAGENTA|INVERSE,1,130,0);
    ::scrc_put_chars(m_display,": Not OK/Excluded",NORMAL,1,132,1);
    ::sprintf(txt," %-10s %-8s %-6s %-6s %-6s    %s",
	      "","Last","No.of","No.of","No.of","< -------- Subfarm Information ------- >");
    ::scrc_put_chars(m_display,txt,BG_BLUE|FG_WHITE|BOLD,CLUSTERLINE_FIRSTPOS-2,1,1);
    ::sprintf(txt," %-10s %-8s %-6s %-6s %-6s   %7s %5s  %-19s           %s",
	      "","Update","Nodes", "Tasks","Conn.","Status","PVSS","Summary",
	      "Controls PC and worker node status");
    ::scrc_put_chars(m_display,txt,BG_BLUE|FG_WHITE|BOLD,CLUSTERLINE_FIRSTPOS-1,1,1);
  }
  else if ( m_mode == HLTDEFER_MODE ) {
    ::scrc_put_chars(m_display,txt,NORMAL|BOLD,1,2,0);
    ::scrc_put_chars(m_display,"<CTRL-H for Help>, <CTRL-E to exit>",NORMAL|BOLD,2,40,0);
    ::scrc_put_chars(m_display,"nn",GREEN|INVERSE,1,80,0);
    ::scrc_put_chars(m_display,": Done,ready",NORMAL,1,82,0);
    ::scrc_put_chars(m_display,"nn",RED|INVERSE|BOLD,1,110,0);
    ::scrc_put_chars(m_display,": Files to be processed",NORMAL,1,112,0);
    ::scrc_put_chars(m_display,"nn",BLUE|INVERSE,2,80,0);
    ::scrc_put_chars(m_display,": Done/Excluded",NORMAL,2,82,0);
    ::scrc_put_chars(m_display,"nn",MAGENTA|INVERSE,2,110,0);
    ::scrc_put_chars(m_display,": Files to be processed/Excluded",NORMAL,2,112,1);
    ::sprintf(txt," %-10s %-8s %6s %6s %6s         %s",
	      "","Last","No.of","No.of","No.of","Subfarm");
    ::scrc_put_chars(m_display,txt,BG_BLUE|FG_WHITE|BOLD,CLUSTERLINE_FIRSTPOS-2,1,1);
    ::sprintf(txt," %-10s %-8s %6s %6s %6s        %7s   %-15s  %s",
	      "","Update","Nodes", "Runs","Files","Status","",
	      "<----------------------- Individual worker node status ----------------------->");
    ::scrc_put_chars(m_display,txt,BG_BLUE|FG_WHITE|BOLD,CLUSTERLINE_FIRSTPOS-1,1,1);
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
    ::sprintf(txt," %-10s %8s %6s %6s %6s %6s     %-9s %8s %8s      %s",
	      "","Last","No.of","Pro","No.of","No.of","Pieces","Download","Upload",
	      "< -------- Subfarm Information ------- >");
    ::scrc_put_chars(m_display,txt,BG_BLUE|FG_WHITE|BOLD,CLUSTERLINE_FIRSTPOS-2,1,1);
    ::sprintf(txt," %-10s %8s %6s %6s %6s %5s %6s/%-6s %8s %8s      %s",
	      "","Update","Session","gress","Upload","Peers","Done","Total","[MB]  ","[MB]  ",
	      "Controls PC and worker torrent status summary");
    ::scrc_put_chars(m_display,txt,BG_BLUE|FG_WHITE|BOLD,CLUSTERLINE_FIRSTPOS-1,1,1);
  }
  else {
    ::sprintf(txt," %-10s %-8s %-6s %-6s %-6s   %35s%44s%47s",
	      "","Last","No.of","No.of","Num.of","",
	      "<<------------ Summ Counters ----------->>",
	      "<<----------- Minimum Counters ---------->>");
    ::scrc_put_chars(m_display,txt,BG_BLUE|FG_WHITE|BOLD,CLUSTERLINE_FIRSTPOS-2,1,1);
    ::sprintf(txt," %-10s %-8s %-6s %-6s %-6s   %-32s  %9s%5s%11s%6s%9s%5s  %9s%5s%10s%7s%9s%5s",
	      "Subfarm","Update","Nodes", "Buffer","Client","Subfarm status",
	      "Overflow","Sl","Events","Sl","Send","Sl","Overflow","Sl","Events","Sl","Send","Sl");
    ::scrc_put_chars(m_display,txt,BG_BLUE|FG_WHITE|BOLD,CLUSTERLINE_FIRSTPOS-1,1,1);
  }

  ::scrc_end_pasteboard_update (m_pasteboard);
  ::scrc_fflush(m_pasteboard);
  ::scrc_set_cursor(m_display, 2, 10);
  ::scrc_cursor_off(m_pasteboard);
  ::wtc_remove(WT_FACILITY_SCR);
  ::wtc_subscribe(WT_FACILITY_SCR, key_rearm, key_action, m_pasteboard);
  MouseSensor::instance().start(pasteboard());
  MouseSensor::instance().add(this,m_display);
  if ( xml ) {
    m_listener = auto_ptr<PartitionListener>(new PartitionListener(this,m_name,m_match,xml));
  }
  else if ( all ) {
    m_svc = ::dic_info_service((char*)"DIS_DNS/SERVER_LIST",MONITORED,0,0,0,dnsDataHandler,(long)this,0,0);
  }
  else {
    m_listener = auto_ptr<PartitionListener>(new PartitionListener(this,m_name,m_match));
  }
}

/// Standard destructor
FarmLineDisplay::~FarmLineDisplay()  {  
  MouseSensor::instance().stop();
  ::wtc_remove(WT_FACILITY_SCR);
  disconnect();
  m_listener = auto_ptr<PartitionListener>(0);
  ::scrc_begin_pasteboard_update(m_pasteboard);
  m_ctrlDisplay = auto_ptr<CtrlNodeDisplay>(0);
  m_mbmDisplay = auto_ptr<BufferDisplay>(0);
  m_summaryDisplay = auto_ptr<HltSummaryDisplay>(0);
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

/// DIM command service callback
void FarmLineDisplay::dnsDataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    FarmLineDisplay* disp = *(FarmLineDisplay**)tag;
    disp->update(address);
  }
}

/// Get the name of the currently selected cluster
string FarmLineDisplay::selectedCluster() const {
  if ( m_sysDisplay.get() )
    return m_sysDisplay->clusterName();
  else if ( m_roDisplay.get() )
    return m_roDisplay->clusterName();
  else if ( m_torrentDisplay.get() )
    return m_torrentDisplay->clusterName();
  else if ( m_subfarmDisplay )
    return m_subfarmDisplay->clusterName();
  else if ( currentDisplay() )
    return currentDisplay()->name();
  return "";
}

/// Get the name of the currently selected cluster and node
pair<string,string> FarmLineDisplay::selectedNode() const {
  string node_name, cl = selectedCluster();
  if ( !cl.empty() ) {
    if ( m_sysDisplay.get() )
      node_name = m_sysDisplay->nodeName(m_subPosCursor);
    else if ( m_torrentDisplay.get() )
      node_name = m_roDisplay->nodeName(m_subPosCursor);
    else if ( m_roDisplay.get() )
      node_name = m_torrentDisplay->nodeName(m_subPosCursor);
    else if ( m_subfarmDisplay )
      node_name = m_subfarmDisplay->nodeName(m_subPosCursor);
  }
  return make_pair(cl,node_name);
}

/// Number of sub-nodes in a cluster
size_t FarmLineDisplay::selectedClusterSize() const {
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
int FarmLineDisplay::key_rearm (unsigned int /* fac */, void* param)  {
  Pasteboard* pb = (Pasteboard*)param;
  return ::scrc_fflush(pb);
}

/// Keyboard action
int FarmLineDisplay::key_action(unsigned int /* fac */, void* /* param */)  {
  int key = ::scrc_read_keyboard(0,0);
  if (!key) return WT_SUCCESS;
  RTL::Lock lock(screenLock());
  return s_fd->handleKeyboard(key);
}

/// Set cursor to position
void FarmLineDisplay::set_cursor() {
  if ( 0 != m_sysDisplay.get() ) {
    Display* d1 = m_sysDisplay->display();
    if ( d1 ) ::scrc_set_cursor(d1, m_subPosCursor+8, 2); // 8 is offset in child window to select nodes
  }
  else if ( 0 != m_roDisplay.get() ) {
    Display* d1 = m_roDisplay->display();
    if ( d1 ) ::scrc_set_cursor(d1, m_subPosCursor+8, 2); // 8 is offset in child window to select nodes
  }
  else if ( 0 != m_torrentDisplay.get() ) {
    Display* d1 = m_torrentDisplay->display();
    if ( d1 ) ::scrc_set_cursor(d1, m_subPosCursor+10, 2); // 8 is offset in child window to select nodes
  }
  else if ( 0 != m_subfarmDisplay ) {
    Display* d1 = m_subfarmDisplay->display();
    if ( d1 ) ::scrc_set_cursor(d1, m_subPosCursor+8, 2); // 8 is offset in child window to select nodes
  }
  else {
    m_currentLine = 0;
    //for(SubDisplays::iterator k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k)
    //  (*k).second->set_cursor();
    for(SubDisplays::iterator k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k) {
      ClusterLine* curr = (*k).second;
      if ( curr->position() == m_posCursor+CLUSTERLINE_FIRSTPOS ) m_currentLine = curr;
      curr->set_cursor();
    }
  }
}

/// Get farm display from cursor position
ClusterLine* FarmLineDisplay::currentDisplay()  const {
  return m_currentLine;
}

/// Get farm display name from cursor position
string FarmLineDisplay::currentDisplayName()  const {
  ClusterLine* d = currentDisplay();
  return d ? d->name() : string("");
}

/// DIM command service callback
void FarmLineDisplay::update(const void* address) {
  char *msg = (char*)address;
  string svc, node;
  size_t idx, idq;
  switch(msg[0]) {
  case '+':
    getServiceNode(++msg,svc,node);
    idx = svc.find("/ROpublish");
    idq = svc.find("/hlt");
    if ( idq == string::npos ) idq = svc.find("/mona");
    if ( idq == string::npos ) idq = svc.find("/store");
    if ( idx != string::npos && idq == 0 ) {
      string f = svc.substr(1,idx-1);
      if ( ::strcase_match_wild(f.c_str(),m_match.c_str()) ) {
        IocSensor::instance().send(this,CMD_ADD,new string(f));
      }
    }
    break;
  case '-':
    break;
  case '!':
    //getServiceNode(++msg,svc,node);
    //log() << "Service " << msg << " in ERROR." << endl;
    break;
  default:
    if ( *(int*)msg != *(int*)"DEAD" )  {
      char *at, *p = msg, *last = msg;
      auto_ptr<Farms> farms(new Farms);
      while ( last != 0 && (at=strchr(p,'@')) != 0 )  {
        last = strchr(at,'|');
        if ( last ) *last = 0;
        getServiceNode(p,svc,node);
        idx = svc.find("/ROpublish");
        idq = svc.find("/hlt");
	if ( idq == string::npos ) idq = svc.find("/mona");
	if ( idq == string::npos ) idq = svc.find("/store");
        if ( idx != string::npos && idq == 0 ) {
          string f = svc.substr(1,idx-1);
          if ( ::strcase_match_wild(f.c_str(),m_match.c_str()) ) {
            farms->push_back(f);
          }
        }
        p = last+1;
      }
      if ( !farms->empty() )
        IocSensor::instance().send(this,CMD_CONNECT,farms.release());
    }
    break;
  }
}

/// Show the run processing summary window
int FarmLineDisplay::showDeferredSummaryWindow() {
  DisplayUpdate update(this,true);
  if ( m_summaryDisplay.get() ) {
    MouseSensor::instance().remove(this,m_summaryDisplay->display());
    m_summaryDisplay = auto_ptr<HltSummaryDisplay>(0);
  }
  else    {
    m_summaryDisplay = auto_ptr<HltSummaryDisplay>(new HltSummaryDisplay(this));
  }
  if ( m_summaryDisplay.get() ) {
    m_summaryDisplay->show(m_anchorY,m_anchorX);
    MouseSensor::instance().add(this,m_summaryDisplay->display());
  }
  return WT_SUCCESS;
}

/// Handle keyboard interrupts
int FarmLineDisplay::handleKeyboard(int key)    {
  if ( m_mode == HLTDEFER_MODE && (key == 'S' || key=='s') ) {
    // Invoke here run processing summary!
    return showDeferredSummaryWindow();
  }
  else if ( FarmDisplayBase::handleKeyboard(key) == WT_SUCCESS ) {
    return WT_SUCCESS;
  }
  try {
    switch (key)    {
    case MOVE_UP:
      if ( int(m_posCursor) < 0 || m_posCursor>=subDisplays().size() )  
	m_posCursor = 0;
      if ( int(m_subPosCursor) < 0 || m_subPosCursor>=selectedClusterSize() )  
        m_subPosCursor = 0;
      if( 0 == m_nodeSelector && m_posCursor > 0 )
        --m_posCursor;
      else if( m_nodeSelector && int(m_subPosCursor) >= 0 )
        --m_subPosCursor;
      break;
    case MOVE_DOWN:
      if ( int(m_posCursor) < 0 || m_posCursor>=subDisplays().size() )  
	m_posCursor = 0;
      if ( int(m_subPosCursor) < 0 || m_subPosCursor>=selectedClusterSize() )  
        m_subPosCursor = 0;
      if( 0 == m_nodeSelector && m_posCursor < subDisplays().size()-1 )
        ++m_posCursor;
      else if( m_nodeSelector && selectedClusterSize() > m_subPosCursor )
        ++m_subPosCursor;
      break;
    case MOVE_LEFT:
      break;
    case MOVE_RIGHT:
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
void FarmLineDisplay::handle(const Event& ev) {
  int cnt = 0;
  time_t now = time(0);
  ClusterLine* d = 0;
  Farms::iterator i;
  SubDisplays::iterator k;
  const MouseEvent* m = 0;

  DimLock dim_lock;
  RTL::Lock lock(screenLock());
  switch(ev.eventtype) {
  case ScrMouseEvent:
    m = ev.get<MouseEvent>();
    if ( handleMouseEvent(m) ) {
    }
    else if ( 0 == m_nodeSelector )    {
      m_posCursor = m->y - (CLUSTERLINE_FIRSTPOS + 1);
      set_cursor();
    }
    else if ( m_nodeSelector && m->msec == (unsigned int)-1 ) {
      Display* disp = m_nodeSelector->display();
      if ( m->display == disp )    {
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
    case CMD_SHOWDEFERREDRUNS:
      if ( m_summaryDisplay.get() )   {
        IocSensor::instance().send(m_summaryDisplay.get(),CMD_UPDATE,&m_farmDisplays);
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
      if ( m_summaryDisplay.get() )   {
        IocSensor::instance().send(m_summaryDisplay.get(),CMD_UPDATE,&m_farmDisplays);
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
      else if ( m_mode == TORRENT_MODE && m_torrentDisplay.get() ) {
        //const void* data = m_torrentDisplay->data().pointer;
        //m_torrentDisplay->setNode(m_subPosCursor);
        //m_torrentDisplay->update(data);
      }
      TimeSensor::instance().add(this,1,m_subfarmDisplay);
      break;
    case CMD_ADD:
      if ( (i=find(m_farms.begin(),m_farms.end(),*ev.iocPtr<string>())) == m_farms.end() )  {
        m_farms.push_back(*ev.iocPtr<string>());
        connect(m_farms);
      }
      delete ev.iocPtr<string>();
      return;
    case CMD_CONNECT:
      m_farms = *ev.iocPtr<vector<string> >();
      connect(m_farms);
      delete ev.iocPtr<vector<string> >();
      return;
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

void FarmLineDisplay::connect(const vector<string>& vfarms) {
  typedef set<string> FarmSet;
  SubDisplays::iterator k;
  SubDisplays copy;
  char txt[256];
  DisplayUpdate update(this,false);

  set<string> farms;
  if ( m_mode == TORRENT_MODE ) {
    //farms.insert("plus10");
  }
  for (Farms::const_iterator v=vfarms.begin(); v != vfarms.end(); ++v) farms.insert(*v);

  ::sprintf(txt,"Total number of subfarms:%d    ",int(farms.size()));
  ::scrc_put_chars(m_display,txt,NORMAL|BOLD,2,3,0);
  m_currentLine = 0;

  int pos = CLUSTERLINE_FIRSTPOS-1;
  DimReverseLock lock;  // unlock DIM here, since connect/disconnect calls DIM
  for (FarmSet::const_iterator i=farms.begin(); i != farms.end(); ++i) {
    k = m_farmDisplays.find(*i);
    ++pos;
    if ( k == m_farmDisplays.end() ) {
      if ( m_mode == RECO_MODE )
	copy.insert(make_pair(*i,createClusterLine("RecFarm",this,pos,*i)));
      else if ( m_mode == CTRL_MODE )
	copy.insert(make_pair(*i,createClusterLine("CtrlFarm",this,pos,*i)));
      else if ( m_mode == HLTDEFER_MODE )
	copy.insert(make_pair(*i,createClusterLine("HLT",this,pos,*i)));
      else if ( m_mode == TORRENT_MODE )
	copy.insert(make_pair(*i,createClusterLine("TorrentFarm",this,pos,*i)));
      else if ( ::strncasecmp((*i).c_str(),"mona0",5)==0 )
	copy.insert(make_pair(*i,createClusterLine("Monitoring",this,pos,*i)));
      else if ( ::strncasecmp((*i).c_str(),"storectl",8)==0 )
	copy.insert(make_pair(*i,createClusterLine("Storage",this,pos,*i)));
      else
	copy.insert(make_pair(*i,createClusterLine("Farm",this,pos,*i)));
    }
    else {
      copy.insert(*k);
      m_farmDisplays.erase(k);
      continue;
    }
    if ( !m_currentLine ) m_currentLine = copy[*i];
  }
  for (k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k)
    delete (*k).second;
  m_farmDisplays = copy;
}

static size_t do_output(void*,int,const char* fmt, va_list args) {
  char buffer[1024];
  size_t len = ::vsnprintf(buffer, sizeof(buffer), fmt, args);
  return len;
}

extern "C" int romon_farmex(int argc,char** argv) {
  FarmLineDisplay* disp = new FarmLineDisplay(argc,argv);
  ::lib_rtl_install_printer(do_output,0);
  IocSensor::instance().run();
  delete disp;
  return 1;
}
