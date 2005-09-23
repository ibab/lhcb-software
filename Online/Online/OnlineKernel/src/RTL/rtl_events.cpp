#include "rtl_internal.h"
#include <vector>
#include <map>
#include <string>
#include <fcntl.h>

namespace {
  struct EventLock  {
    EventLock() {}
    int status() const { return 1; }
    operator bool()  const { return true; }
  };
  struct EventHandlers : public std::map<int, lib_rtl_event_t>  {
  };
  std::map<int, lib_rtl_event_t>& eventHandlers() {
    static EventHandlers s_Handlers;
    return s_Handlers;
  }
  struct NamedEventHandlers : public std::map<std::string, lib_rtl_event_t>  {
  };
  std::map<std::string, lib_rtl_event_t>& namedEventHandlers() {
    static NamedEventHandlers s_Handlers;
    return s_Handlers;
  }
  typedef std::map<int, lib_rtl_thread_t> lib_rtl_thread_map_t;
  lib_rtl_thread_map_t& waitEventThreads() {
    static lib_rtl_thread_map_t s_map;
    return s_map;
  }
}

static lib_rtl_event_t getEventHandle(int flag)  {
  EventLock lock;
  if ( lock )  {
    EventHandlers::const_iterator i=eventHandlers().find(flag);
    if ( i != eventHandlers().end() ) {
      return (*i).second;
    }
    else {
      ::printf("Failed to get event flag [UNKNOWN EVENT FLAG]\n");
    }
  }
  return 0;
}

/* Not used ....
static lib_rtl_event_t getEventHandle(const char* name)  {
  EventLock lock;
  if ( lock )  {
    NamedEventHandlers::const_iterator i=namedEventHandlers().find(name);
    if ( i != namedEventHandlers().end() ) {
      return (*i).second;
    }
    else {
      ::printf("Failed to get event flag %s [UNKNOWN EVENT FLAG]\n", name);
    }
  }
  return 0;
}
*/

static lib_rtl_event_t openEventHandle(const char* name)  {
  EventLock lock;
  if ( lock )  {
    NamedEventHandlers::const_iterator i=namedEventHandlers().find(name);
    if ( i == namedEventHandlers().end() ) {
      int evt_flag = 0;
      lib_rtl_create_named_event(name, &evt_flag);
      return getEventHandle(evt_flag);
    }
    return (*i).second;
  }
  return 0;
}

/// Create named event for inter process communication
int lib_rtl_create_named_event (const char* name, int* event_flag)    {
  static int s_flg = 0;
  lib_rtl_event_t hdl = 0;
#if defined(USE_PTHREADS)
  int sc = 0;
  if ( name )  {
    hdl = ::sem_open(name, O_CREAT, 0644, 1);
  }
  else {
    hdl = new sem_t;
    sc = ::sem_init(hdl, 0, 1); 
    if ( sc != 0 )  {
      hdl = 0;
    }
  }
#elif defined(_WIN32)
  hdl = ::CreateEvent(NULL,TRUE,FALSE,name);
#endif
  if ( hdl == 0 )  {
    const char* n = name ? name : "<unnamed>";
    ::printf("Failed to create %s event flag [%s]\n", n, printError());
    *event_flag = 0;
    return 0;
  }
  EventLock lock;
  if ( lock )  {
    if ( name )  {
      namedEventHandlers().insert(std::make_pair(name,hdl));
    }
    *event_flag = ++s_flg;
    eventHandlers().insert(std::make_pair(*event_flag,hdl));
    return 1;
  }
  return lock.status();
}

int lib_rtl_delete_named_event(const char* name, int* flag)   {
  EventLock lock;
  if ( lock )  {
    if ( name )  {
      NamedEventHandlers::iterator j=namedEventHandlers().find(name);
      if ( j != namedEventHandlers().end() )  {
        namedEventHandlers().erase(j);
      }
    }
    lib_rtl_thread_map_t::iterator k = waitEventThreads().find(*flag);
    if ( k != waitEventThreads().end() )  {
      waitEventThreads().erase(k);
    }
    EventHandlers::iterator i=eventHandlers().find(*flag);
    if ( i != eventHandlers().end() ) {
      lib_rtl_event_t hdl = (*i).second;
      eventHandlers().erase(i);
#if defined(USE_PTHREADS)
      int status;
      if ( name )  {
        status = ::sem_close(hdl);
        ::sem_unlink(name);
      }
      else {
        status = ::sem_destroy(hdl);
        delete hdl;
      }
#elif defined(_WIN32)
      ::CloseHandle(hdl);
#endif
      *flag = 0;
      return 1;
    }
    else {
      ::printf("Failed to delete event flag [UNKNOWN EVENT FLAG]\n");
    }
  }
  return 0;
}

/// Create unnamed event for local process
int lib_rtl_create_event (int* event_flag)  {
  return lib_rtl_create_named_event(0, event_flag);
}

/// Create unnamed event for local process
int lib_rtl_delete_event (int* event_flag)  {
  return lib_rtl_delete_named_event(0, event_flag);
}

int lib_rtl_clear_event(int event_flag) {
  EventLock lock;
  if ( lock )  {
    EventHandlers::const_iterator i=eventHandlers().find(event_flag);
    if ( i != eventHandlers().end() ) {
      lib_rtl_event_t hdl = (*i).second;
#if defined(USE_PTHREADS)
      ::sem_trywait(hdl);
#elif defined(_WIN32)
      if ( ::ResetEvent(hdl) )
#endif
      {
        return 1;
      }
      ::printf("Failed to clear event flag [%s]\n", printError());
    }
    ::printf("Failed to clear event flag [UNKNOWN EVENT FLAG]\n");
    return 0;
  }
  return lock.status();
}

int lib_rtl_wait_for_event(int event_flag)    {
  lib_rtl_event_t hdl = getEventHandle(event_flag);
  if ( hdl )  {
#if defined(USE_PTHREADS)
    if ( ::sem_wait(hdl) == 0 )
#elif defined(_WIN32)
    if ( ::WaitForSingleObjectEx(hdl, INFINITE, TRUE) == WAIT_OBJECT_0 )  
#endif
    {
      return 1;
    }
  }
  return 0;
}

long lib_rtl_wait_event_a_call(void* param)  {
  lib_rtl_action* pars = (lib_rtl_action*)param;
  while(1)  {
    lib_rtl_clear_event(pars->flag);
    if ( pars->action )  {
      (*pars->action)(pars->param);
    }
    lib_rtl_wait_for_event(pars->flag);
  }
  return 1;
}

/// Wait asynchronously for an event flag
int lib_rtl_wait_for_event_a(int flag, lib_rtl_thread_routine_t action, void* param)  {
  int sc = 0;
  if ( waitEventThreads().find(flag) == waitEventThreads().end() )  {
    lib_rtl_action* act = new lib_rtl_action;
    act->action = action;
    act->param = param;
    act->flag = flag;
    lib_rtl_thread_t thread;
    sc = lib_rtl_start_thread(lib_rtl_wait_event_a_call, act, &thread);
    if ( !lib_rtl_is_success(sc) )  {
      lib_rtl_signal_message(0,"Failed to manipulate asynchronous wait event thread");
      return 0;
    }
    waitEventThreads().insert(std::make_pair(flag,thread));
  }
  lib_rtl_set_event(flag);
  return 1;
}

int lib_rtl_wait_for_multiple_events(int /* count */, void** /* handles */)   {
  return 1;
}

int lib_rtl_set_event(int event_flag)   {
  lib_rtl_event_t hdl = getEventHandle(event_flag);
  if ( hdl )  {
#if defined(USE_PTHREADS)
    if ( ::sem_post(hdl) == 0 )
#elif defined(_WIN32)
    if ( ::SetEvent(hdl) == WAIT_OBJECT_0 )  
#endif
    {
      return 1;
    }
  }
  return 0;
}

int lib_rtl_wakeup_process(int /* pid */, const char* flag_name, RTL_ast_t /* astadd */, void* /* astpar */, int /*mode*/)    {
  lib_rtl_event_t hdl = openEventHandle(flag_name);
  if ( hdl )  {
#if defined(USE_PTHREADS)
    if ( ::sem_post(hdl) == 0 )
#elif defined(_WIN32)
    if ( ::SetEvent(hdl) != 0 )  
#endif
    {
      return 1;
    }
  }
  return 0;
}
