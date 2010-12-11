#ifndef MAINTHREAD_H
#define MAINTHREAD_H

//#include "OnlineBase/CPP/CheckpointRestoreWrapper.h"

class CheckpointRestoreWrapper  {
 protected:
  CheckpointRestoreWrapper() {}
 public:
  virtual int stop()            = 0;
  virtual int resume()          = 0;
  virtual int forkInstance()    = 0;
  virtual int startChild()      = 0;
  virtual int checkpoint(int fd)= 0;
  virtual int restore(int fd)   = 0;
  virtual int restart()         = 0;
  virtual int getFileDescriptors(void** ptr) = 0;
  virtual int setFileDescriptors(void* ptr) = 0;
  virtual int dumpFileDescriptors(void* ptr) = 0;
};

class Thread;

typedef int (*libc_clone_entry_t) (int (*fn) (void *arg),
				   void *child_stack,
				   int flags,
				   void *arg,
				   int *parent_tidptr,
				   struct user_desc *newtls,
				   int *child_tidptr);

class MainThread : public CheckpointRestoreWrapper {
  MainThread();
 public:
  static MainThread& accessInstance();
  virtual int     stop();
  virtual int     resume();
  virtual void    initialize();
  virtual int     forkInstance();
  virtual int     startChild();
  virtual int     stopSignal();
  virtual int     motherPID() const;
  virtual Thread* motherOfAll() const;
  virtual libc_clone_entry_t cloneCall() const;

  virtual int checkpoint(int fd);
  virtual int restore(int fd);
  virtual int restart();

  virtual int getFileDescriptors(void** ptr);
  virtual int setFileDescriptors(void* ptr);
  virtual int dumpFileDescriptors(void* ptr);

  static int     currentThreadID();
  static void    finishRestore();
};

#define STOPSIGNAL (MainThread::accessInstance().stopSignal())
#define CLONE_CALL (MainThread::accessInstance().cloneCall())
#endif // MAINTHREAD_H
