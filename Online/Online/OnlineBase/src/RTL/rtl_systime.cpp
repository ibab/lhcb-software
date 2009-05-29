#include "RTL/SysTime.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <ctime>
#endif

unsigned int RTL::SysTime::start() {  
  static unsigned int s_start = SysTime::now();
  return s_start;
}

unsigned int RTL::SysTime::now()   {
#ifdef _WIN32
  return ::GetTickCount();
#else
  timespec t;
  ::clock_gettime(CLOCK_REALTIME,&t);
  return t.tv_sec*1000 + t.tv_nsec/1000000;
#endif
}
