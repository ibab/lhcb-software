#include "Gaucho/MonTimer.h"
#include "Gaucho/MonSubSys.h"
#include <stdexcept>
MonTimer::MonTimer(MonSubSys *HSys, int period) : GenTimer((void*)HSys,period*1000)
{
  m_Hsys = HSys;
  m_dueTime = 0;
  m_dontdimlock = true;
  this->m_lock.m_name = m_Hsys->m_name+"_MonTimerLock";
}

MonTimer::~MonTimer( )
{
}

void MonTimer::timerHandler ( void )
{
//
//  Context: separate thread.
//  Timer lock held at entry
//
  MonSubSys::_Lock l(m_Hsys);
  {
    bool need_return = false;

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
    if ( need_return )
    {
      return;
    }
  }
  m_Hsys->m_genSrv->Update();
}

void MonTimer::Stop()
{
  GenTimer::Stop();
}
