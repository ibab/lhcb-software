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
#include "ROMon/ClusterDisplay.h"
#include "ROMon/FarmDisplayBase.h"
#include "ROMon/TorrentNodeDisplay.h"
#include "ROMon/CPUMon.h"
#include "SCR/MouseSensor.h"
#include "CPP/TimeSensor.h"
#include "CPP/IocSensor.h"
#include "RTL/Lock.h"
#include "SCR/scr.h"
#include "WT/wtdef.h"
#include "ROMonDefs.h"
extern "C" {
#include "dic.h"
}

using namespace ROMon;
using namespace SCR;
using namespace std;

typedef Nodeset::Nodes               Nodes;
typedef Node::Buffers                Buffers;
typedef MBMBuffer::Clients           Clients;
typedef Node::Tasks                  Tasks;
typedef vector<string>               StringV;

namespace ROMon {
  InternalDisplay* createBootDisplay(InternalDisplay* parent, const string& title);
  InternalDisplay* createBenchmarkDisplay(InternalDisplay* parent,int mode, const string& title);
  InternalDisplay* createFarmStatsDisplay(InternalDisplay* parent, const string& title);

  ClusterDisplay*  createSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv);
  ClusterDisplay*  createHltSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv);
  ClusterDisplay*  createRecSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv);
  ClusterDisplay*  createCtrlSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv);
  ClusterDisplay*  createStorageDisplay(int width, int height, int posx, int posy, int argc, char** argv);
  ClusterDisplay*  createMonitoringDisplay(int width, int height, int posx, int posy, int argc, char** argv);
}
namespace BitTorrent {
  ClusterDisplay*  createTorrentDisplay(int width, int height, int posx, int posy, int argc, char** argv);
}

namespace {
  ScrDisplay* swapMouseSelector(CPP::Interactor* ia, ClusterDisplay* from, ClusterDisplay* to) {
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
FarmDisplayBase::FarmDisplayBase()
  : InternalDisplay(0,""), m_subfarmDisplay(0), m_nodeSelector(0),
    m_subDisplayHeight(3), m_anchorX(10), m_anchorY(5), m_mode(HLT_MODE), 
    m_posCursor(0), m_subPosCursor(0), m_reverse(false)
{
  ::dim_init();
}

FarmDisplayBase::~FarmDisplayBase()
{
}

/// Show context dependent help window
int FarmDisplayBase::showHelpWindow() {
  DisplayUpdate update(this,true);
  string input = ::getenv("ROMONROOT") != 0 ? ::getenv("ROMONROOT") : "..";
  if ( m_helpDisplay.get() ) {
    MouseSensor::instance().remove(this,m_helpDisplay->display());
    m_helpDisplay = auto_ptr<HelpDisplay>(0);
  }
  else if ( m_bootDisplay.get() ) {
    string fin = input+"/doc/bootMon.hlp";
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","boot-subfarm",fin));
  }
  else if ( m_statsDisplay.get() ) {
    string fin = input+"/doc/farmStats.hlp";
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","stats-subfarm",fin));
  }
  else if ( m_torrentDisplay.get() || (m_mode == TORRENT_MODE) ) {
    string fin = input+"/doc/torrent.hlp";
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","torrent-subfarm",fin));
  }
  else if ( m_benchDisplay.get() && !(m_mode == CTRL_MODE || m_mode == RECO_MODE) ) {
    string fin = input+"/doc/benchmark.hlp";
    const char* tag = "benchmark-farm";
    if ( m_mbmDisplay.get() ) tag = "benchmark-node";
    else if ( m_subfarmDisplay ) tag = "benchmark-subfarm";
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window",tag,fin));
  }
  else if ( m_sysDisplay.get() ) 
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","subfarm_ctrl"));
  else if ( m_roDisplay.get() ) 
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","subfarm"));
  else if ( m_mbmDisplay.get() ) 
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","mbm"));
  else if ( m_ctrlDisplay.get() ) 
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","ctrl"));
  else if ( m_deferHltDisplay.get() ) 
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","defer_hlt"));
  else if ( m_procDisplay.get() ) 
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","procs"));
  else if ( m_cpuDisplay.get() ) 
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","cpu"));
  else if ( m_subfarmDisplay && m_mode == CTRL_MODE )
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","subfarm_ctrl"));
  else if ( m_subfarmDisplay && m_mode == RECO_MODE )
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","subfarm_reco"));
  else if ( m_subfarmDisplay )
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","subfarm"));
  else
    m_helpDisplay = auto_ptr<HelpDisplay>(new HelpDisplay(this,"Help window","farm"));
  if ( m_helpDisplay.get() ) {
    m_helpDisplay->show(m_anchorY,m_anchorX);
    MouseSensor::instance().add(this,m_helpDisplay->display());
  }
  return WT_SUCCESS;
}

/// Show subfarm display
int FarmDisplayBase::showSubfarm()    {
  string dnam = currentDisplayName();
  string part_name = currentCluster();

  part_name = part_name.empty() ? m_name : part_name.substr(0,part_name.find("/"));
  if ( m_subfarmDisplay ) {
    DisplayUpdate update(this,true);
    m_nodeSelector = swapMouseSelector(this,m_subfarmDisplay,0);
    m_nodeSelector = swapMouseSelector(this,m_torrentDisplay.get(),0);
    m_nodeSelector = swapMouseSelector(this,m_sysDisplay.get(),0);
    m_nodeSelector = swapMouseSelector(this,m_roDisplay.get(),0);
    m_subfarmDisplay->finalize();
    delete m_subfarmDisplay;
    m_subfarmDisplay     = 0;
    m_torrentDisplay     = auto_ptr<ClusterDisplay>(0);
    m_roDisplay          = auto_ptr<ClusterDisplay>(0);
    m_sysDisplay         = auto_ptr<ClusterDisplay>(0);
    m_deferHltDisplay    = auto_ptr<ClusterDisplay>(0);
    m_cpuDisplay         = auto_ptr<CPUDisplay>(0);
    m_mbmDisplay         = auto_ptr<BufferDisplay>(0);
    m_procDisplay        = auto_ptr<ProcessDisplay>(0);
    m_bootDisplay        = auto_ptr<InternalDisplay>(0);
    m_statsDisplay       = auto_ptr<InternalDisplay>(0);
    m_ctrlDisplay        = auto_ptr<CtrlNodeDisplay>(0);
    m_benchDisplay       = auto_ptr<InternalDisplay>(0);
    m_torrentNodeDisplay = auto_ptr<InternalDisplay>(0);
    m_subPosCursor       = 0;
  }
  else if ( !dnam.empty() ) {
    string svc = "-servicename="+svcPrefix()+dnam+"/ROpublish";
    string part= "-partition="+part_name;
    if ( m_mode == CTRL_MODE ) {
      string node = "-node="+strupper(dnam);
      svc = "-servicename="+svcPrefix()+strupper(dnam)+"/TaskSupervisor/Status";
      const char* argv[] = {"",svc.c_str(), node.c_str(), "-delay=300"};
      m_subfarmDisplay = createCtrlSubfarmDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX,m_anchorY,4,(char**)argv);
    }
    else if ( m_mode == TORRENT_MODE ) {
      string node = "-node="+strupper(dnam);
      svc = "-servicename=/"+strupper(dnam)+"/TorrentInfo";
      const char* argv[] = {"",svc.c_str(), node.c_str(), "-delay=300"};
      m_subfarmDisplay = BitTorrent::createTorrentDisplay(SUBFARM_WIDTH+20,SUBFARM_HEIGHT,m_anchorX,m_anchorY,4,(char**)argv);
    }
    else if ( m_mode == HLTDEFER_MODE ) {
      svc = "-servicename="+svcPrefix()+dnam+"/ROpublish/HLT1";
      if ( !svcPostfix().empty() ) svc = "-servicename="+svcPrefix()+dnam+svcPostfix();
      const char* argv[] = {"", svc.c_str(), "-delay=300" };
      m_subfarmDisplay = createHltSubfarmDisplay(SUBFARM_WIDTH+20,SUBFARM_HEIGHT,m_anchorX,m_anchorY,3,(char**)argv);
    }
    else if ( strncasecmp(dnam.c_str(),"storectl01",10)==0 && part_name != "ALL" ) {
      const char* argv[] = {"", svc.c_str(), part.c_str(), "-delay=300"};
      m_subfarmDisplay = createStorageDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX,m_anchorY,4,(char**)argv);
    }
    else if ( strncasecmp(dnam.c_str(),"mona08",6)==0 && part_name != "ALL" ) {
      string relay = "-namerelay="+dnam+"01";
      const char* argv[] = {"", svc.c_str(), part.c_str(), "-delay=300", "-relayheight=12", "-nodeheight=12", relay.c_str()};
      m_subfarmDisplay = createMonitoringDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX,m_anchorY,7,(char**)argv);
    }
    else if ( strncasecmp(dnam.c_str(),"mona09",6)==0 && part_name != "ALL" ) {
      string relay = "-namerelay="+dnam+"01";
      const char* argv[] = {"", svc.c_str(), part.c_str(), "-delay=300", "-relayheight=12", "-nodeheight=12", relay.c_str()};
      m_subfarmDisplay = createMonitoringDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX,m_anchorY,7,(char**)argv);
    }
    else if ( m_mode == RECO_MODE ) {
      const char* argv[] = {"", svc.c_str(), "-delay=300"};
      m_subfarmDisplay = createRecSubfarmDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX,m_anchorY,3,(char**)argv);
    }
    else if ( m_mode == HLT_MODE ) {
      const char* argv[] = {"", svc.c_str(), "-delay=300", "-mooresheight=-1", "-nodesheight=35", part.c_str()};
      m_subfarmDisplay = createSubfarmDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX,m_anchorY,6,(char**)argv);
    }
    else {
      m_nodeSelector = 0;
      m_subfarmDisplay = 0;
      return WT_SUCCESS;
    }
    m_subfarmDisplay->initialize();
    ::lib_rtl_sleep(200);
    m_nodeSelector = swapMouseSelector(this,m_sysDisplay.get(),m_subfarmDisplay);
    IocSensor::instance().send(this,CMD_UPDATE,m_subfarmDisplay);
    m_subPosCursor = 0;
  }
  return WT_SUCCESS;
}

/// Show window with buffer information of a given node
int FarmDisplayBase::showCtrlWindow() {
  DisplayUpdate update(this,true);
  if ( m_ctrlDisplay.get() ) {
    if ( m_helpDisplay.get() ) showHelpWindow();
    MouseSensor::instance().remove(this,m_ctrlDisplay->display());
    m_ctrlDisplay = auto_ptr<CtrlNodeDisplay>(0);
  }
  else if ( m_subfarmDisplay ) {
    m_ctrlDisplay = auto_ptr<CtrlNodeDisplay>(new CtrlNodeDisplay(this,"CTRL Monitor display"));
    m_ctrlDisplay->setNode(m_subPosCursor);
    if ( m_mode == CTRL_MODE ) {
      m_ctrlDisplay->update(m_subfarmDisplay->data().pointer);
    }
    else {
      string node_name = m_subfarmDisplay->nodeName(0);
      node_name = node_name.substr(0,node_name.length()-2);
      m_ctrlDisplay->connect(node_name);
    }
    m_ctrlDisplay->show(m_anchorY+5,m_anchorX+12);
    MouseSensor::instance().add(this,m_ctrlDisplay->display());
  }
  return WT_SUCCESS;
}

/// Show window with deferred trigger information for a subfarm
int FarmDisplayBase::showDeferHltWindow() {
  DisplayUpdate update(this,true);
  if ( m_deferHltDisplay.get() ) {
    if ( m_helpDisplay.get() ) showHelpWindow();
    MouseSensor::instance().remove(this,m_deferHltDisplay->display());
    m_deferHltDisplay->finalize();
    m_nodeSelector = swapMouseSelector(this,m_deferHltDisplay.get(),m_subfarmDisplay);
    m_deferHltDisplay = auto_ptr<ClusterDisplay>(0);
    return WT_SUCCESS;
  }
  string dnam = strlower(selectedCluster());
  if ( !dnam.empty() ) {
    string node = "-node="+dnam;
    string svc = "-servicename="+svcPrefix()+dnam+"/ROpublish/HLT1";
    const char* argv[] = {"", svc.c_str(), "-delay=300" };
    ClusterDisplay* disp = createHltSubfarmDisplay(SUBFARM_WIDTH+20,SUBFARM_HEIGHT,m_anchorX+3,m_anchorY,3,(char**)argv);
    m_deferHltDisplay = auto_ptr<ClusterDisplay>(disp);
    m_deferHltDisplay->initialize();
    m_nodeSelector = swapMouseSelector(this,m_subfarmDisplay,m_deferHltDisplay.get());
    IocSensor::instance().send(this,CMD_UPDATE,m_deferHltDisplay.get());
  }
  return WT_SUCCESS;
}

/// Show window with buffer information of a given node
int FarmDisplayBase::showMbmWindow() {
  DisplayUpdate update(this,true);
  if ( m_mbmDisplay.get() ) {
    if ( m_helpDisplay.get() ) showHelpWindow();
    MouseSensor::instance().remove(this,m_mbmDisplay->display());
    m_mbmDisplay = auto_ptr<BufferDisplay>(0);
  }
  else if ( m_subfarmDisplay ) {
    m_mbmDisplay = auto_ptr<BufferDisplay>(new BufferDisplay(this,"MBM Monitor display",m_partition));
    m_mbmDisplay->setNode(m_subPosCursor);
    m_mbmDisplay->update(m_subfarmDisplay->data().pointer);
    m_mbmDisplay->show(m_anchorY+5,m_anchorX+12);
    MouseSensor::instance().add(this,m_mbmDisplay->display());
  }
  return WT_SUCCESS;
}

/// Show window with torrent information of a given node
int FarmDisplayBase::showTorrentNodeWindow()   {
  DisplayUpdate update(this,true);
  if ( m_torrentNodeDisplay.get() ) {
    if ( m_helpDisplay.get() ) showHelpWindow();
    MouseSensor::instance().remove(this,m_torrentNodeDisplay->display());
    m_torrentNodeDisplay = auto_ptr<InternalDisplay>(0);
    return WT_SUCCESS;
  }
  pair<string,string> node = selectedNode();
  if ( !node.second.empty() ) {
    m_torrentNodeDisplay = auto_ptr<InternalDisplay>(new TorrentNodeDisplay(this,node.first,node.second));
    m_torrentNodeDisplay->show(m_anchorY+5,m_anchorX+12);
    MouseSensor::instance().add(this,m_torrentNodeDisplay->display());
  }
  return WT_SUCCESS;
}

/// Show window with processes on a given node
int FarmDisplayBase::showProcessWindow(int flag) {
  if ( m_procDisplay.get() ) {
    if ( m_helpDisplay.get() ) showHelpWindow();
    MouseSensor::instance().remove(this,m_procDisplay->display());
    m_procDisplay = auto_ptr<ProcessDisplay>(0);
    return WT_SUCCESS;
  }
  pair<string,string> node = selectedNode();
  if ( !node.second.empty() ) {
    m_procDisplay = auto_ptr<ProcessDisplay>(new ProcessDisplay(this,node.second,node.first,flag));
    m_procDisplay->show(m_anchorY+5,m_anchorX+12);
    MouseSensor::instance().add(this,m_procDisplay->display());
  }
  return WT_SUCCESS;
}

/// Show window with CPU information of a given subfarm
int FarmDisplayBase::showCpuWindow() {
  DisplayUpdate update(this,true);
  if ( m_cpuDisplay.get() ) {
    if ( m_helpDisplay.get() ) showHelpWindow();
    MouseSensor::instance().remove(this,m_cpuDisplay->display());
    m_cpuDisplay = auto_ptr<CPUDisplay>(0);
    return WT_SUCCESS;
  }
  pair<string,string> node = selectedNode();
  if ( !node.second.empty() ) {
    m_cpuDisplay = auto_ptr<CPUDisplay>(new CPUDisplay(this,node.first,node.second));
    m_cpuDisplay->show(m_anchorY-3,m_anchorX+4);
    MouseSensor::instance().add(this,m_cpuDisplay->display());
  }
  return WT_SUCCESS;
}

/// Show window with BOOT information of a given subfarm
int FarmDisplayBase::showBootWindow() {
  DisplayUpdate update(this,true);
  if ( m_bootDisplay.get() ) {
    if ( m_helpDisplay.get() ) showHelpWindow();
    MouseSensor::instance().remove(this,m_bootDisplay->display());
    m_bootDisplay = auto_ptr<InternalDisplay>(0);
    return WT_SUCCESS;
  }
  string cluster_name = selectedCluster();
  if ( !cluster_name.empty() ) {
    m_bootDisplay = auto_ptr<InternalDisplay>(createBootDisplay(this,cluster_name));
    m_bootDisplay->show(m_anchorY+5,m_anchorX+12);
    m_bootDisplay->connect();
    MouseSensor::instance().add(this,m_bootDisplay->display());
  }
  return WT_SUCCESS;
}

/// Show window with CPU information of a given subfarm
int FarmDisplayBase::showStatsWindow() {
  DisplayUpdate update(this,true);
  string cluster_name = selectedCluster();
  if ( m_statsDisplay.get() ) {
    if ( m_helpDisplay.get() ) showHelpWindow();
    MouseSensor::instance().remove(this,m_statsDisplay->display());
    m_statsDisplay = auto_ptr<InternalDisplay>(0);
    return WT_SUCCESS;
  }
  if ( !cluster_name.empty() ) {
    m_statsDisplay = auto_ptr<InternalDisplay>(createFarmStatsDisplay(this,cluster_name));
    m_statsDisplay->show(m_anchorY,m_anchorX);
    m_statsDisplay->connect();
    MouseSensor::instance().add(this,m_statsDisplay->display());
  }
  return WT_SUCCESS;
}

/// Show window with TORRENT information of a given subfarm
int FarmDisplayBase::showTorrentWindow() {
  DisplayUpdate update(this,true);
  if ( m_torrentDisplay.get() ) {
    if ( m_helpDisplay.get() ) showHelpWindow();
    MouseSensor::instance().remove(this,m_torrentDisplay->display());
    m_torrentDisplay->finalize();
    m_nodeSelector = swapMouseSelector(this,m_torrentDisplay.get(),m_subfarmDisplay);
    m_torrentDisplay = auto_ptr<ClusterDisplay>(0);
    return WT_SUCCESS;
  }
  string cluster_name = selectedCluster();
  if ( !cluster_name.empty() )   {
    string node = "-node="+strupper(cluster_name);
    string svc  = "-servicename=/"+strupper(cluster_name)+"/TorrentInfo";
    const char* argv[] = {"",svc.c_str(), node.c_str(), "-delay=300"};
    ClusterDisplay* disp = BitTorrent::createTorrentDisplay(SUBFARM_WIDTH+20,SUBFARM_HEIGHT,m_anchorX,m_anchorY,4,(char**)argv);
    m_torrentDisplay = auto_ptr<ClusterDisplay>(disp);
    m_torrentDisplay->initialize();
    m_nodeSelector = swapMouseSelector(this,m_subfarmDisplay,m_torrentDisplay.get());
    IocSensor::instance().send(this,CMD_UPDATE,m_torrentDisplay.get());
  }
  return WT_SUCCESS;
}

/// Show window with SYSTEM information of a given subfarm
int FarmDisplayBase::showSysWindow() {
  DisplayUpdate update(this,true);
  if ( m_sysDisplay.get() ) {
    if ( m_helpDisplay.get() ) showHelpWindow();
    MouseSensor::instance().remove(this,m_sysDisplay->display());
    m_sysDisplay->finalize();
    m_nodeSelector = swapMouseSelector(this,m_sysDisplay.get(),m_subfarmDisplay);
    m_sysDisplay = auto_ptr<ClusterDisplay>(0);
    return WT_SUCCESS;
  }
  string dnam = selectedCluster();
  if ( !dnam.empty() ) {
    string node = "-node="+strupper(dnam);
    string svc  = "-servicename="+svcPrefix()+strupper(dnam)+"/TaskSupervisor/Status";
    const char* argv[] = {"", svc.c_str(), node.c_str(), "-delay=300"};
    ClusterDisplay* disp = createCtrlSubfarmDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX+3,m_anchorY,3,(char**)argv);
    m_sysDisplay = auto_ptr<ClusterDisplay>(disp);
    m_sysDisplay->initialize();
    m_nodeSelector = swapMouseSelector(this,m_subfarmDisplay,m_sysDisplay.get());
    IocSensor::instance().send(this,CMD_UPDATE,m_sysDisplay.get());
  }
  return WT_SUCCESS;
}

/// Show window with SYSTEM information of a given subfarm
int FarmDisplayBase::showReadoutWindow() {
  DisplayUpdate update(this,true);
  if ( m_roDisplay.get() ) {
    if ( m_helpDisplay.get() ) showHelpWindow();
    MouseSensor::instance().remove(this,m_roDisplay->display());
    m_roDisplay->finalize();
    m_nodeSelector = swapMouseSelector(this,m_roDisplay.get(),m_subfarmDisplay);
    m_roDisplay = auto_ptr<ClusterDisplay>(0);
    return WT_SUCCESS;
  }
  string dnam = selectedCluster();
  if ( !dnam.empty() ) {
    string svc = "-servicename="+svcPrefix()+strlower(dnam)+"/ROpublish";
    string part = "-partition="+m_partition;
    const char* argv[] = {"", svc.c_str(), part.c_str(), "-delay=300"};
    ClusterDisplay* disp = createSubfarmDisplay(SUBFARM_WIDTH,SUBFARM_HEIGHT,m_anchorX,m_anchorY,4,(char**)argv);
    m_roDisplay = auto_ptr<ClusterDisplay>(disp);
    m_roDisplay->initialize();
    m_nodeSelector = swapMouseSelector(this,m_subfarmDisplay,m_roDisplay.get());
    IocSensor::instance().send(this,CMD_UPDATE,m_roDisplay.get());
  }
  return WT_SUCCESS;
}

/// Show window to run Moore bench marks
int FarmDisplayBase::showBenchmarkWindow() {
  DisplayUpdate update(this,true);
  if ( m_benchDisplay.get() ) {
    MouseSensor::instance().remove(this,m_benchDisplay->display());
    m_benchDisplay = auto_ptr<InternalDisplay>(0);
  }
  else if ( m_sysDisplay.get() || m_mbmDisplay.get() || m_ctrlDisplay.get() || m_procDisplay.get() ) {
    pair<string,string> node = selectedNode();
    if ( !node.second.empty() ) {
      m_benchDisplay = auto_ptr<InternalDisplay>(createBenchmarkDisplay(this,1,node.second));
      m_benchDisplay->show(m_anchorY+5,m_anchorX+12);
      m_benchDisplay->connect();
      MouseSensor::instance().add(this,m_benchDisplay->display());
    }
  }
  else if ( m_subfarmDisplay ) {
    string cluster_name = selectedCluster();
    if ( !cluster_name.empty() ) {
      m_benchDisplay = auto_ptr<InternalDisplay>(createBenchmarkDisplay(this,2,cluster_name));
      m_benchDisplay->show(m_anchorY+5,m_anchorX+12);
      m_benchDisplay->connect();
      MouseSensor::instance().add(this,m_benchDisplay->display());
    }
  }
  else {
    m_benchDisplay = auto_ptr<InternalDisplay>(createBenchmarkDisplay(this,3,name()));
    m_benchDisplay->show(m_anchorY+5,m_anchorX+12);
    m_benchDisplay->connect();
    MouseSensor::instance().add(this,m_benchDisplay->display());
  }
  return WT_SUCCESS;
}

/// Handle Mouse interrupt
bool FarmDisplayBase::handleMouseEvent(const SCR::MouseEvent* m) {
  if ( m->button == 2 ) {
    IocSensor::instance().send(this,CMD_SHOWHELP,this);
    return true;
  }
  else if ( m->msec != (unsigned int)-1 ) {
    if ( m_helpDisplay.get() )
      IocSensor::instance().send(this,CMD_SHOWHELP,this);
    else if ( m_ctrlDisplay.get() )
      IocSensor::instance().send(this,CMD_SHOWCTRL,this);
    else if ( m_torrentNodeDisplay.get() )
      IocSensor::instance().send(this,CMD_SHOWTORRENTNODE,this);
    else if ( m_torrentDisplay.get() )
      IocSensor::instance().send(this,CMD_SHOWTORRENT,this);
    else if ( m_mbmDisplay.get() )
      IocSensor::instance().send(this,CMD_SHOWMBM,this);
    else if ( m_procDisplay.get() )
      IocSensor::instance().send(this,CMD_SHOWPROCS,this);
    else if ( m_cpuDisplay.get() )
      IocSensor::instance().send(this,CMD_SHOWCPU,this);
    else if ( m_bootDisplay.get() )
      IocSensor::instance().send(this,CMD_SHOWBOOT,this);
    else if ( m_statsDisplay.get() )
      IocSensor::instance().send(this,CMD_SHOWSTATS,this);
    else if ( m_sysDisplay.get() )
      IocSensor::instance().send(this,CMD_SHOWSYS,this);
    else if ( m_deferHltDisplay.get() )
      IocSensor::instance().send(this,CMD_SHOWDEFERRED,this);
    else
      IocSensor::instance().send(this,CMD_SHOWSUBFARM,this);
    return true;
  }
  return false;
}

/// Handle IOC interrupt
bool FarmDisplayBase::handleIocEvent(const CPP::Event& ev) {
  switch(ev.type) {
  case CMD_SHOWSUBFARM:
    showSubfarm();
    return true;
  case CMD_SHOWDEFERRED:
    showDeferHltWindow();
    return true;
  case CMD_SHOWTORRENT:
    showTorrentWindow();
    return true;
  case CMD_SHOWTORRENTNODE:
    showTorrentNodeWindow();
    return true;
  case CMD_SHOWSYS:
    showSysWindow();
    return true;
  case CMD_SHOWREADOUT:
    showReadoutWindow();
    return true;
  case CMD_SHOWBENCHMARK:
    showBenchmarkWindow();
    return true;
  case CMD_SHOWBOOT:
    showBootWindow();
    return true;
  case CMD_SHOWSTATS:
    showStatsWindow();
    return true;
  case CMD_SHOWCPU:
    showCpuWindow();
    return true;
  case CMD_SHOWCTRL:
    showCtrlWindow();
    return true;
  case CMD_SHOWMBM:
    if (m_mode == CTRL_MODE) showCtrlWindow();
    else if ( m_mode == TORRENT_MODE ) showTorrentNodeWindow();
    else if ( m_mode != HLTDEFER_MODE ) showMbmWindow();
    return true;
  case CMD_SHOWPROCS:
    showProcessWindow(0);
    return true;
  case CMD_SHOWHELP:
    showHelpWindow();
    return true;
  case CMD_SETCURSOR:
    set_cursor();
    return true;
  case CMD_HANDLE_KEY:
    handleKeyboard(int((long)ev.data));
    return true;
  case CMD_NOTIFY: {
    unsigned char* ptr = (unsigned char*)ev.data;
    if ( ptr ) {
      if ( m_benchDisplay.get() ) m_benchDisplay->update(ptr + sizeof(int), *(int*)ptr);
      delete [] ptr;
    }
    return true;
  }
  case CMD_DELETE:
    delete this;
    ::lib_rtl_sleep(200);
    ::exit(0);
    return true;

  default:
    return false;
  }
}

/// Handle keyboard interrupts
int FarmDisplayBase::handleKeyboard(int key)    {
  try {
    switch (key)    {
    case CTRL_W:
      ::scrc_repaint_screen (m_pasteboard);
      break;
    case 'E':
    case CTRL_E:
      delete this;
    ::exit(0);
    break;
    case 'h':
    case 'H':
    case CTRL_H:
      return showHelpWindow();
    case 'b':
    case 'B':
      return showBootWindow();
    case 'c':
    case 'C':
      return showCpuWindow();
    case 'd':
    case 'D':
      IocSensor::instance().send(this,CMD_SHOWDEFERRED,this);
    break;
    case 'k':
    case 'K':
      IocSensor::instance().send(this,CMD_SHOWCTRL,this);
    break;
    case 'l':
    case 'L':
      IocSensor::instance().send(this,CMD_SHOWSYS,this);
    break;
    case 'm':
    case 'M':
    case '.':
    case KPD_PERIOD:
      IocSensor::instance().send(this,m_sysDisplay.get()?CMD_SHOWCTRL:CMD_SHOWMBM,this);
    break;
    case CTRL_L:
      IocSensor::instance().send(this,CMD_SHOWBENCHMARK,this);
      break;
    case 'p':
    case 'P':
      return showProcessWindow(0);
    case CTRL_B:
      return showProcessWindow(1);
    case CTRL_V:
      return showProcessWindow(2);
    case CTRL_P:
      return showProcessWindow(3);
    case CTRL_N:
      return showProcessWindow(4);
    case CTRL_M:
      break;      
    case 'r':
    case 'R':
      IocSensor::instance().send(this,CMD_SHOWREADOUT,this);
    break;
    case 's':
    case 'S':
      return showStatsWindow();
    case 't':
      if ( m_torrentDisplay.get() || m_mode == TORRENT_MODE )  {
        IocSensor::instance().send(this,CMD_SHOWTORRENTNODE,this);
        break;
      }
    case 'T':
      IocSensor::instance().send(this,CMD_SHOWTORRENT,this);
      break;
    case RETURN_KEY:
    case ENTER:
      IocSensor::instance().send(this,CMD_SHOWSUBFARM,this);
      break;
    default:
      return WT_ERROR;
    }
  }
  catch(...) {
  }
  set_cursor();
  return WT_SUCCESS;
}
