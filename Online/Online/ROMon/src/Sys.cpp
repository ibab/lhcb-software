#include "ROMon/Sys.h"
#include "ROMonDefs.h"
#include "RTL/rtl.h"
#include "RTL/readdir.h"

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <map>
#include <fcntl.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <io.h>
#include <process.h>
namespace {
  struct passwd {char pw_name[16];};
  passwd* getpwuid(int) { return 0; }
  int getpwuid_r(int uid, passwd *, char *, size_t, passwd**) { return 1; }
}
#define O_RDONLY _O_RDONLY
#define _SC_PAGESIZE 1
#define _SC_CLK_TCK 0
#define sysconf(x)  4096
#else
#include <unistd.h>
#include <pwd.h>
#include <sys/times.h>
#include <sys/param.h>
#endif

using namespace std;
using namespace ROMon;


namespace {
#define _PROCESSFILE(len,name)                                \
static const char* fn_process_##name (int pid) {\
  static char txt[len]; \
  sprintf(txt,"/proc/%d/"#name,pid); \
  return txt; \
}

  static const char* fn_process_dir(int pid) {
    static char txt[32];
    ::sprintf(txt,"/proc/%d",pid);
    return txt;
  }

  _PROCESSFILE(64,stat)
  _PROCESSFILE(64,status)
  _PROCESSFILE(64,cmdline)
  _PROCESSFILE(64,environ)
}

int SysFile::read(char* buf, size_t siz) const  {
  int fd;
  if((fd = ::open(m_name.c_str(),O_RDONLY))<0)  {
    string err = "Failed to open "+m_name+" ";
    throw runtime_error(err+::strerror(errno));
  }
  size_t tmp = 0;
  while ( tmp < siz )  {
    int sc = ::read(fd,buf+tmp,siz-tmp);
    if ( sc >  0 ) {
      tmp += sc;
    }
    else if ( sc == 0 )  {
      buf[tmp] = 0;
      ::close(fd);
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
  string err = "Read of Proc file "+m_name+" failed:";
  err += ::strerror(errno);
  ::close(fd);
  throw runtime_error(err);
}

int SystemUptime::read() {
  char buff[256];
  int cnt = SysFile("/proc/uptime").read(buff,sizeof(buff));
  if ( cnt>0 ) {
    ::sscanf(buff,"%f %f",&uptime,&idletime);
    return 1;
  }
  return 0;
}

void SystemUptime::print() const {
  ::printf("System uptime:%f   idle time:%f\n",uptime,idletime);
}

/// Read system data from proc file system
int EnvironProcess::read(int proc_id) {
  size_t len = 1024*100;
  char* buff = new char[len], *ptr=buff, *end;
  int cnt = SysFile(fn_process_environ(proc_id)).read(buff,len);
  env.clear();
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
        env.push_back(make_pair(tag,++val));
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

/// Read system data from proc file system
int UtgidProcess::read(int proc_id) {
  size_t len = 1024*100;
  char* buff = new char[len], *ptr=buff, *end;
  utgid = "";
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
            utgid = val;
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
  catch(const std::exception& e) {
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

int StatusProcess::read(int proc_id) {
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
      case 1:   ::sscanf(p,"%s",comm);          break;
      case 2:   ::sscanf(p,"%c",&state);        break;
      case 3:   ::sscanf(p,"%d%%",&sleepAvg);   break;
      case 4:   ::sscanf(p,"%d",&tgid);         break;
      case 5:   ::sscanf(p,"%d",&pid);          break;
      case 6:   ::sscanf(p,"%d",&ppid);         break;
      case 8:   ::sscanf(p,"%d",&uid);          break;
      case 9:   ::sscanf(p,"%d",&gid);          break;
      case 10:  ::sscanf(p,"%d",&fdSize);       break;
      case 12:  ::sscanf(p,"%d",&vmPeak);       break;
      case 13:  ::sscanf(p,"%d",&vmSize);       break;
      case 14:  ::sscanf(p,"%d",&vmLock);       break;
      case 15:  ::sscanf(p,"%d",&vmRSS);        break;
      case 16:  ::sscanf(p,"%d",&vmHWM);        break;
      case 17:  ::sscanf(p,"%d",&vmData);       break;
      case 18:  ::sscanf(p,"%d",&vmStack);      break;
      case 19:  ::sscanf(p,"%d",&vmExe);        break;
      case 20:  ::sscanf(p,"%d",&vmLib);        break;
      case 21:  ::sscanf(p,"%d",&vmPTE);        break;
      case 22:  ::sscanf(p,"%08lx",&staBrk);    break;
      case 23:  ::sscanf(p,"%08lx",&brk);       break;
      case 24:  ::sscanf(p,"%08lx",&staStk);    break;
      case 25:  ::sscanf(p,"%d",&nThreads);     break;
      case 26:  break; // SigQ
      case 27:  ::sscanf(p,"%016lx",&sigPend);  break;  // Fmt: SigCgt: 00000001808044e9
      case 28:  ::sscanf(p,"%016lx",&shdPend);  break;
      case 29:  ::sscanf(p,"%016lx",&sigBlk);   break;
      case 30:  ::sscanf(p,"%016lx",&sigIgn);   break;
      case 31:  ::sscanf(p,"%016lx",&sigCgt);   break;
      case 32:  ::sscanf(p,"%016lx",&capInh);   break;
      case 33:  ::sscanf(p,"%016lx",&capPrm);   break;
      case 34:  ::sscanf(p,"%016lx",&capEff);   break;
      default:                                  break;
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

/// Read process data from proc file system
int SysProcess::read(int proc_id) {
  char buff[1024];
  int cnt = SysFile(fn_process_stat(proc_id)).read(buff,sizeof(buff));
  if(cnt>0)  {
    //                       1  2  3  4  5  6  7  8  9   10  1   2   3   4   5   6   7   8   9   20  1   2   3   4   5   6   7   8   9   30  1   2   3   4   5
    int ret = ::sscanf(buff,"%d %s %c %d %d %d %d %d %lu %lu %lu %lu %lu %lu %lu %lu %lu %ld %ld %ld %lu %lu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
             &pid,      // 1
             comm,      // 2
             &state,    // 3
             &ppid,     // 4
             &pgrp,     // 5
             &session,  // 6
             &tty,      // 7
             &tpgid,    // 8
             &flags,    // 9
             &minflt,   // 10
             &cminflt,  // 1
             &majflt,   // 2
             &cmajflt,  // 3
             &utime,    // 4
             &stime,    // 5
             &cutime,   // 6
             &cstime,   // 7
             &priority, // 8
             &nice,     // 9
             &num_threads, // 20
             &itrealvalue, // 1
             &starttime,   // 2
             &vsize,       // 3
             &rss,         // 4
             &rlim,        // 5
             &startcode,   // 6
             &endcode,     // 7
             &startstack,  // 8
             &kstkesp,     // 9
             &kstkeip,     // 30
             &signal,      // 1
             &blocked,     // 2
             &sigignore,   // 3
             &sigcatch,    // 4
             &wchan        // 5
             );
    if ( ret != 35 ) {
      log() << "Failed to convert process information for PID:" << proc_id << endl;
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
int ROMon::read(Memory& memory) {
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

int ROMon::readInfo(CPUset& info, size_t max_len) {
  size_t ncores = 0;
  char buff[12*4096], *p, *q, *desc, *item;
  int cnt = SysFile("/proc/cpuinfo").read(buff,sizeof(buff));
  if(cnt>0)  {
    //const char* core_id="";
    CPUset::Cores::iterator corIt = info.cores.reset();
    CPUset::Cores::iterator begin = corIt;
    ro_get_node_name(info.name,sizeof(info.name));
    for( p=buff; p!=0 && p<buff+cnt; ) {
      desc = p;
      if ( (p=::strchr(desc,'\n')) ) { *p = 0; ++p; }
      item = ::strchr(desc,':');
      if ( item && item < p )  {
        if ( (q=::strchr(desc,'\t')) ) *q = 0;
        if ( ::strncmp(desc,"power management",16)==0 )  {
          /// This is the last item. get the next bugger
          corIt = info.cores.add(corIt)->reset();
          if ( ((char*)corIt)-((char*)begin) > (int)max_len ) {
            log() << "CPU Buffer memory too small.....exiting" << endl;
            break;
          }
          ++ncores;
          continue;
        }
        *item = 0;
        item += 2;
        if ( ::strncmp(desc,"cache size",sizeof("cache size"))==0 )  {
          (*corIt).cache = (int)::atoi(item);
        }
        else if ( ::strncmp(desc,"cpu MHz",sizeof("cpu MHz"))==0 )  {
          (*corIt).clock = (float)::atof(item);
        }
        else if ( ::strncmp(desc,"bogomips",sizeof("bogomips"))==0 )  {
          (*corIt).bogomips = (float)::atof(item);
        }
        else if ( ::strncmp(desc,"vendor_id",sizeof("vendor_id"))==0 )  {
          ::strncpy(info.family,item,sizeof(info.family));
          info.family[sizeof(info.family)-1] = 0;
        }
        else if ( ::strncmp(desc,"core id",sizeof("core id"))==0 ) {
          //core_id = item;
        }
        /*
          else if ( ::strncmp(desc,"model name",sizeof("model"))==0 ) ;
          else if ( ::strncmp(desc,"physical id",sizeof("physical id"))==0 ) ;
        */
      }
    }
    //log() << "Got " << info.cores.size() << " cores  " << (int)(p-buff) << " Bytes " << cnt << endl;
    return ncores;
  }
  return 0;
}


/// Read current CPU statistics
int ROMon::readStat(CPUset& info, size_t max_len, size_t num_cores) {
  int jiffy2second = sysconf(_SC_CLK_TCK);
  char buff[4*4096];
  long cnt, li[10];
  size_t ncore=0;

  if((cnt=SysFile("/proc/stat").read(buff,sizeof(buff)))>0)  {
    CPU::Stat* st;
    bool need_alloc = false;//info.cores.begin()==info.cores.end();
    CPUset::Cores::iterator ci = info.cores.begin();
    CPUset::Cores::iterator begin = ci;
    ro_gettime(&info.time,&info.millitm);
    for (char *p=buff, *desc=p, *end=buff+cnt; p!=0 && p<end; desc=++p ) {
      if ( (p=::strchr(desc,'\n')) ) *p = 0;
      switch(desc[0]) {
      case 'b':     // BOOT TIME
        sscanf(desc,"%d",&info.boot);
        break;
      case 'c':
        switch(desc[1]) {
        case 'p':   // CPUn
          st = desc[3]==' ' ? &info.averages : &((*ci).stats);
          // log() << "Core data:" << desc << endl;
          sscanf(desc+5,"%ld %ld %ld %ld %ld %ld %ld",&li[0],&li[1],&li[2],&li[3],&li[4],&li[5],&li[6]);
          st->user    = (float)(li[0]*1000.)/jiffy2second;
          st->nice    = (float)(li[1]*1000.)/jiffy2second;
          st->system  = (float)(li[2]*1000.)/jiffy2second;
          st->idle    = (float)(li[3]*1000.)/jiffy2second;
          st->iowait  = (float)(li[4]*1000.)/jiffy2second;
          st->IRQ     = (float)(li[5]*1000.)/jiffy2second;
          st->softIRQ = (float)(li[6]*1000.)/jiffy2second;
          if ( desc[3] != ' ' ) {
            if ( ++ncore > num_cores ) return 1;
            ci = need_alloc ? info.cores.add(ci) : info.cores.next(ci);
            if ( ((char*)st)-((char*)begin) > (int)max_len ) {
              log() << "CPU Buffer memory too small.....exiting" << endl;
              break;
            }
          }
        break;
        case 't':   // CTXT
          sscanf(desc+5,"%ld",&li[0]);
          info.ctxtRate = (float)li[0];
          break;
        default:
          break;
        }
      case 'i':     // INTR: first is total interrupt rate
        //::sscanf(desc+5,"%ld",&li[0]);
        break;
      case 'p':     // PROC
        //if ( ::strcmp(desc,"processes")==0 ) {}
        //else if ( ::strcmp(desc,"procs_running")==0 ) {}
        //else if ( ::strcmp(desc,"procs_blocked")==0 ) {}
        break;
      default:
        break;
      }
    }
    ci = need_alloc ? info.cores.add(ci) : info.cores.next(ci);
  }
  return 1;
}

int ROMon::read(CPUset& data, size_t max_len) {
  int res;
  data.reset();
  if ( (res=readInfo(data,max_len)) >= 1 ) {
    if ( readStat(data,max_len,res) == 1 ) {
      return res;
    }
  }
  return res;
}

int ROMon::read(Procset& procset, size_t max_len) {
  //static int pgSize = sysconf(_SC_PAGESIZE);
  int jiffy2second = sysconf(_SC_CLK_TCK);
  int pid, retry = 5, cnt;
  char buff[2048], pwdbuff[1024], *ptr;
  time_t now = ::time(0);
  struct dirent* dp;
  DIR *dir = 0;
  SystemUptime sys;

 Again:
  dir=::opendir("/proc");
  Procset::Processes::iterator pr = procset.reset()->processes.begin();
  Procset::Processes::iterator start = pr;
  if ( dir && sys.read() )    {
    SysProcess proc;
    UtgidProcess utgid;
    StatusProcess status;
    ro_gettime(&procset.time,&procset.millitm);
    ro_get_node_name(procset.name,sizeof(procset.name));
    do {
      if ((dp = readdir(dir)) != 0) {
        const char* n = dp->d_name;
        if ( *n && ::isdigit(*n) ) {
          struct stat st_buf;
          pid = ::atoi(n);
          if ( ::stat(fn_process_dir(pid),&st_buf)==0 ) {
            try {
              int ret = 0;
              string uname;
              static map<unsigned int,string> unames;
              map<unsigned int,string>::const_iterator iu=unames.find(st_buf.st_uid);
              if ( iu == unames.end() ) {
                struct passwd pw, *ppwd = 0;
                ret = ::getpwuid_r(st_buf.st_uid,&pw,pwdbuff,sizeof(pwdbuff),&ppwd);
                if ( 0 == ret ) {
                  if ( 0 != ppwd ) {
                    unames[st_buf.st_uid] = uname = ppwd->pw_name;
                  }
                  else {
                    unames[st_buf.st_uid] = uname = "<unknown>";
                  }
                }
                else {
                  continue;
                }
              }
              else {
                uname = (*iu).second;
              }
              if ( 0 == ret && proc.read(pid) && status.read(pid) ) {
                Process& p = (*pr);
                utgid.utgid = "";
                try {
                  utgid.read(pid);
                 }
                catch(...) {
                  utgid.utgid = "";
                }
                if((cnt=SysFile(fn_process_cmdline(pid)).read(buff,sizeof(buff))) > 0) {
                  for(ptr = buff+strlen(buff); ptr>buff && *ptr!='/';) --ptr;
                  if ( *ptr=='/' ) ++ptr;
                  while( (int)::strlen(buff) < (cnt-1) ) buff[strlen(buff)] = ' ';
                  ::strncpy(p.cmd,ptr,sizeof(p.cmd));
                  p.cmd[sizeof(p.cmd)-1] = 0;
                }
                else {
                  ::strncpy(p.cmd,proc.comm+1,sizeof(p.cmd));
                  p.cmd[sizeof(p.cmd)-1] = 0;
                  if ( (ptr=::strchr(p.cmd,')')) ) *ptr = 0;
                }
                ::strncpy(p.owner,uname.c_str(),sizeof(p.owner));
                p.owner[sizeof(p.owner)-1] = 0;
                ::strncpy(p.utgid,utgid.utgid.empty() ? "N/A" : utgid.utgid.c_str(),sizeof(p.utgid));
                p.utgid[sizeof(p.utgid)-1] = 0;

                // Note: seconds!!!
                p.cpu     = float(proc.stime+proc.utime)/float(jiffy2second);
                p.start   = int(now) - int(sys.uptime) + (proc.starttime/jiffy2second);
                p.mem     = 0.0;
                p.stack   = (float)status.vmStack;
                p.vsize   = (float)status.vmSize;
                p.rss     = (float)status.vmRSS;
                p.state   = status.state;
                p.pid     = (unsigned short)status.pid;
                p.ppid    = (unsigned short)status.ppid;
                p.threads = (unsigned short)status.nThreads;
#if 0
                log() << "ReadProc:" << (void*)pr << " PID:" << p.pid 
                      << " PPID:" << p.ppid 
                      << " CPU:" << p.cpu
                      << " cmd:" << proc.comm 
                      << endl;
#endif
                pr = procset.processes.add(pr);
                if ( ((char*)pr)-((char*)start) > (int)max_len ) {
                  log() << "PROC Buffer memory too small.....exiting" << endl;
                  break;
                }
              }
            }
            catch(exception& e) {
              log() << "Exception reading task information:" << e.what() << endl;
              if ( --retry>0 ) goto Again;
              dp = 0;
            }
            catch(...) {
              log() << "Unknown exception reading task information" << endl;
              if ( --retry>0 ) goto Again;
              dp = 0;
            }
          }
        }
      }
    } while (dp != NULL);
    ::closedir(dir);
    return 1;
  }
  return 0;
}


extern "C" int test_statusProc(int,char**) {
  char txt[32];
  StatusProcess p;
  int pid = ::lib_rtl_pid();
  ::sprintf(txt,"cat %s",fn_process_status(pid));
  if ( 0 != p.read(pid) )
    p.print();
  else
    cout << "Failed to read status info:" << fn_process_status(pid) << endl;
  ::system(txt);
  return 1;
}
extern "C" int test_statProc(int,char**) {
  char txt[32];
  SysProcess p;
  int pid = ::lib_rtl_pid();
  ::sprintf(txt,"cat %s",fn_process_stat(pid));
  if ( 0 != p.read(pid) )
    p.print();
  else
    cout << "Failed to read " << fn_process_stat(pid)  << "." << endl;
  ::system(txt);
  return 1;
}
extern "C" int test_envProc(int,char**) {
  char txt[32];
  EnvironProcess p;
  int pid = ::lib_rtl_pid();
  ::sprintf(txt,"cat %s",fn_process_environ(pid));
  if ( 0 != p.read(pid) )
    p.print();
  else
    cout << "Failed to read " << fn_process_environ(pid)  << "." << endl;
  ::system(txt);
  return 1;
}
extern "C" int test_systemUptime(int,char**) {
  SystemUptime p;
  if ( 0 != p.read() ) p.print();
  ::system("cat /proc/uptime");
  return 1;
}
extern "C" int test_systemUtgid(int,char**) {
  UtgidProcess p;
  if ( 0 != p.read(::lib_rtl_pid()) ) p.print();
  return 1;
}
extern "C" int test_system_memory(int,char**) {
  Memory m;
  if ( 0 != read(m) ) cout << m << endl;
  return 1;
}
