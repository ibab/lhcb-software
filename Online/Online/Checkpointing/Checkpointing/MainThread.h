#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include "Checkpointing/Namespace.h"
#include "setjmp.h"
#include "signal.h"

// Forward declarations
struct user_desc;
/* @class CheckpointRestoreWrapper CheckpointRestoreWrapper.h CPP/CheckpointRestoreWrapper.h
 *
 * Basic interface to interface to checkpoint / restore mechanisms for fast program startup.
 *
 * NOTE:
 * This header structure is explicitly copied to the checkpoint/restart package(s)
 * To decouple them entirely from the application
 *
 * !!!! DO NOT EVEN THINK ABOUT CHANGING IT WITHOUT KNOWNING WHAT YOU DO !!!!
 *
 * @author  M.Frank
 * @version 1.0
 */
class CheckpointRestoreWrapper  {
 protected:
  /// Standard constructor
  CheckpointRestoreWrapper() {}
  /// Standard destructor
  virtual ~CheckpointRestoreWrapper() {}
 public:
  /// Initialization callback with startup arguments
  virtual void init_instance(int argc, char** argv, char** environ) = 0;
};

/*
 * Checkpointing namespace declaration
 */
namespace CHECKPOINTING_NAMESPACE {

  /// Forward declarations
  class Thread;

  /**@struct MainThread
   *
   * @author  M.Frank
   * @version 1.0
   */
  class MainThread : public CheckpointRestoreWrapper  {
    /// Standard constructor
    MainThread();
  public:
    struct __attribute__ ((__aligned__)) jmp_buf {
      __jmp_buf buf;
      int __mask_was_saved;
    };
    struct __attribute__ ((__aligned__)) thread_unwind_buf_t  {
      jmp_buf buf[1];
      void *__pad[4];
    };

    typedef int (*clone_t)(int (*fn) (void *arg),
			   void *child_stack,
			   int flags,
			   void *arg,
			   int *parent_tidptr,
			   struct user_desc *newtls,
			   int *child_tidptr);
    typedef int  (*sigsetjmp_t)(jmp_buf *env, int mask);
    typedef int  (*sigaction_t)(int signum, const struct sigaction *act, struct sigaction *oldact);

    typedef int  (*pthread_create_t)(void* p1, void* p2, void* (*f)(void*), void* p3);
    typedef void (*pthread_cancel_t)(thread_unwind_buf_t* cancel_buffer);

    typedef struct  {
      clone_t _clone;
      sigsetjmp_t _sigsetjmp;
      sigaction_t _sigaction;
      pthread_create_t _thread_create;
      pthread_cancel_t _thread_unwind_next;
      pthread_cancel_t _thread_register_cancel;
      pthread_cancel_t _thread_unregister_cancel;
    } funcs_t;

    /// Static instance accessor
    static MainThread& accessInstance();

    /// Initialization callback with startup arguments
    void init_instance(int argc, char** argv, char** environ);
    void initialize();
    static void     finishRestore();
    static void*    execute(void* arg);
    static funcs_t* F();
  };

}  // End namespace Checkpointing

// LibC's original clone call
#define CLONE_CALL (MainThread::F()->_clone)
// The signal question:
// SIGUSR1/SIGUSR2 may be used by pthread library
// The same for SIGRTMIN,SIGRTMIN+1
// Hence use something higher....
#define STOPSIGNAL  52

#endif // MAINTHREAD_H
