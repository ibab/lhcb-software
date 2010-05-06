//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/src/lib/MonObject.cpp,v 1.20 2010-05-06 15:23:02 evh Exp $

// Include files
#include "Gaucho/MonObject.h"

MonObject::MonObject(IMessageSvc* msgSvc, const std::string& source, int version)
{
  m_msgSvc = msgSvc;
  m_source = source;
  m_version = version;
  m_typeName = s_monObject;
  m_dimPrefix = "MonObj";
  m_comments = "";
  m_endOfRun = false;
  m_serviceActive = true;
}

MonObject::~MonObject(){

}

void MonObject::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  if (version != m_version) m_version = version;

  ar & m_typeName;
  ar & m_version;
  ar & m_comments;
  ar & m_endOfRun;
}

void MonObject::load(boost::archive::binary_iarchive & ar, const unsigned int version){
  if (version != m_version) m_version = version;
  ar & m_typeName;
  ar & m_version;
  ar & m_comments;
  ar & m_endOfRun;
}

void MonObject::combine(MonObject* monObject) {
  if (monObject==NULL) return; // to hide warning
}

void MonObject::subtract(){
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "MonObject::subtract";
  
}
void MonObject::print(){
  MsgStream msgStream = createMsgStream();
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO << " type:" << typeName()<<endreq;
  msgStream <<MSG::INFO << " version:"<< version()<<endreq;
  msgStream <<MSG::INFO << " comments:"<< comments()<<endreq;
  msgStream <<MSG::INFO << " end of run:"<< endOfRun()<<endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  
}

void MonObject::copyFrom(MonObject* monObject){
  if (monObject==NULL) return;
}

void MonObject::reset(){
  return;
}

MsgStream MonObject::createMsgStream(){
  MsgStream msg(m_msgSvc, m_source + ":" + m_typeName);
  return msg;
}
BOOST_CLASS_VERSION(MonObject, 1)
