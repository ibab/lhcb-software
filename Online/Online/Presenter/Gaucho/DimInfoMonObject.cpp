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
  //used to be m_monObject(0)
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
 // if (m_monObject) { delete m_monObject; m_monObject = 0;}
  if (m_dimInfo) { delete m_dimInfo; m_dimInfo = 0; }
}

void DimInfoMonObject::infoHandler() 
{ // This is not inherited from DimInfo
  if (!m_monObject) createMonObject();
  else loadMonObject();
}

bool DimInfoMonObject::createMonObject() {
  MsgStream msg(msgSvc(), name());
  if (m_monObject) {delete m_monObject; m_monObject = 0;}
  
  if (!m_dimInfo) {
    std::cout << "svcName : " << m_svcName << " has no DimInfo"<< std::endl;
    return false;
  }
  int tmpStringSize = m_dimInfo->getSize()/sizeof(char);
  while ( tmpStringSize <= 0 ) {
    //gSystem->Sleep(m_waitTime);
    usleep(20);
    tmpStringSize = m_dimInfo->getSize()/sizeof(char);
   // msg << MSG::INFO << "createmonobject size for service "<< m_svcName << " " << tmpStringSize << endreq;
  }
  //msg << MSG::DEBUG << "size for service "<< m_svcName << " " << tmpStringSize << endreq;
  m_StringSize = tmpStringSize;
  MonObject *monObjectBase;
    
  char* c=NULL;
  try {
    c = const_cast<char *>((const char*) m_dimInfo->getData());
    if ( 0 == m_noValidMonObject.compare(c)) {
      std::cout << m_noValidMonObject << ", for service "<< m_svcName << std::endl;
      std::cout << "received:'" << c << "'" << std::endl;
      return false;
    }
    monObjectBase = new MonObject(m_msgSvc, m_source);
    //std::cout << " -- after new MonObject" << std::endl;
    std::stringstream is;
    is.rdbuf()->pubsetbuf(c, m_StringSize);
    //std::cout << " -- before binary_iarchive" << std::endl;
    boost::archive::binary_iarchive ia(is);
    //std::cout << " -- before load" << std::endl;
    monObjectBase->load(ia);
    //std::cout << " -- after load" << std::endl;
    std::string monObjectTypeName  = monObjectBase->typeName();
    m_monObject = MonObjectCreator::createMonObject(monObjectTypeName, m_msgSvc, m_source);
    //std::cout << "MonObjectCreator for type " << monObjectTypeName << " returned " << m_monObject << std::endl;
    std::stringstream is2;
    is2.rdbuf()->pubsetbuf(c, m_StringSize);
    boost::archive::binary_iarchive ia2(is2);
    m_monObject->load(ia2);
    if (monObjectBase) {delete monObjectBase; monObjectBase = 0;}
  }
  catch (const std::exception &ex){
    std::cout << "std::exception: " << ex.what() << std::endl;
    return false;
  }  
  catch (...){
    std::cout << "unrecognized exception. "<< std::endl;
    return false;
  } 
  
  return true;
}

bool DimInfoMonObject::loadMonObject(){
  //it crashes here
  MsgStream msg(msgSvc(), name());
  
  if (!m_dimInfo) {
    msg << MSG::DEBUG << "svcName : " << m_svcName << " has no DimInfo"<< endreq;
    return false;
  }
  
  if (!m_monObject) {
    msg << MSG::DEBUG << "svcName : " << m_svcName << " has an uncreated MonObject"<< endreq;
    return false;
  }
 // m_monObject->reset(); // <================VERIFICAR ISSO AQUI (ACHO QUE E' DESNECESARIO)


 // msg << MSG::DEBUG << "size for service "<< m_svcName << " " << tmpStringSize << endreq;

  
  char* c;
  try {
     int tmpStringSize = m_dimInfo->getSize()/sizeof(char);
    // 
       //gSystem->Sleep(m_waitTime);
     while ( tmpStringSize <= 0 ) {
       usleep(20);
       tmpStringSize = m_dimInfo->getSize()/sizeof(char);
   //    msg << MSG::INFO << "loadmonobject: size for service "<< m_svcName << " " << tmpStringSize << endreq;
     }
     if (tmpStringSize <=0) return false;
     m_StringSize = tmpStringSize;
   
  
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
    boost::archive::binary_iarchive ia(is);
  //  msg << MSG::INFO << " loading MonObject "<< endreq;
    if (m_monObject!=0) m_monObject->load(ia);
    else return false;
    //msg << MSG::DEBUG << " setServiceActive "<< endreq;
    if (!m_monObject->serviceActive ()) m_monObject->setServiceActive (true);
  }
  catch (const std::exception &ex){
    msg << MSG::DEBUG << "std::exception: " << ex.what() << endreq;
    msg << MSG::DEBUG << "DimInfo->getData() failed."<< endreq;
    return false;
  }  
  catch (...){
    msg << MSG::DEBUG << "unrecognized exception. "<< endreq;
    return false;
  } 
  return true;
    
}

void DimInfoMonObject::reset() {
  if (m_monObject) m_monObject->reset();
}


MonObject *DimInfoMonObject::monObject() {
  //this crashes 
  return m_monObject;
}


