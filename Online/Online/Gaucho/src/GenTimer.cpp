#include "Gaucho/GenTimer.h"
//#include "Gaucho/MonSubSys.h"
//#include "Gaucho/ObjService.h"
//#include "RTL/rtl.h"
#include <cstdio>
#include <cerrno>
#ifdef WIN32
#include "windows.h"
#define onesec (unsigned long long)(10000000)
#else
#include <sys/time.h>
#define onesec_mu (unsigned long long)(1000000)
#define onesec_nano (unsigned long long)(1000000000)
#define onesec_mili (unsigned long long)(1000000)
#endif
#include <time.h>
//#define ONESEC 10*1000*1000
extern "C"
{
 void* ThreadRoutine_C(void*a)
 {
   GenTimer *t = (GenTimer*)a;
   t->ThreadRoutine();
   pthread_exit(0); }
}
GenTimer::GenTimer(void *arg, int period, int typ)
{
  m_arg = arg;
  m_period = period;
  m_type = typ;
  m_dueTime = 0;
  m_periodic = (typ & TIMER_TYPE_PERIODIC) != 0;
  m_synched = (typ & TIMER_MODIFYER_SYNCHRONIZED) != 0;
  m_thread = 0;
  m_extlastdelta = 0;
  m_ForceExit = false;

//  printf("==================GenTimer Constructor.................%llu %d %d\n",m_period, m_periodic, m_synched);
}

GenTimer::~GenTimer( )
{
  Stop();
}
void GenTimer::timerHandler ( void )
{
}
void GenTimer::Start()
{
  int status;
//  printf("Starting Timer Thread...\n");
  m_ForceExit = false;
  status = pthread_create(&m_thread,NULL,ThreadRoutine_C, (void *)this);
  if (status == 0)
  {
//    printf("Timer Thread Started successfully\n");
  }
  else
  {
//    printf("Timer Thread NOT Started successfully %d\n", status);
  }
}
void GenTimer::Stop()
{
//  m_Hsys->Lock();
  if (m_thread != 0)
  {
    void *status;
    m_ForceExit = true;
    pthread_cancel(m_thread);
    pthread_join(m_thread,&status);
    m_thread = 0;
  }
//  m_Hsys->unLock();
}

void GenTimer::makeDeltaT()
{
//
// do all calculations in nanoseconds
// and also return nanoseconds
// Handle the precision outside...
//
#ifdef WIN32
  SYSTEMTIME tim;
  unsigned long long timstamp,ntim;
  GetLocalTime(&tim);
  SystemTimeToFileTime(&tim, (LPFILETIME)&timstamp);
  timestamp *= 10;
  ntim  = (timstamp/(m_period*onesec_nano))*(m_period*onesec_nano)+(m_period*onesec_nano);
  m_dueTime = ntim;
  unsigned long long dtim = (ntim-timstamp);
  return dtim;
#else
  unsigned long long timstamp;
  struct timeval tv;
  struct timezone *tz;
  tz = 0;
  gettimeofday(&tv, tz);
  timstamp  = tv.tv_sec;
  timstamp *= onesec_nano;
  timstamp += tv.tv_usec*1000;
  m_lastdelta = timstamp-m_lastfire;
  if (m_extlastdelta != 0)
  {
    *m_extlastdelta = m_lastdelta;
  }
  return;
#endif
}

unsigned long long GenTimer::getDeltaTime(int incr)
{
//
// do all calculations in nanoseconds
// and also return nanoseconds
// Handle the precision outside...
//
#ifdef WIN32
  SYSTEMTIME tim;
  unsigned long long timstamp,ntim;
  GetLocalTime(&tim);
  SystemTimeToFileTime(&tim, (LPFILETIME)&timstamp);
  timestamp *= 10;
  ntim  = (timstamp/(m_period*onesec_nano))*(m_period*onesec_nano)+(m_period*onesec_nano);
  m_dueTime = ntim;
  unsigned long long dtim = (ntim-timstamp);
  return dtim;
#else
  unsigned long long timstamp,newtim;
	struct timeval tv;
	struct timezone *tz;
	tz = 0;
	gettimeofday(&tv, tz);
  timstamp  = tv.tv_sec;
  timstamp *= onesec_nano;
  timstamp += tv.tv_usec*1000;
  m_lastfire = timstamp;
  if (m_synched)
  {
    newtim  = (timstamp/(m_period*onesec_mili));
    newtim *=(onesec_mili*m_period);
    newtim += (((unsigned long long)incr+m_period)*onesec_mili);
  }
  else
  {
    newtim = timstamp+(((unsigned long long)incr+m_period)*onesec_mili);
  }
//  printf ("+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_ %d\n",m_period);
  m_dueTime = newtim;
  unsigned long long dtim = (newtim-timstamp);
  if (dtim < 20000000)
  {
    dtim += m_period*onesec_mili;
    m_dueTime += m_period*onesec_mili;
  }
//  time_t nt = (time_t)(newtim/onesec_nano);
//  char tstamp[26];
//  char duet[26];
//  ctime_r(&tv.tv_sec,tstamp);
//  ctime_r(&nt,duet);
//  printf("============================%s %s  %llu\n",tstamp,duet,dtim);
  return dtim;
#endif

}
void *GenTimer::ThreadRoutine()
{
  unsigned long long delta;
  struct timespec req;
  int status;
//    MonTimer *m_timer = (MonTimer*)arg;
  while (1)
  {
    delta = getDeltaTime(0);
    unsigned long long s=delta/onesec_nano;
    req.tv_sec = (time_t)s;
    req.tv_nsec = delta % onesec_nano;
    status = -1;
    while (status != 0)
    {
      status = nanosleep(&req,&req);
      if (status == 0)
      {
        makeDeltaT();
        timerHandler();
        break;
      }
      else if (errno == EINTR)
      {
        //if (m_ForceExit) return 0;
        printf("EINTR in nanosleep \n");
        continue;
      }
      else
      {
//        printf("Fatal Return Value from nanosleep %d\n",status);
        break;
      }
    }
    if (!m_periodic)
    {
      break;
    }
  }
  return 0;
}
