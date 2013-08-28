#include "Checkpointing/FileMap.h"
#include "Checkpointing/SysCalls.h"
#include "Checkpointing.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <dirent.h>

using namespace Checkpointing;

/// Count the number of files in the proc file system known to this process
WEAK(int) FileMap::count()    {
  FileCountHandler h;
  int rc = scan(h);
  if ( rc > 0 ) return h.count();
  return -1;
}

/// Count the necessary amount of contiguous memory to save all data 
WEAK(long) FileMap::memoryCount()    {
  MemCountFileHandler h;
  int rc = scan(h);
  if ( rc > 0 ) return h.count();
  return -1;
}

WEAK(int) FileMap::scan(const FileHandler& handler)  const   {
  // Open /proc/self/fd directory - it contains a list of files I have open
  int dsiz = -1, fddir = mtcp_sys_open("/proc/self/fd",O_RDONLY,0);
  if (fddir >= 0) {
    char* p;
    int count = 0;
    char dbuf[BUFSIZ];
    struct dirent *dent;
    while (1) {        // Check each entry
      dsiz = -1;
      if (sizeof(dent->d_ino) == 4) dsiz = mtcp_sys_getdents  (fddir,dbuf,sizeof(dbuf));
      if (sizeof(dent->d_ino) == 8) dsiz = mtcp_sys_getdents64(fddir,dbuf,sizeof(dbuf));
      if (dsiz <= 0) break;
      for (int doff = 0; doff < dsiz; doff += dent->d_reclen) {
        dent = (dirent*)(dbuf + doff);
        // The filename should just be a decimal number = the fd it represents
        // Also, skip the entry for the checkpoint and directory files as we 
        // don't want the restore to know about them
        int fdnum = ::strtol(dent->d_name, &p, 10);
        if ( *p == 0 && fdnum > 2 && fdnum != fddir ) {
          ++count;
          handler.handle(fdnum);
        }
      }
    }
    mtcp_sys_close(fddir);
    if (dsiz < 0) {
      mtcp_output(MTCP_ERROR,"FileMap: error reading /proc/self/fd: %s\n",strerror(mtcp_sys_errno));
      return -1;
    }
    mtcp_output(MTCP_DEBUG,"FileMap: Scanned %d entries.\n",count);
    return count;
  }
  mtcp_output(MTCP_ERROR,"FileMap: error opening directory /proc/self/fd: %s\n",strerror(errno));
  return -1;
}

WEAK(int) FileMap::analyze(const FileHandler& handler) {
  long rc = 0, cnt=0;
  do {
    rc = handler.handle(-1);
    if ( rc ) ++cnt;
  } while (rc > 0);
  return cnt;
}
