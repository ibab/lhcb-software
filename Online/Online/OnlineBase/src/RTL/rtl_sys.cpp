// $Id: $
//====================================================================
//  OnlineBase
//--------------------------------------------------------------------
//
//  Package    : OnlineBase
//
//  Description: Utilities for the LHCb online system
//
//  Author     : M.Frank
//  Created    : 08/1/2015
//
//====================================================================
// $Header: $
// Framework include files
#include "RTL/Sys.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>

using namespace std;
using namespace RTL;

_PROCFILE_NAME(64,stat)
_PROCFILE_NAME(64,status)
_PROCFILE_NAME(64,environ)

/// Empty constructor
SystemMemory::SystemMemory()  {
  reset();
}

/// Reset data content
SystemMemory* SystemMemory::reset() {
  ::memset(this,0,sizeof(SystemMemory));
  return this;
}
 
/// Print system memory data
void SystemMemory::print() const   {
}

/// Empty constructor
SystemCPU::SystemCPU()  {
  reset();
}

/// Reset data content
SystemCPU* SystemCPU::reset() {
  ::memset(this,0,sizeof(SystemCPU));
  return this;
}

/// Print CPU core data
void SystemCPU::print() const   {
  ::printf("Processor:%2d Cores:%2d PhyID:%1d CoreID:%2d Family:%2d "
           "Cache:%5d kB clock:%5.0f MHz Bogo:%5.0f\n",
           int(processor),int(numCore),int(physID),int(coreID),
           int(family),int(cache),clock,bogomips);
}

/// Empty constructor
SystemDiskspace::SystemDiskspace()  {
  reset();
}

/// Reset data content
SystemDiskspace* SystemDiskspace::reset() {
  ::memset(this,0,sizeof(SystemDiskspace));
  return this;
}

/// Reset data content
SysProcess* SysProcess::reset() {
  ::memset(this,0,sizeof(SysProcess));
  return this;
}

/// Reset data content
StatusProcess* StatusProcess::reset() {
  ::memset(this,0,sizeof(StatusProcess));
  return this;
}

/// Default destructor. Non-virtuality is intended. Do not inherit!
SysFile::FileDescriptor::~FileDescriptor()   {
  if (m_fd > 0)  ::close(m_fd);
  m_fd = 0;
}

/// Read buffer from file in  one go
int SysFile::read(char* buf, size_t siz) const  {
  FileDescriptor fd(::open(m_name.c_str(),O_RDONLY));  
  if( fd.get() < 0 )  {
    string err = "Failed to open "+m_name+" ";
    throw runtime_error(err+::strerror(errno));
  }
  size_t tmp = 0;
  while ( tmp < siz )  {
    int sc = ::read(fd.get(),buf+tmp,siz-tmp);
    if ( sc >  0 ) {
      tmp += sc;
    }
    else if ( sc == 0 )  {
      buf[tmp] = 0;
      return tmp;
    }
    else if ( errno == EINTR )  {
      printf("EINTR~!!!!\n");
      continue;
    }
    else  {
      break;
    }
  }
  if ( tmp != siz )  {
    string err = "Read of system file "+m_name+" failed:";
    err += ::strerror(errno);
    throw runtime_error(err);
  }
  return tmp;
}

/// Write buffer to file in  one go
int SysFile::write(char* buf, size_t siz, int flags) const  {
  FileDescriptor fd(::open(m_name.c_str(),O_WRONLY|O_CREAT|O_TRUNC,flags));  
  if( fd.get() < 0 )  {
    string err = "Failed to open "+m_name+" ";
    throw runtime_error(err+::strerror(errno));
  }
  size_t tmp = 0;
  while ( tmp < siz )  {
    int sc = ::write(fd.get(),buf+tmp,siz-tmp);
    if ( sc >  0 ) {
      tmp += sc;
    }
    else if ( sc == 0 )  {
      buf[tmp] = 0;
      return tmp;
    }
    else if ( errno == EINTR )  {
      printf("EINTR~!!!!\n");
      continue;
    }
    else  {
      break;
    }
  }
  if ( tmp != siz )  {
    string err = "Write of system file "+m_name+" failed:";
    err += ::strerror(errno);
    throw runtime_error(err);
  }
  return tmp;
}

/// Read system data from proc file system: system uptime information
int RTL::read(SystemUptime& ut) {
  char buff[256];
  int cnt = SysFile("/proc/uptime").read(buff,sizeof(buff));
  if ( cnt>0 ) {
    ::sscanf(buff,"%f %f",&ut.uptime,&ut.idletime);
    return 1;
  }
  return 0;
}

void SystemUptime::print() const {
  ::printf("System uptime:%f   idle time:%f\n",uptime,idletime);
}

/// Read system data from proc file system: process environment
int RTL::read(EnvironProcess& env, int proc_id) {
  size_t len = 1024*100;
  char* buff = new char[len], *ptr=buff, *end;
  int cnt = SysFile(fn_process_environ(proc_id)).read(buff,len);
  env.env.clear();
  if ( cnt > 0 ) {
    end = buff+cnt;
    while(ptr<end) {
      char* tag = ptr;
      char* val = tag+1;
      while(*val != '=')  {
        ++val;
        if (val >= end) goto Done;
      }
      if ( *val == '=' ) {
        *val = 0;
        env.env.push_back(make_pair(tag,++val));
      }
      ptr = val + ::strlen(val) + 1;
    }
  Done:
    delete [] buff;
    return 1;
  }
  return 0;
}

/// Print system data from proc file system
void EnvironProcess::print() const {
  for(Environ::const_iterator i=env.begin(); i!=env.end();++i)
    ::printf("%s=%s\n",(*i).first.c_str(),(*i).second.c_str());
}

/// Read system data from proc file system: process UTGID
int RTL::read(UtgidProcess& proc, int proc_id) {
  size_t len = 1024*100;
  char* buff = new char[len], *ptr=buff, *end;
  proc.utgid = "";
  try {
    int cnt = SysFile(fn_process_environ(proc_id)).read(buff,len);
    if ( cnt > 0 ) {
      end = buff+cnt;
      while(ptr<end) {
        char* tag = ptr;
        char* val = tag+1;
        while(*val != '=')  {
          ++val;
          if (val >= end) goto Done;
        }
        if ( *val == '=' ) {
          *val = 0;
          ++val;
          if ( ::strcmp(tag,"UTGID")==0 ) {
            proc.utgid = val;
            break;
          }
        }
        ptr = val + ::strlen(val) + 1;
      }
    Done:
      delete [] buff;
      return 1;
    }
  }
  catch(const exception& e) {
    delete [] buff;
    throw e;
  }
  catch(...) {
  }
  if ( buff ) delete [] buff;
  return 0;
}

/// Print system data from proc file system
void UtgidProcess::print() const {
  ::printf("%s=%s\n","UTGID",utgid.c_str());
}

int RTL::read(StatusProcess& proc, int proc_id) {
  char buff[2048], *ptr=buff;
  int nitem=0, cnt=SysFile(fn_process_status(proc_id)).read(buff,sizeof(buff));
  if(cnt>0)  {
    while(ptr && ptr<(buff+cnt)) {
      char* p   = ::strchr(ptr,'\t');
      char* end = ::strchr(ptr,'\n');
      ptr = (end) ? end+1 : 0;
      if ( 0 == p ) continue;
      ++p;
      switch(++nitem) {
      case 1:   ::sscanf(p,"%s",proc.comm);          break;
      case 2:   ::sscanf(p,"%c",&proc.state);        break;
        //case 3:   ::sscanf(p,"%d%%",&proc.sleepAvg);   break;
      case 3:   ::sscanf(p,"%d",&proc.tgid);         break;
      case 4:   ::sscanf(p,"%d",&proc.pid);          break;
      case 5:   ::sscanf(p,"%d",&proc.ppid);         break;
      case 7:   ::sscanf(p,"%d",&proc.uid);          break;
      case 8:   ::sscanf(p,"%d",&proc.gid);          break;
      case 9:   ::sscanf(p,"%d",&proc.utrace);       break;
      case 10:  ::sscanf(p,"%d",&proc.fdSize);       break;
      case 12:  ::sscanf(p,"%d",&proc.vmPeak);       break;
      case 13:  ::sscanf(p,"%d",&proc.vmSize);       break;
      case 14:  ::sscanf(p,"%d",&proc.vmLock);       break;
      case 15:  ::sscanf(p,"%d",&proc.vmHWM);        break;
      case 16:  ::sscanf(p,"%d",&proc.vmRSS);        break;
      case 17:  ::sscanf(p,"%d",&proc.vmData);       break;
      case 18:  ::sscanf(p,"%d",&proc.vmStack);      break;
      case 19:  ::sscanf(p,"%d",&proc.vmExe);        break;
      case 20:  ::sscanf(p,"%d",&proc.vmLib);        break;
      case 21:  ::sscanf(p,"%d",&proc.vmPTE);        break;
      case 22:  ::sscanf(p,"%d",&proc.vmSwap);       break;
        //case 22:  ::sscanf(p,"%08lx",&proc.staBrk);    break;
        //case 23:  ::sscanf(p,"%08lx",&proc.brk);       break;
        //case 24:  ::sscanf(p,"%08lx",&proc.staStk);    break;
      case 23:  ::sscanf(p,"%d",&proc.nThreads);     break;
      case 24:  break; // SigQ
      case 25:  ::sscanf(p,"%016lx",&proc.sigPend);  break;  // Fmt: SigCgt: 00000001808044e9
      case 26:  ::sscanf(p,"%016lx",&proc.shdPend);  break;
      case 27:  ::sscanf(p,"%016lx",&proc.sigBlk);   break;
      case 28:  ::sscanf(p,"%016lx",&proc.sigIgn);   break;
      case 29:  ::sscanf(p,"%016lx",&proc.sigCgt);   break;
      case 30:  ::sscanf(p,"%016lx",&proc.capInh);   break;
      case 31:  ::sscanf(p,"%016lx",&proc.capPrm);   break;
      case 32:  ::sscanf(p,"%016lx",&proc.capEff);   break;
      default:                                       break;
      }
    }
    return 1;
  }
  return 0;
}

void StatusProcess::print() const {
  ::printf("\
           Name:     \t%s\n\
           State:    \t%c\n\
           SleepAVG: \t%8d%%\n\
           Tgid:     \t%8d\n\
           Pid:      \t%8d\n\
           PPid:     \t%8d\n\
           Uid:      \t%8d\n\
           Gid:      \t%8d\n\
           FDSize:   \t%8d\n\
           VmPeak:   \t%8d kB\n\
           VmSize:   \t%8d kB\n\
           VmLck:    \t%8d kB\n\
           VmHWM:    \t%8d kB\n\
           VmRSS:    \t%8d kB\n\
           VmData:   \t%8d kB\n\
           VmStk:    \t%8d kB\n\
           VmExe:    \t%8d kB\n\
           VmLib:    \t%8d kB\n\
           VmPTE:    \t%8d kB\n\
           StaBrk:   \t%08lx kB\n\
           Brk:      \t%08lx kB\n\
           StaStk:   \t%08lx kB\n\
           Threads:  \t%8d\n\
           SigPnd:   \t%016lx\n\
           ShdPnd:   \t%016lx\n\
           SigBlk:   \t%016lx\n\
           SigIgn:   \t%016lx\n\
           SigCgt:   \t%016lx\n\
           CapInh:   \t%016lx\n\
           CapPrm:   \t%016lx\n\
           CapEff:   \t%016lx\n",
           comm, state, sleepAvg, tgid, pid, ppid, uid, gid,
           fdSize, vmPeak, vmSize, vmLock, vmHWM, vmRSS, vmData, vmStack,
           vmExe, vmLib, vmPTE, staBrk, brk, staStk, nThreads, sigPend,
           shdPend, sigBlk, sigIgn, sigCgt, capInh, capPrm, capEff);
}

/// Read system data from proc file system: process statistics /proc/<pid>/stat
int RTL::read(SysProcess& proc, int proc_id) {
  char buff[1024];
  int cnt = SysFile(fn_process_stat(proc_id)).read(buff,sizeof(buff));
  if(cnt>0)  {
    //                       1  2  3  4  5  6  7  8  9   10  1   2   3   4   5   6   7   8   9   20  1   2   3   4   5   6   7   8   9   30  1   2   3   4   5
    int ret = ::sscanf(buff,"%d %s %c %d %d %d %d %d %lu %lu %lu %lu %lu %lu %lu %lu %lu %ld %ld %ld %lu %lu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
                       &proc.pid,      // 1
                       proc.comm,      // 2
                       &proc.state,    // 3
                       &proc.ppid,     // 4
                       &proc.pgrp,     // 5
                       &proc.session,  // 6
                       &proc.tty,      // 7
                       &proc.tpgid,    // 8
                       &proc.flags,    // 9
                       &proc.minflt,   // 10
                       &proc.cminflt,  // 1
                       &proc.majflt,   // 2
                       &proc.cmajflt,  // 3
                       &proc.utime,    // 4
                       &proc.stime,    // 5
                       &proc.cutime,   // 6
                       &proc.cstime,   // 7
                       &proc.priority, // 8
                       &proc.nice,     // 9
                       &proc.num_threads, // 20
                       &proc.itrealvalue, // 1
                       &proc.starttime,   // 2
                       &proc.vsize,       // 3
                       &proc.rss,         // 4
                       &proc.rlim,        // 5
                       &proc.startcode,   // 6
                       &proc.endcode,     // 7
                       &proc.startstack,  // 8
                       &proc.kstkesp,     // 9
                       &proc.kstkeip,     // 30
                       &proc.signal,      // 1
                       &proc.blocked,     // 2
                       &proc.sigignore,   // 3
                       &proc.sigcatch,    // 4
                       &proc.wchan        // 5
                       );
    if ( ret != 35 ) {
      lib_rtl_output(LIB_RTL_ERROR,"Failed to convert process information for PID: %d",proc_id);
    }
    //::printf("%d PID:%d Vsize:%ld RSS:%ld Rlim:%ld\n",ret,pid,vsize,rss,rlim);
  }
  return 1;
}

/// Print process data from proc file system
void SysProcess::print() const {
#define _PRT(x)  cout << setw(16) << left << #x << ":" << right << setw(32) << x << endl
  _PRT(pid);      // 1
  _PRT(comm);      // 2
  _PRT(state);    // 3
  _PRT(ppid);     // 4
  _PRT(pgrp);     // 5
  _PRT(session);  // 6
  _PRT(tty);      // 7
  _PRT(tpgid);    // 8
  _PRT(flags);    // 9
  _PRT(minflt);   // 10
  _PRT(cminflt);  // 1
  _PRT(majflt);   // 2
  _PRT(cmajflt);  // 3
  _PRT(utime);    // 4
  _PRT(stime);    // 5
  _PRT(cutime);   // 6
  _PRT(cstime);   // 7
  _PRT(priority); // 8
  _PRT(nice);     // 9
  _PRT(num_threads); // 20
  _PRT(itrealvalue); // 1
  _PRT(starttime);   // 2
  _PRT(vsize);       // 3
  _PRT(rss);         // 4
  _PRT(rlim);        // 5
  _PRT(startcode);   // 6
  _PRT(endcode);     // 7
  _PRT(startstack);  // 8
  _PRT(kstkesp);     // 9
  _PRT(kstkeip);     // 30
  _PRT(signal);      // 1
  _PRT(blocked);     // 2
  _PRT(sigignore);   // 3
  _PRT(sigcatch);    // 4
  _PRT(wchan);      // 5
}

/// Read memory information block
int RTL::read(SystemMemory& memory) {
  char buff[4096];
  int cnt = SysFile("/proc/meminfo").read(buff,sizeof(buff));
  if( cnt > 0 )  {
    int *is = &memory.memTotal, *ie = &memory._last;
    for(char* p=buff, *item; p!=0 && p<buff+cnt && is < ie; ++is, p=::strchr(item,'\n')) {
      while(*p=='\n')++p;
      if ( (item=::strchr(p,':')) )  {
        for(++item;::isspace(*item);)++item;
        ::sscanf(item,"%d",is);
      }
    }
    return 1;
  }
  return 0;
}

/// Read system data from proc file system: current CPU information
int RTL::read(vector<SystemCPU>& info) {
  size_t ncores = 0;
  char buff[12*4096], *p, *q, *desc, *item;
  int cnt = SysFile("/proc/cpuinfo").read(buff,sizeof(buff));
  if(cnt>0)  {
    SystemCPU cpu;
    for( p=buff; p!=0 && p<buff+cnt; ) {
      desc = p;
      if ( (p=::strchr(desc,'\n')) ) { *p = 0; ++p; }
      item = ::strchr(desc,':');
      if ( item && item < p )  {
        if ( (q=::strchr(desc,'\t')) ) *q = 0;
        if ( ::strncmp(desc,"power management",16)==0 )  {
          /// This is the last item. get the next bugger
          info.push_back(cpu);
          cpu.reset();
          ++ncores;
          continue;
        }
        *item = 0;
        item += 2;
        char dd = desc[0];
        if ( dd == 'p' )  {
          if ( ::strncmp(desc,"processor",sizeof("processor"))==0 )
            cpu.processor = (int)::atoi(item);
          else if ( ::strncmp(desc,"physical id",sizeof("physical id"))==0 )
            cpu.physID = (int)::atoi(item);
        }
        else if ( dd == 'c' )  {
          if ( ::strncmp(desc,"cpu MHz",sizeof("cpu MHz"))==0 )
            cpu.clock = (float)::atof(item);
          else if ( ::strncmp(desc,"cache size",sizeof("cache size"))==0 )
            cpu.cache = (int)::atoi(item);
          else if ( ::strncmp(desc,"core id",sizeof("core id"))==0 )
            cpu.coreID = (int)::atoi(item);
          else if ( ::strncmp(desc,"cpu cores",sizeof("cpu cores"))==0 )
            cpu.numCore = (int)::atoi(item);
          else if ( ::strncmp(desc,"cpu family",sizeof("cpu family"))==0 )
            cpu.family = (int)::atoi(item);
        }
        else if ( dd == 'b' && ::strncmp(desc,"bogomips",sizeof("bogomips"))==0 )  {
          cpu.bogomips = (float)::atof(item);
        }
      }
    }
    return ncores;
  }
  return 0;
}

namespace RTL {
  template <> void ExtractCPUInfo<SYSTEMCPU_EXTRACT_CPUSLOTS>::operator()(const SystemCPU& cpu) 
  {  result.insert(cpu.physID);      }
  template <> void ExtractCPUInfo<SYSTEMCPU_EXTRACT_PROCESSORS>::operator()(const SystemCPU& cpu) 
  {  if ( select_slot < 0 || select_slot == cpu.physID ) result.insert(cpu.processor);   }
  template <> void ExtractCPUInfo<SYSTEMCPU_EXTRACT_CPUCORES>::operator()(const SystemCPU& cpu) 
  {  if ( select_slot < 0 || select_slot == cpu.physID ) result.insert(cpu.coreID);      }
  template <> void ExtractCPUInfo<SYSTEMCPU_PRINT>::operator()(const SystemCPU& cpu) 
  {  if ( select_slot < 0 || select_slot == cpu.physID ) cpu.print();  }
}

/// Retrive the physical CPU IDs of a node from the SystemCPU blocks
set<int> RTL::physical_cpu_ids(const vector<SystemCPU>& data)  {
  ExtractCPUInfo<SYSTEMCPU_EXTRACT_CPUSLOTS> actor;
  actor = for_each(data.begin(), data.end(), actor);
  return actor.result;
}

/// Retrive the core IDs of a node from the SystemCPU blocks
set<int> RTL::cpucore_ids(const vector<SystemCPU>& data, int cpu_slot)  {
  ExtractCPUInfo<SYSTEMCPU_EXTRACT_CPUCORES> actor(cpu_slot);
  actor = for_each(data.begin(), data.end(), actor);
  return actor.result;
}

/// Retrive the processor IDs of a node from the SystemCPU blocks
set<int> RTL::processor_ids(const vector<SystemCPU>& data, int cpu_slot)  {
  ExtractCPUInfo<SYSTEMCPU_EXTRACT_PROCESSORS> actor(cpu_slot);
  actor = for_each(data.begin(), data.end(), actor);
  return actor.result;
}

extern "C" int rtl_test_cpuinfo(int,char**) {
  vector<SystemCPU> info;
  ::system("cat /proc/cpuinfo");
  if ( 0 != read(info) )  {
    stringstream str;
    ostream& os = str;
    ExtractCPUInfo<SYSTEMCPU_PRINT> printer(-1);
    for_each(info.begin(), info.end(), printer);
    ExtractCPUInfo<SYSTEMCPU_EXTRACT_CPUSLOTS> actor(-1);
    actor = for_each(info.begin(), info.end(), actor);
    ::lib_rtl_output(LIB_RTL_ALWAYS,"Got a total of %d CPU slots %s\n",
                     int(actor.result.size()));

    for(set<int>::iterator i=actor.result.begin(); i!=actor.result.end(); ++i)  {
      int slot = *i;
      ExtractCPUInfo<SYSTEMCPU_EXTRACT_PROCESSORS> pact(slot);
      pact = for_each(info.begin(), info.end(), pact);
      str.str("");
      os << pact;
      ::lib_rtl_output(LIB_RTL_ALWAYS,"Got %d processors for CPU slot %s\n",
                       int(pact.result.size()),str.str().c_str());
      ExtractCPUInfo<SYSTEMCPU_EXTRACT_CPUCORES>  cact(slot);
      cact = for_each(info.begin(), info.end(), cact);
      str.str("");
      os << cact;
      ::lib_rtl_output(LIB_RTL_ALWAYS,"Got %d cores      for CPU slot %s\n",
                       int(cact.result.size()),str.str().c_str());      
    }
  }
  else
    ::lib_rtl_output(LIB_RTL_ERROR,"Failed to read cpu info: /proc/cpuinfo");
  return 1;
}

extern "C" int rtl_test_statusProc(int,char**) {
  char txt[32];
  StatusProcess proc;
  int pid = ::lib_rtl_pid();
  ::sprintf(txt,"cat %s",fn_process_status(pid));
  if ( 0 != read(proc,pid) )
    proc.print();
  else
    ::lib_rtl_output(LIB_RTL_ERROR,"Failed to read status info: %s.",fn_process_status(pid));
  ::system(txt);
  return 1;
}

extern "C" int rtl_test_statProc(int,char**) {
  char txt[32];
  SysProcess proc;
  int pid = ::lib_rtl_pid();
  ::sprintf(txt,"cat %s",fn_process_stat(pid));
  if ( 0 != read(proc,pid) )
    proc.print();
  else
    ::lib_rtl_output(LIB_RTL_ERROR,"Failed to read status info: %s.",fn_process_stat(pid));
  ::system(txt);
  return 1;
}

extern "C" int rtl_test_envProc(int,char**) {
  char txt[32];
  EnvironProcess proc;
  int pid = ::lib_rtl_pid();
  ::sprintf(txt,"cat %s",fn_process_environ(pid));
  if ( 0 != read(proc,pid) )
    proc.print();
  else
    ::lib_rtl_output(LIB_RTL_ERROR,"Failed to read: %s.",fn_process_environ(pid));
  ::system(txt);
  return 1;
}

extern "C" int rtl_test_systemUptime(int,char**) {
  SystemUptime proc;
  if ( 0 != read(proc) ) proc.print();
  ::system("cat /proc/uptime");
  return 1;
}

extern "C" int rtl_test_systemUtgid(int,char**) {
  UtgidProcess proc;
  if ( 0 != read(proc,::lib_rtl_pid()) ) proc.print();
  return 1;
}

extern "C" int rtl_test_system_memory(int,char**) {
  SystemMemory m;
  if ( 0 != read(m) ) m.print();
  return 1;
}
