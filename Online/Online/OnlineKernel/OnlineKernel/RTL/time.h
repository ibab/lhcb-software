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

  struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
  };


  char*  strptime(const char *buf, const char *fmt, struct tm *tm);
  #define tzset       _tzset
  #define daylight    _daylight
  #define timezone    _timezone
  #define tzname      _tzname

  void usleep(unsigned int useconds);
  int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info);
  int getfilesystemtime(struct timeval *time_Info) ;

#endif  /* WIN32                          */

#ifdef __cplusplus
}
#endif  /* __cplusplus                    */
#endif  /* ONLINE_ONLINEKERNEL_RTL_TIME_H */
