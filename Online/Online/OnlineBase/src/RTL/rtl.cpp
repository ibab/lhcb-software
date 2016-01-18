#include "rtl_internal.h"
#include <vector>
#include <map>
#include <ctime>
#include <cerrno>
#include <cstdarg>
#include <cstring>
#include <climits>
#include <iostream>
#include <fcntl.h>

using namespace std;

#ifdef _WIN32
#define vsnprintf _vsnprintf
#include <winsock.h>

#include <stdexcept>

namespace {
  struct __init__ {
    __init__()  {
      static WSADATA g_WSAData;
      memset(&g_WSAData, 0, sizeof(WSADATA));
      if (WSAStartup ( MAKEWORD(1,1), &g_WSAData) != 0)    {
        throw runtime_error("RTL: WSAStartup failed!");
      }
    }
  };
  static __init__ g_init;
}

#else
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/statvfs.h>
#endif


#include "RTL/strdef.h"

namespace RTL  {
  static int exit_status;
  static bool s_RTL_exit_handler_print = true;
  static size_t (*s_rtl_printer)(void*, int, const char*, va_list args) = 0;
  static void*    s_rtl_printer_arg = 0;
  static bool s_exitHandlerRunning = false;

  struct EXHDEF   {
    int   flink;
    int  (*exit_handler)(void*);
    void *exit_param;
    int  *exit_status;
  };

  struct ExitHandler : public vector<EXHDEF>  {
    ExitHandler();
    ~ExitHandler();
    static void execute();
    static vector<EXHDEF>& exitHandlers();
    static bool isActive() {  return s_exitHandlerRunning; }
  };
}

#ifdef USE_PTHREADS
#include <unistd.h>
#include <signal.h>
#define ERROR_SUCCESS 0
const char* RTL::errorString(int status)  {
  return strerror(status);
}

#define INSTALL_SIGNAL(x,y) install(x , #x , y);
namespace RTL {
  namespace {
    static char rtl_processName[256] = "";
    static char rtl_dataInterface[64] = "";
    static char rtl_nodeName[64] = "";
    static bool rtl_exit_handler_active = false;
  }
  /**@class ExitSignalHandler
   *
   * Small class to manipulate default signal handling
   *
   * @author M.Frank
   */
  class ExitSignalHandler {
  protected:
    typedef map<int,pair<string, struct sigaction> > SigMap;
    SigMap m_map;
    ExitSignalHandler();
    ~ExitSignalHandler();
  public:
    static ExitSignalHandler& instance();
    void install(int num, const string& name, struct sigaction& action);
    void init();
    static void handler(int signum, siginfo_t *info,void * );
    static void back_trace();
  };
  static ExitSignalHandler& s_RTL_ExitSignalHandler = ExitSignalHandler::instance();
}
extern "C" void* lib_rtl_exithandler_instance() {
  return &RTL::s_RTL_ExitSignalHandler;
}

RTL::ExitSignalHandler::ExitSignalHandler()  {
  init();
}

void RTL::ExitSignalHandler::init()  {
  struct sigaction new_action;
  sigemptyset(&new_action.sa_mask);
  new_action.sa_handler   = 0;
  new_action.sa_sigaction = handler;
  new_action.sa_flags     = SA_SIGINFO;
  INSTALL_SIGNAL(SIGILL,   new_action);
  //INSTALL_SIGNAL(SIGINT,   new_action);
  INSTALL_SIGNAL(SIGTERM,  new_action);
  INSTALL_SIGNAL(SIGHUP,   new_action);
  // INSTALL_SIGNAL(SIGPIPE,  new_action);
  // INSTALL_SIGNAL(SIGKILL,  new_action);
  INSTALL_SIGNAL(SIGQUIT,  new_action);
  INSTALL_SIGNAL(SIGBUS,   new_action);
  INSTALL_SIGNAL(SIGXCPU,  new_action);
  sigaddset(&new_action.sa_mask,SIGSEGV);
  sigaddset(&new_action.sa_mask,SIGABRT);
  sigaddset(&new_action.sa_mask,SIGFPE);
  INSTALL_SIGNAL(SIGABRT,  new_action);
  INSTALL_SIGNAL(SIGFPE,   new_action);
  INSTALL_SIGNAL(SIGSEGV,  new_action);
}

RTL::ExitSignalHandler::~ExitSignalHandler() {
}

RTL::ExitSignalHandler& RTL::ExitSignalHandler::instance() {
  static ExitSignalHandler inst;
  return inst;
}

void RTL::ExitSignalHandler::install(int num, const string& name, struct sigaction& action) {
  pair<string, struct sigaction>& old_action = m_map[num];
  int res = sigaction (num, &action, &old_action.second);
  if ( res != 0 ) {
    cout << "Failed to install exit handler for " << name << endl;
    return;
  }
  //cout << "Successfully installed handler for " << name << endl;
  old_action.first = name;
}
#ifdef _WIN32
void RTL::ExitSignalHandler::back_trace() {
}
#else
#include <execinfo.h>
void RTL::ExitSignalHandler::back_trace() {
  void *bt[256];
  int bt_size = backtrace(bt, sizeof(bt) / sizeof(void *));
  ::fprintf(stdout,"\n[INFO] (ExitSignalHandler) ---------------------- Backtrace ----------------------\n");
  ::fprintf(stdout,"[INFO] Number of elements in backtrace: %d\n", bt_size);
  for (int i = 0; i < bt_size; i++) {
    ::fprintf(stdout,"[INFO] (ExitSignalHandler) %02d --> %p\n", i, bt[i]);
  }
  ::fflush(stdout);
}
#endif

void RTL::ExitSignalHandler::handler(int signum, siginfo_t *info, void *ptr) {
  SigMap& m = instance().m_map;
  SigMap::iterator i=m.find(signum);
  rtl_exit_handler_active = true;
  if ( i != m.end() ) {
    __sighandler_t old = (*i).second.second.sa_handler;
    func_cast<void (*)(int)> dsc0(old);
    func_cast<void (*)(int,siginfo_t*, void*)> dsc(dsc0.ptr);

    if ( s_RTL_exit_handler_print ) {
      ::fprintf(stderr,"[FATAL] Process: '%s' (ExitSignalHandler) RTL:Handled signal: %d [%s] Old action:%p Mem:%p Code:%08X\n",
	       rtl_processName,signum,(*i).second.first.c_str(),dsc.ptr,info->si_addr,info->si_code);
      if ( signum == SIGSEGV || signum == SIGBUS || signum == SIGILL )  {
	back_trace();
      }
    }
    if ( signum == SIGINT || signum == SIGHUP || signum == SIGFPE || signum == SIGPIPE ) {
      if ( old && old != SIG_IGN && dsc.fun )
        dsc.fun(signum,info,ptr);
    }
    else if ( signum == SIGSEGV && old && old != SIG_IGN && old != SIG_DFL ) {
      RTL::ExitHandler::execute();
      if ( dsc.fun ) 
        dsc.fun(signum,info,ptr);
      else 
        (*old)(signum);
      ::_exit(0);
    }
    else if ( old && old != SIG_IGN && dsc.fun )  {
      RTL::ExitHandler::execute();
      dsc.fun(signum,info,ptr);
    }
    else if ( old == SIG_DFL ) {
      RTL::ExitHandler::execute();
      ::_exit(0);
    }
    rtl_exit_handler_active = false;
    return;
  }
  RTL::ExitHandler::execute();
  rtl_exit_handler_active = false;
}

#elif _WIN32

#include <windows.h>
#include <process.h>

const char* RTL::errorString(int status)  {
  static char s[1024] = "No error reporting implemented";
  static int len = sizeof(s)-1;
  void* lpMessageBuffer;
  ::FormatMessage( 
                  FORMAT_MESSAGE_ALLOCATE_BUFFER |  FORMAT_MESSAGE_FROM_SYSTEM,
                  0,
                  status,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default language
                  (LPTSTR) &lpMessageBuffer,
                  0,
                  0 );
  if ( lpMessageBuffer )  {
    strncpy(s, (const char*)lpMessageBuffer, len);
    s[len] = 0;
    size_t l = strlen(s);
    if ( l > 1 )  {
      s[l-1] = 0;
      if ( !::isalnum(s[l-2]) ) s[l-2]=0;
    }
    ::LocalFree( lpMessageBuffer ); 
  }
  else {
    ::snprintf(s,sizeof(s),"RTL Error: Unknown error code: %08X",status);
  }
  return s;
}
#endif

/// Set signal handling output level
extern "C" void lib_rtl_signal_log(int value) {
  RTL::s_RTL_exit_handler_print = (value!=0);
}

const char* RTL::errorString()  {
  return errorString(lib_rtl_get_error());
}

RTL::ExitHandler::ExitHandler() {
}

RTL::ExitHandler::~ExitHandler() {
  execute();
}

extern "C" int lib_rtl_event_exithandler();
extern "C" int lib_rtl_lock_exithandler();
extern "C" int lib_rtl_gbl_exithandler();
void RTL::ExitHandler::execute()  {
  if ( !s_exitHandlerRunning )  {
    s_exitHandlerRunning = true;
    s_rtl_printer = 0;

    const vector<EXHDEF>& v = exitHandlers();
    for (vector<EXHDEF>::const_reverse_iterator i=v.rbegin(); i != v.rend(); ++i)  {
      const EXHDEF& hdlr = *i;
      if ( hdlr.exit_handler )  {
        (*hdlr.exit_handler)(hdlr.exit_param);
      }
    }
    exitHandlers().clear();
    lib_rtl_event_exithandler();
    lib_rtl_lock_exithandler();
    lib_rtl_gbl_exithandler();
  }
}

vector<RTL::EXHDEF>& RTL::ExitHandler::exitHandlers() {
  static ExitHandler s_exitHandlers;
  return s_exitHandlers;
}

/// Access to error code from socket library
extern "C" int lib_rtl_socket_error()  {
#ifdef _WIN32
  return ::WSAGetLastError();
#else
  return errno;
#endif
}

extern "C" int lib_rtl_get_error()   {
#ifdef USE_PTHREADS
  return errno;
#elif _WIN32
  return ::GetLastError();
#endif
}

extern "C" int lib_rtl_remove_rundown(lib_rtl_rundown_handler_t,void*)    {
  return 1;
}

extern "C" int lib_rtl_declare_rundown(lib_rtl_rundown_handler_t,void*)   {
  return 1;
}

extern "C" int lib_rtl_declare_exit(int (*hdlr)(void*), void* param)  {
#if defined(_WIN32) || defined(__linux)
  static bool first = true;
  if ( first )  {
    first = false;
    ::atexit(RTL::ExitHandler::execute);
  }
  RTL::EXHDEF h;
  h.exit_handler = hdlr;
  h.exit_param   = param;
  h.exit_status  = &RTL::exit_status;
  RTL::ExitHandler::exitHandlers().push_back(h);
  return 1;
#endif
}

extern "C" int lib_rtl_remove_exit(int (*hdlr)(void*), void* param) {
#if defined(_WIN32) || defined(__linux)
  RTL::ExitHandler::iterator i=RTL::ExitHandler::exitHandlers().begin();
  RTL::ExitHandler::iterator j=RTL::ExitHandler::exitHandlers().end();
  for(; i!=j; ++i)  {
    if ( (*i).exit_handler == hdlr && (*i).exit_param == param )  {
      RTL::ExitHandler::exitHandlers().erase(i);
      return 1;
    }
  }
  return 0;
#else
  return 1;
#endif
}

extern "C" void* lib_rtl_alloc_int_pointer_map()   {
  return new map<int,void*>;
}

extern "C" int lib_rtl_run_ast (RTL_ast_t astadd, void* param, int)    {
#if defined(_WIN32) || defined(__linux)
  if ( astadd )  {
    return (*astadd)(param);
  }
#endif
  return 1;
}

extern "C" int lib_rtl_pid()  {
  return getpid();
}

int lib_rtl_signal_message(int action, const char* fmt, ...)  {
  int status = 1;
  char buff[2048];
  va_list args;
  va_start( args, fmt );
  if ( fmt )  {
    int err;
    switch(action) {
    case LIB_RTL_ERRNO:
      err = errno;
      if ( err != 0 )  {
        ::lib_rtl_output(LIB_RTL_ERROR,"RTL: %8d : %s\n",err, RTL::errorString(err));
        ::vsnprintf(buff, sizeof(buff), fmt, args);
        ::lib_rtl_output(LIB_RTL_ERROR,"                %s\n",buff);
        status = 0;
      }
      break;
    case LIB_RTL_DEFAULT:
      ::vsnprintf(buff, sizeof(buff), fmt, args);
      ::lib_rtl_output(LIB_RTL_ERROR,"RTL: %s\n",buff);
      status = 0;
      break;
    case LIB_RTL_OS:
    default:
      err = lib_rtl_get_error();
      if ( err != ERROR_SUCCESS )   {
        ::lib_rtl_output(LIB_RTL_ERROR,"RTL: %8d : %s\n",err, RTL::errorString(err));
        ::vsnprintf(buff, sizeof(buff), fmt, args);
        ::lib_rtl_output(LIB_RTL_ERROR,"                %s\n",buff);
        status = 0;
      }
      break;
    }
  }
  va_end(args);
  return status;
}

extern "C" int lib_rtl_start_debugger()    {
#ifdef _WIN32
  _asm int 3
#else
    char txt[128];
  ::snprintf(txt,sizeof(txt),"ddd --pid=%d &",lib_rtl_pid()); 
  ::system(txt);
  ::lib_rtl_sleep(5000);  // Sleep a few seconds to allow 
                          // to attach the debugger
#endif
  return 1;
}

extern "C" const char* lib_rtl_error_message(int status)  {
  return RTL::errorString(status);
}

extern "C" int lib_rtl_default_return()  {
#if defined(_VMS) || defined(_WIN32)
  return 1;
#elif defined(__linux) || defined(_OSK)
  return 0;
#else
#endif
}

/// Disable intercepts
extern "C" int lib_rtl_disable_intercept() {
  return lib_rtl_default_return();
}

/// Enable intercpets
extern "C" int lib_rtl_enable_intercept()    {
  return lib_rtl_default_return();
}

extern "C" int lib_rtl_get_process_name(char* process, size_t len)  {
  const char *tmp;
  char buff[32], buff2[64];
  size_t resultant_length = sizeof(buff2);
  tmp = ::lib_rtl_getenv("UTGID");
  if ( !tmp ) tmp = ::lib_rtl_getenv("PROCESSNAME");
  if ( !tmp ) tmp = ::lib_rtl_getenv("PROCESS");
  if ( !tmp ) { ::snprintf(buff,sizeof(buff),"P%06d",lib_rtl_pid()); tmp=buff;}
  ::str_trim(tmp, buff2, &resultant_length);
  ::strncpy(process, buff2, len);
  return tmp ? strlen(tmp)+1>len ? 0 : 1 : 0;
}

extern "C" int lib_rtl_get_node_name(char* node, size_t len)  {
  char n[64];
  const char *tmp = ::lib_rtl_getenv("NODE");
#if defined(_WIN32)
  if ( !tmp ) tmp = ::lib_rtl_getenv("COMPUTERNAME");
  if ( !tmp && 0 == ::gethostname (n,sizeof(n)) ) tmp = n;
#elif defined(_OSK)
#else
  if ( 0 == ::gethostname (n,sizeof(n)) ) tmp = n;
#endif
  ::strncpy(node,tmp != 0 ? tmp : "UNKNOWN", len);
  return 1;
}

extern "C" int lib_rtl_get_datainterface_name(char* node, size_t len)  {
  const char *tmp = ::lib_rtl_getenv("DATAINTERFACE");
  if ( !tmp )  {
    char n[64], nn[70];
    if ( 0 == ::gethostname (n,sizeof(n)) )  {
      if ( strchr(n,'.') != 0 ) *strchr(n,'.') = 0;
      ::strncpy(nn,n,sizeof(nn));
      nn[sizeof(nn)-1] = 0;
      ::strncat(nn,"-d",3);
      hostent* h = ::gethostbyname(nn);
      if ( h ) {
        tmp = inet_ntoa(*(in_addr*)h->h_addr);
      }
      else {
        ::strncat(nn,"1",3);
        if ( (h=::gethostbyname(nn)) ) {
          tmp = inet_ntoa(*(in_addr*)h->h_addr);
        }
        else if ( (h=::gethostbyname(n)) ) {
          tmp = inet_ntoa(*(in_addr*)h->h_addr);
        }
      }
    }
  }
  ::strncpy(node,tmp != 0 ? tmp : "UNKNOWN", len);
  node[len-1] = 0;
  return 1;
}

/// Printout redirection
extern "C" size_t lib_rtl_output(int level, const char* format, ...)   {
  size_t result;
  va_list args;
  va_start( args, format );
  switch(level) {
  case LIB_RTL_ERRNO:
  case LIB_RTL_OS:
    if ( errno != 0 )  {
      int err = errno;
      ::lib_rtl_output(LIB_RTL_ERROR,"RTL: %8d : %s\n",err, RTL::errorString(err));
      errno = 0;
    }
    level = LIB_RTL_ERROR;
    break;
  case LIB_RTL_DEFAULT:
    level = LIB_RTL_ERROR;
    break;
  default:
    break;
  }

  if ( RTL::s_rtl_printer != 0 && !RTL::rtl_exit_handler_active )  {
    result = (*RTL::s_rtl_printer)(RTL::s_rtl_printer_arg, level, format, args);
  }
  else  {
    result = ::vfprintf(stdout, format, args);
    ::fflush(stdout);
  }
  va_end(args);
  return result;
}

/// Printout redirection
extern "C" size_t lib_rtl_printf(const char* format, ...)   {
  size_t result;
  va_list args;
  va_start( args, format );
  if ( RTL::s_rtl_printer != 0 && !RTL::rtl_exit_handler_active )  {
    result = (*RTL::s_rtl_printer)(RTL::s_rtl_printer_arg, LIB_RTL_ALWAYS, format, args);
  }
  else  {
    result = ::vfprintf(stdout, format, args);
    ::fflush(stdout);
  }
  va_end(args);
  return result;
}

/// Install RTL printer 
extern "C" void lib_rtl_install_printer(size_t (*func)(void*, int, const char*, va_list args), void* param)  {
  RTL::s_rtl_printer = func;
  RTL::s_rtl_printer_arg = param;
}

/// Creates a pipe and executes a command.
extern "C" FILE* lib_rtl_pipe_open(const char* command, const char* mode) {
#ifdef _WIN32
  return ::_popen(command, mode);
#else
  return popen(command, mode);
#endif
}

/// Waits for new command processor and closes stream on associated pipe.
extern "C" int lib_rtl_pipe_close(FILE* stream) {
#ifdef _WIN32
  return ::_pclose(stream);
#else
  return ::pclose(stream);
#endif  
}

/// Safe wrapper around getenv
const char* lib_rtl_getenv(const char* value) {
  return value ? ::getenv(value) : 0;
}

/// Access total/free disk space on file system (linux:statvfs call)
extern "C" int lib_rtl_diskspace(const char* name, 
                                 unsigned long long int* blk_size,
                                 unsigned long long int* total_blk,
                                 unsigned long long int* availible_blk)
{
#ifdef _WIN32
  DWORD SectorsPerCluster,BytesPerSector,NumberOfFreeClusters,TotalNumberOfClusters;
  if ( ::GetDiskFreeSpace(name,&SectorsPerCluster,&BytesPerSector,
                          &NumberOfFreeClusters,&TotalNumberOfClusters) ) {
    *blk_size = BytesPerSector;
    *total_blk = SectorsPerCluster*TotalNumberOfClusters;
    *availible_blk = SectorsPerCluster*NumberOfFreeClusters;
    return 1;
  }
#else
  struct statvfs disk;
  if ( 0 == ::statvfs(name,&disk) ) {
    *blk_size = disk.f_bsize;
    *total_blk = long(float(disk.f_blocks)*float(disk.f_frsize)/float(disk.f_bsize));
    //*total_blk = disk.f_blocks;
    *availible_blk = disk.f_bfree;
    return 1;
  }
#endif
  return 0;
}

/// access checks it is ok to read, write, execute the file *name
/// mode is a bit-mask (1 read, 2 write, 4 execute, 8 file exists)
extern "C" int lib_rtl_access(const char *name, int mode)
{
#ifdef _WIN32
  // under windows AFAICS this requires using the security API so we just assume it's fine
  return 1;
#else
  int amode = 0;

  if (mode & 0x1) amode |= R_OK;
  if (mode & 0x2) amode |= W_OK;
  if (mode & 0x4) amode |= X_OK;
  if (mode & 0x8) amode |= F_OK; 	
  if (::access(name, amode)) return 0;
  return 1;
#endif
}

/// POSIX/ISO compiant wrapper around unlink
extern "C" int lib_rtl_unlink(const char* fname) {
#ifdef _WIN32
  return ::_unlink(fname);
#else
  return ::unlink(fname);
#endif
}

/// Read path associated to file name
extern "C" int lib_rtl_readlink(const char* fname, char* path, size_t path_len)   {
  if ( ::readlink(fname, path, path_len) != -1 )
    return 1;
  return 0;
}

/// Read path according from file descriptor
extern "C" int lib_rtl_file_name(int fd, char* path, size_t path_len)   {
  char buf[128];
  ::snprintf(buf, sizeof(buf), "/proc/self/fd/%d", fd);
  return lib_rtl_readlink(buf, path, path_len);
}

extern "C" int rtl_test_main(int /* argc */, char** /* argv */)  {
  cout << "Executing empty test action ..... finished ......" << endl;
  return 1;
}

extern "C" const char* lib_rtl_timestr(const char* fmt, const time_t* tp)  {
  static char timestr[256];
  time_t t;
  if ( 0 == tp )  {
    tp = &t;
    ::time(&t);
  }
  struct tm *now = ::localtime(&t);
  ::strftime(timestr,sizeof(timestr),fmt,now);
  return timestr;
}

extern "C" const char* lib_rtl_gmtimestr(const char* fmt, const time_t* tp)  {
  static char timestr[256];
  time_t t;
  if ( 0 == tp )  {
    tp = &t;
    ::time(&t);
  }
  struct tm *now = ::gmtime(tp);
  ::strftime(timestr,sizeof(timestr),fmt,now);
  return timestr;
}

namespace RTL {
  static string s_processName, s_dataInterfaceName, s_nodeName, s_nodeNameShort;

  void RTL_reset() {
    s_processName = s_dataInterfaceName = s_nodeName = s_nodeNameShort = "";
    rtl_processName[0] = 0;
  }
  void RTL_init_sigs() {
#ifndef _WIN32
    RTL::ExitSignalHandler::instance().init();
#endif
  }
  const string& processName()  {
    if ( s_processName.empty() )  {
      ::lib_rtl_get_process_name(rtl_processName, sizeof(rtl_processName));
      s_processName = rtl_processName;
    }
    return s_processName;
  }
  const string& dataInterfaceName()  {
    if ( s_dataInterfaceName.empty() )  {
      ::lib_rtl_get_datainterface_name(rtl_dataInterface, sizeof(rtl_dataInterface));
      s_dataInterfaceName = rtl_dataInterface;
    }
    return s_dataInterfaceName;
  }
  const string& nodeName()  {
    if ( s_nodeName.empty() )  {
      ::lib_rtl_get_node_name(rtl_nodeName,sizeof(rtl_nodeName));
      s_nodeName = rtl_nodeName;
    }
    return s_nodeName;
  }
  const string& nodeNameShort()  {
    if ( s_nodeNameShort.empty() )  {
      s_nodeNameShort = nodeName();
      if ( s_nodeNameShort.find(".") != string::npos )  {
        s_nodeNameShort = s_nodeNameShort.substr(0,s_nodeNameShort.find("."));
      }
    }
    return s_nodeNameShort;
  }

  std::string fileFromLink(const std::string& link)   {
    char filePath[PATH_MAX];
    int res = lib_rtl_readlink(link.c_str(),filePath,sizeof(PATH_MAX));
    return lib_rtl_is_success(res) ? filePath : "";
  }

  std::string fileFromDescriptor(int fd)   {
    char filePath[PATH_MAX];
    int res = lib_rtl_file_name(fd,filePath,sizeof(filePath));
    return lib_rtl_is_success(res) ? filePath : "";
  }

}
