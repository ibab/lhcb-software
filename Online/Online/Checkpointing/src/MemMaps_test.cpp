#include "Checkpointing/MemMaps.h"
#include "Checkpointing/MMap.h"
#include "Checkpointing.h"
#include "Restore.h"
#include <cerrno>
#include <cstring>

using namespace Checkpointing;

int test_MemMaps_write() {
  const char* file_name = "memmaps_test.dat";
  MemMaps m;
  MMap f;
  if ( f.create(file_name) ) {
    long len = m.write(f.fd());
    f.close();
    mtcp_output(MTCP_INFO,"Wrote %ld bytes mmaps data to %s.\n",len,file_name);
    return len;
  }
  mtcp_output(MTCP_ERROR,"Failed to estimate mmap size for %s: %s\n",file_name,strerror(errno));
  return 0;
}

int test_MemMaps_read() {
  const char* file_name = "memmaps_test.dat";
  MemMaps m;
  MMap f;
  if ( f.open(file_name) ) {
    void* mem = f.address();
    AreaMapper mapper;
    if ( m.read(mem,mapper) ) {
      f.close();
      return 1;
    }
    mtcp_output(MTCP_ERROR,"Failed to read memory maps from file %s.\n",file_name);
    f.close();
    return 0;
  }
  mtcp_output(MTCP_ERROR,"Failed to access file %s: %s\n",file_name,strerror(errno));
  return 0;
}

int test_MemMaps_sharable() {
  checkpointing_sys_aquire(&chkpt_sys);
  checkpointing_sys_print(&chkpt_sys);
  return 1;
}
