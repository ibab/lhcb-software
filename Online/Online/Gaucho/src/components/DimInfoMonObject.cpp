#include "Gaucho/DimInfoMonObject.h"
#include "Gaucho/MonObject.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "Gaucho/MonObjectCreator.h"

#include <iostream>
#include <string>
#include <stdlib.h>
#include <malloc.h>

DimInfoMonObject::DimInfoMonObject(std::string svcName):
  m_hasData(false),
  m_name("DimInfoMonObject"),
  m_svcName(svcName),
  m_source("UndefinedSource"),
  m_monObject(0),
  m_msgSvc(0),
  m_StringSize(-1)
{
  m_dimInfo = new DimInfo((m_svcName).c_str(), (float)-1.0);
}

DimInfoMonObject::DimInfoMonObject(std::string svcName, int refreshTime):
  m_hasData(false),
  m_name("DimInfoMonObject"),
  m_svcName(svcName),
  m_source("UndefinedSource"),
  m_monObject(0),
  m_msgSvc(0),
  m_StringSize(-1)
{
  m_dimInfo = new DimInfo((m_svcName).c_str(), refreshTime, (float)-1.0);
}

DimInfoMonObject::DimInfoMonObject(std::string svcName, int refreshTime, std::string source):
  m_hasData(false),
  m_name("DimInfoMonObject"),
  m_svcName(svcName),
  m_source(source),
  m_monObject(0),
  m_msgSvc(0),
  m_StringSize(-1)
{
  m_dimInfo = new DimInfo((m_svcName).c_str(), refreshTime, (float)-1.0);
}

DimInfoMonObject::~DimInfoMonObject() {
  if (m_monObject) { delete m_monObject; m_monObject = 0;}
  if (m_dimInfo) { delete m_dimInfo; m_dimInfo = 0; }
  // if (m_msgSvc) { delete m_msgSvc; m_msgSvc = 0; } //extern pointer ?
}

void DimInfoMonObject::infoHandler() 
{ // This is not inherited from DimInfo
  if (m_monObject==0) createMonObject();
  else loadMonObject();
}

void DimInfoMonObject::createMonObject(){
  MsgStream msg(msgSvc(), name());
  //msg << MSG::DEBUG << "getting stream" << endreq;
  int tmpStringSize = -1;
  while ( (tmpStringSize = m_dimInfo->getSize()) <=0 ) {
    //msg << MSG::DEBUG << "stream size"<< m_dimInfo->getSize() << endreq;
    usleep(10000);
  }
 
  m_StringSize = tmpStringSize;

  char* c = const_cast<char *>((const char*) m_dimInfo->getData());
  
  //msg << MSG::DEBUG << "creating MonObject base" << endreq;
  MonObject *monObjectBase = new MonObject(m_msgSvc, m_source);
  
  std::stringstream is;
  is.rdbuf()->pubsetbuf(c, m_StringSize);
  boost::archive::binary_iarchive *ia = new boost::archive::binary_iarchive(is);
  monObjectBase->load(*ia, 0);
  std::string monObjectTypeName  = monObjectBase->typeName();
  
  if (m_monObject) {delete m_monObject; m_monObject = 0;}  
  
  //msg << MSG::DEBUG << "creating MonObject" << endreq;
  m_monObject = MonObjectCreator::createMonObject(monObjectTypeName, m_msgSvc, m_source);
  
  std::stringstream is2;
  is2.rdbuf()->pubsetbuf(c, m_StringSize);
  boost::archive::binary_iarchive *ia2 = new boost::archive::binary_iarchive(is2);
  //msg << MSG::DEBUG << "loading MonObject" << endreq;
  m_monObject->load(*ia2, 0);
  
  //msg << MSG::DEBUG << "deleting things" << endreq;
  if (monObjectBase) {delete monObjectBase; monObjectBase = 0;}
  if (ia) {delete ia; ia = 0;}
  if (ia2) {delete ia2; ia2 = 0;}
  
}

void DimInfoMonObject::loadMonObject(){

  int tmpStringSize = -1;
  while ( (tmpStringSize = m_dimInfo->getSize()) <0 ){usleep(10000);}
  m_StringSize = tmpStringSize;
  char* c = const_cast<char *>((const char*) m_dimInfo->getData());
  m_monObject->reset();
 
  //m_dimMonObjectManager->loadMonObject(c, m_StringSize, m_monObject);
  std::stringstream is;
  is.rdbuf()->pubsetbuf(c, m_StringSize);
  boost::archive::binary_iarchive *ia = new boost::archive::binary_iarchive(is);
  m_monObject->load(*ia, 0);
  if (ia) {delete ia; ia = 0;}
}

MonObject *DimInfoMonObject::monObject() {
  return m_monObject;
}


