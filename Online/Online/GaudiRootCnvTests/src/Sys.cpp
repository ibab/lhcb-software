#include "Sys.h"
#include <stdexcept>
//#include <iostream>
//#include <iomanip>
#include <cstring>
#include <cstdio>
#include <cerrno>
//#include <map>
#include <fcntl.h>
//#include "dirent.h"

#ifdef _WIN32
#include <io.h>
#include <process.h>
#include <sys/stat.h>

#define O_RDONLY _O_RDONLY
//#define _SC_PAGESIZE 1
//#define _SC_CLK_TCK 0
//#define sysconf(x)  4096
#else
#include <unistd.h>
//#include <pwd.h>
//#include <sys/times.h>
//#include <sys/param.h>
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

  _PROCESSFILE(64,stat)
  _PROCESSFILE(64,status)
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
      ::printf("Failed to convert process information for PID:%d\n",proc_id);
    }
    //::printf("%d PID:%d Vsize:%ld RSS:%ld Rlim:%ld\n",ret,pid,vsize,rss,rlim);
  }
  return 1;
}
