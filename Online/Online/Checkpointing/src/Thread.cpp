#include "Checkpointing/MainThread.h"
#include "Checkpointing/Thread.h"
#include "Checkpointing/MB.h"
#include "Checkpointing/ThreadsLock.h"
#include "Checkpointing/Static.h"
#include "Checkpointing/SysCalls.h"
#include "Checkpointing.h"
#include "Restore.h"
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <sched.h>
#include <unistd.h>

using namespace Checkpointing;

/* This allocation hack will work only if calls to mtcp_sys_get_thread_area
 * and mtcp_sys_get_thread_area are both inside the same file (mtcp.c).
 * This is all because get_thread_area is not implemented for x86_64.
 */
static unsigned long int myinfo_gs;

# define mtcp_sys_get_thread_area(uinfo)                                \
  ( mtcp_inline_syscall(arch_prctl,2,ARCH_GET_FS,                       \
                        (unsigned long int)(&(((struct user_desc *)uinfo)->base_addr))), \
    mtcp_inline_syscall(arch_prctl,2,ARCH_GET_GS, &myinfo_gs)           \
    )
# define mtcp_sys_set_thread_area_base(base)                  \
  ( mtcp_inline_syscall(arch_prctl,2,ARCH_SET_FS,             \
                        *(unsigned long*)base),               \
    mtcp_inline_syscall(arch_prctl,2,ARCH_SET_GS, myinfo_gs)	\
    )

# define mtcp_sys_set_thread_area(uinfo)                                \
  ( mtcp_inline_syscall(arch_prctl,2,ARCH_SET_FS,                       \
                        *(unsigned long int *)&(((struct user_desc *)uinfo)->base_addr)), \
    mtcp_inline_syscall(arch_prctl,2,ARCH_SET_GS, myinfo_gs)            \
    )

static Thread     *s_threads = 0;
static FutexState  s_restoreinprog;
static sigset_t    s_sigpending_global;    // pending signals for the process
#define SIG_THREAD_CANCEL 32 // SIGRTMIN:   used by pthread library
#define SIG_THREAD_SETXID 33 // SIGRTMIN+1: used by pthread library

using namespace Checkpointing;

typedef void (*sigaction_func_t) (int, siginfo_t *, void *);

static sigaction_func_t s_thrCancel = 0;           /// Original pthread cancel handler (default=SIG_DFL)
static sigaction_func_t s_thrSetxid = 0;           /// Original pthread handler to change the threads' uid/gid (default=SIG_DFL)

static int _real_sigaction(int signum, const struct sigaction *act, struct sigaction *oact)  {
  int result = mtcp_sys_sigaction(signum, act, oact);
  return result;
}

static const char* _thread_type(bool val)  {
  return val ? "[MOTHER]" : "[CHILD] ";
}

static void _thread_cancel_action(int signum, siginfo_t *si, void *ctx)   {
  if ( s_thrCancel ) {
    Thread *t = checkpointing_thread_current();
    if ( t )   {
      // NOTE: Important for CHECKPOINTING and FORKING using syscall(SYS_fork):
      //
      // We have to set the correct signal pid (si->si_pid). 
      // The pid stored in the pthread tables may since a long time be dead.... 
      // Otherwise the pthread library does not allow us to perform the 
      // proper stack unwind.
      //
      //char* tls_base = t->tlsBase();
      //pid_t pid = *(pid_t*) (tls_base + TLS_PID_OFFSET);
      //si->si_pid = pid;
      s_thrCancel(signum,si,ctx);
      return;
    }
  }
  mtcp_output(MTCP_INFO,"Signal:%d Ignore pthread cancel action.\n",signum);  
}

static void _thread_setxid_action(int signum, siginfo_t *si, void *ctx)   {
  if ( s_thrSetxid )    {
    Thread *t = Thread::current();
    char* tls_base = t->tlsBase();
    pid_t pid = *(pid_t*) (tls_base + TLS_PID_OFFSET);
    si->si_pid = pid;
    s_thrSetxid(signum,si,ctx);
    return;
  }
  mtcp_output(MTCP_INFO,"Signal:%d Ignore pthread cancel action.\n",signum);  
}
/**  Set the thread's STOPSIGNAL handler.  Threads are sent STOPSIGNAL when they are to suspend execution the application, save
 *  their state and wait for the checkpointhread to write the checkpoint file.
 *
 *    Output:
 *
 *	Calling thread will call _thread_stop_action() when sent a STOPSIGNAL
 */
static void _thread_stop_action(int /* signum */, siginfo_t */* si */, void * /* ctx */)   {
  Thread *thread = checkpointing_thread_current();
  thread->stop();
}

/* Grow the stack by kbStack*1024 so that large stack is allocated on restart
 * The kernel won't do it automatically for us any more, since it thinks
 * the stack is in a different place after restart.
 */
/* growstackValue is volatile so compiler doesn't optimize away growstack */
static volatile unsigned int growstackValue = 0;
static void growstack(int kbStack) {
  const int kBincrement = 1024;
  char array[kBincrement * 1024];
  volatile int dummy_value = 1; /* Again, try to prevent compiler optimization */
  array[0] = 0;
  if (kbStack > 0)
    growstack(kbStack - kBincrement);
  else
    growstackValue++;
  if ( dummy_value && array[0]==0 ) {}
}

extern "C" pid_t __getpid()  {
  return mtcp_sys_getpid();
}
__asm__ (".global getpid ; .type getpid,@function ; getpid = __getpid");

extern "C" int __getppid()  {
  return mtcp_sys_getppid();
}
__asm__ (".global getppid ; .type getppid,@function ; getppid = __getppid");

extern "C" int __open(const char* pathname, int flags, mode_t mode)  {
  int rc = mtcp_sys_open(pathname,flags,mode);
  mtcp_output(MTCP_DEBUG,"Open file: fd:%d=%s with flags: %08X Mode:%X\n",rc,pathname,flags,mode);
  return rc;
}
__asm__ (".global open ; .type open,@function ; open = __open");

extern "C" int __creat(const char* pathname, mode_t mode)  {
  int rc = mtcp_sys_creat(pathname,mode);
  mtcp_output(MTCP_DEBUG,"Create file: fd:%d=%s with Mode:%X\n",rc,pathname,mode);
  return rc;
}
__asm__ (".global creat ; .type creat,@function ; creat = __creat");

extern "C" int __close(int fd)  {
  mtcp_output(MTCP_DEBUG,"Close file: %d\n",fd);
  return mtcp_sys_close(fd);
}
__asm__ (".global close ; .type close,@function ; close = __close");

/// pthread_create overload to install cancellation handlers
extern "C" int checkpointing_pthread_create(void** p1, void* p2, void* (*user_func)(void*), void* user_arg)  {
  // ----------------------------------------------------------------------
  // NOTE: Double trampolin
  // checkpointing_pthread_create -> MainThread::create_thread -> pthread_create
  //
  // We want to install out own thread function directly in pthread_create.
  // If not, we cannot hook into the cancellation handlers and everything
  // related to pthread_join, pthread_exit or simple thread exists
  // could only be handled with difficulty, because after thr return of the
  // user action function the pthread library immediately exits the fiber
  // and no additional handling (like e.g. cleanung up mtcp structures would
  // be impossible.  
  // ----------------------------------------------------------------------
  //mtcp_output(MTCP_DEBUG,"pthread_create(%p,%p,%p,%p)\n",p1,p2,user_func,user_arg);
  int ret = MainThread::create_thread(p1, p2, user_func, user_arg);
  // ----------------------------------------------------------------------
  // This is a bit a hack. In principle we would have to update the
  // pthread_t cache in allocatestack.c in glibc/nptl or 
  // provide the thread stack ourselves, which is a horror.
  //
  // This would have to be done in checkpointing_fork_process()!
  // Though this also does the job and tricks libpthread_db and assigns
  // the required threads to the debugger's scan function.
  // ----------------------------------------------------------------------
  THREAD_SET_PID(*p1,mtcp_sys_getpid());
  return ret;
}
__asm__ (".global __pthread_create ; .type __pthread_create,@function ; __pthread_create = checkpointing_pthread_create");
__asm__ (".global pthread_create ; .type pthread_create,@function ; pthread_create = checkpointing_pthread_create");

/****************************************************************************
 *									     
 *  This is our clone system call wrapper				     
 *									     
 *    Note:								     
 *									     
 *     -pthread_create eventually calls __clone to create threads	     
 *     -The argument id the pthread_t - the thread handle.
 *     -It uses flags = 0x3D0F00:					     
 *	CLONE_VM             = VM shared between processes			     
 *      CLONE_FS             = fs info shared between processes (root, cwd, umask) 
 *      CLONE_FILES          = open files shared between processes (fd table)	     
 *      CLONE_SIGHAND        = signal handlers and blocked signals shared	     
 *	 			 (sigaction common to parent and child)	     
 *      CLONE_THREAD         = add to same thread group			     
 *      CLONE_SYSVSEM        = share system V SEM_UNDO semantics		     
 *      CLONE_SETTLS         = create a new TLS for the child from newtls parameter
 *	CLONE_PARENT_SETTID  = set the TID in the parent (before MM copy)    
 *	CLONE_CHILD_CLEARTID = clear the TID in the child and do	     
 *				 futex wake at that address		     
 *	      CLONE_DETACHED = create clone detached			     
 *									     
 *****************************************************************************/
extern "C" int __clone(int (*fn) (void *arg), void *child_stack, int flags, void *arg,
                       int *parent_tidptr, struct user_desc *newtls, int *child_tidptr)
{
  int rc;
  Thread*  mother = chkpt_sys.motherofall;
#if 0
  mtcp_output(MTCP_DEBUG,"clone wrapper: %p(%p) ptid:%p ctid:%p tls:%p %s [%p]\n", 
              fn, arg, parent_tidptr, child_tidptr, newtls, arg==mother ? "MOTHER_OF_ALL" : "",mother);
#endif
  if ( mother != 0 ) {
    // Track this thread
    Thread *t = new Thread;

    t->m_startFunc  = fn;   // this is the user's function
    t->m_startArg   = arg;  // ... and the parameter
    // Save exactly what the caller is supplying
    t->m_cloneFlags = flags;
    t->m_pParentTID = parent_tidptr;
    t->m_pGivenTID  = child_tidptr;
    t->m_parent     = Thread::current();
    t->state.value  = ST_RUNDISABLED;

    // We need the CLEARTID feature so we can detect when the thread has exited
    // So if the caller doesn't want it, we enable it
    // Retain what the caller originally gave us so we can pass the tid back
    if (!(flags & CLONE_CHILD_CLEARTID)) {
      child_tidptr = &t->m_childTID;
    }
    t->m_pActualTID = child_tidptr;
    // Alter call parameters, forcing CLEARTID and make it call the wrapper routine
    flags |= CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID;
    fn  = checkpointing_thread_main;
    arg = t;
  }

  // Now create the thread
  rc = (*CLONE_CALL) (fn, child_stack, flags, arg, parent_tidptr, (user_desc*)newtls, child_tidptr);
  if (rc < 0) {
    mtcp_output(MTCP_ERROR,"wrapper clone*: clone rc=%d, errno=%d\n", rc, errno);
  }
  return (rc);
}
__asm__ (".global clone ; .type clone,@function ; clone = __clone");

/**  Thread execution wrapper
 *
 *   Simply call the object function.
 */
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_thread_main(void *threadv)  {
  Thread *const thread = (Thread*)threadv;
  return thread->Main();
}

/// Entry point to restart thread after a fork.
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_thread_restart_main(void* arg) {
  Thread::RestartArgs* a = (Thread::RestartArgs*)arg;
  mtcp_output(MTCP_DEBUG,"restartMain: thread: %p.\n",a->self);
  return checkpointing_thread_restart(a->self,0);
}

/// Access to the overall thread queue
STATIC(Thread*) CHECKPOINTING_NAMESPACE::checkpointing_thread_queue() {
  return s_threads;
}

STATIC(Thread*) CHECKPOINTING_NAMESPACE::checkpointing_thread_current()  {
  int tid = mtcp_sys_kernel_gettid();
  checkpointing_threads_lock();
  for(Thread* thread = s_threads; thread != 0; thread = thread->next) {
    if (thread->m_tid == tid) {
      checkpointing_threads_unlock();
      return thread;
    }
  }
  mtcp_output(MTCP_INFO,"Thread::current: can't find thread id %d\n", tid);
  return 0;
}

/// Count the number of threads known
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_thread_count()  {
  int cnt = 0;
  for(Thread* thread = s_threads; thread != 0; thread = thread->next) ++cnt;
  return cnt;
}

/// Standard constructor
Thread::Thread() {
  ::memset(this,0,sizeof(Thread));
  m_restartArgs.self = this;
}

/// Standard Destructor
Thread::~Thread() {
  state.destroy();
}

/// Handle thread cleanup in case of cancellation of thread_exit
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_thread_cleanup_handler(void* thread) {
  Thread *t = checkpointing_thread_current();
  if ( thread != t )  {
    mtcp_output(MTCP_ERROR,"+++ Thread CLEANUP inconstency: current: %p <-> %d\n",t,thread);
  }
  if ( t )   {
    char* base = t->tlsBase();
    pid_t pid = THREAD_GET_PID(base);
    pid_t tid = THREAD_GET_TID(base);
    ThreadsLock lock;
    checkpointing_thread_cleanup(t);
    int cnt = checkpointing_thread_count();
    mtcp_output(MTCP_INFO,
                "+++ Thread CLEANUP action [%p] "
                "TLS->pid:%d tid:%d tid:%d otid:%d Checkpoint pid:%d Mother pid:%d Count:%d\n",
                t,pid,tid,t->m_tid,t->m_originalTID,chkpt_sys.chkptPID,chkpt_sys.motherPID,cnt);
  }
}

/// Link thread struct to the lists and finish filling the thread structure.
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_thread_initialize(Thread* t)  {
  // Save the thread's ID number and put in threads list so we can look it up
  // Set state to disable checkpointing so checkpointer won't race between 
  // adding to list and setting up handler
  t->m_tid = mtcp_sys_kernel_gettid();
  t->m_originalTID      = t->m_tid;
  t->m_restartArgs.tid  = t->m_tid;
  t->m_restartArgs.self = t;
  {
    ThreadsLock lock;
    if ((t->next = s_threads) != 0) {
      t->next->prev = &t->next;
    }
    t->prev = &s_threads;
    s_threads = t;
    Thread* parent = t->m_parent;
    if ( parent ) {
      t->m_siblings = parent->m_children;
      parent->m_children = t;
    }
  }
  // Set up signal handler so we can interrupt the thread for checkpointing 
  checkpointing_thread_setup_signals();
}

///  Get current thread struct pointer
Thread *Thread::current()   {
  return checkpointing_thread_current();
}

/// Save the system info of the process
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_thread_save_sys_info() {
  chkpt_sys.saved_break = (void*) mtcp_sys_brk(0);  // kernel returns mm->brk when passed zero
  // Do this once, same for all threads.  But restore for each thread.
  if (mtcp_have_thread_sysinfo_offset())
    chkpt_sys.sysInfo = mtcp_get_thread_sysinfo();
  mtcp_output(MTCP_INFO,"save_sys_info: saved_break=%p\n",chkpt_sys.saved_break);
}

/** Thread has exited - unlink it from lists and free struct
 *
 *	thread removed from 'threads' list and motherofall tree
 *	thread pointer no longer valid
 *	checkpointer woken if waiting for this thread
 */
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_thread_cleanup(Thread* thr)   {
  Thread **lthread, *xthread;
  //ThreadsLock lock;
  mtcp_output(MTCP_DEBUG,"cleanup: %p->tid:%d\n",thr,thr->m_tid);

  // Remove thread block from 'threads' list
  if ( (*(thr->prev) = thr->next) != 0) {
    thr->next->prev = thr->prev;
  }
  // Remove thread block from parent's list of children
  Thread* parent = thr->m_parent;
  if (parent) {
    for(lthread = &parent->m_children; (xthread = *lthread) != thr; lthread = &xthread->m_siblings) {}
    *lthread = xthread->m_siblings;

    // If this thread has children, give them to its parent
    while((xthread = thr->m_children) != 0) {
      thr->m_children   = xthread->m_siblings;
      xthread->m_siblings = parent->m_children;
      parent->m_children  = xthread;
    }
  }
  else {
    while((xthread = thr->m_children) != 0) {
      thr->m_children   = xthread->m_siblings;
      xthread->m_siblings = chkpt_sys.motherofall;
      chkpt_sys.motherofall = xthread;
    }
  }
  // If checkpointer is waiting for us, wake it to see this thread no longer in list
  thr->state.wake();
  delete thr;
  return 1;
}

///  Call this when it's OK to checkpoint
WEAK(int) Thread::instrument()     {
 again:
  switch ( state.value ) {    
  case ST_RUNDISABLED:
    // Thread was running normally with checkpointing disabled.  Enable checkpointing then just return.
    if (!state.set(ST_RUNENABLED, ST_RUNDISABLED)) goto again;
    return (0);

  case ST_RUNENABLED:
    // Thread was running normally with checkpointing already enabled.  So just return as is. */
    return (1);

  case ST_SIGDISABLED:
    // Thread was running with checkpointing disabled, but the checkpointhread wants to 
    // write a checkpoint.  So mark the thread as having checkpointing enabled, then just 
    // 'manually' call the signal handler as if the signal to suspend were just sent.
    if (!state.set(ST_SIGENABLED, ST_SIGDISABLED)) goto again;
    this->stop();
    return (0);

  case ST_SIGENABLED:
    // Thread is running with checkpointing enabled, but the checkpointhread wants to 
    // write a checkpoint and has sent a signal telling the thread to call 'Thread::stop()'.
    // So we'll just keep going as is until the signal is actually delivered.
    return (1);

  case ST_CKPNTHREAD:
    // Thread is the checkpointhread so we just ignore the call (from threadcloned routine).
    return (-1);
      
  default:
    // How'd we get here?
    mtcp_output(MTCP_FATAL,"How the hell did we get here?\n");
    return (0); /* NOTREACHED : stop compiler warning */
  }
}

/** Main thread execution function
 *
 *  This routine is called (via clone) as the top-level routine of a thread that we are tracking.
 *  It fills in remaining items of our thread struct, calls the user function, then cleans up the thread struct before exiting.
 */
WEAK(int) Thread::Main()  {
  int rc;
  char* tls_base=0;
  pid_t motherpid=0, pid=0, tid=0;
  checkpointing_thread_initialize(this);
  checkpointing_thread_restore_sigactions();

  // The new TLS should have the process ID in place at TLS_PID_OFFSET 
  // This is a verification step and is therefore optional as such 
  tls_base = tlsBase();
  motherpid = chkpt_sys.motherPID;
  pid = THREAD_GET_PID(tls_base);
  tid = THREAD_GET_TID(tls_base);
  if (pid != motherpid && pid != chkpt_sys.chkptPID && pid != (pid_t)-1) {
    mtcp_output(MTCP_ERROR,"Thread_Main: tid:%d opid:%d pid:%d, must match tls pid %X/%d or %X/%d\n", 
                m_tid, m_originalTID, motherpid, pid, pid, chkpt_sys.chkptPID, chkpt_sys.chkptPID);
    for (rc = 0; rc < 256; rc += 4) {
      int data = *(pid_t*) (tls_base + rc);
      mtcp_output(MTCP_ERROR|MTCP_NO_HEADER,"   %d: %X", rc, data);
      if ((rc & 31) == 28) mtcp_output(MTCP_ERROR|MTCP_NO_HEADER,"\n");
    }
    //mtcp_output(MTCP_FATAL,"Fatal inconsistency in Thread::Main. tls_pid:%d\n",pid);
    mtcp_output(MTCP_ERROR,"Thread_Main: Fatal inconsistency in TLS:pid:%d\n",pid);
  }

  // If the caller wants the child tid but didn't have CLEARTID, pass the tid back to it
  if ((m_cloneFlags & (CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID)) == CLONE_CHILD_SETTID) {
    *m_pGivenTID = m_childTID;
  }
  // Instrument thread to be "stopable"
  this->instrument();
  mtcp_output(MTCP_DEBUG,"Thread_Main: Starting pid:%d tid:%d TLS->pid:%d tid:%d otid:%d thread %p, call %p(%p)\n",
              motherpid, m_tid, pid, tid, m_originalTID, this, m_startFunc, m_startArg);
  // Call the user's function for whatever processing they want done
  rc = (*m_startFunc)(m_startArg);
  //
  // This part is NEVER REACHED, because the pthread library already exited the thread.
  // To to the necessary cleanup on thread CANCEL and thread-returns, we have installed
  // a trampolin created by the patched call to pthread_create!
  //
  // Keep the compiler happy: Return the user's status as the exit code
  return rc;
}

/// Likewise, disable checkpointing
WEAK(int) Thread::wait()   {
 again:
  switch ( state.value ) {
  case ST_RUNDISABLED:
  case ST_SIGDISABLED:
    return (0);
  case ST_RUNENABLED:
    if (!state.set(ST_RUNDISABLED, ST_RUNENABLED)) goto again;
    return (1);
  case ST_SIGENABLED:
    this->stop();
    goto again;
  default:
    mtcp_output(MTCP_FATAL,"Fatal inconsistency in Thread::wait. tid:%d State:%d\n",m_tid,state.value);
    return (0); /* NOTREACHED : stop compiler warning */
  }
}

/// Stop all threads in order to allow for a consistent checkpoint.
WEAK(int) Thread::stop()  {
  int rc;

  mtcp_output(MTCP_DEBUG,"stop: tid %d returns to %p\n", mtcp_sys_kernel_gettid (), __builtin_return_address (0));
  checkpointing_thread_setup_signals();  // re-establish in case of another STOPSIGNAL so we don't abort by default

  // If this is checkpoint thread - exit immidiately
  if( state.value == ST_CKPNTHREAD ){
    return 1;
  }
  
  if (state.set(ST_SUSPINPROG, ST_SIGENABLED)) {  // make sure we don't get called twice for same thread
    static int is_first_checkpoint = 1;
    ::sigpending (&m_sigpending); // save pending signals
    checkpointing_thread_save_signals(this);                       // save signal state (and block signal delivery)
    this->saveTLS();                                               // save thread local storage state
    // Grow stack only on first ckpt.  Kernel agrees this is main stack and
    // will mmap it.  On second ckpt and later, we would segfault if we tried
    // to grow the former stack beyond the portion that is already mmap'ed.
    //
    if (this == chkpt_sys.motherofall) {
      static char *orig_stack_ptr;
      int kbStack = 2048;
      if (is_first_checkpoint) {
        orig_stack_ptr = (char*)&kbStack;
        is_first_checkpoint = 0;
        mtcp_output(MTCP_DEBUG,"stop: temp. grow main stack by %d kilobytes");
        growstack(kbStack);
      } else if (orig_stack_ptr - (char *)&kbStack > 3 * kbStack*1024 / 4) {
        mtcp_output(MTCP_WARNING,"WARNING:  Stack within %d bytes of end;\n"
                    "  Consider increasing 'kbStack' at line %d of mtcp/%s\n",
                    kbStack*1024/4, __LINE__-9, __FILE__);
      }
    }

    ///JA: new code ported from v54b
    rc = ::getcontext (&m_savctx);
    if (rc < 0) {
      mtcp_output(MTCP_FATAL,"stop: getcontext rc %d errno %d\n", rc, errno);
    }
    mtcp_output(MTCP_DEBUG,"stop: context: %p\n",m_savctx.SAVEDSP);
    if ( s_restoreinprog.value == 0 )  {
      // We are the original process and all context is saved. s_restoreinprog is 0;
      // wait for ckpt thread to write ckpt, and resume.

      WMB; // matched by RMB in checkpointhread

      // Next comes the first time we use the old stack.
      // Tell the checkpoint thread that we're all saved away
      if (!state.set(ST_SUSPENDED, ST_SUSPINPROG))  { // tell checkpointhread all our context is saved
        mtcp_output(MTCP_FATAL,"Fatal inconsistency in Thread::stop. tid:%d State:%d SUSPENDED:%d SUSPINPROG:%d\n",
                    m_tid,state.value, ST_SUSPENDED, ST_SUSPINPROG);
      }
      state.wake();
      // Then we wait for the checkpoint thread to write the checkpoint file then wake us up
      mtcp_output(MTCP_DEBUG,"stop: tid:%d suspending\n",m_tid);
      while ( state.value == ST_SUSPENDED) {
        state.wait(ST_SUSPENDED);
      }
    }
    else {
      // Else s_restoreinprog >= 1;  This stuff executes to do a restart
      mtcp_output(MTCP_DEBUG,"stop(restoreinprog:%d): Change state from %d to Running[%d]/Suspended[%d]\n",
                  s_restoreinprog.value,state.value,ST_RUNENABLED,ST_SUSPENDED);
      if ( !state.set(ST_RUNENABLED, ST_SUSPENDED) )  {
        mtcp_output(MTCP_ERROR,"ERROR: Thread::stop: checkpoint was written when thread in SUSPENDED state\n");
      }
      waitForRestore();
      mtcp_output(MTCP_DEBUG,"stop: tid:%d restored. State:%d\n", m_tid,state.value);
    }
  }
  mtcp_output(MTCP_DEBUG,"stop: tid:%d return to address %p\n",
              (int)mtcp_sys_kernel_gettid(),(void*)__builtin_return_address(0));
  return 1;
}

/// On restore wait until all threads are ready.
WEAK(void) Thread::waitForRestore()    {
  int rip, pid;
  do  {
    rip = s_restoreinprog.value;     // dec number of threads cloned but not completed longjmp'ing
  } while (!s_restoreinprog.set(rip-1, rip));
  if (--rip == 0) {
    /* raise the signals which were pending for the entire process at the time
     * of checkpoint. It is assumed that if a signal is pending for all threads
     * including the ckpt-thread, then it was sent to the process as opposed to
     * sent to individual threads.
     */
    pid = mtcp_sys_getpid();
    for (int i=1; i<NSIG; ++i) {
      if (sigismember(&s_sigpending_global, i) == 1) {
        mtcp_output(MTCP_INFO,"waitForRestore: Send signal %d to pid:%d\n",i,pid);
        kill(pid,i);
      }
    }
    s_restoreinprog.wake(999999999);  // if this was last of all, wake everyone up
    checkpointing_threads_unlock(); // ... and release the thread list
    mtcp_output(MTCP_INFO,"waitForRestore%s restoreinprog:%d State:%d Complete -> Continue.\n",
                _thread_type(this==chkpt_sys.motherofall), s_restoreinprog.value, state.value);
  }
  else {
    // otherwise, wait for last of all to wake this one up
    while ((rip = s_restoreinprog.value) > 0) {
      s_restoreinprog.wait(rip);
    }
  }
  mtcp_output(MTCP_INFO,"waitForRestore%s thread restore finished.\n",
              _thread_type(this==chkpt_sys.motherofall));
}

///  Save state necessary for TLS restore
WEAK(void) Thread::saveTLS()    {
  ::memset(&m_tls, 0, sizeof(m_tls));
  int rc, i = this->TLSSEGREG / 8;
  m_tls[0].entry_number = i;
  rc = mtcp_sys_get_thread_area (&m_tls[0]);
  if (rc < 0) {
    mtcp_output(MTCP_FATAL,"thread: error saving GDT TLS entry[%d]: %s\n",
                i,::strerror(mtcp_sys_errno));
  }
}

/// Access the base address of the thread local storage.
WEAK(char*) Thread::tlsBase()   {
  mtcp_segreg_t TLSSEGREG;
  struct user_desc gdtentrytls;
  __asm__ volatile ("movl %%fs,%0" : "=q" (TLSSEGREG)); /* q = a,b,c,d for i386; 8 low bits of r class reg for x86_64 */
  gdtentrytls.entry_number = TLSSEGREG / 8;
  if ( mtcp_sys_get_thread_area(&gdtentrytls) < 0 ) {
    mtcp_output(MTCP_FATAL,"thread: error getting GDT TLS entry: %s\n",strerror(mtcp_sys_errno));
  }
  unsigned long *val = (unsigned long*)&gdtentrytls.base_addr;
  return *(char**)val;
}

/// Setup the signal handling to stop/restart threads
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_thread_setup_signals()  {
  struct sigaction old, act;
  sigemptyset(&act.sa_mask);
  act.sa_sigaction = _thread_stop_action;
  act.sa_flags = SA_SIGINFO;
  // NOTE: Do NOT call mtcp_sigaction here! The sigaction.sa_restorer 
  //       is only set by __libc_sigaction !
  if ( sigaction(STOPSIGNAL,&act,&old) < 0 )  {
    mtcp_output(MTCP_FATAL,"setupSignals: error setting up signal handler: %s\n",strerror (errno));
  }
  if ((old.sa_handler != SIG_IGN) && (old.sa_handler != SIG_DFL) && (old.sa_sigaction != _thread_stop_action)) {
    mtcp_output(MTCP_FATAL,"setupSignals: signal handler %d already.\n", STOPSIGNAL);
  }
}

///  Save signal handlers and block signal delivery
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_thread_save_signals(Thread* thr)  {
  sigset_t blockall;
  mtcp_output(MTCP_DEBUG,"saveSignals: pid:%d tid:%d\n",mtcp_sys_getpid(),thr->m_tid);
  // Block signal delivery first so signal handlers can't change state of signal handlers on us
  sigfillset(&blockall);
  sigdelset(&blockall,STOPSIGNAL);

  if (sigprocmask(SIG_SETMASK,&blockall,&thr->m_sigblockmask) < 0) {
    mtcp_output(MTCP_FATAL,"Failed to set signal mask in thread. tid:%d\n",thr->m_tid);
  }
  // Now save all the signal handlers
  for (int i=1; i<NSIG; ++i) {
    struct sigaction* a = &thr->m_sigactions[i];
    if (_real_sigaction(i,0,a) < 0) {
      if (errno == EINVAL)  {
        ::memset(a,0,sizeof(struct sigaction));
        continue;
      }
      mtcp_output(MTCP_FATAL,"saveSignals: error saving signal %d action: %s\n", i, ::strerror(errno));
    }
    else if ( i == SIG_THREAD_CANCEL && a->sa_sigaction != _thread_cancel_action )   {
      s_thrCancel = a->sa_sigaction;
    }
    else if ( i == SIG_THREAD_SETXID && a->sa_sigaction != _thread_setxid_action )   {
      s_thrSetxid = a->sa_sigaction;      
    }
  }
  mtcp_output(MTCP_INFO,"saveSignals: Thread %p: %d saved signal actions ...\n",thr,thr->m_tid);
}  

/**  Restore the GDT entries that are part of a thread's state
 *
 *  The kernel provides set_thread_area system call for a thread to 
 *  alter a particular range of GDT entries, and it switches
 *  those entries on a per-thread basis.  So from our perspective, 
 *  this is per-thread state that is saved outside user
 *  addressable memory that must be manually saved.
 */
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_thread_restore_tls(Thread* thr) {
  pid_t motherpid = chkpt_sys.motherPID;
  const char* thr_typ = _thread_type(thr == chkpt_sys.motherofall);
  unsigned long* base = (unsigned long*)&thr->m_tls[0].base_addr;
  pid_t  opid = THREAD_GET_PID(*base);

  // The assumption that this points to the pid was checked by that tls_pid crap near the beginning
  THREAD_SET_PID(*base,motherpid);
  // Restore all three areas
  int rc = mtcp_sys_set_thread_area_base(base);
  if (rc < 0) {
    mtcp_output(MTCP_FATAL,"restoreTLS%s: error %d restoring TLS entry[%d]\n",
                thr_typ,mtcp_sys_errno,thr->m_tls[0].entry_number);
  }
  thr->m_tid = mtcp_sys_kernel_gettid();
  mtcp_output(MTCP_INFO,"restoreTLS%s: pid:%d tid:%d opid:%d otid:%d restoring TLS entry[%d] thread:%p\n",
              thr_typ,motherpid,thr->m_tid,opid,thr->m_originalTID,thr->m_tls[0].entry_number,thr);
}

STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_thread_restore_signals(Thread* thr) {
  const char* thr_typ = _thread_type(thr==chkpt_sys.motherofall);
  mtcp_output(MTCP_DEBUG,"restoreSIG%s: pid:%d tid:%d\n",
              thr_typ,mtcp_sys_getpid(),thr->m_tid);

  if (thr == chkpt_sys.motherofall) {
    // Compute the set of signals which was pending for all the threads at the
    // time of checkpoint. This is a heuristic to compute the set of signals
    // which were pending for the entire process at the time of checkpoint.
    sigset_t tmp;
    sigfillset(&tmp);
    for (Thread* t = s_threads; t != 0; t = t->next) {
      sigandset(&s_sigpending_global, &tmp, &t->m_sigpending);
      tmp = s_sigpending_global;
    }
  }

  if ( sigprocmask(SIG_SETMASK, &thr->m_sigblockmask, 0) >= 0 ) {
    // Raise the signals which were pending for only this thread at the time of checkpoint.
    for (int i = 1; i<NSIG; ++i ) {
      if (sigismember(&thr->m_sigpending, i)  == 1  && 
          sigismember(&thr->m_sigblockmask, i) == 1 &&
          sigismember(&s_sigpending_global, i) == 0) {
        raise(i);
      }
    }
    return;
  }
  mtcp_output(MTCP_FATAL,"Failed to restore signals. pid:%d tid:%d\n",mtcp_sys_getpid(),thr->m_tid);
}

STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_thread_restore_sigactions() {
  for (int i=1; i<NSIG; ++i ) {
    struct sigaction act, *a = &chkpt_sys.motherofall->m_sigactions[i];
    if (_real_sigaction(i,0,&act) < 0) {
      if (errno == EINVAL) ::memset(&act,0,sizeof(act));
    }
    if ( i != STOPSIGNAL ) {
      void** mask = (void**)&a->sa_mask;
      if ( i == SIG_THREAD_CANCEL && (a->sa_sigaction!=_thread_cancel_action || s_thrCancel==0) )  {
        if (a->sa_sigaction!=_thread_cancel_action) s_thrCancel = act.sa_sigaction;
        *a = act;
        a->sa_sigaction = _thread_cancel_action;
      }
      else if ( i == SIG_THREAD_SETXID && (a->sa_sigaction!=_thread_setxid_action || s_thrSetxid==0) )  {
        if (a->sa_sigaction!=_thread_setxid_action) s_thrSetxid  = act.sa_sigaction;
        *a = act;
        a->sa_sigaction = _thread_setxid_action;
      }
      if ( act.sa_sigaction != a->sa_sigaction ) {
        if ( _real_sigaction(i,a,0) < 0) {
          mtcp_output(MTCP_FATAL,"restoreACT: Failed to set stored sigaction[%d]:%p. [%s]\n",
                      i,a->sa_sigaction,::strerror(errno));
        }
        mtcp_output(MTCP_INFO,"restoreACT: Restore action [sig:%d] -> %p Flags:%X Mask:%p\n",
                    i, a->sa_sigaction, a->sa_flags, *mask);
      }
      else if ( 0 != act.sa_sigaction && SIG_DFL != act.sa_handler && SIG_IGN != act.sa_handler ) {
        //mtcp_output(MTCP_DEBUG,"restoreSigActions: Skip... action [sig:%d] -> %p Flags:%X Mask:%p\n",
        //	    i, a->sa_sigaction, a->sa_flags, *mask);
      }
    }
  }
}

/// Restart thread and all depending children 
/// -- after checkpointing and on restart.
/// -- after a fork
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_thread_restart(Thread* thr, int force_context)     {
  pid_t tid = mtcp_sys_kernel_gettid();
  const char* thr_typ = _thread_type(thr == chkpt_sys.motherofall);
  mtcp_output(MTCP_DEBUG,"restart%s pid:%d otid:%d thread:%p ctxt:%p\n",
              thr_typ,chkpt_sys.motherPID,thr->m_tid,thr,thr->m_savctx.SAVEDSP);

  checkpointing_thread_restore_tls(thr);
  checkpointing_thread_setup_signals();
  checkpointing_thread_restore_signals(thr);

  int cnt = 0, rip = 0;
  Thread* child = 0;

  // First loop over all children and increase the restore count. 
  // Restore is finished once this counter is 0
  for (child = thr->m_children; child != 0; child = child->m_siblings)     {
    // Increment number of threads created but haven't completed their longjmp
    do   {
      rip = s_restoreinprog.value;
    } while (!s_restoreinprog.set(rip+1,rip));
  }
  mtcp_output(MTCP_INFO,"restart%s: pid:%d restoreinprog:%d thread:%p Start children...\n",
              thr_typ,chkpt_sys.motherPID,s_restoreinprog.value,thr);
  // No create the children
  for (child = thr->m_children; child != 0; child = child->m_siblings)     {
    // Create the thread so it can finish restoring itself.
    errno = -1;
    /* 
     * DMTCP needs to know m_originalTID of the thread being created by the 
     *  following clone() call.
     *
     * Threads are created by using syscall which is intercepted by DMTCP and
     *  the m_originalTID is sent to DMTCP as a field of MtcpRestartThreadArg
     *  structure. DMTCP will automatically extract the actual argument 
     *  (clone_arg->arg) from clone_arg and will pass it on to the real
     *  clone call.                                              (--Kapil)
     */
    tid = -1;
    int cloneFlags = (child->m_cloneFlags & ~CLONE_SETTLS) | CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID;
    child->m_restartArgs.self = child;
    child->m_restartArgs.tid  = child->m_originalTID;
    void* p = (void*)(child->m_savctx.SAVEDSP - 128);
    mtcp_output(MTCP_DEBUG,"restart%s: pid:%d CLONE: otid:%d %p %p State:%d\n",
                thr_typ, chkpt_sys.motherPID, child->m_originalTID, 
                child->m_pParentTID, child->m_pActualTID,
                child->state.value);

    tid = (*CLONE_CALL)(checkpointing_thread_restart_main,
                        p,  // -128 for red zone
                        cloneFlags,
                        &child->m_restartArgs, child->m_pParentTID, 0, child->m_pActualTID);
    if ( tid < 0 )  {
      mtcp_output(MTCP_FATAL,"restart: error %d recreating thread. Flags:%X SP:%p\n", 
                  errno, child->m_cloneFlags, child->m_savctx.SAVEDSP);
    }
#if 0
    mtcp_output(MTCP_INFO,"%s\t--> [%d] pid:%d CHILD otid:%d/%d State:%d Restart:Progress:%d Type:%d\n",
                thr_typ, cnt, chkpt_sys.motherPID,tid,thr->m_tid,thr->m_originalTID,child->state.value,
                restoreinprog.value,chkpt_sys.restart_type);
#endif
    ++cnt;
  }
  // All my children have been created, jump to the _thread_stop_action routine just after getcontext call
  // Note that if this is the restored checkpointhread, it jumps to the checkpointhread routine
  if (mtcp_have_thread_sysinfo_offset()) {
    mtcp_set_thread_sysinfo(chkpt_sys.sysInfo);
  }
  if ( thr == chkpt_sys.motherofall )  {
    // Restore signal actions
    checkpointing_thread_restore_sigactions();
    // Wait for restore to finish...
    while(s_restoreinprog.value>0)
      usleep(10000);
  }
  if ( thr != chkpt_sys.motherofall || force_context != 0 )  {
    mtcp_output(MTCP_INFO,"restart%s: setting context. Should not return.\n",thr_typ);
    ::setcontext(&thr->m_savctx); // Shouldn't return if everything works fine.
    mtcp_output(MTCP_ERROR,"restart%s: FATAL: returned from setcontext.\n",thr_typ);
  }
  mtcp_output(MTCP_INFO,"restart%s: All done: pid:%d tid:%d, orgTID:%d this:%p\n",
              thr_typ, chkpt_sys.motherPID,thr->m_tid,thr->m_originalTID,thr);
  return 0;
}

/// set_tid_address wrapper routine: save the new address of the tidptr that will get cleared when the thread exits
WEAK(long) Thread::set_tid_address ()   {
  mtcp_output(MTCP_DEBUG,"set_tid_address*: thread %p->tid:%d, tidptr %p\n",this,m_tid,&m_childTID);
  m_pActualTID = &m_childTID;  // save new tidptr so subsequent restore will create with new pointer
  mtcp_sys_set_tid_address(&m_childTID);
  return 0;                    // now we tell kernel to change it for the current thread
}

