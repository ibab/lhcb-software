// $Id: FarmStatClusterDisplay.cpp,v 1.6 2010-10-15 07:42:00 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/FarmStatClusterDisplay.cpp,v 1.6 2010-10-15 07:42:00 frankb Exp $

// Framework include files
#include "ROMon/FarmStatClusterDisplay.h"
#include "ROMon/Constants.h"
#include "ROMonDefs.h"
#include "CPP/Event.h"
#include "SCR/scr.h"

// C++ include files
#include <cstdlib>
#include <cstring>

using namespace ROMon;
using namespace SCR;
using namespace std;

namespace { template <class T> float _frac(T a,T b) {return 100.f*float(float(a)/float(b));} }

#define MAX_FARMSTAT_TIME   600

/// External creator function
InternalDisplay* ROMon::createFarmStatsDisplay(InternalDisplay* parent, const string& title) {
  return new FarmStatClusterDisplay(parent,title);
}

FarmStatClusterDisplay::FarmStatClusterDisplay(InternalDisplay* parent, const string& title, int height, int width)
  : InternalDisplay(parent, title), m_name(title), m_line(0)
{
  m_title = "FarmStat monitor on "+m_title;
  ::scrc_create_display(&m_display,height,width,INVERSE,ON,m_title.c_str());
  ::scrc_put_chars(m_display,".....waiting for data from DIM service.....",BOLD,2,10,1);
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
}

FarmStatClusterDisplay::~FarmStatClusterDisplay() {
  if ( m_line ) delete m_line;
  m_line = 0;
}

/// Explicit connect to data services
void FarmStatClusterDisplay::connect() {
  if ( m_line == 0 ) {
    m_line = new FarmStatClusterLine(this,0,m_name);
    m_line->start();
  }
}

/// Interactor overload: Display callback handler
void FarmStatClusterDisplay::handle(const CPP::Event& ev)    {
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
    case CMD_SHOW: {
      DimLock lock;
      if ( m_line ) update(m_line);
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
void FarmStatClusterDisplay::update(const void* cl_data) {
  const FarmStatClusterLine*      cl = (const FarmStatClusterLine*)cl_data;
  const FarmStatClusterLine::_CI& ci = cl->cpuData();
  const FarmStatClusterLine::_MI& mi = cl->mbmData();
  char txt[512], text1[64], ctxt[512];
  int line = 1, nBuff = 0;
  time_t now = ::time(0);

  map<string,string> cpu, mbm;
  for(FarmStatClusterLine::_CI::const_iterator i=ci.begin(); i!=ci.end(); ++i) {
    typedef map<string,vector<float> > _Node;
    const _Node& n = (*i).second;
    int numCore = 0;
    cpu[(*i).first] = "";
    for(_Node::const_iterator ni=n.begin();ni!=n.end();++ni) {
      const vector<float>& v = (*ni).second;
      if ( (*ni).first == "Avg" ) {
        ::sprintf(txt,"%5.1f%5.1f%6.1f%6.1f%7.2f ",v[0], v[1], v[2], v[3], v[4]);
        cpu[(*i).first] += txt;
        ::sprintf(ctxt," %9s %-30s","CPU [%]:","  User Sys  Nice  Idle IOwait ");
      }
      else {
        ::sprintf(txt,"%4.0f",v[0]+v[1]+v[2]);
        cpu[(*i).first] += txt;
        ::sprintf(txt,"%4d",++numCore);
        ::strcat(ctxt,txt);
      }
    }
  }

  for(FarmStatClusterLine::_MI::const_iterator i=mi.begin(); i!=mi.end(); ++i) {
    typedef map<string,vector<int> > _Node;
    const _Node& n = (*i).second;
    mbm[(*i).first] = "";
    for(_Node::const_iterator ni=n.begin();ni!=n.end();++ni) {
      const vector<int>& v = (*ni).second;
      ::sprintf(txt," %-6s%6.1f/%-6d%6.1f/%-5d   ",(*ni).first.c_str(),_frac(v[1],v[0]),v[0],_frac(v[2]-v[3],v[2]),v[2]-v[3]);
      mbm[(*i).first] += txt;
      ++nBuff;
    }
  }

  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
  ::strftime(text1,sizeof(text1),"%H:%M:%S",::localtime(&now));
  ::sprintf(txt,"      Subfarm Statistics of:%s   %s  [%d nodes]",cl->name().c_str(),text1,int(ci.size()));
  ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
  ::scrc_put_chars(m_display,txt,BOLD,++line,3,1);
  ::scrc_put_chars(m_display,"      <CTRL-H for Help>, <CTRL-E to exit>",NORMAL,++line,3,1);
  ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
  //                       1         2         3         4         5         6         7         8
  //             012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
  ::sprintf(txt," %-9s %-s","Node Name","----- CPU Average Values --------- [User + System + Nice] CPU usage of individual cores in [%]");
  ::scrc_put_chars(m_display,txt,MAGENTA,++line,2,1);
  ::scrc_put_chars(m_display,ctxt,MAGENTA,++line,2,1);
  for(map<string,string>::const_iterator j, i=cpu.begin(); i!=cpu.end();++i) {
    ::sprintf(txt," %-8s: %-s",(*i).first.c_str(),(*i).second.c_str());
    ::scrc_put_chars(m_display,txt,NORMAL,++line,2,1);
  }
  if ( nBuff > 0 ) {
    ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);  
    ::sprintf(txt," %-9s %-s","Node Name"," Buffer Space/Free   Slots/Free     Buffer Space/Free   Slots/Free     Buffer Space/Free   Slots/Free     Buffer Space/Free   Slots/Free");
    ::scrc_put_chars(m_display,txt,MAGENTA,++line,2,1);
    ::sprintf(txt," %9s %-s","MBM:","          [%]/[kb]     [%]/[kb]              [%]/[kb]     [%]/[kb]              [%]/[kb]     [%]/[kb]              [%]/[kb]     [%]/[kb]");
    ::scrc_put_chars(m_display,txt,YELLOW|MAGENTA,++line,2,1);
    for(map<string,string>::const_iterator i=mbm.begin(); i!=mbm.end();++i) {
      if ( (*i).second.length() > 2 ) {
        ::sprintf(txt," %-8s: %-s",(*i).first.c_str(),(*i).second.c_str());
        ::scrc_put_chars(m_display,txt,NORMAL,++line,2,1);
      }
      else {
        ::sprintf(txt," %-8s: %-s",(*i).first.c_str()," --- No MBM buffers present ---");
        ::scrc_put_chars(m_display,txt,BOLD,++line,2,1);
      }
    }
  }

  ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);  
  if ( 0 == ci.size() ) {
    ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
    ::strftime(txt,sizeof(txt),"   No Node information found.         %H:%M:%S",::localtime(&now));
    ::scrc_put_chars(m_display,txt,INVERSE|BOLD,++line,5,1);
    ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED|BOLD);
  }
  while (line+3 < m_display->height ) 
    ::scrc_put_chars(m_display,"",NORMAL,++line,2,1);
}
