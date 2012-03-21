//#define CHECKPOINTING_MUTEXED_WRITE
#include "Checkpointing/SysCallsImp.h"

#if defined(CHECKPOINTING_MUTEXED_WRITE)
#include "Checkpointing/FutexState.h"
#include "linux/futex.h"
#endif
#include <sys/types.h>
#include <elf.h> // For value of AT_SYSINFO, Elf??_auxv_t
#include <unistd.h>

# define ELF_AUXV_T Elf64_auxv_t
# define UINT_T     uint64_t

#define RESET_THREAD_SYSINFO 1
#undef  RESET_THREAD_SYSINFO
// Some reports say it was 0x18 in past.  Should we also check that?
#define DEFAULT_SYSINFO_OFFSET "0x10"

STATIC(void*) get_at_sysinfo() {
  void **stack;
  int i;
  ELF_AUXV_T *auxv;
  static char **my_environ = 0;

  if (my_environ == 0)
    my_environ = environ;
#if 0
  // Walk the stack.
  __asm__ volatile (CLEAN_FOR_64_BIT(mov %%ebp, %0\n\t)
                : "=g" (stack) );
  mtcp_output(MTCP_INFO,"stack 2: %p\n", stack);

  // When popping stack/%ebp yields zero, that's the ELF loader telling us that
  // this is "_start", the first call frame, which was created by ELF.
  for ( ; *stack != 0; stack = *stack );

  // Go beyond first call frame:
  // Next look for &(argv[argc]) on stack;  (argv[argc] == 0)
  for (i = 1; stack[i] != 0; i++);

  // Do some error checks
  if ( &(stack[i]) - stack > 100000 ) {
    mtcp_output(MTCP_INFO,"Error: overshot stack\n");
    syscall(SYS_exit,1);
  }
  stack = &stack[i];
#else
  stack = (void **)&my_environ[-1];
  if (*stack != 0) {
    mtcp_output(MTCP_INFO,"This should be argv[argc] == 0 and it's not.\n"
	"NO &argv[argc], stack: %p\n", stack);
    syscall(SYS_exit,1);
  }
#endif
  // stack[-1] should be argv[argc-1]
  if ( (void **)stack[-1] < stack || (void **)stack[-1] > stack + 100000 ) {
    mtcp_output(MTCP_INFO,"candidate argv[argc-1] failed consistency check\n");
    syscall(SYS_exit,1);
  }
  for (i = 1; stack[i] != 0; i++)
    if ( (void **)stack[i] < stack || (void **)stack[i] > stack + 10000 ) {
      mtcp_output(MTCP_INFO,"candidate argv[%d] failed consistency check\n", i);
      syscall(SYS_exit,1);
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
  //result = 0;
  if (result == -1) {
    volatile void* sysinfo = 0;
    __asm__ volatile (CLEAN_FOR_64_BIT(mov %%gs:) DEFAULT_SYSINFO_OFFSET ", %0\n\t" 
		      : "=r" (sysinfo));
    result = (sysinfo == get_at_sysinfo());
  }
  return result;
}

// AT_SYSINFO is what kernel calls sysenter address in vdso segment.
// Kernel saves it for each thread in %gs:SYSINFO_OFFSET ??
//  as part of kernel TCB (thread control block) at beginning of TLS ??
void* mtcp_get_thread_sysinfo() {
  void *sysinfo;
  __asm__ volatile (CLEAN_FOR_64_BIT(mov %%gs:) DEFAULT_SYSINFO_OFFSET ", %0\n\t" : "=r" (sysinfo) );
  return sysinfo;
}

void mtcp_set_thread_sysinfo(void *sysinfo) {
  __asm__ volatile (CLEAN_FOR_64_BIT(mov %0, %%gs:) DEFAULT_SYSINFO_OFFSET "\n\t" : : "r" (sysinfo) );
}

