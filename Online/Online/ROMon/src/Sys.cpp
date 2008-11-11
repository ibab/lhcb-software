#include "ROMon/Sys.h"
#include "ROMonDefs.h"
#include "RTL/rtl.h"
#include "RTL/readdir.h"

#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <cerrno>
#include <fcntl.h>

#ifdef _WIN32
#include <process.h>
#include <io.h>
#define O_RDONLY _O_RDONLY
#define _SC_PAGESIZE 1
#define _SC_CLK_TCK 0
#define sysconf(x)  4096
#else
#include <unistd.h>
#include <pwd.h>
#include <sys/times.h>
#endif

using namespace std;
using namespace ROMon;


namespace {
#define _PROCESSFILE(len,name)   \
static const char* fn_process_##name (int pid) {\
  static char txt[len]; \
  sprintf(txt,"/proc/%d/"#name,pid); \
  return txt; \
}
  _PROCESSFILE(64,stat);
  _PROCESSFILE(64,cmdline);
};


/// read process data from proc file system
int SysProcess::read(int proc_id) {
  char buff[1024];
  int cnt = SysFile(fn_process_stat(proc_id)).read(buff,sizeof(buff));
  if(cnt>0)  {
    //                       1  2  3  4  5  6  7  8  9   10  1   2   3   4   5   6   7   8   9   20  1   2   3   4   5   6   7   8   9   30  1   2   3   4   5
    int ret = ::sscanf(buff,"%d %s %c %d %d %d %d %d %lu %lu %lu %lu %lu %lu %lu %lu %lu %ld %ld %ld %lu %lu %lu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
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
    else  {
      break;
    }
  }
  ::close(fd);
  string err = "Read of Proc file "+m_name+" failed:";
  throw runtime_error(err+::strerror(errno));
}

/// Read memory information block
int ROMon::read(Memory& memory) {
  char buff[4096];
  int cnt = SysFile("/proc/meminfo").read(buff,sizeof(buff));
  if( cnt > 0 )  {
    int *is = &memory.memTotal, *ie = &memory._last;
    for(char* p=buff, *item; p>0 && p<buff+cnt && is < ie; ++is, p=::strchr(item,'\n')) {
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
    char * core_id="";
    CPUset::Cores::iterator corIt = info.cores.reset();
    CPUset::Cores::iterator begin = corIt;
    ro_get_node_name(info.name,sizeof(info.name));
    for( p=buff; p>0 && p<buff+cnt; ) {
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
	  core_id = item;
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
    for (char *p=buff, *desc=p, *end=buff+cnt; p>0 && p<end; desc=++p ) {
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
  int pid, nthread, retry = 5;
  char buff[1024], txt[64], *ptr;
  struct dirent* dp, *dp2;
  struct tms buf;
  int sys_start = ::time(0) - ::times(&buf)/jiffy2second;

  DIR *dir = 0, *dir2;
 Again:
  dir=::opendir("/proc");
  Procset::Processes::iterator pr = procset.reset()->processes.begin();
  Procset::Processes::iterator start = pr;
  if ( dir )    {
    SysProcess proc;
    ro_gettime(&procset.time,&procset.millitm);
    ro_get_node_name(procset.name,sizeof(procset.name));
    do {
      if ((dp = readdir(dir)) != 0) {
	const char* n = dp->d_name;
	if ( *n && ::isdigit(*n) ) {
	  struct stat st_buf;
	  ::sprintf(txt,"/proc/%s",n);
	  if ( ::stat(txt,&st_buf)==0 ) {
	    pid = ::atoi(n);
	    try {
	      ::strcat(txt,"/task");
	      struct passwd *pw = ::getpwuid(st_buf.st_uid);
	      if ( proc.read(pid) ) {
		Process& p = (*pr);
		dir2 = 0;
		nthread = 0;
		dir2 = ::opendir(txt);
		if ( dir2 ) {
		  do {
		    if ((dp2 = ::readdir(dir2)) != 0) {
		      const char* thn = dp2->d_name;
		      if ( *thn && ::isdigit(*thn) ) ++nthread;
		    }
		  } while (dp2 != NULL);
		}
		if ( dir2 ) {
		  ::closedir(dir2);
		  dir2 = 0;
		}
		//::sprintf(txt,"/proc/%s/cmdline",n);
		if(SysFile(fn_process_cmdline(pid)).read(buff,sizeof(buff)) > 0)
		  ::strncpy(p.cmd,buff,sizeof(p.cmd));
		else
		  ::strncpy(p.cmd,proc.comm+1,sizeof(p.cmd));
		p.cmd[sizeof(p.cmd)-1] = 0;
		if ( (ptr=strchr(p.cmd,')')) ) *ptr = 0;
		if ( (ptr=::strchr(p.cmd,' ')) ) *ptr = 0;
		::strncpy(p.owner,pw->pw_name,sizeof(p.owner));
		p.owner[sizeof(p.owner)-1] = 0;
		// Note: seconds!!!
		p.cpu     = double(proc.stime+proc.utime)/double(jiffy2second);
		p.start   = sys_start - (proc.starttime/jiffy2second);
		p.mem     = 0.0;
		p.stack   = (float)(proc.kstkesp-proc.startstack);
		p.vsize   = (float)(proc.vsize);
		p.rss     = (float)(proc.rss*pgSize);
		p.state   = proc.state;
		p.pid     = proc.pid;
		p.ppid    = proc.ppid;
		p.threads = nthread;
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
	      if ( dir2 ) ::closedir(dir2);
	      dir2 = 0;
	      if ( --retry>0 ) goto Again;
	      dp = 0;
	    }
	    catch(...) {
	      log() << "Unknown exception reading task information" << endl;
	      if ( dir2 ) ::closedir(dir2);
	      dir2 = 0;
	      if ( --retry>0 ) goto Again;
	      dp = 0;
	    }
	  }
	}
      }
    } while (dp != NULL);
    if ( dir2 ) ::closedir(dir2);
    dir2 = 0;
    ::closedir(dir);
    return 1;
  }
  return 0;
}
