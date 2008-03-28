
#include "Gaucho/MonStatEntity.h"
#include <stdlib.h>
#include <iostream>

MonStatEntity::MonStatEntity(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName=s_monStatEntity;
  m_dimPrefix="MonSE";
  m_sumX=0;
  m_sumXw2=0;
  m_sumY=0;
  m_sumYw2=0;
  m_entries=0;
  m_events=0;
  m_min=m_max=0;
}

MonStatEntity::~MonStatEntity(){

}

void MonStatEntity::save(boost::archive::binary_oarchive & ar, const unsigned int version)
{ 
  MonObject::save(ar,version);
  save2(ar);
}

void MonStatEntity::save2(boost::archive::binary_oarchive  & ar)
{
  ar & m_sumX;
  ar & m_sumY;
  ar & m_entries; 
  ar & m_sumXw2; 
  ar & m_sumYw2; 
  ar & m_name; 
}
 
void MonStatEntity::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar,version);
  load2(ar);
}
 
void MonStatEntity::load2(boost::archive::binary_iarchive  & ar)
{ 
  ar & m_sumX;
  ar & m_sumY;
  ar & m_entries; 
  ar & m_sumXw2; 
  ar & m_sumYw2;
  ar & m_name;
}
  
void MonStatEntity::combine(MonObject* S){
  if (S->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<S->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  add( ((MonStatEntity&)*S));
}
 
void MonStatEntity::copyFrom(MonObject * S){
  if (S->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<S->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  MonStatEntity *mo = (MonStatEntity*)S;
  m_sumX=mo->sumY();
  m_sumXw2=mo->sumYw2();
  m_sumY=mo->sumY();
  m_sumYw2=mo->sumYw2();
  m_entries=mo->entries();
  m_events=mo->events();
  m_min=mo->min();
  m_max=mo->max();
  m_comments = mo->comments();
}

void MonStatEntity::reset(){
  m_sumX=0.0;
  m_sumXw2=0.0;
  m_sumY=0.0;
  m_sumYw2=0.0;
  m_entries=0;
  m_events=0;
}

void MonStatEntity::add(MonStatEntity &S){
  m_sumX+=S.sumY();
  m_sumXw2+=S.sumYw2();
  m_sumY+=S.sumY();
  m_sumYw2+=S.sumYw2();
  m_entries+=S.entries();
  m_events+=S.events();
  if (S.min() < m_min) m_min = S.min();
  if (S.max() > m_max) m_max = S.max();
}

void MonStatEntity::fill(float x,float y, float w){
  m_sumX+=x*w;
  m_sumXw2+=x*x*w*w;
  m_sumY+=y*w;
  m_sumYw2+=y*y*w*w;
  m_entries += w;
  // if (f < m_min) m_min = f;
  // if (f > m_max) m_max = f;
}
void MonStatEntity::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO<< "*************************************"<<endreq;
  msgStream << MSG::INFO<<"   " << m_typeName << " value:" << endreq;
  msgStream << MSG::INFO<<"   mean X=" << meanX() << endreq;
  msgStream << MSG::INFO<<"   RMS X="<<rmsX() << endreq;
  msgStream << MSG::INFO<<"   mean Y=" << meanY() << endreq;
  msgStream << MSG::INFO<<"   RMS Y="<<rmsY() << endreq;
  msgStream << MSG::INFO<<"   entries="<<m_entries << endreq;
  msgStream << MSG::INFO<<"   min=" << min() << endreq;
  msgStream << MSG::INFO<<"   max="<<max() << endreq;
  msgStream << MSG::INFO<<"   events="<<m_events << endreq;
  msgStream << MSG::INFO<< "*************************************"<<endreq;
  doOutputMsgStream(msgStream);
}


