#include "Gaucho/MonTimer.h"
#include "Gaucho/MonSubSys.h"
#include <stdexcept>
MonTimer::MonTimer(MonSubSys *HSys, int period) : GenTimer((void*)HSys,period*1000)
{
  m_Hsys = HSys;
  m_dueTime = 0;
  this->m_lock.m_name = m_Hsys->m_name+"_TimerLock";
}

MonTimer::~MonTimer( )
{
}

void MonTimer::timerHandler ( void )
{
  {
    bool need_return = false;
    MonSubSys::_Lock l(m_Hsys);

    m_Hsys->m_genSrv->setRunNo(m_Hsys->m_runno);
    m_Hsys->m_genSrv->setTime(m_dueTime);
    try
    {
      m_Hsys->makeRates(m_lastdelta);
      m_Hsys->m_genSrv->Serialize();
    }
    catch(const std::exception& e)
    {
      ::printf("MonTimer: Exception:%s\n",e.what());
      need_return = true;
    }
    catch(...)
    {
      printf("MonTimer: Unknown Exception.\n");
      need_return = true;
    }
    if ( need_return ) {
      return;
    }
  }
  m_lock.lockMutex();
//  m_Hsys->Lock();
  m_Hsys->m_genSrv->Update();
//  m_Hsys->unLock();
  m_lock.unlockMutex();
}

void MonTimer::Stop()
{
  m_lock.lockMutex();
  GenTimer::Stop();
  m_lock.unlockMutex();
}
