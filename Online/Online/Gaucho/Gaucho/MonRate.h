#ifndef GAUCHO_MONRATE_H
#define GAUCHO_MONRATE_H 1

#include "Gaucho/MonObject.h"
#include <map>

class MonRate: public MonObject {

protected:
  //std::vector<double> *m_vect; // here we will put all the counters
  //std::vector<double>::const_iterator it;
  
  std::map<const std::string, double*, std::less<std::string> > m_counterMap;
  std::map<const std::string, double*, std::less<std::string> >::iterator m_counterMapIt;
  
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

  void addCounter(const std::string& countName, const double& count) {
    m_counterMap[countName] = const_cast<double *>(&count);
  }
  
  void addComplement(int* runNumber, int* cycleNumber, longlong* timeFirstEvInRun, longlong* timeLastEvInCycle){
    m_runNumber = runNumber;
    m_cycleNumber = cycleNumber;
    m_timeFirstEvInRun = timeFirstEvInRun;
    m_timeLastEvInCycle = timeLastEvInCycle;
  }
  
  double counter(std::string countName) {return (*m_counterMap[countName]);}
  std::map<const std::string, double*, std::less<std::string> > counterMap(){return m_counterMap;}
  
  longlong timeFirstEvInRun() {return (*m_timeFirstEvInRun);}
  longlong timeLastEvInCycle() {return (*m_timeLastEvInCycle);}
  int runNumber() {return (*m_runNumber);}
  int cycleNumber() {return (*m_cycleNumber);}
  
  virtual void combine(MonObject * monObject);
  virtual void copyFrom(MonObject* monObject);
  virtual void reset();
  virtual void add(MonRate* monRate);
  virtual void print();
  virtual void write(){};
};

#endif //GAUCHO_MONRATE_H

