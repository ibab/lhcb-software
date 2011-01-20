#include "Checkpoining/ThreadsLock.h"
#include "Checkpoining/MB.h"
#include "linux/futex.h"

static FutexState s_threadslocked;

void ThreadsLock::take()    {
  while (!s_threadslocked.set(1, 0))
    s_threadslocked.wait(1, 0);
  RMB; // don't prefetch anything until we have the lock
}

void ThreadsLock::release()   {
  WMB; // flush data written before unlocking
  s_threadslocked.set(0, 1);
  s_threadslocked.wake(1);
}
