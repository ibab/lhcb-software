// Include files
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "Gaucho/MonitorSvc.h"
#include "Gaucho/DimEngine.h"
#include "Gaucho/DimPropServer.h"
#include "Gaucho/DimCmdServer.h"

#ifdef WIN32
namespace wins {
#include <Winsock.h>
}
#endif
#include <sys/types.h>
#include <unistd.h>

namespace AIDA { class IHistogram; }

// Factory for instantiation of service objects
static SvcFactory<MonitorSvc> s_factory;
const  ISvcFactory& MonitorSvcFactory = s_factory;

// UNIQUE Interface identifiers defined elsewhere
//! need the next declaration ?  compiler seems to find the static var
//extern const InterfaceID IID_IPublish;

// Constructor
MonitorSvc::MonitorSvc(const std::string& name, ISvcLocator* sl)
  : Service(name, sl) {
}

//!destructor

//Query interfaces of Interface
// @param riid       ID of Interface to be retrieved
// @param ppvUnknown Pointer to Location for interface pointer
StatusCode MonitorSvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
  if(IID_IMonitorSvc == riid) {
    *ppvIF = dynamic_cast<IMonitorSvc*> (this);
  } 
  else {
    return Service::queryInterface(riid, ppvIF);
  }
  addRef();
  return StatusCode::SUCCESS;
}

// implement Service methods
StatusCode MonitorSvc::initialize() {
  MsgStream msg(msgSvc(),"MonitorSvc");
  if( IService::INITIALIZED == this->state() ) {
  msg << MSG::INFO << "MonitorSvc already initialized" << endreq;
  return StatusCode::SUCCESS; 
  }
  Service::initialize(); 
  m_nodename=new char[60];
#ifdef WIN32
  int errcode;
  errcode=wins::gethostname(m_nodename,60);
#else
  gethostname(m_nodename,60);
#endif
  //get hostname only (slc)
  tmp=new char[50];
  char * pch;
  pch = strtok(m_nodename,".");
  m_pid=new char[10];
  sprintf(m_pid,"%d",getpid() );
  
  msg << MSG::INFO << "initialize: Setting up DIM for Nodename: " << m_nodename << " Process ID: " << m_pid << endreq;
  
  char * name = new char[60];
  strcpy(name,m_nodename);
  strcat(name,"/");
  strcat(name,m_pid);
  
  msg << MSG::DEBUG << "DimEngine name: " << name <<  endreq;

  m_dimeng=new DimEngine(name,serviceLocator());
  msg << MSG::DEBUG << "DimEngine created with name " << name << endreq;
  m_dimpropsvr= new DimPropServer(name,serviceLocator());
  msg << MSG::DEBUG << "DimPropServer created with name " << name << endreq;
  
  strcpy(tmp,name);
  strcat(tmp,"/");
  m_dimcmdsvr = new DimCmdServer(tmp,serviceLocator());

  delete [] name;
  delete [] tmp;
  msg << MSG::DEBUG << "Initialized successfully " << endreq;
  return StatusCode::SUCCESS;
}


StatusCode MonitorSvc::finalize() {
  MsgStream msg(msgSvc(),"MonitorSvc");
  delete [] m_nodename;
  delete [] m_pid;

  m_InfoNamesMap.clear();
  m_infoDescriptions.clear();
  delete m_dimeng;
  delete m_dimcmdsvr;
  delete m_dimpropsvr;
  msg << MSG::INFO << "finalized successfully" << endreq;
  return StatusCode::SUCCESS;
}

void MonitorSvc::declareInfo(const std::string& name, const bool&  var, const std::string& desc, const IInterface* owner) {
  MsgStream msg(msgSvc(),"MonitorSvc");
  std::string dimName =infoOwnerName( owner )+"/"+name;
  msg << MSG::INFO << "Declaring info: Owner: " << infoOwnerName( owner ) << " Name: " << name << endreq;
  msg << MSG::DEBUG << "dimName: " << dimName << endreq;
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) (*m_InfoNamesMapIt).second.insert(dimName);
  else { // Create a new set for this algo
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(dimName);
  }
  m_dimeng->declSvc(dimName, var);
  m_infoDescriptions[dimName] = desc;
  m_dimeng->declSvc(dimName+"/gauchocomment", m_infoDescriptions[dimName] );

}
void MonitorSvc::declareInfo(const std::string& name, const int&  var, const std::string& desc, const IInterface* owner) {
  MsgStream msg(msgSvc(),"MonitorSvc");
  std::string dimName =infoOwnerName( owner )+"/"+name;
  msg << MSG::INFO << "Declaring info: Owner: " << infoOwnerName( owner ) << " Name: " << name << endreq;
  msg << MSG::DEBUG << "dimName: " << dimName << endreq;
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) (*m_InfoNamesMapIt).second.insert(dimName);
  else { // Create a new set for this algo
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(dimName);
  }
  m_dimeng->declSvc(dimName, var);
  m_infoDescriptions[dimName] = desc;
  m_dimeng->declSvc(dimName+"/gauchocomment", m_infoDescriptions[dimName] );

}
void MonitorSvc::declareInfo(const std::string& name, const long&  var, const std::string& desc, const IInterface* owner) {
  MsgStream msg(msgSvc(),"MonitorSvc");
  std::string dimName =infoOwnerName( owner )+"/"+name;
  msg << MSG::INFO << "Declaring info: Owner: " << infoOwnerName( owner ) << " Name: " << name << endreq;
  msg << MSG::DEBUG << "dimName: " << dimName << endreq;
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) (*m_InfoNamesMapIt).second.insert(dimName);
  else { // Create a new set for this algo
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(dimName);
  }
  m_dimeng->declSvc(dimName, var);
  m_infoDescriptions[dimName] = desc;
  m_dimeng->declSvc(dimName+"/gauchocomment", m_infoDescriptions[dimName] );

}
void MonitorSvc::declareInfo(const std::string& name, const double& var, const std::string& desc, const IInterface* owner) {
  MsgStream msg(msgSvc(),"MonitorSvc");
  std::string dimName =infoOwnerName( owner )+"/"+name;
  msg << MSG::INFO << "Declaring info: Owner: " << infoOwnerName( owner ) << " Name: " << name << endreq;
  msg << MSG::DEBUG << "dimName: " << dimName << endreq;
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) (*m_InfoNamesMapIt).second.insert(dimName);
  else { // Create a new set for this algo
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(dimName);
  }
  m_dimeng->declSvc(dimName, var);
  m_infoDescriptions[dimName] = desc;
  m_dimeng->declSvc(dimName+"/gauchocomment", m_infoDescriptions[dimName] );

}
void MonitorSvc::declareInfo(const std::string& name, const std::string& var, const std::string& desc, const IInterface* owner) {
  MsgStream msg(msgSvc(),"MonitorSvc");
  std::string dimName =infoOwnerName( owner )+"/"+name;
  msg << MSG::INFO << "Declaring info: Owner: " << infoOwnerName( owner ) << " Name: " << name << endreq;
  msg << MSG::DEBUG << "dimName: " << dimName << endreq;
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) (*m_InfoNamesMapIt).second.insert(dimName);
  else { // Create a new set for this algo
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(dimName);
  }
  m_dimeng->declSvc(dimName, var );
  m_infoDescriptions[dimName] = desc;
  m_dimeng->declSvc(dimName+"/gauchocomment", m_infoDescriptions[dimName] );
}
void MonitorSvc::declareInfo(const std::string& name, const std::pair<double,double>& var, const std::string& desc, const IInterface* owner) {
  MsgStream msg(msgSvc(),"MonitorSvc");
   msg << MSG::INFO << "declareInfo: No pair service in DIM for now." << endreq;
   return;
/*
  std::string dimName =infoOwnerName( owner )+"/"+name;
  msg << MSG::INFO << "Declaring info: Owner: " << infoOwnerName( owner ) << " Name: " << name << endreq;
  msg << MSG::DEBUG << "dimName: " << dimName << endreq;
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) (*m_InfoNamesMapIt).second.insert(dimName);
  else { // Create a new set for this algo
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(dimName);
  }
  m_dimeng->declSvc(dimName, var);
  m_infoDescriptions[dimName] = desc;
  m_dimeng->declSvc(dimName+"/gauchocomment", m_infoDescriptions[dimName] );

*/
}
void MonitorSvc::declareInfo(const std::string& name, const AIDA::IHistogram* var, const std::string& desc, const IInterface* owner) {
  MsgStream msg(msgSvc(),"MonitorSvc");
  std::string dimName =infoOwnerName( owner )+"/"+name;
  msg << MSG::INFO << "Declaring info: Owner: " << infoOwnerName( owner ) << " Name: " << name << endreq;
  msg << MSG::DEBUG << "dimName: " << dimName << endreq;
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) (*m_InfoNamesMapIt).second.insert(dimName);
  else { // Create a new set for this algo
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(dimName);
  }
  m_dimeng->declSvc(dimName, var);
  m_infoDescriptions[dimName] = desc;
  m_dimeng->declSvc(dimName+"/gauchocomment", m_infoDescriptions[dimName] );

}

void MonitorSvc::undeclareInfo( const std::string& name, const IInterface* owner ){
  MsgStream msg(msgSvc(),"MonitorSvc");
  std::set<std::string> * infoNamesSet = getInfos( owner );
  if( 0 == infoNamesSet ) {
    msg << MSG::WARNING << "undeclareInfo: Info  " << name << " not declared by " << infoOwnerName(owner) << ". Empty set" << endreq;
    return;
  }
  bool undeclared = false;
  std::set<std::string>::iterator infoNamesIt = (*infoNamesSet).begin();
  while( infoNamesIt!=(*infoNamesSet).end() ){
  // Look for the info name in string dimName sent to Dim. Ensure it is the last substring of dimName, not a casual occurence.
    std::string::size_type pos = (*infoNamesIt).find( "/"+name);
    if( pos != std::string::npos && 
       ( (*infoNamesIt).begin()+pos+name.size()+1) == (*infoNamesIt).end() ) {
      m_dimeng->undeclSvc( (*infoNamesIt) ) ;
      m_dimeng->undeclSvc( (*infoNamesIt)+"/gauchocomment" ) ;
      // Iterate before erasing in order to keep the iterator valid
      std::set<std::string>::iterator infoNameItToErase = infoNamesIt; 
      infoNamesIt++;
      (*infoNamesSet).erase(infoNameItToErase);
      msg << MSG::INFO << "undeclareInfo: " << name << " undeclared" << endreq;
      undeclared = true;
      continue;
    }
  infoNamesIt++;
  }
  if( !undeclared ) {
    msg << MSG::WARNING << "undeclareInfo: Info  " << name << " not declared by " << infoOwnerName(owner) << endreq;
    msg << MSG::DEBUG << infoOwnerName(owner) << " infoNames: " << endreq;
    for( infoNamesIt = (*infoNamesSet).begin();infoNamesIt!=(*infoNamesSet).end();++infoNamesIt)
      msg << MSG::DEBUG << "\t" <<  (*infoNamesIt) << endreq;
  }
}
void MonitorSvc::undeclareAll( const IInterface* owner ){
  MsgStream msg(msgSvc(),"MonitorSvc");
  if( 0!=owner ){
  std::string ownerName = infoOwnerName( owner );
  std::set<std::string> * infoNamesSet = getInfos( owner );
  if( 0 == infoNamesSet ) {
    msg << MSG::WARNING << "undeclareAll: No infos to  undeclare for " << ownerName << endreq;
    return;
  }
  std::set<std::string>::iterator infoNamesIt;
  for( infoNamesIt = (*infoNamesSet).begin();infoNamesIt!=(*infoNamesSet).end();++infoNamesIt){
    m_dimeng->undeclSvc( (*infoNamesIt) ) ;
    m_dimeng->undeclSvc( (*infoNamesIt)+"/gauchocomment" ) ;
    msg << MSG::INFO << "undeclareAll: Undeclared info " << (*infoNamesIt) << " from owner " << ownerName << endreq;
  }
  m_InfoNamesMap.erase(owner );
} else { // Null pointer. Undeclare for all owners
  for(m_InfoNamesMapIt = m_InfoNamesMap.begin();m_InfoNamesMapIt != m_InfoNamesMap.end();++m_InfoNamesMapIt)
    undeclareAll( m_InfoNamesMapIt->first );
  }
}
std::set<std::string> * MonitorSvc::getInfos(const IInterface* owner ){
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) return &(m_InfoNamesMapIt->second);
  else {
    return 0;
  }
}
std::string MonitorSvc::infoOwnerName( const IInterface* owner ){
  const IAlgorithm* ownerAlg = dynamic_cast<const IAlgorithm*>(owner);
  if( 0 != ownerAlg ) // It's an algorithm
    return ownerAlg->name();
  const IAlgTool* ownerTool = dynamic_cast<const IAlgTool*>(owner);
  if( 0 != ownerTool ) // It's a  tool
    return ownerTool->name();
  const IService* ownerSvc  = dynamic_cast<const IService*>(owner);
  if( 0 != ownerSvc ) // It's a service
    return ownerSvc->name();
  // NULL pointer or Unknown interface:
  return "";
}

