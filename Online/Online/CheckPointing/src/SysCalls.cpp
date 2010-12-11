#include "CheckPointing/SysCalls.h"
#include "CheckPointing.h"
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <sys/types.h>
#include <elf.h> // For value of AT_SYSINFO, Elf??_auxv_t
#include <unistd.h>

using namespace CheckPointing;

static int mtcp_debug_syscalls = MTCP_ERROR;

int s_mtcp_sys_errno = 0;
int& __mtcp_sys_errno() {
  return s_mtcp_sys_errno;
}

# define ELF_AUXV_T Elf64_auxv_t
# define UINT_T uint64_t

#define RESET_THREAD_SYSINFO 1
// Some reports say it was 0x18 in past.  Should we also check that?
#define DEFAULT_SYSINFO_OFFSET "0x10"


int mtcp_get_debug_level() {
  return mtcp_debug_syscalls;
}

void mtcp_set_debug_level(int lvl) {
  mtcp_debug_syscalls = lvl;
}

static void * get_at_sysinfo() {
  void **stack;
  int i;
  ELF_AUXV_T *auxv;
  static char **my_environ = 0;

  if (my_environ == 0)
    my_environ = environ;
#if 0
  // Walk the stack.
  asm volatile (CLEAN_FOR_64_BIT(mov %%ebp, %0\n\t)
                : "=g" (stack) );
  mtcp_output(MTCP_INFO,"stack 2: %p\n", stack);

  // When popping stack/%ebp yields zero, that's the ELF loader telling us that
  // this is "_start", the first call frame, which was created by ELF.
  for ( ; *stack != 0; stack = *stack )
    ;

  // Go beyond first call frame:
  // Next look for &(argv[argc]) on stack;  (argv[argc] == 0)
  for (i = 1; stack[i] != 0; i++)
    ;
  // Do some error checks
  if ( &(stack[i]) - stack > 100000 ) {
    mtcp_output(MTCP_INFO,"Error:  overshot stack\n");
    exit(1);
  }
  stack = &stack[i];
#else
  stack = (void **)&my_environ[-1];
  if (*stack != 0) {
    mtcp_output(MTCP_INFO,"This should be argv[argc] == 0 and it's not.\n"
	"NO &argv[argc], stack: %p\n", stack);
    exit(1);
  }
#endif
  // stack[-1] should be argv[argc-1]
  if ( (void **)stack[-1] < stack || (void **)stack[-1] > stack + 100000 ) {
    mtcp_output(MTCP_INFO,"candidate argv[argc-1] failed consistency check\n");
    exit(1);
  }
  for (i = 1; stack[i] != 0; i++)
    if ( (void **)stack[i] < stack || (void **)stack[i] > stack + 10000 ) {
      mtcp_output(MTCP_INFO,"candidate argv[%d] failed consistency check\n", i);
      exit(1);
    }
  stack = &stack[i+1];
  // Now stack is beginning of auxiliary vector (auxv)
  // auxv->a_type = AT_NULL marks the end of auxv
  for (auxv = (ELF_AUXV_T *)stack; auxv->a_type != AT_NULL; auxv++) {
    // mtcp_output(MTCP_INFO,"0x%x 0x%x\n", auxv->a_type, auxv->a_un.a_val);
    if ( auxv->a_type == (UINT_T)AT_SYSINFO ) {
      mtcp_output(MTCP_INFO,"AT_SYSINFO      (at 0x%p) is:  0x%lx\n",
        &auxv->a_un.a_val, auxv->a_un.a_val);
      return (void *)auxv->a_un.a_val;
    }
  }
  return 0;  /* Couldn't find AT_SYSINFO */
}

int mtcp_have_thread_sysinfo_offset() {
#ifdef RESET_THREAD_SYSINFO
  static int result = -1; // Reset to 0 or 1 on first call.
#else
  static int result = 0;
#endif
  result = 0;
  if (result == -1) {
    void * sysinfo;
    asm volatile (CLEAN_FOR_64_BIT(mov %%gs:) DEFAULT_SYSINFO_OFFSET ", %0\n\t" : "=r" (sysinfo));
    result = (sysinfo == get_at_sysinfo());
  }
  return result;
}

// AT_SYSINFO is what kernel calls sysenter address in vdso segment.
// Kernel saves it for each thread in %gs:SYSINFO_OFFSEt ??
//  as part of kernel TCB (thread control block) at beginning of TLS ??
void *mtcp_get_thread_sysinfo() {
  void *sysinfo;
  asm volatile (CLEAN_FOR_64_BIT(mov %%gs:) DEFAULT_SYSINFO_OFFSET ", %0\n\t"
                : "=r" (sysinfo) );
  return sysinfo;
}

void mtcp_set_thread_sysinfo(void *sysinfo) {
  asm volatile (CLEAN_FOR_64_BIT(mov %0, %%gs:) DEFAULT_SYSINFO_OFFSET "\n\t"
                : : "r" (sysinfo) );
}

void mtcp_abort (void)    {
  asm volatile (CLEAN_FOR_64_BIT(hlt ; xor %eax,%eax ; mov (%eax),%eax) );
}

#if 0

void mtcp_printf(const char* fmt,...) {
  printf("PID:%ld :",mtcp_sys_getpid());
  va_list ap;
  va_start(ap,fmt);
  vprintf(fmt,ap);
  va_end(ap);
}
void mtcp_output(int lvl, const char* fmt,...) {
  if ( mtcp_debug_syscalls <= lvl ) {
    printf(" PID:%ld :",mtcp_sys_getpid());
    va_list ap;
    va_start(ap,fmt);
    vprintf(fmt,ap);
    va_end(ap);
  }
  if ( lvl >= MTCP_FATAL ) mtcp_abort();
}
#endif

#include <stdarg.h>
#include <string.h>
#include <unistd.h>

// Force mtcp_sys.h to define this.
#define MTCP_SYS_STRLEN
#define MTCP_SYS_STRCHR


int dmtcp_info_stderr_fd = 2;
/* For the default value of -1, mtcp_printf() should not go to jassertlogs */
int dmtcp_info_jassertlog_fd = -1;

static char const hexdigits[] = "0123456789ABCDEF";
//static MtcpState printflocked = MTCP_STATE_INITIALIZER;

static void rwrite (char const *buff, int size);


void mtcp_output(int lvl,char const *format, ...)  {
  char const *p, *q;
  va_list ap;
  if ( mtcp_debug_syscalls > lvl ) return;
#if 0
  while (!mtcp_state_set (&printflocked, 1, 0)) {
    mtcp_state_futex (&printflocked, FUTEX_WAIT, 1, NULL);
  }
#endif
  va_start (ap, format);
  /* Scan along until we find a % */
  for (p = format; (q = m_chrfind(p, '%')) != NULL; p = ++ q) {
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
      char buff[16];
      int i = sizeof buff;
      int n = va_arg (ap, int);
      int neg = (n < 0);
      if (neg) n = - n;
      do {
	buff[--i] = (n % 10) + '0';
	n /= 10;
      } while (n > 0);
      if (neg) buff[--i] = '-';
      rwrite (buff + i, sizeof buff - i);
      break;
    }

    case 'o': {      /* Unsigned octal number */
      char buff[16];
      int i = sizeof buff;
      unsigned int n = va_arg (ap, unsigned int);
      do {
	buff[--i] = (n & 7) + '0';
	n /= 8;
      } while (n > 0);
      rwrite (buff + i, sizeof buff - i);
      break;
    }

      /* Address in hexadecimal */
      case 'p': {
        char buff[16];
        int i = sizeof buff;
        VA n = (VA)va_arg (ap, void *);
        do {
          buff[--i] = hexdigits[n%16];
          n /= 16;
        } while (n > 0);
        buff[--i] = 'x';
        buff[--i] = '0';
        rwrite (buff + i, sizeof buff - i);
        break;
      }

      /* Null terminated string */
      case 's': {
        p = va_arg (ap, char *);
        rwrite (p, m_strlen(p));
        break;
      }

      /* Unsigned decimal integer */
      case 'u': {
        char buff[16];
        int i = sizeof buff;
        unsigned int n = va_arg (ap, unsigned int);
        do {
          buff[--i] = (n % 10) + '0';
          n /= 10;
        } while (n > 0);
        rwrite (buff + i, sizeof buff - i);
        break;
      }

      /* Unsigned hexadecimal number */
      case 'X':
      case 'x': {
        char buff[16];
        int i = sizeof buff;
        unsigned int n = va_arg (ap, unsigned int);
        do {
          buff[--i] = hexdigits[n%16];
          n /= 16;
        } while (n > 0);
        rwrite (buff + i, sizeof buff - i);
        break;
      }

      /* Anything else, print the character as is */
      default: {
        rwrite (q, 1);
        break;
      }
    }
  }

  va_end (ap);

  /* Print whatever comes after the last format spec */

  rwrite(p,m_strlen (p));
#if 0
  if (!mtcp_state_set (&printflocked, 0, 1)) mtcp_abort ();
  mtcp_state_futex (&printflocked, FUTEX_WAKE, 1, NULL);
#endif

  if ( lvl >= MTCP_FATAL ) mtcp_abort();

}

static void rwrite (char const *buff, int size)

{
  int offs, rc;

  if (dmtcp_info_stderr_fd != -1) {
    for (offs = 0; offs < size; offs += rc) {
#ifdef DMTCP
      /* DEBUG macro says to print debugging info, and DMTCP macro says
       * to do it even when DMTCP doesn't want to allow MTCP debugging.
       * Many mtcp_printf() calls occur inside DEBUG conditional.
       * NOTE:   ./configure --enable-debug doesn't automatically set DEBUG.
       * This is useful, so that when DMTCP runs, MTCP debugging is off by
       * default.  Setting DMTCP in mtcp/Makefile and re-compiling changes this.
       */ 
      rc = mtcp_sys_write (STDERR_FILENO, buff + offs, size - offs);
#else
      /* File descriptor where all the debugging outputs should go. */
      rc = mtcp_sys_write (dmtcp_info_stderr_fd, buff + offs, size - offs);
#endif
      if (rc <= 0) break;
    }
  }
  if (dmtcp_info_jassertlog_fd != -1) {
    for (offs = 0; offs < size; offs += rc) {
      rc = mtcp_sys_write (dmtcp_info_jassertlog_fd, buff + offs, size - offs);
      if (rc <= 0) break;
    }
  }
}
