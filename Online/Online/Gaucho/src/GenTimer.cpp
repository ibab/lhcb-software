#include "Gaucho/GenTimer.h"
#include <cstdio>
#include <cerrno>
#include <ctime>
#ifdef WIN32
#include "windows.h"
#define onesec      (unsigned long long)(10000000)
#define onemilli_nano (unsigned long long 1000000)
#else
#include <sys/time.h>
#define onesec_mu   (unsigned long long)(1000000)
#define onesec_mili (unsigned long long)(1000000)
#endif
#define onesec_nano (unsigned long long)(1000000000)
#include "dic.hxx"
namespace
{
  int ThreadRoutine_C(void* arg)
  {
    int dum;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&dum);
    GenTimer *t = (GenTimer*)arg;
    t->ThreadRoutine();
    return 1;
  }
}

GenTimer::GenTimer(void *arg, int period, int typ)
{
  m_arg          = arg;
  m_period       = period;
  m_type         = typ;
  m_dueTime      = 0;
  m_periodic     = (typ & TIMER_TYPE_PERIODIC) != 0;
  m_synched      = (typ & TIMER_MODIFYER_SYNCHRONIZED) != 0;
  m_thread       = 0;
  m_extlastdelta = 0;
  m_ForceExit    = false;
  m_dontdimlock = false;
}

GenTimer::~GenTimer( )
{
  Stop();
}

//void GenTimer::timerHandler()
//{
//}

void GenTimer::Start()
{
  m_ForceExit = false;
  if ( m_thread == 0 )
  {
    int status = ::lib_rtl_start_thread(ThreadRoutine_C,this,&m_thread);
    if ( !lib_rtl_is_success(status) )
    {
      ::lib_rtl_output(LIB_RTL_ERROR,"Timer Thread NOT Started successfully %d\n", status);
    }
  }
  else
  {
    if (!m_periodic)
    {
      Stop();
      int status = ::lib_rtl_start_thread(ThreadRoutine_C,this,&m_thread);
      if ( !lib_rtl_is_success(status) )
      {
        ::lib_rtl_output(LIB_RTL_ERROR,"Timer Thread NOT Started successfully %d\n", status);
      }
    }
  }
}

void GenTimer::Stop()
{
//  m_lock.lockMutex();
  dim_lock();
  if (m_thread != 0)
  {
    m_ForceExit = true;
    ::lib_rtl_cancel_thread(m_thread);
    ::lib_rtl_join_thread(m_thread);
    m_thread = 0;
  }
  dim_unlock();
//  m_lock.unlockMutex();
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
  unsigned long long timestamp,ntim;
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
  timestamp *= 100;
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
  m_dueTime = newtim;
  unsigned long long dtim = (newtim-timstamp);
  if (dtim < 20000000)
  {
    dtim += m_period*onesec_mili;
    m_dueTime += m_period*onesec_mili;
  }
  return dtim;
#endif

}
void *GenTimer::ThreadRoutine()
{
  unsigned long long delta;
  struct timespec req;
  int status;
  while (1)
  {
    delta = getDeltaTime(0);
#ifdef WIN32
    Sleep(delta/onemilli_nano);
#else
    unsigned long long s=delta/onesec_nano;
    req.tv_sec = (time_t)s;
    req.tv_nsec = delta % onesec_nano;
    status = -1;
    while (status != 0)
    {
      status = nanosleep(&req,&req);
      if (status == 0)
      {
//        m_lock.lockMutex();
        if (!m_dontdimlock) dim_lock();
        makeDeltaT();
        timerHandler();
        if (!m_dontdimlock) dim_unlock();
//        m_lock.unlockMutex();
        break;
      }
      else if (errno == EINTR)
      {
        ::lib_rtl_output(LIB_RTL_WARNING,"EINTR in nanosleep \n");
        continue;
      }
      else
      {
        break;
      }
    }
#endif
    if (!m_periodic)
    {
      break;
    }
  }
  return 0;
}
