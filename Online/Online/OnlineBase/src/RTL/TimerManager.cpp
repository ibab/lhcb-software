#include "RTL/SysTime.h"
#include "RTL/TimerManager.h"
#include "RTL/DoubleLinkedQueue.h"
#include <vector>

/// Timer thread instance
RTL::TimerManager& RTL::TimerManager::instance() {
  static TimerManager t;
  return t;
}

/// Standard Constructor
RTL::TimerManager::TimerManager() 
  : m_head(new qentry_t(0,0)), m_active(false) 
{
  int sc = ::lib_rtl_create_lock(0,&m_lock);
  if ( !lib_rtl_is_success(sc) )  {
    ::lib_rtl_signal_message(0,"Failed to create timer lock");
    return;
  }
  sc = ::lib_rtl_create_event(0,&m_flag);
  if ( !lib_rtl_is_success(sc) )  {
    ::lib_rtl_signal_message(0,"Failed to create event timer flag");
    return;
  }
  sc = ::lib_rtl_declare_exit(exit_timers,this);
  if ( !lib_rtl_is_success(sc) )  {
    ::lib_rtl_signal_message(0,"Failed to declare the timer exit handler");
    return;
  }
}

/// Standard Destructor
RTL::TimerManager::~TimerManager() {
  stop();
  ::lib_rtl_delete_lock(m_lock);
  ::lib_rtl_delete_event(m_flag);
  ::lib_rtl_remove_exit(exit_timers,this);
}

/// Add new timer entry to thread
int RTL::TimerManager::add(timer_entry_t* entry) {
  ::lib_rtl_lock(m_lock);
  insqti(entry, m_head.get());
  ::lib_rtl_unlock(m_lock);
  ::lib_rtl_clear_event(m_flag);
  ::lib_rtl_set_event(m_flag);
  return isActive() ? 1 : start();
}

/// Remove timer entry from thread
int RTL::TimerManager::remove(timer_entry_t* entry) {
  timer_entry_t* e, *fnd = 0;
  ::lib_rtl_lock(m_lock);
  {
    DoubleLinkedQueue<timer_entry_t> que(m_head.get());
    for(e=que.get(); e; e = que.get()) {
      if ( e == entry ) {
        fnd = e;
        break;
      }
    }
  }
  if ( fnd ) {
    fnd->magic = 0;
    remqent(fnd);
    delete fnd;
  }
  return ::lib_rtl_unlock(m_lock);
}

/// Exit handler for timer thread
int RTL::TimerManager::exit_timers(void* param) {
  TimerManager* thr = (TimerManager*)param;
  return thr->cleanup();
}

/// Thread callback
int RTL::TimerManager::timer_call(void* param)  {
  unsigned int millisec = 10;
  TimerManager* thr = (TimerManager*)param;
  while( thr->isActive() )  {
    int sc = ::lib_rtl_timedwait_for_event(thr->m_flag, millisec);
    if ( sc != 1 && sc != 2 ) {
      ::lib_rtl_signal_message(LIB_RTL_OS,"Timer thread failed to get event! sc=%d",sc);
      // return 0;
    }
    if ( thr->isActive() ) {
      ::lib_rtl_clear_event(thr->m_flag);
    }
    if ( thr->isActive() ) {
      millisec = thr->check();
    }
  }
  return thr->stop();
}

/// Internal timers cleanup
int RTL::TimerManager::cleanup() {
  DoubleLinkedQueue<timer_entry_t> que(m_head.get());
  for(timer_entry_t* e=que.get(); e; e = que.get()) {
    remqent(e);
    delete e;
  }
  m_active = false;
  ::lib_rtl_set_event(m_flag);
  return 1;
}

/// Check timer queue for new events
unsigned int RTL::TimerManager::check() {
  std::vector<timer_entry_t*> to_remove;
  unsigned int next = 9999999;
  unsigned int now = SysTime::uptime();
  timer_entry_t* e;
  int nent=0;
  ::lib_rtl_lock(m_lock);
  {
    DoubleLinkedQueue<timer_entry_t> que(m_head.get());
    for(e=que.get(); e; e = que.get()) {
      try {
        nent++;
        //lib_rtl_output(LIB_RTL_DEBUG,"%p [%d] Now: %lld Entry:%lld\n",e, nent, now, e->expire);
        if ( e->magic == 0xFEEDBABE ) {
          if ( now >= e->expire ) {
            lib_rtl_run_ast(e->ast, e->param, 0);
            if ( 0 == e->period ) {
              to_remove.push_back(e);
              continue;
            }
            e->expire += e->period;
          }
          if ( next > (e->expire-now) ) next = e->expire-now;
        }
        else {  
          ::lib_rtl_output(LIB_RTL_ERROR,"FATAL ERROR: BAD timer entry: %p\n",(void*)e);
          ::lib_rtl_sleep(10000);
        }
      }
      catch(...) {
        ::lib_rtl_output(LIB_RTL_ERROR,"Exception in timer AST\n");
      }
    }
  }
  //lib_rtl_output(LIB_RTL_DEBUG,"Timer queue is %d entries long\n",nent);
  std::vector<timer_entry_t*>::iterator i;
  for( i=to_remove.begin(); i != to_remove.end(); ++i) {
    e = *i;
    remqent(e);
    e->magic = 0;
    delete e;
  }
  ::lib_rtl_unlock(m_lock);
  return next;
}

/// Start the timer thread
int RTL::TimerManager::start() {
  ::lib_rtl_lock(m_lock);
  m_active = true;
  int sc = ::lib_rtl_start_thread(timer_call, this, &m_thread);
  if ( !lib_rtl_is_success(sc) )  {
    ::lib_rtl_signal_message(0,"Failed to start timer thread");
  }
  return ::lib_rtl_unlock(m_lock);
}

/// Stop the timer thread
int RTL::TimerManager::stop() {
  ::lib_rtl_lock(m_lock);
  if ( m_thread && isActive() ) {
    lib_rtl_thread_t t = m_thread;
    m_thread = 0;
    m_active = false;
    ::lib_rtl_delete_thread(t);
  }
  return ::lib_rtl_unlock(m_lock);
}
