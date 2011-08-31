#include "Gaucho/AddTimer.h"
#include "Gaucho/MonAdder.h"
#include <stdexcept>
AddTimer::AddTimer(MonAdder *tis, int period, int typ) : GenTimer((void*)tis,period*1000,typ)
{
  m_owner = tis;
  m_dueTime = 0;
}

AddTimer::~AddTimer( )
{
}

void AddTimer::timerHandler ( void )
{
//  DimLock l;
//  dim_lock();
//  try
//  {
//  printf("Timeout from Adder %s\n",m_owner->m_MyName.c_str());
    m_owner->TimeoutHandler();
//  }
//  catch(const std::exception& e)
//  {
//    ::printf("AddTimer: Exception:%s\n",e.what());
//  }
//  catch(...)
//  {
//    printf("AddTimer: Unknown Exception.\n");
//  }
//  dim_unlock();
}

void AddTimer::Stop()
{
  GenTimer::Stop();
}
