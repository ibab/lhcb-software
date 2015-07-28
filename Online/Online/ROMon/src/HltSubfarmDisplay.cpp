// $Id: HltSubfarmDisplay.cpp,v 1.19 2010-10-21 06:04:22 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/HltSubfarmDisplay.cpp,v 1.19 2010-10-21 06:04:22 frankb Exp $

// C++ include files
#include <cstdlib>
#include <iostream>
#include <map>

// Framework include files
#include "RTL/Lock.h"
#include "RTL/strdef.h"
#include "ROMon/CPUMon.h"
#include "ROMon/HltSubfarmDisplay.h"
#include "ROMonDefs.h"
#include "dic.hxx"

using namespace ROMon;
using namespace std;
typedef ROMon::DeferredHLTSubfarmStats _S;
typedef _S::Nodes               _N;
typedef _S::Node::Runs          _R;

static void help() {
  cout <<"  romon_hltsubfarm -option [-option]" << endl
       <<"       -h[eaderheight]=<number>     Height of the header        display.                      " << endl
       <<"       -d[elay]=<number>            Time delay in millisecond between 2 updates.              " << endl
       <<"       -s[ervicename]=<name>        Name of the DIM service  providing monitoring information." << endl
       << endl;
}

/// Static abstract object creator.
ClusterDisplay* ROMon::createHltSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv) {
  return new HltSubfarmDisplay(width,height,posx,posy,argc,argv);
}

/// Standard constructor
HltSubfarmDisplay::HltSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv)
  : ClusterDisplay(width, height), m_nodes(0)
{
  m_position = Position(posx,posy);
  init(argc, argv);
}

/// Standard constructor
HltSubfarmDisplay::HltSubfarmDisplay(int argc, char** argv)
  : ClusterDisplay(), m_nodes(0)
{
  init(argc, argv);
}

void HltSubfarmDisplay::init(int argc, char** argv)   {
  int hdr_height;
  RTL::CLI cli(argc,argv,help);
  cli.getopt("headerheight",  1, hdr_height    =   5);
  cli.getopt("delay",         1, m_delay       = 1000);
  cli.getopt("servicename",   1, m_svcName     = "/hlta01/ROpublish/HLT1");
  m_readAlways = true;
  setup_window();
  m_nodes      = createSubDisplay(Position(m_position.x-2,m_position.y-2+hdr_height), 
                                  Area(m_area.width,m_area.height-hdr_height),
                                  "HLT Deferred Processing Information on "+m_svcName);
  end_update();
}

/// Standard destructor
HltSubfarmDisplay::~HltSubfarmDisplay()  {
  begin_update();
  delete m_nodes;
  end_update();
}
#define GByte (1024.f*1024.f*1024.f)

/// Display the node information
void HltSubfarmDisplay::showNodes()  {
  const _S* stats = m_data.data<const _S>();
  const _N& nodes = *(stats->nodes());
  const char* fmt = " %-11s%5ld%5d  %-90s %s";
  MonitorDisplay* disp = m_nodes;
  char text[512], txt[64];
  string val;

  disp->draw_line_normal("");
  disp->draw_line_bold(" Node       No.of No.of  %37s %15s %37s %9s  %s",
                       "","Run statistics","","Runs not","Disk statistics [GB]");
  disp->draw_line_bold(" Name       Runs  Files  %37s %15s %37s %9s  %s",
                       // 123456789012345678901234567890     123456789012345678901234567890
                       "<------------------------------------",  "   Run/Files   ",
                       "------------------------------------>",
                       "displayed","<-Free Total   Used-->");
  int tot_files = 0, exc_runs = 0, exc_files = 0;
  map<int,int> tot_runs;
  for (_N::const_iterator n=nodes.begin(); n!=nodes.end(); n=nodes.next(n))  {
    const _R& runs = (*n).runs;
    const Diskspace& disk = (*n).localdisk;
    int numFiles = 0;
    val = "";
    exc_runs = exc_files = 0;
    for (_R::const_iterator ri=runs.begin(); ri!=runs.end(); ri=runs.next(ri))  {
      int run = (*ri).first;
      int files = (*ri).second;
      numFiles += files;
      tot_runs[run] += files;
      if ( val.length()+10 <= 92 ) {
        ::sprintf(text,"%7d/%-3d",run,files);
        val += text;
      }
      else {
        ++exc_runs;
        exc_files += files;
      }
    }
    tot_files += numFiles;
    float gb  = float(disk.blockSize)/GByte;
    float fr  = gb*(disk.freeBlocks>0 ? disk.freeBlocks : 1);
    float tot = gb*disk.numBlocks;
    ::sprintf(text,"%7d/%-3d   %5.0f %5.0f  %5.1f %%", 
              exc_runs, exc_files, fr, tot,tot < 1e-10 ? 100. : 100.f*(1.f-fr/tot));
    ::sprintf(txt,"%s/%c",(*n).name, char((*n).overflowState));
    disp->draw_line_normal(fmt, txt , runs.size(), numFiles, val.c_str(), text);
  }
  disp->draw_line_normal("");
  disp->draw_line_bold(fmt, "Total:", tot_runs.size(), tot_files, "", "");
  disp->draw_line_normal("");
  disp->draw_line_normal("");
  disp->draw_line_normal("");
  disp->draw_line_normal("   <Mouse-left double-click> or <ENTER> to close window");
}

/// Update header information
void HltSubfarmDisplay::showHeader()   {
  char b1[64], b2[64];
  const _S* stats = (_S*)m_data.data<char>();
  TimeStamp frst=stats->firstUpdate(), last=stats->lastUpdate();
  time_t t1 = frst.first, t2 = last.first;
  ::strftime(b1,sizeof(b1),"%H:%M:%S",::localtime(&t1));
  ::strftime(b2,sizeof(b1),"%H:%M:%S",::localtime(&t2));
  draw_line_normal ("");
  draw_line_bold   ("         HLT monitoring on %s   [%s]", stats->name, ::lib_rtl_timestr());    
  draw_line_bold   ("         Information updates date between: %s.%03d and %s.%03d",b1,frst.second,b2,last.second);
}

/// Number of nodes in the dataset
size_t HltSubfarmDisplay::numNodes()  {
  size_t n = 0;
  const _S* stats = data().data<const _S>();
  if ( stats ) {
    switch(stats->type)  {
    case _S::TYPE:
      n = stats->nodes()->size();
    default:
      break;
    }
  }
  return n;
}

/// Retrieve cluster name from cluster display
std::string HltSubfarmDisplay::clusterName() const {
  const _S* stats = (const _S*)data().pointer;
  return stats ? stats->name : "";
}

/// Retrieve node name from cluster display by offset
string HltSubfarmDisplay::nodeName(size_t offset) {
  const _S* stats = (const _S*)data().pointer;
  if ( stats ) {
    size_t cnt;
    _N::const_iterator n;
    const _N* nodes = stats->nodes();
    if ( offset > 0 )  {
      --offset;
      for (n=nodes->begin(), cnt=0; n!=nodes->end(); n=nodes->next(n), ++cnt)  {
        if ( cnt == offset ) {
          return (*n).name;
        }
      }
    }
  }
  return "";
}

/// Update all displays
void HltSubfarmDisplay::update()   {
  DimLock lock;
  begin_update();
  m_nodes->begin_update();
  if ( m_data.actual>0 ) {
    showHeader();
    showNodes();
  }
  else {
    m_nodes->draw_line_normal ("");
    m_nodes->draw_line_bold("   ..... No update information present .....");
    m_nodes->draw_line_normal ("");
  }
  m_nodes->end_update();
  end_update();
}

extern "C" int romon_hltsubfarm(int argc,char** argv) {
  HltSubfarmDisplay disp(argc,argv);
  disp.initialize();
  disp.run();
  return 1;
}
