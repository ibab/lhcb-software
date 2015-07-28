#ifndef _RTL_INTERNAL_H
#define _RTL_INTERNAL_H

#ifndef _WIN32
#define USE_PTHREADS
#include <sys/stat.h>
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
#define _WIN32_WINNT 0x0500
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
#if 0
extern "C" __declspec(dllimport) BOOL  __stdcall CancelWaitableTimer(void*);
extern "C" __declspec(dllimport) void* __stdcall CreateWaitableTimerA(void*, DWORD, void*);
extern "C" __declspec(dllimport) BOOL  __stdcall SetWaitableTimer(void*, const LARGE_INTEGER*, DWORD, void*, void*, BOOL);
#endif
#endif

typedef rtl_thread* lib_rtl_thread_t;
typedef rtl_lock*   lib_rtl_lock_t;
typedef rtl_event*  lib_rtl_event_t;

#include "RTL/rtl.h"
struct lib_rtl_gbl_desc : public lib_rtl_gbl {
  int         fd;
  char        name[32];
};

struct lib_rtl_action  {
  lib_rtl_event_t flag;
  int (*action)(void*);
  void* param;
};
#endif // _RTL_INTERNAL_H
