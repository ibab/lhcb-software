#ifndef _RTL_TIMERMANAGER_H
#define _RTL_TIMERMANAGER_H

#include "RTL/rtl.h"
#include "RTL/que.h"

/*
 *   RTL namespace declaration
 */
namespace RTL  {

  struct timer_entry_t : public qentry_t {
    timer_entry_t() : qentry_t(0,0), magic(0), expire(0), hdl(0), ast(0), param(0), period(0) {}
    unsigned int             magic;
    unsigned int             expire;
    void*                    hdl;
    lib_rtl_timer_routine_t  ast;
    void*                    param;
    unsigned int             period;
  };

  /** @class TimerManager
   */
  class TimerManager {
    lib_rtl_thread_t        m_thread;
    lib_rtl_lock_t          m_lock;
    lib_rtl_event_t         m_flag;
    std::auto_ptr<qentry_t> m_head;
    bool                    m_active;

    /// Exit handler for timer thread
    static int exit_timers(void* param);

    /// Thread callback
    static int timer_call(void* param);

    /// Internal timers cleanup
    int cleanup();

    /// Check timer queue for new events
    unsigned int check();

    /// Standard Constructor 
    TimerManager();

    /// Standard Destructor
    ~TimerManager();

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
    /// Timer thread instance
    static TimerManager& instance();
  };
}

#endif // _RTL_TIMERMANAGER_H
