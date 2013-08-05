// $Id: SubfarmDisplay.cpp,v 1.19 2010-10-21 06:04:22 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/SubfarmDisplay.cpp,v 1.19 2010-10-21 06:04:22 frankb Exp $

// C++ include files
#include <cstdlib>
#include <iostream>
#include <limits>

// Framework include files
#include "RTL/Lock.h"
#include "RTL/strdef.h"
#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "ROMon/Constants.h"
#include "ROMon/SubfarmDisplay.h"
#include "dic.hxx"

using namespace ROMon;
using namespace std;

typedef Nodeset::Nodes               Nodes;
typedef Node::Buffers                Buffers;
typedef MBMBuffer::Clients           Clients;
typedef Node::Tasks                  Tasks;

namespace {
  struct Info {
    long data[5];
    Info() { 
      data[0]=data[1]=data[2]=data[3]=data[4]=0;
    }
    Info(const Info& c) { 
      data[0]=c.data[0];
      data[1]=c.data[1];
      data[2]=c.data[2];
      data[3]=c.data[3];
      data[4]=c.data[4];
    }
    Info& operator=(const Info& c)  {
      data[0]=c.data[0];
      data[1]=c.data[1];
      data[2]=c.data[2];
      data[3]=c.data[3];
      data[4]=c.data[4];
      return *this;
    }
  };
}

static void help() {
  cout <<"  romon_subfarm -option [-option]" << endl
       <<"       -h[eaderheight]=<number>     Height of the header        display.                      " << endl
       <<"       -d[elay]=<number>            Time delay in millisecond between 2 updates.              " << endl
       <<"       -p[artition]=<name>          Partition name for buffer filtering.                      " << endl
       <<"       -s[ervicename]=<name>        Name of the DIM service  providing monitoring information." << endl
       << endl;
}

/// Static abstract object creator.
ClusterDisplay* ROMon::createSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv) {
  return new SubfarmDisplay(width,height,posx,posy,argc,argv);
}

/// Standard constructor
SubfarmDisplay::SubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv)
  : ClusterDisplay(width, height), m_nodes(0)
{
  m_position = Position(posx,posy);
  init(argc, argv);
}

/// Standard constructor
SubfarmDisplay::SubfarmDisplay(int argc, char** argv) : ClusterDisplay(), m_nodes(0)   {
  init(argc, argv);
}

void SubfarmDisplay::init(int argc, char** argv)   {
  RTL::CLI cli(argc,argv,help);
  int hdr_height;

  cli.getopt("headerheight",  1, hdr_height    =    5);
  cli.getopt("delay",         1, m_delay       = 1000);
  cli.getopt("partition",     1, m_partition   = "*");
  cli.getopt("servicename",   1, m_svcName     = "/hlte07/ROpublish");
  if ( m_partition == "*" ) m_partition = "";
  if ( m_partition == "ALL" ) m_partition = "";
  if ( cli.getopt("debug", 5) ) {
    printf("To start debugger type:  gdb --pid %d\n",::lib_rtl_pid());
    ::lib_rtl_sleep(30*1000);
  }
  setup_window();
  int posx     = m_position.x-2;
  int posy     = m_position.y-2;
  m_nodes      = createSubDisplay(Position(posx,posy+hdr_height),
				  Area(m_area.width,m_area.height - hdr_height),
				  "Node Information");
  end_update();
  m_measure = 0;
}

/// Standard destructor
SubfarmDisplay::~SubfarmDisplay()  {
  begin_update();
  if ( m_nodes    ) delete m_nodes;
  end_update();
}

/// Display the node information
void SubfarmDisplay::showNodes(const Nodeset& ns)  {
  MonitorDisplay* disp = m_nodes;
  const char* fmt = " %-12s%5d %s";
  char buff_text[256], text1[256], text2[256];
  map<string,Info> totals;
  vector<string>   buffers;
  long ntsk_tot = 0;
  bool partitioned=m_partition.empty();

  ::snprintf(text1,sizeof(text1),"           - MBM -  ");
  ::snprintf(text2,sizeof(text2)," Node      Clients ");
  const char* pattern = "  ----------------------------------- ";

  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const Buffers& buffs = *(*n).buffers();
    bool doBreak = false;
    buff_text[0] = 0;
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      if ( !partitioned || strstr((*ib).name,m_partition.c_str()) ) {
	size_t len = ::strlen(text1);
	::snprintf(text1+len,sizeof(text1),"%s",pattern);
	::snprintf(text1+len+12,sizeof(text1),"%s",(*ib).name);
	text1[len+11] = ' ';
	text1[len+12+strlen((*ib).name)] = ' ';
	::snprintf(text2+len,sizeof(text2),"%11s%11s%6s%8s%2s","Produced","Consumed","Slots","Space","");
	doBreak = true;
      }
    }
    if ( doBreak ) break;
  }
  disp->draw_line_reverse(text1);
  disp->draw_line_bold(   text2);
  ++m_measure;
  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    map<string,map<string,pair<long,long> > >::iterator in = m_minimal.find((*n).name);
    const Buffers& buffs = *(*n).buffers();
    size_t ntsk = 0;
    buff_text[0] = 0;
    if ( in == m_minimal.end() ) in = m_minimal.insert(make_pair((*n).name,map<string,pair<long,long> >())).first;
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      Info info;
      if ( !partitioned || strstr((*ib).name,m_partition.c_str()) ) {
	map<string,pair<long,long> >::iterator ibuf = (*in).second.find((*ib).name);
	const Clients&             clients = (*ib).clients;
	const MBMBuffer::Control&  ctrl = (*ib).ctrl;

	if ( ibuf == (*in).second.end() ) ibuf = (*in).second.insert(make_pair((*ib).name,make_pair(0,0))).first;
	info.data[0] = ctrl.tot_produced;
	info.data[1] = ctrl.p_emax-ctrl.i_events;
	info.data[2] = (ctrl.i_space*ctrl.bytes_p_Bit)/1024;
	long min_task  = numeric_limits<long>::max();
	for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
	  ++ntsk;
	  if ( (*ic).type == 'C' ) info.data[3] += (*ic).events;
	  if ( (*ic).type == 'P' ) info.data[4] += (*ic).events;
	  if ( (*ic).events < min_task && ::strncmp((*ic).name,"MEPRx",5) != 0 )min_task = (*ic).events;
	}
	size_t len = ::strlen(buff_text);
	if ( (m_measure%5)==0 ) {
	  (*ibuf).second.second = min_task <= (*ibuf).second.first ? 1 : 0;
	  (*ibuf).second.first  = min_task;
	}
	if ( 0 == len ) buffers.push_back((*ib).name);
	for(size_t k=0; k<5; ++k) totals[(*ib).name].data[k] += info.data[k];
	::snprintf(buff_text+len,sizeof(buff_text)-len,"%11ld%11ld%6ld%8ld%s",
		   info.data[4],info.data[4],info.data[1],info.data[2],(*ibuf).second.second ? "/S" : "  ");
      }
    }
    ntsk_tot += ntsk;
    disp->draw_line_normal(fmt, (*n).name, ntsk, buff_text);
  }
  disp->draw_line_normal("");
  buff_text[0] = 0;
  for(size_t i=0; i<totals.size(); ++i)  {
    size_t len = ::strlen(buff_text);
    Info& info = totals[buffers[i]];
    ::snprintf(buff_text+len,sizeof(buff_text)-len,"%11ld%11ld%6ld%8ld  ",
	       info.data[4],info.data[4],info.data[1],info.data[2]);
  }
  disp->draw_line_bold(fmt, "Total:", ntsk_tot, buff_text);
}

/// Update header information
void SubfarmDisplay::showHeader(const Nodeset& ns)   {
  char b1[64], b2[64];
  TimeStamp frst=ns.firstUpdate(), last=ns.lastUpdate();
  time_t t1 = frst.first, t2 = last.first;
  ::strftime(b1,sizeof(b1),"%H:%M:%S",::localtime(&t1));
  ::strftime(b2,sizeof(b1),"%H:%M:%S",::localtime(&t2));
  draw_line_normal ("");
  
  draw_line_reverse("         HLT monitoring on %s   [%s]   %s %s", ns.name, ::lib_rtl_timestr(),
		    m_partition.empty() ? "" : "Partition:",
		    m_partition.empty() ? "" : m_partition.c_str());
  draw_line_bold   ("         Information updates date between: %s.%03d and %s.%03d",b1,frst.second,b2,last.second);
}

/// Number of nodes in the dataset
size_t SubfarmDisplay::numNodes()  {
  size_t n = 0;
  const Nodeset* ns = data().data<const Nodeset>();
  if ( ns ) {
    switch(ns->type)  {
    case Nodeset::TYPE:
      n = ns->nodes.size();
    default:
      break;
    }
  }
  return n;
}

/// Retrieve cluster name from cluster display
std::string SubfarmDisplay::clusterName() const {
  const Nodeset* ns = (const Nodeset*)data().pointer;
  return ns ? ns->name : "";
}

/// Retrieve node name from cluster display by offset
string SubfarmDisplay::nodeName(size_t offset) {
  const Nodeset* ns = (const Nodeset*)data().pointer;
  if ( ns ) {
    size_t cnt;
    Nodes::const_iterator n;
    const Nodes& nodes = ns->nodes;
    for (n=nodes.begin(), cnt=0; n!=nodes.end(); n=nodes.next(n), ++cnt)  {
      if ( cnt == offset ) {
        return (*n).name;
      }
    }
  }
  return "";
}

/// Update all displays
void SubfarmDisplay::updateDisplay(const Nodeset& ns)   {
  begin_update();
  m_nodes->begin_update();
  showHeader(ns);
  showNodes(ns);
  m_nodes->end_update();
  end_update();
}

extern "C" int romon_subfarm(int argc,char** argv) {
  SubfarmDisplay disp(argc,argv);
  disp.initialize();
  disp.run();
  return 1;
}
