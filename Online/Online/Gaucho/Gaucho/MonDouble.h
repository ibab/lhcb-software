#ifndef GAUCHO_MONDOUBLE_H
#define GAUCHO_MONDOUBLE_H 1

#include "Gaucho/MonObject.h"

class MonDouble: public MonObject {

protected:
  double *m_double;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonDouble(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonDouble();

  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive  & ar, const unsigned int version);

  void setValue(const double& d){m_double = const_cast<double *>(&d);}
  double value(){return (*m_double);}

  virtual void combine(MonObject * monDouble);  
  virtual void copyFrom(MonObject* monDouble);  
  virtual void reset(){(*m_double)=0.00;}
  virtual void add(MonDouble * monDouble);
  virtual void print();
  virtual void write(){};

};

#endif //GAUCHO_MONDOUBLE_H

