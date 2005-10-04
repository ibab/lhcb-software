#ifndef _RTL_INTERNAL_H
#define _RTL_INTERNAL_H

#ifndef _WIN32
#define USE_PTHREADS
#endif

#ifdef USE_PTHREADS
  #include "pthread.h"
  #include "semaphore.h"
  struct rtl_thread  {
    pthread_t    handle;
  };
  struct rtl_lock  {
    sem_t* handle;
    sem_t  handle2;
    char   name[32];
    int    held;
  };
  struct rtl_event  {
    sem_t* handle;
    sem_t  handle2;
    char  name[32];
    int    held;
  };

#elif defined(_WIN32)
  #include <windows.h>
  struct rtl_thread  {
    HANDLE    handle;
  };
  struct rtl_lock  {
    HANDLE handle;
    char   name[32];
    int    held;
  };
  struct rtl_event  {
    HANDLE handle;
    char   name[32];
    int    held;
  };
  extern "C" __declspec(dllimport) BOOL  __stdcall CancelWaitableTimer(void*);
  extern "C" __declspec(dllimport) void* __stdcall CreateWaitableTimerA(void*, DWORD, void*);
  extern "C" __declspec(dllimport) BOOL  __stdcall SetWaitableTimer(void*, const LARGE_INTEGER*, DWORD, void*, void*, BOOL);
#endif

struct lib_rtl_gbl  {
  void*       address;
  void*       addaux;
  int         size;
  int         fd;
  char        name[32];
};
typedef rtl_thread* lib_rtl_thread_t;
typedef rtl_lock*   lib_rtl_lock_t;
typedef rtl_event*  lib_rtl_event_t;
const char* errorString(int status);
int getError();
const char* errorString();

#include "RTL/rtl.h"
  struct lib_rtl_action  {
  lib_rtl_event_t flag;
  int (*action)(void*);
  void* param;
};
#endif // _RTL_INTERNAL_H
