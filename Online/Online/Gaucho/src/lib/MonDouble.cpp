#include "Gaucho/MonDouble.h"

MonDouble::MonDouble(IMessageSvc* msgSvc, const std::string& source, int version):
  MonObject(msgSvc, source, version)
{
  m_typeName = s_monDouble;
  m_dimPrefix = "MonD";
  m_double = new double();
}

MonDouble::~MonDouble(){
  //if (m_double) {delete m_double; m_double = NULL;}
}

void MonDouble::saveBinary(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  ar & (*m_double);
}

void MonDouble::saveText(boost::archive::text_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  ar & (*m_double);
}

void MonDouble::loadBinary(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  ar & (*m_double);
}

void MonDouble::loadText(boost::archive::text_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  ar & (*m_double);
}

void MonDouble::combine(MonObject * monDouble){
  MsgStream msg= createMsgStream();
  if (monDouble->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monDouble->typeName() << " failed." << endreq;
    return;
  }
  if (monDouble->endOfRun() != this->endOfRun()){
    msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }
  add((MonDouble*) monDouble);
  if (m_comments.find("combine=average") != std::string::npos) (*m_double) = (*m_double)/2.00;
}  

void MonDouble::copyFrom(MonObject * monDouble){
  if (monDouble->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monDouble->typeName() << " failed." << endreq;
    return;
  }
  MonDouble *mo = (MonDouble*)monDouble;
  m_endOfRun = mo->endOfRun();
  (*m_double) = mo->value();
  m_comments = mo->comments();
}

void MonDouble::add(MonDouble * monDouble){
  (*m_double) = (*m_double) + monDouble->value();
}
  
void MonDouble::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " value=" << (*m_double) << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;
  
}
