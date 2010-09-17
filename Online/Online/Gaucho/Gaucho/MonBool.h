#ifndef GAUCHO_MONBOOL_H
#define GAUCHO_MONBOOL_H 1

#include "Gaucho/MonObject.h"

class MonBool: public MonObject {

protected:
  bool *m_bool;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonBool(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonBool();

  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive  & ar);

  void setValue(const bool& b){m_bool = const_cast<bool *>(&b);}
  bool value(){return (*m_bool);}

  virtual void combine(MonObject * monBool);
  virtual void copyFrom(MonObject* monBool);
  virtual void reset(){(*m_bool)=false;}
  virtual void add(MonBool * monBool);
  virtual void print();
  virtual void write(){};

};

#endif //GAUCHO_MONBOOL_H

