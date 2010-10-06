// $Id: FarmStatClusterDisplay.cpp,v 1.1 2010-10-06 21:55:23 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/FarmStatClusterDisplay.cpp,v 1.1 2010-10-06 21:55:23 frankb Exp $

// Framework include files
#include "ROMon/FarmStatClusterDisplay.h"
#include "SCR/scr.h"

// C++ include files
#include <cstdlib>

using namespace ROMon;
using namespace SCR;
using namespace std;

namespace { const char* _flg(int v, int msk)  {  return (v&msk)==msk ? "OK" : "--"; } }
#define _F(x)   _flg(n.status,FarmStatNodeStatus::x)
//namespace { bool _ok(int v, int msk)        { return (v&msk)==msk; }  }
//#define _BAD(x) _ok(n.status,FarmStatNodeStatus::x)
namespace { template <class T> float _frac(T a,T b) {return 100.f*float(float(a)/float(b));} }

#define MAX_FARMSTAT_TIME   600

FarmStatClusterDisplay::FarmStatClusterDisplay(InternalDisplay* parent, const string& title, int height, int width)
  : InternalDisplay(parent, title), m_name(title)
{
  m_title = "FarmStat monitor on "+m_title;
  ::scrc_create_display(&m_display,height,width,INVERSE,ON,m_title.c_str());
  ::scrc_put_chars(m_display,".....waiting for data from DIM service.....",BOLD,2,10,1);
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE|BOLD);
}

FarmStatClusterDisplay::~FarmStatClusterDisplay() {
}

/// Update display content
void FarmStatClusterDisplay::update(const void* cl_data) {
  const FarmStatClusterLine* cl = (const FarmStatClusterLine*)cl_data;
  typedef FarmStatClusterLine::_CI _CI;
  typedef FarmStatClusterLine::_MI _MI;
  const _CI& ci = cl->cpuData();
  const _MI& mi = cl->mbmData();
  char txt[255], text1[64];
  time_t now = ::time(0);
  int line = 1;

  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE|BOLD);
  ::strftime(text1,sizeof(text1),"%H:%M:%S",::localtime(&now));
  ::sprintf(txt,"      FarmStats of farm:%s %s  [%d nodes]",cl->name().c_str(),text1,int(ci.size()));
  ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
  ::scrc_put_chars(m_display,txt,BOLD,++line,3,1);
  ::sprintf(txt,"      <CTRL-H for Help>, <CTRL-E to exit>");
  ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);
  ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
  //                       1         2         3         4         5         6         7         8
  //             012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
  ::sprintf(txt," %-12s %-70s %s","Node  CPU:","  User Sys Idle IOwait [%]  -------- User+Sys of cores [%] -------",
	    "MBM:Buffer Space Slots Buffer Space Slots Buffer Space Slots Buffer Space Slots");
  ::scrc_put_chars(m_display,txt,YELLOW|MAGENTA,++line,3,1);
  ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
  map<string,string> cpu, mbm;
  for(_CI::const_iterator i=ci.begin(); i!=ci.end(); ++i) {
    typedef map<string,vector<float> > _Node;
    const _Node& n = (*i).second;
    cpu[(*i).first] = "";
    for(_Node::const_iterator ni=n.begin();ni!=n.end();++ni) {
      const vector<float>& v = (*ni).second;
      if ( (*ni).first == "Avg" ) {
	::sprintf(txt,"%5.1f%5.1f%5.1f%7.2f ",v[0], v[1], v[2], v[3]);
	cpu[(*i).first] += txt;
      }
      else {
	::sprintf(txt,"%4.0f",v[0]+v[1]);
	cpu[(*i).first] += txt;
      }
    }
  }
  for(_MI::const_iterator i=mi.begin(); i!=mi.end(); ++i) {
    typedef map<string,vector<int> > _Node;
    const _Node& n = (*i).second;
    mbm[(*i).first] = "";
    for(_Node::const_iterator ni=n.begin();ni!=n.end();++ni) {
      const vector<int>& v = (*ni).second;
      ::sprintf(txt," %-6s%6.1f%6.1f",(*ni).first.c_str(),_frac(v[1],v[0]),_frac(v[3],v[2]));
      mbm[(*i).first] += txt;
    }
  }
  for(map<string,string>::const_iterator j,i=cpu.begin(); i!=cpu.end();++i) {
    j = mbm.find((*i).first);
    const char* p = j == mbm.end() ? "" : (*j).second.c_str();
    ::sprintf(txt," %-12s %-70s    %s",(*i).first.c_str(),(*i).second.c_str(),p);
    ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);
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
    ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
}
