#ifndef ONLINE_ONLINEKERNEL_RTL_TIME_H
#define ONLINE_ONLINEKERNEL_RTL_TIME_H
/*
 *   Emulation of the Unix calls gettimeofday and getfilesystemtime
 *   Taken from the PHP distribution.
 *   
 *   M.Frank
 */

#ifdef __cplusplus
#include <ctime>
extern "C" {
#else
#include <time.h>
#endif

  time_t str2time(const char* buff,const char* fmt);

#ifdef _WIN32

#if 0
  VC 6   -> _MSC_VER 1200 
  VC 7   -> _MSC_VER 1300 
  VC 7.1 -> _MSC_VER 1310 
  VC 8   -> _MSC_VER 1400 
  VC 9   -> _MSC_VER 1500 
#endif

#define tzset       _tzset
#define daylight    _daylight
#define timezone    _timezone
#define tzname      _tzname
#if _MSC_VER<1400
  struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
  };
  int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info);
#else
  int gettimeofday(struct timeval *time_Info, const void *timezone_Info);
#endif
  char*  strptime(const char *buf, const char *fmt, struct tm *tm);

  void usleep(unsigned int useconds);
  int getfilesystemtime(struct timeval *time_Info);

#else
#include <sys/time.h>
#endif  /* WIN32                          */

#ifdef __cplusplus
}
#endif  /* __cplusplus                    */
#endif  /* ONLINE_ONLINEKERNEL_RTL_TIME_H */
