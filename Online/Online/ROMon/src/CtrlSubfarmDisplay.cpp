// $Id: CtrlSubfarmDisplay.cpp,v 1.8 2010-02-22 13:05:11 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/CtrlSubfarmDisplay.cpp,v 1.8 2010-02-22 13:05:11 frankb Exp $

// C++ include files
#include <cstdlib>
#include <iostream>

// Framework include files
#include "RTL/Lock.h"
#include "ROMon/CtrlSubfarmDisplay.h"
#include "TaskSupervisorParser.h"

#include "dic.hxx"

using namespace ROMon;
static void help() {
  std::cout <<"  romon_storage -option [-option]" << std::endl
            <<"       -h[eaderheight]=<number>     Height of the header        display.                      " << std::endl
            <<"       -d[elay]=<number>            Time delay in millisecond between 2 updates.              " << std::endl
            <<"       -s[ervicename]=<name>        Name of the DIM service  providing monitoring information." << std::endl
            << std::endl;
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
  int hdr_height;
  cli.getopt("headerheight",  1, hdr_height    =   5);
  cli.getopt("delay",         1, m_delay       = 1000);
  cli.getopt("servicename",   1, m_svcName     = "/HLTE01/TaskSupervisor/Status");
  m_readAlways = true;
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
  Cluster& c = m_cluster;
  MonitorDisplay* disp = m_nodes;
  size_t taskCount=0, missTaskCount=0;
  size_t connCount=0, missConnCount=0;
  const char* fmt = " %-12s %8s %5zd/%-7zd %5zd/%-7zd %6d %6d %6d %3.0f %3.0f %-19s %s";

  //disp->draw_line_reverse(" ----------------------------------   Cluster information   ----------------------------------");
  disp->draw_line_bold(   " %-12s %8s    Tasks       Connections  %6s %6s %6s %3s %3s %-19s %s",
                          "","","RSS","Stack","VSize","CPU","MEM","","");
  disp->draw_line_bold(   " %-12s %8s found/missing found/missing %6s %6s %6s %3s %3s %-19s %s",
                          "Node","Status","[MB]","[MB]","[MB]","[%]","[%]","Boot time","Timestamp");
  for(Cluster::Nodes::const_iterator i=c.nodes.begin(); i!=c.nodes.end();++i) {
    const Cluster::Node& n = (*i).second;
    if ( n.status == "DEAD" ) {
      disp->draw_line_normal(" %-12s %8s %76s",n.name.c_str(),n.status.c_str(),n.time.c_str());
    }
    else {
      taskCount     += n.taskCount;
      missTaskCount += n.missTaskCount;
      connCount     += n.connCount;
      missConnCount += n.missConnCount;
      disp->draw_line_normal(fmt,n.name.c_str(),n.status.c_str(),
                             n.taskCount,n.missTaskCount,n.connCount,n.missConnCount,
                             int(n.rss/1024),int(n.stack/1024),int(n.vsize/1024),
                             n.perc_cpu, n.perc_mem, n.boot.c_str(),n.time.c_str());
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
			   "PVSS Summary/Node:", "Project name","Event Mgr","Data Mgr","Dist Mgr","FSM Server","Dev Handler");
      for(Cluster::Projects::const_iterator q=n.projects.begin(); q != n.projects.end(); ++q)  {
	const Cluster::PVSSProject& p = *q;
        ::sprintf(text," %-24s %-16s %-14s %-14s %-14s %-14s %-14s",
		  n.name.c_str(), p.name.c_str(), 
		  p.eventMgr ? "RUNNING" : "DEAD",
		  p.dataMgr  ? "RUNNING" : "DEAD",
		  p.distMgr  ? "RUNNING" : "DEAD",
		  p.fsmSrv   ? "RUNNING" : "DEAD",
		  p.devHdlr  ? "RUNNING" : "DEAD");
	if ( p.eventMgr && p.dataMgr && p.distMgr )
	  disp->draw_line_normal(text);
	else
	  disp->draw_line_bold(text);
      }
    }
  }
  disp->draw_line_normal("");
}

/// Update header information
void CtrlSubfarmDisplay::showHeader()   {
  draw_line_normal ("");
  draw_line_reverse("         Task control monitoring on %s   [%s]", m_cluster.name.c_str(), ::lib_rtl_timestr());    
  draw_line_reverse("         Information service:%s data size:%zd", m_svcName.c_str(),m_data.actual);
  draw_line_normal ("");
}

/// Update all displays
void CtrlSubfarmDisplay::update()   {
  try {
    int result = 0;
    XML::TaskSupervisorParser ts;
    dim_lock();
    if ( m_data.actual>0 ) {
      const char* ptr = m_data.data<const char>();
      result = ts.parseBuffer(m_svcName, ptr,::strlen(ptr)+1) ? 1 : 2;
      //printf("XML:%d\n%s\n\n",result,ptr);
      //::exit(0);
    }
    dim_unlock();
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
  catch(const std::exception& e) {
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
std::string CtrlSubfarmDisplay::clusterName() const {
  return m_cluster.name;
}

/// Retrieve node name from cluster display by offset
std::string CtrlSubfarmDisplay::nodeName(size_t offset) {
  size_t cnt = 0;
  const Cluster::Nodes& nodes = m_cluster.nodes;
  //::printf("\n\nSubfarm display:%ld %ld %ld\n\n",offset,nodes.size(),cnt);
  for (Cluster::Nodes::const_iterator n=nodes.begin(); n!=nodes.end(); ++n, ++cnt)  {
    //::printf("-->%s %s\n",(*n).first.c_str(),(*n).second.name.c_str());
    if ( cnt == offset ) return std::string((*n).first);
  }
  return "";
}

extern "C" int romon_ctrlsubfarm(int argc,char** argv) {
  CtrlSubfarmDisplay disp(argc,argv);
  disp.initialize();
  disp.run();
  return 1;
}
