#include "Checkpointing/MainThread.h"
#include "Checkpointing/Thread.h"
#include "Checkpointing/MB.h"

#include "Checkpointing/ThreadsLock.h"
#include "Checkpointing/Static.h"
#include "Checkpointing/SysCalls.h"
#include "Checkpointing.h"
#include "linux/futex.h"
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <unistd.h>
#include <termios.h>

using namespace Checkpointing;

/* This allocation hack will work only if calls to mtcp_sys_get_thread_area
 * and mtcp_sys_get_thread_area are both inside the same file (mtcp.c).
 * This is all because get_thread_area is not implemented for x86_64.
 */
static unsigned long int myinfo_gs;

# define mtcp_sys_get_thread_area(uinfo) \
    ( mtcp_inline_syscall(arch_prctl,2,ARCH_GET_FS, \
         (unsigned long int)(&(((struct user_desc *)uinfo)->base_addr))), \
      mtcp_inline_syscall(arch_prctl,2,ARCH_GET_GS, &myinfo_gs) \
    )
# define mtcp_sys_set_thread_area_base(base) \
    ( mtcp_inline_syscall(arch_prctl,2,ARCH_SET_FS, \
	*(unsigned long*)base), \
      mtcp_inline_syscall(arch_prctl,2,ARCH_SET_GS, myinfo_gs) \
    )

# define mtcp_sys_set_thread_area(uinfo) \
    ( mtcp_inline_syscall(arch_prctl,2,ARCH_SET_FS, \
	*(unsigned long int *)&(((struct user_desc *)uinfo)->base_addr)), \
      mtcp_inline_syscall(arch_prctl,2,ARCH_SET_GS, myinfo_gs) \
    )

#ifdef CHECKPOINT_TERMINAL
static struct      termios saved_termios;
static int         saved_termios_exists = 0;
#endif

static Thread     *s_threads = 0;
static FutexState  restoreinprog;
static int         s_restoring = 0;
static sigset_t    sigpending_global;    // pending signals for the process
static int         dmtcp_info_pid_virtualization_enabled = 0;
#define SIG_THREAD_CANCEL 32
#define SIG_THREAD_SETXID 33
//#define SIG_THREAD_CANCEL 34
//#define SIG_THREAD_SETXID 35

using namespace Checkpointing;

/**  Set the thread's STOPSIGNAL handler.  Threads are sent STOPSIGNAL when they are to suspend execution the application, save
 *  their state and wait for the checkpointhread to write the checkpoint file.
 *
 *    Output:
 *
 *	Calling thread will call stopthisthread () when sent a STOPSIGNAL
 */
static void stopthisthread(int /* signum */)  {
  Thread *thread = Thread::current();
  thread->stop();
}

/**  Thread execution wrapper
 *
 *   Simply call the object function.
 */
WEAK(int) Thread::staticMain(void *threadv)  {
  Thread *const thread = (Thread*)threadv;
  return thread->Main();
}

/** Entry point to restart thread after a fork.
 */
WEAK(int) Thread::restartMain(void* arg) {
  Thread::RestartArgs* a = (Thread::RestartArgs*)arg;
  mtcp_output(MTCP_DEBUG,"restartMain: thread: %p.\n",a->self);
  return a->self->restart(0);
}

typedef void (*sigaction_func_t) (int, siginfo_t *, void *);


static sigaction_func_t s_thrCancel = 0;           /// Original pthread cancel handler (default=SIG_DFL)
static sigaction_func_t s_thrSetxid = 0;           /// Original pthread handler to change the threads' uid/gid (default=SIG_DFL)

static sighandler_t _real_signal(int signum, sighandler_t handler){
  return signal(signum, handler);
}
static int _real_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact){
  return mtcp_sys_sigaction(signum, act, oldact);
}
static void _thread_cancel_action(int signum, siginfo_t *si, void *ctx)   {
  if ( s_thrCancel ) {
    Thread *t = Thread::current();
    char* tls_base = t->tlsBase();
    pid_t pid = *(pid_t*) (tls_base + TLS_PID_OFFSET);
    mtcp_output(MTCP_INFO,"Signal:%d Calling defererred pthread CANCEL action for pid:%d "
		"Thread TLS pid:%d tid:[%d-%d] Checkpoint pid:%d Mother pid:%d\n",
		signum,si->si_pid,pid,t->m_tid,t->m_originalTID,chkpt_sys.chkptPID,
		chkpt_sys.motherPID);
    //si->si_pid = chkpt_sys.chkptPID;
    si->si_pid = pid;
    s_thrCancel(signum,si,ctx);
    return;
  }
  mtcp_output(MTCP_INFO,"Signal:%d Ignore pthread cancel action.\n",signum);  
}

static void _thread_setxid_action(int signum, siginfo_t *si, void *ctx)   {
  if ( s_thrSetxid ) {
    Thread *t = Thread::current();
    char* tls_base = t->tlsBase();
    pid_t pid = *(pid_t*) (tls_base + TLS_PID_OFFSET);
    mtcp_output(MTCP_INFO,"Signal:%d Calling defererred pthread SETXID action for pid:%d "
		"Thread TLS pid:%d tid:[%d-%d] Checkpoint pid:%d Mother pid:%d\n",
		signum,si->si_pid,pid,t->m_tid,t->m_originalTID,chkpt_sys.chkptPID,
		chkpt_sys.motherPID);
    si->si_pid = pid;
    s_thrSetxid(signum,si,ctx);
    return;
  }
  mtcp_output(MTCP_INFO,"Signal:%d Ignore pthread cancel action.\n",signum);  
}

static int _real_sigprocmask(int how, const sigset_t *set, sigset_t *oldset)   {
  int rc = sigprocmask(how, set, oldset);
#if 0
  char bn[128], bo[128];
  int i = 0;
  for(i=0; i<64;++i) {
    bn[i] = sigismember(set,i)+'0';
    bo[i] = oldset ? sigismember(oldset,i)+'0' : '-';
  }
  bn[i] = bo[i] = 0;
  mtcp_output(MTCP_INFO,"_real_sigprocmask: New mask:%s\n",bn);
  mtcp_output(MTCP_INFO,"_real_sigprocmask: Old mask:%s\n",bo);
#endif
  return rc;
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

#include <fcntl.h>

extern "C" int __open(const char* pathname, int flags, mode_t mode)  {
  int rc = mtcp_sys_open(pathname,flags,mode);
  mtcp_output(MTCP_DEBUG,"Open file: fd:%d=%s with flags: %08X Mode:%X\n",rc,pathname,flags,mode);
  return rc;
}
__asm__ (".global open ; .type open,@function ; open = __open");


#define mtcp_sys_creat(args...)  mtcp_inline_syscall(creat,2,args)

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

extern "C" int __pipe(int fd[2])  {
  int rc = mtcp_sys_pipe(fd);
  mtcp_output(MTCP_DEBUG,"Pipe: rc=%d fd=[%d,%d]\n",rc,fd[0],fd[1]);
  return rc;
}
__asm__ (".global pipe ; .type pipe,@function ; pipe = __pipe");

/****************************************************************************
 *									     
 *  This is our clone system call wrapper				     
 *									     
 *    Note:								     
 *									     
 *      pthread_create eventually calls __clone to create threads	     
 *      It uses flags = 0x3D0F00:					     
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
  mtcp_output(MTCP_DEBUG,"wrapper clone: Starting to clone threads: %p(%p) ptid:%p ctid:%p tls:%p %s [%p]\n", 
	      fn, arg, parent_tidptr, child_tidptr, newtls, arg==mother ? "MOTHER_OF_ALL" : "",mother);
  if ( mother != 0 ) {
    // Track this thread
    Thread *t = new Thread();
    t->m_startFunc  = fn;   // this is the user's function
    t->m_startArg   = arg;  // ... and the parameter
    // Save exactly what the caller is supplying
    t->m_cloneFlags = flags;
    t->m_pParentTID = parent_tidptr;
    t->m_pGivenTID  = child_tidptr;
    t->m_parent     = Thread::current();
    t->state().init(ST_RUNDISABLED);

    // We need the CLEARTID feature so we can detect when the thread has exited
    // So if the caller doesn't want it, we enable it
    // Retain what the caller originally gave us so we can pass the tid back
    if (!(flags & CLONE_CHILD_CLEARTID)) {
      child_tidptr = &t->m_childTID;
    }
    t->m_pActualTID = child_tidptr;
    // Alter call parameters, forcing CLEARTID and make it call the wrapper routine
    flags |= CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID;
    fn  = Thread::staticMain;
    arg = t;
  }

  // Now create the thread
  rc = (*CLONE_CALL) (fn, child_stack, flags, arg, parent_tidptr, (user_desc*)newtls, child_tidptr);
  if (rc < 0) {
    mtcp_output(MTCP_ERROR,"wrapper clone*: clone rc=%d, errno=%d\n", rc, errno);
  }
  else {
    mtcp_output(MTCP_DEBUG,"wrapper clone*: clone rc=%d  SUCCESS\n", rc);
  }
  return (rc);
}
__asm__ (".global clone ; .type clone,@function ; clone = __clone");


/// Standard constructor
Thread::Thread() {
  ::memset(this,0,sizeof(Thread));
  m_restartArgs.self = this;
}

/// Standard Destructor
Thread::~Thread() {
  m_state.destroy();
}

/// Access to the overall thread queue
WEAK(Thread*) Thread::threads() {
  return s_threads;
}

/// Link thread struct to the lists and finish filling the thread structure.
WEAK(void) Thread::initialize(bool /* mother */)  {
  // Save the thread's ID number and put in threads list so we can look it up
  // Set state to disable checkpointing so checkpointer won't race between 
  // adding to list and setting up handler
  m_tid = mtcp_sys_kernel_gettid ();
  m_originalTID = (int)syscall(SYS_gettid);
  m_restartArgs.tid  = m_tid;
  m_restartArgs.self = this;
  {
    ThreadsLock lock;
    if ((this->next = s_threads) != 0) {
      this->next->prev = &this->next;
    }
    this->prev = &s_threads;
    s_threads = this;
    Thread* parent = this->m_parent;
    if ( parent ) {
      m_siblings = parent->m_children;
      parent->m_children = this;
    }
  }
  // Set up signal handler so we can interrupt the thread for checkpointing 
  setupSignals();
}

///  Get current thread struct pointer
Thread *Thread::current()   {
  int tid = mtcp_sys_kernel_gettid();
  ThreadsLock lock;
  for(Thread* thread = s_threads; thread != 0; thread = thread->next) {
    if (thread->m_tid == tid) {
      ThreadsLock::release ();
      return thread;
    }
  }
  mtcp_output(MTCP_FATAL,"Thread::current: can't find thread id %d\n", tid);
  return 0; // NOTREACHED : stop compiler warning
}

/// Save the system info of the process
WEAK(void) Thread::saveSysInfo() {
  chkpt_sys.saved_break = (void*) mtcp_sys_brk(0);  // kernel returns mm->brk when passed zero
  // Do this once, same for all threads.  But restore for each thread.
  if (mtcp_have_thread_sysinfo_offset())
    chkpt_sys.sysInfo = mtcp_get_thread_sysinfo();
  // Do this once.  It's the same for all threads.
#ifdef CHECKPOINT_TERMINAL
  saved_termios_exists = (isatty(STDIN_FILENO)
			  && tcgetattr(STDIN_FILENO,&saved_termios) >= 0);
#endif
  mtcp_output(MTCP_INFO,"saveSysInfo: saved_break=%p\n",chkpt_sys.saved_break);
}

/// Thread has exited - unlink it from lists and free struct
WEAK(int) Thread::cleanup()   {
  Thread **lthread, *xthread;
  ThreadsLock lock;
  mtcp_output(MTCP_DEBUG,"cleanup: %p->tid:%d\n",this,m_tid);

  // Remove thread block from 'threads' list
  if ((*(this->prev) = this->next) != 0) {
    this->next->prev = this->prev;
  }
  // Remove thread block from parent's list of children
  Thread* parent = m_parent;
  if (parent) {
    for(lthread = &parent->m_children; (xthread = *lthread) != this; lthread = &xthread->m_siblings) {}
    *lthread = xthread->m_siblings;

    // If this thread has children, give them to its parent
    while((xthread = m_children) != 0) {
      m_children   = xthread->m_siblings;
      xthread->m_siblings = parent->m_children;
      parent->m_children  = xthread;
    }
  }
  else {
    while((xthread = m_children) != 0) {
      m_children   = xthread->m_siblings;
      xthread->m_siblings = chkpt_sys.motherofall;
      chkpt_sys.motherofall = xthread;
    }
  }
  // If checkpointer is waiting for us, wake it to see this thread no longer in list
  m_state.wake();
  delete this;
  return 1;
}

///  Call this when it's OK to checkpoint
WEAK(int) Thread::instrument()     {
 again:
  switch ( m_state.value() ) {    
  case ST_RUNDISABLED:
    // Thread was running normally with checkpointing disabled.  Enable checkpointing then just return.
    if (!m_state.set(ST_RUNENABLED, ST_RUNDISABLED)) goto again;
    return (0);

  case ST_RUNENABLED:
    // Thread was running normally with checkpointing already enabled.  So just return as is. */
    return (1);

  case ST_SIGDISABLED:
    // Thread was running with checkpointing disabled, but the checkpointhread wants to 
    // write a checkpoint.  So mark the thread as having checkpointing enabled, then just 
    // 'manually' call the signal handler as if the signal to suspend were just sent.
    if (!m_state.set(ST_SIGENABLED, ST_SIGDISABLED)) goto again;
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
  pid_t motherpid=0, pid=0;
  mtcp_output(MTCP_DEBUG,"threadcloned: starting thread %p\n", this);
  initialize();

  // The new TLS should have the process ID in place at TLS_PID_OFFSET 
  // This is a verification step and is therefore optional as such 
  tls_base = tlsBase();
  motherpid = chkpt_sys.motherPID;
  pid = *(pid_t*) (tls_base + TLS_PID_OFFSET);
  if (pid != motherpid && pid != chkpt_sys.chkptPID && pid != (pid_t)-1) {
    mtcp_output(MTCP_ERROR,"threadcloned: tid:%d getpid %X/%d, must match tls pid %X/%d or %X/%d at offset %d\n", 
		m_tid, motherpid, motherpid, pid, pid, chkpt_sys.chkptPID, chkpt_sys.chkptPID, TLS_PID_OFFSET);
    for (rc = 0; rc < 256; rc += 4) {
      int data = *(pid_t*) (tls_base + rc);
      mtcp_output(MTCP_ERROR|MTCP_NO_HEADER,"   %d: %X", rc, data);
      if ((rc & 31) == 28) mtcp_output(MTCP_ERROR|MTCP_NO_HEADER,"\n");
    }
    //mtcp_output(MTCP_FATAL,"Fatal inconsistency in Thread::Main. tls_pid:%d\n",pid);
    mtcp_output(MTCP_ERROR,"Fatal inconsistency in Thread::Main. tls_pid:%d\n",pid);
  }

  // If the caller wants the child tid but didn't have CLEARTID, pass the tid back to it
  if ((m_cloneFlags & (CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID)) == CLONE_CHILD_SETTID) {
    *m_pGivenTID = m_childTID;
  }

  // Instrument thread to be "stopable"
  this->instrument();
  // Call the user's function for whatever processing they want done
  rc = (*m_startFunc)(m_startArg);
  mtcp_output(MTCP_DEBUG,"threadcloned: Started thread %p, calling %p (%p) rc:%d\n",
	      this,m_startFunc,m_startArg,rc);

  // Make sure checkpointing is inhibited while we clean up and exit
  // Otherwise, checkpointer might wait forever for us to re-enable
  this->wait();
  // Do whatever to unlink and free thread block
  this->cleanup();
  // Return the user's status as the exit code
  return rc;
}

/// Likewise, disable checkpointing
WEAK(int) Thread::wait()   {
 again:
  switch ( m_state.value() ) {
  case ST_RUNDISABLED:
  case ST_SIGDISABLED:
    return (0);
  case ST_RUNENABLED:
    if (!m_state.set(ST_RUNDISABLED, ST_RUNENABLED)) goto again;
    return (1);
  case ST_SIGENABLED:
    this->stop();
    goto again;
  default:
    mtcp_output(MTCP_FATAL,"Fatal inconsistency in Thread::wait. tid:%d State:%d\n",m_tid,m_state.value());
    return (0); /* NOTREACHED : stop compiler warning */
  }
}

/// Stop all threads in order to allow for a consistent checkpoint.
WEAK(int) Thread::stop()  {
  int rc;

  mtcp_output(MTCP_DEBUG,"stop: tid %d returns to %p\n", mtcp_sys_kernel_gettid (), __builtin_return_address (0));
  setupSignals();  // re-establish in case of another STOPSIGNAL so we don't abort by default

  // If this is checkpoint thread - exit immidiately
  if( m_state.value() == ST_CKPNTHREAD ){
    return 1;
  }
  
  if (m_state.set(ST_SUSPINPROG, ST_SIGENABLED)) {  // make sure we don't get called twice for same thread
    static int is_first_checkpoint = 1;
    ::sigpending (&m_sigpending); // save pending signals
    this->saveSignals();                                           // save signal state (and block signal delivery)
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
    if ( restoreinprog.value() == 0 )  {
      // We are the original process and all context is saved. restoreinprog is 0 ; wait for ckpt thread to write ckpt, and resume.

      WMB; // matched by RMB in checkpointhread

      // Next comes the first time we use the old stack.
      // Tell the checkpoint thread that we're all saved away
      if (!m_state.set(ST_SUSPENDED, ST_SUSPINPROG))  {              // tell checkpointhread all our context is saved
	mtcp_output(MTCP_FATAL,"Fatal inconsistency in Thread::stop. tid:%d State:%d SUSPENDED:%d SUSPINPROG:%d\n",
		    m_tid,m_state.value(), ST_SUSPENDED, ST_SUSPINPROG);
      }
      m_state.wake();
      // Then we wait for the checkpoint thread to write the checkpoint file then wake us up
      mtcp_output(MTCP_DEBUG,"stop: thread %d suspending\n",m_tid);
      while ( m_state.value() == ST_SUSPENDED) {
        m_state.wait(ST_SUSPENDED);
      }
    }
    else {
      // Else restoreinprog >= 1;  This stuff executes to do a restart
      mtcp_output(MTCP_DEBUG,"stop(restoreinprog:%d): Change state:%d to Running[%d] Suspended[%d]\n",
		  restoreinprog.value(),m_state.value(),ST_RUNENABLED,ST_SUSPENDED);
      if ( !m_state.set(ST_RUNENABLED, ST_SUSPENDED) )  {
	mtcp_output(MTCP_ERROR,"ERROR: Thread::stop: checkpoint was written when thread in SUSPENDED state\n");
      }
      waitForRestore();
      mtcp_output(MTCP_DEBUG,"stop: thread %d restored. State:%d\n", m_tid,m_state.value());
    }
  }
  mtcp_output(MTCP_DEBUG,"stop: m_tid %d return to address %p\n",
	      (int)mtcp_sys_kernel_gettid(),(void*)__builtin_return_address(0));
  return 1;
}

/// On restore wait until all threads are ready.
WEAK(void) Thread::waitForRestore()    {
  int rip, pid;
  do  {
    rip = restoreinprog.value();     // dec number of threads cloned but not completed longjmp'ing
  } while (!restoreinprog.set(rip-1, rip));
  if (--rip == 0) {
    /* raise the signals which were pending for the entire process at the time
     * of checkpoint. It is assumed that if a signal is pending for all threads
     * including the ckpt-thread, then it was sent to the process as opposed to
     * sent to individual threads.
     */
    pid = mtcp_sys_getpid();
    for (int i=1; i<NSIG; ++i) {
      if (sigismember(&sigpending_global, i) == 1) {
	mtcp_output(MTCP_INFO,"waitForRestore: Send signal %d to pid:%d\n",i,pid);
        kill(pid,i);
      }
    }
    restoreinprog.wake(999999999);  // if this was last of all, wake everyone up
    ThreadsLock::release();                           // ... and release the thread list
    mtcp_output(MTCP_INFO,"waitForRestore[%s] restoreinprog:%d State:%d Complete -> Continue.\n",
		this==chkpt_sys.motherofall ? "MOTHER" : "CHILD", restoreinprog.value(), m_state.value());
  }
  else {
    while ((rip = restoreinprog.value()) > 0) {       // otherwise, wait for last of all to wake this one up
      restoreinprog.wait(rip);
    }
  }
  mtcp_output(MTCP_INFO,"waitForRestore[%s] thread restore finished.\n",
	      this==chkpt_sys.motherofall ? "MOTHER" : "CHILD");
}

///  Save state necessary for TLS restore
WEAK(void) Thread::saveTLS()    {
  ::memset(&m_tls, 0, sizeof(m_tls));
  int rc, i = this->TLSSEGREG / 8;
  m_tls[0].entry_number = i;
  rc = mtcp_sys_get_thread_area (&m_tls[0]);
  if (rc < 0) {
    mtcp_output(MTCP_FATAL,"checkpointeverything: error saving GDT TLS entry[%d]: %s\n",
		i,::strerror(mtcp_sys_errno));
  }
}

/// Access the base address of the thread local storage.
WEAK(char*) Thread::tlsBase()   {
  mtcp_segreg_t TLSSEGREG;
  struct user_desc gdtentrytls;
  __asm__ volatile ("movl %%fs,%0" : "=q" (TLSSEGREG)); /* q = a,b,c,d for i386; 8 low bits of r class reg for x86_64 */
  gdtentrytls.entry_number = TLSSEGREG / 8;
  if ( mtcp_sys_get_thread_area ( &gdtentrytls ) < 0 ) {
    mtcp_output(MTCP_FATAL,"Threads::tlsBase: error getting GDT TLS entry: %s\n",strerror (mtcp_sys_errno));
  }
  unsigned long *val = (unsigned long*)&gdtentrytls.base_addr;
  return *(char**)val;
}

/// Setup the signal handling to stop/restart threads
WEAK(void) Thread::setupSignals()   {
  void (*oldhandler) (int signum);
  oldhandler = _real_signal(STOPSIGNAL, &stopthisthread);
  if (oldhandler == SIG_ERR) {
    mtcp_output(MTCP_FATAL,"setupSignals: error setting up signal handler: %s\n",strerror (errno));
  }
  if ((oldhandler != SIG_IGN) && (oldhandler != SIG_DFL) && (oldhandler != stopthisthread)) {
    mtcp_output(MTCP_FATAL,"setupSignals: signal handler %d already in use (%p).\n"
                 " You may employ a different signal by setting the\n"
                 " environment variable MTCP_SIGCKPT (or DMTCP_SIGCKPT)"
		 " to the number\n of the signal MTCP should "
                 "use for checkpointing.\n", STOPSIGNAL, oldhandler);
  }
}

///  Save signal handlers and block signal delivery
WEAK(void) Thread::saveSignals()  {
  sigset_t blockall;
  mtcp_output(MTCP_DEBUG,"saveSignals: pid:%d tid:%d\n",mtcp_sys_getpid(),m_tid);
  // Block signal delivery first so signal handlers can't change state of signal handlers on us
  sigfillset(&blockall);
  sigdelset(&blockall,STOPSIGNAL);

  if (_real_sigprocmask(SIG_SETMASK,&blockall,&m_sigblockmask) < 0) {
    mtcp_output(MTCP_FATAL,"Failed to set signal mask in thread. tid:%d\n",m_tid);
  }
  // Now save all the signal handlers
  for (int i=1; i<NSIG; ++i) {
    struct sigaction* a = &m_sigactions[i];
    if (mtcp_sys_sigaction(i,0,a) < 0) {
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
  mtcp_output(MTCP_INFO,"saveSignals: Thread %p: %d saved signal actions ...\n",this,m_tid);
}  

/// Restore all the signal handlers with different settings
WEAK(void) Thread::restoreSigActions() {
  for (int i=1; i<NSIG; ++i ) {
    struct sigaction act, *a = &chkpt_sys.motherofall->m_sigactions[i];
    if (mtcp_sys_sigaction(i,0,&act) < 0) {
      if (errno == EINVAL) ::memset(&act,0,sizeof(act));
    }
    if ( i != STOPSIGNAL ) {
      void** mask = (void**)&a->sa_mask;
      if ( i == SIG_THREAD_CANCEL && a->sa_sigaction != _thread_cancel_action )  {
	s_thrCancel  = a->sa_sigaction;
	a->sa_sigaction = _thread_cancel_action;
      }
      else if ( i == SIG_THREAD_SETXID && a->sa_sigaction != _thread_setxid_action )  {
	s_thrSetxid  = a->sa_sigaction;
	a->sa_sigaction = _thread_setxid_action;
      }
      if ( act.sa_handler != a->sa_handler ) {
	if ( _real_sigaction(i,a,0) < 0) {
	  mtcp_output(MTCP_FATAL,"restoreSigActions: Failed to set stored sigaction[%d]:%p. [%s]\n",
		      i,a->sa_handler,::strerror(errno));
	}
	mtcp_output(MTCP_INFO,"restoreSigActions: Restore action [sig:%d] -> %p Flags:%X Mask:%p\n",
		    i, a->sa_handler, a->sa_flags, *mask);
      }
      else if ( 0 != act.sa_handler && SIG_DFL != act.sa_handler && SIG_IGN != act.sa_handler ) {
	mtcp_output(MTCP_DEBUG,"restoreSigActions: Skip... action [sig:%d] -> %p Flags:%X Mask:%p\n",
		    i, a->sa_handler, a->sa_flags, *mask);
      }
    }
  }
}

/// Restart thread and all depending children after checkpointing and on restart.
WEAK(int) Thread::restart(int force_context)     {
  int rip;
  int tid = mtcp_sys_kernel_gettid();
  const char* thr_typ = this==chkpt_sys.motherofall ? "MOTHER" : "CHILD";
  mtcp_output(MTCP_DEBUG,"restart[%s] Pid:%d thread:%p tid:%d motherOfAll:%p ctxt:%p\n",
	      thr_typ,this,tid,chkpt_sys.motherofall,chkpt_sys.motherPID,m_savctx.SAVEDSP);
  restoreTLS();
  setupSignals();

  if (this == chkpt_sys.motherofall) {
    // Compute the set of signals which was pending for all the threads at the
    // time of checkpoint. This is a heuristic to compute the set of signals
    // which were pending for the entire process at the time of checkpoint.
    sigset_t tmp;
    sigfillset(&tmp);
    for (Thread* t = s_threads; t != 0; t = t->next) {
      sigandset(&sigpending_global, &tmp, &t->m_sigpending);
      tmp = sigpending_global;
    }
    //this->set_tid_address();
#ifdef CHECKPOINT_TERMINAL
    // Do it once only, in motherofall thread.
    if (saved_termios_exists)
      if ( ! isatty(STDIN_FILENO)
           || tcsetattr(STDIN_FILENO, TCSANOW, &saved_termios) < 0 )
        mtcp_output(MTCP_WARNING,"WARNING: restart: failed to restore terminal\n");
#endif
  }

  restoreSignals();

  int cnt = 0;
  Thread* child = 0;

  // First loop over all children and increase the restore count. Restore is finished once this counter is 0
  for (child = m_children; child != 0; child = child->m_siblings)     {
    // Increment number of threads created but haven't completed their longjmp
    do   {
      rip = restoreinprog.value();
      ++s_restoring;
    } while (!restoreinprog.set(rip+1,rip));
  }
  mtcp_output(MTCP_INFO,"restart[%s] pid:%d thread:%p tid:%d restoreinprog:%d Start CLONE...\n",
	      thr_typ,chkpt_sys.motherPID,this,tid,restoreinprog.value());
  // No create the children
  for (child = m_children; child != 0; child = child->m_siblings)     {
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
    pid_t tid = 0;
    int cloneFlags = (child->m_cloneFlags & ~CLONE_SETTLS) | CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID;
    child->m_restartArgs.self = child;
    child->m_restartArgs.tid  = child->m_originalTID;
    void* p = (void*)(child->m_savctx.SAVEDSP - 128);
    mtcp_output(MTCP_DEBUG,"*: CLONE:%d %p %p --> %p  Ctxt:%p State:%d CLONE_SIGHAND:%s\n",
		child->m_originalTID, child->m_pParentTID, child->m_pActualTID, CLONE_CALL, p,
		child->state().value(),cloneFlags & CLONE_SIGHAND ? "YES" : "NO");

    if (dmtcp_info_pid_virtualization_enabled == 1) {
      tid = syscall(SYS_clone, Thread::restartMain,
		    (void *)(child->m_savctx.SAVEDSP - 128),  // -128 for red zone
		    cloneFlags,
		    &child->m_restartArgs, child->m_pParentTID, 0, child->m_pActualTID);
    } else {
      tid = (*CLONE_CALL)(Thread::restartMain,
			  p,  // -128 for red zone
			  cloneFlags,
			  &child->m_restartArgs, child->m_pParentTID, 0, child->m_pActualTID);
    }

    if ( tid < 0 )  {
      mtcp_output(MTCP_FATAL,"restart: error %d recreating thread. Flags:%X SP:%p\n", 
		  errno, child->m_cloneFlags, child->m_savctx.SAVEDSP);
    }
    mtcp_output(MTCP_INFO,"\t--> [%d:%d] CHILD thread Cloned:%d MotherOfAll:%p State:%d Restart:Progress:%d Type:%d\n",
		cnt,chkpt_sys.motherPID,m_tid,chkpt_sys.motherofall,child->state().value(),
		restoreinprog.value(),chkpt_sys.restart_type);
    ++cnt;
  }

  // All my children have been created, jump to the stopthisthread routine just after getcontext call
  // Note that if this is the restored checkpointhread, it jumps to the checkpointhread routine
  if (mtcp_have_thread_sysinfo_offset()) {
    mtcp_set_thread_sysinfo(chkpt_sys.sysInfo);
  }
  if ( this == chkpt_sys.motherofall )  {
    // Restore signal actions
    restoreSigActions();
    // Wait for restore to finish...
    while(restoreinprog.value()>0)
      usleep(10000);
  }
  mtcp_output(MTCP_INFO,"restart[%s]: setcontext: pid:%d tid: %d, orgTID:%d Restore:%d\n",
	      thr_typ, chkpt_sys.motherPID, m_tid, m_originalTID, restoreinprog.value());
  if ( this != chkpt_sys.motherofall || force_context != 0 )  {
    mtcp_output(MTCP_INFO,"restart[%s]: setting context. Should not return.\n",thr_typ);
    ::setcontext(&m_savctx); // Shouldn't return if everything works fine.
    mtcp_output(MTCP_ERROR,"restart[%s]: FATAL: returned from setcontext.\n",thr_typ);
  }
  mtcp_output(MTCP_INFO,"restart: All done: this:%p tid: %d, orgTID:%d\n",
	      this,m_tid,m_originalTID);
  return 0;
}

/// Save signal handlers and block signal delivery
WEAK(void) Thread::restoreSignals()  {
  mtcp_output(MTCP_DEBUG,"restoreSignals: pid:%d tid:%d Restoreinprog:%d\n",mtcp_sys_getpid(),m_tid,restoreinprog.value());
  if (_real_sigprocmask(SIG_SETMASK, &m_sigblockmask, 0) >= 0) {
    // Raise the signals which were pending for only this thread at the time of checkpoint.
    for (int i = 1; i<NSIG; ++i ) {
      if (sigismember(&m_sigpending, i)  == 1  && 
	  sigismember(&m_sigblockmask, i) == 1 &&
	  sigismember(&sigpending_global, i) == 0) {
	raise(i);
      }
    }
    return;
  }
  mtcp_output(MTCP_FATAL,"Failed to restore signals. pid:%d tid:%d\n",mtcp_sys_getpid(),m_tid);
}

/// set_tid_address wrapper routine: save the new address of the tidptr that will get cleared when the thread exits
WEAK(long) Thread::set_tid_address ()   {
  long rc = 0;
  mtcp_output(MTCP_DEBUG,"set_tid_address*: thread %p->tid:%d, tidptr %p\n",this,m_tid,&m_childTID);
  m_pActualTID = &m_childTID;  // save new tidptr so subsequent restore will create with new pointer
  mtcp_sys_set_tid_address(&m_childTID);
  return rc;                       // now we tell kernel to change it for the current thread
}

///  Restore the GDT entries that are part of a thread's state
WEAK(void) Thread::restoreTLS()  {
  pid_t motherpid = chkpt_sys.motherPID;
  const char* thr_typ = this == chkpt_sys.motherofall ? "MOTHER" : "CHILD";
  unsigned long* base = (unsigned long*)&m_tls[0].base_addr;
  pid_t *ppid = (pid_t*)(*base + TLS_PID_OFFSET);

  // The assumption that this points to the pid was checked by that tls_pid crap near the beginning
  *ppid = motherpid;

  // Restore all three areas
  int rc = mtcp_sys_set_thread_area_base(base);
  if (rc < 0) {
    mtcp_output(MTCP_FATAL,"restoreTLS[%s]: error %d restoring TLS entry[%d]\n",
		thr_typ,mtcp_sys_errno,m_tls[0].entry_number);
  }
  m_tid = mtcp_sys_kernel_gettid();
  mtcp_output(MTCP_INFO,"restoreTLS[%s]: motherpid:%d Thread tid=%d restoring TLS entry[%d]\n",
	      thr_typ,motherpid,m_tid,m_tls[0].entry_number);
}
