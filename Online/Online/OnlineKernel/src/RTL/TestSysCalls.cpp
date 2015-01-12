#include "RTL/rtl.h"
#include <iostream>
#include <typeinfo>
#include <cstdio>
#include <climits>
#include <ctime>

#ifndef WIN32w
#include <unistd.h>
#include <syscall.h>
#include <sched.h>

using namespace std;

bool rtl_print(int argc, char** argv)  {
  if ( argc && argv ) return true;
  return true;
}


extern "C" int rtl_test_syscall_getcpu(int argc, char** argv)   {
  int ret;
  bool print = rtl_print(argc,argv);
  unsigned cpu=0, node=0, none=0;
  ret = sched_getcpu();
  if ( print ) 
    cout << " sched_getcpu() = " << ret << endl;
#ifdef __NR_getcpu
  ret = syscall(__NR_getcpu,&cpu,&node,&none);
  //ret = sys_get_cpu(&cpu,&node,&none);
#else
  //ret = sys_getcpu(3,&cpu,&node,&none);
  ret = cpu = node = none = -1;
  errno = ENOSYS;
#endif
  //ret = ::getcpu(&cpu,&node,0);
  if ( print ) 
    cout << " syscall(getcpu,CPU,NODE) = " << ret << " CPU:" << cpu << " NODE:" << node << endl;
  return ret < 0 ? 0 : 1;
}

extern "C" int rtl_test_mktime(int argc, char** argv)   {
  time_t t1 = ::time(0), t2 = ::time(0);
  char text1[128], text2[128];
  struct tm tm1, tm2;
  bool print = rtl_print(argc,argv);

  ::localtime_r(&t1, &tm1);
  ::localtime_r(&t1, &tm2);
  ::strftime(text1,sizeof(text1),"%Y-%m-%d %H:%M:%S",    &tm1);
  ::strftime(text2,sizeof(text2),"%Y-%m-%d %H:%M:%S %Z", &tm2);

  time_t tt1 = ::mktime(&tm1);
  time_t tt2 = ::mktime(&tm2);
  if ( print ) {
    cout << " time_t    " << t1 << " -- " << t2 << endl;
    cout << " struct tm " << text1 << " -- " << text2 << endl;
    cout << " mktime    " << tt1 << " -- " << tt2 << endl;
  }
  return 1;
}

extern "C" int rtl_test_syscalls(int argc, char** argv)  {

  int status = 1;
  int ret;

  ret = rtl_test_mktime(argc,argv);
  cout << " +++ rtl_test_mktime:" << ret << endl;
  if ( !lib_rtl_is_success(ret) ) status = ret;

  ret = rtl_test_syscall_getcpu(argc,argv);
  cout << " +++ rtl_test_syscall_getcpu:" << ret << endl;
  if ( !lib_rtl_is_success(ret) ) status = ret;

  return status;
}
#endif
