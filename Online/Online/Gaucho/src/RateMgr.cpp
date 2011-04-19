#include "Gaucho/RateMgr.h"

RateMgr::RateMgr(IMessageSvc* , const std::string& , int ) : m_deltaT(0)
{
}

RateMgr::~RateMgr()
{
}

void RateMgr::makeRates()
{
  for (Rateit rateMapIt = m_rateMap.begin();rateMapIt != m_rateMap.end();rateMapIt++)
  {
    MonRateBase *i = rateMapIt->second;
    i->makeRate(m_deltaT);
  }
}
