#include "CheckPointing/MMap.h"
#include "CheckPointing/Static.h"
#include "CheckPointing/Process.h"
#include "CheckPointing/MainThread.h"
#include "CheckPointing.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

using namespace CheckPointing;



static void* main_thread(void * /* arg */)  {
  mtcp_output(MTCP_INFO,"mtcp_init: MAIN \n");
  while(1)  {
    ::fprintf(stdout,"Chr...");
    ::fflush(stdout);
    ::sleep(2);
  }
  return 0;
}

static long make_checkPoint() {
  // We assume, that at this stage
  const char* file_name = "proc.dat";
  long len;
  MMap m;
  Process p(Process::PROCESS_WRITE);

  mtcp_set_debug_level(0);
  if ( (len=p.length()) > 0 ) {
    p.setLength(len);
    mtcp_output(MTCP_INFO,"The current process scan requires: %ld bytes.\n",len);
    if ( m.create(file_name,len) ) {
      long true_len = p.write(m.address());
      mtcp_output(MTCP_INFO,"Wrote %ld bytes of %ld space.\n",true_len,len);
      m.commit(true_len);
      return true_len;
    }
  }
  mtcp_output(MTCP_ERROR,"Process size scan failed:Got %ld bytes\n",len);
  return 0;
}

int test_thread_checkpoint() {
  MainThread& m=MainThread::accessInstance();
  m.initialize();

  mtcp_set_debug_level(0);
  static pthread_t main_pid;
  if (pthread_create (&main_pid, NULL, main_thread, (void*)5) < 0) {
    mtcp_output(MTCP_FATAL,"mtcp_init: error creating main thread: %s\n",::strerror(errno));
  }
  int count = 0;
  while(1)  {
    ::sleep(5);
    ++count;
    if ( count == 2 ) {
      make_checkPoint();
      _exit(0);
    }
  }
  return 0;
}

int test_thread_restore() {
  // We assume, that at this stage
  const char* file_name = "proc.dat";
  MMap m;
  mtcp_set_debug_level(0);
  if ( m.open(file_name) ) {
    Process p(Process::PROCESS_RESTORE);
    void* mem = m.address();
    if ( p.read(mem) ) {
      return 1;
    }
    mtcp_output(MTCP_ERROR,"Failed to read process information from file %s.\n",file_name);
    return 0;
  }
  mtcp_output(MTCP_ERROR,"Failed to access file %s: %s\n",file_name,strerror(errno));
  return 0;
}
