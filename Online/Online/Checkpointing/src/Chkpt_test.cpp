#include "Checkpointing/MMap.h"
#include "Checkpointing/Static.h"
#include "Checkpointing/Process.h"
#include "Checkpointing/Chkpt.h"
#include "Checkpointing.h"
#include "Restore.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

using namespace Checkpointing;

static bool s_stop = false;

static void* main_thread(void * /* arg */)  {
  int cnt = 0;
  mtcp_output(MTCP_INFO,"init: MAIN SIGRTMIN=%d SIGRTMAX=%d\n",SIGRTMIN,SIGRTMAX);
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
  MMap f;
  if ( f.create(file_name) ) {
    checkpointing_stop_process();
    int ret = checkpointing_write_checkpoint(f.fd());
    int typ = checkpointing_restart_type();
    if ( typ == 1 )   {
      ::fprintf(stdout,"\n...stop threads after restore from file:%s\n",file_name);
      ::sleep(3);
      checkpointing_stop_process();
      ::fprintf(stdout,"... checkpoint successfully restored ...\n");
      return 1;
    }
    ::fprintf(stdout,"...resume after writing checkpoint file:%s with 0x%08X bytes\n",file_name,ret);
    return 2;
  }
  mtcp_output(MTCP_ERROR,"Process size scan failed.\n");
  return 0;
}

int test_thread_checkpoint(int flag) {
  static pthread_t main_pid;
  int rc;

  checkpointing_set_save_flags(flag);
  if ((rc=::pthread_create (&main_pid, NULL, main_thread, (void*)5)) < 0) {
    mtcp_output(MTCP_FATAL,"Error CREATE main thread: %s rc=%d\n",::strerror(errno),rc);
  }
  int count = 0;
  bool run = true;
  while(run)  {
    ::sleep(2);
    ++count;
    if ( count == 2 ) {
      rc = make_checkPoint();
      ::fprintf(stdout,"...restoring main thread. rc=%d...\n",rc);
      checkpointing_resume_process();
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
      int rc;
      FILE* fd = ::fopen("/proc/self/maps","r");
      char* buff = 0, *p, *ptr;
      size_t len=0;
      while( (rc=::getline(&buff,&len,fd)) > 0 ) {
	ptr = buff;
	if ( 0 == (ptr=strstr(ptr,"/tmp/")) ) {
	  for(p=ptr=strchr(buff,'/'); p; ptr=p+1, p=strchr(ptr,'/'));
	}
	// No printout. This confuses QMTest completely
	// if ( ptr ) mtcp_output(MTCP_ALWAYS,"Linked image: %s",ptr);
      }
      if ( buff ) ::free(buff);
      ::fclose(fd);
      ::fprintf(stdout,"...Child ended...exit...\n");
      ::fflush(stdout);
      run = false;
      _exit(0);
    }
  }
  return 0;
}

int test_set_sys_environment() {
  checkpointing_set_restart_flags(MTCP_STDIN_ENV);
  return checkpointing_sys_set_environment(&chkpt_sys);
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
int test_set_environment(int flag);
extern "C" char** environ;

extern "C" int chkpt_tests(int argc, char** argv) {
  int opt = *(int*)"None";
  int prt = MTCP_INFO, flag=0;
  const char* p = (char*)&opt, *q="";
  for(int i=0; i<argc; ++i) {
    if      ( argc>i && argv[i][1] == 'f' ) opt  = *(int*)(q=argv[++i]);
    else if ( argc>i && argv[i][1] == 'p' ) prt   = (argv[++i][0]-'0')|(prt&MTCP_PRINT_NO_PID);
    else if ( argc>i && argv[i][1] == 'n' ) prt  |= MTCP_PRINT_NO_PID;
    else if ( argc>i && argv[i][1] == 'e' ) flag |= MTCP_STDIN_ENV;
    else if ( argc>i && argv[i][1] == 'l' ) flag |= MTCP_SAVE_LIBS;
  }
  ::fprintf(stdout,"Checkpointing_test: print level:%d flag:%d tst function:%s [%c%c%c%c]\n",
	    prt,flag,q,p[0],p[1],p[2],p[3]);
  mtcp_set_debug_level(prt);
  checkpointing_initialize_parent(argc,argv,environ);
  if      ( opt == *(int*)"m_write"   ) test_MemMaps_write();
  else if ( opt == *(int*)"m_read"    ) test_MemMaps_read();
  else if ( opt == *(int*)"m_share"   ) test_MemMaps_sharable();

  else if ( opt == *(int*)"f_scan"    ) test_FileMap_scan();
  else if ( opt == *(int*)"f_memory"  ) test_FileMap_memory();

  else if ( opt == *(int*)"p_write"   ) test_Process_write();
  else if ( opt == *(int*)"p_read"    ) test_Process_read();
  else if ( opt == *(int*)"p_restore" ) test_Process_restore();

  else if ( opt == *(int*)"t_checkpo" ) test_thread_checkpoint(flag);
  else if ( opt == *(int*)"t_environ" ) test_set_environment(flag);
  else if ( opt == *(int*)"t_senviron") test_set_sys_environment();
  else mtcp_output(MTCP_ERROR,"No function given. Test is meaningless.\n");
  return 1;
}
