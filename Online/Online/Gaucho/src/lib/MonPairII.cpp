#include "Gaucho/MonPairII.h"

MonPairII::MonPairII(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName="MonPairII";
  m_dimPrefix="MonPII";
  //m_pair = NULL;
}

MonPairII::~MonPairII(){
}

void MonPairII::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  ar & m_pair.first;
  ar & m_pair.second;
}

void MonPairII::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  ar & m_pair.first;
  ar & m_pair.second;
}

void MonPairII::combine(MonObject * monPairII){
  if (monPairII->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monPairII->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  add((MonPairII*) monPairII);
  if (m_comments.find("combine=average") != std::string::npos){
    m_pair.first = m_pair.first/2.00;
    m_pair.second = m_pair.second/2.00;
  }
}

void MonPairII::copyFrom(MonObject * monPairII){
  if (monPairII->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<< monPairII->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  MonPairII *mo = (MonPairII*)monPairII;
  m_pair.first = (mo->value()).first;
  m_pair.second = (mo->value()).second;
  m_comments = mo->comments();
}

void MonPairII::add(MonPairII * monPairII){
  m_pair.first = m_pair.first + (monPairII->value()).first;
  m_pair.second = m_pair.second + (monPairII->value()).second;
}

void MonPairII::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " value:" << endreq;
  msgStream << MSG::INFO << "   first =  "<<m_pair.first << endreq;
  msgStream << MSG::INFO << "   second =  "<<m_pair.second << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;
  doOutputMsgStream(msgStream);

}
