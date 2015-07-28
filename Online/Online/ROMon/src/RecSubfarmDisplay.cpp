// $Id: RecSubfarmDisplay.cpp,v 1.9 2010-09-17 09:47:12 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/RecSubfarmDisplay.cpp,v 1.9 2010-09-17 09:47:12 frankb Exp $

// C++ include files
#include <cstdlib>
#include <iostream>

// Framework include files
#include "RTL/Lock.h"
#include "RTL/strdef.h"
#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "ROMon/RecSubfarmDisplay.h"
#include "dic.hxx"

using namespace ROMon;
using namespace std;

static const char *sstat[17] = {" nl", "   ", "*SL","*EV","*SP","WSL","WEV","WSP","wsl","wev","wsp"," ps"," ac", "SPR", "WER", "   "};

typedef Nodeset::Nodes               Nodes;
typedef Node::Buffers                Buffers;
typedef MBMBuffer::Clients           Clients;
typedef Node::Tasks                  Tasks;

#define RECEIVER_TASK 'R'
#define SENDER_TASK   'S'
#define RECO_TASK     'B'

#define INPUT_BUFF   'I'
#define OUTPUT_BUFF  'O'

static void getBuffInfo(const MBMBuffer::Control& c, int info[3], int tot[3]) {
  tot[0] += (info[0] = c.tot_produced);
  tot[1] += (info[1] = c.p_emax-c.i_events);
  tot[2] += (info[2] = (c.i_space*c.bytes_p_Bit)/1024);
  return;
}

static void help() {
  cout <<"  romon_storage -option [-option]" << endl
       <<"       -h[eaderheight]=<number>     Height of the header        display.                      " << endl
       <<"       -n[odesheight]=<number>      Height of the Nodes         display.                      " << endl
       <<"       -m[ooresheight]=<number>     Height of the MOORE tasks   display.                      " << endl
       <<"       -d[elay]=<number>            Time delay in millisecond between 2 updates.              " << endl
       <<"       -s[ervicename]=<name>        Name of the DIM service  providing monitoring information." << endl
       << endl;

}

/// Static abstract object creator.
ClusterDisplay* ROMon::createRecSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv) {
  return new RecSubfarmDisplay(width,height,posx,posy,argc,argv);
}

/// Standard constructor
RecSubfarmDisplay::RecSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv)
  : ClusterDisplay(width, height)
{
  m_position = Position(posx,posy);
  init(argc, argv);
}

/// Standard constructor
RecSubfarmDisplay::RecSubfarmDisplay(int argc, char** argv)   {
  init(argc, argv);
}

void RecSubfarmDisplay::init(int argc, char** argv)   {
  RTL::CLI cli(argc,argv,help);
  int hdr_height, nodes_height, moores_height;
  cli.getopt("headerheight",  1, hdr_height    =   5);
  cli.getopt("nodesheight",   1, nodes_height  =  20);
  cli.getopt("mooresheight",  1, moores_height =  10);
  cli.getopt("delay",         1, m_delay       = 1000);
  cli.getopt("servicename",   1, m_svcName     = "/hlta01/ROpublish");

  setup_window();
  int width    = m_area.width;
  int eb_width = width/2;
  int posx     = m_position.x-2;
  int posy     = m_position.y-2;
  m_nodes      = createSubDisplay(Position(posx,posy+hdr_height),         Area(width,nodes_height),"Node Information");
  m_reco       = createSubDisplay(Position(posx,m_nodes->bottom()),       Area(width,moores_height),"Data Processing Processes");
  m_receivers  = createSubDisplay(Position(posx,m_reco->bottom()),        Area(eb_width,posy+m_area.height-m_reco->bottom()),"Event Receivers");
  m_senders    = createSubDisplay(Position(posx+eb_width,m_reco->bottom()),Area(width-eb_width,posy+m_area.height-m_reco->bottom()),"Event Senders");
  end_update();
}

/// Standard destructor
RecSubfarmDisplay::~RecSubfarmDisplay()  {
  begin_update();
  delete m_nodes;
  delete m_reco;
  delete m_receivers;
  delete m_senders;
  end_update();
}

/// Display the node information
void RecSubfarmDisplay::showNodes(const Nodeset& ns)  {
  MonitorDisplay* disp = m_nodes;
  const char* p;
  const char* fmt = " %-12s%5d %12d%11d%6d%7d %12d%6d%7d %12d";
  int evt_tot[3] = {0,0,0}, res_tot[3] = {0,0,0}, accept_tot=0, cons_tot=0, rcv_tot=0, ntsk_tot=0;

  disp->draw_line_reverse("                     ------------- INPUT --------------   ------- OUTPUT --------");
  disp->draw_line_bold(   " Node        Tasks     Produced   Consumed Slots  Space     Produced Slots  Space    Processed");

  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    int evt_info[3] = {0,0,0}, res_info[3] = {0,0,0}, out=0, received=0, consumed=0, ntsk=0;
    const Buffers& buffs = *(*n).buffers();
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      char b = (*ib).name[0];
      const Clients& clients = (*ib).clients;
      switch(b) {
      case OUTPUT_BUFF: getBuffInfo((*ib).ctrl, res_info, res_tot); break;
      case INPUT_BUFF:  getBuffInfo((*ib).ctrl, evt_info, evt_tot); break;
      default:                                                      break;
      }
      for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
        ++ntsk;
        p = ::strchr((*ic).name,'_');
        if ( p ) p = ::strchr(++p,'_');
        if ( p ) {
          if ( b==INPUT_BUFF && p[4] == RECO_TASK )  {
            consumed += (*ic).events;
            cons_tot += (*ic).events;
          }
          else if ( b==INPUT_BUFF && p[8] == RECEIVER_TASK )  {
            received += (*ic).events;
            rcv_tot += (*ic).events;
          }
          else if ( b==OUTPUT_BUFF && p[8] == SENDER_TASK )  {
            out   += (*ic).events;
            accept_tot += (*ic).events;
          }
        }
      }
    }
    disp->draw_line_normal(fmt, (*n).name, ntsk, evt_info[0], consumed, evt_info[1], evt_info[2],
                           res_info[0], res_info[1], res_info[2], out);
  }
  disp->draw_line_normal("");
  disp->draw_line_bold(fmt, "Total:", ntsk_tot, evt_tot[0], cons_tot, evt_tot[1], evt_tot[2],
                       res_tot[0], res_tot[1], res_tot[2], accept_tot);
}

/// Show the event builder information
void RecSubfarmDisplay::showTasks(const Nodeset& ns) {
  map<string,TaskIO> moores;
  string nam;
  char txt[1024];
  int nTsk = 0;
  int eb_width = m_area.width/2;

  m_receivers->draw_line_reverse (" EVENT RECEIVER         Produced [%] INPUT     ");
  m_senders->draw_line_reverse  (" EVENT SENDER            Sent Pend OUTPUT      ");

  ::memset(txt,' ',sizeof(txt));
  for(int i=0; i<2;++i ) {
    ::sprintf(txt+eb_width*i," DATA PROCESSOR                 Seen Reconstructed INPUT OUTPUT ");
    txt[strlen(txt)] = ' ';
  }
  txt[2*eb_width+3] = 0;
  m_reco->draw_line_reverse(txt);

  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const Buffers& buffs = *(*n).buffers();
    string prod_name;
    TaskIO prod;
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      const Buffers::value_type::Control& c = (*ib).ctrl;
      const Clients& clients = (*ib).clients;
      char b = (*ib).name[0];
      for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
        Clients::const_reference cl = (*ic);
        const char* p = ::strchr(cl.name,'_');
        if ( p ) p = ::strchr(++p,'_');
        if ( p ) {
          nam = (*n).name;
          nam += '_';
          if(b==INPUT_BUFF && p[4] == RECO_TASK)  {
            nam += p+4;
            moores[nam].in += cl.events;
            moores[nam].st_in = cl.state;
          }
          else if(b==OUTPUT_BUFF && p[4] == RECO_TASK)  {
            nam += p+4;
            moores[nam].out += cl.events;
            moores[nam].st_out = cl.state;
          }
          else if(b==INPUT_BUFF && p[8] == RECEIVER_TASK) {
            nam += p+8;
            float perc = c.tot_produced>0 ? 100*float(cl.events)/float(c.tot_produced) : 0;
            m_receivers->draw_line_normal(" %-18s %12d %3.0f %5s",nam.c_str(),cl.events,perc,sstat[size_t(cl.state)]);
            continue;
          }
          else if(b==OUTPUT_BUFF && p[8] == SENDER_TASK)  {
            nam += p+8;
            m_senders->draw_line_normal( " %-18s %9d%5d %6s        ",nam.c_str(),cl.events,c.i_events,sstat[size_t(cl.state)]);
          }
        }
      }
    }
  }
  ::memset(txt,' ',sizeof(txt));
  txt[m_area.width] = 0;
  for(map<string,TaskIO>::const_iterator i=moores.begin(); i!=moores.end();++i) {
    const TaskIO& m = (*i).second;
    ::sprintf(txt+eb_width*nTsk," %-18s%11d%14d %5s %6s   ",(*i).first.c_str(),m.in,m.out,sstat[m.st_in],sstat[m.st_out]);
    if ( ++nTsk == 2 ) {
      txt[2*eb_width+3] = 0;
      m_reco->draw_line_normal(txt);
      ::memset(txt,' ',sizeof(txt));
      txt[sizeof(txt)-1] = 0;
      nTsk = 0;
    }
    else {
      txt[strlen(txt)] = ' ';
    }
  }
  txt[m_area.width] = 0;
  if ( nTsk>0 ) m_reco->draw_line_normal(txt);
}

/// Update header information
void RecSubfarmDisplay::showHeader(const Nodeset& ns)   {
  char b1[64], b2[64];
  TimeStamp frst=ns.firstUpdate(), last=ns.lastUpdate();
  time_t t1 = frst.first, t2 = last.first;
  ::strftime(b1,sizeof(b1),"%H:%M:%S",::localtime(&t1));
  ::strftime(b2,sizeof(b1),"%H:%M:%S",::localtime(&t2));
  draw_line_normal ("");
  draw_line_reverse("         RECONSTRUCTION monitoring on %s   [%s]", ns.name, ::lib_rtl_timestr());    
  draw_line_bold   ("         Information updates date between: %s.%03d and %s.%03d",b1,frst.second,b2,last.second);
}

/// Retrieve cluster name from cluster display
std::string RecSubfarmDisplay::clusterName() const {
  const Nodeset* ns = (const Nodeset*)data().pointer;
  return ns ? ns->name : "";
}

/// Number of nodes in the dataset
size_t RecSubfarmDisplay::numNodes()  {
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

/// Retrieve node name from cluster display by offset
string RecSubfarmDisplay::nodeName(size_t offset) {
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
void RecSubfarmDisplay::updateDisplay(const Nodeset& ns)   {
  begin_update();
  m_nodes->begin_update();
  m_reco->begin_update();
  m_receivers->begin_update();
  m_senders->begin_update();

  showHeader(ns);
  showNodes(ns);
  showTasks(ns);

  m_nodes->end_update();
  m_reco->end_update();
  m_receivers->end_update();
  m_senders->end_update();
  end_update();
}

extern "C" int romon_recsubfarm(int argc,char** argv) {
  RecSubfarmDisplay disp(argc,argv);
  disp.initialize();
  disp.run();
  return 1;
}
