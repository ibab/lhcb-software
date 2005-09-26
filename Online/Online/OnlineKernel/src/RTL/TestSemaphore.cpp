#include <cstdio>
#include "RTL/rtl.h"
#include "RTL/Lock.h"

extern "C" int rtl_testSemaphore(int,char**) {
  int msecs = 3000;
  lib_rtl_lock_t id;
  const char* name = "Sem_test";
  int status = RTL::Lock::create(name, id);
  if ( lib_rtl_is_success(status) )  {
    for( int i=0; i <100; ++i )  {
      ::printf("%d >> Process %d waiting for lock %s....\n",i,lib_rtl_pid(),name);
      RTL::Lock lck(name, id);
      ::printf("%d >> Process %d holds lock %s for %d milliseconds....\n",i,lib_rtl_pid(),name,msecs);
      lib_rtl_sleep(msecs);
    }
  }
  return 0;
}
