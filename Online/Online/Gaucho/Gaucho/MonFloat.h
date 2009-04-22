#ifndef GAUCHO_MONFLOAT_H
#define GAUCHO_MONFLOAT_H 1

#include "Gaucho/MonObject.h"

class MonFloat: public MonObject {

protected:
  float *m_float;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonFloat(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonFloat();

  virtual void saveBinary(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void loadBinary(boost::archive::binary_iarchive & ar, const unsigned int version);

  virtual void saveText(boost::archive::text_oarchive & ar, const unsigned int version);
  virtual void loadText(boost::archive::text_iarchive & ar, const unsigned int version);

  void setValue(const float& f){m_float = const_cast<float *>(&f);}
  float value(){return (*m_float);}

  virtual void combine(MonObject * monFloat);  
  virtual void copyFrom(MonObject* monFloat);  
  virtual void reset(){(*m_float)=0.00;}
  virtual void add(MonFloat * monFloat);
  virtual void print();
  virtual void write(){};

};

#endif //GAUCHO_MONFLOAT_H

