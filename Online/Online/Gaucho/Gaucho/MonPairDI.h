#ifndef GAUCHO_MONPAIRDI_H
#define GAUCHO_MONPAIRDI_H 1

#include "Gaucho/MonObject.h"

class MonPairDI: public MonObject {

protected:
  std::pair<double,int> *m_pair;


public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonPairDI(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonPairDI();

  virtual void saveBinary(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void loadBinary(boost::archive::binary_iarchive & ar, const unsigned int version);

  virtual void saveText(boost::archive::text_oarchive & ar, const unsigned int version);
  virtual void loadText(boost::archive::text_iarchive & ar, const unsigned int version);

  void setValue(const std::pair<double,int>& p){m_pair = const_cast<std::pair<double,int> *>(&p);}
  std::pair<double,int> value(){return (*m_pair);}

  virtual void combine(MonObject * monPair);  
  virtual void copyFrom(MonObject* monPair);  
  virtual void reset(){(*m_pair).first=0.00;(*m_pair).second=0;}
  virtual void add(MonPairDI * monPair);
  virtual void print();
  virtual void write(){};
};

#endif //GAUCHO_MONPAIRDI_H

