#include "Checkpointing/Chkpt.h"
#include "Checkpointing.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <pthread.h>
#include <unistd.h>

void mtcp_output(int lvl,const char* fmt,...);
void mtcp_abort (void);

using namespace Checkpointing;
using namespace std;

#define NUM_THREADS 5
static pthread_t pid[NUM_THREADS], main_pid;

static int threads_starting = 6;

static void *test_thread (void *dummy)    {
  long count = (long)dummy;
  --threads_starting;
  while(--count) {
    ::sleep(1);
    mtcp_output(MTCP_INFO,"SLAVE Thread %p needs to sleep for %d seconds\n",pthread_self(),count);
    if ( count == 15 ) break;
  }
  return 0;
}

static void *main_thread (void *dummy)    {
  long count = (long)dummy;
  threads_starting += NUM_THREADS;
  for(int i=0; i<NUM_THREADS;++i) {
    if (pthread_create (&pid[i], NULL, test_thread, (void*)long(3*i+5)) < 0) {
      mtcp_output(MTCP_INFO,"MAIN Thread: error creating SLAVE thread: %s\n",strerror(errno));
      mtcp_abort();
    }
  }
  threads_starting -= NUM_THREADS;
  --threads_starting;
  while(--count) {
    ::sleep(1);
    mtcp_output(MTCP_INFO,"MAIN Thread %p needs to sleep for %d seconds\n",pthread_self(),count);
  }
  return 0;
}

static void fork_apps()    {
  char text[128];
  int tid = (int)pthread_self();

  mtcp_output(MTCP_INFO,"Thread %d: All threads are started now.....\n",tid);
  checkpointing_init_checkpoints();
  checkpointing_stop_process();

  mtcp_output(MTCP_INFO,"All threads are checkpointed :-))) .....\n");
  mtcp_output(MTCP_INFO,"\n\n\nWait for forking ..... now: Hit ENTER to continue!\n\n");
  fscanf(stdin,"%c",&text[0]);
  mtcp_output(MTCP_INFO,"\n\n\n......Now forking ..... now\n\n");
  pid_t pID = checkpointing_fork_process();
  if ( pID == 0 ) {
    pthread_t mm;
    void* val;
    threads_starting += 2;
    //setpgrp();
    pid_t sid = ::setsid();
    if (sid < 0) {
      exit(EXIT_FAILURE);
    }
    if (pthread_create (&mm, NULL, test_thread, (void*)4) < 0) {
      mtcp_output(MTCP_INFO,"MAIN Thread: error creating WAIT thread: %s\n",strerror(errno));
      mtcp_abort();
    }
    --threads_starting;
    if (pthread_join(mm,&val) < 0) {
      mtcp_output(MTCP_INFO,"init: FAILED to join thread: WAIT\n");
    }
    mtcp_output(MTCP_INFO,"init: joined thread: WAIT\n");
  }
  else if ( pID>0 ) {
    sprintf(text,"gdb --pid %d",pID);
    printf("\nParent:\n%s\n\n",text);
    mtcp_output(MTCP_INFO,"Parent process resuming work.....\n");
    checkpointing_resume_process();
  }
  else if (pID < 0)    {        // failed to fork
    mtcp_output(MTCP_INFO,"Failed to fork. %s\n",strerror(errno));
    exit(1);
    // Throw exception
  }
}

extern "C" int fork_test(int /* argc */ , char**  /* argv */) {
  mtcp_set_debug_level(1);
  mtcp_output(MTCP_INFO,"\n\n\nChecking threads.... \n\n\n");

  checkpointing_init_checkpoints();

  int tid = checkpointing_current_thread_id();
  ++threads_starting;
  if (pthread_create (&main_pid, NULL, main_thread, (void*)5) < 0) {
    mtcp_output(MTCP_FATAL,"init: error creating main thread: %s\n", strerror (errno));
  }
  --threads_starting;

  // Wait for workers being active
  while(0 != threads_starting) {
    mtcp_output(MTCP_INFO,"Thread %d waiting for %d slaves\n",tid,threads_starting);
    sleep(2);
  }
  fork_apps();

  void* val;
  mtcp_output(MTCP_INFO,"init: TRY join thread: MAIN\n");

  if (pthread_join(main_pid,&val) < 0) {
    mtcp_output(MTCP_INFO,"init: FAILED to join thread: MAIN\n");
  }
  mtcp_output(MTCP_INFO,"init: joined thread: MAIN\n");
  for(int j=0; j<NUM_THREADS;++j) {
    mtcp_output(MTCP_INFO,"init: TRY: join thread: SLAVE[%d]\n",j);
    if (pthread_join(pid[j],&val) < 0) {
      mtcp_output(MTCP_INFO,"init: FAILED to join thread: SLAVE[%d]\n",j);
    }
    mtcp_output(MTCP_INFO,"init: Joined thread: SLAVE[%d]\n",j);
  }
  mtcp_output(MTCP_INFO,"init: ******************** THREAD TEST FINISHED *************************\n");
  return 1;
}
