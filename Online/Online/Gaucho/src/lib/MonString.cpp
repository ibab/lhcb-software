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

void MonString::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  ar & (*m_string);
}

void MonString::combine(MonObject * monString){
  if (monString->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monString->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  add((MonString*) monString);
}

void MonString::copyFrom(MonObject * monString){
  if (monString->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monString->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  MonString *mo = (MonString*)monString;
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
  doOutputMsgStream(msgStream);

}
