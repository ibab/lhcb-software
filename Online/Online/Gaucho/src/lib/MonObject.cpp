//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/src/lib/MonObject.cpp,v 1.15 2009-04-22 10:18:14 niko Exp $

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

void MonObject::saveBinary(boost::archive::binary_oarchive & ar, const unsigned int version){
  save(ar, version);
}

void MonObject::loadBinary(boost::archive::binary_iarchive & ar, const unsigned int version){
  load(ar, version);
}

void MonObject::saveText(boost::archive::text_oarchive & ar, const unsigned int version){
  save(ar, version);
}

void MonObject::loadText(boost::archive::text_iarchive & ar, const unsigned int version){
  load(ar, version);
}

template <class output_archive>
void MonObject::save(output_archive & ar, const unsigned int version){
  if (version != m_version) m_version = version;

  ar & m_typeName;
  ar & m_version;
  ar & m_comments;
  ar & m_endOfRun;
}

template <class input_archive>
void MonObject::load(input_archive & ar, const unsigned int version){
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
