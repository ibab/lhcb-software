#include "Checkpointing/Namespace.h"
#include "Checkpointing/SysCalls.h"

using namespace Checkpointing;

#include <cstdarg>
#include <unistd.h>

static int s_mtcp_sys_errno = 0;
static int s_mtcp_debug_syscalls = MTCP_ERROR;
static char const s_mtcp_hexdigits[] = "0123456789ABCDEF";

STATIC(void) __mtcp_abort(void)    {
  __asm__ volatile (CLEAN_FOR_64_BIT(hlt ; xor %eax,%eax ; mov (%eax),%eax) );
}

STATIC(void) rwrite(char const *buff, int size)   {
  int offs, rc;
  for (offs = 0; offs < size; offs += rc) {
    rc = mtcp_sys_write(STDOUT_FILENO, buff + offs, size - offs);
    if (rc <= 0) break;
  }
}

STATIC(int) bwrite(char* text, int len, int off, char const *buff, int size)   {
  text += off;
  len -= off;
  for(; len>0 && size>0; --size, --len, ++off, ++text, ++buff)  {
    *text = *buff;
  }
  return off;
}

STATIC(int) mtcp_print_int_dec(char* text, int len, int off, int n) {
  char buff[16];
  int i = sizeof(buff);
  int neg = (n < 0);
  if (neg) n = - n;
  do {
    buff[--i] = (n % 10) + '0';
    n /= 10;
  } while (n > 0);
  if (neg) buff[--i] = '-';
  return bwrite (text, len, off, buff + i, sizeof(buff) - i);
}

STATIC(int) mtcp_print_int_unsigned(char* text, int len, int off, unsigned int n) {
  char buff[16];
  int i = sizeof(buff);
  do {
    buff[--i] = (n % 10) + '0';
    n /= 10;
  } while (n > 0);
  return bwrite (text,len,off, buff + i, sizeof(buff) - i);
}

STATIC(int) mtcp_print_int_oct(char* text, int len, int off, unsigned int n) {
  char buff[16];
  int i = sizeof(buff);
  do {
    buff[--i] = (n & 7) + '0';
    n /= 8;
  } while (n > 0);
  return bwrite (text, len, off, buff + i, sizeof(buff) - i);
}

STATIC(int) mtcp_print_int_hex(char* text, int len, int off, unsigned int n) {
  char buff[16];
  int i = sizeof(buff);
  do {
    buff[--i] = s_mtcp_hexdigits[n%16];
    n /= 16;
  } while (n > 0);
  return bwrite (text, len, off, buff + i, sizeof(buff) - i);
}

STATIC(int) mtcp_print_addr(char* text, int len, int off, VA n) {
  char buff[16];
  int i = sizeof(buff);
  do {
    buff[--i] = s_mtcp_hexdigits[n%16];
    n /= 16;
  } while (n > 0);
  buff[--i] = 'x';
  buff[--i] = '0';
  return bwrite (text, len, off, buff + i, sizeof(buff) - i);
}

STATIC(void) mtcp_output(int lvl,char const *format, ...)  {
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
  va_start (ap, format);
  char buff[400];
  size_t str_len = 0;
  if ( hdr )   {
    if ( s_mtcp_debug_syscalls&MTCP_PRINT_NO_PID ) {
      str_len=bwrite (buff, sizeof(buff), str_len,"mtcp ",5);
    }
    else {
      str_len=bwrite(buff,sizeof(buff),str_len,"mtcp[",5);
      str_len=mtcp_print_int_dec(buff,sizeof(buff),str_len,mtcp_sys_getpid());
      str_len=bwrite (buff,sizeof(buff),str_len,"] ",2);
    }
    switch(lvl) {
    case MTCP_DEBUG:   str_len=bwrite(buff,sizeof(buff), str_len," DEBUG   ",9); break;
    case MTCP_INFO:    str_len=bwrite(buff,sizeof(buff), str_len," INFO    ",9); break;
    case MTCP_WARNING: str_len=bwrite(buff,sizeof(buff), str_len," WARNING ",9); break;
    case MTCP_ERROR:   str_len=bwrite(buff,sizeof(buff), str_len," ERROR   ",9); break;
    case MTCP_FATAL:   str_len=bwrite(buff,sizeof(buff), str_len," FATAL   ",9); break;
    case MTCP_ALWAYS:  str_len=bwrite(buff,sizeof(buff), str_len," ALWAYS  ",9); break;
    default:           str_len=bwrite(buff,sizeof(buff), str_len," ALWAYS  ",9); break;
    }
  }

  // Scan along until we find a % 
  for(p = format; (q = helper::chrfind(p,'%')) != 0; p = ++ q) {
    /* Print all before the % as is */
    if (q > p) str_len = bwrite (buff, sizeof(buff), str_len, p, q - p);
    /* Process based on character following the % */
  gofish:
    switch (*(++ q)) {
    case 'l': {
      goto gofish;
    }
    case '0' ... '9': {      // Ignore digits (field width)
      goto gofish;
    }
    case 'c': {      // Single character
      char chars[4];
      chars[0] = va_arg (ap, int); // va_arg (ap, char);
      str_len = bwrite (buff, sizeof(buff), str_len, chars, 1);
      break;
    }

    case 'd': {      // Signed decimal integer
      int n = va_arg (ap, int);
      str_len=mtcp_print_int_dec(buff, sizeof(buff), str_len, n);
      break;
    }

    case 'u': {      // Unsigned decimal integer
      unsigned int n = va_arg (ap, unsigned int);
      str_len=mtcp_print_int_unsigned(buff, sizeof(buff), str_len, n);
      break;
    }

    case 'o': {      // Unsigned octal number
      unsigned int n = va_arg (ap, unsigned int);
      str_len=mtcp_print_int_oct(buff, sizeof(buff), str_len, n);
      break;
    }

    case 'X':
    case 'x': {      // Unsigned hexadecimal number
      unsigned int n = va_arg (ap, unsigned int);
      str_len=mtcp_print_int_hex(buff, sizeof(buff), str_len, n);
      break;
    }

    case 'p': {      // Address in hexadecimal
      VA n = (VA)va_arg (ap, void*);
      str_len=mtcp_print_addr(buff, sizeof(buff), str_len, n);
      break;
    }

    case 's': {      // Null terminated string
      char* n = va_arg (ap, char*);
      str_len=bwrite (buff, sizeof(buff), str_len, n, helper::stringlen(n));
      break;
    }

    default:         // Anything else, print the character as is
      str_len=bwrite (buff, sizeof(buff), str_len, q, 1);
      break;
    }
  }
  va_end (ap);

  // Print whatever comes after the last format spec
  str_len = bwrite(buff,sizeof(buff), str_len,p,helper::stringlen(p));
  buff[sizeof(buff)-2] = '\n';
  buff[sizeof(buff)-1] = 0;

  //#define CHECKPOINTING_MUTEXED_WRITE 1
#if defined(CHECKPOINTING_MUTEXED_WRITE)
  static volatile int printflocked(0);
 again:
  while (printflocked != 0) mtcp_sys_nanosleep(100);
  if ( printflocked != 0 ) goto again;
  printflocked = 1;
  rwrite(buff,helper::stringlen(buff));
  printflocked = 0;
#else
  rwrite(buff,helper::stringlen(buff));
#endif
  if ( lvl >= MTCP_FATAL )   {
    const char* mp = "Calling abort due to internal errors.\n";
    rwrite(mp,helper::stringlen(mp));
    __mtcp_abort();
  }
}

STATIC(int*) __mtcp_sys_errno() {
  return &s_mtcp_sys_errno;
}

STATIC(void) mtcp_abort(void)    {
  mtcp_output(MTCP_ERROR,"Calling abort due to internal errors.\n");
  __mtcp_abort();
}

STATIC(int) mtcp_get_debug_level() {
  return s_mtcp_debug_syscalls;
}

STATIC(void) mtcp_set_debug_level(int lvl)   {
  if ( (s_mtcp_debug_syscalls&MTCP_MAX_LEVEL) < MTCP_INFO )    {
    mtcp_output(MTCP_ERROR,"Change debug level from %d to:%d\n",s_mtcp_debug_syscalls,lvl);
  }
  s_mtcp_debug_syscalls = lvl;
}
