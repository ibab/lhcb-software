#ifndef GAUCHO_MONSTATENTITY_H
#define GAUCHO_MONSTATENTITY_H 1

#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "Gaucho/MonObject.h"
#include "GaudiKernel/StatEntity.h"
#include <pthread.h>


class MonStatEntity: public MonObject{

protected: 
  StatEntity *m_StatEntity;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()
  
  MonStatEntity(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonStatEntity();

  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive & ar);

  void save2(boost::archive::binary_oarchive  & ar);
  void load2(boost::archive::binary_iarchive  & ar);
  
  void splitObject();
  
  double m_nEntries;
  double m_flag;
  double m_flag2; 
  double m_flagMean;
  double m_flagRMS;
  double m_flagMeanErr;
  double m_flagMin;
  double m_flagMax;
  double m_efficiency;
  double m_efficiencyErr;   
  double nEntries() {return m_nEntries;}
  double flag() {return m_flag;}  
  double flag2() {return m_flag2;}  
  double flagMean() {return m_flagMean;}  
  double flagRMS() {return m_flagRMS;}  
  double flagMeanErr() {return m_flagMeanErr;}  
  double flagMin() {return m_flagMin;}  
  double flagMax() {return m_flagMax;}  
  double efficiency() {return m_efficiency;}  
  double efficiencyErr() {return m_efficiencyErr;}  

 
  std::string format();
  virtual void combine(MonObject* S);
  virtual void copyFrom(MonObject* monObject);
  virtual void reset();
  virtual void add(MonStatEntity &S);
  virtual void print();
  virtual void write(){};
  
  void setMonStatEntity(const StatEntity& se);

  void setValue(const StatEntity& val){m_StatEntity = const_cast<StatEntity *>(&val);}
  
  bool isLoaded;
  bool objectCreated;
};
#endif //GAUCHO_MONSTATENTITY_H
