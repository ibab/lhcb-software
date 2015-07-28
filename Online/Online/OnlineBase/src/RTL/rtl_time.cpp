#include "RTL/rtl.h"
#include "RTL/time.h"
#include <cstring>

time_t str2time(const char* buff,const char* fmt) {
  struct tm tim;
  ::memset(&tim,0,sizeof(tim));
  tim.tm_isdst = daylight;
  return ::strptime(buff, fmt, &tim) != 0 ? ::mktime(&tim) : -1;
}

int lib_rtl_sleep(int millisecs)    {
  ::usleep(1000*millisecs);
  return 1;
}

int lib_rtl_usleep(int microsecs)    {
  ::usleep(microsecs);
  return 1;
}

#ifndef _WIN32
#include <cstdlib>
#include <unistd.h>
#else

#include <cctype>
#include <clocale>
#include <cstring>
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

#if _MSC_VER<1400
int gettimeofday(struct timeval *time_Info, struct timezone *tz_info)  {  
  __int64 timer;
  LARGE_INTEGER li;
  BOOL b;
  double dt;
  static struct timeval starttime = {0,0};
  static __int64      lasttime, freq;

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
  if (tz_info != NULL) {
    _tzset();
    tz_info->tz_minuteswest = timezone;
    tz_info->tz_dsttime = daylight;
  }
  return 0;
}
#else
int gettimeofday(timeval *time_Info, const void*)  {  
  return getfilesystemtime(time_Info);
}
#endif

// this usleep isnt exactly accurate but should do ok
void usleep(unsigned int useconds)    {
  struct timeval tnow, tthen;
  if (useconds >= 1000) {
    ::Sleep(useconds/1000);
    useconds = useconds%1000000;
    if ( 10 > useconds ) return;  // Inaccuracy ... forget about it!
  }
  getfilesystemtime(&tthen); 
  //        gettimeofday(&tthen, NULL);
  tthen.tv_usec += useconds;
  while (1) {
    getfilesystemtime(&tnow); 
    //    gettimeofday(&tnow, NULL);
    if (tnow.tv_sec >= tthen.tv_sec) {
      return;
    }
    if (tnow.tv_sec == tthen.tv_sec) {
      if (tnow.tv_usec > tthen.tv_usec) {
        return;
      }
    }
  }
}

#define asizeof(a)        ((int)(sizeof (a) / sizeof ((a)[0])))

struct dtconv {
  char        *abbrev_month_names[12];
  char        *month_names[12];
  char        *abbrev_weekday_names[7];
  char        *weekday_names[7];
  char        *time_format;
  char        *sdate_format;
  char        *dtime_format;
  char        *am_string;
  char        *pm_string;
  char        *ldate_format;
};

static struct dtconv        En_US = {
  { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" },
  { "January", "February", "March", "April",
    "May", "June", "July", "August",
    "September", "October", "November", "December" },
  { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" },
  { "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday" },
  "%H:%M:%S",
  "%m/%d/%y",
  "%a %b %e %T %Z %Y",
  "AM",
  "PM",
  "%A, %B, %e, %Y"
};

char* strptime(const char *buf, const char *fmt, struct tm *tm)  {
  char        c;
  const char        *ptr;
  int        i, j, ndigit, len = 0;

  ptr = fmt;
  while (*ptr != 0)    {
    if (*buf == 0)      break;

    c = *ptr++;
    if (c != '%')        {
      if (isspace(c))
        while (*buf != 0 && isspace(*buf))
          buf++;
      else if (c != *buf++)
        return 0;
      continue;
    }

    c = *ptr++;
    switch (c)   {
    case 0:
    case '%':
      if (*buf++ != '%')return 0;
    break;

    case 'C':
      buf = ::strptime(buf, En_US.ldate_format, tm);
      if (buf == 0)        return 0;
      break;

    case 'c':
      buf = ::strptime(buf, "%x %X", tm);
      if (buf == 0)        return 0;
      break;

    case 'D':
      buf = ::strptime(buf, "%m/%d/%y", tm);
      if (buf == 0)        return 0;
      break;

    case 'R':
      buf = ::strptime(buf, "%H:%M", tm);
      if (buf == 0)        return 0;
      break;

    case 'r':
      buf = ::strptime(buf, "%I:%M:%S %p", tm);
      if (buf == 0)        return 0;
      break;

    case 'T':
      buf = ::strptime(buf, "%H:%M:%S", tm);
      if (buf == 0)        return 0;
      break;

    case 'X':
      buf = ::strptime(buf, En_US.time_format, tm);
      if (buf == 0)        return 0;
      break;

    case 'x':
      buf = ::strptime(buf, En_US.sdate_format, tm);
      if (buf == 0)        return 0;
      break;

    case 'j':
      if (!isdigit(*buf)) return 0;
      for (i=0, j=0; j<3 && *buf != 0 && isdigit(*buf); ++j, ++buf)        {
        i *= 10;
        i += *buf - '0';
      }
      if (i > 365)
        return 0;
      tm->tm_yday = i;
      break;

    case 'M':
    case 'S':
      if (*buf == 0 || isspace(*buf))
        break;
    if (!isdigit(*buf)) return 0;
    for (i=0, j=0; j<2 && *buf != 0 && isdigit(*buf); ++j, ++buf)        {
      i *= 10;
      i += *buf - '0';
    }
    if (i > 59)
      return 0;
    if (c == 'M')
      tm->tm_min = i;
    else
      tm->tm_sec = i;
    if (*buf != 0 && isspace(*buf))
      while (*ptr != 0 && !isspace(*ptr))
        ptr++;
    break;

    case 'H':
    case 'I':
    case 'k':
    case 'l':
      if (!isdigit(*buf))
        return 0;
    for (i=0, j=0; j<2 && *buf != 0 && isdigit(*buf); ++j, ++buf)     {
      i *= 10;
      i += *buf - '0';
    }
    if (c == 'H' || c == 'k') {
      if (i > 23) return 0;
    } else if (i > 11)
      return 0;
    tm->tm_hour = i;
    if (*buf != 0 && isspace(*buf))
      while (*ptr != 0 && !isspace(*ptr))
        ++ptr;
    break;

    case 'p':
      len = strlen(En_US.am_string);
      if (_strnicmp(buf, En_US.am_string, len) == 0)        {
        if (tm->tm_hour > 12)
          return 0;
        if (tm->tm_hour == 12)
          tm->tm_hour = 0;
        buf += len;
        break;
      }
      len = strlen(En_US.pm_string);
      if (_strnicmp(buf, En_US.pm_string, len) == 0)        {
        if (tm->tm_hour > 12)
          return 0;
        if (tm->tm_hour != 12)
          tm->tm_hour += 12;
        buf += len;
        break;
      }
      return 0;

    case 'A':
    case 'a':
      for (i = 0; i < asizeof(En_US.weekday_names); i++)  {
        len = strlen(En_US.weekday_names[i]);
        if (_strnicmp(buf,En_US.weekday_names[i],len) == 0)
          break;
        len = strlen(En_US.abbrev_weekday_names[i]);
        if (_strnicmp(buf,En_US.abbrev_weekday_names[i],len) == 0)
          break;
      }
    if (i == asizeof(En_US.weekday_names))        return 0;
    tm->tm_wday = i + 1;
    buf += len;
    break;

    case 'd':
    case 'e':
      if (!isdigit(*buf)) return 0;
    for (i=0, j=0; j<2 && *buf != 0 && isdigit(*buf); ++j, ++buf)        {
      i *= 10;
      i += *buf - '0';
    }
    if (i > 31) return 0;
    tm->tm_mday = i;
    if (*buf != 0 && isspace(*buf))
      while (*ptr != 0 && !isspace(*ptr)) ++ptr;
    break;

    case 'B':
    case 'b':
    case 'h':
      for (i = 0; i < asizeof(En_US.month_names); i++)                {
        len = strlen(En_US.month_names[i]);
        if (_strnicmp(buf,En_US.month_names[i],len) == 0)
          break;

        len = strlen(En_US.abbrev_month_names[i]);
        if (_strnicmp(buf,En_US.abbrev_month_names[i],len) == 0)
          break;
      }
    if (i == asizeof(En_US.month_names)) return 0;
    tm->tm_mon = i + 1;
    buf += len;
    break;

    case 'm':
      if (!isdigit(*buf))  return 0;
      for (i=0, j=0; j<2 && *buf != 0 && isdigit(*buf); ++j, ++buf)   {
        i *= 10;
        i += *buf - '0';
      }
      if (i < 1 || i > 12)
        return 0;
      tm->tm_mon = i;
      if (*buf != 0 && isspace(*buf))
        while (*ptr != 0 && !isspace(*ptr))
          ptr++;
      break;

    case 'Y':
    case 'y':
      if (*buf == 0 || isspace(*buf))
        break;
    if (!isdigit(*buf)) return 0;
    ndigit = c=='Y' ? 4 : 2;
    for (i=0, j=0; j<ndigit && *buf != 0 && isdigit(*buf); ++j, ++buf)        {
      i *= 10;
      i += *buf - '0';
    }
    if (c == 'y' && i < 69)        /* Unix Epoch pivot year */
      i += 100;
    if (c == 'Y') i -= 1900;
    if (i < 0   ) return 0;
    tm->tm_year = i;
    if (*buf != 0 && isspace(*buf))
      while (*ptr != 0 && !isspace(*ptr))
        ptr++;
    break;
    }
  }
  return (char*) buf;
}

#endif

