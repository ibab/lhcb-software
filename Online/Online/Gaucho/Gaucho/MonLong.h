#ifndef GAUCHO_MONLONG_H
#define GAUCHO_MONLONG_H 1

#include "Gaucho/MonObject.h"

class MonLong: public MonObject {

protected:
  long *m_long;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonLong(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonLong();

  virtual void saveBinary(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void loadBinary(boost::archive::binary_iarchive & ar, const unsigned int version);

  virtual void saveText(boost::archive::text_oarchive & ar, const unsigned int version);
  virtual void loadText(boost::archive::text_iarchive & ar, const unsigned int version);

  void setValue(const long& l){m_long = const_cast<long *>(&l);}
  long value(){return (*m_long);}

  virtual void combine(MonObject * monLong);  
  virtual void copyFrom(MonObject* monLong);  
  virtual void reset(){(*m_long)=0;}
  virtual void add(MonLong * monLong);
  virtual void print();
  virtual void write(){};

};

#endif //GAUCHO_MONLONG_H

