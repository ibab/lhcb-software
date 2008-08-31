#include "Gaucho/DimInfoMonObject.h"
#include "Gaucho/MonObject.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "Gaucho/MonObjectCreator.h"

#include <iostream>
#include <string>
#include <stdlib.h>
#include <malloc.h>

DimInfoMonObject::DimInfoMonObject(std::string svcName, int refreshTime):
  m_hasData(false),
  m_name("DimInfoMonObject"),
  m_svcName(svcName),
  m_source("UndefinedSource"),
  m_monObject(0),
  m_msgSvc(0),
  m_StringSize(-1)
{
  //m_dimInfo = new DimInfo((m_svcName).c_str(), refreshTime, (float)-1.0);
  m_dimInfo = new DimInfo((m_svcName).c_str(), refreshTime, " ");
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
  //m_dimInfo = new DimInfo((m_svcName).c_str(), refreshTime, (float)-1.0);
  m_dimInfo = new DimInfo((m_svcName).c_str(), refreshTime, " ");
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

bool DimInfoMonObject::createMonObject() {
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "getting stream" << endreq;
  
  if (m_monObject) {delete m_monObject; m_monObject = 0;}    
  
  int tmpStringSize = -1;
  int numTent = 0;
  while ( (tmpStringSize = m_dimInfo->getSize()) <= 40 ) {
    msg << MSG::DEBUG << "svcName : " << m_svcName << ", stream size "<< m_dimInfo->getSize() << endreq;
    if (numTent > 40) {
      msg << MSG::WARNING << "==================> INEXISTENT SERVICE " << m_svcName << endreq;
      return false;
    }
    usleep(10000);
    numTent++;
  }
 
  msg << MSG::DEBUG << " ==> stream size = "<< tmpStringSize << endreq;
  
  m_StringSize = tmpStringSize;
  MonObject *monObjectBase;
  boost::archive::binary_iarchive *ia;
  boost::archive::binary_iarchive *ia2;
  
  char* c;
  try {
    c = const_cast<char *>((const char*) m_dimInfo->getData());
    //msg << MSG::DEBUG << "creating MonObject base" << endreq;
    monObjectBase = new MonObject(m_msgSvc, m_source);
    //msg << MSG::DEBUG << "creating MonObject base" << endreq;
    std::stringstream is;
    is.rdbuf()->pubsetbuf(c, m_StringSize);
    //msg << MSG::DEBUG << "creating iarchive 2" << endreq;  
    ia = new boost::archive::binary_iarchive(is);
    //msg << MSG::DEBUG << "before load" << endreq;  
    monObjectBase->load(*ia, 0);
    //msg << MSG::DEBUG << "after MonObject base Load" << endreq;  
    std::string monObjectTypeName  = monObjectBase->typeName();
    //msg << MSG::DEBUG << "creating MonObject" << endreq;
    m_monObject = MonObjectCreator::createMonObject(monObjectTypeName, m_msgSvc, m_source);
    std::stringstream is2;
    is2.rdbuf()->pubsetbuf(c, m_StringSize);
    ia2 = new boost::archive::binary_iarchive(is2);
    //msg << MSG::DEBUG << "loading MonObject" << endreq;
    m_monObject->load(*ia2, 0);
    //msg << MSG::DEBUG << "deleting things" << endreq;
    if (monObjectBase) {delete monObjectBase; monObjectBase = 0;}
    if (ia) {delete ia; ia = 0;}
    if (ia2) {delete ia2; ia2 = 0;}
  }
  catch (const std::exception &ex){
    msg << MSG::INFO << "std::exception: " << ex.what() << endreq;
    msg << MSG::INFO << "check that the server and client are running in the same plataform: 64 or 32 bits." << endreq;
  }  
  catch (...){
    msg << MSG::INFO << "unrecognized exception. "<< endreq;
  } 
  
  return true;
}

bool DimInfoMonObject::loadMonObject(){
  MsgStream msg(msgSvc(), name());
  int tmpStringSize = -1;
  int numTent = 0;
  
  if (!m_monObject) {
    msg << MSG::DEBUG << "svcName : " << m_svcName << " has an uncreated MonObject"<< endreq;
    return false;
  }
  m_monObject->reset(); // <================VERIFICAR ISSO AQUI (ACHO QUE E' DESNECESARIO)

  //msg << MSG::DEBUG << "getting stream" << endreq;
  while ( (tmpStringSize = m_dimInfo->getSize()) < 40 ){
    //msg << MSG::DEBUG << "svcName : " << m_svcName << ", stream size "<< m_dimInfo->getSize() << endreq;
    if (numTent > 40) {
      msg << MSG::WARNING << "==================> INEXISTENT SERVICE " << m_svcName << endreq;
      m_monObject->setServiceActive (false);
      return false;
    }
    usleep(10000);// <===================== This Line produze a crash when we try to stop the Adder 15/08/2008
    numTent++;
  }
  
  //msg << MSG::DEBUG << " ==> stream size = "<< tmpStringSize << endreq;
  m_StringSize = tmpStringSize;
  char* c = const_cast<char *>((const char*) m_dimInfo->getData());
  
  std::stringstream is;
  is.rdbuf()->pubsetbuf(c, m_StringSize);
  boost::archive::binary_iarchive *ia = new boost::archive::binary_iarchive(is);

  m_monObject->load(*ia, 0);
  if (ia) {delete ia; ia = 0;}
  if (!m_monObject->serviceActive ()) m_monObject->setServiceActive (true);
  return true;
}

MonObject *DimInfoMonObject::monObject() {
  return m_monObject;
}


