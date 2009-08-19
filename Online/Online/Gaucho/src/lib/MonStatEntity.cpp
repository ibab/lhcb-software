#include "Gaucho/MonStatEntity.h"
#include <stdlib.h>
#include <iostream>

MonStatEntity::MonStatEntity(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName=s_monStatEntity;
  m_dimPrefix="MonSE";
  m_nEntries=0;
  m_flag=0;
  m_flag2=0;
  m_flagMean=0;
  m_flagRMS=0;
  m_flagMeanErr=0;
  m_flagMin=0;
  m_flagMax=0;
  m_efficiency=0;
  m_efficiencyErr=0;  
  m_StatEntity = new StatEntity();
  isLoaded = false;
  objectCreated = true;
}

MonStatEntity::~MonStatEntity(){
  delete m_StatEntity;
}

void MonStatEntity::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  save2(ar);
}


void MonStatEntity::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  load2(ar);
}


std::string MonStatEntity::format() {
   StatEntity * se = (StatEntity *)m_StatEntity; 
   return se->format();
}   


void MonStatEntity::setMonStatEntity(const StatEntity& se) {
  MsgStream msg = createMsgStream();
  msg <<MSG::INFO<<"Setting MonStatEntity." << endreq;
   m_StatEntity = (StatEntity *)&se;
   splitObject();
}

void MonStatEntity::splitObject(){
  MsgStream msg = createMsgStream();
  StatEntity * se = (StatEntity *)m_StatEntity; 
  m_nEntries=se->nEntries();
  m_flag=se->flag();
  m_flag2=se->flag2(); 
  m_flagMean=se->flagMean();
  m_flagRMS=se->flagRMS();
  m_flagMeanErr=se->flagMeanErr();
  m_flagMin=se->flagMin();
  m_flagMax=se->flagMax();
  m_efficiency=se->efficiency();
  m_efficiencyErr=se->efficiencyErr(); 
  
  isLoaded=true;
}

void MonStatEntity::save2(boost::archive::binary_oarchive  & ar){
    ar & m_nEntries;
    ar & m_flag;
    ar & m_flag2;
    ar & m_flagMean;
    ar & m_flagRMS;
    ar & m_flagMeanErr;
    ar & m_flagMin;
    ar & m_flagMax;
    ar & m_efficiency;
    ar & m_efficiencyErr;
}
 
void MonStatEntity::load2(boost::archive::binary_iarchive  & ar){
    ar & m_nEntries;
    ar & m_flag;
    ar & m_flag2;
    ar & m_flagMean;
    ar & m_flagRMS;
    ar & m_flagMeanErr;
    ar & m_flagMin;
    ar & m_flagMax;
    ar & m_efficiency;
    ar & m_efficiencyErr;    
}
  
void MonStatEntity::combine(MonObject* S){
  MsgStream msg = createMsgStream();
  if (S->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<S->typeName() << " failed." << endreq;
    return;
  }
  if (S->endOfRun() != this->endOfRun()){
    msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }
  add( ((MonStatEntity&)*S));
}
 
void MonStatEntity::copyFrom(MonObject * S){
  if (S->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<S->typeName() << " failed." << endreq;
    return;
  }
  MonStatEntity *mo = (MonStatEntity*)S;
  m_endOfRun = mo->endOfRun();
  m_nEntries= mo->nEntries();
  m_flag= mo->flag();
  m_flag2= mo->flag2();
  m_flagMean= mo->flagMean();
  m_flagRMS= mo->flagRMS();
  m_flagMeanErr= mo->flagMeanErr();
  m_flagMin= mo->flagMin();
  m_flagMax= mo->flagMax();
  m_efficiency= mo->efficiency();
  m_efficiencyErr= mo->efficiencyErr();
  m_comments = mo->comments();
}

void MonStatEntity::reset(){
    m_nEntries= 0.0;
    m_flag= 0.0;
    m_flag2= 0.0;
    m_flagMean= 0.0;
    m_flagRMS= 0.0;
    m_flagMeanErr= 0.0;
    m_flagMin= 0.0;
    m_flagMax= 0.0;
    m_efficiency= 0.0;
    m_efficiencyErr= 0.0;
}

void MonStatEntity::add(MonStatEntity &S){
    m_nEntries+= S.nEntries();
    m_flag+= S.flag();
    m_flag2+= S.flag2();    
    m_flagMin = std::min(m_flagMin,S.flagMin());
    m_flagMax = std::max(m_flagMax,S.flagMax());
}

void MonStatEntity::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO<< "*************************************"<<endreq;
  msgStream << MSG::INFO<<"   " << m_typeName << " value:" << endreq;
  msgStream << MSG::INFO<<"   flagmean =" << flagMean() << endreq;
  msgStream << MSG::INFO<<"   flagRMS ="<<flagRMS() << endreq;
  msgStream << MSG::INFO<<"   flagmeanErr=" << flagMeanErr() << endreq;
  msgStream << MSG::INFO<<"   nEntries="<< nEntries() << endreq;
  msgStream << MSG::INFO<<"   flagMin=" << flagMin() << endreq;
  msgStream << MSG::INFO<<"   flagMax="<< flagMax() << endreq;
  msgStream << MSG::INFO<<"   efficiency="<< efficiency() << endreq;
  msgStream << MSG::INFO<<"   efficiencyErr="<< efficiencyErr() << endreq;
  msgStream << MSG::INFO<< "*************************************"<<endreq;
}


