#ifndef GAUCHO_MONINT_H
#define GAUCHO_MONINT_H 1

#include "Gaucho/MonObject.h"

class MonInt: public MonObject {

protected:
  int m_int;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonInt(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonInt();

  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive  & ar, const unsigned int version);

  void setValue(int i){m_int = i;}
  int value(){return m_int;}

  virtual void combine(MonObject * monInt);  
  virtual void copyFrom(MonObject* monInt);  
  virtual void reset(){m_int=0;}
  virtual void add(MonInt * monInt);
  virtual void print();
  virtual void write(){};

};

#endif //GAUCHO_MONINT_H

