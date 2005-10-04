#include "rtl_internal.h"
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <cerrno>
#include <fcntl.h>

using namespace RTL;
typedef std::map<lib_rtl_event_t, lib_rtl_thread_t> lib_rtl_thread_map_t;
static lib_rtl_thread_map_t& waitEventThreads() {
  static lib_rtl_thread_map_t s_map;
  return s_map;
}

/// Create named event for inter process communication
int lib_rtl_create_event (const char* name, lib_rtl_event_t* event_flag)    {
  std::auto_ptr<rtl_event> h(new rtl_event);
  ::memset(h->name,0,sizeof(h->name));
  if ( name )  {
    ::sprintf(h->name,"/%s",name);
    h->name[sizeof(h->name)-1] = 0;
  }
#if defined(USE_PTHREADS)
  h->handle = h->name[0] ? ::sem_open(h->name, O_CREAT, 0644, 1) : &h->handle2;
  int sc = h->handle ? ::sem_init(h->handle, h->name[0] ? 1 : 0, 1) : (errno=EBADR); 
  if ( sc != 0 )  {
    h->handle = 0;
  }
#elif defined(_WIN32)
  h->handle = ::CreateEvent(NULL,TRUE,FALSE,name ? h->name : 0);
#endif
  if ( h->handle == 0 )  {
    lib_rtl_signal_message(LIB_RTL_OS,"Failed to create %s event flag [%s]", 
          name ? name : "<unnamed>");
    *event_flag = 0;
    return 0;
  }
  *event_flag = h.release();
  return 1;
}

int lib_rtl_delete_event(lib_rtl_event_t handle)   {
  if ( handle )  {
    std::auto_ptr<rtl_event> h(handle);
#if defined(USE_PTHREADS)
    h->name[0] ? ::sem_close(h->handle) : ::sem_destroy(h->handle);
    if ( h->handle == &h->handle2 )  {
      ::sem_unlink(h->name);
    }
#elif defined(_WIN32)
    HRESULT sc = ::CloseHandle(h->handle);
    if ( sc != 0 )  {
      return lib_rtl_signal_message(LIB_RTL_OS,"Failed to delete event flag 0x%08X", h->handle);
    }
#endif
    return 1;
  }
  lib_rtl_signal_message(LIB_RTL_DEFAULT,"Failed to delete event flag [UNKNOWN EVENT FLAG]");
  return 0;
}

int lib_rtl_clear_event(lib_rtl_event_t h) {
  if ( h )  {
#if defined(USE_PTHREADS)
    ::sem_trywait(h->handle);
#elif defined(_WIN32)
    if ( ::ResetEvent(h->handle) )
#endif
    {
      return 1;
    }
    lib_rtl_signal_message(LIB_RTL_OS,"Failed to clear event flag 0x%08X", h->handle);
    return 0;
  }
  return 1;
}

int lib_rtl_wait_for_event(lib_rtl_event_t h)    {
  if ( h )  {
#if defined(USE_PTHREADS)
    if ( ::sem_wait(h->handle) == 0 )
#elif defined(_WIN32)
    if ( ::WaitForSingleObjectEx(h->handle, INFINITE, TRUE) == WAIT_OBJECT_0 )  
#endif
    {
      return 1;
    }
  }
  return 0;
}

int lib_rtl_wait_event_a_call(void* param)  {
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
int lib_rtl_wait_for_event_a(lib_rtl_event_t flag, lib_rtl_thread_routine_t action, void* param)  {
  int sc = 0;
  if ( waitEventThreads().find(flag) == waitEventThreads().end() )  {
    lib_rtl_action* act = new lib_rtl_action;
    act->action = action;
    act->param  = param;
    act->flag   = flag;
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

int lib_rtl_set_event(lib_rtl_event_t h)   {
  if ( h )  {
#if defined(USE_PTHREADS)
    if ( ::sem_post(h->handle) == 0 )
#elif defined(_WIN32)
    if ( ::SetEvent(h->handle) == WAIT_OBJECT_0 )  
#endif
    {
      return 1;
    }
  }
  return 0;
}

int lib_rtl_set_global_event(const char* name)   {
  typedef std::map<std::string,lib_rtl_event_t> event_map;
  static event_map events;
  event_map::const_iterator i = events.find(std::string(name));
  lib_rtl_event_t h;
  if ( i == events.end() ) {
    int sc = lib_rtl_create_event(name, &h);
    if ( sc == 1 ) {
      events.insert(event_map::value_type(name,h));
    }
  }
  else {
    h = (*i).second;
  }
  if ( h )  {
#if defined(USE_PTHREADS)
    if ( ::sem_post(h->handle) == 0 )
#elif defined(_WIN32)
    if ( ::SetEvent(h->handle) == WAIT_OBJECT_0 )  
#endif
    {
      return 1;
    }
  }
  return 0;
}
