#ifndef GAUCHO_MONVECTORI_H
#define GAUCHO_MONVECTORI_H 1

#include "Gaucho/MonObject.h"

class MonVectorI: public MonObject {

protected:
  std::vector<int> m_vect;
  std::vector<int>::const_iterator it;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonVectorI(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonVectorI();

  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive  & ar, const unsigned int version);

  void setValue(std::vector<int> v){m_vect = v;}
  std::vector<int> value(){return m_vect;}

  virtual void combine(MonObject * monObject);  
  virtual void copyFrom(MonObject* monObject);  
  virtual void reset();
  virtual void add(MonVectorI * monVectorI);
  virtual void print();
  virtual void write(){};

};

#endif //GAUCHO_MONVECTORI_H

