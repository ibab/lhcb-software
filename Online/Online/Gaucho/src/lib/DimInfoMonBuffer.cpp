#include "Gaucho/DimInfoMonBuffer.h"
#include "Gaucho/MonBuffer.h"
#include "Gaucho/MonBufferObject.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include <string>
#include <stdlib.h>
#include <malloc.h>

DimInfoMonBuffer::DimInfoMonBuffer(std::string svcName, int refreshTime):
  m_hasData(false),
  m_name("DimInfoMonBuffer"),
  m_svcName(svcName),
  m_source("UndefinedSource"),
  m_noValidMonBuffer("No valid MonBuffer"),
  m_monBuffer(0),
  m_msgSvc(0),
  m_StringSize(-1)
{
  m_dimInfo = new DimInfo((m_svcName).c_str(), refreshTime, (char*)m_noValidMonBuffer.c_str());
}

DimInfoMonBuffer::DimInfoMonBuffer(std::string svcName, int refreshTime, std::string source):
  m_hasData(false),
  m_name("DimInfoMonBuffer"),
  m_svcName(svcName),
  m_source(source),
  m_noValidMonBuffer("No valid MonBuffer"),
  m_monBuffer(0),
  m_msgSvc(0),
  m_StringSize(-1)
{
  //m_dimInfo = new DimInfo((m_svcName).c_str(), refreshTime, (float)-1.0);
  m_dimInfo = new DimInfo((m_svcName).c_str(), refreshTime, (char*)m_noValidMonBuffer.c_str());
}

DimInfoMonBuffer::~DimInfoMonBuffer() {
  if (m_monBuffer) { delete m_monBuffer; m_monBuffer = 0;}
  if (m_dimInfo) { delete m_dimInfo; m_dimInfo = 0; }
}

void DimInfoMonBuffer::infoHandler() 
{ // This is not inherited from DimInfo
  if (m_monBuffer==0) createMonBuffer();
  else loadMonBuffer();
}

bool DimInfoMonBuffer::createMonBuffer() {
  MsgStream msg(msgSvc(), name());
  if (m_monBuffer) {delete m_monBuffer; m_monBuffer = 0;}
  
  int tmpStringSize = m_dimInfo->getSize()/sizeof(char);
  while ( tmpStringSize <= 0 ) {
    //gSystem->Sleep(m_waitTime);
    usleep(10);
    tmpStringSize = m_dimInfo->getSize()/sizeof(char);
    msg << MSG::DEBUG << "size for service "<< m_svcName << " " << tmpStringSize << endreq;
  }
  msg << MSG::DEBUG << "size for service "<< m_svcName << " " << tmpStringSize << endreq;
  m_StringSize = tmpStringSize;
  
  char* c;
  try {
    c = const_cast<char *>((const char*) m_dimInfo->getData());
    if ( 0 == m_noValidMonBuffer.compare(c)) {
      //msg << MSG::WARNING << "timeout trying to create  service "<< m_svcName << endreq;
      msg << MSG::WARNING << m_noValidMonBuffer << ", for service "<< m_svcName << endreq;
      return false;
    }
    //msg << MSG::DEBUG << "creating MonBuffer base" << endreq;
    m_monBuffer = new MonBufferObject(m_msgSvc, m_source);
    //msg << MSG::DEBUG << "creating TBufferFile" << endreq;
    TBufferFile *bufferFile = new TBufferFile(TBuffer::kRead, m_StringSize, c,kFALSE);
    //msg << MSG::DEBUG << "load MonBufferObject" << endreq;
    m_monBuffer->load(bufferFile, m_monBuffer->version());
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

bool DimInfoMonBuffer::loadMonBuffer(){
  MsgStream msg(msgSvc(), name());

  if (!m_monBuffer) {
    msg << MSG::WARNING << "svcName : " << m_svcName << " has an uncreated MonBuffer"<< endreq;
    return false;
  }
  m_monBuffer->reset(); 

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
    if ( 0 == m_noValidMonBuffer.compare(c)) {
      //msg << MSG::WARNING << "timeout trying to load  service "<< m_svcName << endreq;
      msg << MSG::WARNING << m_noValidMonBuffer << ", for service "<< m_svcName << endreq;
      return false;
    }
  
    TBufferFile *bufferFile = new TBufferFile(TBuffer::kRead, m_StringSize, c, kFALSE);
    //msg << MSG::DEBUG << "load MonBufferObject" << endreq;
    m_monBuffer->load(bufferFile, m_monBuffer->version());
    //msg << MSG::DEBUG << " setServiceActive "<< endreq;
    if (!m_monBuffer->serviceActive ()) m_monBuffer->setServiceActive (true);
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

MonBuffer *DimInfoMonBuffer::monBuffer() {
  return m_monBuffer;
}
