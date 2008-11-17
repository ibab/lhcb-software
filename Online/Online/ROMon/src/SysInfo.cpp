// $Id: SysInfo.cpp,v 1.4 2008-11-17 07:40:40 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/SysInfo.cpp,v 1.4 2008-11-17 07:40:40 frankb Exp $
#include "ROMon/SysInfo.h"
#include "ROMon/CPUMonOstream.h"
#include "ROMon/ROMonInfo.h"
#include "ROMon/Sys.h"
#include "ROMonDefs.h"
#include "RTL/Lock.h"
#include "RTL/rtl.h"

#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <cerrno>
#include <fcntl.h>

extern "C" {
  #include "dis.h"
}

using namespace std;
using namespace ROMon;
#define CPUINFO_SIZE  4*1024
#define PROCINFO_SIZE 100*1024

/// Default constructor
SysInfo::SysInfo(NodeStats* buff, size_t len, int nbuffs)
: m_buffer(buff), m_buffLen(len), m_idx(1), m_readings(0), m_nBuffs(nbuffs)  {
  int j;
  vector<RODimListener*> servers;
  string match, n = RTL::nodeNameShort();
  for(size_t i=0; i<n.length();++i) n[i]=::toupper(n[i]);
  match = "/FMC/"+n+"/task_manager";
  log() << "SysInfo: Listening to service:" << match << endl;
  m_cpuInfo = new char*[m_nBuffs];
  for(j=0; j<m_nBuffs; ++j) m_cpuInfo[j] = new char[CPUINFO_SIZE];
  m_procInfo = new char*[m_nBuffs];
  for(j=0; j<m_nBuffs; ++j) m_procInfo[j] = new char[PROCINFO_SIZE];

  // Setup the object
  read(m_mem);
  m_utgid = new FMCMonListener();
  m_utgid->setMatch(match);
  m_utgid->setItem("longList");
  servers.push_back(m_utgid);
  m_dns = new ROMonInfo(servers);
  statistics()->reset();
}

SysInfo::~SysInfo()  {
  int i;
  delete m_dns;
  delete m_utgid;
  for(i=0; i<m_nBuffs; ++i) delete [] m_procInfo[i];
  for(i=0; i<m_nBuffs; ++i) delete [] m_cpuInfo[i];
  delete [] m_cpuInfo;
  delete [] m_procInfo;
}

/// Convert task manager information into something useful to extract the UTGID
int SysInfo::readTmProcs(DSC& info, TmProcs& procs) {
  char* p;
  int it;
  dim_lock();
  if ( m_utgid->clients().begin() != m_utgid->clients().end() ) {
    RODimListener::Clients::const_iterator ic=m_utgid->clients().begin();
    DSC* data = (*ic).second->data<DSC>();
    info.copy(data->data,data->actual);
    info.time    = data->time;
    info.millitm = data->millitm;
    dim_unlock();
    procs.clear();
    for(p=info.data, it=0; p<info.data+info.actual; ++it) {
      const char *pid   = p;
      const char *cmd   = pid   + strlen(pid)+1;
      const char *utgid = cmd   + strlen(cmd)+1;
      const char *date  = utgid + strlen(utgid)+1;
      const char *state = date  + strlen(date)+1;
      const char *end   = state + strlen(state)+1;
      if ( strcmp(state,"running")==0 ) {
        int proc_id = ::atoi(pid);
        procs[proc_id] = TmProc(p,proc_id,cmd,utgid,date,state);
#if 0
        log() << "PID:"    << proc_id 
              << " utgid:" << utgid
              << " cmd:"   << cmd
              << " date:"  << date
              << " state:" << state << endl;
#endif
      }
      p = (char*)end;
    }
  }
  return 1;
}

int SysInfo::combineCPUInfo() {
  CPUset&       cr = *(statistics()->cpu());
  const CPUset& cn = *cpuNow();
  const CPUset& cl = *cpuLast();
  CPUset::Cores::iterator m = cr.cores.begin();
  CPUset::Cores::const_iterator i=cn.cores.begin(), j=cl.cores.begin();
  float d_user, d_system, d_nice, d_idle, d_iowait, d_IRQ, d_softIRQ, d_tot, d_percent;

  ::memcpy(&cr,&cn,cn.length());
  d_user    = cn.averages.user    - cl.averages.user;
  d_system  = cn.averages.system  - cl.averages.system;
  d_nice    = cn.averages.nice    - cl.averages.nice;
  d_idle    = cn.averages.idle    - cl.averages.idle;
  d_iowait  = cn.averages.iowait  - cl.averages.iowait;
  d_IRQ     = cn.averages.IRQ     - cl.averages.IRQ;
  d_softIRQ = cn.averages.softIRQ - cl.averages.softIRQ;
  d_tot     = d_user+d_system+d_nice+d_idle+d_IRQ+d_softIRQ+d_iowait;
  d_percent = (float)(d_tot/1e2);
  //cout << d_user << " " << d_system << " " << d_nice << " " << d_idle << " " << d_iowait << " " << d_IRQ << " " << d_softIRQ << " " << d_tot << " " << d_percent << endl;
  cr.averages.user     = d_user/d_percent;
  cr.averages.system   = d_system/d_percent;
  cr.averages.nice     = d_nice/d_percent;
  cr.averages.idle     = d_idle/d_percent;
  cr.averages.iowait   = d_iowait/d_percent;
  cr.averages.IRQ      = d_IRQ/d_percent;
  cr.averages.softIRQ  = d_softIRQ/d_percent;
  cr.ctxtRate          = (float)((cn.ctxtRate-cl.ctxtRate)*1e3/d_tot);
  cr.memory            = m_mem.memTotal;
  cr.memfree           = m_mem.memFree;
  for( ; i != cn.cores.end(); i=cn.cores.next(i), j=cl.cores.next(j), m=cr.cores.next(m) ) {
    d_user    = (*i).stats.user    - (*j).stats.user;
    d_system  = (*i).stats.system  - (*j).stats.system;
    d_nice    = (*i).stats.nice    - (*j).stats.nice;
    d_idle    = (*i).stats.idle    - (*j).stats.idle;
    d_iowait  = (*i).stats.iowait  - (*j).stats.iowait;
    d_IRQ     = (*i).stats.IRQ     - (*j).stats.IRQ;
    d_softIRQ = (*i).stats.softIRQ - (*j).stats.softIRQ;
    d_percent = (float)((d_user+d_system+d_nice+d_idle+d_IRQ+d_softIRQ+d_iowait)/1e2);
    (*m).stats.user     = d_user    / d_percent;
    (*m).stats.system   = d_system  / d_percent;
    (*m).stats.nice     = d_nice    / d_percent;
    (*m).stats.idle     = d_idle    / d_percent;
    (*m).stats.iowait   = d_iowait  / d_percent;
    (*m).stats.IRQ      = d_IRQ     / d_percent;
    (*m).stats.softIRQ  = d_softIRQ / d_percent;
  }
  return 1;
}

int SysInfo::combineProcessInfo() {
  const Procset& pn = *procsNow();
  const Procset& pl = *procsLast();
  Procset& pr = *statistics()->procs();
  const Procset::Processes& now = pn.processes;
  const Procset::Processes& last = pl.processes;
  
  Procset::Processes& res = pr.reset()->processes;
  Procset::Processes::const_iterator i, j, k;
  Procset::Processes::iterator m = res.reset();
  double diff = ((1e3*double(pn.time-pl.time))+double(pn.millitm)-double(pl.millitm))/1e3; // seconds!
  pr.time = pn.time;
  pr.millitm = pn.millitm;
  ::strcpy(pr.name,pn.name);
  for(i=now.begin(), j=last.begin(); i != now.end(); i=now.next(i) ) {
    const Process& q = *i;
    int pid = q.pid;
    for(k=j; j != last.end(); j=last.next(j) )
      if ( (*j).pid == pid ) break;
    if ( j == last.end() ) {
      for(j=last.begin(); j != k; j=last.next(j))
        if ( (*j).pid == pid ) break;
    }
    if ( j != last.end() && (*j).pid == pid ) {
      TmProcs::const_iterator tmi=m_procs.find(pid);
      Process& p = *m;
      if ( tmi != m_procs.end() )
        ::strncpy(p.utgid,(*tmi).second.utgid,sizeof(p.utgid));
      else
        ::strcpy(p.utgid,"N/A");
      p.utgid[sizeof(p.utgid)-1] = 0;
      ::strncpy(p.owner,q.owner,sizeof(p.owner));
      p.owner[sizeof(p.owner)-1] = 0;
      ::strncpy(p.cmd,q.cmd,sizeof(p.cmd));
      p.cmd[sizeof(p.cmd)-1] = 0;
      p.cpu     = (q.cpu-(*j).cpu)*1e2/diff;
      p.mem     = (float)(q.vsize * 1e2 / float(m_mem.memTotal));
      p.vsize   = q.vsize;
      p.rss     = q.rss;
      p.stack   = q.stack;
      p.pid     = q.pid;
      p.ppid    = q.ppid;
      p.threads = q.threads;
      p.start   = q.start;
      p.state   = q.state;
      m = res.add(m);
      if ( ((char*)m)-((char*)m_buffer) > (int)m_buffLen ) {
        log() << "Global section memory too small.....exiting" << endl;
        return 0;
      }
    }
  }
  return 1;
}

/// Initialize the object with all static information
int SysInfo::init() {
  log() << "SysInfo: Initializing statistics...." << endl;
  for(int i=0; i<m_nBuffs-1; ++i) {
    newReading();
    read(*cpuNow()->reset(),CPUINFO_SIZE);
    read(*procsNow()->reset(),PROCINFO_SIZE);
    ::lib_rtl_sleep(1000);
  }
  return 1;
}

/// Update changing object data items
int SysInfo::update() {
  const RODimListener::Clients& cl = m_utgid->clients();
  if ( cl.size() == 1 ) {
    newReading();
    read(statistics()->memory);
    read(*cpuNow()->reset(),CPUINFO_SIZE);
    //readStat(*cpuNow(), CPUINFO_SIZE, m_numCores);
    read(*procsNow()->reset(), PROCINFO_SIZE);
    combineCPUInfo();
    readTmProcs(m_taskInfo,m_procs);
    combineProcessInfo();
    statistics()->fixup();
  }
  else {
    log() << "SysInfo: Bad number of information clients:" << cl.size() << " .... stop." << endl;
    string match, n = RTL::nodeNameShort();
    for(size_t i=0; i<n.length();++i) n[i]=::toupper(n[i]);
    match = "/FMC/"+n+"/task_manager";
    m_dns->servers().clear();
    delete m_utgid;
    m_utgid = new FMCMonListener();
    m_utgid->setMatch(match);
    m_utgid->setItem("longList");
    m_dns->servers().push_back(m_utgid);
    return 0;
  }
  return 1;
}

extern "C" int romon_sysinfo(int,char**)  {
  size_t cnt=0, len = CPUINFO_SIZE+PROCINFO_SIZE;
  char* buff = new char[len];
  SysInfo sys((NodeStats*)buff,len,2);
  sys.init();
  while(1) {
    sys.update();
    log() << "============================   " << ++cnt << "   ===========================" << endl;
    cout << *sys.statistics() << endl;
    log() << "===== DATA SIZE:" << sys.statistics()->length() << endl;
    ::lib_rtl_sleep(5000);
  }
  return 1;
}
