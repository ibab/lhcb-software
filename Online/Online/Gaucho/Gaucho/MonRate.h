#ifndef GAUCHO_MONRATE_H
#define GAUCHO_MONRATE_H 1

#include "Gaucho/MonProfile.h"
#include <map>

class MonRate: public MonProfile {

protected:
//  std::map<const std::string, std::pair<void*, std::string*>, std::less<std::string> > m_counterMap;
//  std::map<const std::string, std::pair<void*, std::string*>, std::less<std::string> >::iterator m_counterMapIt;
  std::map<const std::string, std::pair<std::string*, std::pair<std::string, void*> >, std::less<std::string> > m_counterMap;
  std::map<const std::string, std::pair<std::string*, std::pair<std::string, void*> >, std::less<std::string> >::iterator m_counterMapIt;
  
  int    *m_runNumber;       // Maybe we have to use double
  int    *m_cycleNumber;
  double *m_deltaT;
  double *m_offsetTimeFirstEvInRun;
  double *m_offsetTimeLastEvInCycle; 
  double *m_offsetGpsTimeLastEvInCycle; 
  
  int m_maxNumCounters;
  int m_numCounters;
  bool isServer;
  
    
public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonRate(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonRate();

  virtual void saveBinary(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void saveText(boost::archive::text_oarchive & ar, const unsigned int version);
  virtual void loadBinary(boost::archive::binary_iarchive  & ar, const unsigned int version);
  virtual void loadText(boost::archive::text_iarchive  & ar, const unsigned int version);

  void mapToProfile();

  void addCounter(const std::string& countName, const std::string& countDescription, const double& count) {
    
    std::string* descr = new std::string(countDescription);
    //m_counterMap[countName] = std::pair<void*, std::string*> ((void*) const_cast<double *>(&count), descr);
    m_counterMap[countName] = std::pair<std::string*, std::pair<std::string, void*> > (descr, std::pair<std::string, void*> ("double", (void*) const_cast<double *>(&count)));
  }
  
  void addCounter(const std::string& countName, const std::string& countDescription, const int& count) {
    
    std::string* descr = new std::string(countDescription);
    //m_counterMap[countName] = std::pair<void*, std::string*> ((void*) (const_cast<int *>(&count)), descr);
//    m_counterMap[countName] = std::pair<std::string*, std::pair<std::string, void*> > (descr, ("int", (void*) const_cast<double *>(&count)));
    m_counterMap[countName] = std::pair<std::string*, std::pair<std::string, void*> > (descr, std::pair<std::string, void*> ("int", (void*) const_cast<int *>(&count)));
  }
  void addCounter(const std::string& countName, const std::string& countDescription, const long& count) {
    
    std::string* descr = new std::string(countDescription);
    //m_counterMap[countName] = std::pair<void*, std::string*> ((void*) (const_cast<int *>(&count)), descr);
//    m_counterMap[countName] = std::pair<std::string*, std::pair<std::string, void*> > (descr, ("int", (void*) const_cast<double *>(&count)));
    m_counterMap[countName] = std::pair<std::string*, std::pair<std::string, void*> > (descr, std::pair<std::string, void*> ("long", (void*) const_cast<long *>(&count)));
  }
  
  void addComplement(int* runNumber, int* cycleNumber, double* deltaT, double* offsetTimeFirstEvInRun, double* offsetTimeLastEvInCycle, double* offsetGpsTimeLastEvInCycle){
    m_runNumber = runNumber;
    m_cycleNumber = cycleNumber;
    m_deltaT = deltaT;
    m_offsetTimeFirstEvInRun = offsetTimeFirstEvInRun;
    m_offsetTimeLastEvInCycle = offsetTimeLastEvInCycle;
    m_offsetGpsTimeLastEvInCycle = offsetGpsTimeLastEvInCycle;
  }
  
//  std::map<const std::string, std::pair<void*, std::string*>, std::less<std::string> > counterMap(){return m_counterMap;}
  std::map<const std::string, std::pair<std::string*, std::pair<std::string, void*> >, std::less<std::string> > counterMap(){return m_counterMap;}
  virtual void combine(MonObject * monObject);
  virtual void copyFrom(MonObject* monObject);
  virtual void reset();
  virtual void print();
  virtual void write(){};
  void setMaxNumCounters(int maxNumCounters) {m_maxNumCounters = maxNumCounters;}
  int numCounters(){return m_numCounters;}
  void setNumCounters(int numCounters){m_numCounters = numCounters;}
private:
  //double counter(std::string countName) {return (*(double*) (m_counterMap[countName].first));}
  //std::string counterDescription(std::string countName) {return (*(m_counterMap[countName].second));}
  double offsetTimeFirstEvInRun() {return (*m_offsetTimeFirstEvInRun);}
  double offsetTimeLastEvInCycle() {return (*m_offsetTimeLastEvInCycle);}
  double offsetGpsTimeLastEvInCycle() {return (*m_offsetGpsTimeLastEvInCycle);}
  int runNumber() {return (*m_runNumber);}
  int cycleNumber() {return (*m_cycleNumber);}
  double deltaT() {return (*m_deltaT);}
};

#endif //GAUCHO_MONRATE_H

