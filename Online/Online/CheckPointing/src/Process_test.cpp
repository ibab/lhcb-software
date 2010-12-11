#include "CheckPointing/MMap.h"
#include "CheckPointing/Process.h"
#include "CheckPointing.h"

#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

using namespace CheckPointing;

int test_Process_write() {
  // We assume, that at this stage
  const char* file_name = "proc.dat";
  long len;
  MMap m;
  Process p(Process::PROCESS_WRITE);

  int fd1=::open("/home/frankm/.bashrc",O_RDONLY);
  int fd2=::open("/home/frankm/.bashrc",O_RDONLY|O_APPEND);

  mtcp_set_debug_level(0);
  if ( (len=p.length()) > 0 ) {
    p.setLength(len);
    mtcp_output(MTCP_INFO,"The current process scan requires: %ld bytes.\n",len);
    if ( m.create(file_name,len) ) {
      void* mem = m.address();
      long true_len = p.write(mem);
      mtcp_output(MTCP_INFO,"Wrote %ld bytes of %ld space.\n",true_len,len);
      m.commit(true_len);
      ::close(fd1);
      ::close(fd2);
      return true_len;
    }
  }
  mtcp_output(MTCP_ERROR,"Process size scan failed:Got %ld bytes\n",len);
  ::close(fd1);
  ::close(fd2);
  return 0;
}

int test_Process_read() {
  // We assume, that at this stage
  const char* file_name = "proc.dat";
  MMap m;
  mtcp_set_debug_level(0);
  if ( m.open(file_name) ) {
    Process p;
    void* mem = m.address();
    if ( p.read(mem) ) {
      m.close();
      return 1;
    }
    mtcp_output(MTCP_ERROR,"Failed to read process information from file %s.\n",file_name);
    return 0;
  }
  mtcp_output(MTCP_ERROR,"Failed to access file %s: %s\n",file_name,strerror(errno));
  return 0;
}

int test_Process_restore() {
  // We assume, that at this stage
  const char* file_name = "proc.dat";
  MMap m;
  mtcp_set_debug_level(0);
  if ( m.open(file_name) ) {
    Process p(Process::PROCESS_RESTORE);
    void* mem = m.address();
    if ( p.read(mem) ) {
      m.close();
      return 1;
    }
    mtcp_output(MTCP_ERROR,"Failed to read process information from file %s.\n",file_name);
    return 0;
  }
  mtcp_output(MTCP_ERROR,"Failed to access file %s: %s\n",file_name,strerror(errno));
  return 0;
}
