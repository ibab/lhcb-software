#ifndef GAUCHO_MONRATE_H
#define GAUCHO_MONRATE_H 1

#include "Gaucho/MonProfile.h"
#include <map>

class MonRate: public MonProfile {

protected:
  //std::vector<double> *m_vect; // here we will put all the counters
  //std::vector<double>::const_iterator it;
  
  std::map<const std::string, std::pair<double*, std::string*>, std::less<std::string> > m_counterMap;
  std::map<const std::string, std::pair<double*, std::string*>, std::less<std::string> >::iterator m_counterMapIt;
  
/*  std::map<const std::string, double*, std::less<std::string> > m_counterMap;
  std::map<const std::string, double*, std::less<std::string> >::iterator m_counterMapIt;*/
  
  int    *m_runNumber;       // Maybe we have to use double
  int    *m_cycleNumber;
  longlong *m_timeFirstEvInRun;
  longlong *m_timeLastEvInCycle; // Maybe we have to use double, have to check serialization
  
public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonRate(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonRate();

  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive  & ar, const unsigned int version);

  void addCounter(const std::string& countName, const std::string& countDescription, const double& count) {
    //m_counterMap[countName] = const_cast<double *>(&count);
    //m_counterMap[countName] = const_cast<std::pair<double*, std::string*> > (&count, &countTitle);
    
    std::string* descr = new std::string(countDescription);
    m_counterMap[countName] = std::pair<double*, std::string*> (const_cast<double *>(&count), descr);
  }
  
  void addComplement(int* runNumber, int* cycleNumber, longlong* timeFirstEvInRun, longlong* timeLastEvInCycle){
    m_runNumber = runNumber;
    m_cycleNumber = cycleNumber;
    m_timeFirstEvInRun = timeFirstEvInRun;
    m_timeLastEvInCycle = timeLastEvInCycle;
  }
  
  double counter(std::string countName) {return (*(m_counterMap[countName].first));}
  std::string counterDescription(std::string countName) {return (*(m_counterMap[countName].second));}
  
  std::map<const std::string, std::pair<double*, std::string*>, std::less<std::string> > counterMap(){return m_counterMap;}
  
  longlong timeFirstEvInRun() {return (*m_timeFirstEvInRun);}
  longlong timeLastEvInCycle() {return (*m_timeLastEvInCycle);}
  int runNumber() {return (*m_runNumber);}
  int cycleNumber() {return (*m_cycleNumber);}
  
  virtual void combine(MonObject * monObject);
  virtual void copyFrom(MonObject* monObject);
  virtual void reset();
  virtual void print();
  virtual void write(){};
};

#endif //GAUCHO_MONRATE_H

