#include <cstdio>
#include "RTL/rtl.h"
#include "RTL/Lock.h"

extern "C" int rtl_testSemaphoreSubProcess(int argc,char** argv) {
  int msecs = 2000;
  int turns = 5;
  lib_rtl_lock_t id;
  std::string proc = argc>1 ? std::string(argv[1]) : RTL::processName();
  std::string name = argc>2 ? std::string(argv[2]) : RTL::processName();
  ::unlink(("/dev/shm/sem."+name).c_str());
  int status = RTL::Lock::create(name.c_str(), id);
  if ( lib_rtl_is_success(status) )  {
    for( int i=0; i <turns; ++i )  {
      ::printf(  "%d >> Process %s waiting for lock %s....\n",i,proc.c_str(),name.c_str());
      ::fflush(stdout);
      {
        RTL::Lock lck(id);
        ::printf("%d >> Process %s holds       lock %s for   %d milliseconds....\n",
		 i,proc.c_str(),name.c_str(),msecs);
	::fflush(stdout);
        lib_rtl_sleep(msecs);
        ::printf("%d >> Process %s released    lock %s after %d milliseconds....\n",
		 i,proc.c_str(),name.c_str(),msecs);
	::fflush(stdout);
      }
      lib_rtl_sleep(msecs);
    }
  }
  return 0;
}

extern "C" int rtl_testsemaphore(int,char**) {
  char text[256];
  ::sprintf(text,"gentest.exe libOnlineKernel.so %s  Process__%d Sem_test_0x%08X&",
	    "rtl_testSemaphoreSubProcess",1,::lib_rtl_pid());
  system(text);
  lib_rtl_sleep(1500);
  ::sprintf(text,"gentest.exe libOnlineKernel.so %s  Process__%d Sem_test_0x%08X&",
	    "rtl_testSemaphoreSubProcess",2,::lib_rtl_pid());
  int ret = system(text);
  lib_rtl_sleep(1500);
  return ret;
}
