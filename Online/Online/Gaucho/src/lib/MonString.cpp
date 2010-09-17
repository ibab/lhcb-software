#include "Gaucho/MonString.h"

MonString::MonString(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName=s_monString;
  m_dimPrefix="MonS";
  m_string = new std::string();
}
  
MonString::~MonString(){
}

void MonString::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  ar & (*m_string);
}

void MonString::load(boost::archive::binary_iarchive  & ar)
{
  MonObject::load(ar);
  ar & (*m_string);
}

void MonString::combine(MonObject * monString){
  MsgStream msg = createMsgStream();
  if (monString->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monString->typeName() << " failed." << endreq;
    return;
  }
  if (monString->endOfRun() != this->endOfRun()){
    msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }
  add((MonString*) monString);
}

void MonString::copyFrom(MonObject * monString){
  if (monString->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monString->typeName() << " failed." << endreq;
    return;
  }
  MonString *mo = (MonString*)monString;
  m_endOfRun = mo->endOfRun();
  (*m_string) = mo->value();
  m_comments = mo->comments();
}

void MonString::add(MonString * monString){
  (*m_string) = (*m_string) + monString->value();
}
  
void MonString::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " value=" << (*m_string) << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;
  

}
