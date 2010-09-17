#include "Gaucho/MonPair.h"

MonPair::MonPair(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName="MonPair";
  m_dimPrefix="MonP";
  //m_pair = NULL;
}

MonPair::~MonPair(){
}

void MonPair::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  ar & m_pair.first;
  ar & m_pair.second;
}

void MonPair::load(boost::archive::binary_iarchive  & ar)
{
  MonObject::load(ar);
  ar & m_pair.first;
  ar & m_pair.second;
}

void MonPair::combine(MonObject * monPair){
  MsgStream msg = createMsgStream();
  if (monPair->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monPair->typeName() << " failed." << endreq;
    return;
  }
  if (monPair->endOfRun() != this->endOfRun()){
    msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }
  add((MonPair*) monPair);
  if (m_comments.find("combine=average") != std::string::npos){
    m_pair.first = m_pair.first/2.00;
    m_pair.second = m_pair.second/2.00;
  }
}

void MonPair::copyFrom(MonObject * monPair){
  if (monPair->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monPair->typeName() << " failed." << endreq;
    return;
  }
  MonPair *mo = (MonPair*)monPair;
  m_endOfRun = mo->endOfRun();
  m_pair.first = (mo->value()).first;
  m_pair.second = (mo->value()).second;
  m_comments = mo->comments();
}

void MonPair::add(MonPair * monPair){
  m_pair.first = m_pair.first + (monPair->value()).first;
  m_pair.second = m_pair.second + (monPair->value()).second;
}

void MonPair::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " value:" << endreq;
  msgStream << MSG::INFO << "   first =  "<<m_pair.first << endreq;
  msgStream << MSG::INFO << "   second =  "<<m_pair.second << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;
}
