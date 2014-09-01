#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <syscall.h>

#define gettid() syscall(SYS_gettid)

//#define CHECK_THREAD(x,y) test_tid(x)
//#define CHECK_THREAD(x,y) kill(x,y)
#define CHECK_THREAD(x,y) syscall(SYS_tkill,3416,y)

using namespace std;
namespace {
  pid_t thread_tid = 0;
  bool run = true;
  int test_tid(pid_t tid)  {
    char text[256];
    struct stat buf;
    ::sprintf(text,"/proc/%d",tid);
    int status = ::stat(text,&buf);
    ::printf("+++ tid:%5d: Check file %s status:%d\n",gettid(),text,status);
    return status;
  }

  void cleanup_thread(void*) {
    ::printf("+++++ CANCEL+++CANCEL+++ Foo was cancelled\n");
  }

  static void *test_thread(void *)   {
    //pthread_cleanup_push(cleanup_thread,NULL);
    ::printf("+++ tid:%5d: Thread starting.\n",gettid());
    thread_tid = gettid();
    while(run) ::usleep(1000);
    //pthread_cleanup_pop(0);
    ::printf("+++ tid:%5d: Thread about to exit.\n",gettid());
    return 0;
  }
  void (*_cancel_action)(int signum, siginfo_t *si, void *ctx);
  void _test_action(int signum, siginfo_t *si, void *ctx)   {
    ::write(2,"+++ Thread CANCELLED.\n",22);
    (*_cancel_action)(signum,si,ctx);
  }
}

int main(int, char**)   {
  void* ret;
  int status;
  pthread_t t_id = 0;
  struct sigaction old, act;

  ::pthread_create(&t_id, 0, test_thread, 0);
  ::sleep(1);
  ::printf("+++ tid:%5d: Send signal 0 to (ALIVE) thread.\n",gettid());
  status = CHECK_THREAD(thread_tid,0);
  if ( status<0 )  {
    ::perror("kill");
  }
  ::printf("+++ tid:%5d: Kill status: %d errno:%d\n",gettid(),status,errno);

  //run = 0;
  ::sleep(2);
  ::printf("+++ tid:%5d: Send signal 0 to (EXITED) thread.\n",gettid());
  status = CHECK_THREAD(thread_tid,0);
  if ( status<0 )  {
    ::perror("kill");
  }
  ::printf("+++ tid:%5d: Kill status: %d errno:%d\n",gettid(),status,errno);
  ::sleep(1);
  if ( syscall(SYS_rt_sigaction,32,0,&old,NSIG/8) < 0 )  {
    ::perror("sigaction");
  }
  ::memcpy(&act,&old,sizeof(act));
  _cancel_action = old.sa_sigaction;
  act.sa_sigaction = _test_action;
  if ( syscall(SYS_rt_sigaction,32,&act,0,NSIG/8) < 0 )  {
    ::perror("sigaction");
  }

  ::pthread_cancel(t_id);
  ::pthread_join(t_id,&ret);
  ::printf("+++ Thread joined.\n");
  ::sleep(2);
  ::printf("+++ tid:%5d: Send signal 0 to (DEAD) thread.\n",gettid());
  status = CHECK_THREAD(thread_tid,0);
  if ( status<0 )  {
    ::perror("kill");
  }
  ::printf("+++ tid:%5d: Kill status: %d errno:%d %s\n",
	   gettid(),status,errno,strerror(errno));
  ::sleep(2);
  ::printf("+++ Process exiting.\n");
  return 1;
}
