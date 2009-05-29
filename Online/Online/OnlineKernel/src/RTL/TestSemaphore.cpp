#include <cstdio>
#include "RTL/rtl.h"
#include "RTL/Lock.h"

extern "C" int rtl_testSemaphoreSubProcess(int argc,char** argv) {
  int msecs = 1000;
  int turns = 10;
  lib_rtl_lock_t id;
  std::string proc = argc>1 ? std::string(argv[1]) : RTL::processName();
  const char* name = "Sem_test";
  ::unlink("/dev/shm/sem.Sem_test");
  int status = RTL::Lock::create(name, id);
  if ( lib_rtl_is_success(status) )  {
    for( int i=0; i <turns; ++i )  {
      ::printf(  "%d >> Process %s waiting for lock %s....\n",i,proc.c_str(),name);
      ::fflush(stdout);
      {
        RTL::Lock lck(id);
        ::printf("%d >> Process %s holds       lock %s for   %d milliseconds....\n",i,proc.c_str(),name,msecs);
	::fflush(stdout);
        lib_rtl_sleep(msecs);
        ::printf("%d >> Process %s released    lock %s after %d milliseconds....\n",i,proc.c_str(),name,msecs);
	::fflush(stdout);
      }
      lib_rtl_sleep(msecs);
    }
  }
  return 0;
}

extern "C" int rtl_testsemaphore(int,char**) {
  system("gentest.exe libOnlineKernel.so rtl_testSemaphoreSubProcess  Process__1&");
  lib_rtl_sleep(700);
  return system("gentest.exe libOnlineKernel.so rtl_testSemaphoreSubProcess Process__2");
}
