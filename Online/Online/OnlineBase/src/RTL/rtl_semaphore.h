#include "rtl_internal.h"
#include <cerrno>

#if defined(USE_PTHREADS)
#include <fcntl.h>

static inline sem_t *i_sem_open(const char *name, int oflag, int mode, int val) {
  sem_t* h = SEM_FAILED;
  do {
    h = ::sem_open(name,oflag,mode,val);
  } while ( h==SEM_FAILED && errno==EINTR );
  return h;
}
static inline int i_sem_wait(sem_t *sem) {
  int res = -1;
  do {
    res = ::sem_wait(sem);
  } while ( res==-1 && errno==EINTR );
  return res;
}
static inline int i_sem_trywait(sem_t *sem) {
  int res = -1;
  //do {
  res = ::sem_trywait(sem);
  //} while ( res==-1 && errno==EINTR );
  return res;
}
static inline int i_sem_timedwait(sem_t *sem, timespec* sp) {
  int res = -1;
  //do {
  res = ::sem_timedwait(sem, sp);
  //} while ( res==-1 && errno==EINTR );
  return res;
}
#endif
