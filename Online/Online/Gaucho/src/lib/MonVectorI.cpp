#include "Gaucho/MonVectorI.h"

MonVectorI::MonVectorI(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName="MonVectorI";
  m_dimPrefix="MonVI";
  
}
  
MonVectorI::~MonVectorI(){
}

void MonVectorI::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  int val = 0;
  val = m_vect.size();
  ar & val; 
  for (it = m_vect.begin(); it != m_vect.end(); ++it){
    val = (*it);
    ar & val;
  }
}

void MonVectorI::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  int size = 0;
  int val = 0;
  ar & size; 
  m_vect.resize(size, 0);
  for (int i = 0; i < size; i++)
  {
    ar & val;
    m_vect[i] = val;
  }
}

void MonVectorI::combine(MonObject * monObject){
  if (monObject->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monObject->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
//  if (monObject->value().size() != this->value().size()){
//    MsgStream msgStream = createMsgStream();
//    msgStream <<MSG::ERROR<<"Trying to combine vectors with size "<<this->value().size() <<" and "<< monObject->value////().size() << " failed." << endreq;
//    doOutputMsgStream(msgStream);
//    return;
//  }
  add((MonVectorI*) monObject);
  if (m_comments.find("combine=average") != std::string::npos) {
    for (unsigned int i = 0; i < m_vect.size(); i++)
      m_vect[i] = m_vect.at(i)/2;
  }
}

void MonVectorI::copyFrom(MonObject * monObject){
  if (monObject->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monObject->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  MonVectorI *mo = (MonVectorI*) monObject;
  m_vect = mo->value();
  m_comments = mo->comments();
}

void MonVectorI::reset(){
  for (unsigned int i = 0; i < m_vect.size(); i++)
    m_vect[i] = 0;
}


void MonVectorI::add(MonVectorI * MonVectorI){
  //if (m_int !=NULL)
  for (unsigned int i = 0; i < m_vect.size(); i++)
    m_vect[i] = m_vect.at(i) + MonVectorI->value().at(i);
}

void MonVectorI::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " values :" << endreq;
  for (it = m_vect.begin(); it != m_vect.end(); ++it){
    msgStream << MSG::INFO << " value = "<< (*it) << endreq;
  }
  msgStream << MSG::INFO << "*************************************"<<endreq;
  doOutputMsgStream(msgStream);
}

