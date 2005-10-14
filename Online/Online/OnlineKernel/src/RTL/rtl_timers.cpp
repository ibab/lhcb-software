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

#include <list>
struct lib_rtl_timer_entry  {
  timer_ast_t  ast;
  void*        param;
  lib_rtl_timer_entry() : ast(0), param(0) {}
  lib_rtl_timer_entry(timer_ast_t a, void* p) : ast(a), param(p) {}
  lib_rtl_timer_entry(const lib_rtl_timer_entry& copy) : ast(copy.ast), param(copy.param) {}
  lib_rtl_timer_entry& operator=(const lib_rtl_timer_entry c)  {
    ast = c.ast;
    param = c.param;
    return *this;
  }
  void execute()  {
    lib_rtl_run_ast(ast, param, 3);
  }
};

typedef std::list<lib_rtl_timer_entry> timer_collection_t;
timer_collection_t& timers()  {
  static timer_collection_t s;
  return s;
}

int lib_rtl_get_ticks()  {
  return 0;
}

static int lib_rtl_timer_call(void* param)  {
  timer_collection_t* c = (timer_collection_t*)param;
  if ( c )  {
    int now = lib_rtl_get_ticks();
//    for (timer_collection_t::iterator i=c->begin(); i!=c->end(); ++i)  {
//      timer_entry_t* e = (*i).second;
//      if ( now - e->start >= e->count )  {
//      }
//    }
  }
  return 1;
}

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
  timers().push_back(lib_rtl_timer_entry(ast,ast_param));
  *timer_id = id;  
  lib_rtl_action* act = new lib_rtl_action;
  act->action = action;
  act->param  = param;
  act->flag   = flag;
  lib_rtl_thread_t thread;
  sc = lib_rtl_start_thread(lib_rtl_timer_call, &timers(), &thread);
  if ( !lib_rtl_is_success(sc) )  {
    lib_rtl_signal_message(0,"Failed to manipulate asynchronous wait event thread");
    return 0;
  }
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
    timer_collection_t::iterator i=timers().find(timer_id);
    if ( i != timers().end() )  {
      timers().erase(i);
    }
#endif
  }
  return 0;
}
