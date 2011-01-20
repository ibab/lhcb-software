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

static bool s_stop = false;

static void* main_thread(void * /* arg */)  {
  int cnt = 0;
  mtcp_output(MTCP_INFO,"init: MAIN \n");
  while( !s_stop )  {
    struct timeval tv;
    ::fprintf(stdout,"Chr[%d]...",++cnt);
    ::fflush(stdout);
    tv.tv_sec = 4000;
    tv.tv_usec = 0;
    int ret=::select(0,0,0,0,&tv);
    ::fprintf(stdout,"...Child out of select:%d errno:%d [%s]\n",ret,errno,strerror(errno));
  }
  ::fprintf(stdout,"...Child thread exiting...\n");  
  ::fflush(stdout);
  return 0;
}

static long make_checkPoint() {
  // We assume, that at this stage
  const char* file_name = "proc.dat";
  MainThread& m=MainThread::accessInstance();
  MMap f;
  mtcp_set_debug_level(0);
  if ( f.create(file_name) ) {
    m.stop();
    int ret = m.checkpoint(f.fd());
    int typ =  m.restartType();
    if ( typ == 1 )   {
      ::fprintf(stdout,"\n...stop threads after restore from file:%s\n",file_name);
      ::sleep(3);
      m.stop();
      ::fprintf(stdout,"... checkpoint successfully restored ...\n");
      return 1;
    }
    ::fprintf(stdout,"...resume after writing checkpoint file:%s rc=%d\n",file_name,ret);
    return 2;
  }
  mtcp_output(MTCP_ERROR,"Process size scan failed.\n");
  return 0;
}

int test_thread_checkpoint() {
  MainThread& m=MainThread::accessInstance();
  static pthread_t main_pid;
  int rc;

  m.initialize();
  m.setPrint(0);

  if ((rc=::pthread_create (&main_pid, NULL, main_thread, (void*)5)) < 0) {
    mtcp_output(MTCP_FATAL,"Error CREATE main thread: %s rc=%d\n",::strerror(errno),rc);
  }
  int count = 0;
  while(1)  {
    ::sleep(2);
    ++count;
    if ( count == 2 ) {
      rc = make_checkPoint();
      ::fprintf(stdout,"...restoring main thread. rc=%d...\n",rc);
      m.resume();
    }
    if ( count == 4 ) {
      if ( (rc=::pthread_cancel(main_pid)) < 0 ) {     
	mtcp_output(MTCP_FATAL,"Error CANCEL main thread: %s rc=%d\n",::strerror(errno),rc);
      }
      ::fprintf(stdout,"\n...main thread cancelled ...\n");
      count = 6;
    }
    if ( count == 6 ) {
      void* val = 0;
      ::fprintf(stdout,"...wait for Child thread to exit...\n");
      ::fflush(stdout);
      s_stop = true;
      if ( (rc=::pthread_join(main_pid,&val)) < 0 ) {
	mtcp_output(MTCP_FATAL,"Error JOIN main thread: %s rc=%d\n",::strerror(errno),rc);
      }
      ::fprintf(stdout,"...Child ended...exit...\n");
      ::fflush(stdout);
      _exit(0);
    }
  }
  return 0;
}



int test_FileMap_scan();
int test_FileMap_memory();

int test_MemMaps_read();
int test_MemMaps_write();
int test_MemMaps_sharable();

int test_Process_write();
int test_Process_read();
int test_Process_restore();

int test_thread_checkpoint();

extern "C" int chkpt_tests(int argc, char** argv) {
  int opt = *(int*)"None";
  if ( argc>1        ) opt = *(int*)argv[1];

  if      ( opt == *(int*)"m_write"   ) test_MemMaps_write();
  else if ( opt == *(int*)"m_read"    ) test_MemMaps_read();
  else if ( opt == *(int*)"m_share"   ) test_MemMaps_sharable();

  else if ( opt == *(int*)"f_scan"    ) test_FileMap_scan();
  else if ( opt == *(int*)"f_memory"  ) test_FileMap_memory();

  else if ( opt == *(int*)"p_write"   ) test_Process_write();
  else if ( opt == *(int*)"p_read"    ) test_Process_read();
  else if ( opt == *(int*)"p_restore" ) test_Process_restore();

  else if ( opt == *(int*)"t_checkpo" ) test_thread_checkpoint();

  return 1;
}
