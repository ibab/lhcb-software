#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#include "RTL/GlobalSection.h"

extern "C" int rtl_test_gbl(int argc, char** argv) {
  int msecs = 300;
  lib_rtl_lock_t id;
  int pid = lib_rtl_pid();
  char txt[132];
  int turns = 20;
  const char* name = "Sem_test"; // argv[1];
  std::string proc = argc>1 ? std::string(argv[1]) : RTL::processName();
  ::unlink("/dev/shm/sem.Sem_test");
  ::unlink("/dev/shm/MyMemory");
  int status = RTL::Lock::create(name, id);
  if ( lib_rtl_is_success(status) )  {
    RTL::GlobalSection gbl("MyMemory",512, true);
    if ( gbl )  {
      char* buff = gbl.buffer();
      for( int i=0; i <turns; ++i )  {
        ::lib_rtl_sleep(msecs);
        RTL::Lock lck(id);
        if ( i != 0 )   {
          ::printf("%4d[%s] >> Read from shared memory: %s\n",i,proc.c_str(),buff);
          if ( strcmp(buff,txt) != 0 )   {
            ::printf("Found changed shared memory.....\n");
          }
        }
        ::sprintf(txt,"%s (%s):%3d",name,proc.c_str(),i);
        ::strcpy(buff,txt);
        {
          ::printf("%4d[%s] >> Wrote to shared memory:  %s\n",i,proc.c_str(),buff);
        }
      }
    }
  }
  return 0;
}
