#include "Gaucho/MonPairII.h"

MonPairII::MonPairII(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName=s_monPairII;
  m_dimPrefix="MonPII";
  m_pair = new std::pair<int,int>();
}

MonPairII::~MonPairII(){
}

void MonPairII::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  ar & (*m_pair).first;
  ar & (*m_pair).second;
}

void MonPairII::load(boost::archive::binary_iarchive  & ar)
{
  MonObject::load(ar);
  ar & (*m_pair).first;
  ar & (*m_pair).second;
}

void MonPairII::combine(MonObject * monPairII){
  MsgStream msg = createMsgStream();
  if (monPairII->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monPairII->typeName() << " failed." << endreq;
    return;
  }
  if (monPairII->endOfRun() != this->endOfRun()){
    msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }
  add((MonPairII*) monPairII);
  if (m_comments.find("combine=average") != std::string::npos){
    (*m_pair).first = (*m_pair).first/2;
    (*m_pair).second = (*m_pair).second/2;
  }
}

void MonPairII::copyFrom(MonObject * monPairII){
  if (monPairII->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<< monPairII->typeName() << " failed." << endreq;
    
    return;
  }
  MonPairII *mo = (MonPairII*)monPairII;
  m_endOfRun = mo->endOfRun();
  (*m_pair).first = (mo->value()).first;
  (*m_pair).second = (mo->value()).second;
  m_comments = mo->comments();
}

void MonPairII::add(MonPairII * monPairII){
  (*m_pair).first = (*m_pair).first + (monPairII->value()).first;
  (*m_pair).second = (*m_pair).second + (monPairII->value()).second;
}

void MonPairII::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " value:" << endreq;
  msgStream << MSG::INFO << "   first =  "<<(*m_pair).first << endreq;
  msgStream << MSG::INFO << "   second =  "<<(*m_pair).second << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;
  

}
