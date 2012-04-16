// $Id: BootClusterDisplay.cpp,v 1.7 2010-10-15 07:42:00 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/BootClusterDisplay.cpp,v 1.7 2010-10-15 07:42:00 frankb Exp $

// Framework include files
#include "ROMon/BootClusterDisplay.h"
#include "ROMon/BootMon.h"
#include "SCR/scr.h"
extern "C" {
#include "dic.h"
}

// C++ include files
#include <cstdlib>

using namespace ROMon;
using namespace SCR;
using namespace std;

namespace { const char* _flg(int v, int msk)  {  return (v&msk)==msk ? "OK" : "--"; } }
#define _F(x)   _flg(n.status,BootNodeStatus::x)

#define MAX_BOOT_TIME   600

/// External creator function
InternalDisplay* ROMon::createBootDisplay(InternalDisplay* parent, const string& title) {
  return new BootClusterDisplay(parent,title);
}

/// Initializing constructor
BootClusterDisplay::BootClusterDisplay(InternalDisplay* parent, const string& title, int height, int width)
  : InternalDisplay(parent, title), m_name(title)
{
  m_title = "Boot monitor on "+m_title;
  ::scrc_create_display(&m_display,height,width,INVERSE,ON,m_title.c_str());
  ::scrc_put_chars(m_display,".....waiting for data from DIM service.....",BOLD,2,10,1);
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
}

/// Standard destructor
BootClusterDisplay::~BootClusterDisplay() {
}

/// Explicit connect to data services
void BootClusterDisplay::connect() {
  if ( m_svc == 0 ) {
    string svc = "/";
    for(size_t i=0; i<m_name.length() && m_name[i]!='.';++i) svc += ::tolower(m_name[i]);
    svc += "/BootMonitor";
    m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
  }
}

/// Update display content
void BootClusterDisplay::update(const void* data) {
  const BootNodeStatusset& ns = *(const BootNodeStatusset*)data;
  typedef BootNodeStatusset::Nodes _N;
  char txt[255], text1[64], text2[64], text3[64];
  time_t t1 = ns.time, now = ::time(0);
  int line = 1;

  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
  ::strftime(text1,sizeof(text1),"%H:%M:%S",::localtime(&t1));
  ::sprintf(txt,"      Boot status of farm:%s %s  [%d nodes]",ns.name,text1,ns.nodes.size());
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
  for(_N::const_iterator i=ns.nodes.begin(); i!=ns.nodes.end(); i=ns.nodes.next(i)) {
    const _N::value_type& n = *i;
    int st = n.status, col=NORMAL;
    time_t dhcp = n.dhcpReq, mount = n.mountReq, fmc = n.fmcStart;
    if ( n.dhcpReq>0  ) ::strftime(text1,sizeof(text1),"%H:%M:%S",::localtime(&dhcp));
    else                ::strcpy(text1,"<Unknown>");
    if ( n.mountReq>0 ) ::strftime(text2,sizeof(text2),"%H:%M:%S",::localtime(&mount));
    else                ::strcpy(text2,"<Unknown>");
    if ( n.fmcStart>0 ) ::strftime(text3,sizeof(text3),"%Y-%m-%d %H:%M:%S",::localtime(&fmc));
    else                ::strcpy(text3,"<Unknown>");
    ::sprintf(txt,"  %-10s %4s %5s %4s %4s %4s %4s %4s %4s %4s   %03X %10s%10s%21s",
              n.name,_F(DHCP_REQUESTED),_F(MOUNT_REQUESTED),_F(CPU_STARTED),_F(ETH0_STARTED),_F(ETH1_STARTED),
              _F(PCI_STARTED),_F(TCP_STARTED),_F(FMC_STARTED),_F(TASKMAN_OK),st,text1,text2,text3);
    if      ( 0 != (st&BootNodeStatus::TASKMAN_OK)      ) col = GREEN|INVERSE;
    else if ( 0 != (st&BootNodeStatus::FMC_STARTED)     ) col = GREEN|INVERSE;
    else if ( 0 != (st&BootNodeStatus::TCP_STARTED)     ) col = GREEN|BOLD;
    else if ( 0 != (st&BootNodeStatus::ETH1_STARTED)    ) col = BLUE|INVERSE;
    else if ( 0 != (st&BootNodeStatus::ETH0_STARTED)    ) col = BLUE|BOLD;
    else if ( 0 != (st&BootNodeStatus::PCI_STARTED)     ) col = CYAN|INVERSE;
    else if ( 0 != (st&BootNodeStatus::CPU_STARTED)     ) col = CYAN|BOLD;
    else if ( 0 != (st&BootNodeStatus::MOUNT_REQUESTED) ) col = MAGENTA|BOLD;
    else if ( 0 != (st&BootNodeStatus::DHCP_REQUESTED)  ) col = MAGENTA|INVERSE;
    else col = RED|BOLD;
    if ( col&RED )
      strcat(txt," --> Not yet booted!");
    else if ( (col&(MAGENTA+INVERSE)) == (MAGENTA+INVERSE)) 
      strcat(txt," --> Boot started...");
    else if ( col&(MAGENTA+CYAN+BLUE) )
      strcat(txt," --> Boot in progress.");
    else if ( (col&(GREEN+INVERSE)) == (GREEN+INVERSE)) 
      strcat(txt," --> Booted OK.");
    else if ( abs(long(now-dhcp))<MAX_BOOT_TIME && (col&GREEN) == GREEN )
      strcat(txt," --> Nearly Booted.");
    else if ( abs(long(now-dhcp))>MAX_BOOT_TIME && 
              0 != (st&BootNodeStatus::DHCP_REQUESTED) && 
              0 == (st&BootNodeStatus::FMC_STARTED) ) {
      strcat(txt," --> Something is WRONG.");
      col = RED|BOLD|FLASH;
    }
    ::scrc_put_chars(m_display,txt,col,++line,3,1);
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
