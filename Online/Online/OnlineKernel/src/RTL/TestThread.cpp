#include <cstdio>
#include <cstring>
#include "RTL/rtl.h"

/// Thread execution call signature
static int thread_routine(void* param)    {
  ::fprintf(stdout,"Thread started. Parameter=%p\n", param);
  ::fflush(stdout);
  ::lib_rtl_sleep(2000);
  ::fprintf(stdout,"Thread exiting. \n");
  ::fflush(stdout);
  return 1;
}

extern "C" int rtl_test_thread(int /* argc */, char ** /* argv */) {
  lib_rtl_thread_t handle;
  void* param = reinterpret_cast<void*>(0xFEEDBABEL);
  int sc = ::lib_rtl_start_thread(thread_routine,param, &handle);
  if ( lib_rtl_is_success(sc) )  {
    ::lib_rtl_sleep(2000);
    ::fprintf(stdout,"Started new thread with param:%p\n", param);
    ::fflush(stdout);
    sc = ::lib_rtl_join_thread(handle);
    if ( !lib_rtl_is_success(sc) )  {
      ::fprintf(stdout,"Thread CANNOT be joined.\n");
      ::fflush(stdout);
      return 0;
    }
    ::fprintf(stdout,"Thread joined successfully.\n");
    ::fflush(stdout);
  }
  return 0;
}
