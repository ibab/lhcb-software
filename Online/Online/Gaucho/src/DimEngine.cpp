//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/src/DimEngine.cpp,v 1.3 2004-05-26 18:39:41 vanphil Exp $

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "Gaucho/DimH1D.h"
#include "Gaucho/DimEngine.h"


DimEngine::DimEngine(char* nodename, ISvcLocator* svclocator) : m_nodename(nodename),
  m_dimInfos() , m_dimInfosIt(), m_dimHistos(), m_dimHistosIt() {
// get msg logging
  StatusCode sc;
  sc = svclocator->service("MessageSvc", m_msgsvc);
}

DimEngine::~DimEngine() {
}

void DimEngine::declSvc(std::string InfoName, int& InfoVar){
//! hardcoded string size limits
  char* diminfoname=new char[100];
  strcpy(diminfoname,m_nodename);
  strcat(diminfoname,InfoName.c_str());
  std::pair<DimServiceMapIt,bool> p;
  m_dimInfosIt = m_dimInfos.end();
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname,InfoVar);
  }
  else {
    MsgStream log(m_msgsvc,"DimEngine");
    log << MSG::ERROR << "already existing DimService:" << (*(p.first)).first
		<< " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::declSvc(std::string InfoName, float& InfoVar){
  char* diminfoname=new char[100];
  strcpy(diminfoname,m_nodename);
  strcat(diminfoname,InfoName.c_str());
  std::pair<DimServiceMapIt,bool> p;
  m_dimInfosIt = m_dimInfos.end();
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname,InfoVar);
  }
  else {
    MsgStream log(m_msgsvc,"DimEngine");
	log << MSG::ERROR << "already existing DimService:" << (*(p.first)).first
		<< " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::declSvc(std::string InfoName, char* InfoVar){
  char* diminfoname=new char[100];
  strcpy(diminfoname,m_nodename);
  strcat(diminfoname,InfoName.c_str());
  std::pair<DimServiceMapIt,bool> p;
  m_dimInfosIt = m_dimInfos.end();
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname,InfoVar);
  }
  else {
    MsgStream log(m_msgsvc,"DimEngine");
	log << MSG::ERROR << "already existing DimService:" << (*(p.first)).first
		<< " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::declSvc(std::string InfoName, AIDA::IHistogram1D* InfoVar){
  char* diminfoname=new char[100];
  strcpy(diminfoname,m_nodename);
  strcat(diminfoname,InfoName.c_str());
  std::pair<DimServiceMapIt,bool> p;
  m_dimInfosIt = m_dimInfos.end();
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimHistos[InfoName]=new DimH1D(diminfoname,InfoVar);
  }
  else {
    MsgStream log(m_msgsvc,"DimEngine");
	log << MSG::ERROR << "already existing DimH1D:" << (*(p.first)).first
		<< " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::undeclSvc(std::string InfoName){
  m_dimInfosIt = m_dimInfos.find(InfoName);
  if(m_dimInfosIt != m_dimInfos.end()) {
	delete (*m_dimInfosIt).second;
    m_dimInfos.erase(m_dimInfosIt);
  }
  else {
    m_dimHistosIt = m_dimHistos.find(InfoName);
    if(m_dimHistosIt != m_dimHistos.end()) {
	  delete (*m_dimHistosIt).second;
      m_dimHistos.erase(m_dimHistosIt);
	}
	else {
    MsgStream log(m_msgsvc,"DimEngine");
	  log << MSG::ERROR << "undeclare: no DimSvc or DimH1D found with the name:" 
		<< InfoName << endreq;
	}
  }
}
