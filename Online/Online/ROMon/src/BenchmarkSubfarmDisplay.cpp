// $Id: BenchmarkSubfarmDisplay.cpp,v 1.7 2010-10-15 07:42:00 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/BenchmarkSubfarmDisplay.cpp,v 1.7 2010-10-15 07:42:00 frankb Exp $

// Framework include files
#include "ROMon/BenchmarkSubfarmDisplay.h"
#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "SCR/scr.h"
extern "C" {
#include "dic.h"
}

// C++ include files
#include <cstdlib>
#include <map>

using namespace ROMon;
using namespace SCR;
using namespace std;

/// External creator function
InternalDisplay* ROMon::createBenchmarkSubfarmDisplay(InternalDisplay* parent, const string& title) {
  return new BenchmarkSubfarmDisplay(parent,title);
}
InternalDisplay* ROMon::createBenchmarkNodeDisplay(InternalDisplay* parent, const string& title) {
  return new BenchmarkSubfarmDisplay(parent,title);
}
InternalDisplay* ROMon::createBenchmarkFarmDisplay(InternalDisplay* parent, const string& title) {
  return new BenchmarkSubfarmDisplay(parent,title);
}

/// Initializing constructor
BenchmarkSubfarmDisplay::BenchmarkSubfarmDisplay(InternalDisplay* parent, const string& title, int height, int width)
: InternalDisplay(parent, title), m_name(title)
{
  m_partition = "LHCb";
  m_title = "Benchmark monitor on subfarm "+m_title;
  ::scrc_create_display(&m_display,height,width,INVERSE,ON,m_title.c_str());
  ::scrc_put_chars(m_display,".....waiting for data from DIM service.....",BOLD,2,10,1);
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
}

/// Standard destructor
BenchmarkSubfarmDisplay::~BenchmarkSubfarmDisplay() {
}

/// Explicit connect to data services
void BenchmarkSubfarmDisplay::connect() {
}

/// Update display content
void BenchmarkSubfarmDisplay::update(const void* data) {
  const Nodeset& ns = *(const Nodeset*)data;
  if ( ns.type == Nodeset::TYPE ) {
    if ( ns.name == m_name ) {
      typedef Nodeset::Nodes _N;
      char txt[255], text1[64];
      time_t t1 = ns.firstUpdate().first, now = ::time(0);
      int line = 1;
      SubfarmStats& sfstat = m_stat[m_name];

      ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
      ::strftime(text1,sizeof(text1),"%H:%M:%S",::localtime(&t1));
      ::sprintf(txt,"      Benchmark display of subfarm:%s %s  [%d nodes]",ns.name,text1,ns.nodes.size());
      ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
      ::scrc_put_chars(m_display,txt,BOLD,++line,3,1);
      ::sprintf(txt,"      <CTRL-H for Help>, <CTRL-E to exit>");
      ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);
      ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);

      ::scrc_put_chars(m_display,"Color Legend:",NORMAL,++line,3,1);
      ::scrc_put_chars(m_display,"  ",GREEN|INVERSE,++line,3,0);
      ::scrc_put_chars(m_display,"Finished, FMC started",NORMAL,  line,7,0);
      ::scrc_put_chars(m_display,"  ",               GREEN,             line,30,0);
      ::scrc_put_chars(m_display,"...TCP started",   NORMAL|BOLD,       line,33,0);
      ::scrc_put_chars(m_display,"  ",               BLUE|INVERSE,      line,60,0);
      ::scrc_put_chars(m_display,"...ETH1 started",  NORMAL,            line,63,0);
      ::scrc_put_chars(m_display,"  ",               BLUE,              line,90,0);
      ::scrc_put_chars(m_display,"...ETH0 started",  NORMAL|BOLD,       line,93,0);

      ::scrc_put_chars(m_display,"  ",               CYAN|INVERSE,    ++line, 3,0);
      ::scrc_put_chars(m_display,"...PCI started",   NORMAL,            line, 7,0);
      ::scrc_put_chars(m_display,"  ",               CYAN,              line,30,0);
      ::scrc_put_chars(m_display,"...CPU(s) started",NORMAL|BOLD,       line,33,0);
      ::scrc_put_chars(m_display,"  ",               MAGENTA,           line,60,0);
      ::scrc_put_chars(m_display,"...Mounting disks",NORMAL|BOLD,       line,63,0);

      ::scrc_put_chars(m_display,"  ",               MAGENTA|INVERSE,   line,90,0);
      ::scrc_put_chars(m_display,"DHCP request seen",NORMAL,            line,93,0);
      ::scrc_put_chars(m_display,"  ",               RED|INVERSE|BOLD,++line, 3,0);
      ::scrc_put_chars(m_display,"...NOT BOOTED",    BOLD,              line, 7,0);
      ::scrc_put_chars(m_display,"TMGR",             BOLD,              line,30,0);
      ::scrc_put_chars(m_display,"TaskSupervisor OK",NORMAL,            line,35,0);

      ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);

      //                       1         2         3         4         5         6         7         8
      //             012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
      ::sprintf(txt,"  Node       DHCP Mount  CPU ETH0 ETH1  PCI  TCP  FMC TMGR  Flag Times:DHCP     MOUNT       FMC");
      ::scrc_put_chars(m_display,txt,YELLOW|MAGENTA,++line,3,1);
      ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);

      typedef MBMBuffer::Clients           Clients;
      typedef Node::Buffers                Buffers;
      typedef Nodeset::Nodes               Nodes;

      for(_N::const_iterator i=ns.nodes.begin(); i!=ns.nodes.end(); i=ns.nodes.next(i)) {
	const _N::value_type& n = *i;
	const char* nnam = n.name;
	const Buffers& buffs = *(n.buffers());
	NodeClientStats& nstat = sfstat[nnam];
	for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
	  const Clients& clients = (*ib).clients;
	  const char* bnam = (*ib).name;
	  for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
	    const char* cnam = (*ic).name;
	    ClientBufferStats&  cbstat = nstat[cnam];
	    ClientStat& cstat = cbstat[bnam];
	    int dt = n.time-cstat.tm[0];
	    int dtm = int(n.millitm)-cstat.tm[1];
	    if ( dtm<0 ) { ++dt; dtm=1000-dtm; }
	    dtm += dt*1000;
	    if ( cstat.first != 0 ) {
	      long long int d = (*ic).events - cstat.first;
	      if ( d > 0 ) {
		double t = double(dtm);
		cstat.time    += t;
		cstat.timesqr += t*t/double(d);
	      }
	      else {
		cstat.time = cstat.timesqr = 0.;
	      }
	    }
	    cstat.first = (*ic).events;
	    cstat.tm[0] = n.time;
	    cstat.tm[1] = n.millitm;
	  }
	}
      }
      for(SubfarmStats::const_iterator i=sfstat.begin();i!=sfstat.end();++i) {
	const NodeClientStats& ncstat = (*i).second;
	for(NodeClientStats::const_iterator j=ncstat.begin(); j!=ncstat.end();++j) {
	  const ClientBufferStats& cbstat = (*j).second;
	  for(ClientBufferStats::const_iterator k=cbstat.begin(); k!=cbstat.end();++k) {
	    const ClientStat& cstat = (*k).second;
	    if ( cstat.first ) {
	    }
	  }
	}
      }
      ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
  
      if ( 0 == ns.nodes.size() ) {
	::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
	::strftime(txt,sizeof(txt),"   No Node information found.         %H:%M:%S",::localtime(&now));
	::scrc_put_chars(m_display,txt,INVERSE|BOLD,++line,5,1);
	::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
	::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED|BOLD);
      }
      while (line+3 < m_display->height ) 
	::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
    }
  }
}
