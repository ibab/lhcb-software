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
#include "Gaucho/MonRate.h"
#include "MonTimer.h"
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
  Service(name, sl), m_refreshTime(10) 
{ 
  declareProperty("refreshtime", m_refreshTime);
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

// implement Service methods
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
 
   
/*  msg << MSG::INFO << "Getting the IGauchoMonitorSvc Interface " << endreq;   
  StatusCode sc = serviceLocator()->service("MonitorSvc", m_gauchoMonitorSvc, true );
  if( sc.isSuccess() ) msg << MSG::INFO << "Found the IGauchoMonitorSvc interface" << endreq; 	
  else msg << MSG::FATAL << "Unable to locate the IGauchoMonitorSvc interface." << endreq;*/
   
  msg << MSG::INFO << "Creating Timer" << endreq;
  setTimerElapsed(false);
  m_monTimer = new MonTimer(m_refreshTime, serviceLocator(), this);
  msg << MSG::INFO << "MonTimer created. Services will be updated every " << m_refreshTime << " seconds" << endreq;
  return StatusCode::SUCCESS;
}


StatusCode MonitorSvc::finalize() {
  
  //dim_lock();
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::INFO << "MonitorSvc Destructor" << endreq;
//  m_InfoNamesMap.clear();
  msg << MSG::INFO << "delete m_dimcmdsvr" << endreq;
  delete m_dimcmdsvr;  m_dimcmdsvr = 0;
  msg << MSG::INFO << "delete m_monTimer" << endreq;
  delete m_monTimer; m_monTimer = 0;
  msg << MSG::INFO << "delete m_dimpropsvr" << endreq;
  delete m_dimpropsvr; m_dimpropsvr = 0;
  //dim_unlock();
  msg << MSG::INFO << "finalized successfully" << endreq;
  //this->~MonitorSvc();
  return StatusCode::SUCCESS;
}

void MonitorSvc::declareInfo(const std::string& name, const bool&  var, 
                             const std::string& desc, const IInterface* owner) 
{
  MonBool* monBool = new MonBool(msgSvc(),"MonitorSvc");
  monBool->setValue(var);
  declareInfoMonObject(name, monBool, desc, owner);
}
void MonitorSvc::declareInfo(const std::string& name, const int&  var, 
                             const std::string& desc, const IInterface* owner) 
{
  MonInt* monInt = new MonInt(msgSvc(),"MonitorSvc");
  monInt->setValue(var);
  declareInfoMonObject(name, monInt, desc, owner);
}
void MonitorSvc::declareInfo(const std::string& name, const long&  var, 
                             const std::string& desc, const IInterface* owner) 
{
  MonLong* monLong = new MonLong(msgSvc(),"MonitorSvc");
  monLong->setValue(var);
  declareInfoMonObject(name, monLong, desc, owner);
}
void MonitorSvc::declareInfo(const std::string& name, const double& var, 
                             const std::string& desc, const IInterface* owner) 
{
  MonDouble* monDouble = new MonDouble(msgSvc(),"MonitorSvc");
  monDouble->setValue(var);
  declareInfoMonObject(name, monDouble, desc, owner);
}
void MonitorSvc::declareInfo(const std::string& name, const float& var, 
                             const std::string& desc, const IInterface* owner) 
{
  MonFloat* monFloat = new MonFloat(msgSvc(),"MonitorSvc");
  monFloat->setValue(var);
  declareInfoMonObject(name, monFloat, desc, owner);
}
void MonitorSvc::declareInfo(const std::string& name, const std::string& var, 
                             const std::string& desc, const IInterface* owner) 
{
  MonString* monString = new MonString(msgSvc(),"MonitorSvc");
  monString->setValue(var);
  declareInfoMonObject(name, monString, desc, owner);
}
void MonitorSvc::declareInfo(const std::string& name, const std::pair<double,double>& var, 
                             const std::string& desc, const IInterface* owner) 
{
  MonPairDD* monPair = new MonPairDD(msgSvc(),"MonitorSvc");
  monPair->setValue(var);
  declareInfoMonObject(name, monPair, desc, owner);
}
void MonitorSvc::declareInfo(const std::string& name, const std::pair<int,int>& var, 
                             const std::string& desc, const IInterface* owner) 
{
  MonPairII* monPair = new MonPairII(msgSvc(),"MonitorSvc");
  monPair->setValue(var);
  declareInfoMonObject(name, monPair, desc, owner);
}
void MonitorSvc::declareInfo(const std::string& name, const std::pair<double,int>& var, 
                             const std::string& desc, const IInterface* owner) 
{
  MonPairDI* monPair = new MonPairDI(msgSvc(),"MonitorSvc");
  monPair->setValue(var);
  declareInfoMonObject(name, monPair, desc, owner);
}

void MonitorSvc::declareInfo(const std::string& name, const std::string& format, const void* var,
                             int size, const std::string& desc, const IInterface* owner) 
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::ERROR << "Trying to use an Obsolete declare info for structure which is incompatible with MonObjects" << endreq;
  msg << MSG::ERROR << "name=" << name << endreq;
  msg << MSG::ERROR << "format=" << format << endreq;
  msg << MSG::ERROR << "var=" << var << endreq;
  msg << MSG::ERROR << "size=" << size << endreq;
  msg << MSG::ERROR << "desc=" << desc << endreq;
  msg << MSG::ERROR << "ownerName=" << infoOwnerName( owner ) << endreq;
    
}

void MonitorSvc::declareInfo(const std::string& name, const AIDA::IBaseHistogram* var, 
                             const std::string& desc, const IInterface* owner) 
{
  if( 0 != dynamic_cast<const AIDA::IProfile1D * >(var) ) { 
  	MonProfile* monPro = new MonProfile(msgSvc(),"MonitorSvc");
  	monPro->setAidaProfile(const_cast<AIDA::IProfile1D *>(dynamic_cast<const AIDA::IProfile1D *>(var)));
  	declareInfoMonObject(name, monPro, desc, owner);
  }
  else if( 0 != dynamic_cast<const AIDA::IHistogram1D * >(var) ){ 
  	MonH1F* monH1F = new MonH1F(msgSvc(),"MonitorSvc");
  	monH1F->setAidaHisto(const_cast<AIDA::IHistogram1D *>(dynamic_cast<const AIDA::IHistogram1D *>(var)));
  	declareInfoMonObject(name, monH1F, desc, owner);
  }
  else if( 0 != dynamic_cast<const AIDA::IHistogram2D * >(var) ){ 
  	MonH2F* monH2F = new MonH2F(msgSvc(),"MonitorSvc");
  	monH2F->setAidaHisto(const_cast<AIDA::IHistogram2D *>(dynamic_cast<const AIDA::IHistogram2D *>(var)));
  	declareInfoMonObject(name, monH2F, desc, owner);
  }
  //else std::cout << "Unknown histogram type: " << diminfoname  << endreq;

}

void MonitorSvc::declareInfo(const std::string& name, 
                             const MonObject* var, 
                             const std::string& desc, 
                             const IInterface* owner) 
{
//  MsgStream msg(msgSvc(),"MonitorSvc");
//  msg << MSG::DEBUG << "declareInfo MONOBJECT" << endreq;
  MonObject *tmpvar = const_cast<MonObject *>(var);
  if (s_monRate == tmpvar->typeName()){
    MonRate* monRate = (MonRate*) tmpvar;
    monRate->declareComplement(m_runNumber, m_cycleNumber, m_timeFirstEvInRun, m_timeLastEvInCycle);
  }
  declareInfoMonObject(name, tmpvar, desc, owner);
}

void MonitorSvc::declareMonRateComplement( int& runNumber, int& cycleNumber, longlong& timeFirstEvInRun, longlong& timeLastEvInCycle){
  m_runNumber = &runNumber;
  m_cycleNumber = &cycleNumber;
  m_timeFirstEvInRun = &timeFirstEvInRun;
  m_timeLastEvInCycle = &timeLastEvInCycle;
}  

void MonitorSvc::declareInfoMonObject(const std::string& name, 
                             MonObject* var, 
                             const std::string& desc, 
                             const IInterface* owner) 
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  //msg << MSG::DEBUG << "declareInfoMonObject: " << endreq;
  m_InfoNamesMapIt = m_InfoNamesMap.find(owner);
  std::string ownerName = infoOwnerName(owner);
  if( m_InfoNamesMapIt != m_InfoNamesMap.end()) {
    std::pair<std::set<std::string>::iterator,bool> p = (*m_InfoNamesMapIt).second.insert(name);
    if( p.second) msg << MSG::INFO << "Declaring info: Owner: " << ownerName << " Name: " << name << endreq;
    else { // Insertion failed: Name already exists
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
  
  var->setComments(desc);
  declServiceMonObject(dimName, var);
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
void MonitorSvc::updateSvc( const std::string& name, bool endOfRun, 
                                const IInterface* owner )
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  std::set<std::string> * infoNamesSet = getInfos( owner );
  if( 0 == infoNamesSet ) {
    msg << MSG::WARNING << "update: Info  " << name 
        << ": Nothing to update for " << infoOwnerName(owner) 
        << ". Empty set" << endreq;
    return;
  }
  std::string ownerName = infoOwnerName( owner );
  if( (*infoNamesSet).find( name) !=  (*infoNamesSet).end() ){
    std::string dimName = name;
    if (dimName.find(ownerName) == std::string::npos) dimName = ownerName + "/" + dimName;
    updateServiceMonObject( dimName, endOfRun ) ;
    msg << MSG::DEBUG << "update: " << name << " from owner " 
        << ownerName  << " updated" << endreq;
  }  
  else{
    msg << MSG::WARNING << "update: Info  " << name << " declared by " 
        << infoOwnerName(owner) << " not found" << endreq;
    msg << MSG::DEBUG << infoOwnerName(owner) << " infoNames: " << endreq;
    for( std::set<std::string>::iterator infoNamesIt = (*infoNamesSet).begin();
         infoNamesIt!=(*infoNamesSet).end();++infoNamesIt)
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
  msg << MSG::INFO << " inside updateAll" << endreq;
  if( 0!=owner ){
    std::string ownerName = infoOwnerName( owner );
    std::set<std::string> * infoNamesSet = getInfos( owner );
    if( 0 == infoNamesSet ) {
      msg << MSG::WARNING << "updateAll: No infos to update for " 
          << ownerName << endreq;
      return;
    }
    std::set<std::string>::iterator infoNamesIt;
    msg << MSG::DEBUG << "updateAll: List of services published by " 
        << ownerName << endreq;
    for( infoNamesIt = (*infoNamesSet).begin();
         infoNamesIt!=(*infoNamesSet).end();++infoNamesIt)
      msg << MSG::DEBUG << (*infoNamesIt) << " ";
    msg << MSG::DEBUG << endreq;
    for( infoNamesIt = (*infoNamesSet).begin();
         infoNamesIt!=(*infoNamesSet).end();++infoNamesIt){
      std::string dimName = (*infoNamesIt);
      if (dimName.find(ownerName) == std::string::npos) dimName = ownerName + "/" + dimName;
      updateServiceMonObject( dimName, endOfRun ) ;
      msg << MSG::DEBUG << "updateAll: Updated info " << (*infoNamesIt) 
          << " from owner " << ownerName << endreq;
    }
  } else { // Null pointer. Update for all owners
    for(m_InfoNamesMapIt = m_InfoNamesMap.begin();
        m_InfoNamesMapIt != m_InfoNamesMap.end();++m_InfoNamesMapIt)
      updateAll(endOfRun, m_InfoNamesMapIt->first );
  }
 // setLastUpdateTime();
  setTimerElapsed(false); 
}

void MonitorSvc::resetHistos( const IInterface* owner ) {
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

void MonitorSvc::setTimerElapsed(bool elapsed) {
 m_TimerElapsed=elapsed; 
}

bool MonitorSvc::getTimerElapsed() const {
    return m_TimerElapsed;  
}

//void setTimeFirstEvElapsed(time_t time) {
//  m_time = time;
//}
  
void MonitorSvc::declServiceMonObject(std::string infoName, const MonObject* monObject){
  MsgStream msg(msgSvc(),"MonitorSvc");
  std::string typeName = monObject->typeName();
  std::string dimPrefix = monObject->dimPrefix();

  std::string dimSvcName = dimPrefix + "/"+m_utgid+"/"+infoName;

  std::pair<DimServiceMonObjectMapIt,bool> p = m_dimMonObjects.insert(DimServiceMonObjectMap::value_type(infoName,0));

  if (p.second) {

    MonObject* monObjectAux = const_cast<MonObject *>(monObject);
    //monObjectAux->print();
    std::stringstream ss; 
    boost::archive::binary_oarchive oa(ss);
    monObjectAux->save(oa, 0);
    int size = ss.str().length();
    msg << MSG::DEBUG << " size = " << size << endreq;
    m_dimMonObjects[infoName]=new DimServiceMonObject(dimSvcName, monObjectAux, size);

    msg << MSG::DEBUG << "New DimMonObject: " + dimSvcName << endreq;
  }
  else msg << MSG::ERROR << "Already existing DimMonObject: " + dimSvcName << endreq;
}

void MonitorSvc::undeclServiceMonObject(std::string infoName){
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

void MonitorSvc::updateServiceMonObject(std::string infoName, bool endOfRun){
  MsgStream msg(msgSvc(),"MonitorSvc");
  //for (m_dimMonObjectsIt = m_dimMonObjects.begin(); m_dimMonObjectsIt != m_dimMonObjects.end(); m_dimMonObjectsIt++)
  //  mes << MSG::DEBUG << (*m_dimMonObjectsIt).first << endreq;
  m_dimMonObjectsIt = m_dimMonObjects.find(infoName);
  if(m_dimMonObjectsIt != m_dimMonObjects.end()) {
    (*m_dimMonObjectsIt).second->updateService(endOfRun);
    msg << MSG::DEBUG << "updateSvc: Service " + infoName + " updated" << endreq;
    return;
  }
  msg << MSG::WARNING << "updateSvc: No DimServiceMonObject found with the name:" + infoName << endreq;
}

