// $Id: MonitoringDisplay.cpp,v 1.18 2010-09-17 09:47:12 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/MonitoringDisplay.cpp,v 1.18 2010-09-17 09:47:12 frankb Exp $

// C++ include files
#include <cstdlib>
#include <iostream>
#include <set>

// Framework include files
#include "RTL/Lock.h"
#include "RTL/strdef.h"
#include "ROMonDefs.h"
#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "ROMon/MonitoringDisplay.h"
#include "dic.hxx"

using namespace ROMon;
using namespace std;

static const char *sstat[17] = {" nl", "   ", "*SL","*EV","*SP","WSL","WEV","WSP","wsl","wev","wsp"," ps"," ac", "SPR", "WER", "   "};

typedef Nodeset::Nodes               Nodes;
typedef Node::Buffers                Buffers;
typedef MBMBuffer::Clients           Clients;
typedef Node::Tasks                  Tasks;

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
       <<"       -no[deheight]=<number>       Height of the Node          display.                      " << endl
       <<"       -d[elay]=<number>            Time delay in millisecond between 2 updates.              " << endl
       <<"       -na[merelay]=<name>          Name of the relay node.                                   " << endl
       <<"       -s[ervicename]=<name>        Name of the DIM service  providing monitoring information." << endl
       <<"       -p[partition]=<name>         Name of the partition to be displayed.                    " << endl
       << endl;
}

/// Static abstract object creator.
ClusterDisplay* ROMon::createMonitoringDisplay(int width, int height, int posx, int posy, int argc, char** argv) {
  return new MonitoringDisplay(width,height,posx,posy,argc,argv);
}

/// Initializing constructor for using display as sub-display
MonitoringDisplay::MonitoringDisplay(int width, int height, int posx, int posy, int argc, char** argv)
  : ClusterDisplay(width,height+15)
{
  m_position = Position(posx,posy);
  init(1, argc, argv);
}

/// Standard constructor
MonitoringDisplay::MonitoringDisplay(int argc, char** argv) {
  init(0, argc,argv);
}

/// Initialize the display
void MonitoringDisplay::init(int flag, int argc, char** argv)   {
  RTL::CLI cli(argc,argv,help);
  int hdr_height = 5, relay_height = 5, node_height = 10;
  cli.getopt("headerheight",  1, hdr_height);
  cli.getopt("relayheight",   1, relay_height);
  cli.getopt("nodeheight",    2, node_height);
  cli.getopt("delay",         1, m_delay       = 1000);
  cli.getopt("servicename",   1, m_svcName     = "/mona08/ROpublish");
  cli.getopt("partitionname", 1, m_partName    = "LHCb");
  cli.getopt("namerelay",     2, m_relayNode   = "mona0801");

  m_select = 0;
  setup_window();
  int posx  = m_position.x-2;
  int posy  = m_position.y-2;
  int right = posx;
  int width = m_area.width;

  if ( flag ) {
    m_select  = createSubDisplay(Position(posx,posy+hdr_height),Area(10, m_area.height),"Select");
    right += 11;
    width -= 11;
  }

  m_relay = createSubDisplay(Position(right,posy+hdr_height), Area(width,relay_height),"Relay Information");
  m_nodes = createSubDisplay(Position(right,m_relay->bottom()-1),Area(width,node_height),"Monitoring Nodes");
  m_tasks = createSubDisplay(Position(right,m_nodes->bottom()-1),Area(width,m_area.height-m_nodes->bottom()+1),
                             "Monitoring Tasks");
  end_update();
}

/// Standard destructor
MonitoringDisplay::~MonitoringDisplay()   {
  begin_update();
  if ( m_select ) delete m_select;
  delete m_relay;
  delete m_nodes;
  delete m_tasks;
  end_update();
}

/// Number of nodes in the dataset
size_t MonitoringDisplay::numNodes() {
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
std::string MonitoringDisplay::clusterName() const {
  return m_relayNode.substr(0,m_relayNode.length()-2);
}

/// Retrieve node name from cluster display by offset
std::string MonitoringDisplay::nodeName(size_t offset) {
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

/// Access Node display
MonitorDisplay* MonitoringDisplay::nodeDisplay() const {
  return m_select;
}

/// Show the file information
void MonitoringDisplay::showTasks(const Nodeset& ns) {
  int nTsk;
  char txt[2][256];
  string part = m_partName + "_";
  MonitorDisplay* disp = m_tasks;
  const char* fmt = " %-24s%4s %c%c%c%c%11d %3.0f ";
  sprintf(txt[0],   " %-23s%5s %c%c%c%c%11s %3s ","Monitoring Task","State",'R','e','q','s',"Seen","[%]");
  disp->draw_line_reverse("%-50s%-50s",txt[0],txt[0]);
  txt[0][0] = txt[1][0] = 0;
  nTsk = 0;
  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    if ( ::str_ncasecmp((*n).name,m_relayNode.c_str(),m_relayNode.length()) != 0 ) {
      const Buffers& buffs = *(*n).buffers();
      for (Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
        if ( !ro_match_end(m_partName,(*ib).name) ) continue;
        string buff_nam = (*ib).name;
        const Clients& clients = (*ib).clients;
        for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
          const MBMClient& c = *ic;
          if (strncmp(c.name,part.c_str(),part.length())==0) {
            char nam[BM_USER_NAME_LEN], node[128], *typ;
            strcpy(nam,c.name);
            strcpy(node,nam+part.length());
            if ( (typ=nullstr(nam,"_SND")) )  {}
            else if ( (typ=nullstr(nam,"_RCV")) ) {}
            else if ( c.type=='C' && (typ=strchr(node,'_')) )   {
              if ( typ>node ) typ -= 3;  // Keep the last 2 digits of the node name
              *typ = 0;
              float perc=(*ib).ctrl.tot_produced>0 ? 100*(float(c.events)/(*ib).ctrl.tot_produced) : 0;
              sprintf(txt[nTsk++],fmt,++typ,sstat[size_t(c.state)],c.reqs[0],c.reqs[1],c.reqs[2],c.reqs[3],c.events,perc);
              if ( nTsk==2 ) {
                disp->draw_line_normal("%-50s%-50s",txt[0],txt[1]);
                txt[0][0] = txt[1][0] = 0;
                nTsk = 0;
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
    if ( ::str_ncasecmp((*n).name,m_relayNode.c_str(),m_relayNode.length()) != 0 ) {
      const Buffers& buffs = *(*n).buffers();
      for (Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
        if ( !ro_match_end(m_partName,(*ib).name) ) continue;
        const MBMBuffer::Control& ctrl = (*ib).ctrl;
        const Clients& clients = (*ib).clients;
        string buff_nam = (*ib).name;
        for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
          const MBMClient& c = *ic;
          if (strncmp(c.name,part.c_str(),part.length())==0) {
            char nam[BM_USER_NAME_LEN],*node=nam+part.length();
            ::strcpy(nam,c.name);
            if ( nullstr(nam,"_RCV") )  {
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
  map<string,Stream> streams;
  MonitorDisplay* disp = m_relay;
  string part = m_partName + "_";
  string part2 = "_" + m_partName;
  bool is_reco = false;

  disp->draw_line_reverse(" %-14s %-12s %11s%12s%11s%11s%10s%6s%6s",
                          "Node","Buffer","Received","Produced","Pending",
                          "Sent","Free[kB]","Slots","Users");
  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    if ( ::str_ncasecmp((*n).name,m_relayNode.c_str(),m_relayNode.length()) == 0 ) {
      is_reco = ::str_ncasecmp((*n).name,"mona09",6) == 0;
      const Buffers& buffs = *(*n).buffers();
      for (Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
        if ( !ro_match_end(m_partName,(*ib).name) ) continue;
        string buff_nam = (*ib).name;
        if ( buff_nam.find(part2) != string::npos )  {
          const MBMBuffer::Control& c = (*ib).ctrl;
          const Clients& clients = (*ib).clients;
          for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
            const MBMClient& cl = *ic;
            if (strncmp(cl.name,part.c_str(),part.length())==0) {
              char nam[BM_USER_NAME_LEN], *typ, *ptr, *str;
              strcpy(nam,cl.name);
              if ( (typ=nullstr(nam,"_RCV")) )  {
                str=typ+4;
                received += cl.events;
                streams[str].received += cl.events;
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
          disp->draw_line_normal(" %-14s %-12s %11d%12d%11d%11d%10d%6d%6d",
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
  set<string> to_nodes;
  sprintf(txt[0]," %-14s %-10s %11s%11s","Stream","Node",is_reco ? "Sent" : "Received",is_reco ? "" : "Sent");
  disp->draw_line_reverse("%-50s%-50s",txt[0],txt[0]);
  txt[0][0] = txt[1][0] = 0;
  nStr = received = sent = 0;
  for (map<string,Stream>::const_iterator is=streams.begin(); is!=streams.end(); ++is)  {
    const Stream& s = (*is).second;
    for( map<string,int>::const_iterator t=s.to.begin(); t!=s.to.end(); ++t)  {
      if ( is_reco )
        sprintf(txt[nStr++]," %-14s %-10s %11d%11s",(*is).first.c_str(),(*t).first.c_str(),int((*t).second),"");
      else
        sprintf(txt[nStr++]," %-14s %-10s %11d%11d",(*is).first.c_str(),(*t).first.c_str(),int(s.received),int((*t).second));
      sent += (*t).second;
      to_nodes.insert((*t).first);
      if ( nStr==2 ) {
        disp->draw_line_normal("%-50s%-50s",txt[0],txt[1]);
        txt[0][0] = txt[1][0] = 0;
        nStr = 0;
      }
    }
    received += s.received;
  }
  if ( nStr > 0 ) disp->draw_line_normal("%-50s%-50s",txt[0],txt[1]);
  disp->draw_line_bold(" %-26s%11d   Distributed:%10d to %ld Nodes",
                       "Total events received:",received,sent,to_nodes.size());
}

/// Update header information
void MonitoringDisplay::showHeader(const Nodeset& ns)   {
  char b1[64], b2[64];
  TimeStamp frst=ns.firstUpdate(), last=ns.lastUpdate();
  time_t t1 = frst.first, t2 = last.first;
  ::strftime(b1,sizeof(b1),"%H:%M:%S",::localtime(&t1));
  ::strftime(b2,sizeof(b1),"%H:%M:%S",::localtime(&t2));
  draw_line_normal ("");
  draw_line_reverse("         Monitoring for partition %s on %s   [%s]",
                    m_partName.c_str(), RTL::nodeNameShort().c_str(), ::lib_rtl_timestr());    
  draw_line_bold   ("         Information updates date between: %s.%03d and %s.%03d",b1,frst.second,b2,last.second);
}

/// Update selection window information
void MonitoringDisplay::showSelect(const Nodeset& ns)   {
  if ( m_select ) {
    string part2 = "_" + m_partName;
    std::vector<std::string>::const_iterator i;
    m_select->begin_update();
    m_select->draw_line_normal("");
    m_select->draw_line_normal("");
    for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
      const Buffers& buffs = *(*n).buffers();
      bool filled = false;
      for (Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
        string buff_nam = (*ib).name;
        if ( buff_nam.find(part2) != string::npos )  {
          m_select->draw_line_bold(" %s",(*n).name);
          filled = true;
          break;
        }
      }
      if ( !filled ) {
        m_select->draw_line_normal(" %s",(*n).name);
      }
    }
    m_select->draw_line_normal("");
    m_select->draw_line_normal("");
    m_select->draw_line_normal("---------");
    m_select->draw_line_normal("BOLD=used");
    m_select->draw_line_normal("---------");
    m_select->draw_line_normal("select   ");
    m_select->draw_line_normal("node with");
    m_select->draw_line_normal("         ");
    m_select->draw_line_normal(" Cursor  ");
    m_select->draw_line_normal(" - or -  ");
    m_select->draw_line_normal(" Mouse   ");
    m_select->draw_line_normal("---------");
    m_select->draw_line_normal("Help: C-H");
    m_select->draw_line_normal("Exit: C-E");
    m_select->draw_line_normal("---------");
    m_select->draw_line_normal("");
    m_select->end_update();
  }
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
  showSelect(ns);
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
