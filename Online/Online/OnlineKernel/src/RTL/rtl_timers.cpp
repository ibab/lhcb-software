#include "RTL/rtl.h"
#include <vector>
#include <map>
#include <string>

namespace {
  typedef int (*timer_ast_t)(void*);
  struct timer_entry_t  {
    void*  arg;
    void*  hdl;
    timer_ast_t ast;
  };
}
#ifdef _WIN32
#include <windows.h>
  extern "C" __declspec(dllimport) BOOL  __stdcall CancelWaitableTimer(void*);
  extern "C" __declspec(dllimport) void* __stdcall CreateWaitableTimerA(void*, DWORD, void*);
  extern "C" __declspec(dllimport) BOOL  __stdcall SetWaitableTimer(void*, const LARGE_INTEGER*, DWORD, void*, void*, BOOL);
#endif


#if defined(_WIN32)
static void CALLBACK lib_rtl_timer_proc(void* arg, DWORD dwTimeHigh, DWORD dwTimeLow)  {
  timer_entry_t *t = (timer_entry_t*)arg;
  ::CloseHandle( t->hdl );
  if ( t->ast )  {
    t->ast ( t->arg );
  }
  delete t;
}
#elif defined(USE_PTHREADS)
#endif

int lib_rtl_set_timer(int milli_seconds, timer_ast_t ast, void* ast_param, unsigned int* timer_id)  {
  timer_entry_t* t = new timer_entry_t;
  t->arg = ast_param;
#if defined(_WIN32)
  LARGE_INTEGER liDueTime;
  liDueTime.QuadPart=-10000*milli_seconds;
  t->hdl = ::CreateWaitableTimerA(0,TRUE,0);
  if ( t->hdl )  {
    t->ast = ast;
    BOOL result = ::SetWaitableTimer(t->hdl,&liDueTime,0,lib_rtl_timer_proc,t, FALSE);
    if ( result )  {
      *timer_id = (int)t;
      return 1;
    }
    ::CloseHandle(t->hdl);
  }
#elif defined(USE_PTHREADS)
#endif
  delete t;
  return 0;
}

int lib_rtl_kill_timer(int timer_id) {
  timer_entry_t* t = (timer_entry_t*)timer_id;
  if ( t )  {
#if defined(_WIN32)
    BOOL result = ::CancelWaitableTimer(t->hdl);
    result = ::CloseHandle( t->hdl );
    delete t;
    return 1;
#elif defined(USE_PTHREADS)
#endif
  }
  return 0;
}
