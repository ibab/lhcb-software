#ifndef GAUCHO_MONPAIRII_H
#define GAUCHO_MONPAIRII_H 1

#include "Gaucho/MonObject.h"

class MonPairII: public MonObject {

protected:
  std::pair<int,int> m_pair;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonPairII(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonPairII();

  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive  & ar, const unsigned int version);

  void setValue(std::pair<int,int> p){m_pair = p;}
  std::pair<int,int> value(){return m_pair;}

  virtual void combine(MonObject * monPair);  
  virtual void copyFrom(MonObject* monPair);  
  virtual void reset(){m_pair.first=0;m_pair.second=0;}
  virtual void add(MonPairII * monPair);
  virtual void print();
  virtual void write(){};
};

#endif //GAUCHO_MONPAIRII_H

