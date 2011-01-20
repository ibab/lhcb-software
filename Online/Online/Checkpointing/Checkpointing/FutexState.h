#ifndef FUTEXSTATE_H
#define FUTEXSTATE_H
#include <pthread.h>

#define MTCP_STATE_INITIALIZER {0}

struct MtcpState {
  int volatile value;
};

struct FutexState {
  MtcpState m_state;
  FutexState();
  FutexState(int val);
  int init(int val)      { m_state.value = val;   return 1; }
  void destroy()         {                                  }
  int value()  const     {  return m_state.value;           }
  int set(int new_value, int old_value);
  void wait(int val, struct timespec const *timeout=0);
  void wake(int val=1);
};
#endif // FUTEXSTATE_H

