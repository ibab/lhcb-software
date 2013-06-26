// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/Platform.cpp,v 1.5 2007-10-01 14:46:55 frankm Exp $
//  ====================================================================
//  Platform.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: Platform.cpp,v 1.5 2007-10-01 14:46:55 frankm Exp $

#include "PVSS/Internals.h"
#include <cstring>
#include <memory>
#include <string>

/// Access to global lock object
void* PVSS::pvss_global_lock()  {
  static void* lock = 0;
  if ( 0 == lock )  pvss_create_lock(&lock);
  return lock;
}

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
  // printf("PVSS> --------> Loading DLL:%s\n",dll.c_str());
  void* handle = ::LoadLibrary(dll.c_str());
  if ( 0 == handle )  {
    ::printf("PVSS> Failed to load library:%s\n",dll.c_str());
    ::printf("PVSS> Error: %s\n",pvss_load_error());
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

/// Create lock to protect PVSS API manager from multithreaded trouble
int PVSS::pvss_create_lock(void** handle)   {
  *handle = ::CreateMutex(0,FALSE,0);
  if ( !*handle )
    ::perror("SEVERE: CreateMutex: ");
  return *handle ? 1 : 0;
}

/// Delete PVSS lock object
int PVSS::pvss_delete_lock(void* handle)   {
  return handle ? ::CloseHandle(handle) == 0 ? 0 : 1 : 0;
}

/// Take lock object
int PVSS::pvss_take_lock(void* h) {
  if ( h )  {
    DWORD sc = WAIT_TIMEOUT;
    const char* opt = "";
    while ( h != 0 && sc == WAIT_TIMEOUT )  {
      sc = ::WaitForSingleObject( h, 1000 /*INFINITE*/ );
      switch(sc)  {
        case WAIT_FAILED:       opt="WAIT_FAILED";    break;
        case WAIT_ABANDONED:
          sc = ::WaitForSingleObject( h, 1000 /*INFINITE*/ );
          opt="WAIT_ABANDONED"; break;
        case WAIT_TIMEOUT:      opt="WAIT_TIMEOUT";   break;
        case WAIT_OBJECT_0:     opt=0;                break;
        default:                                      break;
      }
      if ( opt )  {
        ::perror(opt);
      }
    }
    return sc==WAIT_FAILED ? 0 : 1;
  }
  return 0;
}
/// Release lock object
int PVSS::pvss_release_lock(void* handle) {
  return handle ? ::ReleaseMutex(handle) == 0 ? 0 : 1 : 0;
}

#else
#include <dlfcn.h>
#include <unistd.h>
#include <semaphore.h>

#define LOAD_LIB(x)  ::dlopen( x , )
#define DLERROR      ::dlerror()

/// Sleep a specified amount of time
int  PVSS::pvss_sleep(int millisecs)  {
  int sec  = millisecs/1000;
  int msec = millisecs%1000;
  if ( sec ) ::sleep(sec);
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
  std::string dll = "lib";
  dll += lib;
  dll += ".so";
  printf("PVSS> --------> Loading DLL:%s\n",dll.c_str());
  void* handle = ::dlopen(dll.c_str(),RTLD_NOW);
  if ( 0 == handle )  {
    ::printf("PVSS> Failed to load library:%s\n",dll.c_str());
    ::printf("PVSS> Error: %s\n",pvss_load_error());
    return pvss_function_t(0);
  }
  return (pvss_function_t)::dlsym(handle,fun);
}

/// Start native thread
int PVSS::pvss_start_thread(void (*fun)(void*), void* arg)  {
  typedef void* (*pthread_fun)(void*);
  pthread_t h;
  int sc = ::pthread_create(&h,                     // Thread handle
                            0,                      // Thread attribute (0=default)
                            (pthread_fun)fun,       // thread function 
                            arg);                   // argument to thread function 
  return 0 == sc ? 1 : 0;
}

/// Terminate native thread
int PVSS::pvss_end_thread(int exit_code) {
  return 1;
}

/// Create lock to protect PVSS API manager from multithreaded trouble
int PVSS::pvss_create_lock(void** handle)   {
  sem_t* s = new sem_t;
  if ( 0 != ::sem_init(s, 0, 1) ) {
    delete s;
    ::perror("SEVERE: sem_open: ");
    *handle = 0;
    return 0;
  }
  *handle = s;
  return 1;
}

/// Delete PVSS lock object
int PVSS::pvss_delete_lock(void* handle)   {
  std::auto_ptr<sem_t> s((sem_t*)handle);
  return handle ? ::sem_destroy(s.get()) == 0 ? 1 : 0 : 0;
}

/// Take lock object
int PVSS::pvss_take_lock(void* handle) {
  return handle ? ::sem_wait((sem_t*)handle) == 0 ? 1 : 0 : 0;
}

/// Release lock object
int PVSS::pvss_release_lock(void* handle) {
  return handle ? ::sem_post((sem_t*)handle) == 0 ? 1 : 0 : 0;
}
#endif
