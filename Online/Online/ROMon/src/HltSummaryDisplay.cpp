// $Id: HltSummaryDisplay.cpp,v 1.1 2010-09-20 19:00:10 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/HltSummaryDisplay.cpp,v 1.1 2010-09-20 19:00:10 frankb Exp $

// Framework include files
#include "SCR/scr.h"
#include "ROMon/HltSummaryDisplay.h"
#include "ROMon/ClusterLine.h"
#include "ROMon/Constants.h"
#include "ROMon/CPUMon.h"
#include "ROMonDefs.h"
#include "CPP/Event.h"
#include "RTL/Lock.h"

// C++ include files
#include <set>
#include <map>
#include <cstdio>
#include <cstdlib>

using namespace ROMon;
using namespace SCR;
using namespace std;

// Initializing constructor with default file
HltSummaryDisplay::HltSummaryDisplay(InternalDisplay* parent) 
  : InternalDisplay(parent,"Deferred HLT Runs")
{
  string head("Deferred HLT run summary");
  ::scrc_create_display(&m_display,60,132,INVERSE,ON,head.c_str());
  ::scrc_put_chars(m_display,".....waiting for data.....",BOLD,2,2,1);
  ::scrc_set_border(m_display,head.c_str(),INVERSE|BLUE);
}

/// Interactor overload: Display callback handler
void HltSummaryDisplay::handle(const Event& ev)    {
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
    case CMD_UPDATE: {
      DimLock   dimlock;
      RTL::Lock lock(screenLock());
      Pasteboard* pb = pasteboard();
      if ( pb ) {
        ::scrc_cursor_off(pb);
        ::scrc_begin_pasteboard_update (pb);
      }
      if ( ev.data ) update(ev.data);
      if ( pb ) ::scrc_end_pasteboard_update(pb);
      if ( parent() ) parent()->set_cursor(this);
      if ( pb ) ::scrc_cursor_on(pb);
      return;
    }
    default:
      break;
    }
  default:
    break;
  }
  InternalDisplay::handle(ev);
}

/// Update display content
void HltSummaryDisplay::update(const void* ptr) {
  typedef std::map<std::string,ClusterLine*> SubDisplays;
  typedef DeferredHLTSubfarmStats _S;
  typedef _S::Nodes _N;
  typedef _S::Runs  _R;

  int line = 1, col = NORMAL;
  static int count = 0;
  char text[512];


  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
  ::sprintf(text," Deferred HLT summary monitor for all nodes  [%s] %30s %d updates",
            ::lib_rtl_timestr(), "", ++count);
  ::scrc_put_chars(m_display,"",BOLD|BLUE,++line,2,1);
  ::scrc_put_chars(m_display,text,BOLD|BLUE,++line,2,1);
  ::scrc_put_chars(m_display,"",BOLD|BLUE,++line,2,1);
  ::scrc_put_chars(m_display,"",NORMAL,++line,2,1);


  map<int,int> run_files;
  map<int,set<string> > run_nodes;
  if ( ptr ) {
    SubDisplays& sd = *(SubDisplays*)ptr;
    _R::const_iterator ri;
    for(SubDisplays::const_iterator i=sd.begin(); i!=sd.end(); ++i) {
      const ClusterLine* l = (*i).second;
      const _S* s = l->data<_S>();
      if ( s )  {
        const _R& runs  = s->runs;
        const _N* nodes = s->nodes();
        for (ri=runs.begin(); ri!=runs.end(); ri=runs.next(ri))  {
          int run = (*ri).first;
          int files = (*ri).second;
          run_files[run] += files;
          map<int,set<string> >::iterator i = run_nodes.find(run);
          if ( i == run_nodes.end() ) {
            run_nodes[run] = set<string>();
          }
        }
        if ( nodes->size() > 0 ) {
          for (_N::const_iterator ni=nodes->begin(); ni!=nodes->end(); ni=nodes->next(ni))  {
            const DeferredHLTStats& n = *ni;
            string nn = n.name;
            if ( n.length()>0 && nn != s->name ) {
              const _R& nr = n.runs;
              for (ri=nr.begin(); ri!=nr.end(); ri=nr.next(ri))  {
                int run = (*ri).first;
                run_nodes[run].insert(nn);
              }
            }
          }
        }
      }
    }
  }
  else {
    ::scrc_put_chars(m_display,"+---------------------------------------------+",INVERSE|BOLD|RED,++line,2,1);
    ::scrc_put_chars(m_display,"| No valid information found to be displayed  |",INVERSE|BOLD|RED,++line,2,1);
    ::scrc_put_chars(m_display,"+---------------------------------------------+",INVERSE|BOLD|RED,++line,2,1);
  }

  if ( !run_files.empty() ) {
    ::sprintf(text,"%8s %6s %5s %-90s %s","Run","No.of","No.of","","No.of nodes");
    ::scrc_put_chars(m_display,text,MAGENTA|BOLD,++line,2,1);
    ::sprintf(text,"%8s %6s %5s %-90s %s","Number","Files","Nodes","Nodes where files exist","not shown");
    ::scrc_put_chars(m_display,text,MAGENTA|BOLD,++line,2,1);
    ::scrc_put_chars(m_display,"",MAGENTA|BOLD,++line,2,1);
  }
  set<string> tot_nodes;
  int tot_runs = 0, tot_files = 0;
  for(map<int,int>::const_iterator i=run_files.begin(); i!= run_files.end(); ++i) {
    int run = (*i).first;
    int num_files = (*i).second;
    int num_nodes = -1;
    map<int,set<string> >::const_iterator j=run_nodes.find(run);
    ++tot_runs;
    tot_files += num_files;
    if ( j != run_nodes.end() ) {
      string nodes;
      int exc_nodes=0;
      num_nodes = (*j).second.size();
      for(set<string>::const_iterator k=(*j).second.begin(); k!=(*j).second.end();++k) {
        if ( nodes.length()<80 ) nodes += (*k)+" ";
        else ++exc_nodes;
        tot_nodes.insert(*k);
      }
      ::sprintf(text,"%6d %5d %-90s %3d",num_files,num_nodes,nodes.c_str(),exc_nodes);
    }
    else {
      ::sprintf(text,"%6d %-96s",num_files,"ERROR: Run not in node-map!");
    }
    ::scrc_put_chars(m_display,text,NORMAL,++line,11,1);    
    ::sprintf(text,"%8d ",run);
    ::scrc_put_chars(m_display,text,BLUE|BOLD,line,2,0);
  }
  ::scrc_put_chars(m_display,"",NORMAL,++line,2,1);
  ::sprintf(text,"  Grand Total Statistics:  %8d runs with %6d files on %5ld nodes still to be processed",
            tot_runs,tot_files,long(tot_nodes.size()));
  col = tot_files>0 ? RED : BOLD|GREEN;
  ::scrc_put_chars(m_display,"",col,++line,2,1);
  ::scrc_put_chars(m_display,text,col,++line,2,1);    
  ::scrc_put_chars(m_display,"",col,++line,2,1);

  ::scrc_put_chars(m_display,"",NORMAL,++line,2,1);
  ::scrc_put_chars(m_display,"",NORMAL,++line,2,1);
  ::scrc_put_chars(m_display,"<CTRL-H for Help>, <CTRL-E to exit>, 'S' or 's' to close the display.",NORMAL,++line,2,1);
  while (line+3 < m_display->height ) 
    ::scrc_put_chars(m_display,"",NORMAL,++line,2,1);
}
