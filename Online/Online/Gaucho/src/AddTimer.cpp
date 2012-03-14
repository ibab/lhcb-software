#include "Gaucho/AddTimer.h"
#include "Gaucho/MonAdder.h"
#include <stdexcept>
AddTimer::AddTimer(MonAdder *tis, int period, int typ) : GenTimer((void*)tis,period*1000,typ)
{
  m_owner = tis;
  m_dueTime = 0;
  this->m_dontdimlock = true;
  this->m_lock.m_name = m_owner->m_name+"_AddTimerLock";
}

AddTimer::~AddTimer( )
{
}

void AddTimer::timerHandler ( void )
{
  int arg = 1;
  DimClient::sendCommandNB(m_owner->m_cmdname.c_str(),&arg,sizeof(arg));
}

void AddTimer::Stop()
{
  GenTimer::Stop();
}
