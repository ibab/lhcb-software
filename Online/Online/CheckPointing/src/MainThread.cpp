#include "CheckPointing/MainThread.h"
#include "CheckPointing/MB.h"
#include "CheckPointing/LibC.h"
#include "CheckPointing/SysCalls.h"
#include "CheckPointing/FutexState.h"
#include "CheckPointing/Static.h"
#include "CheckPointing/Thread.h"
#include "CheckPointing/ThreadsLock.h"

#include <cstdio>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <ucontext.h>
#include <sys/types.h>
#include <linux/futex.h>
#include <unistd.h>

#define MTCP_DEFAULT_SIGNAL SIGUSR2
static int s_stopSignal = MTCP_DEFAULT_SIGNAL;

int                s_motherPID = 0;
Thread*            s_motherOfAll = 0;
libc_clone_entry_t s_cloneCall = 0;

void *const restore_finish_function_pointer = (void*)MainThread::finishRestore;

extern "C" CheckpointRestoreWrapper* libProcessRestore_main_instance() {
  static CheckpointRestoreWrapper* p = 0;
  if ( !p ) {
    MainThread& m = MainThread::accessInstance();
    m.initialize();
    p = &m;
  }
  return p;
}

MainThread::MainThread()  {
  LibC::getSymbol("__clone",s_cloneCall);
  s_motherOfAll = new Thread();
  s_motherPID = mtcp_sys_getpid();
  asm (".global clone ; .type clone,@function ; clone = __clone");
}

MainThread& MainThread::accessInstance() {
  static MainThread s_main;
  return s_main;
}

int MainThread::stopSignal() {
  return s_stopSignal;
}

int MainThread::currentThreadID() {
  return Thread::current()->tid();
}

int     MainThread::motherPID() const            {
  return s_motherPID;   
}

Thread* MainThread::motherOfAll() const          {
  return s_motherOfAll; 
}

libc_clone_entry_t MainThread::cloneCall() const {
  return s_cloneCall;   
}

void MainThread::initialize() {
  static bool inited = false;
  if ( !inited ) {
    s_motherOfAll->initialize(inited=true);
  }
  else {
    s_motherOfAll->m_tid = mtcp_sys_kernel_gettid ();
    s_motherOfAll->m_restartArgs.tid = s_motherOfAll->m_tid;
    s_motherOfAll->m_restartArgs.self = s_motherOfAll;
    s_motherOfAll->m_originalTID = (int)syscall(SYS_gettid);
    s_motherOfAll->setupSignals();
  }
  s_motherOfAll->m_childTID = mtcp_sys_kernel_gettid();
  s_motherOfAll->set_tid_address();
  s_motherOfAll->state().init(ST_CKPNTHREAD);
}

int MainThread::startChild() {
  s_motherOfAll->restart();
  mtcp_output(MTCP_INFO,"ALL Child threads are now started:%d\n",s_motherPID);
  return 1;
}

int MainThread::forkInstance() {
  pid_t pID = mtcp_sys_fork();
  if (pID == 0)     {           // child
    // sleep(5);
    // Code only executed by child process
    //LibC::getSymbol("__clone",s_cloneCall);
    //s_motherPID = mtcp_sys_getpid();
    s_motherOfAll->m_tid = mtcp_sys_kernel_gettid();
    s_motherOfAll->m_childTID = mtcp_sys_kernel_gettid();
    mtcp_output(MTCP_INFO,"forkInstance: Child process started:%d -- Need to still start threads\n",
		s_motherPID);
  }
#if 0
  else {
    char text[128];
    sprintf(text,"gdb --pid %d",pID);
    printf("\nChild:\n%s\n\n",text);
    sleep(1);
    system(text);
  }
#endif
  return pID;
}

int MainThread::stop()   {
  int needrescan;
  Thread* thread;
  static struct timespec const enabletimeout = { 10, 0 };

  s_motherOfAll->saveSignals();
  s_motherOfAll->saveTLS();

  if (getcontext (&s_motherOfAll->m_savctx) < 0) mtcp_abort ();

  while (1) {
    sigpending (&s_motherOfAll->m_sigpending);
    Thread::setupSignals();

    // Halt all other threads - force them to call stopthisthread
    // If any have blocked checkpointing, wait for them to unblock before signalling
rescan:
    needrescan = 0;
    ThreadsLock::take();
    for (thread = Thread::threads(); thread != NULL; thread = thread->next) {

again:
      Thread::setupSignals(); //keep pounding the signal handler in
      // If thread no longer running, remove it from thread list
      if (*(thread->m_pActualTID) == 0) {
        mtcp_output(MTCP_WARNING,"stopThreads*: thread %d disappeared\n", thread->m_tid);
        ThreadsLock::release ();
        thread->cleanup();
        goto rescan;
      }
      FutexState& st = thread->state();
      // Do various things based on thread's state
      switch (thread->state().value() ) {
      case ST_RUNDISABLED:
        // Thread is running but has checkpointing disabled
        // Tell the instrumentStop routine that we are waiting for it
        // We will need to rescan so we will see it suspended
	if (!st.set(ST_SIGDISABLED, ST_RUNDISABLED)) goto again;
	needrescan = 1;
	break;
      
      case ST_RUNENABLED:
        // Thread is running and has checkpointing enabled
        // Send it a signal so it will call Thread::stop
        // We will need to rescan (hopefully it will be suspended by then)
	if (!st.set(ST_SIGENABLED, ST_RUNENABLED)) goto again;
	if (mtcp_sys_kernel_tkill (thread->m_tid, STOPSIGNAL) < 0) {
	  if (mtcp_sys_errno != ESRCH) {
	    mtcp_output(MTCP_WARNING,"stopThreads: error signalling thread %d: %s\n",
			thread->m_tid, strerror (mtcp_sys_errno));
	  }
	  else {
	    mtcp_output(MTCP_INFO,"stopThreads: send KILL to thread %d\n",thread->m_tid);
	  }
	  ThreadsLock::release ();
	  thread->cleanup();
	  goto rescan;
	}
	needrescan = 1;
	break;

        case ST_SIGDISABLED:
	  // Thread is running, we have signalled it to stop, but it has checkpointing disabled
	  // So we wait for it to change state
	  // We have to unlock because it may need lock to change state
          ThreadsLock::release ();
	  mtcp_output(MTCP_INFO,"stopThreads: ST_SIGDISABLED waiting for thread %d\n",thread->m_tid);
          st.wait(FUTEX_WAIT, ST_SIGDISABLED, &enabletimeout);
          goto rescan;

        /* Thread is running and we have sent signal to stop it             */
        /* So we have to wait for it to change state (enter signal handler) */
        /* We have to unlock because it may try to use lock meanwhile       */
        case ST_SIGENABLED: {
          ThreadsLock::release ();
	  mtcp_output(MTCP_INFO,"stopThreads: ST_SIGENABLED waiting for thread %d\n",thread->m_tid);
          st.wait(FUTEX_WAIT, ST_SIGENABLED, &enabletimeout);
          goto rescan;
        }

        /* Thread has entered signal handler and is saving its context             */
        /* So we have to wait for it to finish doing so                            */
        /* We don't need to unlock because it won't use lock before changing state */
        case ST_SUSPINPROG: {
	  mtcp_output(MTCP_INFO,"stopThreads: ST_SUSPINPROG waiting for thread %d\n",thread->m_tid);
          st.wait(FUTEX_WAIT, ST_SUSPINPROG, &enabletimeout);
          goto again;
        }

        /* Thread is suspended and all ready for us to write checkpoint file */
        case ST_SUSPENDED: {
          break;
        }

        /* Don't do anything to the stopThreads (this) thread */
        case ST_CKPNTHREAD: {
          break;
        }

        /* Who knows? */
        default: {
          mtcp_abort ();
        }
      }
    }
    ThreadsLock::release();

    // If need to rescan (ie, some thread possibly not in ST_SUSPENDED STATE), check them all again
    if (needrescan) goto rescan;
    RMB; // matched by WMB in stopthisthread
    mtcp_output(MTCP_INFO,"stopThreads*: everything suspended\n");

    // If no threads, we're all done
    if ( 0 == Thread::threads() ) {
      mtcp_output(MTCP_WARNING,"stopThreads*: exiting (no threads)\n");
      return 0;
    }
    Thread::saveSysInfo();
    return 1;
  }
  return 0;
}

int MainThread::resume()   {
  // Resume all threads.  But if we're doing a checkpoint verify, abort all threads except
  // the main thread, as we don't want them running when we exec the mtcp_restore program.
  mtcp_printf("mtcp resumeThreads*: resuming everything\n");
  ThreadsLock lock;
  for (Thread* thread = Thread::threads(); thread != NULL; thread = thread->next) {
    FutexState& st = thread->state();
    if (st.value() != ST_CKPNTHREAD) {
      if (!st.set(ST_RUNENABLED, ST_SUSPENDED)) mtcp_abort ();
      st.wait(FUTEX_WAKE, 1, NULL);
    }
  }
  mtcp_output(MTCP_INFO,"mtcp resumeThreads*: everything resumed\n");
  // But if we're doing a restore verify, just exit.  The main thread is doing the exec to start the restore.
  return 1;
}


static struct timeval restorestarted;

void MainThread::finishRestore() {
  struct timeval stopped;
  //int nnamelen;

  mtcp_sys_gettimeofday (&restorestarted, NULL);

  mtcp_output(MTCP_INFO,"finishrestore*: mtcp works; libc should work\n");
  mtcp_sys_gettimeofday(&stopped,0);
  stopped.tv_usec += (stopped.tv_sec - restorestarted.tv_sec) * 1000000 - restorestarted.tv_usec;
  mtcp_output(MTCP_INFO,"finishrestore*: time %u uS\n", stopped.tv_usec);

  // Now we can access all our files and memory that existed at the time of the checkpoint
  // We are still on the temporary stack, though

  // Fill in the new mother process id
  s_motherPID = mtcp_sys_getpid();

  Thread::finishRestore(s_motherOfAll);
  mtcp_output(MTCP_INFO,"ALL Child threads are now started:%d\n",s_motherPID);
}

int MainThread::checkpoint(int fd) {
  if ( fd > 0 ) {
  }
  return 1;
}

int MainThread::restore(int fd) {
  if ( fd > 0 ) {
  }
  return 1;
}

int MainThread::restart() {
  return 1;
}
