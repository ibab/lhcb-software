#include "rtl_internal.h"
#include <memory>
#include <fcntl.h>

int lib_rtl_create_lock(const char* mutex_name, lib_rtl_lock_t* handle)   {
  std::auto_ptr<rtl_lock> h(new rtl_lock); 
  ::memset(h->name,0,sizeof(h->name));
  if ( mutex_name )  {
    ::sprintf(h->name,"/%s",mutex_name);
    h->name[sizeof(h->name)-1] = 0;
  }
#ifdef VMS
  int iosb[2];
  char mutexName[128] = "";
  TABNAM_LIST name[2];
  $DESCRIPTOR(ltab,"LNM$SYSTEM");
  $DESCRIPTOR(lnam,"SYS$NODE");
  $DESCRIPTOR(lonam,mutexName);
  char eqname[32];
  int eqlen = 0;
  name[0].len     = sizeof(eqname);
  name[0].code    = LNM$_STRING;
  name[0].addr    = eqname;
  name[0].retaddr = &eqlen;
  name[1].len     = 0;
  name[1].code    = 0;
  int status = sys$trnlnm (0, &ltab, &lnam,0, &name) ;
  eqname[eqlen]=0;
  strcpy(mutexName, eqname);
  strcat(mutexName,mutex_name);
  DESCRIPTOR(lonam,mutexName);
  str$upcase(&lonam,&lonam);
  status  = sys$enqw (0, LCK$K_NLMODE, iosb, LCK$M_SYSTEM+LCK$M_NODLCKWT,&lonam,0,0,0,0,3,0);
  if (lib_rtl_is_success(status))    {
    status = iosb[0];
    if (iosb[0] & 1)      {
      *handle = lib_rtl_handle_t(iosb[1]);
    }
    else      {
      ::printf("error in creating lock:%s. IOSB %d\n",mutex_name,status);
    }
  }
  else    {
    ::printf("error in creating lock:%s. Status %d\n",mutex_name,status);
  }
  return status;
#elif defined(USE_PTHREADS)
  h->handle = h->name[0] ? ::sem_open(h->name, O_CREAT, 0644, 1) : &h->handle2;
  int sc = ::sem_init(h->handle, h->name[0] ? 0 : 1, 1);     
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
#ifdef VMS
    int status = sys$deq(h->handle,0,3,0) ;
    if (!(lib_rtl_is_success(status)))  {
      ::printf("error in deleting lock %s. Status %d\n",mutex_name,status);
    }
    kutil_enable_kill();
    return status;
#elif defined(USE_PTHREADS)
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
#ifdef VMS
    int status = sys$deq (h,0,3,LCK$M_CANCEL) ;
    if (!lib_rtl_is_success(status))  {
      ::printf("error in cancelling lock %s. Status %d\n",h->name, status);
    }
    kutil_enable_kill();
    return status;
#elif defined(USE_PTHREADS)
    int status = ::sem_post(h->handle);
    return status==0 ? 1 : 0;
#elif defined(_WIN32)
    return 1;
#endif
  }
  return 0;
}

int lib_rtl_lock(lib_rtl_lock_t h) {
  if ( h )  {
#ifdef VMS
    int iosb[2] = {0,h};
    int status = sys$enqw(0,LCK$K_EXMODE,iosb,LCK$M_CONVERT+LCK$M_NODLCKWT+LCK$M_SYNCSTS,0,0,0,0,0,3,0);
    if (lib_rtl_is_success(status))  {
      status = iosb[0];
      if (lib_rtl_is_success(status))    {
        kutil_disable_kill();
        /*	    kutil_boost_prio(lock_prio, &def_prio);*/
        return 1;
      }
      else    {
        ::printf("error in locking tables. IOSB %d\n",status);
      }
    }
    else  {
      ::printf("error in locking tables. Status %d\n",status);
    }
    return status;
#elif defined(USE_PTHREADS)
    int sc = ::sem_wait(h->handle);
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
    return 1;
  }
  return lib_rtl_signal_message(LIB_RTL_DEFAULT,"Error in locking semaphore [INVALID MUTEX].");
}

int lib_rtl_unlock(lib_rtl_lock_t h) {
  if ( h )  {
#ifdef VMS
    int iosb[2] = {0,h};
    int status = sys$enqw (0,LCK$K_NLMODE,iosb,LCK$M_CONVERT+LCK$M_NODLCKWT+LCK$M_SYNCSTS,0,0,0,0,0,3,0);
    if (lib_rtl_is_success(status))  {
      status = iosb[0];
      if (lib_rtl_is_success(status))    {
        kutil_enable_kill();
        /*	    kutil_boost_prio(def_prio, &def_prio);*/
        return 1;
      }
    }
    errno = status;
    kutil_enable_kill();
#elif defined(USE_PTHREADS)
    if ( ::sem_post(h->handle) == 0 )   {
      return 1;
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

