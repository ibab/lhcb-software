#include "Gaucho/MonTimer.h"
#include "Gaucho/MonSubSys.h"
#include <stdexcept>
MonTimer::MonTimer(MonSubSys *HSys, int period) : GenTimer((void*)HSys,period*1000)
{
  m_Hsys = HSys;
  m_dueTime = 0;
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
      m_Hsys->makeRates();
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
  m_Hsys->m_genSrv->Update();
}

void MonTimer::Stop()
{
  m_Hsys->Lock();
  GenTimer::Stop();
  m_Hsys->unLock();
}
