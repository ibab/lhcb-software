#include "Gaucho/MonLong.h"

MonLong::MonLong(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName=s_monLong;
  m_dimPrefix="MonL";
  m_long = new long();
}
  
MonLong::~MonLong(){
}

void MonLong::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  ar & (*m_long);
}

void MonLong::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  ar & (*m_long);
}

void MonLong::combine(MonObject * monLong){
  MsgStream msg = createMsgStream();
  if (monLong->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monLong->typeName() << " failed." << endreq;
    return;
  }
  if (monLong->endOfRun() != this->endOfRun()){
    msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }
  add((MonLong*) monLong);
  if (m_comments.find("combine=average") != std::string::npos) (*m_long) = (*m_long)/2;
}

void MonLong::copyFrom(MonObject * monLong){
  if (monLong->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monLong->typeName() << " failed." << endreq;
    return;
  }
  MonLong *mo = (MonLong*)monLong;
  m_endOfRun = mo->endOfRun();
  (*m_long) = mo->value();
  m_comments = mo->comments();
}

void MonLong::add(MonLong * monLong){
  (*m_long) = (*m_long) + monLong->value();
}
  
void MonLong::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " value=" << (*m_long) << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;
  
}
