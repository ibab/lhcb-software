#include "Gaucho/RateMgr.h"

RateMgr::RateMgr(IMessageSvc* , const std::string& , int ) : m_deltaT(0)
{
}

RateMgr::~RateMgr()
{
}

void RateMgr::makeRates(unsigned long long dt)
{
  for (Rateit rateMapIt = m_rateMap.begin();rateMapIt != m_rateMap.end();rateMapIt++)
  {
    MonRateBase *i = rateMapIt->second;
    i->makeRate(dt);
  }
}
void RateMgr::print()
{
  for (Rateit rateMapIt = m_rateMap.begin();rateMapIt != m_rateMap.end();rateMapIt++)
  {
    MonRateBase *i = rateMapIt->second;
    printf("%s %f\n",i->name(), i->m_rate);
  }
}
