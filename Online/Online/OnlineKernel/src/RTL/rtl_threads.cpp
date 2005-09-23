#include <cstdio>
#include "rtl_internal.h"
  
namespace {   // Don't clutter global namespace
  typedef int (*timer_ast_t)(void*);
  struct timer_t  {
    void*  arg;
    void*  hdl;
    timer_ast_t ast;
  };
}

int lib_rtl_start_thread(lib_rtl_thread_routine_t start_routine, void* thread_arg, lib_rtl_thread_t* handle)  {
#ifdef USE_PTHREADS
  typedef void* (*pthread_fun)(void*);
  int sc = ::pthread_create(handle,                 // Thread handle
                            0,                      // Thread attribute (0=default)
                            (pthread_fun)start_routine,      // thread function 
                            thread_arg);            // argument to thread function 
  if ( 0 != sc )  {
    lib_rtl_signal_message(LIB_RTL_ERRNO, "lib_rtl_suspend_thread failed");
    return 0;
  }
#elif defined(_WIN32)
  DWORD thr_id;
  *handle = ::CreateThread( 
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
#endif
  return 1;
}

int lib_rtl_delete_thread(lib_rtl_thread_t handle)  {
  if ( handle )  {
#ifdef USE_PTHREADS
    void* value = 0;
    ::pthread_cancel(handle);
    ::pthread_join(handle, &value);
#elif defined(_WIN32)
    ::CloseHandle(handle);
#endif
    return 1;
  }
  lib_rtl_signal_message(LIB_RTL_DEFAULT, "lib_rtl_delete_thread failed [Invalid Handle]");
  return 0;
}

int lib_rtl_suspend_thread(lib_rtl_thread_t handle)  {
  if ( handle )  {
#ifdef USE_PTHREADS
    int sc = ::pthread_detach(handle); 
    if ( sc == 0 )  {
      return 1;
    }
    lib_rtl_signal_message(LIB_RTL_ERRNO, "lib_rtl_suspend_thread failed");
#elif defined(_WIN32)
    DWORD ret=::SuspendThread(handle);
    if ( ret == -1 )  {
      lib_rtl_signal_message(LIB_RTL_OS, "lib_rtl_suspend_thread failed");
    }
    return 1;
#endif
  }
  lib_rtl_signal_message(LIB_RTL_DEFAULT, "lib_rtl_suspend_thread failed [Invalid Handle]");
  return 0;
}

int lib_rtl_resume_thread(lib_rtl_thread_t handle)  {
  if ( handle )  {
#ifdef USE_PTHREADS
    lib_rtl_signal_message(LIB_RTL_DEFAULT,"lib_rtl_resume_thread failed [No pthread call]");
    return 0;
#elif defined(_WIN32)
    DWORD ret=::ResumeThread(handle);
    if ( ret == -1 )  {
      lib_rtl_signal_message(LIB_RTL_OS,"lib_rtl_resume_thread failed");
      return 0;
    }
    return 1;
#endif
  }
  lib_rtl_signal_message(LIB_RTL_DEFAULT,"lib_rtl_resume_thread failed [Invalid Handle]");
  return 0;
}
