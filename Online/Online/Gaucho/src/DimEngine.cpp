// $Id: DimEngine.cpp,v 1.8 2007-01-16 16:33:44 evh Exp $

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"
#include "DimHisto.h"
#include "DimEngine.h"



DimEngine::DimEngine(std::string name, ISvcLocator* svclocator) : 
  GaudiAlgorithm(name,svclocator),
  m_dimName(name)
                                                                , m_svclocator(svclocator)
                                                                , m_dimInfos()
                                                                , m_dimInfosIt()
                                                                , m_dimHistos()
                                                                , m_dimHistosIt()
{}

DimEngine::~DimEngine() {
}

void DimEngine::declSvc(std::string InfoName, const bool& InfoVar){
  std::string diminfoname=m_dimName+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(),"C:1", 
                                         (void*)&InfoVar, sizeof(bool) );
    info()  << "New DimService (bool): " << diminfoname << endreq;
  }
  else {
    error() << "already existing DimService:" << (*(p.first)).first
            << " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::declSvc(std::string InfoName, const int& InfoVar){
  std::string diminfoname=m_dimName+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(),(int&)InfoVar);
    info()  << "New DimService (int): " << diminfoname << endreq;
  }
  else {
    error() << "already existing DimService:" << (*(p.first)).first
            << " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::declSvc(std::string InfoName, const long& InfoVar){
  // No DIM service for long. Cast to int.
  std::string diminfoname=m_dimName+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(),(int&)InfoVar);
    info()  << "New DimService (long): " << diminfoname << endreq;
    warning() << InfoName 
              << ": No DIM service for long. Casting to int" << endreq;
  }
  else {
    error() << "already existing DimService:" << (*(p.first)).first
            << " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::declSvc(std::string InfoName, const double& InfoVar){
  std::string diminfoname=m_dimName+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(),(double&)InfoVar);
    info()  << "New DimService (double): " << diminfoname << endreq;
  }
  else {
    error() << "already existing DimService:" << (*(p.first)).first
            << " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::declSvc(std::string InfoName, const char* InfoVar){
  std::string diminfoname=m_dimName+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(),(char*)InfoVar);
    info()  << "New DimService (char): " << diminfoname << endreq;
  }
  else {
    error() << "already existing DimService:" << (*(p.first)).first
            << " -> " << (*(p.first)).second << endreq;
  }
}
void DimEngine::declSvc(std::string InfoName, const std::string& InfoVar){
  // No DIM service for string. Cast to char.
  std::string diminfoname=m_dimName+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(),
                                         (char*)InfoVar.c_str() );
    info()  << "New DimService (string): " << diminfoname << endreq;
    warning() << InfoName 
              << ": No DimService for strings. Casting into char*" << endreq;
  }
  else {
    error() << "already existing DimService:" << (*(p.first)).first
            << " -> " << (*(p.first)).second << endreq;
  }
}

void DimEngine::declSvc(std::string InfoName, 
                        const std::pair<double,double>& InfoVar){
  // Publishing a pair of doubles (DimPair) as two DimServices for doubles.
  //  "/1" and "/2" added to name for first and second member, respectively.
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
    info()  << "New DimPair (pair of doubles): " 
            << diminfoname1 << "," << diminfoname2 << endreq;
    warning() << InfoName 
              << ": No DimService for pair: Publishing as two doubles" << endreq;
  }
  else {
    error() 
      << "DimPair(double): already existing DimService with name " 
      << InfoName <<  endreq;
    return;
  }
}

void DimEngine::declSvc(std::string InfoName, const AIDA::IBaseHistogram* InfoVar){
  std::string diminfoname;
  if( 0 != dynamic_cast<const AIDA::IProfile1D * >(InfoVar) ) 
    diminfoname="HPD/"+m_dimName+"/"+InfoName;
  else if( 0 != dynamic_cast<const AIDA::IHistogram1D * >(InfoVar) )
    diminfoname="H1D/"+m_dimName+"/"+InfoName;
  else if( 0 != dynamic_cast<const AIDA::IHistogram2D * >(InfoVar) )
    diminfoname="H2D/"+m_dimName+"/"+InfoName;
  else {
    error() << "Unknown histogram type: " << diminfoname  << endreq;
    return;
  }
  
  std::pair<DimHistoMapIt,bool> p = 
    m_dimHistos.insert(DimHistoMap::value_type(InfoName,0));
  if (p.second) {
    m_dimHistos[InfoName]=new DimHisto(diminfoname, InfoVar,m_svclocator);
    
    info()  << "New DimHisto: " << diminfoname << endreq;
    return;
  }
  else {
    error() << "already existing DimHisto: " << diminfoname << endreq;
    return;
  }
}

void DimEngine::declSvc(std::string InfoName, const std::string& format, const void* InfoVar, int size ) {
  std::string diminfoname=m_dimName+"/"+InfoName;
  std::pair<DimServiceMapIt,bool> p;
  p = m_dimInfos.insert(DimServiceMap::value_type(InfoName,0));
  if (p.second) {
    m_dimInfos[InfoName]= new DimService(diminfoname.c_str(), (char *)format.c_str(), (void *)InfoVar, size);
    info()  << "New DimService ( arbitrary structure ): " << diminfoname << endreq;
  }
  else {
    error() << "already existing DimService:" << (*(p.first)).first
            << " -> " << (*(p.first)).second << endreq;
  }
}

void DimEngine::undeclSvc(std::string InfoName){
  debug() << "undeclSvc: Contents of dimInfos: " << endreq;
  for (m_dimInfosIt=m_dimInfos.begin();m_dimInfosIt!=m_dimInfos.end();
       m_dimInfosIt++) debug() << (*m_dimInfosIt).first << " ";
  debug() << endreq;
  debug() << "undeclSvc: Contents of dimHistos: " << endreq;
  for (m_dimHistosIt=m_dimHistos.begin();
       m_dimHistosIt!=m_dimHistos.end();m_dimHistosIt++)
    debug() << (*m_dimHistosIt).first << " ";
  debug() << endreq;
  debug() << "undeclSvc: Contents of dimPairs: " << endreq;
  for (m_dimPairsIt=m_dimPairs.begin();
       m_dimPairsIt!=m_dimPairs.end();m_dimPairsIt++)
    debug() << (*m_dimPairsIt).first << " ";
  debug() << endreq;
  
  m_dimInfosIt = m_dimInfos.find(InfoName);
  if(m_dimInfosIt != m_dimInfos.end()) {
    delete (*m_dimInfosIt).second;
    m_dimInfos.erase(m_dimInfosIt);
    info()  << "undeclSvc: Service " << InfoName 
            << " undeclared" << endreq;
    return;
  }
  m_dimHistosIt = m_dimHistos.find(InfoName);
  if(m_dimHistosIt != m_dimHistos.end()) {
    delete (*m_dimHistosIt).second;
    m_dimHistos.erase(m_dimHistosIt);
    info()  << "undeclSvc: Service (DimHisto) " << InfoName 
            << " undeclared" << endreq;
    return;
  }
  m_dimPairsIt = m_dimPairs.find(InfoName);
  if(m_dimPairsIt != m_dimPairs.end()) {
    delete (*m_dimPairsIt).second.first;
    delete (*m_dimPairsIt).second.second;
    m_dimPairs.erase(m_dimPairsIt);
    info()  << "undeclSvc: Service (DimPair) " << InfoName 
            << " undeclared" << endreq;
    return;
  }
  warning() 
    << "undeclSvc: no DimSvc,DimHisto or pair found with the name:" 
    << InfoName << endreq;
}
