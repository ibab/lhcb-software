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
  static const char* fn_process_dir(int pid) {
    static char txt[32];
    ::sprintf(txt,"/proc/%d",pid);
    return txt;
  }
}

_PROCFILE_NAME(64,cmdline)

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
  static int pgSize = sysconf(_SC_PAGESIZE);
  int jiffy2second = sysconf(_SC_CLK_TCK);
  int pid, retry = 5, cnt;
  char buff[2048], pwdbuff[1024], *ptr;
  time_t now = ::time(0);
  struct dirent* dp;
  DIR *dir = 0;
  SystemUptime sys;

 Again:
  if ( dir ) ::closedir(dir);
  dir=::opendir("/proc");
  Procset::Processes::iterator pr = procset.reset()->processes.begin();
  Procset::Processes::iterator start = pr;
  if ( dir && read(sys) )    {
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
              if ( 0 == ret && RTL::read(proc,pid) ) {
                Process& p = (*pr);
                utgid.utgid = "";
                try {
                  RTL::read(utgid,pid);
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
                p.start   = int(float(now) - sys.uptime + float(proc.starttime)/jiffy2second);
                p.mem     = 0.0;
                p.stack   = (float)(proc.startstack-proc.kstkesp)/1024e0;
                p.vsize   = (float)proc.vsize/1024e0;
                p.rss     = proc.rss*pgSize/1024e0;
                p.state   = proc.state;
                p.pid     = pid;
                p.ppid    = (unsigned short)proc.ppid;
                p.threads = (unsigned short)proc.num_threads;
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
  if ( dir ) ::closedir(dir);
  return 0;
}
