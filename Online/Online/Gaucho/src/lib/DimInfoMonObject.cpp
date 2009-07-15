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
  m_noValidMonObject("No valid MonObject"),
  m_monObject(0),
  m_msgSvc(0),
  m_StringSize(-1)
{
  //m_dimInfo = new DimInfo((m_svcName).c_str(), refreshTime, (float)-1.0);
  m_dimInfo = new DimInfo((m_svcName).c_str(), refreshTime, (char*)m_noValidMonObject.c_str());
}

DimInfoMonObject::DimInfoMonObject(std::string svcName, int refreshTime, std::string source):
  m_hasData(false),
  m_name("DimInfoMonObject"),
  m_svcName(svcName),
  m_source(source),
  m_noValidMonObject("No valid MonObject"),
  m_monObject(0),
  m_msgSvc(0),
  m_StringSize(-1)
{
  //m_dimInfo = new DimInfo((m_svcName).c_str(), refreshTime, (float)-1.0);
  m_dimInfo = new DimInfo((m_svcName).c_str(), refreshTime, (char*)m_noValidMonObject.c_str());
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
  if (m_monObject) {delete m_monObject; m_monObject = 0;}
  
  int tmpStringSize = m_dimInfo->getSize()/sizeof(char);
  while ( tmpStringSize <= 0 ) {
    //gSystem->Sleep(m_waitTime);
    usleep(10);
    tmpStringSize = m_dimInfo->getSize()/sizeof(char);
    //msg << MSG::DEBUG << "size for service "<< m_svcName << " " << tmpStringSize << endreq;
  }
  msg << MSG::DEBUG << "size for service "<< m_svcName << " " << tmpStringSize << endreq;
  m_StringSize = tmpStringSize;
  MonObject *monObjectBase;
  
  char* c;
  try {
    c = const_cast<char *>((const char*) m_dimInfo->getData());
    if ( 0 == m_noValidMonObject.compare(c)) {
      //msg << MSG::WARNING << "timeout trying to create  service "<< m_svcName << endreq;
      msg << MSG::DEBUG << m_noValidMonObject << ", for service "<< m_svcName << endreq;
      return false;
    }
    //msg << MSG::DEBUG << "creating MonObject base" << endreq;
    monObjectBase = new MonObject(m_msgSvc, m_source);
    //msg << MSG::DEBUG << "creating MonObject base" << endreq;
    std::stringstream is;
    is.rdbuf()->pubsetbuf(c, m_StringSize);
    //msg << MSG::DEBUG << "creating iarchive 2" << endreq;  
    boost::archive::binary_iarchive ia(is);
    //msg << MSG::DEBUG << "before load" << endreq;  
    monObjectBase->load(ia, 0);
    //msg << MSG::DEBUG << "after MonObject base Load" << endreq;  
    std::string monObjectTypeName  = monObjectBase->typeName();
    //msg << MSG::DEBUG << "creating MonObject" << endreq;
    m_monObject = MonObjectCreator::createMonObject(monObjectTypeName, m_msgSvc, m_source);
    std::stringstream is2;
    is2.rdbuf()->pubsetbuf(c, m_StringSize);
    boost::archive::binary_iarchive ia2(is2);
    //msg << MSG::DEBUG << "loading MonObject" << endreq;
    m_monObject->load(ia2, 0);
    //msg << MSG::DEBUG << "deleting " << endreq;
    if (monObjectBase) {delete monObjectBase; monObjectBase = 0;}
  }
  catch (const std::exception &ex){
    msg << MSG::WARNING << "std::exception: " << ex.what() << endreq;
    //msg << MSG::WARNING << "check that the server and client are running in the same plataform: 64 or 32 bits." << endreq;
    return false;
  }  
  catch (...){
    msg << MSG::WARNING << "unrecognized exception. "<< endreq;
    return false;
  } 
  
  return true;
}

bool DimInfoMonObject::loadMonObject(){
  MsgStream msg(msgSvc(), name());
  
  if (!m_monObject) {
    msg << MSG::WARNING << "svcName : " << m_svcName << " has an uncreated MonObject"<< endreq;
    return false;
  }
  m_monObject->reset(); // <================VERIFICAR ISSO AQUI (ACHO QUE E' DESNECESARIO)

  int tmpStringSize = m_dimInfo->getSize()/sizeof(char);
  while ( tmpStringSize <= 0 ) {
    //gSystem->Sleep(m_waitTime);
    usleep(10);
    tmpStringSize = m_dimInfo->getSize()/sizeof(char);
  }
  msg << MSG::DEBUG << "size for service "<< m_svcName << " " << tmpStringSize << endreq;
  m_StringSize = tmpStringSize;
  
  char* c;
  try {
    c = const_cast<char *>((const char*) m_dimInfo->getData());
    if ( 0 == m_noValidMonObject.compare(c)) {
      //msg << MSG::WARNING << "timeout trying to load  service "<< m_svcName << endreq;
      msg << MSG::DEBUG << m_noValidMonObject << ", for service "<< m_svcName << endreq;
      return false;
    }
  
    //msg << MSG::DEBUG << "creating stringstream " << endreq;
    std::stringstream is;
    //msg << MSG::DEBUG << "putsetbuf " << endreq;
    is.rdbuf()->pubsetbuf(c, m_StringSize);
    //msg << MSG::DEBUG << "binary_iarchive " << endreq;
    boost::archive::binary_iarchive ia(is);
    //msg << MSG::DEBUG << " loading MonObject "<< endreq;
    m_monObject->load(ia, 0);
    //msg << MSG::DEBUG << " setServiceActive "<< endreq;
    if (!m_monObject->serviceActive ()) m_monObject->setServiceActive (true);
  }
  catch (const std::exception &ex){
    msg << MSG::WARNING << "std::exception: " << ex.what() << endreq;
    msg << MSG::WARNING << "check that the server and client are running in the same plataform: 64 or 32 bits." << endreq;
    return false;
  }  
  catch (...){
    msg << MSG::WARNING << "unrecognized exception. "<< endreq;
    return false;
  } 
  return true;
    
}

MonObject *DimInfoMonObject::monObject() {
  return m_monObject;
}


