//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/src/DimEngine.cpp,v 1.4 2005-03-22 16:39:17 evh Exp $

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "Gaucho/DimH1D.h"
#include "Gaucho/DimEngine.h"
#include "AIDA/IHistogram1D.h"



DimEngine::DimEngine(char* nodename, ISvcLocator* svclocator) : m_nodename(nodename),
                                                                m_dimInfos() , m_dimInfosIt(), m_dimHistos(), m_dimHistosIt() {
  // get msg logging
  StatusCode sc;
  sc = svclocator->service("MessageSvc", m_msgsvc);
  MsgStream msg(m_msgsvc,"DimEngine");
}

DimEngine::~DimEngine() {
}

void DimEngine::declSvc(std::string InfoName, const bool& InfoVar){
  MsgStream msg(m_msgsvc,"DimEngine");
  std::string diminfoname=m_nodename+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  m_dimInfosIt = m_dimInfos.end();
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(),"C:1", (void
*)&InfoVar, sizeof(bool) );
    msg << MSG::INFO << "New DimService (bool): " << diminfoname << endreq;
  }
  else {
    msg << MSG::ERROR << "already existing DimService:" << (*(p.first)).first
        << " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::declSvc(std::string InfoName, const int& InfoVar){
  MsgStream msg(m_msgsvc,"DimEngine");
  std::string diminfoname=m_nodename+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  m_dimInfosIt = m_dimInfos.end();
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(),(int&)InfoVar);
    msg << MSG::INFO << "New DimService (int): " << diminfoname << endreq;
  }
  else {
    msg << MSG::ERROR << "already existing DimService:" << (*(p.first)).first
        << " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::declSvc(std::string InfoName, const long& InfoVar){
// No DIM service for long. Cast to int.
  MsgStream msg(m_msgsvc,"DimEngine");
  std::string diminfoname=m_nodename+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  m_dimInfosIt = m_dimInfos.end();
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(),(int&)InfoVar);
    msg << MSG::INFO << "New DimService (long): " << diminfoname << endreq;
  }
  else {
    msg << MSG::ERROR << "already existing DimService:" << (*(p.first)).first
        << " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::declSvc(std::string InfoName, const double& InfoVar){
  MsgStream msg(m_msgsvc,"DimEngine");
  std::string diminfoname=m_nodename+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  m_dimInfosIt = m_dimInfos.end();
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(),(double&)InfoVar);
    msg << MSG::INFO << "New DimService (double): " << diminfoname << endreq;
  }
  else {
    msg << MSG::ERROR << "already existing DimService:" << (*(p.first)).first
        << " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::declSvc(std::string InfoName, const char* InfoVar){
  MsgStream msg(m_msgsvc,"DimEngine");
  std::string diminfoname=m_nodename+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  m_dimInfosIt = m_dimInfos.end();
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(),(char*)InfoVar);
    msg << MSG::INFO << "New DimService (char): " << diminfoname << endreq;
  }
  else {
    msg << MSG::ERROR << "already existing DimService:" << (*(p.first)).first
        << " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::declSvc(std::string InfoName, const std::string& InfoVar){
// No DIM service for string. Cast to char.
  MsgStream msg(m_msgsvc,"DimEngine");
  std::string diminfoname=m_nodename+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  m_dimInfosIt = m_dimInfos.end();
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(),(char*)InfoVar.c_str() );
    msg << MSG::INFO << "New DimService (string): " << diminfoname << endreq;
  }
  else {
    msg << MSG::ERROR << "already existing DimService:" << (*(p.first)).first
        << " -> " << (*(p.first)).second << endreq;
  }
}
/*
void DimEngine::declSvc(std::string InfoName, const std::pair<double,double>& InfoVar){
  MsgStream msg(m_msgsvc,"DimEngine");
  std::string diminfoname=m_nodename+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  m_dimInfosIt = m_dimInfos.end();
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(),"D:1",(void *)&InfoVar,sizeof(std::pair));
    msg << MSG::INFO << "New DimService (pair): " << diminfoname << endreq;
  }
  else {
    msg << MSG::ERROR << "already existing DimService:" << (*(p.first)).first
        << " -> " << (*(p.first)).second << endreq;
  }
}
*/
void DimEngine::declSvc(std::string InfoName, const AIDA::IHistogram* InfoVar){
  MsgStream msg(m_msgsvc,"DimEngine");
  std::string diminfoname=m_nodename+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  m_dimInfosIt = m_dimInfos.end();
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
   // AIDA::IHistogram1D* new_h = dynamic_cast<AIDA::IHistogram1D*> (InfoVar);
    m_dimHistos[InfoName]=new DimH1D(diminfoname.c_str(),
    const_cast<AIDA::IHistogram1D*>(dynamic_cast<const  AIDA::IHistogram1D*> (InfoVar)));
    msg << MSG::INFO << "New DimH1D: " << diminfoname << endreq;
  }
  else {
    msg << MSG::ERROR << "already existing DimH1D:" << (*(p.first)).first
        << " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::undeclSvc(std::string InfoName){
  MsgStream msg(m_msgsvc,"DimEngine");
  m_dimInfosIt = m_dimInfos.find(InfoName);
  if(m_dimInfosIt != m_dimInfos.end()) {
    delete (*m_dimInfosIt).second;
    m_dimInfos.erase(m_dimInfosIt);
    msg << MSG::INFO << "Service " << InfoName << " undeclared" << endreq;
  }
  else {
    m_dimHistosIt = m_dimHistos.find(InfoName);
    if(m_dimHistosIt != m_dimHistos.end()) {
      delete (*m_dimHistosIt).second;
      m_dimHistos.erase(m_dimHistosIt);
    msg << MSG::INFO << "Service (H1D) " << InfoName << " undeclared" << endreq;
    }
    else {
      msg << MSG::WARNING << "undeclare: no DimSvc or DimH1D found with the name:" 
          << InfoName << endreq;
    }
  }
}
