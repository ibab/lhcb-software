#include "ROLogger/PVSSLogger.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <memory>

#include "RTL/rtl.h"
#include "RTL/readdir.h"
#include <fcntl.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#include <direct.h>
#define O_NONBLOCK 0x0
#define W_OK       0x6
inline bool S_ISFIFO(int) { return false; }
#endif

using namespace ROLogger;
using namespace std;

namespace ROLogger {
  class LogFileEntry {
  public:
    struct stat   data;
    size_t        pointer;
    int           fd;
    string        remainder;
    LogFileEntry() : pointer(0), fd(0)     {    
      ::memset(&data,0,sizeof(data));
    }
    LogFileEntry(const string& name) : fd(0)    {
      ::memset(&data,0,sizeof(data));
      ::stat(name.c_str(),&data);
      pointer = data.st_size;
    }
    LogFileEntry(const struct stat& st) : data(st), fd(0)    {
      pointer = data.st_size;
    }
    LogFileEntry(const LogFileEntry& c) : data(c.data), pointer(c.pointer), fd(c.fd)   {    
    }
    ~LogFileEntry()     {    
      if ( fd != 0 ) ::close(fd);
    }
    int open(const string& fn, bool begin) {
      fd = ::open(fn.c_str(),O_RDONLY);
      pointer = begin ? 0 : data.st_size;
      if ( fd != -1 ) {
        if ( !begin ) ::lseek(fd,0,SEEK_END);
        return 1;
      }
      fd = 0;
      return 0;
    }
  };
}

string PVSSLogger::_prefix(const string& tag) const {
  char prefix[132];
  time_t tim = ::time(0);
  tm* now = ::localtime(&tim);
  ::strftime(prefix,sizeof(prefix),"%b%d-%H%M%S[",now);
  ::strcat(prefix,tag.c_str());
  ::strcat(prefix,"] ");
  ::strcat(prefix,m_node.c_str());
  ::strcat(prefix,": ");
  return prefix;
}

/// Initializing constructor
PVSSLogger::PVSSLogger(const string& nam)
: m_name(nam), m_node("Unknown"), m_fifo(-1)
{
  m_checkLimit = 3;
  m_maxFileChange = 7*24*3600;
  m_node = RTL::nodeNameShort();
  m_process = RTL::processName();
  ::chdir(m_name.c_str());
}

/// Default destructor
PVSSLogger::~PVSSLogger()
{
  if ( m_fifo != -1 ) {
    ::close(m_fifo);
    m_fifo = -1;
  }
}

/// Connect output fifo
int PVSSLogger::connect(const string& fifo) {
  struct stat statBuf;
  if( ::access(fifo.c_str(),W_OK) == -1 )    { // access denied
    ::printf("access(\"%s\"): %s!\n",fifo.c_str(),::strerror(errno));
    if ( errno == ENOENT )
      ::printf("Please, run \"logSrv -p %s -s <srv_name>\"!\n",fifo.c_str());
    return 0;
  }
  // get fifo information
  if( ::stat(fifo.c_str(),&statBuf )==-1)  {
    ::printf("stat(\"%s\"): %s!\n",fifo.c_str(),::strerror(errno));
    return 0;
  }
  // check if fifo is a FIFO */
  if( !S_ISFIFO(statBuf.st_mode) )  {
    ::printf("I-node: \"%s\" is not a FIFO!\n",fifo.c_str());
    return 0;
  }
  // open error log 
  m_fifo = ::open(fifo.c_str(),O_WRONLY|O_NONBLOCK|O_APPEND);
  if ( m_fifo==-1 )  {
    if(errno==ENXIO)    {
      ::printf("No process has the FIFO \"%s\" open for reading!\n",fifo.c_str());
      ::printf("Please, run \"logSrv -p %s -s <srv_name>\"!\n",fifo.c_str());
    }
    else    {
      ::printf("open(\"%s\"): %s!\n",fifo.c_str(),::strerror(errno));
    }
    return 0;
  }
  return 1;
}

/// Read directory content
int PVSSLogger::read(bool prt) {
  string prefix = _prefix();
  DIR* dir=::opendir(m_name.c_str());
  if ( dir ) {
    time_t now = time(0);
    struct dirent* dp;
    while( (dp=::readdir(dir)) != 0 ) {
      string fn = dp->d_name;
      size_t idx = fn.rfind(".");
      if ( fn.substr(idx) == ".log" ) {
        Files::iterator i=m_files.find(fn);
        if ( i == m_files.end() ) {
          auto_ptr<LogFileEntry> e(new LogFileEntry(m_name+"/"+fn));
          if ( now-e->data.st_mtime < m_maxFileChange ) {
            // file needs to be modified at least during the last week.....
            m_files[fn] = e.release();
          }
          else if ( prt ) {
            const struct stat& s = e->data;
            string mod = ::ctime(&e->data.st_mtime);
            stringstream os;
            os << prefix << left << setw(28) 
              << fn.substr(0,fn.length()-4)+": " 
              << "Last Modified: " << mod.substr(0,mod.length()-1)
              << " Size:" << setw(9) << right << s.st_size
              << " INode:" << setw(6) << right << s.st_ino
              << "   ---> File too old and ignored"
              << endl;
            print(os.str());
          }
        }
      }
    }
    ::closedir(dir);
    m_lastChecked = time(0);
    return 1;
  }
  return 0;
}

/// Dump directory content
void PVSSLogger::dump() {
  string prefix = _prefix();
  for(Files::const_iterator i=m_files.begin(); i!=m_files.end();++i)  {
    const struct stat& s = (*i).second->data;
    const string& fn = (*i).first;
    string mod = ::ctime(&s.st_mtime);
    stringstream os;
    os << prefix << left << setw(28) 
      << fn.substr(0,fn.length()-4)+": " 
      << "Last Modified: " << mod.substr(0,mod.length()-1)
      << " Size:" << setw(9) << right << s.st_size
      << " INode:" << setw(6) << right << s.st_ino
      << endl;
    print(os.str());
  }
}

/// Open all log files
int PVSSLogger::open(bool begin, bool prt) {
  int ret = 1;
  for(Files::iterator i=m_files.begin(); i!=m_files.end();++i)  {
    if ( (*i).second->fd == 0 ) {
      if ( !(*i).second->open((*i).first,begin) ) 
        ret = 0;
      else if ( prt )
        print("Opened successfully log file:"+(*i).first+"\n");
    }
  }
  return ret;
}

/// Close all log files
int PVSSLogger::close() {
  for(Files::iterator i=m_files.begin(); i!=m_files.end();++i)
    delete (*i).second;
  m_files.clear();
  return 1;
}

/// Print message to output device
int PVSSLogger::print(const string& msg) {
  //cout << msg << endl;
  const char* p = msg.c_str();
  for(size_t wr=msg.length(), written=0; written<wr; ) {
    int w = ::write(m_fifo,p+written,wr-written);
    if ( w > 0 ) written += w;
    else if ( errno != EAGAIN ) {
      ::lib_rtl_sleep(50);
    }
  }
  return 1;
}

/// Print content of log files as they come....
int PVSSLogger::publish() {
  size_t bytes, rd, buff_len = 0;
  char *buff = 0;
  string prefix = _prefix();
  string pref_err = _prefix("ERROR");
  string pref_warn = _prefix("WARN");

  ::lib_rtl_sleep(50);
  for(Files::iterator i=m_files.begin(); i!=m_files.end();++i)  {
    const string& fn = (*i).first;
    LogFileEntry* e = (*i).second;
    int fd = e->fd;
    int res = ::fstat(fd,&e->data);
    if ( res < 0 ) {
      cout << "Failed to stat " << fn << endl;
      continue;
    }
    bytes = e->data.st_size-e->pointer;
    if ( bytes>0 ) {
      if ( bytes+1 > buff_len ) {
        if ( buff ) delete [] buff;
        buff = new char[buff_len=bytes+1];
      }
      for(rd=0; rd<bytes; ) {
        int cnt = ::read(fd,buff+rd,bytes-rd);
        if ( cnt > 0 ) rd += cnt;
        else if ( errno == EINTR ) continue;
        else break;
      }
      e->pointer += bytes;
      if ( rd > 0 ) {
        char *p, *p0 = buff;
        for(size_t j=0; j<bytes;++j) {
          if ( buff[j]==0 ) buff[j] = ' ';
        }
        buff[bytes] = 0;
        while ( (p=::strchr(p0,'\n')) != 0 ) {
          *p = 0;
          size_t len = ::strlen(p0)+e->remainder.length();
          bool use = len > 0;
          if ( ::strncmp(fn.c_str(),"smi",3)==0 ) {
            if ( ::strstr(p0,"_FWDM>") != 0 ) {
              use = use && ::strstr(p0,"<DISABLE/DEVICE") == 0;
              use = use && ::strstr(p0,"<ERROR>") == 0;
            }
            if ( ::strstr(p0,"_FWM>") != 0 ) {
              use = use && ::strstr(p0,"<INCLUDED>") == 0;
              use = use && ::strstr(p0,"executing action : ") == 0;
            }
            use = use && ::strstr(p0,"External action :") == 0;
            use = use && ::strstr(p0,"executing action : <LOAD/PART_NAME=") == 0;
          }
          use = use && ::strstr(p0,"Started Gaudijob ") == 0;
          use = use && ::strstr(p0,"parenttype ") == 0;
          use = use && ::strstr(p0,"<GAUDIJOB:") == 0;
          if ( use ) {
            stringstream os;
            string pref = prefix;
            if ( strstr(p0,"ERROR") )
              pref = pref_err;
            else if ( strstr(p0,"> can not execute action <") )
              pref = pref_warn;
            os << left << setw(60) << pref+fn.substr(0,fn.length()-4)+": " 
              << e->remainder << p0 << endl;
            print(os.str());
          }
          e->remainder = "";
          p0 = ++p;
        }
        e->remainder = (p0 < buff+bytes) ? p0 : "";
      }
    }
  }
  if ( buff ) delete [] buff;
  checkFiles();
  return 1;
}

/// Check directory for new log files
void PVSSLogger::checkFiles() {
  if ( ::time(0)-m_lastChecked > m_checkLimit ) {
    for(Files::iterator i=m_files.begin(); i!=m_files.end();++i)  {
      struct stat buf;
      const string& fn = (*i).first;
      LogFileEntry* e = (*i).second;
      int res = ::stat(fn.c_str(),&buf);
      if ( 0 == res  ) {
        if ( buf.st_ino == e->data.st_ino ) {
          continue;
        }
        print("Log file:"+fn+" changed. Closing old entry.\n");
        (*i).second = new LogFileEntry(buf);
        (*i).second->open(fn,true);
        continue;
      }
      print("Failed to stat "+fn+" Removing log file from check-list....\n");
      m_files.erase(i);
      i = m_files.begin();
      delete e;
    }
    read(false);
    open(true,true);
  }
}

extern "C" int romon_pvss_logger(int, char** argv) {
  //PVSSLogger dir("/localdisk/pvss/STORAGE/log");
  PVSSLogger dir(argv[1]);
  if ( dir.connect("/tmp/logPVSS.fifo") ) {
    if ( dir.read(true) ) {
      dir.dump();
      if ( dir.open(false,false) ) {
        while(1) {
          dir.publish();
        }
      }
    }
  }
  return 0x1;
}
