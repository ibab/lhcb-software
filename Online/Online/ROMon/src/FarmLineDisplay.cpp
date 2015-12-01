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
  char txt[512];
  vector<string> listeners;
  string anchor, prefix, postfix, tmp;
  RTL::CLI cli(argc,argv,help);
  bool all = 0 != cli.getopt("all",2);
  bool xml = 0 != cli.getopt("xml",2);
  cli.getopt("partition",   2, m_name = "ALL");
  cli.getopt("match",       2, m_match = "*");
  cli.getopt("prefix",      2, prefix);
  cli.getopt("postfix",     2, postfix);
  cli.getopt("sdh",         2, m_subDisplayHeight);
  cli.getopt("node-height", 7, m_subDisplayHeight);

  if ( m_name!="*" && m_name!="ALL" ) m_partition = m_name;
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
    ::lib_rtl_sleep(15000);
  }

  tmp = m_name;
  for(size_t idx = tmp.find('+'); idx != string::npos; idx = tmp.find('+')) {
    listeners.push_back(string(tmp.substr(0,idx)));
    tmp = string(tmp.substr(++idx));
  }
  listeners.push_back(tmp);
  m_name = *listeners.begin();

  if ( !prefix.empty()  ) InternalDisplay::setSvcPrefix(prefix);
  if ( !postfix.empty() ) InternalDisplay::setSvcPostfix(postfix);
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
    ::scrc_put_chars(m_display,"nn",RED|INVERSE,1,110,0);
    ::scrc_put_chars(m_display,": Files to be processed",NORMAL,1,112,0);
    ::scrc_put_chars(m_display,"  ",YELLOW|INVERSE|BOLD,1,140,0);
    ::scrc_put_chars(m_display,": No information",NORMAL,1,142,0);
    ::scrc_put_chars(m_display,"nn",GREEN|INVERSE|BOLD,1,160,0);
    ::scrc_put_chars(m_display,": Done / overflow ON",NORMAL,1,162,0);

    ::scrc_put_chars(m_display,"nn",BLUE|INVERSE,2,80,0);
    ::scrc_put_chars(m_display,": Done/Excluded",NORMAL,2,82,0);
    ::scrc_put_chars(m_display,"nn",MAGENTA|INVERSE,2,110,0);
    ::scrc_put_chars(m_display,": Files present / Excluded",NORMAL,2,112,1);
    ::scrc_put_chars(m_display,"nn",RED|INVERSE|BOLD,2,160,0);
    ::scrc_put_chars(m_display,": Files present / overflow ON",NORMAL,2,162,0);

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
    ::scrc_put_chars(m_display," nn ",GREEN|INVERSE,1,78,0);
    ::scrc_put_chars(m_display,": OK, TorrentInfo present",NORMAL,1,82,0);
    ::scrc_put_chars(m_display," nn ",BLUE|INVERSE,2,78,0);
    ::scrc_put_chars(m_display,": Excluded from HLT",NORMAL,2,82,0);
    ::scrc_put_chars(m_display," nn ",RED|INVERSE|BOLD,1,110,0);
    ::scrc_put_chars(m_display,": Not OK, No torrent info",NORMAL,1,114,0);
    ::scrc_put_chars(m_display," nn ",BG_BLACK|FG_YELLOW|BOLD,2,110,0);
    ::scrc_put_chars(m_display,": Executing",NORMAL,2,114,0);
    ::scrc_put_chars(m_display,"    ",MAGENTA|INVERSE|BOLD,2,138,0);
    ::scrc_put_chars(m_display,": No information",NORMAL,2,142,0);
    ::sprintf(txt," %-10s %8s %6s %6s %6s %6s     %-9s %8s %8s      %s",
              "","Last","No.of","Pro","No.of","No.of","Pieces","Download","Upload",
              "< -------- Subfarm Information ------- >");
    ::scrc_put_chars(m_display,txt,BG_BLUE|FG_WHITE|BOLD,CLUSTERLINE_FIRSTPOS-2,1,1);
    ::sprintf(txt," %-10s %8s %6s %6s %6s %5s %6s/%-6s %8s %8s      %s",
              "","Update","Session","gress","Torr.","Peers","Done","Total","[MB]  ","[MB]  ",
              "Controls PC and worker torrent status summary");
    ::scrc_put_chars(m_display,txt,BG_BLUE|FG_WHITE|BOLD,CLUSTERLINE_FIRSTPOS-1,1,1);
  }
  else {
    ::sprintf(txt," %-10s %-8s %-6s %-6s %-6s  %35s %-27s  %-27s  %-27s  %-27s",
              "","Last","No.of","No.of","Num.of","<<------------------------------>>",
              "<<----------------------->>",
              "<<----------------------->>",
              "<<----------------------->>",
              "<<----------------------->>");
    ::scrc_put_chars(m_display,txt,BG_BLUE|FG_WHITE|BOLD,CLUSTERLINE_FIRSTPOS-2,1,1);
    ::sprintf(txt," %-10s %-8s %-6s %-6s %-6s  %-33s   %-11s%10s%6s  %-11s%10s%6s  %-11s%10s%6s  %-11s%10s%6s",
              "Subfarm","Update","Nodes", "Buffer","Client","          Subfarm status         ",
              "Buffer Name","Events","Slots","Buffer Name","Events","Slots",
              "Buffer Name","Events","Slots","Buffer Name","Events","Slots");
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
    m_listeners.push_back(new PartitionListener(this,m_name,m_match,xml));
  }
  else {
    for(vector<string>::const_iterator il=listeners.begin();il!=listeners.end(); ++il)
      m_listeners.push_back(new PartitionListener(this,*il,m_match));
  }
}

/// Standard destructor
FarmLineDisplay::~FarmLineDisplay()  {  
  MouseSensor::instance().stop();
  ::wtc_remove(WT_FACILITY_SCR);
  disconnect();
  for(Listeners::iterator il=m_listeners.begin(); il!=m_listeners.end();++il)
    delete (*il);
  m_listeners.clear();
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
  m_lines.clear();
  close();
  ::scrc_end_pasteboard_update (m_pasteboard);
  ::scrc_delete_pasteboard(m_pasteboard);
  m_pasteboard = 0;
  ::scrc_resetANSI();
  ::printf("Farm display deleted and resources freed......\n");
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
    if ( m_sysDisplay.get() && m_sysDisplay.get() == m_nodeSelector )
      node_name = m_sysDisplay->nodeName(m_subPosCursor);
    else if ( m_torrentDisplay.get() && m_torrentDisplay.get() == m_nodeSelector )
      node_name = m_torrentDisplay->nodeName(m_subPosCursor);
    else if ( m_roDisplay.get() && m_roDisplay.get() == m_nodeSelector )
      node_name = m_roDisplay->nodeName(m_subPosCursor);
    else if ( m_deferHltDisplay.get() && m_deferHltDisplay.get() == m_nodeSelector )
      node_name = m_deferHltDisplay->nodeName(m_subPosCursor);
    else if ( m_subfarmDisplay )
      node_name = m_subfarmDisplay->nodeName(m_subPosCursor);
  }
  return make_pair(cl,node_name);
}

/// Number of sub-nodes in a cluster
size_t FarmLineDisplay::selectedClusterSize() const {
  if ( m_sysDisplay.get() && m_sysDisplay.get() == m_nodeSelector )
    return m_sysDisplay->numNodes();
  else if ( m_torrentDisplay.get() && m_torrentDisplay.get() == m_nodeSelector )
    return m_torrentDisplay->numNodes();
  else if ( m_roDisplay.get() && m_roDisplay.get() == m_nodeSelector )
    return m_roDisplay->numNodes();
  else if ( m_deferHltDisplay.get() && m_deferHltDisplay.get() == m_nodeSelector )
    return m_deferHltDisplay->numNodes();
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
    Display* disp = m_sysDisplay->display();
    if ( disp ) ::scrc_set_cursor(disp, m_subPosCursor+8, 2); // 8 is offset in child window to select nodes
  }
  else if ( 0 != m_roDisplay.get() ) {
    Display* disp = m_roDisplay->display();
    if ( disp ) ::scrc_set_cursor(disp, m_subPosCursor+8, 2); // 8 is offset in child window to select nodes
  }
  else if ( 0 != m_torrentDisplay.get() ) {
    Display* disp = m_torrentDisplay->display();
    if ( disp ) ::scrc_set_cursor(disp, m_subPosCursor+10, 2); // 8 is offset in child window to select nodes
  }
  else if ( 0 != m_subfarmDisplay ) {
    Display* disp = m_subfarmDisplay->display();
    if ( disp ) {
      int pos = m_subPosCursor+8; // 8 is offset in child window to select nodes
      if ( m_nodeSelector )  {
        disp = m_nodeSelector->display();
        pos = m_subPosCursor + 2 + 1;  // Border + offset
      }
      ::scrc_set_cursor(disp, pos, 2);
    }
  }
  else {
    m_currentLine = 0;
    for(SubDisplays::iterator k=m_lines.begin(); k != m_lines.end(); ++k) {
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

/// Get farm <partition>/<display name> from cursor position
std::string FarmLineDisplay::currentCluster()  const {
  ClusterLine* d = currentDisplay();
  if ( d ) return d->partition() +"/" + d->name();
  return "";
}

/// Get farm display name from cursor position
string FarmLineDisplay::currentDisplayName()  const {
  ClusterLine* d = currentDisplay();
  return d ? d->name() : string("");
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
  // Invoke here run processing summary!
  if ( m_mode == HLTDEFER_MODE && (key == 'S' || key=='s') ) {
    return showDeferredSummaryWindow();
  }
  else if ( FarmDisplayBase::handleKeyboard(key) == WT_SUCCESS ) {
    return WT_SUCCESS;
  }
  try {
    switch (key)    {
    case MOVE_UP:
      if ( int(m_posCursor) < 0 || m_posCursor>=m_lines.size() )  
        m_posCursor = 0;
      if ( int(m_subPosCursor) < 0 || m_subPosCursor>=selectedClusterSize() )  
        m_subPosCursor = 0;
      if( 0 == m_nodeSelector && m_posCursor > 0 )
        --m_posCursor;
      else if( m_nodeSelector && int(m_subPosCursor) > 0 )
        --m_subPosCursor;
      break;
    case MOVE_DOWN:
      if ( int(m_posCursor) < 0 || m_posCursor>=m_lines.size() )  
        m_posCursor = 0;
      if ( int(m_subPosCursor) < 0 || m_subPosCursor>=selectedClusterSize() )  
        m_subPosCursor = 0;
      if( 0 == m_nodeSelector && m_posCursor < m_lines.size()-1 )
        ++m_posCursor;
      else if( m_nodeSelector && selectedClusterSize()-1 > m_subPosCursor )
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
void FarmLineDisplay::handle(const CPP::Event& ev) {
  typedef vector<string> StringV;
  int cnt = 0;
  time_t now = time(0);
  ClusterLine* d = 0;
  SubDisplays::iterator k;
  const SCR::MouseEvent* m = 0;

  DimLock dim_lock;
  RTL::Lock lock(screenLock());
  switch(ev.eventtype) {
  case ScrMouseEvent:
    m = ev.get<SCR::MouseEvent>();
    if ( handleMouseEvent(m) ) {
    }
    else if ( 0 == m_nodeSelector )    {
      m_posCursor = m->y - (CLUSTERLINE_FIRSTPOS + 1);
      set_cursor();
    }
    else if ( m_nodeSelector && m->msec == (unsigned int)-1 ) {
      Display* disp = m_nodeSelector->display();
      if ( m->display == disp )    {
        size_t pos = m->y - 2 - disp->row;
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
      for(k=m_lines.begin(); k != m_lines.end(); ++k, ++cnt) {
        if ( (d=(*k).second) == ev.data )  {
          m_posCursor = cnt;
          IocSensor::instance().send(this,CMD_SHOWSUBFARM,this);
          return;
        }
      }
      break;
    case CMD_POSCURSOR:
      for(k=m_lines.begin(); k != m_lines.end(); ++k, ++cnt) {
        if ( (d=(*k).second) == ev.data )  {
          m_posCursor = cnt;
          set_cursor();
          return;
        }
      }
      break;
    case CMD_SHOWDEFERREDRUNS:
      if ( m_summaryDisplay.get() )   {
        IocSensor::instance().send(m_summaryDisplay.get(),CMD_UPDATE,&m_lines);
      }
      IocSensor::instance().send(this,CMD_SUMMARIZE,this);
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
      if ( m_deferHltDisplay.get() )   {
        IocSensor::instance().send(m_deferHltDisplay.get(),ROMonDisplay::CMD_UPDATEDISPLAY,this);
      }
      if ( m_summaryDisplay.get() )   {
        IocSensor::instance().send(m_summaryDisplay.get(),CMD_UPDATE,&m_lines);
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

    case CMD_SUMMARIZE: {
      char text[132];
      ClusterLine::Summary summary;
      for(k=m_lines.begin(); k != m_lines.end(); ++k, ++cnt)
        (*k).second->collect(summary);
      if ( summary.size() > 0 ) {
        DisplayUpdate update(this,false);
        if ( m_mode == HLTDEFER_MODE ) {
          ::sprintf(text," Total:%13s %6ld %6ld %6ld ","",summary[0].second,summary[1].second,summary[2].second);
          ::scrc_put_chars(m_display,text,BG_BLUE|FG_WHITE|BOLD,CLUSTERLINE_FIRSTPOS+int(m_lines.size())+1,1,1);
        }
      }
      break;
    }
    case CMD_ADD: {
      StringV farms;
      for(k=m_lines.begin(); k != m_lines.end(); ++k) {
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
      for(k=m_lines.begin(); k != m_lines.end(); ++k)
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

void FarmLineDisplay::connect(const string& section, const vector<string>& vfarms) {
  typedef set<string> FarmSet;
  char txt[256], text[256];
  SubDisplays::iterator k;
  SubDisplays copy;
  FarmSet farms;
  string nam;

  DisplayUpdate update(this,false);
  for(vector<string>::const_iterator v=vfarms.begin(); v != vfarms.end(); ++v) farms.insert(*v);

  m_currentLine = 0;
  nam = section+"/";
  for (k=m_lines.begin(); k != m_lines.end(); ++k) {
    if ( (*k).first.find(nam) == string::npos ) {
      copy.insert(*k);
      (*k).second = 0;
    }
  }
  for (FarmSet::const_iterator i=farms.begin(); i != farms.end(); ++i) {
    nam = section + "/" + *i;
    k = m_lines.find(nam);
    if ( k == m_lines.end() ) {
      if ( m_mode == RECO_MODE )
        copy.insert(make_pair(nam,createClusterLine("RecFarm",this,section,*i)));
      else if ( m_mode == CTRL_MODE )
        copy.insert(make_pair(nam,createClusterLine("CtrlFarm",this,section,*i)));
      else if ( m_mode == HLTDEFER_MODE )
        copy.insert(make_pair(nam,createClusterLine("HLT",this,section,*i)));
      else if ( m_mode == TORRENT_MODE )
        copy.insert(make_pair(nam,createClusterLine("TorrentFarm",this,section,*i)));
      else if ( ::strncasecmp((*i).c_str(),"mona0",5)==0 )
        copy.insert(make_pair(nam,createClusterLine("Monitoring",this,section,*i)));
      else if ( ::strncasecmp((*i).c_str(),"storectl",8)==0 )
        copy.insert(make_pair(nam,createClusterLine("Storage",this,section,*i)));
      else
        copy.insert(make_pair(nam,createClusterLine("Farm",this,section,*i)));
    }
    else  {
      copy.insert(*k);
      (*k).second = 0;
      continue;
    }
    if ( !m_currentLine ) m_currentLine = copy[nam];
  }
  for (k=m_lines.begin(); k != m_lines.end(); ++k)
    if ( (*k).second ) delete (*k).second;

  m_lines = copy;
  int num_partition = 0;
  for (k=m_lines.begin(), nam=""; k != m_lines.end(); ++k) {
    if ( nam != (*k).second->partition() )   {
      ++num_partition;
      nam = (*k).second->partition();
    }
  }

  // Re-adjust positions
  int pos = CLUSTERLINE_FIRSTPOS-1;
  for (k=m_lines.begin(), nam=""; k != m_lines.end(); ++k) {
    ClusterLine* l = (*k).second;
    if ( num_partition>1 && nam != l->partition() ) {
      nam = l->partition();
      ::memset(text,'-',sizeof(text));
      text[sizeof(text)-1] = 0;
      ::sprintf(txt,"  Partition:   %s ",nam.c_str());
      ::strncpy(text+42,txt,::strlen(txt));
      ::scrc_put_chars(m_display,text,BLUE|BOLD,++pos,1,1);
    }
    (*k).second->setPosition(++pos);
  }
  ::sprintf(txt,"Total number of subfarms:%d    ",int(m_lines.size()));
  ::scrc_put_chars(m_display,txt,NORMAL|BOLD,2,3,0);
  while(pos<100)
    ::scrc_put_chars(m_display,"",NORMAL,++pos,1,1); // Clear old lines    

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
