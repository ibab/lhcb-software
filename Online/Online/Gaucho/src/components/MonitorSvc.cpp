// Include files
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "Gaucho/DimServiceMonObject.h"
#include "Gaucho/MonObject.h"
#include "Gaucho/MonInt.h"
#include "Gaucho/MonDouble.h"
#include "Gaucho/MonLong.h"
#include "Gaucho/MonFloat.h"
#include "Gaucho/MonBool.h"
#include "Gaucho/MonPairDD.h"
#include "Gaucho/MonPairDI.h"
#include "Gaucho/MonPairII.h"
#include "Gaucho/MonString.h"
#include "Gaucho/MonProfile.h"
#include "Gaucho/MonH1D.h"
#include "Gaucho/MonH2D.h"
#include "Gaucho/MonH1F.h"
#include "Gaucho/MonH2F.h"
#include "Gaucho/MonVectorI.h"
#include "Gaucho/MonVectorD.h"
#include "Gaucho/MonObjectCreator.h"
#include "Gaucho/MonRate.h"
#include "MonitorSvc.h"
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
MonitorSvc::MonitorSvc(const std::string& name, ISvcLocator* sl): 
  Service(name, sl)
{ 
  
}


MonitorSvc::~MonitorSvc() {
}
//Query interfaces of Interface
// @param riid       ID of Interface to be retrieved
// @param ppvUnknown Pointer to Location for interface pointer
StatusCode MonitorSvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
  if(IID_IMonitorSvc == riid) {
    *ppvIF = dynamic_cast<IMonitorSvc*> (this);
  } 
  else if (IID_IGauchoMonitorSvc == riid) {
    *ppvIF = dynamic_cast<IGauchoMonitorSvc*> (this);
  }
  else {
    return Service::queryInterface(riid, ppvIF);
  }
  addRef();
  return StatusCode::SUCCESS;
}


StatusCode MonitorSvc::initialize() {
  MsgStream msg(msgSvc(),"MonitorSvc");
  if( IService::INITIALIZED == this->state() ) {
    msg << MSG::INFO << "MonitorSvc already initialized" << endreq;
    return StatusCode::SUCCESS; 
  }
  Service::initialize(); 
  //const std::string& utgid = RTL::processName();
  m_utgid = RTL::processName();
  msg << MSG::INFO << "initialize: Setting up DIM for UTGID " << m_utgid << endreq;
  
  m_dimpropsvr= new DimPropServer(m_utgid, serviceLocator());
  msg << MSG::INFO << "DimPropServer created with name " << m_utgid << endreq;

  m_dimcmdsvr = new DimCmdServer( (m_utgid+"/"), serviceLocator());
  msg << MSG::INFO << "DimCmdServer created with name " << (m_utgid+"/") << endreq;

  msg << MSG::DEBUG << "Declaring MonRate Information" << endreq;
  
  m_monRate = new MonRate(msgSvc(), "MonitorSvc", 0);
  m_monRateDeclared = false;
  
  return StatusCode::SUCCESS;
}


StatusCode MonitorSvc::finalize() {
  
  //dim_lock();
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::INFO << "MonitorSvc Destructor" << endreq;
//  m_InfoNamesMap.clear();
  msg << MSG::INFO << "delete m_dimcmdsvr" << endreq;
  delete m_dimcmdsvr;  m_dimcmdsvr = 0;
  
  msg << MSG::INFO << "delete m_dimpropsvr" << endreq;
  delete m_dimpropsvr; m_dimpropsvr = 0;
  
  msg << MSG::INFO << "delete m_monRate" << endreq;
  delete m_monRate; m_monRate = 0;
  //dim_unlock();
  msg << MSG::INFO << "finalized successfully" << endreq;
  
  return StatusCode::SUCCESS;
}
  
void MonitorSvc::declareInfo(const std::string& name, const bool&  var, 
                             const std::string& desc, const IInterface* owner) 
{
  MonObject *monObject = MonObjectCreator::createMonObject(s_monBool, msgSvc(), "MonitorSvc");
  ((MonBool*) monObject)->setValue(var);
  declareInfoMonObject(name, monObject, desc, owner);
}

void MonitorSvc::declareInfo(const std::string& name, const int&  var, 
                             const std::string& desc, const IInterface* owner) 
{
  MonObject *monObject = MonObjectCreator::createMonObject(s_monInt, msgSvc(), "MonitorSvc");
  ((MonInt*) monObject)->setValue(var);
  declareInfoMonObject(name, monObject, desc, owner);
}

void MonitorSvc::declareInfo(const std::string& name, const long&  var, 
                             const std::string& desc, const IInterface* owner) 
{
  MonObject *monObject = MonObjectCreator::createMonObject(s_monLong, msgSvc(), "MonitorSvc");
  ((MonLong*) monObject)->setValue(var);
  declareInfoMonObject(name, monObject, desc, owner);
}

void MonitorSvc::declareInfo(const std::string& name, const double& var, 
                             const std::string& desc, const IInterface* owner) 
{
  if (name.find("COUNTER_TO_RATE") != std::string::npos) {
    std::string newName = name;
    newName.erase(0, 16);
    newName.erase(newName.size() - 1, 1);
    if (!m_monRateDeclared) declareInfoMonObject("monRate", m_monRate, "MonRate Description", this);
    m_monRate->addCounter(newName, desc, var);
    return;
  } 
  
  MonObject *monObject = MonObjectCreator::createMonObject(s_monDouble, msgSvc(), "MonitorSvc");
  ((MonDouble*) monObject)->setValue(var);
  declareInfoMonObject(name, monObject, desc, owner);
}

void MonitorSvc::declareInfo(const std::string& name, const std::string& var, 
                             const std::string& desc, const IInterface* owner) 
{
  MonObject *monObject = MonObjectCreator::createMonObject(s_monString, msgSvc(), "MonitorSvc");
  ((MonString*) monObject)->setValue(var);
  declareInfoMonObject(name, monObject, desc, owner);
}

void MonitorSvc::declareInfo(const std::string& name, const std::pair<double,double>& var, 
                             const std::string& desc, const IInterface* owner) 
{
  MonObject *monObject = MonObjectCreator::createMonObject(s_monPairDD, msgSvc(), "MonitorSvc");
  ((MonPairDD*) monObject)->setValue(var);
  declareInfoMonObject(name, monObject, desc, owner);
}

void MonitorSvc::declareInfo(const std::string& name, const std::string& format, const void* var,
                             int size, const std::string& desc, const IInterface* owner) 
{
  // We use this funtion to publish MonObjects. unhappyness we can not modify the IMonitorSvc 
  // we igore values format and size. To hide the warnings because they are not used we print them.
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::DEBUG << "format=" << format << endreq;
  msg << MSG::DEBUG << "size=" << size << endreq;
  
  MonObject *monObject = const_cast<MonObject *>((const MonObject*) var);
  declareInfoMonObject(name, monObject, desc, owner);
}

void MonitorSvc::declareInfo(const std::string& name, const AIDA::IBaseHistogram* var, 
                             const std::string& desc, const IInterface* owner) 
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  MonObject *monObject=0;
  if( 0 != dynamic_cast<const AIDA::IProfile1D * >(var) ) {
    monObject = MonObjectCreator::createMonObject(s_monProfile, msgSvc(), "MonitorSvc");
    ((MonProfile*) monObject)->setAidaProfile(const_cast<AIDA::IProfile1D *>(dynamic_cast<const AIDA::IProfile1D *>(var)));
  }
  else if( 0 != dynamic_cast<const AIDA::IHistogram1D * >(var) ){ 
    monObject = MonObjectCreator::createMonObject(s_monH1F, msgSvc(), "MonitorSvc");
    ((MonH1F*) monObject)->setAidaHisto(const_cast<AIDA::IHistogram1D *>(dynamic_cast<const AIDA::IHistogram1D *>(var)));
  
  }
  else if( 0 != dynamic_cast<const AIDA::IHistogram2D * >(var) ){ 
    monObject = MonObjectCreator::createMonObject(s_monH2F, msgSvc(), "MonitorSvc");
    ((MonH2F*) monObject)->setAidaHisto(const_cast<AIDA::IHistogram2D *>(dynamic_cast<const AIDA::IHistogram2D *>(var)));
  }
  else {
    msg << MSG::ERROR << "Unknown histogram type, name = " << name << ", desc = " << desc << endreq;
    return;
  }
  
  declareInfoMonObject(name, monObject, desc, owner);

}

void MonitorSvc::declareMonRateComplement( int& runNumber, int& cycleNumber, longlong& timeFirstEvInRun, longlong& timeLastEvInCycle){
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::DEBUG << "Inside declareMonRateComplement" << endreq;
// /*  m_runNumber = &runNumber;
//   m_cycleNumber = &cycleNumber;
//   m_timeFirstEvInRun = &timeFirstEvInRun;
//   m_timeLastEvInCycle = &timeLastEvInCycle;*/
  m_monRate->addComplement(&runNumber, &cycleNumber, &timeFirstEvInRun, &timeLastEvInCycle);
  m_monRate->print();
}  

void MonitorSvc::declareInfoMonObject(const std::string& name, MonObject* monObject, const std::string& desc, const IInterface* owner) 
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  
  //msg << MSG::DEBUG << "declareInfoMonObject: " << endreq;
  m_InfoNamesMapIt = m_InfoNamesMap.find(owner);
  std::string ownerName = infoOwnerName(owner);
  if( m_InfoNamesMapIt != m_InfoNamesMap.end()) {
    std::pair<std::set<std::string>::iterator,bool> p = (*m_InfoNamesMapIt).second.insert(name);
    if( p.second) msg << MSG::INFO << "Declaring info: Owner: " << ownerName << " Name: " << name << endreq;
    else 
    { // Insertion failed: Name already exists
      msg << MSG::ERROR << "Already existing info " << name << " from owner " << ownerName << " not published" << endreq;
      return;
    }
  }
  else { // Create a new set for this algo and insert name
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(name);
  }
  
  std::string dimName = name;
  if (dimName.find(ownerName) == std::string::npos) dimName = ownerName + "/" + dimName;
  
  msg << MSG::DEBUG << "dimName: " << dimName << endreq;
  
  monObject->setComments(desc);
  
  std::string dimPrefix = monObject->dimPrefix();
  std::string dimSvcName = dimPrefix + "/"+m_utgid+"/"+dimName;
  std::pair<DimServiceMonObjectMapIt,bool> p = m_dimMonObjects.insert(DimServiceMonObjectMap::value_type(dimName,0));

  if (p.second) {
    m_dimMonObjects[dimName]=new DimServiceMonObject(dimSvcName, monObject);
    msg << MSG::DEBUG << "New DimMonObject: " + dimSvcName << endreq;
  }
  else msg << MSG::ERROR << "Already existing DimMonObject: " + dimSvcName << endreq;
  
}

void MonitorSvc::undeclareInfo( const std::string& name, const IInterface* owner )
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

    std::string dimName = name;
    if (dimName.find(ownerName) == std::string::npos) dimName = ownerName + "/" + dimName;
    undeclServiceMonObject( dimName ) ;
    
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

//updateSvc and resetHistos methods are for fast run changes
void MonitorSvc::updateSvc( const std::string& name, bool endOfRun, const IInterface* owner )
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  std::set<std::string> * infoNamesSet = getInfos( owner );
  if( 0 == infoNamesSet ) {
    msg << MSG::WARNING << "update: Info  " << name << ": Nothing to update for " << infoOwnerName(owner) << ". Empty set" << endreq;
    return;
  }
  std::string ownerName = infoOwnerName( owner );
  if( (*infoNamesSet).find( name) !=  (*infoNamesSet).end() ){
    std::string dimName = name;
    if (dimName.find(ownerName) == std::string::npos) dimName = ownerName + "/" + dimName;
    updateServiceMonObject( dimName, endOfRun ) ;
    //msg << MSG::DEBUG << "update: " << name << " from owner " << ownerName  << " updated" << endreq;
  }  
  else{
    msg << MSG::WARNING << "update: Info  " << name << " declared by " << infoOwnerName(owner) << " not found" << endreq;
    msg << MSG::DEBUG << infoOwnerName(owner) << " infoNames: " << endreq;
    for( std::set<std::string>::iterator infoNamesIt = (*infoNamesSet).begin(); infoNamesIt!=(*infoNamesSet).end();++infoNamesIt)
      msg << MSG::DEBUG << "\t" <<  (*infoNamesIt) << endreq;
  }
}

void MonitorSvc::undeclareAll( const IInterface* owner)
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
      std::string dimName = (*infoNamesIt);
      if (dimName.find(ownerName) == std::string::npos) dimName = ownerName + "/" + dimName;
      undeclServiceMonObject( dimName ) ;
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

void MonitorSvc::updateAll( bool endOfRun, const IInterface* owner)
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::DEBUG << " inside updateAll" << endreq;
  if( 0!=owner ){
    std::string ownerName = infoOwnerName( owner );
    std::set<std::string> * infoNamesSet = getInfos( owner );
    if( 0 == infoNamesSet ) {
      msg << MSG::WARNING << "updateAll: No infos to update for " << ownerName << endreq;
      return;
    }
    std::set<std::string>::iterator infoNamesIt;
    msg << MSG::DEBUG << "updateAll: List of services published by " << ownerName << endreq;
    for( infoNamesIt = (*infoNamesSet).begin();
         infoNamesIt!=(*infoNamesSet).end();++infoNamesIt)
      msg << MSG::DEBUG << (*infoNamesIt) << " ";
    msg << MSG::DEBUG << endreq;
    for( infoNamesIt = (*infoNamesSet).begin();
         infoNamesIt!=(*infoNamesSet).end();++infoNamesIt){
      std::string dimName = (*infoNamesIt);
      if (dimName.find(ownerName) == std::string::npos) dimName = ownerName + "/" + dimName;
      updateServiceMonObject( dimName, endOfRun ) ;
      //msg << MSG::DEBUG << "updateAll: Updated info " << (*infoNamesIt) << " from owner " << ownerName << endreq;
    }
  } else { // Null pointer. Update for all owners
    for(m_InfoNamesMapIt = m_InfoNamesMap.begin();
        m_InfoNamesMapIt != m_InfoNamesMap.end();++m_InfoNamesMapIt)
      updateAll(endOfRun, m_InfoNamesMapIt->first );
  }
}

void MonitorSvc::resetHistos( const IInterface* owner ) 
{
 MsgStream msg(msgSvc(),"MonitorSvc");
 if( 0!=owner ){
    std::string ownerName = infoOwnerName( owner );
    std::set<std::string> * infoNamesSet = getInfos( owner );
    if( 0 == infoNamesSet ) {
      msg << MSG::WARNING << "resethistos: No histograms to reset for " 
          << ownerName << endreq;
      return;
    }
   ownerName=ownerName+"/";
   DimRpcInfo rpc(ownerName.c_str(),"");
   rpc.setData("resethistos");
 }
 else{ // Null pointer. reset for all owners
    for(m_InfoNamesMapIt = m_InfoNamesMap.begin();
        m_InfoNamesMapIt != m_InfoNamesMap.end();++m_InfoNamesMapIt)
      resetHistos(m_InfoNamesMapIt->first);
 }
 
}

void MonitorSvc::undeclServiceMonObject(std::string infoName)
{
  MsgStream msg(msgSvc(),"MonitorSvc");

  for (m_dimMonObjectsIt = m_dimMonObjects.begin(); m_dimMonObjectsIt != m_dimMonObjects.end(); m_dimMonObjectsIt++)
      msg << MSG::DEBUG << (*m_dimMonObjectsIt).first << endreq;
  
  m_dimMonObjectsIt = m_dimMonObjects.find(infoName);

  if(m_dimMonObjectsIt != m_dimMonObjects.end()) {
    delete (*m_dimMonObjectsIt).second;
    m_dimMonObjects.erase(m_dimMonObjectsIt);
    msg << MSG::DEBUG << "undeclSvc: Service " + infoName + " undeclared" << endreq;
    return;
  }
  msg << MSG::ERROR << "undeclSvc: No DimServiceMonObject found with the name:" + infoName << endreq;
}

void MonitorSvc::updateServiceMonObject(std::string infoName, bool endOfRun)
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  //for (m_dimMonObjectsIt = m_dimMonObjects.begin(); m_dimMonObjectsIt != m_dimMonObjects.end(); m_dimMonObjectsIt++)
  //  mes << MSG::DEBUG << (*m_dimMonObjectsIt).first << endreq;
  m_dimMonObjectsIt = m_dimMonObjects.find(infoName);
  if(m_dimMonObjectsIt != m_dimMonObjects.end()) {
    (*m_dimMonObjectsIt).second->updateServiceMonObject(endOfRun);
    //msg << MSG::DEBUG << "updateSvc: Service " + infoName + " updated" << endreq;
    return;
  }
  msg << MSG::WARNING << "updateSvc: No DimServiceMonObject found with the name:" + infoName << endreq;
}

