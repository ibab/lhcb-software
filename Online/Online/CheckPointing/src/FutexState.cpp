#include "CheckPointing/FutexState.h"
#include "CheckPointing/SysCalls.h"
#include "CheckPointing.h"
#include "linux/futex.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static const MtcpState futex_init_state = MTCP_STATE_INITIALIZER;

/* cmpxchgl is only supported on Intel 486 processors and later. */
static inline int atomic_setif_int (int volatile *loc, int newval, int oldval)   {
  char rc;
  asm volatile ("lock ; cmpxchgl %2,%1 ; sete %%al" : "=a" (rc) : "m" (*loc), "r" (newval), "a" (oldval) : "cc", "memory");
  return (rc);
}

static inline int atomic_setif_ptr (void *volatile *loc, void *newval, void *oldval)  {
  char rc;
  asm volatile ("lock ; cmpxchg %2,%1 ; sete %%al" : "=a" (rc) : "m" (*loc), "r" (newval), "a" (oldval) : "cc", "memory");
  return (rc);
}

static inline int mtcp_futex (int *uaddr, int op, int val,
                              const struct timespec *timeout)

{
#if defined(__x86_64__)
  int rc;
  register long int a1 asm ("rdi") = (long int)uaddr;
  register long int a2 asm ("rsi") = (long int)op;
  register long int a3 asm ("rdx") = (long int)val;
  register long int a4 asm ("r10") = (long int)timeout;
  register long int a5 asm ("r8")  = (long int)0;

  asm volatile ("syscall"
                : "=a" (rc)
                : "0" (__NR_futex),
                  "r" (a1), "r" (a2), "r" (a3), "r" (a4), "r" (a5)
                : "memory", "cc", "r11", "cx");
  return (rc);
#elif defined(__i386__)
  int rc;

  asm volatile ("int $0x80"
                : "=a" (rc)
                : "0" (__NR_futex),
                  "b" (uaddr), "c" (op), "d" (val), "S" (timeout), "D" (0)
                : "memory", "cc");
  return (rc);
#else
#error need to define mtcp_futex
#endif
}

FutexState::FutexState() : m_state(futex_init_state) {
}

int FutexState::set(int newval,int oldval) {
  return atomic_setif_int(&(m_state.value), newval, oldval); 
}

void FutexState::wait(int func, int val, struct timespec const *timeout) {
  int rc;
  // (int *) cast needed since state->value is "int volatile"  - Gene
  while ((rc = mtcp_futex ((int *)&m_state.value, func, val, timeout)) < 0) {
    rc = -rc;
    if ((rc == ETIMEDOUT) || (rc == EWOULDBLOCK)) break;
    if (rc != EINTR) {
      mtcp_output(MTCP_FATAL,"FutexState::wait: futex error %d: %s\n"
		  "FutexState::wait: (%p, %d, %d, %p, NULL, 0)\n", rc, strerror (rc),
		  &m_state.value, func, val, timeout);
    }
  }
}
