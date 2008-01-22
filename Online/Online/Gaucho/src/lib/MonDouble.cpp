#include "Gaucho/MonDouble.h"

MonDouble::MonDouble(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName="MonDouble";
  m_dimPrefix="MonD";
  //m_double = NULL;
}

MonDouble::~MonDouble(){
}

void MonDouble::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  ar & m_double;
}

void MonDouble::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  ar & m_double;
}

void MonDouble::combine(MonObject * monDouble){
  if (monDouble->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monDouble->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  add((MonDouble*) monDouble);
  if (m_comments.find("combine=average") != std::string::npos) m_double = m_double/2.00;
}  

void MonDouble::copyFrom(MonObject * monDouble){
  if (monDouble->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monDouble->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  MonDouble *mo = (MonDouble*)monDouble;
  m_double = mo->value();
  m_comments = mo->comments();
}

void MonDouble::add(MonDouble * monDouble){
  m_double = m_double + monDouble->value();
}
  
void MonDouble::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " value=" << m_double << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;
  doOutputMsgStream(msgStream);
}
