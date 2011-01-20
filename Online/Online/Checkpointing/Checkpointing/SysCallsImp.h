#include "Checkpointing/Namespace.h"
#include "Checkpointing/SysCalls.h"

using namespace Checkpointing;

#include <cstdarg>
#include <unistd.h>

static int s_mtcp_sys_errno = 0;
static int s_mtcp_debug_syscalls = MTCP_ERROR;
static char const s_mtcp_hexdigits[] = "0123456789ABCDEF";

WEAK(int&) __mtcp_sys_errno() {
  return s_mtcp_sys_errno;
}

WEAK(void) mtcp_abort(void)    {
  mtcp_output(MTCP_ERROR,"Calling abort due to internal errors.\n");
  asm volatile (CLEAN_FOR_64_BIT(hlt ; xor %eax,%eax ; mov (%eax),%eax) );
}

WEAK(int) mtcp_get_debug_level() {
  return s_mtcp_debug_syscalls;
}

WEAK(void) mtcp_set_debug_level(int lvl)   {
  if ( (s_mtcp_debug_syscalls&MTCP_MAX_LEVEL) < MTCP_INFO )    {
    mtcp_output(MTCP_ERROR,"Change debug level from %d to:%d\n",s_mtcp_debug_syscalls,lvl);
  }
  s_mtcp_debug_syscalls = lvl;
}

STATIC(void) rwrite(char const *buff, int size)   {
  int offs, rc;
  for (offs = 0; offs < size; offs += rc) {
    rc = mtcp_sys_write(STDOUT_FILENO, buff + offs, size - offs);
    if (rc <= 0) break;
  }
}

STATIC(void) mtcp_print_int_dec(int n) {
  char buff[16];
  int i = sizeof(buff);
  int neg = (n < 0);
  if (neg) n = - n;
  do {
    buff[--i] = (n % 10) + '0';
    n /= 10;
  } while (n > 0);
  if (neg) buff[--i] = '-';
  rwrite (buff + i, sizeof(buff) - i);
}

STATIC(void) mtcp_print_int_unsigned(unsigned int n) {
  char buff[16];
  int i = sizeof(buff);
  do {
    buff[--i] = (n % 10) + '0';
    n /= 10;
  } while (n > 0);
  rwrite (buff + i, sizeof(buff) - i);
}

STATIC(void) mtcp_print_int_oct(unsigned int n) {
  char buff[16];
  int i = sizeof(buff);
  do {
    buff[--i] = (n & 7) + '0';
    n /= 8;
  } while (n > 0);
  rwrite (buff + i, sizeof(buff) - i);
}
STATIC(void) mtcp_print_int_hex(unsigned int n) {
  char buff[16];
  int i = sizeof(buff);
  do {
    buff[--i] = s_mtcp_hexdigits[n%16];
    n /= 16;
  } while (n > 0);
  rwrite (buff + i, sizeof(buff) - i);
}

STATIC(void) mtcp_print_addr(VA n) {
  char buff[16];
  int i = sizeof(buff);
  do {
    buff[--i] = s_mtcp_hexdigits[n%16];
    n /= 16;
  } while (n > 0);
  buff[--i] = 'x';
  buff[--i] = '0';
  rwrite (buff + i, sizeof(buff) - i);
}

WEAK(void) mtcp_output(int lvl,char const *format, ...)  {
#if defined(CHECKPOINTING_MUTEXED_WRITE)
  static FutexState printflocked(0);
#endif
  struct helper {
    static inline const char* chrfind(const char* s, const char pattern) {
      for(; *s; ++s)
	if (*s==pattern) return s;
      return 0;
    }
    static inline size_t stringlen(const char* s) {
      size_t l = 0;
      for(; *s; ++s) ++l;
      return l;
    }
  };
  char const *p, *q;
  va_list ap;
  bool hdr = (lvl&MTCP_NO_HEADER) == 0;
  lvl &= MTCP_MAX_LEVEL;

  if ( ((s_mtcp_debug_syscalls&~MTCP_NO_HEADER)&~MTCP_PRINT_NO_PID) > lvl ) return;
#if defined(CHECKPOINTING_MUTEXED_WRITE)
  while (!printflocked.set(1,0))
    printflocked.wait(1);
#endif
  va_start (ap, format);

  if ( hdr )   {
    if ( s_mtcp_debug_syscalls&MTCP_PRINT_NO_PID ) {
      rwrite ("mtcp ",5);
    }
    else {
      rwrite ("mtcp[",5);
      mtcp_print_int_dec(mtcp_sys_getpid());
      rwrite ("] ",3);
    }
    switch(lvl) {
    case MTCP_DEBUG:   rwrite(" DEBUG   ",10); break;
    case MTCP_INFO:    rwrite(" INFO    ",10); break;
    case MTCP_WARNING: rwrite(" WARNING ",10); break;
    case MTCP_ERROR:   rwrite(" ERROR   ",10); break;
    case MTCP_FATAL:   rwrite(" FATAL   ",10); break;
    default:           rwrite(" ALWAYS  ",10); break;
    }
    //rwrite (",",1);
    //mtcp_print_int_dec(lvl);
    //rwrite ("]> ",3);
  }

  // Scan along until we find a % 
  for(p = format; (q = helper::chrfind(p, '%')) != 0; p = ++ q) {
    /* Print all before the % as is */
    if (q > p) rwrite (p, q - p);
    /* Process based on character following the % */
gofish:
    switch (*(++ q)) {
    case 'l': {
      goto gofish;
    }
    case '0' ... '9': {      /* Ignore digits (field width) */
      goto gofish;
    }
    case 'c': {      /* Single character */
      char buff[4];
      buff[0] = va_arg (ap, int); // va_arg (ap, char);
      rwrite (buff, 1);
      break;
    }

    case 'd': {      /* Signed decimal integer */
      int n = va_arg (ap, int);
      mtcp_print_int_dec(n);
      break;
    }

    case 'u': {      /* Unsigned decimal integer */
      unsigned int n = va_arg (ap, unsigned int);
      mtcp_print_int_unsigned(n);
      break;
    }

    case 'o': {      /* Unsigned octal number */
      unsigned int n = va_arg (ap, unsigned int);
      mtcp_print_int_oct(n);
      break;
    }

    case 'X':
    case 'x': {      /* Unsigned hexadecimal number */
      unsigned int n = va_arg (ap, unsigned int);
      mtcp_print_int_hex(n);
      break;
    }

    case 'p': {      /* Address in hexadecimal */
      VA n = (VA)va_arg (ap, void*);
      mtcp_print_addr(n);
      break;
    }

    case 's': {      /* Null terminated string */
      p = va_arg (ap, char*);
      rwrite (p, helper::stringlen(p));
      break;
    }

    default:        /* Anything else, print the character as is */
      rwrite (q, 1);
      break;
    }
  }
  va_end (ap);

  // Print whatever comes after the last format spec
  rwrite(p,helper::stringlen (p));

#if defined(CHECKPOINTING_MUTEXED_WRITE)
  printflocked.set(0,1);
  printflocked.wake(1);
#endif
  if ( lvl >= MTCP_FATAL ) mtcp_abort();
}

