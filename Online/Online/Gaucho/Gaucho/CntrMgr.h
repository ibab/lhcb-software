#ifndef GAUCHO_CNTRMGR_H
#define GAUCHO_CNTRMGR_H 1

#include "GaudiKernel/StatEntity.h"
#include <map>
class IMessageSvc;
class CntrMgr
{

public:
//  std::map<const std::string, std::pair<void*, std::string*>, std::less<std::string> > m_counterMap;
//  std::map<const std::string, std::pair<void*, std::string*>, std::less<std::string> >::iterator m_counterMapIt;

  int           *m_runNumber;       // Maybe we have to use double
  int           *m_cycleNumber;
  unsigned int  *m_triggerConfigurationKey;
  void          *m_deltaT;
  double        *m_offsetTimeFirstEvInRun;
  double        *m_offsetTimeLastEvInCycle;
  double        *m_offsetGpsTimeLastEvInCycle;

  int m_maxNumCounters;
  int m_numCounters;


public:
  bool m_newcounter;
  bool m_AllowAdd;
  std::map<const std::string, std::pair<std::string*, std::pair<std::string, void*> >, std::less<std::string> > m_counterMap;
  std::map<const std::string, std::pair<std::string*, std::pair<std::string, void*> >, std::less<std::string> >::iterator m_counterMapIt;
  CntrMgr(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~CntrMgr();

  void addCounter(const std::string& countName, const std::string& countDescription, const double& count)
  {
    if (m_AllowAdd)
    {
      std::string* descr = new std::string(countDescription);
      //m_counterMap[countName] = std::pair<void*, std::string*> ((void*) const_cast<double *>(&count), descr);
      m_counterMap[countName] = std::pair<std::string*, std::pair<std::string, void*> > (descr, std::pair<std::string, void*> ("double", (void*) const_cast<double *>(&count)));
      m_newcounter = true;
    }
    else
    {
//      printf("Counter Manager: Adding Counters not allowed anymore...\n");
    }
  }

  void addCounter(const std::string& countName, const std::string& countDescription, const int& count)
  {

    if (m_AllowAdd)
    {
      std::string* descr = new std::string(countDescription);
      //m_counterMap[countName] = std::pair<void*, std::string*> ((void*) (const_cast<int *>(&count)), descr);
      //m_counterMap[countName] = std::pair<std::string*, std::pair<std::string, void*> > (descr, ("int", (void*) const_cast<double *>(&count)));
      m_counterMap[countName] = std::pair<std::string*, std::pair<std::string, void*> > (descr, std::pair<std::string, void*> ("int", (void*) const_cast<int *>(&count)));
      m_newcounter = true;
    }
    else
    {
//      printf("Counter Manager: Adding Counters not allowed anymore...\n");
    }
  }
  void addCounter(const std::string& countName, const std::string& countDescription, const long& count)
  {

    if (m_AllowAdd)
    {
      std::string* descr = new std::string(countDescription);
      //m_counterMap[countName] = std::pair<void*, std::string*> ((void*) (const_cast<int *>(&count)), descr);
      //m_counterMap[countName] = std::pair<std::string*, std::pair<std::string, void*> > (descr, ("int", (void*) const_cast<double *>(&count)));
      m_counterMap[countName] = std::pair<std::string*, std::pair<std::string, void*> > (descr, std::pair<std::string, void*> ("long", (void*) const_cast<long *>(&count)));
      m_newcounter = true;
    }
    else
    {
//      printf("Counter Manager: Adding Counters not allowed anymore...\n");
    }
  }

  void addCounter(const std::string& countName, const std::string& countDescription, const StatEntity& count)
  {
    //we only make rates of flag() and nEntries()
    if (m_AllowAdd)
    {
      std::string* descr1 = new std::string(countDescription+"-flag");
      std::string* descr2 = new std::string(countDescription+"-nEntries");
      m_counterMap[countName+"-flag"] = std::pair<std::string*, std::pair<std::string, void*> >(descr1,std::pair<std::string,void*>("StatEntityflag",(void*) const_cast<StatEntity *>(&count)));
      m_counterMap[countName+"-nEntries"] = std::pair<std::string*, std::pair<std::string, void*> >(descr2,std::pair<std::string,void*>("StatEntitynEntries",(void*) const_cast<StatEntity *>(&count)));
      m_newcounter = true;
    }
    else
    {
//      printf("Counter Manager: Adding Counters not allowed anymore...\n");
    }
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

//  std::map<const std::string, std::pair<void*, std::string*>, std::less<std::string> > counterMap(){return m_counterMap;}
  std::map<const std::string, std::pair<std::string*, std::pair<std::string, void*> >, std::less<std::string> > counterMap(){return m_counterMap;}
  void setMaxNumCounters(int maxNumCounters) {m_maxNumCounters = maxNumCounters;}
  int numCounters(){return m_numCounters;}
  void setNumCounters(int numCounters){m_numCounters = numCounters;}
  void clearNewCounter(void ){m_newcounter = false;return;};
//private:
  double offsetTimeFirstEvInRun() {return (m_offsetTimeFirstEvInRun!=0 ? *m_offsetTimeFirstEvInRun : 0);}
  double offsetTimeLastEvInCycle() {return (m_offsetTimeLastEvInCycle != 0 ? *m_offsetTimeLastEvInCycle : 0);}
  double offsetGpsTimeLastEvInCycle() {return (m_offsetGpsTimeLastEvInCycle != 0 ? *m_offsetGpsTimeLastEvInCycle : 0);}
  int runNumber() {return (m_runNumber != 0 ? *m_runNumber : 0);}
  unsigned int triggerConfigurationKey() {return (m_triggerConfigurationKey != 0 ? *m_triggerConfigurationKey : 0);}
  int cycleNumber() {return (m_cycleNumber != 0 ? *m_cycleNumber : 0);}
  double deltaT() {return (m_deltaT != 0 ? (double)*(unsigned long long *)m_deltaT : 0);}
  void open(){m_AllowAdd = true;return;}
  void close(){m_AllowAdd = false;return;}
};

#endif //GAUCHO_CNTRMGR_H

