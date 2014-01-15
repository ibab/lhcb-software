#include "Checkpointing/MMap.h"
#include "Checkpointing/Process.h"
#include "Checkpointing.h"

#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

using namespace Checkpointing;

int test_Process_write() {
  // We assume, that at this stage
  const char* file_name = "process_test.dat";
  MMap m;
  Process p;

  int fd1 = ::open("/home/frankm/.bashrc",O_RDONLY);
  int fd2 = ::open("/home/frankm/.bashrc",O_RDONLY|O_APPEND);
  if ( fd1 != -1 && fd2 != -1 )  {
    mtcp_output(MTCP_INFO,"Start process scan.\n");
    if ( m.create(file_name) ) {
      long true_len = p.write(m.fd());
      mtcp_output(MTCP_INFO,"Wrote %ld bytes.\n",true_len);
      ::close(fd1);
      ::close(fd2);
      return true_len;
    }
    mtcp_output(MTCP_ERROR,"Process size scan failed. Could not create output file.\n");
    ::close(fd1);
    ::close(fd2);
  }
  return 0;
}

int test_Process_read() {
  // We assume, that at this stage
  const char* file_name = "process_test.dat";
  MMap m;
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
  const char* file_name = "process_test.dat";
  MMap m;
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
