#include "Gaucho/MonPairDD.h"

MonPairDD::MonPairDD(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName="MonPairDD";
  m_dimPrefix="MonPDD";
  //m_pair = NULL;
}

MonPairDD::~MonPairDD(){
}

void MonPairDD::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  ar & m_pair.first;
  ar & m_pair.second;
}

void MonPairDD::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  ar & m_pair.first;
  ar & m_pair.second;
}

void MonPairDD::combine(MonObject * monPairDD){
  if (monPairDD->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monPairDD->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  add((MonPairDD*) monPairDD);
  if (m_comments.find("combine=average") != std::string::npos){
    m_pair.first = m_pair.first/2.00;
    m_pair.second = m_pair.second/2.00;
  }
}

void MonPairDD::copyFrom(MonObject * monPairDD){
  if (monPairDD->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<< monPairDD->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  MonPairDD *mo = (MonPairDD*)monPairDD;
  m_pair.first = (mo->value()).first;
  m_pair.second = (mo->value()).second;
  m_comments = mo->comments();
}

void MonPairDD::add(MonPairDD * monPairDD){
  m_pair.first = m_pair.first + (monPairDD->value()).first;
  m_pair.second = m_pair.second + (monPairDD->value()).second;
}

void MonPairDD::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " value:" << endreq;
  msgStream << MSG::INFO << "   first =  "<<m_pair.first << endreq;
  msgStream << MSG::INFO << "   second =  "<<m_pair.second << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;
  doOutputMsgStream(msgStream);

}
