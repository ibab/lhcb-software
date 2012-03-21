#ifndef FUTEXSTATE_H
#define FUTEXSTATE_H
#include <pthread.h>

#define MTCP_STATE_INITIALIZER {0}

struct FutexState {
  int volatile value;
  FutexState();
  FutexState(int val);
  void destroy()         {                          }
  int  set(int new_value, int old_value);
  void wait(int val, struct timespec const *timeout=0);
  void wake(int val=1);
};
#endif // FUTEXSTATE_H

