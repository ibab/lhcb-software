// ---------------------------------------------------------------------------
//
// Usage: gentest.exe libGaudiOnlineTests.so check_libc_symbols
//
// ---------------------------------------------------------------------------

#include <dlfcn.h>
#include <signal.h>
#include <cstdio>
using namespace std;
void check_sym(const char* hn, void* h, const char* name)    {
  void *sym = ::dlsym(h,name);
  ::printf("%-24s %32s: 0x%016lX %s\n",hn,name,
	   (unsigned long)sym,
	   (sym?"\t  FOUND" : "\t  NOT_FOUND"));
}

#define check(h,n) check_sym(#h,h,#n)
//#define LIBC "/lib64/libc-2.12.so"
#define LIBC "/home/frankm/rpmbuild/BUILDROOT/glibc-2.12-1.7.el6.x86_64/lib64/libc-2.12.so"

#define LIBPTHREAD "/lib64/libpthread-2.12.so"

extern "C" int check_libc_symbols(int,char**)  {
  void* libc_handle    = ::dlopen(LIBC,RTLD_LAZY|RTLD_GLOBAL|RTLD_LOCAL);
  void* pthread_handle = ::dlopen(LIBPTHREAD,RTLD_LAZY|RTLD_GLOBAL|RTLD_LOCAL);

  sigaction(1,0,0);

  ::printf("LIBC:\t%s\n", "" /* LIBC */);
  check(0,     __libc_sigaction);
  check(0,     __sigaction);
  check(libc_handle,     __libc_sigaction);
  check(libc_handle,     __GI___libc_sigaction);
  check(libc_handle,     __the_real_sigaction);
  check(libc_handle,     __sigaction);
  ::printf("LIBPTHREAD:\t%s\n", "" /* LIBPTHREAD */);
  check(pthread_handle,  pthread_create);
  check(pthread_handle,  __pthread_create_2_1);
  check(pthread_handle,  pthread_create@@GLIBC_2.2.5);
  check(pthread_handle,  __sigaction);
  check(pthread_handle,  pthread_sigmask);
  check(pthread_handle,  sigcancel_handler);
  check(pthread_handle,  __GI___pthread_register_cancel);
  check(pthread_handle,  __pthread_register_cancel);
  return 0;
}
