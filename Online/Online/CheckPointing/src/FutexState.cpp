#include "Checkpoining/FutexState.h"
#include "Checkpoining.h"
#include "linux/futex.h"
#include <syscall.h>
#include <cstring>
#include <cerrno>

static const MtcpState futex_init_state = MTCP_STATE_INITIALIZER;

using namespace Checkpoining;

// cmpxchgl is only supported on Intel 486 processors and later.
static inline int atomic_setif_int (int volatile *loc, int newval, int oldval)   {
  char rc;
  asm volatile ("lock ; cmpxchgl %2,%1 ; sete %%al" : "=a" (rc) : "m" (*loc), "r" (newval), "a" (oldval) : "cc", "memory");
  return rc;
}

static inline int atomic_setif_ptr (void *volatile *loc, void *newval, void *oldval)  {
  char rc;
  asm volatile ("lock ; cmpxchg %2,%1 ; sete %%al" : "=a" (rc) : "m" (*loc), "r" (newval), "a" (oldval) : "cc", "memory");
  return rc;
}

static inline int mtcp_futex(int *uaddr, int op, int val, const struct timespec *timeout)  {
  int rc;
  register long int a1 asm ("rdi") = (long int)uaddr;
  register long int a2 asm ("rsi") = (long int)op;
  register long int a3 asm ("rdx") = (long int)val;
  register long int a4 asm ("r10") = (long int)timeout;
  register long int a5 asm ("r8")  = (long int)0;

  asm volatile ("syscall"
                : "=a" (rc)
                : "0" (SYS_futex),
                  "r" (a1), "r" (a2), "r" (a3), "r" (a4), "r" (a5)
                : "memory", "cc", "r11", "cx");
  return rc;
}

FutexState::FutexState() : m_state(futex_init_state) {
}

FutexState::FutexState(int val) :  m_state(futex_init_state) {
  init(val);
}

WEAK(int) FutexState::set(int newval,int oldval) {
  return atomic_setif_int(&(m_state.value), newval, oldval); 
}

WEAK(void) FutexState::wait(int val, struct timespec const *timeout) {
  int rc;
  // (int *) cast needed since state->value is "int volatile"  - Gene
  while ((rc=mtcp_futex((int*)&m_state.value, FUTEX_WAIT, val, timeout)) < 0)   {
    rc = -rc;
    if ((rc == ETIMEDOUT) || (rc == EWOULDBLOCK)) break;
    if (rc != EINTR) {
      mtcp_output(MTCP_FATAL,"FutexState::wait: futex error %d: %s "
		  "FutexState::wait: (%p[%d], WAIT, %d, %p, NULL, 0)\n", rc, ::strerror(rc),
		  &m_state.value, m_state.value, val, timeout);
    }
  }
}

WEAK(void) FutexState::wake(int val)    {
  int rc;
  // (int *) cast needed since state->value is "int volatile"  - Gene
  while ((rc=mtcp_futex((int*)&m_state.value, FUTEX_WAKE, val, 0)) < 0)   {
    rc = -rc;
    if ((rc == ETIMEDOUT) || (rc == EWOULDBLOCK)) break;
    if (rc != EINTR) {
      mtcp_output(MTCP_FATAL,"FutexState::wake: futex error %d: %s: "
		  "FutexState::wait: (%p[%d], WAKE, %d, 0, 0, 0)\n", rc, ::strerror(rc),
		  &m_state.value, m_state.value, val);
    }
  }
}
