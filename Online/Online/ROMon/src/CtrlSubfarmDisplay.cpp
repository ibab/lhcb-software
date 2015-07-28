// $Id: CtrlSubfarmDisplay.cpp,v 1.11 2010-10-19 15:36:26 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/CtrlSubfarmDisplay.cpp,v 1.11 2010-10-19 15:36:26 frankb Exp $

// C++ include files
#include <cstdlib>
#include <iostream>

// Framework include files
#include "RTL/Lock.h"
#include "ROMonDefs.h"
#include "ROMon/CtrlSubfarmDisplay.h"
#include "ROMon/TaskSupervisorParser.h"

#include "dic.hxx"

using namespace ROMon;
using namespace std;

static void help() {
  cout <<"  romon_storage -option [-option]" << endl
       <<"       -h[eaderheight]=<number>     Height of the header        display.                      " << endl
       <<"       -d[elay]=<number>            Time delay in millisecond between 2 updates.              " << endl
       <<"       -s[ervicename]=<name>        Name of the DIM service  providing monitoring information." << endl
       << endl;
}


/// Static abstract object creator.
ClusterDisplay* ROMon::createCtrlSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv) {
  return new CtrlSubfarmDisplay(width,height,posx,posy,argc,argv);
}

/// Standard constructor
CtrlSubfarmDisplay::CtrlSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv)
  : ClusterDisplay(width,height)
{
  m_position = Position(posx,posy);
  init(argc, argv);
}

/// Standard constructor
CtrlSubfarmDisplay::CtrlSubfarmDisplay(int argc, char** argv)   {
  init(argc, argv);
}

void CtrlSubfarmDisplay::init(int argc, char** argv)   {
  RTL::CLI cli(argc,argv,help);
  string node;
  int hdr_height;
  cli.getopt("headerheight",  1, hdr_height    =   5);
  cli.getopt("delay",         1, m_delay       = 1000);
  cli.getopt("servicename",   1, m_svcName     = "/HLTA01/TaskSupervisor/Status");
  cli.getopt("node",          1, node          = "HLTA01");
  m_readAlways = true;

  if ( !node.empty() ) {
    string svc = "HLT/ExcludedNodes/"+strupper(node);  
    m_svcID2   = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,excludedHandler,(long)this,0,0);
  }
  setup_window();
  int width    = m_area.width;
  int height   = m_area.height;
  int posx     = m_position.x-2;
  int posy     = m_position.y-2;
  m_nodes      = createSubDisplay(Position(posx,posy+hdr_height),Area(width,height-hdr_height),"Processing Cluster Information");
  m_clusterData.pointer = (char*)&m_cluster;
  showHeader();
  end_update();
}

/// Standard destructor
CtrlSubfarmDisplay::~CtrlSubfarmDisplay()  {
  m_clusterData.pointer = 0;
  begin_update();
  delete m_nodes;
  end_update();
}

/// Display the node information
void CtrlSubfarmDisplay::showNodes()  {
  char text[1024];
  float GB = 1.0/float(1024*1024*1024);
  Cluster& c = m_cluster;
  MonitorDisplay* disp = m_nodes;
  size_t taskCount=0, missTaskCount=0;
  size_t connCount=0, missConnCount=0;
  const char* fmt = " %-12s %3s %8s %5zd/%-4zd %5zd/%-5zd %4d %5d %5d %3.0f %3.0f %-19s %4.0f/%-4.0f %s";

  //disp->draw_line_reverse(" ----------------------------------   Cluster information   ----------------------------------");
  disp->draw_line_bold(   " %-12s %3s %8s    Tasks   Connections %4s %5s %5s %3s %3s %-18s %10s %10s",
                          "","","","RSS","Stack","VSize","CPU","MEM","","LocalDisk","Last");
  disp->draw_line_bold(   " %-12s %3s %8s found/miss found/miss  %4s %5s %5s %3s %3s %-18s %10s %10s",
                          "Node","","Status","[MB]","[MB]","[MB]","[%]","[%]","Boot time","[GB/GB] ","Reading");
  for(Cluster::Nodes::const_iterator i=c.nodes.begin(); i!=c.nodes.end();++i)    {
    const Cluster::Node& n = (*i).second;
    if ( n.status == "DEAD" ) {
      disp->draw_line_normal(" %-12s %3s %8s %76s",n.name.c_str(),"",n.status.c_str(),n.time.c_str());
    }
    else {
      float blk_size = float(n.blk_size != 0 ? n.blk_size : 4096)*GB;
      const char* inc_exc = m_excluded.find(n.name)==m_excluded.end() ? "INC" : "EXC";
      taskCount     += n.taskCount;
      missTaskCount += n.missTaskCount;
      connCount     += n.connCount;
      missConnCount += n.missConnCount;
      disp->draw_line_normal(fmt,n.name.c_str(),inc_exc,n.status.c_str(),
                             n.taskCount,n.missTaskCount,n.connCount,n.missConnCount,
                             int(n.rss/1024),int(n.stack/1024),int(n.vsize/1024),
                             n.perc_cpu, n.perc_mem, n.boot.c_str(),
                             float(n.blk_availible)*blk_size,float(n.blk_total)*blk_size,
                             n.time.length()>10 ? n.time.c_str()+10 : n.time.c_str()
                             );
      //                             n.perc_cpu, n.perc_mem, n.boot.substr(4,12).c_str(),n.time.c_str());
    }
  }
  disp->draw_line_normal("");
  disp->draw_line_bold(" %-12s %8s %5zd/%-7zd %5zd/%-7zd", "Total:", 
                       c.status.c_str(), taskCount, missTaskCount, connCount, missConnCount);
  disp->draw_line_normal("");
  disp->draw_line_normal("");
  for(Cluster::Nodes::const_iterator i=c.nodes.begin(); i!=c.nodes.end();++i) {
    const Cluster::Node& n = (*i).second;
    if ( n.projects.size() > 0 ) {
      disp->draw_line_bold(" %-24s %-16s %-14s %-14s %-14s %-14s %-14s",
                           "PVSS Summary/Node:", "Project name","Event Mgr","Data Mgr","Dist Mgr","FSM Server","<Project State>");
      for(Cluster::Projects::const_iterator q=n.projects.begin(); q != n.projects.end(); ++q)  {
        const Cluster::PVSSProject& p = *q;
        ::sprintf(text," %-24s %-16s %-14s %-14s %-14s %-14s %-14s",
                  n.name.c_str(), p.name.c_str(), p.state(p.eventMgr), p.state(p.dataMgr), 
                  p.state(p.distMgr), p.state(p.fsmSrv), p.ok() ? "RUNNING" : "==NOT RUNNING==");
        p.ok() ? disp->draw_line_normal(text) : disp->draw_line_bold(text);
      }
    }
  }
  disp->draw_line_normal("");
}

/// Update header information
void CtrlSubfarmDisplay::showHeader()   {
  char text[256];
  text[0] = 0;
  if ( !m_excluded.empty() ) ::sprintf(text,"%d node(s) excluded",int(m_excluded.size()));    
  draw_line_normal ("");
  draw_line_reverse("         Task control monitoring on %s   [%s]  %s", 
                    m_cluster.name.c_str(), ::lib_rtl_timestr(), text);
  draw_line_reverse("         Information service:%s data size:%zd", m_svcName.c_str(),m_data.actual);
  draw_line_normal ("");
}

/// DIM command service callback
void CtrlSubfarmDisplay::excludedHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    CtrlSubfarmDisplay* disp = *(CtrlSubfarmDisplay**)tag;
    char *p = (char*)address, *end = p+*size;
    set<string> nodes;
    while(p<end) {
      nodes.insert(strlower(p));
      p += (::strlen(p)+1);
    }
    if ( nodes.size() > 0 )  {
      RTL::Lock lock(disp->m_lock);
      disp->m_excluded = nodes;
    }
  }
}

/// Update all displays
void CtrlSubfarmDisplay::update()   {
  try {
    int result = 0;
    XML::TaskSupervisorParser ts;
    {  DimLock lock;
      if ( m_data.actual>0 ) {
        const char* ptr = m_data.data<const char>();
        result = ts.parseBuffer(m_svcName, ptr,::strlen(ptr)+1) ? 1 : 2;
      }
    }
    RTL::Lock lock(m_lock);
    begin_update();
    m_nodes->begin_update();
    showHeader();
    switch(result) {
    case 0:
      m_nodes->draw_line_normal ("");
      m_nodes->draw_line_bold("   ..... No XML information present .....");
      m_nodes->draw_line_normal ("");
      break;
    case 1:
      m_cluster.nodes.clear();
      ts.getClusterNodes(m_cluster);
      showNodes();
      break;
    case 2:
      m_nodes->draw_line_normal ("");
      m_nodes->draw_line_bold("   ..... Failed to parse XML information .....");
      m_nodes->draw_line_normal ("");
      break;
    default:
      break;
    }
  }
  catch(const exception& e) {
    m_nodes->draw_line_normal ("");
    m_nodes->draw_line_bold("   ..... Exception during data parsing: %s",e.what());
    m_nodes->draw_line_normal ("");
  }
  catch(...) {
    m_nodes->draw_line_normal ("");
    m_nodes->draw_line_bold("   ..... Exception during data parsing .....");
    m_nodes->draw_line_normal ("");
  }
  m_nodes->end_update();
  end_update();
}

/// Retrieve cluster name from cluster display
string CtrlSubfarmDisplay::clusterName() const {
  return m_cluster.name;
}

/// Retrieve node name from cluster display by offset
string CtrlSubfarmDisplay::nodeName(size_t offset) {
  size_t cnt = 0;
  const Cluster::Nodes& nodes = m_cluster.nodes;
  for (Cluster::Nodes::const_iterator n=nodes.begin(); n!=nodes.end(); ++n, ++cnt)  {
    if ( cnt == offset ) return string((*n).first);
  }
  return "";
}

extern "C" int romon_ctrlsubfarm(int argc,char** argv) {
  CtrlSubfarmDisplay disp(argc,argv);
  disp.initialize();
  disp.run();
  return 1;
}
