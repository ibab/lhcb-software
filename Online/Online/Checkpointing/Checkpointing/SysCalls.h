#ifndef SYSCALLS_H
#define SYSCALLS_H
#include "Checkpointing/Namespace.h"
#include "Checkpointing/Namespace.h"
#include <gnu/libc-version.h>
#include <sys/syscall.h>
#include "sysdep-x86_64.h"

#if defined(__x86_64__) || defined(__INTEL_COMPILER)
# define eax rax
# define ebx rbx
# define ecx rcx
# define edx rax
# define ebp rbp
# define esi rsi
# define edi rdi
# define esp rsp
# define CLEAN_FOR_64_BIT_HELPER(args...) #args
# define CLEAN_FOR_64_BIT(args...) CLEAN_FOR_64_BIT_HELPER(args)
#else
# define CLEAN_FOR_64_BIT(args...) #args
#endif

#define mtcp_inline_syscall(name, num_args, args...) INLINE_SYSCALL(name, num_args, args)

#define mtcp_sys_rt_sigaction(args...)   mtcp_inline_syscall(rt_sigaction,4,args)
#define mtcp_sys_sigaction(sig,nact,oact) mtcp_sys_rt_sigaction(sig,nact,oact,NSIG/8)

#define mtcp_sys_fstat(args...)   mtcp_inline_syscall(fstat,2,args)
#define mtcp_sys_lstat(args...)   mtcp_inline_syscall(lstat,2,args)
#define mtcp_sys_read(args...)    mtcp_inline_syscall(read,3,args)
#define mtcp_sys_write(args...)   mtcp_inline_syscall(write,3,args)
#define mtcp_sys_lseek(args...)   mtcp_inline_syscall(lseek,3,args)
#define mtcp_sys_open(args...)    mtcp_inline_syscall(open,3,args)
#define mtcp_sys_open2(args...)   mtcp_sys_open(args,0777)
#define mtcp_sys_close(args...)   mtcp_inline_syscall(close,1,args)
#define mtcp_sys_access(args...)  mtcp_inline_syscall(access,2,args)
#define mtcp_sys_unlink(args...)  mtcp_inline_syscall(unlink,1,args)
#define mtcp_sys_fchmod(args...)  mtcp_inline_syscall(fchmod,2,args)
#define mtcp_sys_exit(args...)    mtcp_inline_syscall(exit,1,args)
#define mtcp_sys_pipe(args...)    mtcp_inline_syscall(pipe,1,args)
#define mtcp_sys_dup(args...)     mtcp_inline_syscall(dup,1,args)
#define mtcp_sys_dup2(args...)    mtcp_inline_syscall(dup2,2,args)
#define mtcp_sys_getpid(args...)  mtcp_inline_syscall(getpid,0)
#define mtcp_sys_getppid(args...) mtcp_inline_syscall(getppid,0)
#define mtcp_sys_fork(args...)             mtcp_inline_syscall(fork,0)
#define mtcp_sys_vfork(args...)            mtcp_inline_syscall(vfork,0)
#define mtcp_sys_execve(args...)           mtcp_inline_syscall(execve,3,args)
#define mtcp_sys_wait4(args...)            mtcp_inline_syscall(wait4,4,args)
#define mtcp_sys_gettimeofday(args...)     mtcp_inline_syscall(gettimeofday,2,args)
#define mtcp_sys_mmap(args...)             (void*)mtcp_inline_syscall(mmap,6,args)
#define mtcp_sys_munmap(args...)           mtcp_inline_syscall(munmap,2,args)
#define mtcp_sys_msync(args...)            (void*)mtcp_inline_syscall(msync,3,args)
#define mtcp_sys_mprotect(args...)         mtcp_inline_syscall(mprotect,3,args)
#define mtcp_sys_set_tid_address(args...)  mtcp_inline_syscall(set_tid_address,1,args)
#define mtcp_sys_brk(args...)              (void*)(mtcp_inline_syscall(brk,1,args))
#ifdef __NR_getdents
#define mtcp_sys_getdents(args...)         mtcp_inline_syscall(getdents,3,args)
#endif
#ifdef __NR_getdents64
#define mtcp_sys_getdents64(args...)       mtcp_inline_syscall(getdents64,3,args)
#endif

#define mtcp_sys_fcntl2(args...)           mtcp_inline_syscall(fcntl,2,args)
#define mtcp_sys_fcntl3(args...)           mtcp_inline_syscall(fcntl,3,args)
#define mtcp_sys_mkdir(args...)            mtcp_inline_syscall(mkdir,2,args)

#define mtcp_sys_kernel_gettid(args...)    mtcp_inline_syscall(gettid,0)
#define mtcp_sys_kernel_tkill(args...)     mtcp_inline_syscall(tkill,2,args)
#define mtcp_sys_personality(args...)      mtcp_inline_syscall(personality,1,args)

int mtcp_have_thread_sysinfo_offset();
void *mtcp_get_thread_sysinfo(void);
void mtcp_set_thread_sysinfo(void *sysinfo);

#ifndef CHECKPOINTING_OUTPUT
#define CHECKPOINTING_OUTPUT
STATIC(void) mtcp_output(int lvl, const char* fmt,...);
STATIC(void) mtcp_set_debug_level(int lvl);
STATIC(int)  mtcp_get_debug_level();
#ifndef MTCP_ERRNO
#define MTCP_ERRNO
STATIC(int&) __mtcp_sys_errno();
#define mtcp_sys_errno (__mtcp_sys_errno())
#endif
#endif // CHECKPOINTING_OUTPUT

#define __set_errno(Val) mtcp_sys_errno = (Val) /* required for sysdep-XXX.h */

// gcc-3.4 issues a warning that noreturn function returns, if declared noreturn
// static void inline mtcp_abort (void) __attribute__ ((noreturn));
//static void inline mtcp_abort (void);
STATIC(void) mtcp_abort (void);

/* Offset computed (&x.pid - &x) for
 *   struct pthread x;
 * as found in:  glibc-2.5/nptl/descr.h
 * It was 0x4c and 0x48 for pid and tid for i386.
 * Roughly, the definition is:
 *glibc-2.5/nptl/descr.h:
 * struct pthread
 * {
 *  union {
 *   tcbheader_t tcbheader;
 *   void *__padding[16];
 *  };
 *  list_t list;
 *  pid_t tid;
 *  pid_t pid;
 *  ...
 * } __attribute ((aligned (TCB_ALIGNMENT)));
 *
 *glibc-2.5/nptl/sysdeps/pthread/list.h:
 * typedef struct list_head
 * {
 *  struct list_head *next;
 *  struct list_head *prev;
 * } list_t;
 *
 * NOTE: glibc-2.10 changes the size of __padding from 16 to 24.  --KAPIL
 *
 * NOTE: glibc-2.10 further changes the size tcphead_t without updating the
 *       size of __padding in struct pthread. We need to add an extra 512 bytes
 *       to accomodate this.                                     -- KAPIL
 */

#if __GLIBC_PREREQ (2,11)
# if defined(__x86_64__) || defined(__INTEL_COMPILER)
#  define TLS_PID_OFFSET \
           (512+26*sizeof(void *)+sizeof(pid_t))  // offset of pid in pthread struct
#  define TLS_TID_OFFSET (512+26*sizeof(void *))  // offset of tid in pthread struct
# else
#  define TLS_PID_OFFSET \
           (26*sizeof(void *)+sizeof(pid_t))  // offset of pid in pthread struct
#  define TLS_TID_OFFSET (26*sizeof(void *))  // offset of tid in pthread struct
# endif
#elif __GLIBC_PREREQ (2,10)
# define TLS_PID_OFFSET \
	  (26*sizeof(void *)+sizeof(pid_t))  // offset of pid in pthread struct
# define TLS_TID_OFFSET (26*sizeof(void *))  // offset of tid in pthread struct
#else
# define TLS_PID_OFFSET \
	  (18*sizeof(void *)+sizeof(pid_t))  // offset of pid in pthread struct
# define TLS_TID_OFFSET (18*sizeof(void *))  // offset of tid in pthread struct
#endif

typedef int (*libc_clone_entry_t) (int (*fn) (void *arg),
				   void *child_stack,
				   int flags,
				   void *arg,
				   int *parent_tidptr,
				   struct user_desc *newtls,
				   int *child_tidptr);
#endif // SYSCALLS_H
