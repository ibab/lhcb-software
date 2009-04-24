#ifndef GAUCHO_MONSTRING_H
#define GAUCHO_MONSTRING_H 1

#include "Gaucho/MonObject.h"

class MonString: public MonObject {

protected:
  std::string *m_string;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonString(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonString();

  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive  & ar, const unsigned int version);

  void setValue(const std::string& s){m_string = const_cast<std::string *>(&s);}
  std::string value(){return (*m_string);}

  virtual void combine(MonObject * monString);
  virtual void copyFrom(MonObject* monObject);
  virtual void reset(){(*m_string)="";}
  virtual void add(MonString * monString);
  virtual void print();
  virtual void write(){};

};

#endif //GAUCHO_MONSTRING_H

