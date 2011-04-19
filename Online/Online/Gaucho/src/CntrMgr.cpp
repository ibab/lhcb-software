#include "Gaucho/CntrMgr.h"

CntrMgr::CntrMgr(IMessageSvc* , const std::string& , int )
{
  m_runNumber = 0;
  m_triggerConfigurationKey = 0;
  m_cycleNumber = 0;
  m_deltaT = 0;
  m_offsetTimeFirstEvInRun = 0;
  m_offsetTimeLastEvInCycle = 0;
  m_offsetGpsTimeLastEvInCycle = 0;
  open();
}

CntrMgr::~CntrMgr()
{
}

void CntrMgr::i_addCounter(const std::string& nam, const std::string& descr, const std::string& type, const void* count) 
{
  if (m_AllowAdd)
  {
    m_counterMap[nam] = std::make_pair(descr,std::make_pair(type,&count));
    m_newcounter = true;
  }
}
