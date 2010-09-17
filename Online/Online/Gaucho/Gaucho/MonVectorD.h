#ifndef GAUCHO_MONVECTORD_H
#define GAUCHO_MONVECTORD_H 1

#include "Gaucho/MonObject.h"

class MonVectorD: public MonObject {

protected:
  std::vector<double> *m_vect;
  std::vector<double>::const_iterator it;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonVectorD(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonVectorD();

  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive  & ar);

  void setValue(std::vector<double>& v){m_vect = (&v);}
  std::vector<double> value(){return (*m_vect);}

  virtual void combine(MonObject * monObject);
  virtual void copyFrom(MonObject* monObject);
  virtual void reset();
  virtual void add(MonVectorD* monVectorD);
  virtual void print();
  virtual void write(){};

};

#endif //GAUCHO_MONVECTORD_H

