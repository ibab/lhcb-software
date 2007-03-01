// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/Platform.cpp,v 1.1 2007-03-01 10:40:00 frankb Exp $
//  ====================================================================
//  System.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $ID: $
#include "PVSS/Internals.h"
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <process.h>

/// Access DLL load error
const char* PVSS::pvss_load_error()  {
  static char s[1024] = "No error reporting implemented";
  void* lpMessageBuffer;
  ::FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER |  FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    ::GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default language
    (LPTSTR) &lpMessageBuffer,
    0,
    NULL );
  strcpy(s, (const char*)lpMessageBuffer);
  ::LocalFree( lpMessageBuffer ); 
  return s;
}

/// Sleep a specified amount of time
int PVSS::pvss_sleep(int millisecs)  {
  ::Sleep(millisecs);
  return 1;
}

/// Load function from DLL
PVSS::pvss_function_t PVSS::pvss_load_function(const char* lib, const char* fun)  {
  std::string dll = lib;
  dll += ".dll";
  printf("--------> Loading DLL:%s\n",dll.c_str());
  void* handle = ::LoadLibrary(dll.c_str());
  if ( 0 == handle )  {
    ::printf("Failed to load library:%s\n",dll.c_str());
    ::printf("Error: %s\n",pvss_load_error());
    return pvss_function_t(0);
  }
  return (pvss_function_t)::GetProcAddress(HMODULE(handle),fun);
}

/// Start native thread
int PVSS::pvss_start_thread(void (*fun)(void*), void* arg)  {
  return ::_beginthread(fun, 0, arg);
}

/// Terminate native thread
int PVSS::pvss_end_thread(int exit_code) {
  ::_endthreadex(exit_code);
  return 1;
}

#else
#include <dlfcn.h>
#include <unistd.h>
#define LOAD_LIB(x)  ::dlopen( x , )
#define DLERROR      ::dlerror()

/// Sleep a specified amount of time
int  PVSS::pvss_sleep(int millisecs)  {
  int sec  = millisecs/1000;
  int msec = millisecs%1000;
  if ( sec ) ::sleep(srec);
  if ( msec ) ::usleep(msec*1000);
  return 1;
}

/// Access DLL load error
const char* PVSS::pvss_load_error()  {
  static char s[1024];
  static const char* def = "No error reporting implemented";
  const char* e = ::dlerror();
  strcpy(s,e ? e : def);
  return s;
}

/// Load function from DLL
PVSS::pvss_function_t PVSS::pvss_load_function(const char* lib, const char* fun)  {
  std::string dll = lib;
  dll += ".dll";
  printf("--------> Loading DLL:%s\n",dll.c_str());
  void* handle = ::dlopen(dll.c_str(),RTLD_NOW);
  if ( 0 == handle )  {
    ::printf("Failed to load library:%s\n",dll.c_str());
    ::printf("Error: %s\n",pvss_load_error());
    return pvss_function_t(0);
  }
  return (pvss_function_t)::dlsym(handle,fun);
}
#endif
