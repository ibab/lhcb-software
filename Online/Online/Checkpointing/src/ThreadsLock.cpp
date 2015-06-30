#include "Checkpointing/ThreadsLock.h"
#include "Checkpointing/MB.h"
#include "linux/futex.h"
#include "Restore.h"

static FutexState s_threadslocked(0);

STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_threads_unlock() {
  WMB; // flush data written before unlocking
  s_threadslocked.set(0, 1);
  s_threadslocked.wake(1);
}

STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_threads_lock() {
  while (!s_threadslocked.set(1, 0))
    s_threadslocked.wait(1, 0);
  RMB; // don't prefetch anything until we have the lock
}

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
