#include <cstdio>
#include <fcntl.h>
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#include "RTL/GlobalSection.h"

extern "C" int rtl_testGBL(int,char **) {
  int msecs = 3000;
  lib_rtl_lock_t id;
  const char* name = "Sem_test"; // argv[1];
  int status = RTL::Lock::create(name, id);
  if ( lib_rtl_is_success(status) )  {
    RTL::GlobalSection gbl("MyMemory",512);
    if ( gbl )  {
      char* buff = gbl.buffer();
      for( int i=0; i <100; ++i )  {
        ::lib_rtl_sleep(msecs);
        RTL::Lock lck(name, id);
        if ( i != 0 ) ::printf("%d >> Read from shared memory: %s\n",i,buff);
        ::sprintf(buff,"%s (%d):%d",name,lib_rtl_pid(),i);
        ::printf("%d >> Wrote to shared memory: %s\n",i,buff);
      }
    }
  }
  return 0;
}
