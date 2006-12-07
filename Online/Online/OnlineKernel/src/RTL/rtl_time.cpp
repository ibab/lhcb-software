#include "RTL/rtl.h"
#include "RTL/time.h"
#ifndef _WIN32
#include <cstdlib>
#include <unistd.h>
#else

#include <windows.h>

int getfilesystemtime(struct timeval *time_Info)  {
  FILETIME ft;
  __int64 ff;

  GetSystemTimeAsFileTime(&ft);   /* 100 ns blocks since 01-Jan-1641 */
  /* resolution seems to be 0.01 sec */ 
  ff = *(__int64*)(&ft);
  time_Info->tv_sec = (int)(ff/(__int64)10000000-(__int64)11644473600);
  time_Info->tv_usec = (int)(ff % 10000000)/10;
  return 0;
}

int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info)  {	
  __int64 timer;
  LARGE_INTEGER li;
  BOOL b;
  double dt;
  static struct timeval starttime = {0,0};
  static __int64			lasttime, freq;

  /* Get the time, if they want it */
  if (time_Info != NULL) {
    if (starttime.tv_sec == 0) {
      b = QueryPerformanceFrequency(&li);
      if (!b) {
        starttime.tv_sec = -1;
      }
      else {
        freq = li.QuadPart;
        b = QueryPerformanceCounter(&li);
        if (!b) {
          starttime.tv_sec = -1;
        }
        else {
          getfilesystemtime(&starttime);
          timer = li.QuadPart;
          dt = (double)timer/freq;
          starttime.tv_usec -= (int)((dt-(int)dt)*1000000);
          if (starttime.tv_usec < 0) {
            starttime.tv_usec += 1000000;
            --starttime.tv_sec;
          }
          starttime.tv_sec -= (int)dt;
        }
      }
    }
    if (starttime.tv_sec > 0) {
      b = QueryPerformanceCounter(&li);
      if (!b) {
        starttime.tv_sec = -1;
      }
      else {
        timer = li.QuadPart;
        if (timer < lasttime) {
          getfilesystemtime(time_Info);
          dt = (double)timer/freq;
          starttime = *time_Info;
          starttime.tv_usec -= (int)((dt-(int)dt)*1000000);
          if (starttime.tv_usec < 0) {
            starttime.tv_usec += 1000000;
            --starttime.tv_sec;
          }
          starttime.tv_sec -= (int)dt;
        }
        else {
          lasttime = timer;
          dt = (double)timer/freq;
          time_Info->tv_sec = starttime.tv_sec + (int)dt;
          time_Info->tv_usec = starttime.tv_usec + (int)((dt-(int)dt)*1000000);
          if (time_Info->tv_usec > 1000000) {
            time_Info->tv_usec -= 1000000;
            ++time_Info->tv_sec;
          }
        }
      }
    }
    if (starttime.tv_sec < 0) {
      getfilesystemtime(time_Info);
    }

  }
  // Get the timezone, if they want it
  if (timezone_Info != NULL) {
    _tzset();
    timezone_Info->tz_minuteswest = timezone;
    timezone_Info->tz_dsttime = daylight;
  }
  return 0;
}

// this usleep isnt exactly accurate but should do ok
void usleep(unsigned int useconds)    {
  struct timeval tnow, tthen, t0;

  gettimeofday(&tthen, NULL);
  t0 = tthen;
  tthen.tv_usec += useconds;
  while (tthen.tv_usec > 1000000) {
    tthen.tv_usec -= 1000000;
    tthen.tv_sec++;
  }
  if (useconds > 10000) {
    useconds -= 10000;
    Sleep(useconds/1000);
  }
  while (1) {
    gettimeofday(&tnow, NULL);
    if (tnow.tv_sec > tthen.tv_sec) {
      break;
    }
    if (tnow.tv_sec == tthen.tv_sec) {
      if (tnow.tv_usec > tthen.tv_usec) {
        break;
      }
    }
  }
}
#endif

int lib_rtl_sleep(int millisecs)    {
  ::usleep(1000*millisecs);
  return 1;
}

int lib_rtl_usleep(int microsecs)    {
  ::usleep(microsecs);
  return 1;
}
