#ifndef GAUCHO_MONFLOAT_H
#define GAUCHO_MONFLOAT_H 1

#include "Gaucho/MonObject.h"

class MonFloat: public MonObject {

protected:
  float m_float;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonFloat(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonFloat();

  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive  & ar, const unsigned int version);

  void setValue(float f){m_float = f;}
  float value(){return m_float;}

  virtual void combine(MonObject * monFloat);  
  virtual void copyFrom(MonObject* monFloat);  
  virtual void reset(){m_float=0.00;}
  virtual void add(MonFloat * monFloat);
  virtual void print();
  virtual void write(){};

};

#endif //GAUCHO_MONFLOAT_H

