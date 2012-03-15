#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include "Checkpointing/Namespace.h"

// Forward declarations
struct user_desc;

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
  class MainThread  {
    /// Standard constructor
    MainThread();
  public:
    typedef int (*clone_t)(int (*fn) (void *arg),
			   void *child_stack,
			   int flags,
			   void *arg,
			   int *parent_tidptr,
			   struct user_desc *newtls,
			   int *child_tidptr);

    /// Static instance accessor
    static MainThread& accessInstance();

    /// Initialization callback with startup arguments
    void init_instance(int argc, char** argv, char** environ);
    void    initialize();
    static void     finishRestore();
    static clone_t  cloneFunction();
  };

}  // End namespace Checkpointing

// LibC's original clone call
#define CLONE_CALL (MainThread::cloneFunction())
// The signal question:
// SIGUSR1/SIGUSR2 may be used by pthread library
// The same for SIGRTMIN,SIGRTMIN+1
// Hence use something higher....
#define STOPSIGNAL  52

#endif // MAINTHREAD_H
