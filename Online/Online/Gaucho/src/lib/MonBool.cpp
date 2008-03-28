#include "Gaucho/MonBool.h"

MonBool::MonBool(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName=s_monBool;
  m_dimPrefix="MonB";
  m_bool = new bool();
}
  
MonBool::~MonBool(){
}

void MonBool::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  ar & (*m_bool);
}

void MonBool::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  ar & (*m_bool);
}

void MonBool::combine(MonObject * monBool){
  if (monBool->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monBool->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  add((MonBool*) monBool);
}

void MonBool::copyFrom(MonObject * monBool){
  if (monBool->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monBool->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  MonBool *mo = (MonBool*)monBool;
  (*m_bool) = mo->value();
  m_comments = mo->comments();
}

void MonBool::add(MonBool * monBool){
  (*m_bool) = (*m_bool) + monBool->value();
}
  
void MonBool::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " value=" << (*m_bool) << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;
  doOutputMsgStream(msgStream);
}
