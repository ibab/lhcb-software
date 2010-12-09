#ifndef PROCESSFORK_THREAD_H
#define PROCESSFORK_THREAD_H

#include "FutexState.h"
#include "csignal"
#include "ucontext.h"
#include "asm/prctl.h"
#include "asm/ldt.h"

#ifdef __i386__
typedef unsigned short mtcp_segreg_t;
#endif
#ifdef __x86_64__
typedef unsigned int mtcp_segreg_t;
#endif


#define ST_RUNDISABLED 0     // thread is running normally but with checkpointing disabled
#define ST_RUNENABLED 1      // thread is running normally and has checkpointing enabled
#define ST_SIGDISABLED 2     // thread is running normally with cp disabled, but checkpoint thread is waiting for it to enable
#define ST_SIGENABLED 3      // thread is running normally with cp enabled, and checkpoint thread has signalled it to stop
#define ST_SUSPINPROG 4      // thread context being saved (very brief)
#define ST_SUSPENDED 5       // thread is suspended waiting for checkpoint to complete
#define ST_CKPNTHREAD 6      // thread is the checkpointing thread (special state just for that thread)


struct Thread {
  typedef int tid_t;
  Thread *next;                       // next thread in 'threads' list
  Thread **prev;                      // prev thread in 'threads' list

  tid_t      m_tid;                   /// this thread's id as returned by mtcp_sys_kernel_gettid ()
  tid_t      m_originalTID;           /// this is the the thread's "original" tid
  FutexState m_state;                 /// Thread state during the checkpoint

  Thread    *m_parent;                /// parent thread (or NULL if top-level thread)
  Thread    *m_children;              /// one of this thread's child threads
  Thread    *m_siblings;              /// one of this thread's sibling threads

  int        m_cloneFlags;            /// parameters to __clone that created this thread
  int       *m_pParentTID;
  int       *m_pGivenTID;             /// (this is what __clone caller passed in)
  int       *m_pActualTID;            /// (this is what we passed to the system call, either given_tidptr or &child_tid)
  int        m_childTID;              /// this is used for child_tidptr if the original call did not
                                                    // ... have both CLONE_CHILD_SETTID and CLONE_CHILD_CLEARTID
  int (*m_startFunc) (void *arg);     /// thread's initial function entrypoint and argument
  void *m_startArg;

  sigset_t m_sigblockmask;            /// blocked signals
  sigset_t m_sigpending;              /// pending signals
  struct sigaction m_sigactions[NSIG];/// signal handlers

                ///JA: new code ported from v54b
  ucontext_t m_savctx;                /// context saved on suspend

  mtcp_segreg_t fs, gs;               /// thread local storage pointers
  struct user_desc m_tls[1];          ///

  int        m_wait_for_children;
  struct RestartArgs {
    Thread *self;
    pid_t   tid;
  }          m_restartArgs;

  Thread();
  ~Thread();

  static void setupSignals();

  /** Save the system info of the process
   */
  static void saveSysInfo();

  /** Access to the overall thread queue
   */
  static Thread* threads();

  /**  Get current thread struct pointer
   *  It is keyed by the calling thread's gettid value
   *  Maybe improve someday by using TLS
   */
  static Thread* current();

  /** Thread execution wrapper
   *
   *   Simply call the object function.
   */
  static int staticMain(void *thread_ptr);

  /** Entry point to restart thread after a fork.
   */
  static int restartMain(void* arg);

  /** Entry point to restart main thread after a process restore from file.
   */
  static void finishRestore(Thread* t);

  FutexState& state() {  return m_state; }
  int stop();
  /** Thread has exited - unlink it from lists and free struct
   *
   *	thread removed from 'threads' list and motherofall tree
   *	thread pointer no longer valid
   *	checkpointer woken if waiting for this thread
   */
  int cleanup();

  /**  Call this when it's OK to checkpoint. Called by main tread start routine 
   */
  int instrument();

  /** Likewise, disable checkpointing
   */
  int wait();

  char* tlsBase();

  /**  Save state necessary for TLS restore
   *   Linux saves stuff in the GDT, switching it on a per-thread basis
   */
  void  saveTLS();

  void waitForRestore();

  /**  Save signal handlers and block signal delivery
   */
  void saveSignals();

  // Restart methods
  int  restart();

  /** set_tid_address wrapper routine
   *  We save the new address of the tidptr that will get cleared when the thread exits
   */
  long set_tid_address();
  /**  Restore the GDT entries that are part of a thread's state
   *
   *  The kernel provides set_thread_area system call for a thread to alter a particular range of GDT entries, and it switches
   *  those entries on a per-thread basis.  So from our perspective, this is per-thread state that is saved outside user
   *  addressable memory that must be manually saved.
   */
  void restoreTLS();

  /**  Save signal handlers and block signal delivery
   */
  void restoreSignals();

  /// Access thread identifier
  int tid()  const {   return m_tid; }

  /** Main thread execution function
   *
   *  This routine is called (via clone) as the top-level routine of a thread that we are tracking.
   *
   *  It fills in remaining items of our thread struct, calls the user function, then cleans up the thread struct before exiting.
   */
  int Main();

  /** Link thread struct to the lists and finish filling the thread structure.
   *
   *	thread linked to 'threads' list and 'motherofall' tree
   *	thread->tid = filled in with thread id
   *	thread->state = ST_RUNDISABLED (thread initially has checkpointing disabled)
   *	signal handler set up
   */
  void initialize(bool mother_of_all=false);


};
#endif // PROCESSFORK_THREAD_H
