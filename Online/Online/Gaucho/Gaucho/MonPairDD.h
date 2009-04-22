#ifndef GAUCHO_MONPAIRDD_H
#define GAUCHO_MONPAIRDD_H 1

#include "Gaucho/MonObject.h"

class MonPairDD: public MonObject {

protected:
  std::pair<double,double> *m_pair;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonPairDD(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonPairDD();

  virtual void saveBinary(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void loadBinary(boost::archive::binary_iarchive & ar, const unsigned int version);

  virtual void saveText(boost::archive::text_oarchive & ar, const unsigned int version);
  virtual void loadText(boost::archive::text_iarchive & ar, const unsigned int version);

  void setValue(const std::pair<double,double>& p){m_pair = const_cast<std::pair<double,double> *>(&p);}
  std::pair<double,double> value(){return (*m_pair);}

  virtual void combine(MonObject * monPair);  
  virtual void copyFrom(MonObject* monPair);  
  virtual void reset(){(*m_pair).first=0.00;(*m_pair).second=0.00;}
  virtual void add(MonPairDD * monPair);
  virtual void print();
  virtual void write(){};
};

#endif //GAUCHO_MONPAIRDD_H

