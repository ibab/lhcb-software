// $Id: DimEngine.cpp,v 1.6 2005-06-15 15:02:30 cattanem Exp $

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "DimH1D.h"
#include "DimH2D.h"
#include "DimEngine.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"



DimEngine::DimEngine(std::string name, ISvcLocator* svclocator) : 
  m_dimName(name)
, m_dimInfos()
, m_dimInfosIt()
, m_dim1DHistos()
, m_dim1DHistosIt()
, m_dim2DHistos()
, m_dim2DHistosIt() {
  // get msg logging
  StatusCode sc;
  sc = svclocator->service("MessageSvc", m_msgsvc);
  MsgStream msg(m_msgsvc,"DimEngine");
}

DimEngine::~DimEngine() {
}

void DimEngine::declSvc(std::string InfoName, const bool& InfoVar){
  MsgStream msg(m_msgsvc,"DimEngine");
  std::string diminfoname=m_dimName+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(),"C:1", 
                                         (void*)&InfoVar, sizeof(bool) );
    msg << MSG::INFO << "New DimService (bool): " << diminfoname << endreq;
  }
  else {
    msg << MSG::ERROR << "already existing DimService:" << (*(p.first)).first
        << " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::declSvc(std::string InfoName, const int& InfoVar){
  MsgStream msg(m_msgsvc,"DimEngine");
  std::string diminfoname=m_dimName+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
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
  std::string diminfoname=m_dimName+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(),(int&)InfoVar);
    msg << MSG::INFO << "New DimService (long): " << diminfoname << endreq;
    msg << MSG::WARNING << InfoName 
        << ": No DIM service for long. Casting to int" << endreq;
  }
  else {
    msg << MSG::ERROR << "already existing DimService:" << (*(p.first)).first
        << " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::declSvc(std::string InfoName, const double& InfoVar){
  MsgStream msg(m_msgsvc,"DimEngine");
  std::string diminfoname=m_dimName+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
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
  std::string diminfoname=m_dimName+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
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
  std::string diminfoname=m_dimName+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(),
                                         (char*)InfoVar.c_str() );
    msg << MSG::INFO << "New DimService (string): " << diminfoname << endreq;
    msg << MSG::WARNING << InfoName 
        << ": No DimService for strings. Casting into char*" << endreq;
  }
  else {
    msg << MSG::ERROR << "already existing DimService:" << (*(p.first)).first
        << " -> " << (*(p.first)).second << endreq;
  }
}

void DimEngine::declSvc(std::string InfoName, 
                        const std::pair<double,double>& InfoVar){
  // Publishing a pair of doubles (DimPair) as two DimServices for doubles.
  //  "/1" and "/2" added to name for first and second member, respectively.
  MsgStream msg(m_msgsvc,"DimEngine");
  std::string diminfoname1="DimPair/"+m_dimName+"/"+InfoName+"/1";
  std::string diminfoname2="DimPair/"+m_dimName+"/"+InfoName+"/2";
  std::pair<DimPairMapIt,bool> p;
  p = m_dimPairs.insert(DimPairMap::value_type(InfoName,std::pair<DimService*,
                                               DimService*>(0,0) ));
  if (p.second) {
    m_dimPairs[InfoName].first = new DimService(diminfoname1.c_str(),
                                                (double&)InfoVar.first);
    m_dimPairs[InfoName].second= new DimService(diminfoname2.c_str(),
                                                (double&)InfoVar.second);
    msg << MSG::INFO << "New DimPair (pair of doubles): " 
        << diminfoname1 << "," << diminfoname2 << endreq;
    msg << MSG::WARNING << InfoName 
        << ": No DimService for pair: Publishing as two doubles" << endreq;
  }
  else {
    msg << MSG::ERROR 
        << "DimPair(double): already existing DimService with name " 
        << InfoName <<  endreq;
    return;
  }
}

void DimEngine::declSvc(std::string InfoName, const AIDA::IHistogram* InfoVar){
  MsgStream msg(m_msgsvc,"DimEngine");
  std::string diminfoname=m_dimName+"/"+InfoName;
  int dimension = InfoVar->dimension();
  msg << MSG::DEBUG << "AIDA::IHistogram: Dimensao: " << dimension << endreq;
  if( 1 == dimension ) {
  	std::pair<DimH1DMapIt,bool> p = 
      m_dim1DHistos.insert(DimH1DMap::value_type(InfoName,0));
    if (p.second) {
      m_dim1DHistos[InfoName]=
        new DimH1D(diminfoname,
                   const_cast<AIDA::IHistogram1D*>
                   (dynamic_cast<const AIDA::IHistogram1D*> (InfoVar)));
      msg << MSG::INFO << "New DimH1D: " << diminfoname << endreq;
      return;
    }
    else {
      msg << MSG::ERROR << "already existing DimH1D: " 
          << diminfoname << endreq;
      return;
    }
  }
  if( 2 == dimension ) {
  	std::pair<DimH2DMapIt,bool> p = 
      m_dim2DHistos.insert(DimH2DMap::value_type(InfoName,0));
  	if (p.second) {
      m_dim2DHistos[InfoName]=
        new DimH2D(diminfoname,
                   const_cast<AIDA::IHistogram2D*>
                   (dynamic_cast<const  AIDA::IHistogram2D*> (InfoVar)));
      msg << MSG::INFO << "New DimH2D: " << diminfoname << endreq;
      return;
   	}
    else {
   		msg << MSG::ERROR << "already existing DimH2D: " 
          << diminfoname << endreq;
   		return;
   	}  
  }
  msg << MSG::ERROR << "Unknown histogram type: " << diminfoname 
      << " dimension: " << dimension << endreq;
}
void DimEngine::undeclSvc(std::string InfoName){
  MsgStream msg(m_msgsvc,"DimEngine");
  msg << MSG::DEBUG << "undeclSvc: Contents of dimInfos: " << endreq;
  for (m_dimInfosIt=m_dimInfos.begin();m_dimInfosIt!=m_dimInfos.end();
       m_dimInfosIt++) msg << MSG::DEBUG << (*m_dimInfosIt).first << " ";
  msg << MSG::DEBUG << endreq;
  msg << MSG::DEBUG << "undeclSvc: Contents of dim1DHistos: " << endreq;
  for (m_dim1DHistosIt=m_dim1DHistos.begin();
       m_dim1DHistosIt!=m_dim1DHistos.end();m_dim1DHistosIt++)
    msg << MSG::DEBUG << (*m_dim1DHistosIt).first << " ";
  msg << MSG::DEBUG << endreq;
  msg << MSG::DEBUG << "undeclSvc: Contents of dim2DHistos: " << endreq;
  for (m_dim2DHistosIt=m_dim2DHistos.begin();
       m_dim2DHistosIt!=m_dim2DHistos.end();m_dim2DHistosIt++)
    msg << MSG::DEBUG << (*m_dim2DHistosIt).first << " ";
  msg << MSG::DEBUG << endreq;
  msg << MSG::DEBUG << "undeclSvc: Contents of dimPairs: " << endreq;
  for (m_dimPairsIt=m_dimPairs.begin();
       m_dimPairsIt!=m_dimPairs.end();m_dimPairsIt++)
    msg << MSG::DEBUG << (*m_dimPairsIt).first << " ";
  msg << MSG::DEBUG << endreq;
  
  m_dimInfosIt = m_dimInfos.find(InfoName);
  if(m_dimInfosIt != m_dimInfos.end()) {
    delete (*m_dimInfosIt).second;
    m_dimInfos.erase(m_dimInfosIt);
    msg << MSG::INFO << "undeclSvc: Service " << InfoName 
        << " undeclared" << endreq;
    return;
  }
  m_dim1DHistosIt = m_dim1DHistos.find(InfoName);
  if(m_dim1DHistosIt != m_dim1DHistos.end()) {
    delete (*m_dim1DHistosIt).second;
    m_dim1DHistos.erase(m_dim1DHistosIt);
    msg << MSG::INFO << "undeclSvc: Service (DimH1D) " << InfoName 
        << " undeclared" << endreq;
    return;
  }
  m_dim2DHistosIt = m_dim2DHistos.find(InfoName);
  if(m_dim2DHistosIt != m_dim2DHistos.end()) {
    delete (*m_dim2DHistosIt).second;
    m_dim2DHistos.erase(m_dim2DHistosIt);
    msg << MSG::INFO << "undeclSvc: Service (DimH2D) " << InfoName 
        << " undeclared" << endreq;
    return;
  }
  m_dimPairsIt = m_dimPairs.find(InfoName);
  if(m_dimPairsIt != m_dimPairs.end()) {
    delete (*m_dimPairsIt).second.first;
    delete (*m_dimPairsIt).second.second;
    m_dimPairs.erase(m_dimPairsIt);
    msg << MSG::INFO << "undeclSvc: Service (DimPair) " << InfoName 
        << " undeclared" << endreq;
    return;
  }
  msg << MSG::WARNING 
      << "undeclSvc: no DimSvc,DimH1D, DimH2D or pair found with the name:" 
      << InfoName << endreq;
}
