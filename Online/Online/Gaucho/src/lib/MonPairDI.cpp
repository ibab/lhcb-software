#include "Gaucho/MonPairDI.h"

MonPairDI::MonPairDI(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName=s_monPairDI;
  m_dimPrefix="MonPDI";
  m_pair = new std::pair<double,int>();
}

MonPairDI::~MonPairDI(){
}

void MonPairDI::saveBinary(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  ar & (*m_pair).first;
  ar & (*m_pair).second;
}

void MonPairDI::saveText(boost::archive::text_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  ar & (*m_pair).first;
  ar & (*m_pair).second;
}

void MonPairDI::loadBinary(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  ar & (*m_pair).first;
  ar & (*m_pair).second;
}

void MonPairDI::loadText(boost::archive::text_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  ar & (*m_pair).first;
  ar & (*m_pair).second;
}

void MonPairDI::combine(MonObject * monPairDI){
  MsgStream msg = createMsgStream();
  if (monPairDI->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monPairDI->typeName() << " failed." << endreq;
    return;
  }
  if (monPairDI->endOfRun() != this->endOfRun()){
    msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }
  add((MonPairDI*) monPairDI);
  if (m_comments.find("combine=average") != std::string::npos){
    (*m_pair).first = (*m_pair).first/2.00;
    (*m_pair).second = (*m_pair).second/2;
  }
}

void MonPairDI::copyFrom(MonObject * monPairDI){
  if (monPairDI->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<< monPairDI->typeName() << " failed." << endreq;
    return;
  }
  MonPairDI *mo = (MonPairDI*)monPairDI;
  m_endOfRun = mo->endOfRun();
  (*m_pair).first = (mo->value()).first;
  (*m_pair).second = (mo->value()).second;
  m_comments = mo->comments();
}

void MonPairDI::add(MonPairDI * monPairDI){
  (*m_pair).first = (*m_pair).first + (monPairDI->value()).first;
  (*m_pair).second = (*m_pair).second + (monPairDI->value()).second;
}

void MonPairDI::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " value:" << endreq;
  msgStream << MSG::INFO << "   first =  "<<(*m_pair).first << endreq;
  msgStream << MSG::INFO << "   second =  "<<(*m_pair).second << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;
  

}

