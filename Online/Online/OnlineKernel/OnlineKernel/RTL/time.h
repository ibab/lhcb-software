#ifndef ONLINE_ONLINEKERNEL_RTL_TIME_H
#define ONLINE_ONLINEKERNEL_RTL_TIME_H
/*
 *   Emulation of the Unix calls gettimeofday and getfilesystemtime
 *   Taken from the PHP distribution.
 *   
 *   M.Frank
 */
#include <time.h>
#ifdef _WIN32

struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};

#ifdef __cplusplus
extern "C" {
#endif

  void usleep(unsigned int useconds);
  int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info);
  int getfilesystemtime(struct timeval *time_Info) ;

#ifdef __cplusplus
}
#endif  /* __cplusplus                    */
#endif  /* WIN32                          */
#endif  /* ONLINE_ONLINEKERNEL_RTL_TIME_H */
