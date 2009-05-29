#include "RTL/rtl.h"

#define GENERIC_TIMERS 1

#if defined(GENERIC_TIMERS)
#include "RTL/SysTime.h"
#include "RTL/TimerManager.h"
#elif defined(_WIN32)
#include <windows.h>
extern "C" __declspec(dllimport) BOOL  __stdcall CancelWaitableTimer(void*);
extern "C" __declspec(dllimport) void* __stdcall CreateWaitableTimerA(void*, DWORD, void*);
extern "C" __declspec(dllimport) BOOL  __stdcall SetWaitableTimer(void*, const LARGE_INTEGER*, DWORD, void*, void*, BOOL);
static void CALLBACK lib_rtl_timer_proc(void* arg, DWORD dwTimeHigh, DWORD dwTimeLow)  {
  RTL::timer_entry_t *t = (RTL::timer_entry_t*)arg;
  ::CloseHandle( t->hdl );
  if ( t->ast )  {
    t->ast ( t->arg );
  }
  delete t;
}
#endif

/// Get tick count since image startup
unsigned int lib_rtl_get_ticks()  {
  return RTL::SysTime::now();
}

int lib_rtl_set_timer(int milli_seconds, lib_rtl_timer_routine_t ast, void* ast_param, unsigned long* timer_id)  {
  std::auto_ptr<RTL::timer_entry_t> t(new RTL::timer_entry_t);
  t->next = t->prev = 0;
  t->magic = 0xFEEDBABE;
  t->hdl = 0;
  t->ast = ast;
  t->param = ast_param;
  t->period = 0;
  t->expire = RTL::SysTime::uptime()+milli_seconds;
#if defined(GENERIC_TIMERS)
  int sc = RTL::TimerManager::instance().add(t.get());
  if ( lib_rtl_is_success(sc) ) {
    *timer_id = (long)t.release();
    return sc;
  }
#elif defined(_WIN32)
  LARGE_INTEGER liDueTime;
  liDueTime.QuadPart=-10000*milli_seconds;
  t->hdl = ::CreateWaitableTimerA(0,TRUE,0);
  if ( t->hdl )  {
    BOOL result = ::SetWaitableTimer(t->hdl,&liDueTime,0,lib_rtl_timer_proc,t.get(), FALSE);
    if ( result )  {
      *timer_id = (int)t.release();
      return 1;
    }
    ::CloseHandle(t->hdl);
  }
#endif
  return 0;
}

int lib_rtl_kill_timer(long timer_id) {
  RTL::timer_entry_t* t = (RTL::timer_entry_t*)timer_id;
  if ( t )  {
    if ( t->magic == 0xFEEDBABE ) {
#if defined(GENERIC_TIMERS)
      int sc = RTL::TimerManager::instance().remove(t);
      return sc;
#elif defined(_WIN32)
      BOOL result = ::CancelWaitableTimer(t->hdl);
      result = ::CloseHandle( t->hdl );
      delete t;
      return 1;
#endif
    }
  }
  return 0;
}
