#include <fcntl.h>
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#include "RTL/strdef.h"
#include "RTL/GlobalSection.h"

extern "C" int rtl_test_gbl(int argc, char** argv) {
  int msecs = 300;
  lib_rtl_lock_t id;
  char txt[132];
  int turns = 20;
  std::string proc = argc>1 ? std::string(argv[1]) : RTL::processName();
  ::snprintf(txt,sizeof(txt),"GBL_test_0x%08X",lib_rtl_pid());
  std::string name = txt;
  ::lib_rtl_unlink(("/dev/shm/sem."+name).c_str());
  ::lib_rtl_unlink(("/dev/shm/"+name).c_str());
  int status = RTL::Lock::create(name.c_str(), id);
  if ( lib_rtl_is_success(status) )  {
    RTL::GlobalSection gbl(name,512, true);
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
        ::snprintf(txt,sizeof(txt),"%s (%s):%3d",name.c_str(),proc.c_str(),i);
        ::strncpy(buff,txt,512);
        {
          ::printf("%4d[%s] >> Wrote to shared memory:  %s\n",i,proc.c_str(),buff);
        }
      }
    }
  }
  return 0;
}
