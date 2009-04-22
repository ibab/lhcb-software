#ifndef GAUCHO_MONPAIRII_H
#define GAUCHO_MONPAIRII_H 1

#include "Gaucho/MonObject.h"

class MonPairII: public MonObject {

protected:
  std::pair<int,int> *m_pair;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonPairII(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonPairII();

  virtual void saveBinary(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void loadBinary(boost::archive::binary_iarchive & ar, const unsigned int version);

  virtual void saveText(boost::archive::text_oarchive & ar, const unsigned int version);
  virtual void loadText(boost::archive::text_iarchive & ar, const unsigned int version);

  void setValue(const std::pair<int,int>& p){m_pair = const_cast<std::pair<int,int> *>(&p);}
  std::pair<int,int> value(){return (*m_pair);}

  virtual void combine(MonObject * monPair);  
  virtual void copyFrom(MonObject* monPair);  
  virtual void reset(){(*m_pair).first=0;(*m_pair).second=0;}
  virtual void add(MonPairII * monPair);
  virtual void print();
  virtual void write(){};
};

#endif //GAUCHO_MONPAIRII_H

