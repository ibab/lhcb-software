#include <cstdio>
#include <cstring>
#include "RTL/rtl.h"

/// Thread execution call signature
static int thread_routine(void* param)    {
  ::printf("Thread started. Parameter=%p\n", param);
  lib_rtl_sleep(2000);
  ::printf("Thread exiting. \n");
  return 1;
}

extern "C" int rtl_test_thread(int /* argc */, char ** /* argv */) {
  lib_rtl_thread_t handle;
  void* param = reinterpret_cast<void*>(0xFEEDBABEL);
  int sc = ::lib_rtl_start_thread(thread_routine,param, &handle);
  if ( lib_rtl_is_success(sc) )  {
    ::printf("Starting new thread with param:%p\n", param);
    sc = ::lib_rtl_join_thread(handle);
    if ( !lib_rtl_is_success(sc) )  {
      ::printf("Thread CANNOT be joined.\n");
      return 0;
    }
    ::printf("Thread joined successfully.\n");
  }
  return 0;
}
