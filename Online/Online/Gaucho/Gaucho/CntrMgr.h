#ifndef GAUCHO_CNTRMGR_H
#define GAUCHO_CNTRMGR_H 1

#include "GaudiKernel/StatEntity.h"
#include <map>
class IMessageSvc;
class CntrMgr
{
public:
  typedef std::map<std::string, std::pair<std::string,std::pair<std::string, const void*> > > CounterMap;
  typedef CounterMap::iterator counterMapIt;

protected:
  int           *m_runNumber;       // Maybe we have to use double
  int           *m_cycleNumber;
  unsigned int  *m_triggerConfigurationKey;
  void          *m_deltaT;
  double        *m_offsetTimeFirstEvInRun;
  double        *m_offsetTimeLastEvInCycle;
  double        *m_offsetGpsTimeLastEvInCycle;

  bool m_AllowAdd;

public:
  bool m_newcounter;
  CounterMap m_counterMap;

  CntrMgr(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~CntrMgr();

  void i_addCounter(const std::string& nam, const std::string& descr, const std::string& type, const void* count);

  void addCounter(const std::string& countName, const std::string& countDescription, const double& count)
  {
    i_addCounter(countName,countDescription,"double",&count);
  }
  void addCounter(const std::string& countName, const std::string& countDescription, const int& count)
  {
    i_addCounter(countName,countDescription,"int",&count);
  }
  void addCounter(const std::string& countName, const std::string& countDescription, const long& count)
  {
    i_addCounter(countName,countDescription,"long",&count);
  }
  void addCounter(const std::string& countName, const std::string& countDescription, const StatEntity& count)
  {
    i_addCounter(countName+"-flag",countDescription+"-flag","StatEntityflag",&count);
    i_addCounter(countName+"-nEntries",countDescription+"-nEntries","StatEntitynEntries",&count);
  }
  void removeCounter (std::string &name)
  {
    counterMapIt it = m_counterMap.find(name);
    if (it != m_counterMap.end())
    {
      m_counterMap.erase(it);
    }
  }
  void removeCounterAll()
  {
    m_counterMap.clear();
  }

  void addComplement(int* runNumber, unsigned int* triggerConfigurationKey, int* cycleNumber, double* deltaT, double* offsetTimeFirstEvInRun, double* offsetTimeLastEvInCycle, double* offsetGpsTimeLastEvInCycle)
  {
    m_runNumber = runNumber;
    m_triggerConfigurationKey = triggerConfigurationKey;
    m_cycleNumber = cycleNumber;
    m_deltaT = (void*)deltaT;
    m_offsetTimeFirstEvInRun = offsetTimeFirstEvInRun;
    m_offsetTimeLastEvInCycle = offsetTimeLastEvInCycle;
    m_offsetGpsTimeLastEvInCycle = offsetGpsTimeLastEvInCycle;
  }

  double offsetTimeFirstEvInRun()  const       { return (m_offsetTimeFirstEvInRun!=0 ? *m_offsetTimeFirstEvInRun : 0);}
  double offsetTimeLastEvInCycle() const       { return (m_offsetTimeLastEvInCycle != 0 ? *m_offsetTimeLastEvInCycle : 0);}
  double offsetGpsTimeLastEvInCycle() const    { return (m_offsetGpsTimeLastEvInCycle != 0 ? *m_offsetGpsTimeLastEvInCycle : 0);}
  int runNumber() const                        { return (m_runNumber != 0 ? *m_runNumber : 0);}
  unsigned int triggerConfigurationKey() const { return (m_triggerConfigurationKey != 0 ? *m_triggerConfigurationKey : 0);}
  int cycleNumber() const                      { return (m_cycleNumber != 0 ? *m_cycleNumber : 0);}
  double deltaT()  const                       { return (m_deltaT != 0 ? (double)*(unsigned long long *)m_deltaT : 0);}
  void open()                                  { m_AllowAdd = true;return;}
  void close()                                 { m_AllowAdd = false;return;}
};

#endif //GAUCHO_CNTRMGR_H

