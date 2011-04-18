#ifndef GAUCHO_RATEMGR_H
#define GAUCHO_RATEMGR_H 1

#include "GaudiKernel/StatEntity.h"
#include <map>
#include "Gaucho/MonRate.h"
class IMessageSvc;
class RateMgr
{

public:
  unsigned long long m_deltaT;

  int m_maxNumCounters;
  int m_numCounters;


public:
  bool m_newcounter;
  bool m_AllowAdd;
  std::map<const std::string, MonRateBase*>  m_rateMap;
  typedef std::map<const std::string, MonRateBase*>::iterator Rateit;
  RateMgr(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~RateMgr();

  void addRate(const std::string& countName, const std::string& , MonRateBase& count)
  {
//      std::string* descr = new std::string(countDescription);
      m_rateMap[countName] = &count;
  }

  MonRateBase* removeRate (std::string &name)
  {
    Rateit rateMapIt = m_rateMap.find(name);
    if (rateMapIt != m_rateMap.end())
    {
      MonRateBase *p = rateMapIt->second;
      m_rateMap.erase(rateMapIt);
      return p;
//      delete (rateMapIt->second);
    }
    return 0;
  }
  void removeRateAll ()
  {
//    for (Rateit rateMapIt = m_rateMap.begin();rateMapIt!= m_rateMap.end();rateMapIt++)
//    {
////      delete (rateMapIt->second);
//    }
    m_rateMap.clear();
  }

 void makeRates();
};

#endif //GAUCHO_RATEMGR_H

