#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#include "RTL/GlobalSection.h"

extern "C" int rtl_testGBL(int,char **) {
  int msecs = 3000;
  lib_rtl_lock_t id;
  int pid = lib_rtl_pid();
  char txt[132];
  const char* name = "Sem_test"; // argv[1];
  int status = RTL::Lock::create(name, id);
  if ( lib_rtl_is_success(status) )  {
    RTL::GlobalSection gbl("MyMemory",512, true);
    if ( gbl )  {
      char* buff = gbl.buffer();
      for( int i=0; i <100; ++i )  {
        ::lib_rtl_sleep(msecs);
        RTL::Lock lck(id);
        if ( i != 0 ) {
          ::printf("%4d[%06d] >> Read from shared memory: %s\n",i,pid,buff);
          if ( strcmp(buff,txt) != 0 )   {
            ::printf("Found changed shared memory.....\n");
          }
        }
        ::sprintf(txt,"%s (%05d):%3d",name,lib_rtl_pid(),i);
        ::strcpy(buff,txt);
        {
          ::printf("%4d[%06d] >> Wrote to shared memory:  %s\n",i,pid,buff);
        }
      }
    }
  }
  return 0;
}
