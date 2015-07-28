// $Id: CPUDisplay.cpp,v 1.7 2010-10-15 08:06:25 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/CPUDisplay.cpp,v 1.7 2010-10-15 08:06:25 frankb Exp $

// Framework include files
#include "ROMon/TaskSupervisor.h"
#include "ROMon/FarmDisplay.h"
#include "ROMon/CPUMon.h"
#include "CPP/Event.h"
#include "SCR/scr.h"
extern "C" {
#include "dic.h"
}
// C++ include files
#include <cstdlib>

using namespace ROMon;
using namespace SCR;
using namespace std;


CPUDisplay::CPUDisplay(InternalDisplay* parent, const string& title, const string& node, int height, int width)
  : InternalDisplay(parent, title), m_node(node)
{
  string svc = svcPrefix();
  for(size_t i=0; i<title.length() && title[i]!='.';++i) svc += ::tolower(title[i]);
  svc += "/ROpublish/CPU";
  m_title = "CPU monitor on "+m_title+" Service:"+svc;
  ::scrc_create_display(&m_display,height,width,NORMAL,ON,m_title.c_str());
  ::scrc_put_chars(m_display,".....waiting for data from DIM service.....",BOLD,2,10,1);
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
}

CPUDisplay::~CPUDisplay() {
}

/// DIM command service callback
void CPUDisplay::update(const void* address) {
  const CPUfarm* f = (const CPUfarm*)address;
  if ( f->type == CPUfarm::TYPE ) {
    updateContent(*f);
  }
}

/// Update display content
void CPUDisplay::updateContent(const CPUfarm& f) {
  typedef CPUset::Cores _C;
  typedef CPUfarm::Nodes _N;
  char txt[255], text[64];
  int cnt = 0;
  int line = 1;
  time_t t1 = f.time;
  ::strftime(text,sizeof(text),"%H:%M:%S",::localtime(&t1));
  ::snprintf(txt,sizeof(txt),"      CPU farm:%s %s  [%d nodes]",f.name,text,f.nodes.size());
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
  ::scrc_put_chars(m_display,txt,BOLD,++line,3,1);
  ::scrc_put_chars(m_display,"      Mtot:Total memory in MB  Mfree:Free memory in MB"
                   "  Ctxt:Context switch rate in Hz",NORMAL,++line,3,1);
  ::scrc_put_chars(m_display,"      Type 'C' or Mouse-Left-Double-Click to close the window",NORMAL,++line,3,1);

  ::snprintf(txt,sizeof(txt)," %-8s %8s %15s %5s %5s %5s %5s %8s %9s %8s %8s %10s %8s %8s",
             "Node","Update","Familiy","Cores","Mtot","Mfree","Ctxt","User[%]",
             "System[%]","Nice[%]","Idle[%]","IO wait[%]","IRQ","SoftIRQ");
  ::scrc_put_chars(m_display,txt,BOLD,++line,1,1);
  for(_N::const_iterator i=f.nodes.begin(); i!=f.nodes.end(); i=f.nodes.next(i)) {
    const CPUset& cs = (*i);
    const CPU::Stat& avg = cs.averages;
    t1 = cs.time;
    ::strftime(text,sizeof(text),"%H:%M:%S",::localtime(&t1));
    ::snprintf(txt,sizeof(txt)," %-8s %8s %15s %5d %5.0f %5.0f %5.0f %8.3f %9.3f %8.3f %8.3f %10.2f %8.3f %8.3f",
               cs.name,text,cs.family,cs.cores.size(),double(cs.memory/1024),double(cs.memfree/1024),
               cs.ctxtRate,avg.user,avg.system,avg.nice,avg.idle,avg.iowait,avg.IRQ,avg.softIRQ);
    ::scrc_put_chars(m_display,txt,INVERSE,++line,1,1);
  }

  for(_N::const_iterator i=f.nodes.begin(); i!=f.nodes.end(); i=f.nodes.next(i)) {
    const CPUset& cs = (*i);
    const _C& cores = cs.cores;
    if ( strcasecmp(m_node.c_str(),cs.name) != 0 ) continue;
    ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
    ::scrc_put_chars(m_display,(" Detailed information for node:"+m_node).c_str() ,INVERSE,++line,3,1);
    ::snprintf(txt,sizeof(txt),"        %9s %5s %9s %9s %9s %9s %9s %10s %8s %8s",
               "Clock","Cache","Mips","User[%]","System[%]","Nice[%]","Idle[%]","IO wait[%]","IRQ","SoftIRQ");
    ::scrc_put_chars(m_display,txt,BOLD,++line,1,1);
    for(_C::const_iterator ic=cores.begin(); ic!=cores.end(); ic=cores.next(ic)) {
      const CPU& c = *ic;
      ::snprintf(txt,sizeof(txt),"Core %3d:%6.0f %5d %9.0f %9.3f %9.3f %9.3f %9.3f %10.3f %8.3f %8.3f",
                 ++cnt, c.clock,c.cache,c.bogomips,c.stats.user,c.stats.system,c.stats.nice,c.stats.idle,
                 c.stats.iowait,c.stats.IRQ,c.stats.softIRQ);
      ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);
    }
    ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
  }

  if ( 0 == f.nodes.size() ) {
    t1 = ::time(0);
    ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
    ::strftime(txt,sizeof(txt),"   No CPU information found.         %H:%M:%S",::localtime(&t1));
    ::scrc_put_chars(m_display,txt,INVERSE|BOLD,++line,5,1);
    ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED|BOLD);
  }
}
