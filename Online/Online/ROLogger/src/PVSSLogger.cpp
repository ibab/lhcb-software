#include <map>
#include <string>

namespace ROLogger {

  // Forward declarations
  class LogFileEntry;
  /**
  */
  class LogDirectory {
    /// Definition of file map
    typedef std::map<std::string,LogFileEntry*> Files;
    /// Directory path
    std::string m_name;
    /// Node name
    std::string m_node;
    /// Process name
    std::string m_process;
    /// Maximum time difference to changes to log files in order to be taken into account
    time_t      m_maxFileChange;
    /// Map of known log files
    Files       m_files;
    /// FIFO file descriptor
    int m_fifo;
    /// Max. number of seconds before checking for new files
    int m_checkLimit;
    /// Time stamp of last file existence check.
    time_t m_lastChecked;

    std::string _prefix(const std::string& tag="INFO") const;
  public:

    /// Initializing constructor
    LogDirectory(const std::string& nam);
    /// Default destructor
    virtual ~LogDirectory(); 

    /// Connect output fifo
    int connect(const std::string& fifo);
    /// Open all log files
    int open(bool begin,bool prt);
    /// Close all log files
    int close();
    /// Read directory content
    int read(bool prt);
    /// Print content of log files as they come....
    int publish();
    /// Check directory for new log files
    void checkFiles();
    /// Print message to output device
    virtual int print(const std::string& msg);
    /// Dump directory content
    void dump();
    /// Access directory name
    const std::string& name() const { return m_name;  }
    /// Access file map
    const Files& files() const      { return m_files; }
  };
}

#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cerrno>

#include <fcntl.h>
#include "RTL/rtl.h"
#include "RTL/readdir.h"
#ifdef _WIN32
#include <io.h>
#include <direct.h>
#include <sys/stat.h>
#define O_NONBLOCK 0x0
#define W_OK       0x6
inline bool S_ISFIFO(int) { return false; }
#endif

namespace ROLogger {
  class LogFileEntry {
  public:
    struct stat   data;
    size_t        pointer;
    int           fd;
    std::string   remainder;
    LogFileEntry() : pointer(0), fd(0)     {    
      ::memset(&data,0,sizeof(data));
    }
    LogFileEntry(const std::string& name) : fd(0)    {
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
    int open(const std::string& fn, bool begin) {
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

using namespace ROLogger;

std::string LogDirectory::_prefix(const std::string& tag) const {
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
LogDirectory::LogDirectory(const std::string& nam)
: m_name(nam), m_node("Unknown"), m_fifo(-1)
{
  m_checkLimit = 3;
  m_maxFileChange = 7*24*3600;
  m_node = RTL::nodeNameShort();
  m_process = RTL::processName();
  ::chdir(m_name.c_str());
}

/// Default destructor
LogDirectory::~LogDirectory()
{
  if ( m_fifo != -1 ) {
    ::close(m_fifo);
    m_fifo = -1;
  }
}

/// Connect output fifo
int LogDirectory::connect(const std::string& fifo) {
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
int LogDirectory::read(bool prt) {
  std::string prefix = _prefix();
  DIR* dir=::opendir(m_name.c_str());
  if ( dir ) {
    time_t now = time(0);
    struct dirent* dp;
    while( (dp=::readdir(dir)) != 0 ) {
      std::string fn = dp->d_name;
      size_t idx = fn.rfind(".");
      if ( fn.substr(idx) == ".log" ) {
        Files::iterator i=m_files.find(fn);
        if ( i == m_files.end() ) {
          std::auto_ptr<LogFileEntry> e(new LogFileEntry(m_name+"/"+fn));
          if ( now-e->data.st_mtime < m_maxFileChange ) {
            // file needs to be modified at least during the last week.....
            m_files[fn] = e.release();
          }
          else if ( prt ) {
            const struct stat& s = e->data;
            std::string mod = ::ctime(&e->data.st_mtime);
            std::stringstream os;
            os << prefix << std::left << std::setw(28) 
              << fn.substr(0,fn.length()-4)+": " 
              << "Last Modified: " << mod.substr(0,mod.length()-1)
              << " Size:" << std::setw(9) << std::right << s.st_size
              << " INode:" << std::setw(6) << std::right << s.st_ino
              << "   ---> File too old and ignored"
              << std::endl;
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
void LogDirectory::dump() {
  std::string prefix = _prefix();
  for(Files::const_iterator i=m_files.begin(); i!=m_files.end();++i)  {
    const struct stat& s = (*i).second->data;
    const std::string& fn = (*i).first;
    std::string mod = ::ctime(&s.st_mtime);
    std::stringstream os;
    os << prefix << std::left << std::setw(28) 
      << fn.substr(0,fn.length()-4)+": " 
      << "Last Modified: " << mod.substr(0,mod.length()-1)
      << " Size:" << std::setw(9) << std::right << s.st_size
      << " INode:" << std::setw(6) << std::right << s.st_ino
      << std::endl;
    print(os.str());
  }
}

/// Open all log files
int LogDirectory::open(bool begin, bool prt) {
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
int LogDirectory::close() {
  for(Files::iterator i=m_files.begin(); i!=m_files.end();++i)
    delete (*i).second;
  m_files.clear();
  return 1;
}

/// Print message to output device
int LogDirectory::print(const std::string& msg) {
  //std::cout << msg << std::endl;
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
int LogDirectory::publish() {
  size_t bytes, rd, buff_len = 0;
  char *buff = 0;
  std::string prefix = _prefix();
  std::string pref_err = _prefix("ERROR");
  std::string pref_warn = _prefix("WARN");

  ::lib_rtl_sleep(50);
  for(Files::iterator i=m_files.begin(); i!=m_files.end();++i)  {
    const std::string& fn = (*i).first;
    LogFileEntry* e = (*i).second;
    int fd = e->fd;
    int res = ::fstat(fd,&e->data);
    if ( res < 0 ) {
      std::cout << "Failed to stat " << fn << std::endl;
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
            std::stringstream os;
            std::string pref = prefix;
            if ( strstr(p0,"ERROR") )
              pref = pref_err;
            else if ( strstr(p0,"> can not execute action <") )
              pref = pref_warn;
            os << std::left << std::setw(60) << pref+fn.substr(0,fn.length()-4)+": " 
              << e->remainder << p0 << std::endl;
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
void LogDirectory::checkFiles() {
  if ( ::time(0)-m_lastChecked > m_checkLimit ) {
    for(Files::iterator i=m_files.begin(); i!=m_files.end();++i)  {
      struct stat buf;
      const std::string& fn = (*i).first;
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
  //LogDirectory dir("/localdisk/pvss/STORAGE/log");
  LogDirectory dir(argv[1]);
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
