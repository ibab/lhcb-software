// Include files
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "MonitorSvc.h"
#include "DimEngine.h"
#include "DimPropServer.h"
#include "DimCmdServer.h"
#include "RTL/rtl.h"

namespace AIDA { class IBaseHistogram; }

// Factory for instantiation of service objects
DECLARE_SERVICE_FACTORY(MonitorSvc)

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
  const std::string& utgid = RTL::processName();
  
  msg << MSG::INFO << "initialize: Setting up DIM for UTGID " 
      << utgid << endreq;
  
  m_dimeng=new DimEngine(utgid , serviceLocator());
  msg << MSG::DEBUG << "DimEngine created with name " 
      << utgid  << endreq;
  m_dimpropsvr= new DimPropServer(utgid ,serviceLocator());
  msg << MSG::DEBUG << "DimPropServer created with name " << utgid << endreq;
  
  m_dimcmdsvr = new DimCmdServer( (utgid+"/"), serviceLocator());
  msg << MSG::DEBUG << "DimCmdServer created with name " 
      << (utgid+"/") << endreq;
  
  msg << MSG::DEBUG << "Initialized successfully " << endreq;
  return StatusCode::SUCCESS;
}


StatusCode MonitorSvc::finalize() {
  MsgStream msg(msgSvc(),"MonitorSvc");
  
  m_InfoNamesMap.clear();
  m_infoDescriptions.clear();
  delete m_dimeng;
  delete m_dimcmdsvr;
  delete m_dimpropsvr;
  msg << MSG::INFO << "finalized successfully" << endreq;
  return StatusCode::SUCCESS;
}

void MonitorSvc::declareInfo(const std::string& name, const bool&  var, 
                             const std::string& desc, const IInterface* owner) 
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  std::string ownerName = infoOwnerName( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) {
  	std::pair<std::set<std::string>::iterator,bool> p = 
      (*m_InfoNamesMapIt).second.insert(name);
	  if( p.second) {
      msg << MSG::INFO << endreq;
      msg << MSG::INFO << "Declaring info: Owner: " 
          << infoOwnerName( owner ) << " Name: " << name << endreq;
	  }
	  else { // Insertion failed: Name already exists
      msg << MSG::INFO << endreq;
	  	msg << MSG::ERROR << "Already existing info " << name << " from owner " 
          << infoOwnerName( owner ) << " not published" << endreq;
      return;
    }
  }
  else { // Create a new set for this algo and insert name
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(name);
  }
  std::string dimName =ownerName+"/"+name;
  msg << MSG::DEBUG << "dimName: " << dimName << endreq;
  m_dimeng->declSvc(dimName, var );
  m_infoDescriptions[dimName] = desc;
  m_dimeng->declSvc( dimName+"/gauchocomment", 
                     m_infoDescriptions[dimName].c_str() );
  
}
void MonitorSvc::declareInfo(const std::string& name, const int&  var, 
                             const std::string& desc, const IInterface* owner) 
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  std::string ownerName = infoOwnerName( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) {
  	std::pair<std::set<std::string>::iterator,bool> p = 
      (*m_InfoNamesMapIt).second.insert(name);
	  if( p.second) {
      msg << MSG::INFO << endreq;
      msg << MSG::INFO << "Declaring info: Owner: " 
          << infoOwnerName( owner ) << " Name: " << name << endreq;
	  }
	  else { // Insertion failed: Name already exists
      msg << MSG::INFO << endreq;
	  	msg << MSG::ERROR << "Already existing info " << name << " from owner " 
          << infoOwnerName( owner ) << " not published" << endreq;
      return;
    }
  }
  else { // Create a new set for this algo and insert name
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(name);
  }
  std::string dimName =ownerName+"/"+name;
  msg << MSG::DEBUG << "dimName: " << dimName << endreq;
  m_dimeng->declSvc(dimName, var );
  m_infoDescriptions[dimName] = desc;
  m_dimeng->declSvc( dimName+"/gauchocomment", 
                     m_infoDescriptions[dimName].c_str() );
}
void MonitorSvc::declareInfo(const std::string& name, const long&  var, 
                             const std::string& desc, const IInterface* owner) 
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  std::string ownerName = infoOwnerName( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) {
  	std::pair<std::set<std::string>::iterator,bool> p = 
      (*m_InfoNamesMapIt).second.insert(name);
	  if( p.second) {
      msg << MSG::INFO << endreq;
      msg << MSG::INFO << "Declaring info: Owner: " 
          << infoOwnerName( owner ) << " Name: " << name << endreq;
	  }
	  else { // Insertion failed: Name already exists
      msg << MSG::INFO << endreq;
	  	msg << MSG::ERROR << "Already existing info " << name << " from owner " 
          << infoOwnerName( owner ) << " not published" << endreq;
      return;
    }
  }
  else { // Create a new set for this algo and insert name
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(name);
  }
  std::string dimName =ownerName+"/"+name;
  msg << MSG::DEBUG << "dimName: " << dimName << endreq;
  m_dimeng->declSvc(dimName, var );
  m_infoDescriptions[dimName] = desc;
  m_dimeng->declSvc( dimName+"/gauchocomment", 
                     m_infoDescriptions[dimName].c_str() );
}
void MonitorSvc::declareInfo(const std::string& name, const double& var, 
                             const std::string& desc, const IInterface* owner) 
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  std::string ownerName = infoOwnerName( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) {
  	std::pair<std::set<std::string>::iterator,bool> p = 
      (*m_InfoNamesMapIt).second.insert(name);
	  if( p.second) {
      msg << MSG::INFO << endreq;
      msg << MSG::INFO << "Declaring info: Owner: " 
          << infoOwnerName( owner ) << " Name: " << name << endreq;
	  }
	  else { // Insertion failed: Name already exists
      msg << MSG::INFO << endreq;
	  	msg << MSG::ERROR << "Already existing info " << name << " from owner " 
          << infoOwnerName( owner ) << " not published" << endreq;
      return;
    }
  }
  else { // Create a new set for this algo and insert name
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(name);
  }
  std::string dimName =ownerName+"/"+name;
  msg << MSG::DEBUG << "dimName: " << dimName << endreq;
  m_dimeng->declSvc(dimName, var );
  m_infoDescriptions[dimName] = desc;
  m_dimeng->declSvc( dimName+"/gauchocomment", 
                     m_infoDescriptions[dimName].c_str() );
}
void MonitorSvc::declareInfo(const std::string& name, const std::string& var, 
                             const std::string& desc, const IInterface* owner) 
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  std::string ownerName = infoOwnerName( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) {
  	std::pair<std::set<std::string>::iterator,bool> p = 
      (*m_InfoNamesMapIt).second.insert(name);
	  if( p.second) {
      msg << MSG::INFO << endreq;
      msg << MSG::INFO << "Declaring info: Owner: " 
          << infoOwnerName( owner ) << " Name: " << name << endreq;
	  }
	  else { // Insertion failed: Name already exists
      msg << MSG::INFO << endreq;
	  	msg << MSG::ERROR << "Already existing info " << name << " from owner " 
          << infoOwnerName( owner ) << " not published" << endreq;
      return;
    }
  }
  else { // Create a new set for this algo and insert name
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(name);
  }
  std::string dimName =ownerName+"/"+name;
  msg << MSG::DEBUG << "dimName: " << dimName << endreq;
  m_dimeng->declSvc(dimName, var );
  m_infoDescriptions[dimName] = desc;
  m_dimeng->declSvc( dimName+"/gauchocomment", 
                     m_infoDescriptions[dimName].c_str() );
}
void MonitorSvc::declareInfo(const std::string& name, 
                             const std::pair<double,double>& var, 
                             const std::string& desc, const IInterface* owner) 
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  std::string ownerName = infoOwnerName( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) {
    std::pair<std::set<std::string>::iterator,bool> p = 
      (*m_InfoNamesMapIt).second.insert(name);
    if( p.second) {
      msg << MSG::INFO << endreq;
      msg << MSG::INFO << "Declaring info: Owner: " 
          << infoOwnerName( owner ) << " Name: " << name << endreq;
    }
    else { // Insertion failed: Name already exists
      msg << MSG::INFO << endreq;
      msg << MSG::ERROR << "Already existing info " << name << " from owner "
          << infoOwnerName( owner ) << " not published" << endreq;
      return;
    }
  }
  else { // Create a new set for this algo and insert name
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(name);
  }
  std::string dimName =ownerName+"/"+name;
  msg << MSG::DEBUG << "dimName: " << dimName << endreq;
  m_dimeng->declSvc(dimName, var );
  m_infoDescriptions[dimName] = desc;
  m_dimeng->declSvc( dimName+"/gauchocomment", 
                     m_infoDescriptions[dimName].c_str() );
}
void MonitorSvc::declareInfo(const std::string& name, 
                             const AIDA::IBaseHistogram* var, 
                             const std::string& desc, const IInterface* owner) 
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  std::string ownerName = infoOwnerName( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) {
  	std::pair<std::set<std::string>::iterator,bool> p = 
      (*m_InfoNamesMapIt).second.insert(name);
	  if( p.second) {
      msg << MSG::INFO << endreq;
      msg << MSG::INFO << "Declaring info: Owner: " 
          << infoOwnerName( owner ) << " Name: " << name << endreq;
	  }
	  else { // Insertion failed: Name already exists
      msg << MSG::INFO << endreq;
	  	msg << MSG::ERROR << "Already existing info " << name << " from owner " 
          << infoOwnerName( owner ) << " not published" << endreq;
      return;
    }
  }
  else { // Create a new set for this algo and insert name
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(name);
  }
  std::string dimName =ownerName+"/"+name;
  msg << MSG::DEBUG << "dimName: " << dimName << endreq;
  m_dimeng->declSvc(dimName, var );
  m_infoDescriptions[dimName] = desc;
  m_dimeng->declSvc( dimName+"/gauchocomment", 
                     m_infoDescriptions[dimName].c_str() );
}

void MonitorSvc::declareInfo(const std::string& name, const std::string& format, const void * var,
                             int size, const std::string& desc, const IInterface* owner) 
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  std::string ownerName = infoOwnerName( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) {
    std::pair<std::set<std::string>::iterator,bool> p =
      (*m_InfoNamesMapIt).second.insert(name);
    if( p.second) {
      msg << MSG::INFO << endreq;
      msg << MSG::INFO << "Declaring info: Owner: "
          << infoOwnerName( owner ) << " Name: " << name << endreq;
    }
    else { // Insertion failed: Name already exists
      msg << MSG::INFO << endreq;
      msg << MSG::ERROR << "Already existing info " << name << " from owner "
          << infoOwnerName( owner ) << " not published" << endreq;
      return;
    }
  }
  else { // Create a new set for this algo and insert name
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(name);
  }
  std::string dimName =ownerName+"/"+name;
  msg << MSG::DEBUG << "dimName: " << dimName << endreq;
  m_dimeng->declSvc(dimName, format, var, size );
  m_infoDescriptions[dimName] = desc;
  m_dimeng->declSvc( dimName+"/gauchocomment",
                     m_infoDescriptions[dimName].c_str() );
}

void MonitorSvc::undeclareInfo( const std::string& name, 
                                const IInterface* owner )
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  std::set<std::string> * infoNamesSet = getInfos( owner );
  if( 0 == infoNamesSet ) {
    msg << MSG::WARNING << "undeclareInfo: Info  " << name 
        << ": No info to undeclare for " << infoOwnerName(owner) 
        << ". Empty set" << endreq;
    return;
  }
  std::string ownerName = infoOwnerName( owner );
  if( (*infoNamesSet).find( name) !=  (*infoNamesSet).end() ){
    m_dimeng->undeclSvc( ownerName+"/"+name ) ;
    m_dimeng->undeclSvc( ownerName+"/"+name+"/gauchocomment" );
    (*infoNamesSet).erase(name);
    msg << MSG::INFO << "undeclareInfo: " << name << " from owner " 
        << ownerName  << " undeclared" << endreq;
  }  
  else{
    msg << MSG::WARNING << "undeclareInfo: Info  " << name << " declared by " 
        << infoOwnerName(owner) << " not found" << endreq;
    msg << MSG::DEBUG << infoOwnerName(owner) << " infoNames: " << endreq;
    for( std::set<std::string>::iterator infoNamesIt = (*infoNamesSet).begin();
         infoNamesIt!=(*infoNamesSet).end();++infoNamesIt)
      msg << MSG::DEBUG << "\t" <<  (*infoNamesIt) << endreq;
  }
}
void MonitorSvc::undeclareAll( const IInterface* owner )
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  if( 0!=owner ){
    std::string ownerName = infoOwnerName( owner );
    std::set<std::string> * infoNamesSet = getInfos( owner );
    if( 0 == infoNamesSet ) {
      msg << MSG::WARNING << "undeclareAll: No infos to  undeclare for " 
          << ownerName << endreq;
      return;
    }
    std::set<std::string>::iterator infoNamesIt;
    msg << MSG::DEBUG << "undeclareAll: List of services published by " 
        << ownerName << endreq;
    for( infoNamesIt = (*infoNamesSet).begin();
         infoNamesIt!=(*infoNamesSet).end();++infoNamesIt)
      msg << MSG::DEBUG << (*infoNamesIt) << " ";
    msg << MSG::DEBUG << endreq;
    for( infoNamesIt = (*infoNamesSet).begin();
         infoNamesIt!=(*infoNamesSet).end();++infoNamesIt){
      m_dimeng->undeclSvc( ownerName+"/"+(*infoNamesIt) ) ;
      m_dimeng->undeclSvc( ownerName+"/"+(*infoNamesIt)+"/gauchocomment" ) ;
      msg << MSG::INFO << "undeclareAll: Undeclared info " << (*infoNamesIt) 
          << " from owner " << ownerName << endreq;
    }
    m_InfoNamesMap.erase(owner );
  } else { // Null pointer. Undeclare for all owners
    for(m_InfoNamesMapIt = m_InfoNamesMap.begin();
        m_InfoNamesMapIt != m_InfoNamesMap.end();++m_InfoNamesMapIt)
      undeclareAll( m_InfoNamesMapIt->first );
  }
}
std::set<std::string> * MonitorSvc::getInfos(const IInterface* owner )
{
  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
  if( m_InfoNamesMapIt != m_InfoNamesMap.end() ) 
    return &(m_InfoNamesMapIt->second);
  else {
    return 0;
  }
}
std::string MonitorSvc::infoOwnerName( const IInterface* owner )
{
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
