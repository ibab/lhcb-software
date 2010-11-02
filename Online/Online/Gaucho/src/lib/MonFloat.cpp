#include "Gaucho/MonFloat.h"

MonFloat::MonFloat(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName=s_monFloat;
  m_dimPrefix="MonF";
  m_float = new float();
}
  
MonFloat::~MonFloat(){
}

void MonFloat::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  ar & (*m_float);
}

void MonFloat::load(boost::archive::binary_iarchive  & ar)
{
  MonObject::load(ar);
  ar & (*m_float);
}

void MonFloat::combine(MonObject * monFloat){
  MsgStream msg = createMsgStream();
  if (monFloat->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monFloat->typeName() << " failed." << endreq;
    return;
  }
  if (monFloat->endOfRun() != this->endOfRun()){
    msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }
  add((MonFloat*) monFloat);
  if (m_comments.find("combine=average") != std::string::npos) (*m_float) = (*m_float)/2.00;
}

void MonFloat::copyFrom(MonObject * monFloat){
  if (monFloat->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monFloat->typeName() << " failed." << endreq;
    return;
  }
  MonFloat *mo = (MonFloat*)monFloat;
  m_endOfRun = mo->endOfRun();
  (*m_float) = mo->value();
  m_comments = mo->comments();
}

void MonFloat::add(MonFloat * monFloat){
  (*m_float) = (*m_float) + monFloat->value();
}

void MonFloat::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " value=" << (*m_float) << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;
  
}
