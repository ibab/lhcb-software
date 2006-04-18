#include "rtl_internal.h"
#include <vector>
#include <map>
#include <string>
#include <cerrno>
#include <cstdarg>
#include <iostream>
#include <fcntl.h>
#ifdef _WIN32
#include <winsock.h>
#else
#include <cstdlib>
#include <unistd.h>
#endif
#include "RTL/strdef.h"

namespace RTL  {
  struct EXHDEF   {
    int   flink;
    int  (*exit_handler)(void*);
    void *exit_param;
    int  *exit_status;
  };
  struct ExitHandler : public std::vector<EXHDEF>  {
    ExitHandler();
    ~ExitHandler();
    static void execute();
    static std::vector<EXHDEF>& exitHandlers();
  };
}

static int exit_status;

#ifdef USE_PTHREADS
#include <unistd.h>
#define ERROR_SUCCESS 0
const char* errorString(int status)  {
  return strerror(status);
}

#elif _WIN32

#define  _WIN32_WINNT 0x0401 
#include <windows.h>
#include <process.h>

const char* errorString(int status)  {
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
    if ( l > 0 ) s[l-1] = 0;
    ::LocalFree( lpMessageBuffer ); 
  }
  else {
    sprintf(s,"RTL Error: Unknown error code: %08X",status);
  }
  return s;
}

#elif defined(_VMS)
#include <ssdef.h>
#include <descrip.h>

const char* errorString(int status)  {
  static char message[1024];
  dsc$descriptor_s msg;
  short len = sizeof(message) - 1;
  msg.dsc$w_length  = sizeof(message) - 1;
  msg.dsc$a_pointer = message;
  msg.dsc$b_dtype   = DSC$K_DTYPE_T;
  msg.dsc$b_class   = DSC$K_CLASS_S;
  sys$getmsg (status, len, msg, 15, 0);
  message[len] = '\0';
  return message;
}
#endif

/// Access to error code from socket library
int lib_rtl_socket_error()  {
#ifdef _WIN32
  return ::WSAGetLastError();
#else
  return errno;
#endif
}

int lib_rtl_get_error()   {
#ifdef USE_PTHREADS
  return errno;
#elif _WIN32
  return ::GetLastError();
#endif
}

const char* errorString()  {
  return errorString(lib_rtl_get_error());
}

RTL::ExitHandler::ExitHandler() {
}

RTL::ExitHandler::~ExitHandler() {
  execute();
}

void RTL::ExitHandler::execute()  {
  static bool executing = false;
  if ( !executing )  {
    executing = true;
    const std::vector<EXHDEF>& v = exitHandlers();
    for (std::vector<EXHDEF>::const_reverse_iterator i=v.rbegin(); i != v.rend(); ++i)  {
      const EXHDEF& hdlr = *i;
      if ( hdlr.exit_handler )  {
        (*hdlr.exit_handler)(hdlr.exit_param);
      }
    }
    exitHandlers().clear();
    executing = false;
  }
}

std::vector<RTL::EXHDEF>& RTL::ExitHandler::exitHandlers() {
  static ExitHandler s_exitHandlers;
  return s_exitHandlers;
}

int lib_rtl_remove_rundown(lib_rtl_rundown_handler_t,void*)    {
  return 1;
}

int lib_rtl_declare_rundown(lib_rtl_rundown_handler_t,void*)   {
  return 1;
}

int lib_rtl_declare_exit(int (*hdlr)(void*), void* param)  {
#if defined(_WIN32) || defined(__linux)
  static bool first = true;
  if ( first )  {
    first = false;
    atexit(RTL::ExitHandler::execute);
  }
  RTL::EXHDEF h;
  h.exit_handler = hdlr;
  h.exit_param   = param;
  h.exit_status  = &exit_status;
  RTL::ExitHandler::exitHandlers().push_back(h);
  return 1;
#endif
}

int lib_rtl_remove_exit(int (*hdlr)(void*), void* param) {
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
  return new std::map<int,void*>;
}

int lib_rtl_run_ast (RTL_ast_t astadd, void* param, int)    {
#if defined(_WIN32) || defined(__linux)
  if ( astadd )  {
    return (*astadd)(param);
  }
#endif
  return 1;
}

int lib_rtl_pid()  {
  return getpid();
}

int lib_rtl_signal_message(int action, const char* fmt, ...)  {
  va_list args;
  va_start( args, fmt );
  if ( fmt )  {
    int err;
    switch(action) {
    case LIB_RTL_ERRNO:
      err = errno;
      if ( err != 0 )  {
        ::lib_rtl_printf("RTL: %8d : %s\n",err, errorString(err));
        ::lib_rtl_printf("                ");
        ::vprintf(fmt, args);
        ::lib_rtl_printf("\n");
        return 0;
      }
      return 1;
    case LIB_RTL_DEFAULT:
      ::lib_rtl_printf("RTL: ");
      ::vprintf(fmt, args);
      ::lib_rtl_printf("\n");
      break;
    case LIB_RTL_OS:
    default:
      err = lib_rtl_get_error();
      if ( err != ERROR_SUCCESS )   {
        ::lib_rtl_printf("RTL: %8d : %s\n",err, errorString(err));
        ::lib_rtl_printf("                ");
        ::vprintf(fmt, args);
        ::lib_rtl_printf("\n");
        return 0;
      }
      return 1;
    }
  }
  return 1;
}

int lib_rtl_signal_message(int,int,int) {
  return 1;
}

int lib_rtl_start_debugger()    {
#ifdef _WIN32
  _asm int 3
#else
  char txt[128];
  sprintf(txt,"ddd --pid=%d",lib_rtl_pid()); 
  system(txt);
#endif
  return 1;
}

int lib_rtl_sleep(int millisecs)    {
#ifdef _WIN32
  ::Sleep(millisecs);
#elif __linux
  ::usleep(1000*millisecs);
#endif
  return 1;
}

int lib_rtl_usleep(int microsecs)    {
#ifdef _WIN32
  timeval tv;
  tv.tv_sec = microsecs/1000000;
  tv.tv_usec = microsecs%1000000;
  ::select(0,0,0,0,&tv);
#elif __linux
  ::usleep(microsecs);
#endif
  return 1;
}

const char* lib_rtl_error_message(int status)  {
  return errorString(status);
}

int lib_rtl_default_return()  {
#if defined(_VMS) || defined(_WIN32)
  return 1;
#elif defined(__linux) || defined(_OSK)
  return 0;
#else
#endif
}

/// Disable intercepts
int lib_rtl_disable_intercept() {
  return lib_rtl_default_return();
}

/// Enable intercpets
int lib_rtl_enable_intercept()    {
  return lib_rtl_default_return();
}

int lib_rtl_get_process_name(char* process, size_t len)  {
#ifdef __VMS
  int status;
  short len;
  int id = 0;
  acpp_iosb theiosb;
  item it[2] = { { 22, JPI$_PRCNAM, process , &len } , {0,0,0,0} };
  status = sys$getjpiw(0, id, 0, it, &theiosb, 0, 0, 0) ;
  if ( !SUCCESS(status) )   {
    return status;
  }
  process[len]='\0';
  return 1;
#else
  const char *tmp;
  char buff[32], buff2[64];
  size_t resultant_length = sizeof(buff2);
  tmp = ::getenv("UTGID");
  if ( !tmp ) tmp = ::getenv("PROCESSNAME");
  if ( !tmp ) tmp = ::getenv("PROCESS");
  if ( !tmp ) { sprintf(buff,"P%06d",lib_rtl_pid()); tmp=buff;}
  ::str_trim(tmp, buff2, &resultant_length);
  ::strncpy(process, buff2, len);
  return tmp ? strlen(tmp)+1>len ? 0 : 1 : 0;
#endif
}

namespace RTL {
  const std::string& processName()  {
    static std::string s;
    if ( s.empty() )  {
      char txt[64];
      ::lib_rtl_get_process_name(txt, sizeof(txt));
      s = txt;
    }
    return s;
  }
}

int lib_rtl_get_node_name(char* node, size_t len)  {
#ifdef __VMS
  short len;
  int id=0;
  acpp_iosb theiosb;
  item it[2] = { { 22, JPI$_NODENAME, node , &len } , {0,0,0,0} };
  int status = sys$getjpiw(0, id, 0, it, &theiosb, 0, 0, 0) ;
  if ( !SUCCESS(status) )   {
    return status;
  }
  node[len]='\0';
#else
#if defined(_WIN32)
  const char *tmp = getenv("COMPUTERNAME");
#elif defined(_OSK)
  const char *tmp = getenv("NODE");
#else
  const char *tmp = getenv("NODE");
#endif
  ::strncpy(node,tmp != 0 ? tmp : "UNKNOWN", len);
#endif
  return 1;
}

static size_t (*s_rtl_printer)(void*, const char*, va_list args) = 0;
static void*    s_rtl_printer_arg = 0;

/// Printout redirection
size_t lib_rtl_printf(const char* format, ...)   {
  size_t result;
  va_list args;
  va_start( args, format );
  result = (s_rtl_printer != 0)
    ? (*s_rtl_printer)(s_rtl_printer_arg, format, args)
    : vfprintf(stdout, format, args);
  va_end(args);
  return result;
}

/// Install RTL printer 
void lib_rtl_install_printer(size_t (*func)(void*, const char*, va_list args), void* param)  {
  s_rtl_printer = func;
  s_rtl_printer_arg = param;
}

extern "C" int rtl_test_main(int /* argc */, char** /* argv */)  {
  std::cout << "Executing empty test action ..... finished ......" << std::endl;
  return 1;
}
