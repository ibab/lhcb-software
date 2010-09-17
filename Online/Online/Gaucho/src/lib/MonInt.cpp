#include "Gaucho/MonInt.h"

MonInt::MonInt(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName=s_monInt;
  m_dimPrefix="MonI";
  m_int = new int();
}
  
MonInt::~MonInt(){
}

void MonInt::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  int val = (*m_int);
  ar & val;
}

void MonInt::load(boost::archive::binary_iarchive  & ar)
{
  MonObject::load(ar);
  int val;
  ar & val;
  (*m_int) = val;
}

void MonInt::combine(MonObject * monInt){
  MsgStream msg = createMsgStream();
  if (monInt->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monInt->typeName() << " failed." << endreq;
    return;
  }
  if (monInt->endOfRun() != this->endOfRun()){
    msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }
  add((MonInt*) monInt);
  if (m_comments.find("combine=average") != std::string::npos) (*m_int) = (*m_int)/2;
}

void MonInt::copyFrom(MonObject * monInt){
  if (monInt->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monInt->typeName() << " failed." << endreq;
    return;
  }
  MonInt *mo = (MonInt*)monInt;
  m_endOfRun = mo->endOfRun();
  (*m_int) = mo->value();
  m_comments = mo->comments();
}

void MonInt::add(MonInt * monInt){
  (*m_int) = (*m_int) + monInt->value();
}

void MonInt::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " value=" << (*m_int) << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;
  
}
