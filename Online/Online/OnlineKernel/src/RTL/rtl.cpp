#include "rtl_internal.h"
#include <vector>
#include <map>
#include <string>
#include <cerrno>
#include <fcntl.h>

#ifdef USE_PTHREADS
#include <unistd.h>

const char* errorString(int status)  {
  return strerror(status);
}

#elif _WIN32

#define  _WIN32_WINNT 0x0401 
#include <windows.h>
#include <process.h>

const char* errorString(int status)  {
  static char s[1024] = "No error reporting implemented";
  void* lpMessageBuffer;
  ::FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER |  FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    status,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default language
    (LPTSTR) &lpMessageBuffer,
    0,
    NULL );
  strcpy(s, (const char*)lpMessageBuffer);
  ::LocalFree( lpMessageBuffer ); 
  return s;
}

#endif

int getError()   {
#ifdef USE_PTHREADS
  return errno;
#elif _WIN32
  return ::GetLastError();
#endif
}

const char* errorString()  {
  return errorString(getError());
}

const char* printError()  {
  return "RTL library error";
};


namespace {
  struct ExitHandler : public std::vector<EXHDEF>  {
    ExitHandler() {
    }
    ~ExitHandler() {
      execute();
    }
    static void execute();
  };
  std::vector<EXHDEF>& exitHandlers() {
    static ExitHandler s_exitHandlers;
    return s_exitHandlers;
  }
  void ExitHandler::execute()  {
    static bool executing = false;
    if ( !executing )  {
      executing = true;
      const std::vector<EXHDEF>& v = exitHandlers();
      for (std::vector<EXHDEF>::const_iterator i=v.begin(); i != v.end(); ++i)  {
        const EXHDEF& hdlr = *i;
        if ( hdlr.exit_handler )  {
          (*hdlr.exit_handler)(hdlr.exit_param);
        }
      }
      exitHandlers().clear();
      executing = false;
    }
  }
}

int lib_rtl_remove_rundown(lib_rtl_rundown_handler_t,void*)    {
  return 1;
}

int lib_rtl_declare_rundown(lib_rtl_rundown_handler_t,void*)   {
  return 1;
}

int lib_rtl_declare_exit(EXHDEF* handler_block) {
#ifdef _VMS
  return sys$dclexh (handler_block);
#elif defined(_WIN32) || defined(linux)
  static bool first = true;
  if ( first )  {
    first = false;
    atexit(ExitHandler::execute);
  }
  exitHandlers().push_back(*handler_block);
  return 1;
#endif
}

int lib_rtl_run_ast (RTL_ast_t astadd, void* param, int)    {
#if defined(_WIN32) || defined(linux)
  if ( astadd )  {
    return (*astadd)(param);
  }
#endif
  return 1;
}

int lib_rtl_pid()  {
  return getpid();
}

int lib_rtl_signal_message(int action, const char* fmt)  {
  if ( fmt )  {
    int err;
    switch(action) {
    case LIB_RTL_OS:
      err = getError();
      ::printf("%s : %d  %s\n",fmt, err, errorString(err));
    case LIB_RTL_ERRNO:
      err = errno;
      ::printf("%s : %d  %s\n",fmt, err, errorString(err));
      break;
    case LIB_RTL_DEFAULT:
      ::printf("%s\n",fmt);
      break;
    default:
      err = getError();
      ::printf("%s : %d  %s\n",fmt, err, errorString(err));
      break;
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
#endif
  return 1;
}

int lib_rtl_sleep(int millisecs)    {
#ifdef _WIN32
  ::Sleep(millisecs);
#elif linux
  ::usleep(1000*millisecs);
#endif
  return 1;
}

const char* lib_rtl_error_message(int status)  {
  return errorString(status);
}

