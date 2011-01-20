#ifndef THREADLOCK_H
#define THREADLOCK_H
#include "FutexState.h"

class ThreadsLock {
public:
  static void take();
  static void release();

  ThreadsLock()  {  ThreadsLock::take();   }
  ~ThreadsLock() {  ThreadsLock::release();   }
};
#endif // THREADLOCK_H
