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
#include "ROMon/BenchmarkDisplay.h"
#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "RTL/strdef.h"
#include "SCR/scr.h"
extern "C" {
#include "dic.h"
}

// C++ include files
#include <cstdlib>
#include <cmath>
#include <map>

using namespace ROMon;
using namespace SCR;
using namespace std;

InternalDisplay* ROMon::createBenchmarkDisplay(InternalDisplay* parent, int mode, const string& title) {
  return new BenchmarkDisplay(parent,mode,title);
}

namespace {
  struct ClientStat {
    double    timesqr;
    double    time;
    long long nevt;
    long long last;
    unsigned int tm[2];
      
    ClientStat()
      : timesqr(0), time(0), nevt(0), last(0)
    { tm[0]=tm[1]=0;}
    ClientStat(const ClientStat& c) 
      : timesqr(c.timesqr), time(c.time), nevt(c.nevt), last(c.last)
    {tm[0]=c.tm[0];tm[1]=c.tm[1];}
#ifndef __INTEL_COMPILER
    ClientStat& operator=(const ClientStat& c) {
      if ( this != &c )  {
        timesqr = c.timesqr; 
        time    = c.time; 
        nevt    = c.nevt; 
        last    = c.last;
        tm[0]   = c.tm[0]; 
        tm[1]   = c.tm[1];
      }
      return *this;
    }
#endif
    long long entries() const { return nevt;                        }
    double mean()       const { return time/double(nevt);           }
    double mean2()      const { return timesqr/double(nevt);        }
    double sigma()      const { return sqrt(mean2()-mean()*mean()); }
  };
  typedef std::map<std::string, ClientStat>        BufferStats;
  typedef std::map<std::string, BufferStats>       NodeStats;
  typedef std::map<std::string, NodeStats>         SubfarmStats;
  typedef std::map<std::string, SubfarmStats>      FarmStats;

  size_t append(char* txt, const ClientStat* cs) {
    if ( 0 == cs || cs->nevt <= 0 )
      return ::sprintf(txt," %10s %5s %6s ","--","--","--");
    double m = cs->mean();
    const char* fmt = " %10lld %5.0f %c%5.0f ";
    if ( m < 99.9999 ) fmt = " %10lld %5.2f %c%5.1f ";
    return ::sprintf(txt,fmt,cs->entries(),m, char(177), cs->sigma());
  }  

  size_t append(char* txt, const BufferStats& bs, const string& task) {
    BufferStats::const_iterator l = bs.find(task);
    if ( l == bs.end() ) return append(txt,0);
    const ClientStat& cs = (*l).second;
    return append(txt,&cs);
  }

  void makeNodeStatLine(char* txt, const char* nnam, const NodeStats& ns)   {
    size_t len = ::sprintf(txt,"%-10s ",nnam);
    NodeStats::const_iterator k;
    if ( (k=ns.find("Events")) != ns.end() )   {
      len += append(txt+len, (*k).second, "MEPRx");
      len += append(txt+len, (*k).second, "GauchoJob");
    }	  
    if ( (k=ns.find("Send")) != ns.end() )   {
      len += append(txt+len, (*k).second, "DiskWR");
    }	  
    if ( (k=ns.find("Overflow")) != ns.end() )   {
      len += append(txt+len, (*k).second, "OvflowWR");
    }	  
  }
}

namespace ROMon {
  class FarmBenchStats : public FarmStats {
  public:
    explicit FarmBenchStats() {}
  };
}
#define FARMSTAT (*m_stat)

/// Initializing constructor
BenchmarkDisplay::BenchmarkDisplay(InternalDisplay* parent, int mode, const string& title, int height, int width)
  : InternalDisplay(parent, title), m_mode(mode), m_name(title)
{
  m_last = ::time(0);
  m_partition = "LHCb";
  m_stat = new FarmBenchStats();
  m_title = "Benchmark monitor on subfarm "+m_title;
  ::scrc_create_display(&m_display,height,width,INVERSE,ON,m_title.c_str());
  ::scrc_put_chars(m_display,".....waiting for data from DIM service.....",BOLD,2,10,1);
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
}

/// Standard destructor
BenchmarkDisplay::~BenchmarkDisplay() {
  delete m_stat;
}

/// Explicit connect to data services
void BenchmarkDisplay::connect() {
}

/// Update display content
void BenchmarkDisplay::update(const void* data) {
  const Nodeset& ns = *(const Nodeset*)data;
  if ( ns.type == Nodeset::TYPE ) {
    int line = 1;
    if ( ::str_ncasecmp(ns.name,"hlt",3) != 0 ) {
      return;
    }
    else if ( 0 == ns.nodes.size() ) {
      char txt[255];
      time_t now = ::time(0);
      ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
      ::strftime(txt,sizeof(txt),"   No Node information found.         %H:%M:%S",::localtime(&now));
      ::scrc_put_chars(m_display,txt,INVERSE|BOLD,++line,5,1);
      ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
      ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED|BOLD);
    }
    else if ( m_mode == NODE_STATS ) {
      line = updateNode(ns);
    }
    else if ( m_mode == SUBFARM_STATS ) {
      line = updateSubfarm(ns);
    }
    else if ( m_mode == FARM_STATS ) {
      line = updateFarm(ns);
    }
    if ( line > 1 ) {
      while (line+3 < m_display->height ) 
        ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
    }
  }
}

/// Update statistics
void BenchmarkDisplay::updateStats(const Nodeset& ns) {
  typedef Nodeset::Nodes               _N;
  typedef MBMBuffer::Clients           Clients;
  typedef Node::Buffers                Buffers;
  SubfarmStats& sfstat = FARMSTAT[ns.name];

  for(_N::const_iterator i=ns.nodes.begin(); i!=ns.nodes.end(); i=ns.nodes.next(i)) {
    const _N::value_type& n = *i;
    const char* nnam = n.name;
    const Buffers& buffs = *(n.buffers());
    NodeStats& nstat = sfstat[nnam];
    size_t nnam_len = ::strlen(nnam);
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      const Clients& clients = (*ib).clients;
      const char*    bnam    = (*ib).name;
      for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
        const char*         cnam   = (*ic).name;
        if ( ::str_ncasecmp(cnam,nnam,nnam_len)==0 )  {
          cnam += nnam_len+1;
        }
        if ( ::strstr(cnam,"GauchoJob") && ::strstr(bnam,"Send") )  {
          continue;
        }
        if ( ::strstr(cnam,"EvtHolder") )  {
          continue;
        }
        BufferStats&  cbstat = nstat[bnam];
        ClientStat&   cstat  = cbstat[cnam];
        int dt  = n.time-cstat.tm[0];
        int dtm = int(n.millitm)-cstat.tm[1];
        if ( dtm<0 ) { ++dt; dtm=1000-dtm; }
        dtm += dt*1000;
        if ( cstat.last != 0 ) {
          long long int d = (*ic).events - cstat.last;
          if ( d > 0 ) {
            double t = double(dtm);
            cstat.time    += t;
            cstat.timesqr += t*t/d;
            cstat.nevt    += d;
          }
          else {
            continue;
          }
        }
        cstat.tm[0] = n.time;
        cstat.tm[1] = n.millitm;
        cstat.last  = (*ic).events;
      }
    }
  }
}

/// Update node display content
int BenchmarkDisplay::updateNode(const Nodeset& ns) {
  int line = 1;
  if ( ::str_ncasecmp(m_name.c_str(),ns.name,::strlen(ns.name))==0 )  {
    SubfarmStats& sfstat = FARMSTAT[ns.name];
    time_t t1 = ns.firstUpdate().first;
    char txt[255], text1[64];
    size_t len;

    updateStats(ns);
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
    ::strftime(text1,sizeof(text1),"%H:%M:%S",::localtime(&t1));
    ::snprintf(txt,sizeof(txt),"      Benchmark display of node:%s subfarm:%s %s",m_name.c_str(),ns.name,text1);
    ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
    ::scrc_put_chars(m_display,txt,BOLD,++line,3,1);
    ::snprintf(txt,sizeof(txt),"      <CTRL-H for Help>, <CTRL-E to exit>");
    ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);
    ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
    ::scrc_put_chars(m_display,"",BOLD|BLUE,++line,3,1);
    ::scrc_put_chars(m_display,"   Detailed View:",BOLD|BLUE,++line,3,1);
    ::scrc_put_chars(m_display,"",BOLD|BLUE,++line,3,1);

    ::snprintf(txt,sizeof(txt),"%-32s%25s%25s%25s",
               //           0123456789012345678901   0123456789012345678901   01234567890123456789012
               "Task name","---- Events Buffer ---","----- Send Buffer ----"," -- Overflow Buffer ---");
    ::scrc_put_chars(m_display,txt,MAGENTA,++line,3,1);
    ::snprintf(txt,sizeof(txt),"%-32s  %10s %5s %6s  %10s %5s %6s  %10s %5s %6s ",
               "[UTGID]","[Events]","[ms]","[ms]","[Events]","[ms]","[ms]","[Events]","[ms]","[ms]");
    ::scrc_put_chars(m_display,txt,MAGENTA,++line,3,1);

    for(SubfarmStats::const_iterator i=sfstat.begin();i!=sfstat.end();++i) {
      const string& nnam = (*i).first;
      const NodeStats& nstat = (*i).second;
      if ( ::str_casecmp(nnam.c_str(),m_name.c_str())==0 ) {
        NodeStats ns_rel, ns_tot;
        NodeStats::const_iterator j;
        for(j=nstat.begin(); j!=nstat.end();++j)   {
          const BufferStats& bstat = (*j).second;
          const char* bnam = (*j).first.c_str();
          for(BufferStats::const_iterator k=bstat.begin(); k!=bstat.end();++k) {
            const ClientStat& cs = (*k).second;
            string cnam = (*k).first;
            size_t idx  = cnam.find('_');
            ClientStat& cstat_rel = ns_rel[cnam][bnam];
            cstat_rel.nevt    += cs.nevt;
            cstat_rel.time    += cs.time;
            cstat_rel.timesqr += cs.timesqr;

            if ( idx != string::npos ) cnam = cnam.substr(0,idx);
            else if ( cnam.substr(0,5)=="MEPRx" ) cnam = "MEPRx";
            ClientStat& cstat_tot = ns_tot[cnam][bnam];
            cstat_tot.nevt    += cs.nevt;
            cstat_tot.time    += cs.time;
            cstat_tot.timesqr += cs.timesqr;
          }
        }
        for(j=ns_rel.begin(); j!=ns_rel.end();++j)   {
          const BufferStats& bstat = (*j).second;
          const char* cnam = (*j).first.c_str();
          char* p = txt;
          ::memset(txt,' ',sizeof(txt));
          txt[sizeof(txt)-1]=0;
          for(BufferStats::const_iterator k=bstat.begin(); k!=bstat.end();++k) {
            const string& bnam = (*k).first;
            const ClientStat& cs = (*k).second;
            p = txt + 32;
            if      ( bnam == "Events"   ) ;
            else if ( bnam == "Send"     ) p += 25;
            else if ( bnam == "Overflow" ) p += 25+25;
            len = ::snprintf(txt,sizeof(txt),"%s",cnam);
            txt[len]=' ';
            len = append(p, &cs);
            p[len]=' ';
          }
          ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);	
        }
        ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
        ::scrc_put_chars(m_display,"",BOLD|BLUE,++line,3,1);
        ::scrc_put_chars(m_display,"   Summary Overview:",BOLD|BLUE,++line,3,1);
        ::scrc_put_chars(m_display,"",BOLD|BLUE,++line,3,1);
        ::snprintf(txt,sizeof(txt),"%-32s%25s%25s%25s",
                   //           0123456789012345678901   0123456789012345678901   01234567890123456789012
                   "Task name","---- Events Buffer ---","----- Send Buffer ----"," -- Overflow Buffer ---");
        ::scrc_put_chars(m_display,txt,MAGENTA,++line,3,1);
        ::snprintf(txt,sizeof(txt),"%-32s  %10s %5s %6s  %10s %5s %6s  %10s %5s %6s ",
                   "[UTGID]","[Events]","[ms]","[ms]","[Events]","[ms]","[ms]","[Events]","[ms]","[ms]");
        ::scrc_put_chars(m_display,txt,MAGENTA,++line,3,1);
        for(j=ns_tot.begin(); j!=ns_tot.end();++j)   {
          const BufferStats& bstat = (*j).second;
          const char* cnam = (*j).first.c_str();
          char* p = txt;
          ::memset(txt,' ',sizeof(txt));
          txt[sizeof(txt)-1]=0;
          for(BufferStats::const_iterator k=bstat.begin(); k!=bstat.end();++k) {
            const string& bnam = (*k).first;
            const ClientStat& cs = (*k).second;
            p = txt + 32;
            if      ( bnam == "Events"   ) ;
            else if ( bnam == "Send"     ) p += 25;
            else if ( bnam == "Overflow" ) p += 25+25;
            len = ::snprintf(txt,sizeof(txt),"%s",cnam);
            txt[len]=' ';
            len = append(p, &cs);
            p[len]=' ';
          }
          ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);	
        }

        break;
      }
    }
    ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
  }
  return line;
}

/// Update subfarm display content
int BenchmarkDisplay::updateSubfarm(const Nodeset& ns) {
  int line = 1;
  if ( ns.name == m_name ) {
    char txt[255], text1[64];
    time_t t1 = ns.firstUpdate().first;
    SubfarmStats& sfstat = FARMSTAT[ns.name];

    updateStats(ns);

    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
    ::strftime(text1,sizeof(text1),"%H:%M:%S",::localtime(&t1));
    ::snprintf(txt,sizeof(txt),"      Benchmark display of subfarm:%s %s  [%d nodes]",ns.name,text1,ns.nodes.size());
    ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
    ::scrc_put_chars(m_display,txt,BOLD,++line,3,1);
    ::snprintf(txt,sizeof(txt),"      <CTRL-H for Help>, <CTRL-E to exit>");
    ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);
    ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);

    size_t len = ::snprintf(txt,sizeof(txt),"%-11s","Buffer/Task");
    len += ::sprintf(txt+len," %23s "," --- Events/MEPRx ---");
    len += ::sprintf(txt+len," %23s "," - Events/GauchoJob -");
    len += ::sprintf(txt+len," %23s "," ---- Send/DiskWR ---");
    len += ::sprintf(txt+len," %23s "," ---- OverflowWR ----");
    ::scrc_put_chars(m_display,txt,MAGENTA,++line,3,1);
    len = ::snprintf(txt,sizeof(txt),"%-10s ","");
    len += ::sprintf(txt+len,"   %8s %5s %6s ","Events","Mean","Sigma");
    len += ::sprintf(txt+len,"   %8s %5s %6s ","Events","Mean","Sigma");
    len += ::sprintf(txt+len,"   %8s %5s %6s ","Events","Mean","Sigma");
    len += ::sprintf(txt+len,"   %8s %5s %6s ","Events","Mean","Sigma");
    ::scrc_put_chars(m_display,txt,MAGENTA,++line,3,1);
    len = ::snprintf(txt,sizeof(txt),"%-10s ","Node name");
    len += ::sprintf(txt+len,"   %8s %5s %6s ","","[ms]","[ms]");
    len += ::sprintf(txt+len,"   %8s %5s %6s ","","[ms]","[ms]");
    len += ::sprintf(txt+len,"   %8s %5s %6s ","","[ms]","[ms]");
    len += ::sprintf(txt+len,"   %8s %5s %6s ","","[ms]","[ms]");
    ::scrc_put_chars(m_display,txt,MAGENTA,++line,3,1);
    ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);

    NodeStats ns_rel, ns_tot;
    for(SubfarmStats::const_iterator i=sfstat.begin();i!=sfstat.end();++i) {
      const NodeStats& nstat = (*i).second;
      ns_rel.clear();
      for(NodeStats::const_iterator j=nstat.begin(); j!=nstat.end();++j) {
        const BufferStats& bstat = (*j).second;
        const char* bnam = (*j).first.c_str();
        BufferStats& bs     = ns_rel[bnam];
        BufferStats& bs_tot = ns_tot[bnam];
        for(BufferStats::const_iterator k=bstat.begin(); k!=bstat.end();++k) {
          const ClientStat& cs = (*k).second;
          string cnam = (*k).first;
          size_t idx  = cnam.find('_');
          if ( idx != string::npos ) cnam=cnam.substr(0,idx);
          else if ( cnam.substr(0,5)=="MEPRx" ) cnam = "MEPRx";
          ClientStat& cstat_tot = bs_tot[cnam];
          cstat_tot.nevt    += cs.nevt;
          cstat_tot.time    += cs.time;
          cstat_tot.timesqr += cs.timesqr;
          ClientStat& cstat = bs[cnam];
          cstat.nevt    += cs.nevt;
          cstat.time    += cs.time;
          cstat.timesqr += cs.timesqr;
        }
      }
      makeNodeStatLine(txt, (*i).first.c_str(), ns_rel);
      ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);
    }
    ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
    makeNodeStatLine(txt, "Total", ns_tot);
    ::scrc_put_chars(m_display,txt,BOLD,++line,3,1);
  }
  return line;
}

/// Update farm display content
int BenchmarkDisplay::updateFarm(const Nodeset& ns) {
  int line = 1;
  char txt[255], text1[64];
  time_t t1 = ns.firstUpdate().first;

  updateStats(ns);

  if ( ::time(0) - m_last < 2 ) return line;
  m_last = ::time(0);
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
  ::strftime(text1,sizeof(text1),"%H:%M:%S",::localtime(&t1));
  ::snprintf(txt,sizeof(txt),"      Benchmark display of HLT farm of partition:%s %s",m_name.c_str(), text1);
  ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
  ::scrc_put_chars(m_display,txt,BOLD,++line,3,1);
  ::snprintf(txt,sizeof(txt),"      <CTRL-H for Help>, <CTRL-E to exit>");
  ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);
  ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);

  size_t len = ::snprintf(txt,sizeof(txt),"%-11s","");
  len += ::snprintf(txt+len,sizeof(txt)-len," %23s "," -- Events/MEPRx ----");
  len += ::snprintf(txt+len,sizeof(txt)-len," %23s "," - Events/GauchoJob -");
  len += ::snprintf(txt+len,sizeof(txt)-len," %23s "," ---- Send/DiskWR ---");
  len += ::snprintf(txt+len,sizeof(txt)-len," %23s "," ---- OverflowWR ----");
  ::scrc_put_chars(m_display,txt,MAGENTA,++line,3,1);
  len = ::snprintf(txt,sizeof(txt),"%-10s ","Subfarm");
  len += ::snprintf(txt+len,sizeof(txt)-len," %10s %5s %6s ","Events","Mean","Sigma");
  len += ::snprintf(txt+len,sizeof(txt)-len," %10s %5s %6s ","Events","Mean","Sigma");
  len += ::snprintf(txt+len,sizeof(txt)-len," %10s %5s %6s ","Events","Mean","Sigma");
  len += ::snprintf(txt+len,sizeof(txt)-len," %10s %5s %6s ","Events","Mean","Sigma");
  ::scrc_put_chars(m_display,txt,MAGENTA,++line,3,1);
  len = ::snprintf(txt,sizeof(txt),"%-10s ","Name");
  len += ::snprintf(txt+len,sizeof(txt)-len," %10s %5s %6s ","","[ms]","[ms]");
  len += ::snprintf(txt+len,sizeof(txt)-len," %10s %5s %6s ","","[ms]","[ms]");
  len += ::snprintf(txt+len,sizeof(txt)-len," %10s %5s %6s ","","[ms]","[ms]");
  len += ::snprintf(txt+len,sizeof(txt)-len," %10s %5s %6s ","","[ms]","[ms]");
  ::scrc_put_chars(m_display,txt,MAGENTA,++line,3,1);
  ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);

  NodeStats ns_rel, ns_tot;
  for(FarmStats::const_iterator h=FARMSTAT.begin();h!=FARMSTAT.end();++h) {
    const SubfarmStats& sfstat = (*h).second;
    ns_rel.clear();
    for(SubfarmStats::const_iterator i=sfstat.begin();i!=sfstat.end();++i) {
      const NodeStats& nstat = (*i).second;
      for(NodeStats::const_iterator j=nstat.begin(); j!=nstat.end();++j) {
        const BufferStats& bstat = (*j).second;
        const char* bnam = (*j).first.c_str();
        BufferStats& bs     = ns_rel[bnam];
        BufferStats& bs_tot = ns_tot[bnam];
        for(BufferStats::const_iterator k=bstat.begin(); k!=bstat.end();++k) {
          const ClientStat& cs = (*k).second;
          string cnam = (*k).first;
          size_t idx  = cnam.find('_');
          if ( idx != string::npos ) cnam=cnam.substr(0,idx);
          else if ( cnam.substr(0,5)=="MEPRx" ) cnam = "MEPRx";
          ClientStat& cstat_tot = bs_tot[cnam];
          cstat_tot.nevt    += cs.nevt;
          cstat_tot.time    += cs.time;
          cstat_tot.timesqr += cs.timesqr;
          ClientStat& cstat = bs[cnam];
          cstat.nevt    += cs.nevt;
          cstat.time    += cs.time;
          cstat.timesqr += cs.timesqr;
        }
      }
    }
    makeNodeStatLine(txt, (*h).first.c_str(), ns_rel);
    ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);
  }
  ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
  makeNodeStatLine(txt, "Total", ns_tot);
  ::scrc_put_chars(m_display,txt,BOLD,++line,3,1);
  return line;
}
