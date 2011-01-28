#include "Gaucho/MonTimer.h"
#include "Gaucho/MonSubSys.h"
#include "Gaucho/ObjService.h"
#include "stdio.h"
//#include "RTL/rtl.h"
//#include "errno.h"
//#ifdef WIN32
//#include "windows.h"
//#define onesec (unsigned long long)(10000000)
//#else
//#include <sys/time.h>
//#define onesec_mu (unsigned long long)(1000000)
//#define onesec_nano (unsigned long long)(1000000000)
//#endif
//#include <time.h>
MonTimer::MonTimer(MonSubSys *HSys, int period) : GenTimer((void*)HSys,period*1000)
{
  m_Hsys = HSys;
  m_dueTime = 0;
//  printf("==================MonTimer Constructor.................%llu\n",m_period);
}

MonTimer::~MonTimer( )
{
}
void MonTimer::timerHandler ( void )
{
//  printf(" timer_handler Monitor Sun System Locking\n");
  m_Hsys->Lock();
//  printf("++++++++++++++++timer_handler Monitor Sun System LockED\n");
  m_Hsys->m_genSrv->setRunNo(m_Hsys->m_runno);
  m_Hsys->m_genSrv->setTime(m_dueTime);
  dim_lock();
//  printf("Updating the monitor service\n");
  m_Hsys->m_genSrv->Updater();
  dim_unlock();
//  printf(" timer_handler Monitor Sun System Un-Locking\n");
  m_Hsys->unLock();
//  printf("---------------timer_handler Monitor Sun System Un-LockED\n");
}
void MonTimer::Stop()
{
  m_Hsys->Lock();
  GenTimer::Stop();
  m_Hsys->unLock();
}
