// ---------------------------------------------------------------------------
//
// Usage:  gentest.exe libGaudiOnlineTests.so pthread_cancellation
//
// To test signals with libCheckpointing.so use:
//
// $> export LD_PRELOAD=${CHECKPOINTINGROOT}/${CMTCONFIG}/libCheckpointing.so;
// $> export MTCP_OUTPUT=DEBUG
// $> GaudiCheckpoint.exe libGaudiOnlineTests.so pthread_cancellation
//
// ---------------------------------------------------------------------------
#include <pthread.h>
#include <cstdlib>
#include <csignal>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <syscall.h>

#define gettid() ((int)syscall(SYS_gettid))

//#define CHECK_THREAD(x,y) test_tid(x)
//#define CHECK_THREAD(x,y) kill(x,y)
#define CHECK_THREAD(x,y) syscall(SYS_tkill,3416,y)

using namespace std;

namespace {
  pid_t thread_tid = 0;
  bool run = true;
  void cleanup_thread(void*) {
    ::printf("CXX using stack-unwind +++++ CANCEL+++CANCEL+++ Foo was cancelled\n");
    ::fflush(stdout);
  }

  static void *test_thread(void *)   {
    pthread_cleanup_push(cleanup_thread,NULL);
    ::printf("+++ tid: 0x%08X: Thread starting.\n",gettid());
    thread_tid = gettid();
    while(run) ::usleep(1000);
    pthread_cleanup_pop(0);
    ::printf("+++ tid: 0x%08X: Thread about to exit.\n",gettid());
    return 0;
  }
  void (*_cancel_action)(int signum, siginfo_t *si, void *ctx);
  void _test_action(int signum, siginfo_t *si, void *ctx)   {
    ::printf("C/Signal  +++ Thread CANCELLED.\n");
    ::fflush(stdout);
    (*_cancel_action)(signum,si,ctx);
  }
}

extern "C" int pthread_cancellation(int, char**)   {
  void* ret;
  int status;
  pthread_t t_id = 0;
  struct sigaction old, act;

  ::pthread_create(&t_id, 0, test_thread, 0);
  ::sleep(1);
  ::printf("+++ tid: 0x%08X: Send signal 0 to (ALIVE) thread.\n",gettid());
  status = CHECK_THREAD(thread_tid,0);
  if ( status<0 )  {
    ::printf("+++ Error[%d]: kill %s\n",errno, ::strerror(errno));
  }
  ::printf("+++ tid: 0x%08X: Kill status: %d errno:%d\n",gettid(),status,errno);

  //run = 0;
  ::sleep(2);
  ::printf("+++ tid: 0x%08X: Send signal 0 to (EXITED) thread.\n",gettid());
  status = CHECK_THREAD(thread_tid,0);
  if ( status<0 )  {
    ::printf("+++ Error[%d]: kill %s\n",errno, ::strerror(errno));
  }
  ::printf("+++ tid: 0x%08X: Kill status: %d errno:%d\n",gettid(),status,errno);
  ::sleep(1);

  if ( syscall(SYS_rt_sigaction,32,0,&old,NSIG/8) < 0 )
    ::printf("+++ Error[%d]: sigaction %s\n",errno, ::strerror(errno));
  ::memcpy(&act,&old,sizeof(act));
  _cancel_action = old.sa_sigaction;
  act.sa_sigaction = _test_action;
  if ( syscall(SYS_rt_sigaction,32,&act,0,NSIG/8) < 0 )
    ::printf("+++ Error[%d]: sigaction %s\n",errno, ::strerror(errno));

  ::pthread_cancel(t_id);
  ::pthread_join(t_id,&ret);
  ::printf("+++ Thread joined.\n");
  ::sleep(2);
  ::printf("+++ tid: 0x%08X: Send signal 0 to (DEAD) thread.\n",gettid());
  status = CHECK_THREAD(thread_tid,0);
  if ( status<0 )  {
    ::printf("+++ Error[%d]: kill %s\n",errno, ::strerror(errno));
  }
  ::printf("+++ tid: 0x%08X: Kill status: %d errno:%d %s\n",
	   gettid(),status,errno,strerror(errno));
  ::sleep(2);
  ::printf("+++ Process exiting.\n");
  return 0;
}
