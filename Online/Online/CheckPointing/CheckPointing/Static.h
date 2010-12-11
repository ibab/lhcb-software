#include <sys/resource.h>

#define HIGHEST_VA 0x7f00000000
namespace CheckPointing {
  typedef unsigned long VA; /* VA = virtual address */
  struct SysInfo {
    struct rlimit stack;
    VA            addrStart;
    VA            addrEnd;
    unsigned long addrSize;
    void*         curr_break;
    void*         next_break;
    void*         saved_break;
    void        (*startRestore)();
    void        (*finishRestore)();
    void*         sysInfo;
    unsigned long pageSize;
    int           checkpointFD;
    char          checkpointFile[4096];
    SysInfo();
    void print();
    void aquire();
    long length();
    long write(void* addr);
    long read(const void* addr);
    long restore(const void* addr);
  };
}

#ifndef CHECKPOINTING_STATIC_CPP
namespace CheckPointing {  extern SysInfo chkpt_sys;   }
#endif
