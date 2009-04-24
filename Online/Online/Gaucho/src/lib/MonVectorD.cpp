#include "Gaucho/MonVectorD.h"

MonVectorD::MonVectorD(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName=s_monVectorD;
  m_dimPrefix="MonVD";  
  m_vect = new std::vector<double>();
}
  
MonVectorD::~MonVectorD(){
}

void MonVectorD::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  int size = 0;
  double val = 0.00;
  size = m_vect->size();
  ar & size;
  for (it = m_vect->begin(); it != m_vect->end(); ++it){
    val = (*it);
    ar & val;
  }
}

void MonVectorD::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  int size = 0;
  double val = 0.00;
  ar & size;
  m_vect->resize(size, 0);
  for (int i = 0; i < size; i++)
  {
    ar & val;
    m_vect->at(i) = val;
  }
}

void MonVectorD::combine(MonObject * monObject){
  MsgStream msg = createMsgStream();
  if (monObject->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<monObject->typeName() << " failed." << endreq;
    return;
  }
  if (monObject->endOfRun() != this->endOfRun()){
    msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }
  if (((MonVectorD*)monObject)->value().size() != this->value().size()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to combine vectors with size "<<this->value().size() <<" and "<< ((MonVectorD*)monObject)->value().size() << " failed." << endreq;
    
    return;
  }
  add((MonVectorD*) monObject);
  if (m_comments.find("combine=average") != std::string::npos) {
    for (unsigned int i = 0; i < m_vect->size(); i++)
      m_vect->at(i) = m_vect->at(i)/2.00;
  }
}

void MonVectorD::copyFrom(MonObject * monObject){
  if (monObject->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monObject->typeName() << " failed." << endreq;
    return;
  }
  MonVectorD *mo = (MonVectorD*) monObject;
  m_endOfRun = mo->endOfRun();
  (*m_vect) = mo->value();
  m_comments = mo->comments();
}

void MonVectorD::reset(){
  for (unsigned int i = 0; i < m_vect->size(); i++)
    m_vect->at(i) = 0.00;
}

void MonVectorD::add(MonVectorD * MonVectorD){
  //if (m_int !=NULL)
  for (unsigned int i = 0; i < m_vect->size(); i++)
    m_vect->at(i) = m_vect->at(i) + MonVectorD->value().at(i);
}

void MonVectorD::print(){
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

