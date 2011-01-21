#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include "Checkpointing/Namespace.h"
//#include "OnlineBase/CPP/CheckpointRestoreWrapper.h"

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
  /// Set the printout level for the checkpoint/restore mechanism
  virtual int setPrint(int new_level) = 0;
  /// Give access to the restart type after a call to "checkpoint". 0==checkpointing, 1=restart from checkpoint
  virtual int restartType() const = 0;
  /// Stop all threads to allow producing a consistent checkpoint
  virtual int stop()            = 0;
  //virtual void     stop() = 0;
  /// Resume all threads after the checkpoint was produced
  virtual int resume()          = 0;
  /// Fork child instance
  virtual int forkInstance()    = 0;
  /// Child fork: Re-establish child to running state
  virtual int startChild()      = 0;
  
  /// Create a checkpoint file by writing to file descriptor fd
  virtual int checkpoint(int fd)= 0;

  /// Save file descriptor information to opaque memory block (allocated inside!)
  virtual int getFileDescriptors(void** ptr) = 0;
  /// Restore file descriptors from opaque memory block
  virtual int setFileDescriptors(void* ptr) = 0;
  /// For debugging: Dump file descriptors to stdout
  virtual int dumpFileDescriptors(void* ptr) = 0;
};


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
  class MainThread : public CheckpointRestoreWrapper {
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
    /// Set the printout level for the checkpoint/restore mechanism
    virtual int     setPrint(int new_level);
    /// Give access to the restart type after a call to "checkpoint". 0==checkpointing, 1=restart from checkpoint
    virtual int restartType() const;

    /// Stop all threads to allow producing a consistent checkpoint
    virtual int     stop();
    //virtual void     stop();
    /// Resume all threads after the checkpoint was produced
    virtual int     resume();

    virtual void    initialize();

    /// Fork child instance
    virtual int     forkInstance();
    /// Child fork: Re-establish child to running state
    virtual int     startChild();

    /// Create a checkpoint file by writing to file descriptor fd
    virtual int     checkpoint(int fd);

    /// Save file descriptor information to opaque memory block (allocated inside!)
    virtual int     getFileDescriptors(void** ptr);
    /// Restore file descriptors from opaque memory block
    virtual int     setFileDescriptors(void* ptr);
    /// For debugging: Dump file descriptors to stdout
    virtual int     dumpFileDescriptors(void* ptr);

    virtual void    showSigActions();

    static int      currentThreadID();
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
