#ifndef GAUCHO_MONPAIR_H
#define GAUCHO_MONPAIR_H 1

#include "Gaucho/MonObject.h"

class MonPair: public MonObject {

protected:
  std::pair<double,double> m_pair;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonPair(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonPair();

  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive  & ar, const unsigned int version);

  void setValue(std::pair<double,double> p){m_pair = p;}
  std::pair<double,double> value(){return m_pair;}

  virtual void combine(MonObject * monPair);  
  virtual void copyFrom(MonObject* monPair);  
  virtual void reset(){m_pair.first=0.00;m_pair.second=0.00;}
  virtual void add(MonPair * monPair);
  virtual void print();
  virtual void write(){};
};

#endif //GAUCHO_MONPAIR_H

