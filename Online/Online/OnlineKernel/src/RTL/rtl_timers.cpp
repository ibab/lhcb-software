#include "RTL/rtl.h"
#include <vector>
#include <map>
#include <string>

#ifdef _WIN32
#include <windows.h>
  extern "C" __declspec(dllimport) BOOL  __stdcall CancelWaitableTimer(void*);
  extern "C" __declspec(dllimport) void* __stdcall CreateWaitableTimerA(void*, DWORD, void*);
  extern "C" __declspec(dllimport) BOOL  __stdcall SetWaitableTimer(void*, const LARGE_INTEGER*, DWORD, void*, void*, BOOL);

  typedef int (*timer_ast_t)(void*);
  struct timer_t  {
    void*  arg;
    HANDLE hdl;
    timer_ast_t ast;
  };
#endif


static void CALLBACK lib_rtl_timer_proc(void* arg, DWORD dwTimeHigh, DWORD dwTimeLow)  {
  timer_t *t = (timer_t*)arg;
  ::CloseHandle( t->hdl );
  if ( t->ast )  {
    t->ast ( t->arg );
  }
  delete t;
}

int lib_rtl_set_timer(int milli_seconds, timer_ast_t ast, void* ast_param, unsigned int* timer_id)  {
  LARGE_INTEGER liDueTime;
  liDueTime.QuadPart=-10000*milli_seconds;
  timer_t* t = new timer_t;
  t->arg = ast_param;
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
  delete t;
  return 0;
}

int lib_rtl_kill_timer(int timer_id) {
  timer_t* t = (timer_t*)timer_id;
  if ( t )  {
    BOOL result = ::CancelWaitableTimer(t->hdl);
    result = ::CloseHandle( t->hdl );
    delete t;
    return 1;
  }
  return 0;
}
