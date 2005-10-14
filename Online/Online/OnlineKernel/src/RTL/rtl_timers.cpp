#include "rtl_internal.h"
#include "RTL/que.h"
#include "RTL/DoubleLinkedQueue.h"

#define GENERIC_TIMERS 1

typedef unsigned long long int uint64_t;
namespace {
  typedef int (*timer_ast_t)(void*);
  struct timer_entry_t : public qentry_t {
    unsigned int magic;
    uint64_t     expire;
    void*        hdl;
    timer_ast_t  ast;
    void*        param;
    uint64_t     period;
    timer_entry_t(timer_ast_t r, void* a, uint64_t p=0) 
      : qentry_t(0,0), magic(0xFEEDBABE), hdl(0), ast(r), param(a), period(p) {}
  };
}
#if defined(GENERIC_TIMERS)

#include <ctime>

namespace RTL {

  /** @class SysTime
  */
  class SysTime {
  protected:
    uint64_t m_start;
    SysTime() {
      m_start = SysTime::now();
    }
    static SysTime& instance() {
      static SysTime t;
      return t;
    }
  public:
    static uint64_t start() {  return instance().m_start;    }
    static uint64_t now()   {
#ifdef _WIN32
      return ::GetTickCount();
#else
      timespec t;
      ::clock_gettime(CLOCK_REALTIME,&t);
      return uint64_t(t.tv_sec*1000) + uint64_t(t.tv_nsec)/1000000;
#endif
    }
    static uint64_t uptime() { return now() - start(); }
  };


  /** @class TimerThread
  */
  class TimerThread {
    lib_rtl_thread_t        m_thread;
    lib_rtl_lock_t          m_lock;
    lib_rtl_event_t         m_flag;
    std::auto_ptr<qentry_t> m_head;
    bool                    m_active;
    uint64_t                m_waitTime;

    /// Exit handler for timer thread
    static int exit_timers(void* param);

    /// Thread callback
    static int timer_call(void* param);

    /// Internal timers cleanup
    int cleanup();

    /// Check timer queue for new events
    uint64_t check();

    /// Standard Constructor 
    TimerThread();

    /// Standard Destructor
    ~TimerThread();

    /// Start the timer thread
    int start();

    /// Stop the timer thread
    int stop();

  public:

    /// Add new timer entry to thread
    int add(timer_entry_t* entry);
    /// Remove timer entry from thread
    int remove(timer_entry_t* entry);
    /// Check if timer thread is active
    bool isActive()  const {
      return m_active;
    }

  public:
    static TimerThread& instance() {
      static TimerThread t;
      return t;
    }
  };
}
#elif defined(_WIN32)

#include <windows.h>
extern "C" __declspec(dllimport) BOOL  __stdcall CancelWaitableTimer(void*);
extern "C" __declspec(dllimport) void* __stdcall CreateWaitableTimerA(void*, DWORD, void*);
extern "C" __declspec(dllimport) BOOL  __stdcall SetWaitableTimer(void*, const LARGE_INTEGER*, DWORD, void*, void*, BOOL);

static void CALLBACK lib_rtl_timer_proc(void* arg, DWORD dwTimeHigh, DWORD dwTimeLow)  {
  timer_entry_t *t = (timer_entry_t*)arg;
  ::CloseHandle( t->hdl );
  if ( t->ast )  {
    t->ast ( t->arg );
  }
  delete t;
}

#endif

/// Standard Constructor
RTL::TimerThread::TimerThread() 
: m_head(new qentry_t(0,0)), m_active(false) 
{
  m_waitTime = 9999999999999L;
  int sc = lib_rtl_create_lock(0,&m_lock);
  if ( !lib_rtl_is_success(sc) )  {
    lib_rtl_signal_message(0,"Failed to create timer lock");
    return;
  }
  sc = lib_rtl_create_event(0,&m_flag);
  if ( !lib_rtl_is_success(sc) )  {
    lib_rtl_signal_message(0,"Failed to create event timer flag");
    return;
  }
  sc = lib_rtl_declare_exit(exit_timers,this);
  if ( !lib_rtl_is_success(sc) )  {
    lib_rtl_signal_message(0,"Failed to declare the timer exit handler");
    return;
  }
}
/// Standard Destructor
RTL::TimerThread::~TimerThread() {
  stop();
  lib_rtl_delete_lock(m_lock);
  lib_rtl_delete_event(m_flag);
  lib_rtl_remove_exit(exit_timers,this);
}

/// Add new timer entry to thread
int RTL::TimerThread::add(timer_entry_t* entry) {
  lib_rtl_lock(m_lock);
  insqti(entry, m_head.get());
  lib_rtl_unlock(m_lock);
  lib_rtl_clear_event(m_flag);
  lib_rtl_set_event(m_flag);
  //printf("Add timer:%lld\n",entry->expire-SysTime::uptime());
  if ( !isActive() ) start();
  return 1;
}

/// Remove timer entry from thread
int RTL::TimerThread::remove(timer_entry_t* entry) {
  lib_rtl_lock(m_lock);
  remqent(entry);
  return lib_rtl_unlock(m_lock);
  if ( !isActive() ) start();
  return 1;
}

/// Exit handler for timer thread
int RTL::TimerThread::exit_timers(void* param) {
  TimerThread* thr = (TimerThread*)param;
  return thr->cleanup();
}

/// Thread callback
int RTL::TimerThread::timer_call(void* param)  {
  uint64_t millisec = 10;
  TimerThread* thr = (TimerThread*)param;
  while( thr->isActive() )  {
    int sc = lib_rtl_timedwait_for_event(thr->m_flag, millisec);
    if ( !lib_rtl_is_success(sc) ) {
      lib_rtl_signal_message(LIB_RTL_OS,"Timer thread failed to get event!");
      // return 0;
    }
    if ( thr->isActive() ) {
      lib_rtl_clear_event(thr->m_flag);
    }
    if ( thr->isActive() ) {
      millisec = thr->check();
    }
  }
  return thr->stop();
}

/// Internal timers cleanup
int RTL::TimerThread::cleanup() {
  DoubleLinkedQueue<timer_entry_t> que(m_head.get());
  for(timer_entry_t* e=que.get(); e; e = que.get()) {
    remqent(e);
    delete e;
  }
  m_active = false;
  lib_rtl_set_event(m_flag);
  return 1;
}

/// Check timer queue for new events
uint64_t RTL::TimerThread::check() {
  uint64_t next = 9999999999L;
  uint64_t now = SysTime::uptime();
  lib_rtl_lock(m_lock);
  int nent=0;
  DoubleLinkedQueue<timer_entry_t> que(m_head.get());
  for(timer_entry_t* e=que.get(); e; e = que.get()) {
    try {
      nent++;
      //printf("Now: %lld Entry:%lld\n",now, e->expire);
      if ( now >= e->expire ) {
        lib_rtl_run_ast(e->ast, e->param, 0);
        if ( 0 == e->period ) {
          remqent(e);
          delete e;
          continue;
        }
        e->expire += e->period;
      }
      if ( next > (e->expire-now) ) next = e->expire-now;
    }
    catch(...) {
      ::printf("Exception in timer AST\n");
    }
  }
  lib_rtl_unlock(m_lock);
  m_waitTime = now+next;
  static int cnt = 0;
  //if ( (cnt%10000) == 0 )
  //printf("Now: %lld Wait for %lld milliseconds Nent:%d\n",m_waitTime, next, nent);
  cnt++;
  return next;
}

/// Start the timer thread
int RTL::TimerThread::start() {
  lib_rtl_lock(m_lock);
  m_active = true;
  int sc = lib_rtl_start_thread(timer_call, this, &m_thread);
  if ( !lib_rtl_is_success(sc) )  {
    lib_rtl_signal_message(0,"Failed to start timer thread");
  }
  return lib_rtl_unlock(m_lock);
}

/// Stop the timer thread
int RTL::TimerThread::stop() {
  lib_rtl_lock(m_lock);
  if ( m_thread && isActive() ) {
    lib_rtl_thread_t t = m_thread;
    m_thread = 0;
    m_active = false;
    lib_rtl_delete_thread(t);
  }
  return lib_rtl_unlock(m_lock);
}

int64_t lib_rtl_get_ticks()  {
  return RTL::SysTime::now();
}

int lib_rtl_set_timer(int milli_seconds, timer_ast_t ast, void* ast_param, unsigned int* timer_id)  {
  timer_entry_t* t = new timer_entry_t(ast, ast_param, 0);
  t->expire = RTL::SysTime::uptime()+milli_seconds;
#if defined(GENERIC_TIMERS)
  int sc = RTL::TimerThread::instance().add(t);
  if ( lib_rtl_is_success(sc) ) {
    *timer_id = (int)t;
    return sc;
  }
#elif defined(_WIN32)
  LARGE_INTEGER liDueTime;
  liDueTime.QuadPart=-10000*milli_seconds;
  t->hdl = ::CreateWaitableTimerA(0,TRUE,0);
  if ( t->hdl )  {
    BOOL result = ::SetWaitableTimer(t->hdl,&liDueTime,0,lib_rtl_timer_proc,t, FALSE);
    if ( result )  {
      *timer_id = (int)t;
      return 1;
    }
    ::CloseHandle(t->hdl);
  }
#endif
  delete t;
  return 0;
}

int lib_rtl_kill_timer(int timer_id) {
  timer_entry_t* t = (timer_entry_t*)timer_id;
  if ( t )  {
    if ( t->magic == 0xFEEDBABE ) {
#if defined(GENERIC_TIMERS)
      int sc = RTL::TimerThread::instance().remove(t);
      delete t;
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
