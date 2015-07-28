#include <cstdio>
#include "rtl_internal.h"
#ifdef USE_PTHREADS
#include <signal.h>
#endif


namespace {   // Don't clutter global namespace
  typedef int (*timer_ast_t)(void*);
  struct timer_t  {
    void*  arg;
    void*  hdl;
    timer_ast_t ast;
  };
}

void* lib_rtl_thread_id()  {
#ifdef USE_PTHREADS
  pthread_t id = ::pthread_self();
#elif defined(_WIN32)
  int64_t id = ::GetCurrentThreadId();
#endif
  return (void*)id;
}

int lib_rtl_is_current_thread(lib_rtl_thread_t handle)  {
  if ( handle )  {
#ifdef USE_PTHREADS
    return ::pthread_equal(pthread_self(),handle->handle) != 0 ? 1 : 0;
#elif defined(_WIN32)
    return handle->handle == ::GetCurrentThread() ? 1 : 0;
#endif
  }
  lib_rtl_signal_message(LIB_RTL_DEFAULT, "lib_rtl_suspend_thread failed [Invalid Handle]");
  return 0;
}

int lib_rtl_start_thread(lib_rtl_thread_routine_t start_routine, void* thread_arg, lib_rtl_thread_t* handle)  {
#ifdef USE_PTHREADS
  typedef void* (*pthread_fun)(void*);
  pthread_t h;
  int sc = ::pthread_create(&h,                     // Thread handle
                            0,                      // Thread attribute (0=default)
                            (pthread_fun)start_routine,      // thread function 
                            thread_arg);            // argument to thread function 
  if ( 0 != sc )  {
    lib_rtl_signal_message(LIB_RTL_ERRNO, "lib_rtl_suspend_thread failed");
    return 0;
  }
  *handle = new rtl_thread;
  (*handle)->handle = h;
#elif defined(_WIN32)
  DWORD thr_id;
  HANDLE h = ::CreateThread( 
                            NULL,                        // default security attributes 
                            0,                           // use default stack size  
                            (LPTHREAD_START_ROUTINE)start_routine,               // thread function 
                            thread_arg,                  // argument to thread function 
                            0,                           // use default creation flags 
                            (LPDWORD)&thr_id);           // returns the thread identifier 
  // Check the return value for success.  
  if (handle == 0)    {
    lib_rtl_signal_message(LIB_RTL_OS, "CreateThread failed");
    return 0;
  }
  *handle = new rtl_thread;
  (*handle)->handle = h;
#endif
  return 1;
}

int lib_rtl_delete_thread(lib_rtl_thread_t handle)  {
  if ( handle )  {
#ifdef USE_PTHREADS
    void* value = 0;
    ::pthread_cancel(handle->handle);
    ::pthread_join(handle->handle, &value);
#elif defined(_WIN32)
    ::CloseHandle(handle->handle);
#endif
    delete handle;
    return 1;
  }
  lib_rtl_signal_message(LIB_RTL_DEFAULT, "lib_rtl_delete_thread failed [Invalid Handle]");
  return 0;
}

int lib_rtl_suspend_thread(lib_rtl_thread_t handle)  {
  if ( handle )  {
#ifdef USE_PTHREADS
    int sc = ::pthread_detach(handle->handle); 
    if ( sc == 0 )  {
      return 1;
    }
    lib_rtl_signal_message(LIB_RTL_ERRNO, "lib_rtl_suspend_thread failed");
#elif defined(_WIN32)
    DWORD ret=::SuspendThread(handle->handle);
    if ( ret == -1 )  {
      lib_rtl_signal_message(LIB_RTL_OS, "lib_rtl_suspend_thread failed");
    }
    return 1;
#endif
  }
  lib_rtl_signal_message(LIB_RTL_DEFAULT, "lib_rtl_suspend_thread failed [Invalid Handle]");
  return 0;
}

/// Joind (and wait for finishing) executing thread
int lib_rtl_join_thread(lib_rtl_thread_t handle)   {
  if ( handle )  {
#ifdef USE_PTHREADS
    void* ret;
    int sc = ::pthread_join(handle->handle, &ret); 
    if ( sc == 0 )  {
      return 1;
    }
    lib_rtl_signal_message(LIB_RTL_ERRNO, "lib_rtl_join_thread failed");
#elif defined(_WIN32)
    DWORD sc = ::WaitForSingleObject(handle->handle, INFINITE);
    switch(sc)  {
    case WAIT_OBJECT_0:
    case WAIT_ABANDONED:
      ::CloseHandle(handle->handle);
      delete handle;
      return 1;
    case WAIT_TIMEOUT:
      lib_rtl_signal_message(LIB_RTL_OS, "lib_rtl_join_thread failed");
      delete handle;
      return 0;
    }
#endif
  }
  lib_rtl_signal_message(LIB_RTL_DEFAULT, "lib_rtl_join_thread failed [Invalid Handle]");
  return 0;
}

extern "C" int lib_rtl_kill_thread(lib_rtl_thread_t handle, int sig)  {
  if ( handle )  {
#ifdef USE_PTHREADS
    ::pthread_kill(handle->handle, sig);
#elif defined(_WIN32)
#endif
    return 1;
  }
  lib_rtl_signal_message(LIB_RTL_DEFAULT, "lib_rtl_delete_thread failed [Invalid Handle]");
  return 0;
}

/// Cancel thread execution
extern "C" int lib_rtl_cancel_thread(lib_rtl_thread_t handle)  {
  if ( handle )  {
#ifdef USE_PTHREADS
    ::pthread_cancel(handle->handle);
#elif defined(_WIN32)
#endif
    return 1;
  }
  lib_rtl_signal_message(LIB_RTL_DEFAULT, "lib_rtl_cancel_thread failed [Invalid Handle]");
  return 0;
}

