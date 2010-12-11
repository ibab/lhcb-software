#include "CheckPointing/MainThread.h"
#include "CheckPointing/Thread.h"
#include "CheckPointing/MB.h"

#include "CheckPointing/ThreadsLock.h"
#include "CheckPointing/Static.h"
#include "CheckPointing/SysCalls.h"
#include "CheckPointing.h"
#include "linux/futex.h"
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <unistd.h>
#include <termios.h>

#include <execinfo.h> 

/* TLS segment registers used differently in i386 and x86_64. - Gene */
#ifdef __i386__
# define TLSSEGREG gs
#endif
#ifdef __x86_64__
# define TLSSEGREG fs
#endif

/* Retrieve saved stack pointer saved by getcontext () */
#ifdef __x86_64__
//#define REG_RSP 15  // defined in ucontext.h
#define SAVEDSP uc_mcontext.gregs[REG_RSP]
#else
//#define REG_ESP 7 // defined in ucontext.h
#define SAVEDSP uc_mcontext.gregs[REG_ESP]
#endif

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
# define mtcp_sys_set_thread_area(uinfo) \
    ( mtcp_inline_syscall(arch_prctl,2,ARCH_SET_FS, \
	*(unsigned long int *)&(((struct user_desc *)uinfo)->base_addr)), \
      mtcp_inline_syscall(arch_prctl,2,ARCH_SET_GS, myinfo_gs) \
    )

static Thread     *s_threads = 0;
static Thread     *s_motherofall = 0;
static FutexState  restoreinprog;
static sigset_t    sigpending_global;    // pending signals for the process
static int         dmtcp_info_pid_virtualization_enabled = 0;
static struct      termios saved_termios;
static int         saved_termios_exists = 0;

using namespace CheckPointing;

/**  Set the thread's STOPSIGNAL handler.  Threads are sent STOPSIGNAL when they are to suspend execution the application, save
 *  their state and wait for the checkpointhread to write the checkpoint file.
 *
 *    Output:
 *
 *	Calling thread will call stopthisthread () when sent a STOPSIGNAL
 */
static void stopthisthread(int signum)  {
  Thread *thread = Thread::current();
  thread->stop();
}

/**  Thread execution wrapper
 *
 *   Simply call the object function.
 */
int Thread::staticMain(void *threadv)  {
  Thread *const thread = (Thread*)threadv;
  return thread->Main();
}

/** Entry point to restart thread after a fork.
 */
int Thread::restartMain(void* arg) {
  Thread::RestartArgs* a = (Thread::RestartArgs*)arg;
  return a->self->restart();
}

static sighandler_t _real_signal(int signum, sighandler_t handler){
  return signal(signum, handler);
}
static int _real_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact){
  return sigaction(signum, act, oldact);
}
static int _real_sigprocmask(int how, const sigset_t *set, sigset_t *oldset){
  return sigprocmask(how, set, oldset);
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
  volatile int dummy_value = 1; /*Again, try to prevent compiler optimization*/
  if (kbStack > 0)
    growstack(kbStack - kBincrement);
  else
    growstackValue++;
}

extern "C" pid_t __getpid()  {
  return mtcp_sys_getpid();
}
asm (".global getpid ; .type getpid,@function ; getpid = __getpid");

extern "C" int __getppid()  {
  return mtcp_sys_getppid();
}
asm (".global getppid ; .type getppid,@function ; getppid = __getppid");

#include <fcntl.h>

extern "C" int __open(const char* pathname, int flags, mode_t mode)  {
  int rc = mtcp_sys_open(pathname,flags,mode);
  mtcp_output(MTCP_DEBUG,"Open file: fd:%d=%s with flags: %08X Mode:%X\n",rc,pathname,flags,mode);
  return rc;
}
asm (".global open ; .type open,@function ; open = __open");


#define mtcp_sys_creat(args...)  mtcp_inline_syscall(creat,2,args)

extern "C" int __creat(const char* pathname, mode_t mode)  {
  int rc = mtcp_sys_creat(pathname,mode);
  mtcp_output(MTCP_DEBUG,"Create file: fd:%d=%s with Mode:%X\n",rc,pathname,mode);
  return rc;
}
asm (".global creat ; .type creat,@function ; creat = __creat");

extern "C" int __close(int fd)  {
  mtcp_output(MTCP_DEBUG,"Close file: %d\n",fd);
  return mtcp_sys_close(fd);
}
asm (".global close ; .type close,@function ; close = __close");

extern "C" int __pipe(int fd[2])  {
  int rc = mtcp_sys_pipe(fd);
  mtcp_output(MTCP_DEBUG,"Pipe: rc=%d fd=[%d,%d]\n",rc,fd[0],fd[1]);
  return rc;
}
asm (".global pipe ; .type pipe,@function ; pipe = __pipe");

/*****************************************************************************/
/*									     */
/*  This is our clone system call wrapper				     */
/*									     */
/*    Note:								     */
/*									     */
/*      pthread_create eventually calls __clone to create threads	     */
/*      It uses flags = 0x3D0F00:					     */
/*	      CLONE_VM = VM shared between processes			     */
/*	      CLONE_FS = fs info shared between processes (root, cwd, umask) */
/*	   CLONE_FILES = open files shared between processes (fd table)	     */
/*	 CLONE_SIGHAND = signal handlers and blocked signals shared	     */
/*	 			 (sigaction common to parent and child)	     */
/*	  CLONE_THREAD = add to same thread group			     */
/*	 CLONE_SYSVSEM = share system V SEM_UNDO semantics		     */
/*	  CLONE_SETTLS = create a new TLS for the child from newtls parameter*/
/*	 CLONE_PARENT_SETTID = set the TID in the parent (before MM copy)    */
/*	CLONE_CHILD_CLEARTID = clear the TID in the child and do	     */
/*				 futex wake at that address		     */
/*	      CLONE_DETACHED = create clone detached			     */
/*									     */
/*****************************************************************************/
extern "C" int __clone(int (*fn) (void *arg), void *child_stack, int flags, void *arg,
		       int *parent_tidptr, struct user_desc *newtls, int *child_tidptr)
{
  int rc;
  Thread*  motherofall = MainThread::accessInstance().motherOfAll();
  mtcp_output(MTCP_INFO,"mtcp wrapper clone*: Starting to clone threads: %p(%p) ptid:%p ctid:%p tls:%p %s\n", 
		fn, arg, parent_tidptr, child_tidptr, newtls, arg==motherofall ? "MOTHER_OF_ALL" : "");
  if ( motherofall != NULL ) {
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
  rc = (*CLONE_CALL) (fn, child_stack, flags, arg, parent_tidptr, newtls, child_tidptr);
  if (rc < 0) {
    mtcp_output(MTCP_ERROR,"mtcp wrapper clone*: clone rc=%d, errno=%d\n", rc, errno);
  }
  else {
    mtcp_output(MTCP_INFO,"mtcp wrapper clone*: clone rc=%d  SUCCESS\n", rc);
  }
  return (rc);
}
asm (".global clone ; .type clone,@function ; clone = __clone");


/// Standard constructor
Thread::Thread() {
  ::memset(this,0,sizeof(Thread));
  m_restartArgs.self = this;
}

Thread::~Thread() {
  m_state.destroy();
}

Thread* Thread::threads() {
  return s_threads;
}

/// Link thread struct to the lists and finish filling the thread structure.
void Thread::initialize(bool mother)  {
  // Save the thread's ID number and put in threads list so we can look it up
  // Set state to disable checkpointing so checkpointer won't race between 
  // adding to list and setting up handler

  if ( mother ) s_motherofall = this;

  m_tid = mtcp_sys_kernel_gettid ();
  m_originalTID = (int)syscall(SYS_gettid);
  m_restartArgs.tid = m_tid;
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
      return (thread);
    }
  }
  mtcp_output(MTCP_FATAL,"mtcp Thread::current: can't find thread id %d\n", tid);
  return 0; /* NOTREACHED : stop compiler warning */
}

/// Save the system info of the process
void Thread::saveSysInfo() {
  chkpt_sys.saved_break = (void*) mtcp_sys_brk(NULL);  // kernel returns mm->brk when passed zero
  // Do this once, same for all threads.  But restore for each thread.
  if (mtcp_have_thread_sysinfo_offset())
    chkpt_sys.sysInfo = mtcp_get_thread_sysinfo();
  // Do this once.  It's the same for all threads.
  saved_termios_exists = ( isatty(STDIN_FILENO)
			   && tcgetattr(STDIN_FILENO, &saved_termios) >= 0 );
  mtcp_output(MTCP_INFO,"mtcp stopThreads*: saved_break=%p\n", chkpt_sys.saved_break);
}

/// Thread has exited - unlink it from lists and free struct
int Thread::cleanup()   {
  Thread **lthread, *xthread;
  ThreadsLock lock;
  mtcp_output(MTCP_INFO,"Thread::cleanup: %p->tid:%d\n",this,m_tid);

  // Remove thread block from 'threads' list
  if ((*(this->prev) = this->next) != NULL) {
    this->next->prev = this->prev;
  }
  // Remove thread block from parent's list of children
  Thread* parent = m_parent;
  if (parent) {
    for(lthread = &parent->m_children; (xthread = *lthread) != this; lthread = &xthread->m_siblings) {}
    *lthread = xthread->m_siblings;

    // If this thread has children, give them to its parent
    while((xthread = m_children) != NULL) {
      m_children   = xthread->m_siblings;
      xthread->m_siblings = parent->m_children;
      parent->m_children  = xthread;
    }
  }
  else {
    while((xthread = m_children) != NULL) {
      m_children   = xthread->m_siblings;
      xthread->m_siblings = s_motherofall;
      s_motherofall       = xthread;
    }
  }
  // If checkpointer is waiting for us, wake it to see this thread no longer in list
  m_state.wait(FUTEX_WAKE, 1, 0);
  delete this;
  return 1;
}

///  Call this when it's OK to checkpoint
int Thread::instrument()     {
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
    mtcp_abort ();
    return (0); /* NOTREACHED : stop compiler warning */
  }
}


/** Main thread execution function
 *
 *  This routine is called (via clone) as the top-level routine of a thread that we are tracking.
 *  It fills in remaining items of our thread struct, calls the user function, then cleans up the thread struct before exiting.
 */
int Thread::Main()  {
  int rc;
  pid_t motherpid = MainThread::accessInstance().motherPID();
  mtcp_output(MTCP_DEBUG,"mtcp threadcloned*: starting thread %p\n", this);
  initialize();

  char* tls_base = tlsBase();
  // The new TLS should have the process ID in place at TLS_PID_OFFSET 
  // This is a verification step and is therefore optional as such 
  pid_t  tls_pid = *(pid_t*) (tls_base + TLS_PID_OFFSET);
  if ((tls_pid != motherpid) && (tls_pid != (pid_t)-1)) {
    mtcp_output(MTCP_ERROR,"mtcp threadcloned: getpid %X %d, tls pid %d at offset %d, must match\n", 
	       motherpid, motherpid, tls_pid, TLS_PID_OFFSET);
    for (rc = 0; rc < 256; rc += 4) {
      tls_pid = *(pid_t*) (tls_base + rc);
      mtcp_output(MTCP_ERROR,"   %d: %X", rc, tls_pid);
      if ((rc & 31) == 28) mtcp_output(MTCP_ERROR,"\n");
    }
    mtcp_abort ();
  }

  // If the caller wants the child tid but didn't have CLEARTID, pass the tid back to it

  if ((m_cloneFlags & (CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID)) == CLONE_CHILD_SETTID) {
    *m_pGivenTID = m_childTID;
  }

  // Instrument thread to be "stopable"
  this->instrument();
  // Call the user's function for whatever processing they want done
  mtcp_output(MTCP_INFO,"mtcp threadcloned*: Start thread %p, calling %p (%p) ...",this,m_startFunc,m_startArg);
  rc = (*m_startFunc)(m_startArg);
  mtcp_output(MTCP_INFO," rc=%d\n", rc);

  // Make sure checkpointing is inhibited while we clean up and exit
  // Otherwise, checkpointer might wait forever for us to re-enable
  this->wait();
  // Do whatever to unlink and free thread block
  this->cleanup();
  // Return the user's status as the exit code
  return rc;
}

/// Likewise, disable checkpointing
int Thread::wait()   {
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
    mtcp_abort ();
    return (0); /* NOTREACHED : stop compiler warning */
  }
}

int Thread::stop()  {
  int rc;

#define BT_SIZE 1024
#define STDERR_FD 826
#define LOG_FD 826

  mtcp_output(MTCP_DEBUG,"mtcp Thread::stop*: tid %d returns to %p\n", mtcp_sys_kernel_gettid (), __builtin_return_address (0));
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
    if (this == s_motherofall) {
      static char *orig_stack_ptr;
      int kbStack = 2048;
      if (is_first_checkpoint) {
	orig_stack_ptr = (char*)&kbStack;
        is_first_checkpoint = 0;
        mtcp_output(MTCP_INFO,"Thread::stop: temp. grow main stack by %d kilobytes");
        growstack(kbStack);
      } else if (orig_stack_ptr - (char *)&kbStack > 3 * kbStack*1024 / 4) {
        mtcp_output(MTCP_WARNING,"WARNING:  Stack within %d bytes of end;\n"
		    "  Consider increasing 'kbStack' at line %d of mtcp/%s\n",
		    kbStack*1024/4, __LINE__-9, __FILE__);
      }
    }

    ///JA: new code ported from v54b
    rc = getcontext (&m_savctx);
    if (rc < 0) {
      mtcp_output(MTCP_FATAL,"mtcp Thread::stop: getcontext rc %d errno %d\n", rc, errno);
    }
    mtcp_output(MTCP_DEBUG,"mtcp Thread::stop*: after getcontext: %p\n",m_savctx.SAVEDSP);
    if (restoreinprog.value() == 0) {
      // We are the original process and all context is saved
      // restoreinprog is 0 ; wait for ckpt thread to write ckpt, and resume.

      WMB; // matched by RMB in checkpointhread

      /* Next comes the first time we use the old stack. */
      /* Tell the checkpoint thread that we're all saved away */
      if (!m_state.set(ST_SUSPENDED, ST_SUSPINPROG)) mtcp_abort();   // tell checkpointhread all our context is saved
      m_state.wait(FUTEX_WAKE, 1, 0);                                // wake checkpoint thread if it's waiting for me
      // Then we wait for the checkpoint thread to write the checkpoint file then wake us up
      mtcp_output(MTCP_DEBUG,"mtcp Thread::stop*: thread %d suspending\n",m_tid);
      while ( m_state.value() == ST_SUSPENDED) {
        m_state.wait(FUTEX_WAIT,ST_SUSPENDED,0);
      }
    }
    // Else restoreinprog >= 1;  This stuff executes to do a restart
    else {
      if (!m_state.set(ST_RUNENABLED, ST_SUSPENDED)) mtcp_abort ();  // checkpoint was written when thread in SUSPENDED state
      waitForRestore();
      mtcp_output(MTCP_DEBUG,"mtcp Thread::stop*: thread %d restored\n", m_tid);
    }
  }
  mtcp_output(MTCP_INFO,"mtcp Thread::stop*: m_tid %d returning to %p\n",
	      mtcp_sys_kernel_gettid(),__builtin_return_address(0));
  return 1;
}

///  Save state necessary for TLS restore
void Thread::saveTLS()    {
  ::memset(&m_tls, 0, sizeof(m_tls));
  int rc, i = this->TLSSEGREG / 8;
  m_tls[0].entry_number = i;
  rc = mtcp_sys_get_thread_area (&m_tls[0]);
  if (rc < 0) {
    mtcp_output(MTCP_FATAL,"mtcp checkpointeverything: error saving GDT TLS entry[%d]: %s\n",
		i,strerror(mtcp_sys_errno));
  }
}

char* Thread::tlsBase()   {
  mtcp_segreg_t TLSSEGREG;
  struct user_desc gdtentrytls;
  asm volatile ("movl %%fs,%0" : "=q" (TLSSEGREG)); /* q = a,b,c,d for i386; 8 low bits of r class reg for x86_64 */
  gdtentrytls.entry_number = TLSSEGREG / 8;
  if ( mtcp_sys_get_thread_area ( &gdtentrytls ) < 0 ) {
    mtcp_output(MTCP_FATAL,"mtcp_init: error getting GDT TLS entry: %s\n",strerror (mtcp_sys_errno));
  }
  return (char*)(*(unsigned long *)&(gdtentrytls.base_addr));
}

void Thread::setupSignals()   {
  void (*oldhandler) (int signum);
  oldhandler = _real_signal (STOPSIGNAL, &stopthisthread);
  if (oldhandler == SIG_ERR) {
    mtcp_output(MTCP_FATAL,"mtcp setupthread: error setting up signal handler: %s\n",strerror (errno));
  }
  if ((oldhandler != SIG_IGN) && (oldhandler != SIG_DFL) && (oldhandler != stopthisthread)) {
    mtcp_output(MTCP_FATAL,"mtcp setupthread: signal handler %d already in use (%p).\n"
                 " You may employ a different signal by setting the\n"
                 " environment variable MTCP_SIGCKPT (or DMTCP_SIGCKPT)"
		 " to the number\n of the signal MTCP should "
                 "use for checkpointing.\n", STOPSIGNAL, oldhandler);
  }
}

///  Save signal handlers and block signal delivery
void Thread::saveSignals()  {
  sigset_t blockall;

  // Block signal delivery first so signal handlers can't change state of signal handlers on us */
  memset (&blockall, -1, sizeof blockall);
  if (_real_sigprocmask (SIG_SETMASK, &blockall, &(m_sigblockmask)) < 0) {
    mtcp_abort ();
  }
  // Now save all the signal handlers
  for (int i = NSIG; -- i >= 0;) {
    if (_real_sigaction (i, NULL, m_sigactions + i) < 0) {
      if (errno == EINVAL)  {
	memset (m_sigactions + i, 0, sizeof(m_sigactions[i]));
	continue;
      }
      mtcp_output(MTCP_FATAL,"mtcp save_sig_state: error saving signal %d action: %s\n", i, strerror (errno));
    }
  }
  mtcp_output(MTCP_INFO,"mtcp save_sig_state: Thread %d saved signal actions ...\n", m_tid);
  sigdelset(&blockall,STOPSIGNAL);
  if (_real_sigprocmask(SIG_SETMASK,&blockall,&m_sigblockmask) < 0) {
    mtcp_abort ();
  }
}  

void Thread::waitForRestore()    {
  int rip;
  do  {
    rip = restoreinprog.value();     // dec number of threads cloned but not completed longjmp'ing
  } while (!restoreinprog.set(rip-1, rip));
  if (--rip == 0) {
    /* raise the signals which were pending for the entire process at the time
     * of checkpoint. It is assumed that if a signal is pending for all threads
     * including the ckpt-thread, then it was sent to the process as opposed to
     * sent to individual threads.
     */
    for (int i = NSIG; -- i >= 0;) {
      if (sigismember(&sigpending_global, i) == 1) {
        kill(mtcp_sys_getpid(),i);
      }
    }
    restoreinprog.wait(FUTEX_WAKE, 999999999, NULL);  // if this was last of all, wake everyone up
    ThreadsLock::release ();                                                 // ... and release the thread list
  } else {
    while ((rip = restoreinprog.value()) > 0) {           // otherwise, wait for last of all to wake this one up
      restoreinprog.wait(FUTEX_WAIT, rip, NULL);
    }
  }
}


int Thread::restart()     {
  int rip;

  //m_tid = mtcp_sys_kernel_gettid ();
  restoreTLS();
  setupSignals();

  if (this == s_motherofall) {
    // Compute the set of signals which was pending for all the threads at the
    // time of checkpoint. This is a heuristic to compute the set of signals
    // which were pending for the entire process at the time of checkpoint.
    sigset_t tmp;
    sigfillset(&tmp);
    for (Thread* t = s_threads; t != NULL; t = t->next) {
      sigandset(&sigpending_global, &tmp, &t->m_sigpending);
      tmp = sigpending_global;
    }
    //this->set_tid_address();
    #if 0
    // Do it once only, in motherofall thread.
    if (saved_termios_exists)
      if ( ! isatty(STDIN_FILENO)
           || tcsetattr(STDIN_FILENO, TCSANOW, &saved_termios) < 0 )
        mtcp_output(MTCP_WARNING,"WARNING: mtcp finishrestore*: failed to restore terminal\n");
    #endif
  }
  
  restoreSignals();

  int cnt = 0;
  m_wait_for_children = 1;
  for (Thread* child = m_children; child != 0; child = child->m_siblings) {

    // Increment number of threads created but haven't completed their longjmp
    do rip = restoreinprog.value();
    while (!restoreinprog.set(rip+1,rip));

    /* Create the thread so it can finish restoring itself.                       */
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
    child->m_restartArgs.self = child;
    child->m_restartArgs.tid  = child->m_originalTID;
    void* p = (void *)(child->m_savctx.SAVEDSP - 128);
    mtcp_output(MTCP_INFO,"*: CLONE:%d %p %p --> %p  Ctxt:%p\n",
		child->m_originalTID, child->m_pParentTID, child->m_pActualTID, CLONE_CALL, p);
    if (dmtcp_info_pid_virtualization_enabled == 1) {
      tid = syscall(SYS_clone, Thread::restartMain,
		    (void *)(child->m_savctx.SAVEDSP - 128),  // -128 for red zone
		    (child->m_cloneFlags & ~CLONE_SETTLS) | CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID,
		    &child->m_restartArgs, child->m_pParentTID, NULL, child->m_pActualTID);
    } else {
      tid = (*CLONE_CALL)(Thread::restartMain,
			  p,  // -128 for red zone
			  (child->m_cloneFlags & ~CLONE_SETTLS) | CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID,
			  &child->m_restartArgs, child->m_pParentTID, NULL, child->m_pActualTID);
    }
    
    if (tid < 0) {
      mtcp_output(MTCP_FATAL,"mtcp restarthread: error %d recreating thread. Flags:%X SP:%p\n", 
		  errno, child->m_cloneFlags, child->m_savctx.SAVEDSP);
    }
    mtcp_output(MTCP_INFO,"\t--> [%d:%d] CHILD thread Cloned:%d MotherOfAll:%p\n",
		cnt,m_tid,tid,s_motherofall);
    ++cnt;
  }
  m_wait_for_children = 0;

  // All my children have been created, jump to the stopthisthread routine just after getcontext call
  // Note that if this is the restored checkpointhread, it jumps to the checkpointhread routine
  if (mtcp_have_thread_sysinfo_offset())
    mtcp_set_thread_sysinfo(chkpt_sys.sysInfo);
  mtcp_output(MTCP_DEBUG,"mtcp restarthread*-> setcontext: m_tid: %d, orgTID:%d this:%p curr:%p\n",
	      m_tid, m_originalTID, this, Thread::current());
  if (this != s_motherofall)  {
    setcontext(&m_savctx); /* Shouldn't return */
  }
  mtcp_output(MTCP_INFO,"mtcp restarthread*: All done: this:%p tid: %d, orgTID:%d\n",this,m_tid,m_originalTID);
  return 0;
}

/// Save signal handlers and block signal delivery
void Thread::restoreSignals()  {
  if (_real_sigprocmask(SIG_SETMASK, &m_sigblockmask, NULL) >= 0) {
    // Raise the signals which were pending for only this thread at the time of checkpoint.
    for (int i = NSIG; -- i >= 0;) {
      if (sigismember(&m_sigpending, i)  == 1  && 
	  sigismember(&m_sigblockmask, i) == 1 &&
	  sigismember(&sigpending_global, i) == 0) {
	raise(i);
      }
    }
    return;
  }
  mtcp_abort ();
}

/// set_tid_address wrapper routine: save the new address of the tidptr that will get cleared when the thread exits
long Thread::set_tid_address ()   {
  long rc = 0;
  mtcp_output(MTCP_DEBUG,"set_tid_address*: thread %p->tid:%d, tidptr %p\n",this,m_tid,&m_childTID);
  m_pActualTID = &m_childTID;  // save new tidptr so subsequent restore will create with new pointer
  mtcp_sys_set_tid_address(&m_childTID);
  return (rc);                       // now we tell kernel to change it for the current thread
}

///  Restore the GDT entries that are part of a thread's state
void Thread::restoreTLS()    {
  pid_t motherpid = MainThread::accessInstance().motherPID();

  // The assumption that this points to the pid was checked by that tls_pid crap near the beginning
  *(pid_t*)(*(unsigned long *)&(m_tls[0].base_addr) + TLS_PID_OFFSET) = motherpid;

  // Likewise, we must jam the new pid into the mother thread's tid slot (checked by tls_tid carpola)
  if (this == s_motherofall) {
    *(pid_t*)(*(unsigned long *)&(m_tls[0].base_addr) + TLS_TID_OFFSET) = motherpid;
  }

  // Restore all three areas
  int rc = mtcp_sys_set_thread_area (&(m_tls[0]));
  if (rc < 0) {
    mtcp_output(MTCP_FATAL,"Thread:restoreTLS: error %d restoring TLS entry[%d]\n",mtcp_sys_errno,m_tls[0].entry_number);
  }
#ifdef __x86_64__
/* Don't directly set fs.  It would only set 32 bits, and we just
 *  set the full 64-bit base of fs, using sys_set_thread_area,
 *  which called arch_prctl.
 *asm volatile ("movl %0,%%fs" : : "m" (thisthread -> fs));
 *asm volatile ("movl %0,%%gs" : : "m" (thisthread -> gs));
 */
#endif
  m_tid = mtcp_sys_kernel_gettid ();
}

/// Entry point to restart main thread after a process restore from file.
void Thread::finishRestore(Thread* t) {
  // Call another routine because our internal stack is whacked and we can't have local vars
  ///JA: v54b port
  // so restarthread will have a big stack
  asm volatile (CLEAN_FOR_64_BIT(mov %0,%%esp)
		: : "g" (t->m_savctx.SAVEDSP - 128 ) : "memory");  // -128 for red zone
  t->restart();
}
