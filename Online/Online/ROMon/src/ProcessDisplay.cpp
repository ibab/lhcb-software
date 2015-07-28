// $Id: ProcessDisplay.cpp,v 1.5 2010-10-15 10:53:54 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ProcessDisplay.cpp,v 1.5 2010-10-15 10:53:54 frankb Exp $

// Framework include files
#include "ROMon/TaskSupervisor.h"
#include "ROMon/FarmDisplay.h"
#include "ROMon/CPUMon.h"
#include "ROMonDefs.h"
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

ProcessDisplay::ProcessDisplay(InternalDisplay* parent, const string& node, const string& cluster, int flag, int height, int width)
  : InternalDisplay(parent, node), m_flag(flag)
{
  string svc = svcPrefix()+strlower(cluster);
  svc += (flag ? "/ROpublish/Tasks" : "/ROpublish/ROTasks");
  m_name  = strlower(node);
  m_title = "Process monitor on "+m_title+" Service:"+svc;
  ::scrc_create_display(&m_display,height,width,NORMAL,ON,m_title.c_str());
  ::scrc_put_chars(m_display,".....waiting for data from DIM service.....",BOLD,2,10,1);
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
}

ProcessDisplay::~ProcessDisplay() {
}

/// DIM command service callback
void ProcessDisplay::update(const void* address) {
  const ProcFarm* pf = (const ProcFarm*)address;
  if ( pf->type == ProcFarm::TYPE ) {
    updateContent(*pf);
  }
}

static inline bool __procmatch(const char* nam, const char* pattern) {
  return ::strncmp(nam,pattern,strlen(pattern))==0;
}
static bool is_driver(const char* cmd) {
  return
    __procmatch(cmd,"md/") ||
    __procmatch(cmd,"md_misc/") ||
    __procmatch(cmd,"aio/") ||
    __procmatch(cmd,"ata/") ||
    __procmatch(cmd,"crypto/") ||
    __procmatch(cmd,"rpciod/") ||
    __procmatch(cmd,"events/") ||
    __procmatch(cmd,"migration/") ||
    __procmatch(cmd,"ksoftirqd/") ||
    __procmatch(cmd,"kintegrityd/") ||
    __procmatch(cmd,"kblockd/") ||
    __procmatch(cmd,"kondemand/") ||
    __procmatch(cmd,"kthrotld/") ||
    __procmatch(cmd,"bnx2i_thread/") ||
    __procmatch(cmd,"watchdog/");
}

static inline bool is_sys_task(const char* cmd) { 
  return
    __procmatch(cmd,"init") ||
    __procmatch(cmd,"gpm ") ||
    __procmatch(cmd,"xfs ") ||
    __procmatch(cmd,"nfsd") ||
    __procmatch(cmd,"ntpd") ||
    __procmatch(cmd,"rpc.") ||
    __procmatch(cmd,"crond") ||
    __procmatch(cmd,"dhcpd") ||
    __procmatch(cmd,"klogd") ||
    __procmatch(cmd,"cupsd") ||
    __procmatch(cmd,"mdadm") ||
    __procmatch(cmd,"acpid") ||
    __procmatch(cmd,"xinetd") ||
    __procmatch(cmd,"lm_ip ") ||
    __procmatch(cmd,"portmap") ||
    __procmatch(cmd,"syslogd") ||
    __procmatch(cmd,"irqbalance") ||
    __procmatch(cmd,"sendmail:") ||
    __procmatch(cmd,"dbus-daemon") ||
    __procmatch(cmd,"clientmqueue") ||
    __procmatch(cmd,"mingetty") ||
    __procmatch(cmd,"smartd") ||
    __procmatch(cmd,"dhclient") ||
    __procmatch(cmd,"lockd") ||
    __procmatch(cmd,"khubd") ||
    __procmatch(cmd,"kacpid") ||
    __procmatch(cmd,"kjournald") ||
    __procmatch(cmd,"kauditd") ||
    __procmatch(cmd,"khelper") ||
    __procmatch(cmd,"kswapd") ||
    __procmatch(cmd,"kseriod") ||
    __procmatch(cmd,"krfcommd") ||
    __procmatch(cmd,"ata_aux") ||
    __procmatch(cmd,"scsi_eh_") ||
    __procmatch(cmd,"md0_raid") ||
    __procmatch(cmd,"md1_raid") ||
    __procmatch(cmd,"md2_raid") ||
    __procmatch(cmd,"pdflush");
}
static inline bool is_pvss_task(const char* cmd) { 
  return __procmatch(cmd,"PVSS00") || __procmatch(cmd,"smiSM ") || __procmatch(cmd,"WCC") ;
}

/// Update display content
void ProcessDisplay::updateContent(const ProcFarm& pf) {
  typedef Procset::Processes _P;
  typedef ProcFarm::Nodes _N;
  char state, txt[255], text[64], tmb[32];
  int cnt = 0;
  int line = 1;
  time_t t1;
  for(_N::const_iterator i=pf.nodes.begin(); i!=pf.nodes.end(); i=pf.nodes.next(i)) {
    const Procset& ps = (*i);
    const char* dot = strchr(ps.name,'.');
    if (0 == ::strncmp(ps.name,m_name.c_str(),3+(dot ? dot-ps.name : m_name.length()))) {
      const _P& procs = ps.processes;
      t1 = ps.time;
      ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
      ::strftime(text,sizeof(text),"%H:%M:%S",::localtime(&t1));
      ::sprintf(txt,"Node:%s last update:%s [%d processes]",ps.name,text,procs.size());
      ::scrc_put_chars(m_display,txt,BOLD,++line,10,1);
      if ( m_flag == 0 )
        ::scrc_put_chars(m_display,"The display shows tasks with a UTGID",NORMAL,++line,10,1);
      else if ( m_flag == 1 )
        ::scrc_put_chars(m_display,"The display shows NON-system, NON-UTGID, NON-PVSS tasks",NORMAL,++line,10,1);
      else if ( m_flag == 2 )
        ::scrc_put_chars(m_display,"The display shows SYSTEM tasks",NORMAL,++line,10,1);
      else if ( m_flag == 3 )
        ::scrc_put_chars(m_display,"The display shows PVSS tasks",NORMAL,++line,10,1);
      else if ( m_flag == 4 )
        ::scrc_put_chars(m_display,"The display shows DRIVER tasks",NORMAL,++line,10,1);
      ::scrc_put_chars(m_display,"Type 'P' or Mouse-Left-Double-Click to close the window",NORMAL,++line,10,1);

      ::sprintf(txt,"      %-32s %-6s %-5s %5s %5s %6s %6s %7s %7s %6s %3s %s",
                "UTGID","Owner","State","PID","PPID","Mem[%]","VM[MB]","RSS[MB]","Stk[kB]","CPU[%]","Thr","Started");
      ::scrc_put_chars(m_display,txt,INVERSE,++line,1,1);
      for(_P::const_iterator ip=procs.begin(); ip!=procs.end(); ip=procs.next(ip)) {
        const Process& p = *ip;
        switch(m_flag) {
        case 0:
          if ( ::strncmp(p.utgid,"N/A",3)==0 ) continue;
          break;
        case 1:  // Fall-through option if nothing else catches....
          //if ( ::strncmp(p.owner,"root",4)==0 ) continue;
          if ( ::strncmp(p.utgid,"N/A",3)!=0 ) continue;
          if ( is_pvss_task(p.cmd) ) continue;
          if ( is_sys_task(p.cmd) ) continue;
          if ( is_driver(p.cmd) ) continue;
          break;
        case 2:
          if ( !is_sys_task(p.cmd) ) continue;
          break;
        case 3:
          if ( !is_pvss_task(p.cmd) ) continue;
          break;
        case 4:
          if ( !is_driver(p.cmd) ) continue;
          break;
        default:
          break;
        }
        t1 = p.start;
        ::strftime(tmb,sizeof(tmb),"%b %d %H:%M",::localtime(&t1));
        state = p.state == 0 ? '?' : p.state;
        if ( ::strncmp(p.utgid,"N/A",3)==0 ) {
          ::strncpy(text,p.cmd,sizeof(text));
          text[27] = 0;
          text[26]=text[25]=text[24]='.';
          ::sprintf(txt,"%3d: %3s:%-28s %-10s %c %5d %5d %6.3f %6.1f %7.1f %7.0f %6.1f %3d %s",
                    ++cnt, p.utgid,text,p.owner,state,p.pid,p.ppid,p.mem,p.vsize/1024.,p.rss/1024.,p.stack,p.cpu,p.threads,tmb);
        }
        else {
          ::sprintf(txt,"%3d: %-32s %-10s %c %5d %5d %6.3f %6.1f %7.1f %7.0f %6.1f %3d %s",
                    ++cnt, p.utgid,p.owner,state,p.pid,p.ppid,p.mem,p.vsize/1024.,p.rss/1024.,p.stack,p.cpu,p.threads,tmb);
        }
        ::scrc_put_chars(m_display,txt,NORMAL,++line,2,1);
      }
      ::memset(txt,' ',m_display->cols);
      txt[m_display->cols-1]=0;
      while(line<m_display->rows)
        ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
      return;
    }
  }
  t1 = ::time(0);
  ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
  ::sprintf(txt,"   Unknown Node '%s'. No corresponding process information found.",m_name.c_str());
  ::scrc_put_chars(m_display,txt, INVERSE|BOLD,++line,5,1);
  ::strftime(txt,sizeof(txt),"         %H:%M:%S",::localtime(&t1));
  ::scrc_put_chars(m_display,txt,INVERSE|BOLD,++line,5,1);
  ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
  ::scrc_put_chars(m_display,"",NORMAL,++line,2,1);
  ::memset(txt,' ',m_display->cols);
  txt[m_display->cols-1]=0;
  while(line<m_display->rows)
    ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
}
