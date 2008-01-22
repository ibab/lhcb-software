#include "Gaucho/MonInt.h"

MonInt::MonInt(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName="MonInt";
  m_dimPrefix="MonI";
  //m_int = NULL;
}
  
MonInt::~MonInt(){
}

void MonInt::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  ar & m_int;
}

void MonInt::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  ar & m_int;
}

void MonInt::combine(MonObject * monInt){
  if (monInt->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monInt->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  add((MonInt*) monInt);
  if (m_comments.find("combine=average") != std::string::npos) m_int = m_int/2;
}

void MonInt::copyFrom(MonObject * monInt){
  if (monInt->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monInt->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  MonInt *mo = (MonInt*)monInt;
  m_int = mo->value();
  m_comments = mo->comments();
}

void MonInt::add(MonInt * monInt){
  //if (m_int !=NULL)
  m_int = m_int + monInt->value();
}

void MonInt::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " value=" << m_int << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;
  doOutputMsgStream(msgStream);
}
