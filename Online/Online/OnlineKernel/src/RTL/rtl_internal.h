#ifndef _RTL_INTERNAL_H
#define _RTL_INTERNAL_H

#ifndef _WIN32
#define USE_PTHREADS
#endif

#ifdef USE_PTHREADS
  #include "pthread.h"
  #include "semaphore.h"
  typedef pthread_t lib_rtl_thread_t;
  typedef sem_t*    lib_rtl_lock_t;
  typedef sem_t*    lib_rtl_event_t;

#elif defined(_WIN32)
  #include <windows.h>
  typedef HANDLE lib_rtl_thread_t;
  typedef HANDLE lib_rtl_event_t;
  typedef HANDLE lib_rtl_lock_t;
  extern "C" __declspec(dllimport) BOOL __stdcall CancelWaitableTimer(void*);
  extern "C" __declspec(dllimport) void* __stdcall CreateWaitableTimerA(void*, DWORD, void*);
  extern "C" __declspec(dllimport) BOOL __stdcall SetWaitableTimer(void*, const LARGE_INTEGER*, DWORD, void*, void*, BOOL);
#endif
const char* errorString(int status);
int getError();
const char* errorString();
const char* printError();

#include "RTL/rtl.h"
  struct lib_rtl_action  {
  int   flag;
  long (*action)(void*);
  void* param;
};

#include <map>
#include <string>
#include <vector>
typedef std::map<int, lib_rtl_event_t>         lib_rtl_event_map_t;
typedef std::map<std::string, lib_rtl_event_t> lib_rtl_named_event_map_t;
typedef std::map<int, lib_rtl_thread_t>        lib_rtl_thread_map_t;

namespace RTL  {
  struct ExitHandler : public std::vector<EXHDEF>  {
    ExitHandler();
    ~ExitHandler();
    static void execute();
    static std::vector<EXHDEF>& exitHandlers();
  };
  struct EventLock  {
    EventLock() {}
    ~EventLock() {}
    int status() const { return 1; }
    operator bool()  const { return true; }
  };
  lib_rtl_event_map_t&       eventHandlers();
  lib_rtl_named_event_map_t& namedEventHandlers();
  lib_rtl_thread_map_t&      waitEventThreads();
}

#endif // _RTL_INTERNAL_H
