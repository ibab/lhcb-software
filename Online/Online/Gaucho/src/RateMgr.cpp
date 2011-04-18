#include "Gaucho/RateMgr.h"

RateMgr::RateMgr(IMessageSvc* , const std::string& , int )
{
//  m_typeName=s_monRate;
//  m_dimPrefix="MonR";
  m_deltaT = 0;
  m_maxNumCounters = 0;
  m_numCounters = 0;
  m_newcounter = false;
  m_rateMap.clear();
}

RateMgr::~RateMgr()
{
  m_rateMap.clear();
}
void RateMgr::makeRates()
{
  for (Rateit rateMapIt = m_rateMap.begin();rateMapIt != m_rateMap.end();rateMapIt++)
  {
    MonRateBase *i=rateMapIt->second;
    i->makeRate(this->m_deltaT);
  }
}
