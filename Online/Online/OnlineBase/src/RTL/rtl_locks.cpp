#include "rtl_semaphore.h"
#include <map>
#include <memory>
#include <string>
#include <cstring>
#include <iostream>

using namespace RTL;

static std::auto_ptr<lib_rtl_lock_map_t> s_map(new lib_rtl_lock_map_t);
lib_rtl_lock_map_t& RTL::allLocks() {
  return *s_map.get();
}

extern "C" int lib_rtl_lock_exithandler() {
  if ( s_map.get() )  {
    lib_rtl_lock_map_t m = allLocks();
    lib_rtl_lock_map_t::iterator i = m.begin();
    for( ; i != m.end(); ++i ) {
      // ::lib_rtl_output(LIB_RTL_DEBUG,"Deleting lock: %s\n",(*i).first.c_str());
      ::lib_rtl_delete_lock((*i).second);
    }
    delete s_map.release();
  }
  return 1;
}

extern "C" int lib_rtl_lock_value(lib_rtl_lock_t handle, int* value)   {
  if ( handle ) {
#if defined(USE_PTHREADS)
    int sc = ::sem_getvalue(handle->handle, value);
    return sc==0 ? 1 : 0;
#elif defined(_WIN32)
    return *value=0;
#endif
  }
  return 0;
}

/// Create named lock. if the lock_name is 0 the lock is priovate to the currect process.
int lib_rtl_create_lock(const char* mutex_name, lib_rtl_lock_t* handle)   {
  std::auto_ptr<rtl_lock> h(new rtl_lock); 
  ::memset(h->name,0,sizeof(h->name));
  bool created = false;
  if ( mutex_name )  {
    ::snprintf(h->name,sizeof(h->name),"/%s",mutex_name);
    h->name[sizeof(h->name)-1] = 0;
  }
  h->held = 0;
#if defined(USE_PTHREADS)
  int sc = 1;
  h->handle = h->name[0] ? ::i_sem_open(h->name, O_CREAT|O_EXCL, 0666, 1) : &h->handle2;
  if (h->name[0] && !h->handle) {
    h->handle = ::i_sem_open(h->name, O_CREAT, 0666, 1);
    if ( !h->handle ) {
      ::lib_rtl_signal_message(LIB_RTL_OS,"sem_open: error in creating lock %s %08X.",h->name,h->handle);
      // ::perror("SEVERE: sem_open: ");
      return 0;
    }
    sc = 0;
  }
  else if ( h->name[0] ) {
    created = true;
    std::string nn="/dev/shm/sem.";
    nn += mutex_name;
    int ret = ::chmod(nn.c_str(),0666);
    if ( 0 != ret ) ::perror("SEVERE: chmod.");
    sc = 0;
  }
  else  {
    sc = ::sem_init(h->handle, h->name[0] ? 1 : 0, 1);
    if ( sc != 0 ) {
      sc = errno;
      ::sem_destroy(h->handle);
      h->handle = 0;
      errno = sc;
    }
  }
  if ( sc != 0 )  {
    h->handle = 0;
  }
#elif defined(_WIN32)
  // Create a mutex with no initial owner.
  h->handle = ::CreateMutex(0,FALSE,mutex_name ? h->name : 0);
  if ( h->handle != 0 && ::GetLastError() == ERROR_ALREADY_EXISTS )   {
    ::CloseHandle(h->handle);
    h->handle = ::OpenMutex(MUTEX_ALL_ACCESS,FALSE,mutex_name ? h->name : 0);
  }
#endif
  if ( h->handle == 0 )   {
    return ::lib_rtl_signal_message(LIB_RTL_OS,"error in creating lock %s %08X.",h->name,h->handle);
  }
  else if ( created && mutex_name != 0 )  {
    allLocks().insert(std::make_pair(h->name,h.get()));
  }
  *handle = h.release();
  return 1;
}

/// Create named event for inter process communication
#ifndef _WIN32
int lib_rtl_create_lock2 (sem_t* handle, lib_rtl_lock_t* lock_handle, bool initialize)    {
  std::auto_ptr<rtl_lock> h(new rtl_lock);
  ::memset(h->name,0,sizeof(h->name));
  h->handle = handle;
  if ( initialize ) {
    int sc = h->handle ? ::sem_init(h->handle, 1, 1) : (errno=EBADR); 
    if ( sc != 0 )  {
      h->handle = 0;
    }
    if ( h->handle == 0 )  {
      ::lib_rtl_signal_message(LIB_RTL_OS,"Failed to create lock");
      *lock_handle = 0;
      return 0;
    }
  }
  *lock_handle = h.release();
  return 1;
}

#endif

/// Delete lock
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
      if ( 0 == status ) {}
    }
#elif defined(_WIN32)
    if ( ::CloseHandle(h->handle) != 0 ) 
#endif
    {
      if ( h->name[0] ) {
        lib_rtl_lock_map_t& m = allLocks();
        lib_rtl_lock_map_t::iterator i = m.find(h->name);
        if ( i != m.end() ) {
          //::lib_rtl_output(LIB_RTL_DEBUG,"Deleting lock: %s\n",(*i).first.c_str());
          m.erase(i);
        }
      }
      return 1;
    }
#if defined(_WIN32)
    return ::lib_rtl_signal_message(LIB_RTL_OS,"error in deleting lock %s %08X.",h->name,h->handle);
#endif
  }
  return ::lib_rtl_signal_message(LIB_RTL_DEFAULT,"Cannot delete lock [INVALID HANDLE].");
}

/// Cancel lock
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
      ::lib_rtl_output(LIB_RTL_ERROR,"Cannot cancel lock [%s]. Semaphore count is:%d\n",h->name, val);
    }
    return 0;
#elif defined(_WIN32)
    return 1;
#endif
  }
  return ::lib_rtl_signal_message(LIB_RTL_DEFAULT,"Cannot cancel lock [INVALID HANDLE].");
}

/// Aquire lock
int lib_rtl_lock(lib_rtl_lock_t h) {
  if ( h )  {
#if defined(USE_PTHREADS)
    int sc = ::i_sem_wait(h->handle);
    if ( sc != 0 ) {
      int val;
      ::lib_rtl_lock_value(h, &val);
#if 0
      if ( val != 0 ) {
        ::lib_rtl_output(LIB_RTL_ERROR,"Lock: Bad lock count [%s]:%d Held:%d\n",h->name,val,h->held);
      }
#endif
    }
    if ( sc != 0 )
#elif defined(_WIN32)
      DWORD sc = WAIT_TIMEOUT;
    const char* opt = "";
    // while ( mutexHandle != 0 && sc != WAIT_OBJECT_0 )  {
    while ( h->handle != 0 && sc == WAIT_TIMEOUT )  {
      sc = ::WaitForSingleObject( h->handle, 1000 /*INFINITE*/ );
      switch(sc)  {
      case WAIT_FAILED:       opt="WAIT_FAILED";    break;
      case WAIT_ABANDONED:
        sc = ::WaitForSingleObject( h->handle, 1000 /*INFINITE*/ );
        opt="WAIT_ABANDONED"; break;
      case WAIT_TIMEOUT:      opt="WAIT_TIMEOUT";   break;
      case WAIT_OBJECT_0:     opt=0;                break;
      default:                                      break;
      }
      if ( opt )  {
        ::lib_rtl_signal_message(LIB_RTL_OS,"Error locking semaphore [%s]: %08X [%s]",
                                 h->name,h->handle,opt);
      }
    }
    if ( sc == WAIT_FAILED )    
#endif
    {
      return ::lib_rtl_signal_message(LIB_RTL_OS,"Error locking semaphore [%s]: %08X",
                                      h->name,h->handle);
    }
    h->held = 1;
    return 1;
  }
  return ::lib_rtl_signal_message(LIB_RTL_DEFAULT,"Error in locking semaphore [INVALID MUTEX].");
}

/// Try to aquire lock; returns immediately with return code 2 if unsuccessful.
int lib_rtl_trylock(lib_rtl_lock_t h) {
  if ( h )  {
#if defined(USE_PTHREADS)
    int sc = ::i_sem_trywait(h->handle);
    if ( sc == 0 ) return 1;
    else if ( errno == EAGAIN ) return 2;
#elif defined(_WIN32)
    DWORD sc = ::WaitForSingleObjectEx(h->handle,0,TRUE);
    if ( sc == WAIT_OBJECT_0 ) return 1;
    else if ( sc == WAIT_ABANDONED || sc == WAIT_TIMEOUT ) return 2;
#endif
    h->held = 1;
    return ::lib_rtl_signal_message(LIB_RTL_OS,"Error locking semaphore [%s]: %08X",
                                    h->name,h->handle);
  }
  ::lib_rtl_signal_message(LIB_RTL_DEFAULT,"Error in locking semaphore [INVALID MUTEX].");
  return 0;
}

/// Wait for lock flag with timeout given in milliseconds
int lib_rtl_lock_timedwait(lib_rtl_lock_t h, int milliseconds)    {
  if ( h )  {
#if defined(USE_PTHREADS)
    timespec sp;
    ::clock_gettime(CLOCK_REALTIME, &sp);    
    milliseconds += sp.tv_nsec/1000000;
    sp.tv_sec  += milliseconds/1000;
    sp.tv_nsec  = (milliseconds%1000)*1000000;
    int sc = milliseconds==LIB_RTL_INFINITE 
      ? ::i_sem_wait(h->handle) 
      : ::i_sem_timedwait(h->handle, &sp);
    if ( sc != 0 && errno == ETIMEDOUT ) 
      return 2;
    else if ( sc == 0 )
#elif defined(_WIN32)
      DWORD diff = (milliseconds>0) ? milliseconds : INFINITE;
    if ( ::WaitForSingleObjectEx(h->handle,diff, TRUE) == WAIT_OBJECT_0 )  
#endif
      return 1;
  }
  return 0;
}

/// Release lock
int lib_rtl_unlock(lib_rtl_lock_t h) {
  if ( h )  {
#if defined(USE_PTHREADS)
    int val = 0;
    //lib_rtl_lock_value(h, &val);
    if ( val == 0 ) {
      if ( ::sem_post(h->handle) == 0 )   {
        h->held = 0;
        return 1;
      }
      return ::lib_rtl_signal_message(LIB_RTL_OS,"Error in unlocking semaphore [%s] %08X Held:%d",
                                      h->name,h->handle,h->held);
    }
#if 0
    else {
      ::lib_rtl_output(LIB_RTL_ERROR,"Unlock: Bad lock count [%s]:%d\n",h->name,val);
    }
#endif
#elif defined(_WIN32)
    if ( ::ReleaseMutex(h->handle) )    {
      return 1;
    }
#endif
    return ::lib_rtl_signal_message(LIB_RTL_OS,"Error in unlocking semaphore [%s] %08X",h->name,h->handle);
  }
  return ::lib_rtl_signal_message(LIB_RTL_DEFAULT,"Error in unlocking semaphore [INVALID MUTEX].");
}

