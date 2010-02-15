#include "../CCPCHist/HistTimer.h"
#include "CCPCSubSys.h"
#include "ObjService.h"
#include "RTL/rtl.h"
#ifdef WIN32
#include "windows.h"
#define onesec 10*1000*1000
#else
#include <sys/time.h>
#define onesec 1000*1000
#endif
#define ONESEC 10*1000*1000
HistTimer::HistTimer(CCPCSubSys *HSys, int period) : DimTimer()
{
  m_Hsys = HSys;
  m_period = period;
  m_dueTime = 0;
}

HistTimer::~HistTimer( )
{
}
void HistTimer::timerHandler ( void )
{
  //printf("Timer Fired...\n");
  m_Hsys->m_genSrv->setHeader(m_dueTime);
  dim_unlock();
  m_Hsys->Lock();
  dim_lock();
  m_Hsys->m_genSrv->Updater();
  m_Hsys->unLock();
  //m_dueTime += m_period*ONESEC;
  DimTimer::start(getDeltaTime());
}
void HistTimer::Start()
{
  int dtim = getDeltaTime();
  DimTimer::start(dtim+m_period);
}
int HistTimer::getDeltaTime()
{
#ifdef WIN32
  SYSTEMTIME tim;
  long long timstamp,ntim;
  GetLocalTime(&tim);
  tim.wMilliseconds = 0;
  SystemTimeToFileTime(&tim, (LPFILETIME)&timstamp);
  ntim  = (timstamp/(m_period*onesec))*(m_period*onesec)+(m_period*onesec);
  m_dueTime = ntim;
  long long dtim = (ntim-timstamp)/(onesec);
  int nsec= (int)dtim;
  return nsec;
#else
  long long timstamp,ntim;
	struct timeval tv;
	struct timezone *tz;
	tz = 0;
	gettimeofday(&tv, tz);
  timstamp  = tv.tv_sec;
  timstamp *= onesec;
  timstamp += tv.tv_usec;
  ntim  = (timstamp/(m_period*onesec))*(m_period*onesec)+(m_period*onesec);
  m_dueTime = 10*ntim;
  //printf("due Time: %lli\n",m_dueTime);
  long long dtim = (ntim-timstamp)/(onesec);
  int nsec= (int)dtim;
  return nsec;
#endif

}