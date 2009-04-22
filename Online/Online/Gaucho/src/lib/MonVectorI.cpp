#include "Gaucho/MonVectorI.h"

MonVectorI::MonVectorI(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName=s_monVectorI;
  m_dimPrefix="MonVI";
  m_vect = new std::vector<int>();
}
  
MonVectorI::~MonVectorI(){
}

void MonVectorI::saveBinary(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  int size = 0;
  int val = 0;
  size = m_vect->size();
  ar & size; 
  for (it = m_vect->begin(); it != m_vect->end(); ++it){
    val = (*it);
    ar & val;
  }
}

void MonVectorI::saveText(boost::archive::text_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  int size = 0;
  int val = 0;
  size = m_vect->size();
  ar & size; 
  for (it = m_vect->begin(); it != m_vect->end(); ++it){
    val = (*it);
    ar & val;
  }
}

void MonVectorI::loadBinary(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  int size = 0;
  int val = 0;
  ar & size; 
  m_vect->resize(size, 0);
  for (int i = 0; i < size; i++)
  {
    ar & val;
    m_vect->at(i) = val;
  }
}

void MonVectorI::loadText(boost::archive::text_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  int size = 0;
  int val = 0;
  ar & size; 
  m_vect->resize(size, 0);
  for (int i = 0; i < size; i++)
  {
    ar & val;
    m_vect->at(i) = val;
  }
}

void MonVectorI::combine(MonObject * monObject){
  MsgStream msg = createMsgStream();
  if (monObject->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monObject->typeName() << " failed." << endreq;
    return;
  }
  if (monObject->endOfRun() != this->endOfRun()){
    msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }
  if (((MonVectorI*)monObject)->value().size() != this->value().size()){
    msg <<MSG::ERROR<<"Trying to combine vectors with size "<<this->value().size() <<" and "<< ((MonVectorI*)monObject)->value().size() << " failed." << endreq;
    return;
  }
  add((MonVectorI*) monObject);
  if (m_comments.find("combine=average") != std::string::npos) {
    for (unsigned int i = 0; i < m_vect->size(); i++)
      m_vect->at(i) = m_vect->at(i)/2;
  }
}

void MonVectorI::copyFrom(MonObject * monObject){
  if (monObject->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monObject->typeName() << " failed." << endreq;
    return;
  }
  MonVectorI *mo = (MonVectorI*) monObject;
  m_endOfRun = mo->endOfRun();
  (*m_vect) = mo->value();
  m_comments = mo->comments();
}

void MonVectorI::reset(){
  for (unsigned int i = 0; i < m_vect->size(); i++)
    m_vect->at(i) = 0;
}

void MonVectorI::add(MonVectorI * MonVectorI){
  //if (m_int !=NULL)
  for (unsigned int i = 0; i < m_vect->size(); i++)
    m_vect->at(i) = m_vect->at(i) + MonVectorI->value().at(i);
}

void MonVectorI::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " size :"<< m_vect->size() << endreq;
  msgStream << MSG::INFO << m_typeName << " values :" << endreq;
  for (it = m_vect->begin(); it != m_vect->end(); ++it){
    msgStream << MSG::INFO << " value = "<< (*it) << endreq;
  }
  msgStream << MSG::INFO << "*************************************"<<endreq;
  
}


