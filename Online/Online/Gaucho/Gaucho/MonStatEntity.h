#ifndef GAUCHO_MONSTATENTITY_H
#define GAUCHO_MONSTATENTITY_H 1

#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "Gaucho/MonObject.h"


#include <pthread.h>

class MonStatEntity: public MonObject{

protected:
  float m_min;
  float m_max;
  float m_sumX;
  float m_sumXw2;
  float m_entries;
  float m_sumY;
  float m_sumYw2;
  std::string m_name;
  float m_events;

  BOOST_SERIALIZATION_SPLIT_MEMBER()

public:
  MonStatEntity(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonStatEntity();

  virtual void saveBinary(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void loadBinary(boost::archive::binary_iarchive & ar, const unsigned int version);

  virtual void saveText(boost::archive::text_oarchive & ar, const unsigned int version);
  virtual void loadText(boost::archive::text_iarchive & ar, const unsigned int version);

  template <class output_archive>
  void save2(output_archive  & ar);
  template <class input_archive>
  void load2(input_archive  & ar);

  float sumX(){return m_sumX;}
  float sumXw2(){return m_sumXw2;}

  float sumY(){return m_sumY;}
  float sumYw2(){return m_sumYw2;}
  float entries(){return m_entries;}
  float events(){return m_events;}
  void incevents(){m_events++;}

  float min(){return m_min;}
  float max(){return m_max;}

  float meanX(){return m_sumX/m_entries;}
  float rmsX(){return sqrt( (m_sumXw2 -2*meanX()*m_sumX + m_sumX*meanX()) / m_entries );}

  float meanY(){return m_sumY/m_entries;}
  float rmsY(){return sqrt( (m_sumYw2 -2*meanY()*m_sumY + m_sumY*meanY()) / m_entries );}

  virtual void combine(MonObject* S);
  virtual void copyFrom(MonObject* monObject);
  virtual void reset();
  virtual void add(MonStatEntity &S);
  virtual void print();
  virtual void write(){};
  void fill(float x,float y, float w=1);

};
#endif //GAUCHO_MONSTATENTITY_H
