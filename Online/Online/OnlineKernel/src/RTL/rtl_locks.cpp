#include "rtl_internal.h"
#include <memory>
#include <fcntl.h>

extern "C" int lib_rtl_lock_value(lib_rtl_lock_t handle, int* value)   {
  if ( handle ) {
#if defined(USE_PTHREADS)
    int sc = ::sem_getvalue(handle->handle, value);
    return sc==0 ? 1 : 0;
#elif defined(_WIN32)
    return 0;
#endif
  }
  return 0;
}

int lib_rtl_create_lock(const char* mutex_name, lib_rtl_lock_t* handle)   {
  std::auto_ptr<rtl_lock> h(new rtl_lock); 
  ::memset(h->name,0,sizeof(h->name));
  if ( mutex_name )  {
    ::sprintf(h->name,"/%s",mutex_name);
    h->name[sizeof(h->name)-1] = 0;
  }
  h->held = 0;
#if defined(USE_PTHREADS)
  int sc = 0;
  h->handle = h->name[0] ? ::sem_open(h->name, O_CREAT|O_EXCL, 0644, 1) : &h->handle2;
  if ( h->handle ) {
    sc = ::sem_init(h->handle, h->name[0] ? 0 : 1, 1);
  }
  else if ( h->name[0] ) {
    h->handle = ::sem_open(h->name, O_CREAT, 0644, 1);
  }
  if ( sc != 0 )  {
    h->handle = 0;
  }
#elif defined(_WIN32)
  // Create a mutex with no initial owner.
  h->handle = ::CreateMutex(NULL,FALSE,mutex_name ? h->name : 0);
  if ( h->handle != 0 && ::GetLastError() == ERROR_ALREADY_EXISTS )   {
    ::CloseHandle(h->handle);
    h->handle = ::OpenMutex(MUTEX_ALL_ACCESS,FALSE,mutex_name ? h->name : 0);
  }
#endif
  if ( h->handle == NULL )   {
    return lib_rtl_signal_message(LIB_RTL_OS,"error in creating lock %s %08X.",h->name,h->handle);
    return 0;
  }
  *handle = h.release();
  return 1;
}

int lib_rtl_delete_lock(lib_rtl_lock_t handle)   {
  if ( handle )  {
    std::auto_ptr<rtl_lock> h(handle);
#if defined(USE_PTHREADS)
    int status;
    if ( h->name[0] )  {
      status = ::sem_close(h->handle);
      ::sem_unlink(h->name);
    }
    else {
      status = ::sem_destroy(h->handle);
    }
#elif defined(_WIN32)
    if ( ::CloseHandle(h->handle) != 0 ) 
#endif
    {
      return 1;
    }
    return lib_rtl_signal_message(LIB_RTL_OS,"error in deleting lock %s %08X.",h->name,h->handle);
  }
  return lib_rtl_signal_message(LIB_RTL_DEFAULT,"Error in deleting semaphore [INVALID MUTEX].");
}

int lib_rtl_cancel_lock(lib_rtl_lock_t h) {
  if ( h )  {
#if defined(USE_PTHREADS)
    int val;
    lib_rtl_lock_value(h, &val);
    if ( val == 0 ) {
      int status = ::sem_post(h->handle);
      h->held = 0;
      return status==0 ? 1 : 0;
    }
    else {
      ::printf("Cannot cancel lock [%s]. Semaphore count is:%d\n",h->name, val);
    }
    return 0;
#elif defined(_WIN32)
    return 1;
#endif
  }
  return 0;
}

int lib_rtl_lock(lib_rtl_lock_t h) {
  if ( h )  {
#if defined(USE_PTHREADS)
    int sc = ::sem_wait(h->handle);
    if ( sc != 0 ) {
      int val;
      lib_rtl_lock_value(h, &val);
      if ( val != 0 ) {
	printf("Lock: Bad lock count [%s]:%d Held:%d\n",h->name,val,h->held);
      }
    }
    if ( sc != 0 )
#elif defined(_WIN32)
    DWORD sc = WAIT_TIMEOUT;
    // while ( mutexHandle != 0 && sc != WAIT_OBJECT_0 )  {
    while ( h->handle != 0 && sc == WAIT_TIMEOUT )  {
      sc = ::WaitForSingleObject( h->handle, 1000 /*INFINITE*/ );
      if ( sc == WAIT_TIMEOUT )  {
        ::printf("Timeout on semaphore \"%s\"\n",h->name);
      }
      else if ( sc != WAIT_OBJECT_0 )  {
        lib_rtl_signal_message(LIB_RTL_OS,"Error locking semaphore [%s]: %08X",h->name,h->handle);
      }
    }
    if ( sc == WAIT_FAILED )    
#endif
    {
      return lib_rtl_signal_message(LIB_RTL_OS,"Error locking semaphore [%s]: %08X",h->name,h->handle);
    }
    h->held = 1;
    return 1;
  }
  return lib_rtl_signal_message(LIB_RTL_DEFAULT,"Error in locking semaphore [INVALID MUTEX].");
}

int lib_rtl_unlock(lib_rtl_lock_t h) {
  if ( h )  {
#if defined(USE_PTHREADS)
    int val;
    lib_rtl_lock_value(h, &val);
    if ( val == 0 ) {
      if ( ::sem_post(h->handle) == 0 )   {
	h->held = 0;
	return 1;
      }
      return lib_rtl_signal_message(LIB_RTL_OS,"Error in unlocking semaphore [%s] %08X Held:%d",
				    h->name,h->handle,h->held);
    }
    else {
      printf("Unlock: Bad lock count [%s]:%d\n",h->name,val);
    }
#elif defined(_WIN32)
    if ( ::ReleaseMutex(h->handle) )    {
      return 1;
    }
#endif
    return lib_rtl_signal_message(LIB_RTL_OS,"Error in unlocking semaphore [%s] %08X",h->name,h->handle);
  }
  return lib_rtl_signal_message(LIB_RTL_DEFAULT,"Error in unlocking semaphore [INVALID MUTEX].");
}

