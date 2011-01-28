#include "Gaucho/CntrMgr.h"

CntrMgr::CntrMgr(IMessageSvc* , const std::string& , int )
{
//  m_typeName=s_monRate;
//  m_dimPrefix="MonR";
  m_runNumber = 0;
  m_triggerConfigurationKey = 0;
  m_cycleNumber = 0;
  m_deltaT = 0;
  m_offsetTimeFirstEvInRun = 0;
  m_offsetTimeLastEvInCycle = 0;
  m_offsetGpsTimeLastEvInCycle = 0;
  m_maxNumCounters = 0;
  m_numCounters = 0;
  m_newcounter = false;
  m_counterMap.clear();
  open();
}

CntrMgr::~CntrMgr()
{
  m_counterMap.clear();
}



