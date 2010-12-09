#ifndef FUTEXSTATE_H
#define FUTEXSTATE_H
#include <pthread.h>

#define MTCP_STATE_INITIALIZER {0, PTHREAD_COND_INITIALIZER, PTHREAD_MUTEX_INITIALIZER }

struct MtcpState {
  int volatile value;
  pthread_cond_t cond;
  pthread_mutex_t mutex;
};

struct FutexState {
  MtcpState m_state;
  FutexState();
  int init(int val)      { m_state.value = val;   return 1; }
  void destroy()         {                                  }
  int value()  const     {  return m_state.value;           }
  int set(int new_value, int old_value);
  void wait(int func, int val, struct timespec const *timeout);
};
#endif // FUTEXSTATE_H

