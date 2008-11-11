// $Id: MonitoringDisplay.cpp,v 1.11 2008-11-11 15:09:26 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/MonitoringDisplay.cpp,v 1.11 2008-11-11 15:09:26 frankb Exp $

// C++ include files
#include <cstdlib>
#include <iostream>

// Framework include files
#include "GaudiKernel/strcasecmp.h"
#include "RTL/Lock.h"
#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "ROMon/MonitoringDisplay.h"
#include "dic.hxx"

using namespace ROMon;
using namespace std;

static const char *sstat[17] = {" nl", "   ", "*SL","*EV","*SP","WSL","WEV","WSP","wsl","wev","wsp"," ps"," ac", "SPR", "WER", "   "};

namespace {
  struct Stream {
    typedef map<string,int> Targets;
    int received;
    Targets to;
    const MBMBuffer* buffer;
    Stream() : received(0), buffer(0) {}
    Stream(const Stream& s) : received(s.received), to(s.to), buffer(s.buffer) {}
    Stream& operator=(const Stream& s) { received=s.received; to = s.to; buffer = s.buffer; return *this;}
  };
}

typedef Nodeset::Nodes               Nodes;
typedef Node::Buffers                Buffers;
typedef MBMBuffer::Clients           Clients;
typedef Node::Tasks                  Tasks;
typedef map<string,Stream> Streams;

static char* nullchr(char* src,char match) {
  char* p = strchr(src,match);
  if(p) *p = 0;
  return p;
}
static char* nullstr(char* src,const char* match) {
  char* p = strstr(src,match);
  if(p) *p = 0;
  return p;
}

static void help() {
  cout <<"  romon_storage -option [-option]" << endl
       <<"       -h[eaderheight]=<number>     Height of the header        display.                      " << endl
       <<"       -r[elayheight]=<number>      Height of the Relay         display.                      " << endl
       <<"       -n[odeheight]=<number>       Height of the Node          display.                      " << endl
       <<"       -d[elay]=<number>            Time delay in millisecond between 2 updates.              " << endl
       <<"       -n[oderelay]=<name>          Name of the relay node.                                   " << endl
       <<"       -s[ervicename]=<name>        Name of the DIM service  providing monitoring information." << endl
       <<"       -p[partition]=<name>         Name of the partition to be displayed.                    " << endl
       << endl;
}

/// Standard constructor
MonitoringDisplay::MonitoringDisplay(int argc, char** argv)   {
  RTL::CLI cli(argc,argv,help);
  int hdr_height, relay_height, node_height;
  cli.getopt("headerheight",  1, hdr_height    =    5);
  cli.getopt("relayheight",   1, relay_height  =    8);
  cli.getopt("nodeheight",    1, node_height   =    8);
  cli.getopt("delay",         1, m_delay       = 1000);
  cli.getopt("servicename",   1, m_svcName     = "/mona08/ROpublish");
  cli.getopt("partitionname", 1, m_partName    = "LHCb");
  cli.getopt("namerelay",     1, m_relayNode   = "mona0801");

  setup_window();
  int right = 0;
  int width = m_area.width;
  m_relay = createSubDisplay(Position(right,hdr_height), Area(width,relay_height),"Relay Information");
  m_nodes = createSubDisplay(Position(right,m_relay->bottom()-1),Area(width,node_height),"Monitoring Nodes");
  m_tasks = createSubDisplay(Position(right,m_nodes->bottom()-1),Area(width,m_area.height-m_nodes->bottom()+1),
			       "Monitoring Tasks");
  end_update();
}

/// Standard destructor
MonitoringDisplay::~MonitoringDisplay()   {
}

/// Show the file information
void MonitoringDisplay::showTasks(const Nodeset& ns) {
  int nTsk;
  char txt[2][256];
  string part = m_partName + "_";
  string part2 = "_" + m_partName;
  MonitorDisplay* disp = m_tasks;
  const char* fmt = " %-24s%4s %c%c%c%c%11d %3.0f ";
  sprintf(txt[0],   " %-23s%5s %c%c%c%c%11s %3s ","Monitoring Task","State",'R','e','q','s',"Seen","[%]");
  disp->draw_line_reverse("%-50s%-50s",txt[0],txt[0]);
  txt[0][0] = txt[1][0] = nTsk = 0;
  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    if ( ::strncasecmp((*n).name,m_relayNode.c_str(),m_relayNode.length()) != 0 ) {
      const Buffers& buffs = *(*n).buffers();
      for (Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
	string buff_nam = (*ib).name;
	const Clients& clients = (*ib).clients;
	if ( buff_nam.find(part2) == string::npos ) continue;
	for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
	  const MBMClient& c = *ic;
	  if (strncmp(c.name,part.c_str(),part.length())==0) {
	    char nam[BM_USER_NAME_LEN], *typ, *node=nam+part.length();
	    strcpy(nam,c.name);
	    if ( (typ=nullstr(nam,"_SND")) )  {}
	    else if ( (typ=nullstr(nam,"_RCV")) ) {}
	    else if ( (typ=nullchr(node,'_')) && c.type=='C' ) {
	      *typ = 0;
	      float perc=(*ib).ctrl.tot_produced>0 ? 100*(float(c.events)/(*ib).ctrl.tot_produced) : 0;
	      sprintf(txt[nTsk++],fmt,++typ,sstat[size_t(c.state)],c.reqs[0],c.reqs[1],c.reqs[2],c.reqs[3],c.events,perc);
	      if ( nTsk==2 ) {
		disp->draw_line_normal("%-50s%-50s",txt[0],txt[1]);
		txt[0][0] = txt[1][0] = nTsk = 0;
	      }
	    }
	  }
	}
      }
    }
  }
  if ( nTsk > 0 ) disp->draw_line_normal("%-50s%-50s",txt[0],txt[1]);
}

/// Show the node information
void MonitoringDisplay::showNodes(const Nodeset& ns) {
  long seen=0;
  MBMBuffer::Control total;
  MonitorDisplay* disp = m_nodes;
  string part = m_partName + "_";
  string part2 = "_" + m_partName;
  const char* fmt = " %-10s %-12s %12d%12d%12d%12d%12d%12d";

  ::memset(&total,0,sizeof(total));
  disp->draw_line_reverse(" %-10s %-12s %12s%12s%12s%12s%12s%12s",
			  "Node","Buffer","Received","Produced","Pending","Space[kB]","Free Slots","Free Users");
  
  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    if ( ::strncasecmp((*n).name,m_relayNode.c_str(),m_relayNode.length()) != 0 ) {
      const Buffers& buffs = *(*n).buffers();
      for (Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
	const MBMBuffer::Control& ctrl = (*ib).ctrl;
	const Clients& clients = (*ib).clients;
	string buff_nam = (*ib).name;
	if ( buff_nam.find(part2) == string::npos ) continue;
	for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
	  const MBMClient& c = *ic;
	  if (strncmp(c.name,part.c_str(),part.length())==0) {
	    char nam[BM_USER_NAME_LEN],*typ,*node=nam+part.length();
	    ::strcpy(nam,c.name);
	    if ( (typ=nullstr(nam,"_RCV")) )  {
	      disp->draw_line_normal(fmt,node,buff_nam.c_str(),c.events,ctrl.tot_produced,ctrl.i_events,
				     (ctrl.i_space*ctrl.bytes_p_Bit)/1024,
				     ctrl.p_emax-ctrl.i_events,ctrl.p_umax-ctrl.i_users);
	      seen += c.events;
	      total.tot_produced += ctrl.tot_produced;
	      total.i_events     += ctrl.i_events;
	      total.i_users      += ctrl.i_users;
	      total.p_emax       += ctrl.p_emax;
	      total.p_umax       += ctrl.p_umax;
	      total.bm_size      += (ctrl.bm_size*ctrl.bytes_p_Bit)/1024;
	      total.i_space      += (ctrl.i_space*ctrl.bytes_p_Bit)/1024;
	    }
	  }
	}
      }
    }
  }
  disp->draw_line_bold(fmt,"Total","(all)",seen,total.tot_produced,total.i_events,
		       total.i_space,total.p_emax-total.i_events,total.p_umax-total.i_users);
}

/// Show the relay information
void MonitoringDisplay::showRelay(const Nodeset& ns) {
  int nStr;
  char txt[2][256];
  int received = 0, sent = 0, nlines = 0;
  Stream all;
  Streams streams;
  MonitorDisplay* disp = m_relay;
  string part = m_partName + "_";
  string part2 = "_" + m_partName;

  disp->draw_line_reverse(" %-10s %-12s %11s%12s%11s%11s%10s%6s%6s",
			  "Node","Buffer","Received","Produced","Pending",
			  "Sent","Free[kB]","Slots","Users");
  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    if ( ::strncasecmp((*n).name,m_relayNode.c_str(),m_relayNode.length()) == 0 ) {
      const Buffers& buffs = *(*n).buffers();
      for (Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
	string buff_nam = (*ib).name;
	if ( buff_nam.find(part2.c_str()) != string::npos )  {
	  const MBMBuffer::Control& c = (*ib).ctrl;
	  const Clients& clients = (*ib).clients;
	  for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
	    const MBMClient& cl = *ic;
	    if (strncmp(cl.name,part.c_str(),part.length())==0) {
	      char nam[BM_USER_NAME_LEN], *typ, *ptr, *str;
	      strcpy(nam,cl.name);
	      if ( (typ=nullstr(nam,"_RCV")) )  {
		*typ = 0;
		str=typ+4;
		received += cl.events;
		streams[str].received  = cl.events;
		streams[str].buffer    = &(*ib);
	      }
	      else if ( (typ=nullstr(nam,"_SND")) ) {
		str=typ+4;
		ptr = nullchr(typ+4,'_');
		streams[str].to[++ptr] = cl.events;
		streams[str].buffer    = &(*ib);
	      }
	    }
	  }
	  nlines++;
	  disp->draw_line_normal(" %-10s %-12s %11d%12d%11d%11d%10d%6d%6d",
				 (*n).name,buff_nam.c_str(),received,c.tot_produced,c.i_events,received,
				 (c.i_space*c.bytes_p_Bit)/1024,c.p_emax-c.i_events,c.i_users);
	}
      }
    }
  }
  if ( 0 == nlines ) {
    ::memset(txt[0],'-',sizeof(txt[0]));
    txt[0][sizeof(txt[0])-1]=0;
    disp->draw_line_normal("%-50s%-50s",txt[0],txt[0]);
  }
  sprintf(txt[0]," %-10s %-10s %11s%11s","Stream","Node","Received","Sent");
  disp->draw_line_reverse("%-50s%-50s",txt[0],txt[0]);
  txt[0][0] = txt[1][0] = nStr = received = sent = 0;
  for (Streams::const_iterator is=streams.begin(); is!=streams.end(); ++is)  {
    bool first = true;
    const Stream& s = (*is).second;
    for (Stream::Targets::const_iterator t=s.to.begin(), t1=t; t!=s.to.end(); ++t, first=false)  {
      sprintf(txt[nStr++]," %-10s %-10s %11d%11d",(*is).first.c_str(),(*t).first.c_str(),s.received,(*t).second);
      sent += (*t).second;
      if ( nStr==2 ) {
	disp->draw_line_normal("%-50s%-50s",txt[0],txt[1]);
	txt[0][0] = txt[1][0] = nStr = 0;
      }
    }
    received += s.received;
  }
  if ( nStr > 0 ) disp->draw_line_normal("%-50s%-50s",txt[0],txt[1]);
  disp->draw_line_bold(" Total events received:%11d   Distributed:%10d to %ld Nodes",
		  received,sent,streams.size());
}

/// Update header information
void MonitoringDisplay::showHeader(const Nodeset& ns)   {
  char b1[64], b2[64];
  Nodeset::TimeStamp frst=ns.firstUpdate(), last=ns.lastUpdate();
  time_t t1 = frst.first, t2 = last.first;
  ::strftime(b1,sizeof(b1),"%H:%M:%S",::localtime(&t1));
  ::strftime(b2,sizeof(b1),"%H:%M:%S",::localtime(&t2));
  draw_line_normal ("");
  draw_line_reverse("         Monitoring for partition %s on %s   [%s]",
		    m_partName.c_str(), RTL::nodeNameShort().c_str(), ::lib_rtl_timestr());    
  draw_line_bold   ("         Information updates date between: %s.%03d and %s.%03d",b1,frst.second,b2,last.second);
}

/// Update all displays
void MonitoringDisplay::updateDisplay(const Nodeset& ns)   {
  begin_update();
  m_relay->begin_update();
  m_nodes->begin_update();
  m_tasks->begin_update();

  showHeader(ns);
  showRelay(ns);
  showNodes(ns);
  showTasks(ns);

  m_tasks->end_update();
  m_nodes->end_update();
  m_relay->end_update();
  end_update();
}

extern "C" int romon_monitoring(int argc,char** argv) {
  MonitoringDisplay disp(argc,argv);
  disp.initialize();
  disp.run();
  return 1;
}
