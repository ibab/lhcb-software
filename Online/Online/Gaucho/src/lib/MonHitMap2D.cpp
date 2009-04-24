#include "Gaucho/MonHitMap2D.h"
#include <stdlib.h>
#include <iostream>


MonHitMap2D::MonHitMap2D(IMessageSvc* msgSvc, const std::string& source, int version, std::string name, int fromX, int toX,int fromY, int toY):
MonStatEntity(msgSvc, source, version)
{
  MonHitMap2D(msgSvc, source, name, fromX, toX, fromY, toY);
}

MonHitMap2D::MonHitMap2D(IMessageSvc* msgSvc, const std::string& source, std::string name, int fromX, int toX,int fromY, int toY):
MonStatEntity(msgSvc, source, 0)
{
  m_typeName=s_monHitMap2D;
  m_dimPrefix="MonHM2D";
  m_name = name;
  m_fromX = fromX;
  m_toX = toX;
  m_fromY = fromY;
  m_toY = toY;

  m_chanX = m_toX-m_fromX +1;
  m_chanY = m_toY-m_fromY +1;
  m_chan =  m_chanX*m_chanY;

  m_data = (double *)malloc((m_chan * sizeof(double)));
  if (m_data == NULL) {
    MsgStream msgStream = createMsgStream();
    msgStream << MSG::ERROR << "Out of memory" << endreq;
    

  }
  reset();
}

MonHitMap2D::~MonHitMap2D(){
}

void MonHitMap2D::save(boost::archive::binary_oarchive & ar, const unsigned int version){ 
  MonObject::save(ar,version);
  save2(ar);
}

void MonHitMap2D::save2(boost::archive::binary_oarchive & ar){ 
  MonStatEntity::save2(ar);
  ar & m_fromX; 
  ar & m_toX; 
  ar & m_fromY; 
  ar & m_toY; 
  for (unsigned int i=0;i<m_chan;++i) ar & m_data[i]; 
}

void MonHitMap2D::load(boost::archive::binary_iarchive & ar, const unsigned int version)
{
  MonObject::load(ar,version);
  load2(ar);
}

void MonHitMap2D::load2(boost::archive::binary_iarchive  & ar)
{ 
  if (version() >=0){

    MonStatEntity::load2(ar);

    ar & m_fromX; 
    ar & m_toX; 
    ar & m_fromY; 
    ar & m_toY; 
    m_chanX =  m_toX - m_fromX+1;
    m_chanY =  m_toY - m_fromY+1;
    m_chan = m_chanX*m_chanY;

    m_data = (double *)realloc(m_data,(m_chan) * sizeof(double));

    for (unsigned int i=0;i<m_chan;++i)
    ar & m_data[i]; 
  }
}

void MonHitMap2D::combine(MonObject * H){
  MsgStream msg = createMsgStream();
  if (H->typeName() != this->typeName()){
    msg << MSG::ERROR << "Trying to combine "<<this->typeName() <<" and "<<H->typeName() << " failed."<<endreq; 
    return;
  }
  if (H->endOfRun() != this->endOfRun()){
    msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }
  addHitMap(((MonHitMap2D&)*H));
}

void MonHitMap2D::copyFrom(MonObject * monHitMap2D){
  if (monHitMap2D->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monHitMap2D->typeName() << " failed." << endreq;
    return;
  }
  MonStatEntity::copyFrom(monHitMap2D);
  MonHitMap2D *mo = (MonHitMap2D*) monHitMap2D;
  m_endOfRun = mo->endOfRun();
  if (m_chan == mo->channels())
    for (unsigned int i = 0; i < m_chan; ++i) m_data[i] = mo->data(i);
  m_comments = mo->comments();
}

void MonHitMap2D::addHitMap(MonHitMap2D &H, double w){
  MonStatEntity::add(H);
  if (m_chan == H.channels())
    for (unsigned int i =0;i<m_chan; ++i) m_data[i] += H.data(i)*w;
}

void MonHitMap2D::setBinContent(int x, int y, double f){
  if (x>=m_fromX && x<=m_toX) 
  if (y>=m_fromY && y<=m_toY) m_data[x+m_chanX*y] = f;
}

double MonHitMap2D::binContent(int x, int y){
  if (x>=m_fromX && x<=m_toX)   
    if (y>=m_fromY && y<=m_toY) 
      return m_data[x+m_chanX*y]; 
  return 0;
}

void MonHitMap2D::reset(){
  MonStatEntity::reset();
  for (unsigned int i =0;i<m_chan; ++i) m_data[i] =0;
}

void MonHitMap2D::fill(int x,int y, double f ){
  MonStatEntity::fill(x,y,f);
  if (x>=m_fromX && x<=m_toX)  
    if (y>=m_fromY && y<=m_toY) 
      m_data[x+m_chanX*y] += f; 

  if (m_data[x+m_chanX*y] > m_max) m_max = m_data[x+m_chanX*y];
  if (m_data[x+m_chanX*y] < m_min) m_min = m_data[x+m_chanX*y];

  return;
}

void MonHitMap2D::print(){
  MonStatEntity::print();
  MsgStream msgStream = createMsgStream();
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO << m_typeName << " values :" << endreq;
  for (int y =m_toY;y>=m_fromY;y--){
    for (int x =m_fromX;x<=m_toX;x++){
      msgStream << m_data[x+m_chanX*y]<<" ";
    }
    msgStream << "\n";
  }
  msgStream << "\n";
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  
}

