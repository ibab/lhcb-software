#include "Checkpointing/MainThread.h"
#include "Checkpointing/MB.h"
#include "Checkpointing/LibC.h"
#include "Checkpointing/SysCalls.h"
#include "Checkpointing/FutexState.h"
#include "Checkpointing/Static.h"
#include "Checkpointing/Thread.h"
#include "Checkpointing/ThreadsLock.h"
#include "Checkpointing/FileMap.h"
#include "Checkpointing/Chkpt.h"
#include "Checkpointing.h"
#include "Restore.h"

#include <cstdio>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <ucontext.h>
#include <sys/types.h>
#include <linux/futex.h>
#include <linux/personality.h>
#include <unistd.h>

using namespace Checkpointing;

extern "C" MainThread* libProcessRestore_main_instance() {
  static MainThread* p = 0;
  if ( !p ) {
    mtcp_sys_personality(ADDR_NO_RANDOMIZE|mtcp_sys_personality(0xFFFFFFFFUL));
    MainThread& m = MainThread::accessInstance();
    m.initialize();
    p = &m;
  }
  return p;
}

extern "C" MainThread* libProcessRestore_main_instance_init(int argc, char** argv, char** environment) {
  static MainThread* p = 0;
  if ( !p ) {
    p = libProcessRestore_main_instance();
  }
  checkpointing_sys_init_stack(&chkpt_sys,argc,argv,environment);
  return p;
}


/// The function pointer of libC's original clone call
static MainThread::clone_t s_cloneFunc;
static Thread s_thread;

void MainThread::init_instance(int argc, char** argv, char** environment) {
  checkpointing_sys_init_stack(&chkpt_sys,argc,argv,environment);
}

MainThread::MainThread()  {
  LibC::getSymbol("__clone",s_cloneFunc);
  chkpt_sys.motherofall   = &s_thread;// new Thread();
  chkpt_sys.savedContext  = &chkpt_sys.motherofall->m_savctx;
  chkpt_sys.restart_type  = SysInfo::RESTART_NORMAL;
  chkpt_sys.finishRestore = checkpointing_finish_restore;
  chkpt_sys.motherPID     = chkpt_sys.chkptPID = mtcp_sys_getpid();
  __asm__ (".global clone ; .type clone,@function ; clone = __clone");
}

WEAK(MainThread&) MainThread::accessInstance() {
  static MainThread s_main;
  return s_main;
}

WEAK(MainThread::clone_t) MainThread::cloneFunction() {
  return s_cloneFunc;
}

WEAK(void) MainThread::initialize() {
  static bool inited = false;
  if ( !inited ) {
    chkpt_sys.motherofall->initialize(inited=true);
  }
  else {
    chkpt_sys.motherofall->m_tid = mtcp_sys_kernel_gettid ();
    chkpt_sys.motherofall->m_restartArgs.tid  = chkpt_sys.motherofall->m_tid;
    chkpt_sys.motherofall->m_restartArgs.self = chkpt_sys.motherofall;
    chkpt_sys.motherofall->m_originalTID      = (int)syscall(SYS_gettid);
    checkpointing_thread_setup_signals();
  }
  chkpt_sys.motherofall->m_childTID = mtcp_sys_kernel_gettid();
  chkpt_sys.motherofall->set_tid_address();
  chkpt_sys.motherofall->state.value = ST_CKPNTHREAD;
}

WEAK(void) MainThread::finishRestore() {
  checkpointing_finish_restore();
}

STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_finish_restore() {
  // Fill in the new mother process id
  chkpt_sys.motherPID    = mtcp_sys_getpid();
  chkpt_sys.restart_type = SysInfo::RESTART_CHECKPOINT;
  mtcp_output(MTCP_INFO,"checkpointing_finish_restore: motherPID:%d motherofall:%p; mtcp works; libc should work as well.\n",
	      chkpt_sys.motherPID,chkpt_sys.motherofall);

  // Now move to the temporary stack for restoring all process properties at the time of the checkpoint.
  // Call another routine because our internal stack is whacked and we can't have local vars
  // Thread::restart() will have a big stack
  // checkpointing_print_stack("restored");
  __asm__ volatile (CLEAN_FOR_64_BIT(mov %0,%%esp)
		: : "g" (chkpt_sys.motherofall->m_savctx.SAVEDSP - 128 ) : "memory");  // -128 for red zone

  mtcp_output(MTCP_INFO,"checkpointing_finish_restore: Restarting mother thread:%d\n",chkpt_sys.motherPID);
  checkpointing_thread_restart(chkpt_sys.motherofall,1);
  mtcp_output(MTCP_INFO,"checkpointing_finish_restore: ALL Child threads are now started:%d\n",chkpt_sys.motherPID);
}

extern "C" int checkpointing_current_thread_id() {
  return Thread::current()->tid();
}

extern "C" int checkpointing_start_child() {
  checkpointing_thread_restart(chkpt_sys.motherofall,0);
  mtcp_output(MTCP_INFO,"startChild: ALL Child threads are now started:%d\n",chkpt_sys.motherPID);
  return 1;
}

extern "C" int checkpointing_fork_process() {
  pid_t pID = mtcp_sys_fork();
  if (pID == 0)     {           // child
    chkpt_sys.motherPID    = mtcp_sys_getpid();
    chkpt_sys.restart_type = SysInfo::RESTART_NORMAL;
    chkpt_sys.motherofall->m_tid = mtcp_sys_kernel_gettid();
    chkpt_sys.motherofall->m_childTID = mtcp_sys_kernel_gettid();
    mtcp_output(MTCP_INFO,"forkInstance: Child process started:%d -- Need to still start threads\n",
		chkpt_sys.motherPID);
  }
  else {
    mtcp_output(MTCP_INFO,"forkMain:     Child process started:%d -- Need to still start threads\n",
		chkpt_sys.motherPID);
  }
  return pID;
}

extern "C" int checkpointing_stop_process()   {
  int needrescan;
  Thread* thread;
  static struct timespec const enabletimeout = { 10, 0 };

  checkpointing_thread_save_signals(chkpt_sys.motherofall);
  chkpt_sys.motherofall->saveTLS();
  if ( ::getcontext (&chkpt_sys.motherofall->m_savctx) < 0 )   {
    mtcp_output(MTCP_FATAL,"stopThreads: failed to retrieve context of motherofall.\n");
    mtcp_abort();
  }
  while (1) {
    sigpending (&chkpt_sys.motherofall->m_sigpending);
    checkpointing_thread_setup_signals();

    // Halt all other threads - force them to call stopthisthread
    // If any have blocked checkpointing, wait for them to unblock before signalling
rescan:
    needrescan = 0;
    checkpointing_threads_lock();
    for (thread = checkpointing_thread_queue(); thread != NULL; thread = thread->next) {

again:
      checkpointing_thread_setup_signals(); //keep pounding the signal handler in
      // If thread no longer running, remove it from thread list
      if (*(thread->m_pActualTID) == 0) {
        mtcp_output(MTCP_WARNING,"stopThreads[pid:%d]: thread %d disappeared\n",
		    chkpt_sys.motherPID,thread->m_tid);
        checkpointing_threads_unlock ();
        checkpointing_thread_cleanup(thread);
        goto rescan;
      }
      FutexState& st = thread->state;
      // Do various things based on thread's state
      switch (thread->state.value ) {
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
	    mtcp_output(MTCP_WARNING,"stopThreads[pid:%d]: error signalling thread %d: %s\n",
			chkpt_sys.motherPID,thread->m_tid, strerror (mtcp_sys_errno));
	  }
	  else {
	    mtcp_output(MTCP_INFO,"stopThreads[pid:%d]: send KILL to thread %d\n",
			chkpt_sys.motherPID,thread->m_tid);
	  }
	  checkpointing_threads_unlock ();
	  checkpointing_thread_cleanup(thread);
	  goto rescan;
	}
	needrescan = 1;
	break;

        case ST_SIGDISABLED:
	  // Thread is running, we have signalled it to stop, but it has checkpointing disabled
	  // So we wait for it to change state
	  // We have to unlock because it may need lock to change state
          checkpointing_threads_unlock ();
	  mtcp_output(MTCP_INFO,"stopThreads[pid:%d]: ST_SIGDISABLED waiting for thread %d\n",
		      chkpt_sys.motherPID,thread->m_tid);
          st.wait(ST_SIGDISABLED, &enabletimeout);
          goto rescan;

        /* Thread is running and we have sent signal to stop it
	 * So we have to wait for it to change state (enter signal handler)
	 * We have to unlock because it may try to use lock meanwhile
	 */
        case ST_SIGENABLED: {
          checkpointing_threads_unlock ();
	  mtcp_output(MTCP_INFO,"stopThreads[pid:%d]: ST_SIGENABLED waiting for thread %d\n",
		      chkpt_sys.motherPID,thread->m_tid);
          st.wait(ST_SIGENABLED, &enabletimeout);
          goto rescan;
        }

        /* Thread has entered signal handler and is saving its context
	 * So we have to wait for it to finish doing so
         * We don't need to unlock because it won't use lock before changing state
	 */
        case ST_SUSPINPROG: {
	  mtcp_output(MTCP_INFO,"stopThreads[pid:%d]: ST_SUSPINPROG waiting for thread %d\n",
		      chkpt_sys.motherPID,thread->m_tid);
          st.wait(ST_SUSPINPROG, &enabletimeout);
          goto again;
        }

	// Thread is suspended and all ready for us to write checkpoint file
        case ST_SUSPENDED: {
          break;
        }

	// Don't do anything to the stopThreads (this) thread 
        case ST_CKPNTHREAD: {
          break;
        }

	// Who knows?
        default: {
	  mtcp_output(MTCP_FATAL,"Fatal inconsistency in MainThread::stop. tid:%d State:%d\n",
		      thread->m_tid, thread->state.value);
        }
      }
    }
    mtcp_output(MTCP_INFO,"stopThreads: return address[1] %p\n",__builtin_return_address (0));
    checkpointing_threads_unlock();

    // If need to rescan (ie, some thread possibly not in ST_SUSPENDED STATE), check them all again
    if (needrescan) goto rescan;

    RMB; // matched by WMB in stopthisthread
    mtcp_output(MTCP_INFO,"stopThreads: everything suspended\n");

    // If no threads, we're all done
    if ( checkpointing_thread_queue() == 0 ) {
      mtcp_output(MTCP_WARNING,"stopThreads: exiting (no threads)\n");
      return 0;
    }
    checkpointing_thread_save_sys_info();
    mtcp_output(MTCP_INFO,"stopThreads: finished --pid %d  return address:%p\n",
		mtcp_sys_getpid(),__builtin_return_address(0));
    return 1;
  }
}

extern "C" int checkpointing_resume_process()   {
  // Resume all threads. We do not want to execute this on a restore from file,
  // Since the threads were already started!
  mtcp_output(MTCP_INFO,"resume: resuming main thread\n");
  checkpointing_threads_lock ();
  for (Thread* thread = checkpointing_thread_queue(); thread != NULL; thread = thread->next) {
    FutexState& st = thread->state;
    int val = st.value;
    if (val != ST_CKPNTHREAD) {
      mtcp_output(MTCP_DEBUG,"resume[pid:%d]: tid:%d State:%d RUNENABLED:%d SUSPENDED:%d\n",
		  chkpt_sys.motherPID, thread->m_tid, val, ST_RUNENABLED, ST_SUSPENDED);
      if (!st.set(ST_RUNENABLED, ST_SUSPENDED))   {
	mtcp_output(val==ST_RUNENABLED ? MTCP_INFO : MTCP_FATAL,
		    "Fatal inconsistency in resume[pid:%d]: tid:%d State:%d RUNENABLED:%d SUSPENDED:%d\n",
		    chkpt_sys.motherPID, thread->m_tid, val, ST_RUNENABLED, ST_SUSPENDED);
      }
      st.wake();
    }
  }
  checkpointing_thread_restore_signals(chkpt_sys.motherofall);
  mtcp_output(MTCP_INFO,"resume: main thread - everything resumed\n");
  // But if we're doing a restore verify, just exit.  The main thread is doing the exec to start the restore.
  checkpointing_threads_unlock ();
  return 1;
}

extern "C" int checkpointing_init_checkpoints() {
  MainThread* p = libProcessRestore_main_instance();
  p->initialize();
  return p ? 1 : 0;
}

extern "C" int checkpointing_initialize_parent(int argc, char** argv, char** environment)   {
  MainThread* p = libProcessRestore_main_instance_init(argc,argv,environment);
  return p ? 1 : 0;
}

extern "C" int checkpointing_set_save_flags(int flags)   {
  checkpointing_sys_set_save_flags(&chkpt_sys, flags);
  return 1;
}

extern "C" int checkpointing_set_restart_flags(int flags)   {
  checkpointing_sys_set_restart_flags(&chkpt_sys, flags);
  return 1;
}

extern "C" int checkpointing_set_lib_directory(const char* lib_dir)   {
  checkpointing_sys_set_lib_directory(&chkpt_sys, lib_dir);
  return 1;
}

extern "C" int checkpointing_set_utgid(const char* new_utgid)   {
  return checkpointing_sys_set_utgid(&chkpt_sys, new_utgid);
}

extern "C" int checkpointing_force_utgid(const char* new_utgid)   {
  return checkpointing_sys_force_utgid(&chkpt_sys, new_utgid);
}

extern "C" int checkpointing_show_sig_actions()   {
  struct sigaction a;
  for (int i=1; i<NSIG; ++i ) {
    void** mask = (void**)&a.sa_mask;
    if (mtcp_sys_sigaction(i,0,&a) < 0) {
      if (errno == EINVAL) ::memset(&a,0,sizeof(a));
    }
    if ( a.sa_handler != 0 ) {
      mtcp_output(MTCP_INFO,"Signal action [%d] -> %p Flags:%X Mask:%p\n",
		  i, a.sa_handler, a.sa_flags, *mask);
    }
  }
  return 1;
}

extern "C" int checkpointing_write_checkpoint(int fd) {
  if ( fd > 0 ) {
    checkpointing_sys_set_checkpoint_file(&chkpt_sys,fd);
    long true_len = checkpointing_process_fwrite(fd);
    mtcp_output(MTCP_INFO,"checkpoint: Wrote %ld bytes of checkpoint data to fd:%d.\n",true_len,fd);
    checkpointing_thread_restore_signals(chkpt_sys.motherofall);
    return true_len;
  }
  mtcp_output(MTCP_ERROR,"Failed to write checkpoint file. [%s]\n",::strerror(EBADF));
  return 0;
}

/// Give access to the restart type after a call to "checkpoint". 0==checkpointing, 1=restart from checkpoint
extern "C" int checkpointing_restart_type()  {
  return chkpt_sys.restart_type;
}

extern "C" int checkpointing_get_file_descriptors(void** ptr) {
  FileMap m;
  long bytes = m.memoryCount();
  if ( bytes > 0 )  {
    void* mem = ::malloc(bytes);
    FileStreamOutHandler wr(mem);
    if ( m.scan(wr.start()) > 0 ) {
      wr.stop();
      *ptr = mem;
      return 1;
    }
    ::free(mem);
  }
  *ptr = 0;
  return 0;
}

extern "C" int checkpointing_dump_file_descriptors(void* ptr) {
  if ( ptr ) {
    FileMap m;
    int lvl = mtcp_get_debug_level();
    mtcp_set_debug_level(MTCP_DEBUG);
    FileMemPrintHandler rd(ptr);
    m.analyze(rd.start());
    rd.stop();
    mtcp_set_debug_level(lvl);
    return 1;
  }
  return 0;
}

extern "C" int checkpointing_set_file_descriptors(void* ptr) {
  if ( ptr ) {
    FileMap m;
    FileReadHandler rd(ptr, true);
    if ( m.analyze(rd.start()) > 0 ) {
      rd.stop();
      return 1;
    }
  }
  return 0;
}

/// After a restart allow to set environment etc. from stdin.
extern "C" int checkpointing_update_environment() {
  return checkpointing_sys_set_environment(&chkpt_sys);
}

/// Set the printout level for the checkpoint/restore mechanism
extern "C" int checkpointing_set_print_level(int new_level) {
  int lvl = mtcp_get_debug_level();
  mtcp_set_debug_level(new_level);
  return lvl;
}
