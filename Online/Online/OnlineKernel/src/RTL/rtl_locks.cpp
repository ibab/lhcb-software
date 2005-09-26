#include "rtl_internal.h"
#include <cstdio>
#include <fcntl.h>

int lib_rtl_create_lock(const char* mutex_name, lib_rtl_lock_t* handle)   {
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
  int sc = 0;
  int shared = 0;
  if ( mutex_name )  {
    *handle = ::sem_open(mutex_name, O_CREAT, 0644, 1);
    shared = 1;
  }
  else  {
    *handle = new sem_t;
  }
  sc = ::sem_init(*handle, shared, 1);     
  if ( sc != 0 )  {
    delete *handle;
    *handle = 0;
  }
#elif defined(_WIN32)
  // Create a mutex with no initial owner.
  *handle = ::CreateMutex(NULL,FALSE,mutex_name);
  if ( *handle != 0 && ::GetLastError() == ERROR_ALREADY_EXISTS )   {
    ::CloseHandle(*handle);
    *handle = ::OpenMutex(MUTEX_ALL_ACCESS,FALSE,mutex_name);
  }
#endif
  if ( *handle == NULL )   {
    int status = getError();
    ::printf("error in creating lock. Status %d [%s]\n",status, errorString(status));
    return 0;
  }
  return 1;
}

int lib_rtl_delete_lock(const char* mutex_name, lib_rtl_lock_t handle)   {
  if ( handle )  {
#ifdef VMS
    int status = sys$deq(handle,0,3,0) ;
    if (!(lib_rtl_is_success(status)))  {
      ::printf("error in deleting lock %s. Status %d\n",mutex_name,status);
    }
    kutil_enable_kill();
    return status;
#elif defined(USE_PTHREADS)
    int status;
    if ( mutex_name )  {
      status = ::sem_close(handle);
      ::sem_unlink(mutex_name);
    }
    else {
      status = ::sem_destroy(handle);
      delete handle;
    }
#elif defined(_WIN32)
    if ( ::CloseHandle(handle) == S_OK ) 
#endif
    {
      return 1;
    }
    int err = getError();
    ::printf("error in deleting lock %s. Status %d %s\n",mutex_name,err,errorString(err));
    return 0;
  }
  ::printf("error in deleting lock %s [Invalid Mutex].\n",mutex_name);
  return 0;
}

int lib_rtl_cancel_lock(const char* lock_name, lib_rtl_lock_t lock_handle) {
#ifdef VMS
  int status = sys$deq (lock_handle,0,3,LCK$M_CANCEL) ;
  if (!lib_rtl_is_success(status))  {
    ::printf("error in cancelling lock %s. Status %d\n",lock_name, status);
  }
  kutil_enable_kill();
  return status;
#elif defined(USE_PTHREADS)
  if ( lock_name ) {}
  int status = ::sem_post(lock_handle);
  return status==0 ? 1 : 0;
#elif defined(_WIN32)
  return 1;
#endif
}

int lib_rtl_lock(const char* lock_name, lib_rtl_lock_t lock_handle) {
  if ( lock_handle )  {
#ifdef VMS
    int iosb[2] = {0,lock_handle};
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
    if ( lock_name ) {}
    int sc = sem_wait(lock_handle);
    if ( sc != 0 )
#elif defined(_WIN32)
    DWORD sc = WAIT_TIMEOUT;
    // while ( mutexHandle != 0 && sc != WAIT_OBJECT_0 )  {
    while ( lock_handle != 0 && sc == WAIT_TIMEOUT )  {
      sc = ::WaitForSingleObject( lock_handle, 1000 /*INFINITE*/ );
      if ( sc == WAIT_TIMEOUT )  {
        ::printf("Timeout on semaphore \"%s\"\n",lock_name);
      }
      else if ( sc != WAIT_OBJECT_0 )  {
        ::printf("Error on semaphore \"%s\" %d, Status=%d Error=%d Msg:%s\n", 
          lock_name, lock_handle, sc, ::GetLastError(), errorString());
      }
    }
    if ( sc == WAIT_FAILED )    
#endif
    {
      int err = getError();
      ::printf("Error in locking %s tables. Status=%d Error=%d Msg:%s\n", 
          lock_name, sc, err, errorString(err));
      return 0;
    }
    return 1;
  }
  ::printf("error in locking tables [Invalid Mutex].\n");
  return 0;
}

int lib_rtl_unlock(lib_rtl_lock_t lock_handle) {
  if ( lock_handle )  {
#ifdef VMS
    int iosb[2] = {0,lock_handle};
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
    if ( ::sem_post(lock_handle) == 0 )   {
      return 1;
    }
#elif defined(_WIN32)
    if ( ::ReleaseMutex(lock_handle) )    {
      return 1;
    }
#endif
    int err = getError();
    ::printf("error in unlocking tables. Status %d [%s]\n",err,errorString(err));
    return 0;
  }
  ::printf("error in unlocking tables [Invalid Mutex].\n");
  return 0;
}

