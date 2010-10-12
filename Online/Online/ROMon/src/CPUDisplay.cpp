// $Id: CPUDisplay.cpp,v 1.3 2010-10-12 18:44:51 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/CPUDisplay.cpp,v 1.3 2010-10-12 18:44:51 frankb Exp $

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


CPUDisplay::CPUDisplay(FarmDisplay* parent, const string& title, const string& node, int height, int width)
  : InternalDisplay(parent, title), m_node(node)
{
  string svc = "/";
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
  bool found = false;
  time_t t1 = f.time;
  ::strftime(text,sizeof(text),"%H:%M:%S",::localtime(&t1));
  ::sprintf(txt,"      CPU farm:%s %s  [%d nodes]",f.name,text,f.nodes.size());
  ::scrc_put_chars(m_display,txt,BOLD,++line,3,1);
  ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
  for(_N::const_iterator i=f.nodes.begin(); i!=f.nodes.end(); i=f.nodes.next(i)) {
    const CPUset& cs = (*i);
    const _C& cores = cs.cores;
    const CPU::Stat& avg = cs.averages;
    t1 = cs.time;
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
    ::strftime(text,sizeof(text),"%H:%M:%S",::localtime(&t1));
    ::sprintf(txt,"      Node:%-8s  Family: %s last update:%s [%d cores] Context switch rate:%9.0f Hz",
              cs.name,cs.family,text,cores.size(),cs.ctxtRate);
    ::scrc_put_chars(m_display,txt,INVERSE,++line,3,1);
    ::sprintf(txt,"      Average values: %9s %9.3f %9.3f %9.3f %9.3f %10.3f %9.3f %9.3f",
              "",avg.user,avg.system,avg.nice,avg.idle,avg.iowait,avg.IRQ,avg.softIRQ);
    ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);

    if ( strcasecmp(m_node.c_str(),cs.name) != 0 && !found ) continue;
    // No does not look that good.... 
    //found = true;
    ::sprintf(txt,"        %9s %5s %9s %9s %9s %9s %9s %10s %9s %9s",
              "Clock","Cache","Mips","User[%]","System[%]","Nice[%]","Idle[%]","IO wait[%]","IRQ","SoftIRQ");
    ::scrc_put_chars(m_display,txt,BOLD,++line,1,1);
    for(_C::const_iterator ic=cores.begin(); ic!=cores.end(); ic=cores.next(ic)) {
      const CPU& c = *ic;
      ::sprintf(txt,"Core %3d:%6.0f %5d %9.0f %9.3f %9.3f %9.3f %9.3f %10.3f %9.3f %9.3f",
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
