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
   declareProperty("disableMonRate", m_disableMonRate = 0);
   declareProperty("disableDimPropServer", m_disableDimPropServer = 0);
   declareProperty("disableDimCmdServer", m_disableDimCmdServer = 0);
   
   declareProperty("disableMonObjectsForBool", m_disableMonObjectsForBool = 1);
   declareProperty("disableMonObjectsForInt", m_disableMonObjectsForInt = 1);
   declareProperty("disableMonObjectsForLong", m_disableMonObjectsForLong = 1);
   declareProperty("disableMonObjectsForDouble", m_disableMonObjectsForDouble = 1);
   declareProperty("disableMonObjectsForString", m_disableMonObjectsForString = 1);
   declareProperty("disableMonObjectsForPairs", m_disableMonObjectsForPairs = 0);
   declareProperty("disableMonObjectsForHistos", m_disableMonObjectsForHistos = 0);
   
   declareProperty("disableDeclareInfoBool", m_disableDeclareInfoBool = 0);
   declareProperty("disableDeclareInfoInt", m_disableDeclareInfoInt = 0);
   declareProperty("disableDeclareInfoLong", m_disableDeclareInfoLong = 0);
   declareProperty("disableDeclareInfoDouble", m_disableDeclareInfoDouble = 0);
   declareProperty("disableDeclareInfoString", m_disableDeclareInfoString = 0);
   declareProperty("disableDeclareInfoPair", m_disableDeclareInfoPair = 0);
   declareProperty("disableDeclareInfoFormat", m_disableDeclareInfoFormat = 0);
   declareProperty("disableDeclareInfoHistos", m_disableDeclareInfoHistos = 0);
      
   declareProperty("teste", m_teste);
 
  MsgStream msg(msgSvc(),"MonitorSvc");
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

  msg << MSG::DEBUG << "Initialize=====>m_disableDeclareInfoHistos : " << m_disableDeclareInfoHistos << endreq;


  //const std::string& utgid = RTL::processName();
  m_utgid = RTL::processName();
  msg << MSG::DEBUG << "initialize: Setting up DIM for UTGID " << m_utgid << endreq;

  if ( 0 == m_disableDimPropServer) {
    m_dimpropsvr= new DimPropServer(m_utgid, serviceLocator());
    msg << MSG::DEBUG << "DimPropServer created with name " << m_utgid << endreq;
  }
  else msg << MSG::DEBUG << "DimPropServer process is disabled." << endreq;

  if ( 0 == m_disableDimPropServer) {
    m_dimcmdsvr = new DimCmdServer( (m_utgid+"/"), serviceLocator(), this);
    msg << MSG::DEBUG << "DimCmdServer created with name " << (m_utgid+"/") << endreq;
  }
  else msg << MSG::DEBUG << "DimCmdServer process is disabled." << endreq; 

  if ( 0 == m_disableMonRate) {
    msg << MSG::DEBUG << "new MonRate " << endreq;
    m_monRate = new MonRate(msgSvc(), "MonitorSvc", 0);
    m_monRateDeclared = false;
    msg << MSG::DEBUG << "End of new MonRate Information" << endreq;
  }
  else  msg << MSG::DEBUG << "MonRate process is disabled." << endreq; 

  return StatusCode::SUCCESS;
}


StatusCode MonitorSvc::finalize() {
  
  //dim_lock();
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::DEBUG << "MonitorSvc Destructor" << endreq;
//  m_InfoNamesMap.clear();
  if ( 0 == m_disableDimCmdServer){
    msg << MSG::DEBUG << "delete m_dimcmdsvr" << endreq;
    if (m_dimcmdsvr) delete m_dimcmdsvr;  m_dimcmdsvr = 0;
  }
  if ( 0 == m_disableDimPropServer){
    msg << MSG::DEBUG << "delete m_dimpropsvr" << endreq;
    if (m_dimpropsvr) delete m_dimpropsvr; m_dimpropsvr = 0;
  }

  if ( 0 == m_disableMonRate){
    msg << MSG::DEBUG << "delete m_monRate" << endreq;
    //if (m_monRate) delete m_monRate; m_monRate = 0;
  }
  //dim_unlock();
  msg << MSG::DEBUG << "finalized successfully" << endreq;
  
  return StatusCode::SUCCESS;
}
  
void MonitorSvc::declareInfo(const std::string& name, const bool&  var, 
                             const std::string& desc, const IInterface* owner) 
{
  if (0 != m_disableDeclareInfoBool) return;

  MsgStream msg(msgSvc(),"MonitorSvc");
  MonObject *monObject = 0;
  bool isMonObject = false;
  std::string prefix = "";
  if (m_disableMonObjectsForBool == 0) {
    isMonObject = true;
    monObject = MonObjectCreator::createMonObject(s_monBool, msgSvc(), "MonitorSvc");
    monObject->setComments(desc);
    ((MonBool*) monObject)->setValue(var);
    prefix = monObject->dimPrefix() + "/";
  }

  if (!registerName(name, owner)) return;
  std::pair<std::string, std::string> dimSvcName = registerDimSvc(name, prefix, owner, false);
  if ("" == dimSvcName.second) return;

  if (isMonObject) m_dimSrv[dimSvcName.first]=new DimServiceMonObject(dimSvcName.second, monObject);
  else {
    m_dimSrv[dimSvcName.first]= new DimService(dimSvcName.second.c_str(), "C:1", (void*)&var, sizeof(bool));
    std::pair<std::string, std::string> dimSvcNameComment = registerDimSvc(name, prefix, owner, true);
    if ("" != dimSvcNameComment.second) m_dimSrv[dimSvcNameComment.first]= new DimService(dimSvcNameComment.second.c_str(),(char*)desc.c_str());
  }
  msg << MSG::DEBUG << "New DimService: " + dimSvcName.second << endreq;
}

void MonitorSvc::declareInfo(const std::string& name, const int&  var, 
                             const std::string& desc, const IInterface* owner) 
{
  if (0 != m_disableDeclareInfoInt) return;
  MsgStream msg(msgSvc(),"MonitorSvc");
  MonObject *monObject = 0;
  bool isMonObject = false;
  std::string prefix = "";
  if (m_disableMonObjectsForInt == 0) {
    isMonObject = true;
    monObject = MonObjectCreator::createMonObject(s_monInt, msgSvc(), "MonitorSvc");
    monObject->setComments(desc);
    ((MonInt*) monObject)->setValue(var);
    prefix = monObject->dimPrefix() + "/";
  }

  if (!registerName(name, owner)) return;
  std::pair<std::string, std::string> dimSvcName = registerDimSvc(name, prefix, owner, false);
  if ("" == dimSvcName.second) return;

  if (isMonObject) m_dimSrv[dimSvcName.first]=new DimServiceMonObject(dimSvcName.second, monObject);
  else {
    m_dimSrv[dimSvcName.first]= new DimService(dimSvcName.second.c_str(),(int&)var);
    std::pair<std::string, std::string> dimSvcNameComment = registerDimSvc(name, prefix, owner, true);
    if ("" != dimSvcNameComment.second) m_dimSrv[dimSvcNameComment.first]= new DimService(dimSvcNameComment.second.c_str(),(char*)desc.c_str());
  }
  msg << MSG::DEBUG << "New DimService: " + dimSvcName.second << endreq;
}

void MonitorSvc::declareInfo(const std::string& name, const long&  var, 
                             const std::string& desc, const IInterface* owner) 
{
  if (0 != m_disableDeclareInfoLong) return;

  MsgStream msg(msgSvc(),"MonitorSvc");
  MonObject *monObject=0;
  bool isMonObject = false;
  std::string prefix = "";
  if (m_disableMonObjectsForLong == 0) {
    isMonObject = true;
    monObject = MonObjectCreator::createMonObject(s_monLong, msgSvc(), "MonitorSvc");
    monObject->setComments(desc);
    ((MonLong*) monObject)->setValue(var);
    prefix = monObject->dimPrefix() + "/";
  }

  if (!registerName(name, owner)) return;
  std::pair<std::string, std::string> dimSvcName = registerDimSvc(name, prefix, owner, false);
  if ("" == dimSvcName.second) return;

  if (isMonObject) m_dimSrv[dimSvcName.first]=new DimServiceMonObject(dimSvcName.second, monObject);
  else {
    m_dimSrv[dimSvcName.first]= new DimService(dimSvcName.second.c_str(),(int&)var);
    std::pair<std::string, std::string> dimSvcNameComment = registerDimSvc(name, prefix, owner, true);
    if ("" != dimSvcNameComment.second) m_dimSrv[dimSvcNameComment.first]= new DimService(dimSvcNameComment.second.c_str(),(char*)desc.c_str());
  }
  msg << MSG::DEBUG << "New DimService: " + dimSvcName.second << endreq;
}

void MonitorSvc::declareInfo(const std::string& name, const double& var, 
                             const std::string& desc, const IInterface* owner) 
{
  if (0 != m_disableDeclareInfoDouble) return;

  MsgStream msg(msgSvc(),"MonitorSvc");
  if (name.find("COUNTER_TO_RATE") != std::string::npos) {
    std::string newName = extract("COUNTER_TO_RATE", name);
    if ( 0 == m_disableMonRate) {
      if (!m_monRateDeclared) {
        msg << MSG::DEBUG << "Declaring MonRate " << endreq; 
        if (!registerName("monRate", this)) return;
        msg << MSG::DEBUG << "Setting comments in MonRate " << endreq; 
        m_monRate->setComments("My name is MonRate. Nice to meet you !!");
        msg << MSG::DEBUG << "Registering MonRate " << endreq; 
        std::pair<std::string, std::string> dimSvcName = registerDimSvc("monRate", "MonR/", this, false);
        if ("" == dimSvcName.second) return;
        msg << MSG::DEBUG << "Printing MonRate " << endreq;
        m_monRate->print();
        msg << MSG::DEBUG << "Creating DimServiceMonObject for MonRate " << endreq;
        m_dimSrv[dimSvcName.first]=new DimServiceMonObject(dimSvcName.second, m_monRate);
        m_monRateDeclared = true;
      }
      msg << MSG::DEBUG << "Adding Counter to MonRate"<< newName << ", whit description: " << desc << endreq; 
      m_monRate->addCounter(newName, desc, var);
    }
    else msg << MSG::INFO << "Counter "<< newName << " can not be declared because MonRate process is disable." << endreq; 
    return;
  }

  MonObject *monObject=0;
  bool isMonObject = false;
  std::string prefix = "";
  if (m_disableMonObjectsForDouble == 0) {
    isMonObject = true;
    monObject = MonObjectCreator::createMonObject(s_monDouble, msgSvc(), "MonitorSvc");
    monObject->setComments(desc);
    ((MonDouble*) monObject)->setValue(var);
    prefix = monObject->dimPrefix() + "/";
  }

  if (!registerName(name, owner)) return;
  std::pair<std::string, std::string> dimSvcName = registerDimSvc(name, prefix, owner, false);
  if ("" == dimSvcName.second) return;

  if (isMonObject) m_dimSrv[dimSvcName.first]=new DimServiceMonObject(dimSvcName.second, monObject);
  else {
    m_dimSrv[dimSvcName.first]= new DimService(dimSvcName.second.c_str(),(double&)var);
    std::pair<std::string, std::string> dimSvcNameComment = registerDimSvc(name, prefix, owner, true);
    if ("" != dimSvcNameComment.second) m_dimSrv[dimSvcNameComment.first]= new DimService(dimSvcNameComment.second.c_str(),(char*)desc.c_str());
  }
  msg << MSG::DEBUG << "New DimService: " + dimSvcName.second << endreq;
}

void MonitorSvc::declareInfo(const std::string& name, const std::string& var, 
                             const std::string& desc, const IInterface* owner) 
{
  if (0 != m_disableDeclareInfoString) return;

  MsgStream msg(msgSvc(),"MonitorSvc");
  MonObject *monObject=0;
  bool isMonObject = false;
  std::string prefix = "";
  if (m_disableMonObjectsForString == 0) {
    isMonObject = true;
    monObject = MonObjectCreator::createMonObject(s_monString, msgSvc(), "MonitorSvc");
    monObject->setComments(desc);
    ((MonString*) monObject)->setValue(var);
    prefix = monObject->dimPrefix() + "/";
  }

  if (!registerName(name, owner)) return;
  std::pair<std::string, std::string> dimSvcName = registerDimSvc(name, prefix, owner, false);
  if ("" == dimSvcName.second) return;

  if (isMonObject) m_dimSrv[dimSvcName.first]=new DimServiceMonObject(dimSvcName.second, monObject);
  else {
    m_dimSrv[dimSvcName.first]= new DimService(dimSvcName.second.c_str(),(char*)var.c_str());
    std::pair<std::string, std::string> dimSvcNameComment = registerDimSvc(name, prefix, owner, true);
    if ("" != dimSvcNameComment.second) m_dimSrv[dimSvcNameComment.first]= new DimService(dimSvcNameComment.second.c_str(),(char*)desc.c_str());
  }
  msg << MSG::DEBUG << "New DimService: " + dimSvcName.second << endreq;
}

void MonitorSvc::declareInfo(const std::string& name, const std::pair<double,double>& var, 
                             const std::string& desc, const IInterface* owner) 
{
  if (0 != m_disableDeclareInfoPair) return;

  MsgStream msg(msgSvc(),"MonitorSvc");
  MonObject *monObject=0;
  bool isMonObject = false;
  std::string prefix = "";
  if (m_disableMonObjectsForPairs == 0) {
    isMonObject = true;
    monObject = MonObjectCreator::createMonObject(s_monPairDD, msgSvc(), "MonitorSvc");
    monObject->setComments(desc);
    ((MonPairDD*) monObject)->setValue(var);
    prefix = monObject->dimPrefix() + "/";
  }else {
    msg << MSG::ERROR << "MonitorSvc doesn't have support for declaring pairs without using MonObjects " << endreq;
    return;
  }

  if (!registerName(name, owner)) return;
  std::pair<std::string, std::string> dimSvcName = registerDimSvc(name, prefix, owner, false);
  if ("" == dimSvcName.second) return;

  m_dimSrv[dimSvcName.first]=new DimServiceMonObject(dimSvcName.second, monObject);
}

void MonitorSvc::declareInfo(const std::string& name, const std::string& format, const void* var,
                             int size, const std::string& desc, const IInterface* owner) 
{
  if (0 != m_disableDeclareInfoFormat) return;

  MsgStream msg(msgSvc(),"MonitorSvc");
  MonObject *monObject=0;
  bool isMonObject = false;
  std::string prefix = "";
  if ("MonObject" == format)
  {
    isMonObject = true;
    monObject = const_cast<MonObject *>((const MonObject*) var);
    monObject->setComments(desc);
    prefix = monObject->dimPrefix() + "/";
  }

  if (!registerName(name, owner)) return;
  std::pair<std::string, std::string> dimSvcName = registerDimSvc(name, prefix, owner, false);
  if ("" == dimSvcName.second) return;

  if (isMonObject) m_dimSrv[dimSvcName.first]=new DimServiceMonObject(dimSvcName.second, monObject);
  else {
    m_dimSrv[dimSvcName.first]= new DimService(dimSvcName.second.c_str(), (char*)format.c_str(), (void *)var, size);
    std::pair<std::string, std::string> dimSvcNameComment = registerDimSvc(name, prefix, owner, true);
    if ("" != dimSvcNameComment.second) m_dimSrv[dimSvcNameComment.first]= new DimService(dimSvcNameComment.second.c_str(),(char*)desc.c_str());
  }
  msg << MSG::DEBUG << "New DimService: " + dimSvcName.second << endreq;
}

void MonitorSvc::declareInfo(const std::string& name, const AIDA::IBaseHistogram* var, 
                             const std::string& desc, const IInterface* owner) 
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::DEBUG << "m_disableDeclareInfoHistos : " << m_disableDeclareInfoHistos << endreq;

  if (0 != m_disableDeclareInfoHistos) return;

  MonObject *monObject=0;
  bool isMonObject = false;
  std::string prefix = "";
  if (m_disableMonObjectsForHistos == 0) {
    isMonObject = true;
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
    monObject->setComments(desc);
    prefix = monObject->dimPrefix() + "/";
  }else {
    msg << MSG::ERROR << "MonitorSvc doesn't have support for declaring histograms/profiles without using MonObjects " << endreq;
    return;
  }

  if (!registerName(name, owner)) return;
  std::pair<std::string, std::string> dimSvcName = registerDimSvc(name, prefix, owner, false);
  if ("" == dimSvcName.second) return;

  m_dimSrv[dimSvcName.first]=new DimServiceMonObject(dimSvcName.second, monObject);

}

void MonitorSvc::declareMonRateComplement( int& runNumber, int& cycleNumber, double& deltaT, ulonglong& timeFirstEvInRun, ulonglong& timeLastEvInCycle, ulonglong& gpsTimeLastEvInCycle){
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::DEBUG << "Inside declareMonRateComplement" << endreq;

  if ( 0 == m_disableMonRate) {
    m_monRate->addComplement(&runNumber, &cycleNumber, &deltaT, &timeFirstEvInRun, &timeLastEvInCycle, &gpsTimeLastEvInCycle);
    m_monRate->print();
  }
  else  msg << MSG::INFO << "Complemet of MonRate can not be declared because MonRate process is disable." << endreq; 

}

std::string MonitorSvc::extract(const std::string mascara, std::string value){
  if (value.find(mascara) != std::string::npos) {
    value.erase(0, mascara.size()+1);
    value.erase(value.size() - 1, 1);
  }
  return value;
}

std::pair<std::string, std::string> MonitorSvc::registerDimSvc(const std::string& name, const std::string& dimPrefix, const IInterface* owner, bool isComment){
  MsgStream msg(msgSvc(),"MonitorSvc");
  std::string ownerName = infoOwnerName(owner);
  std::string dimName = name;
  if (dimName.find(ownerName) == std::string::npos) dimName = ownerName + "/" + dimName;
  if (isComment) dimName = dimName + "/gauchocomment";
  std::pair<DimServiceMapIt,bool> p = m_dimSrv.insert(DimServiceMap::value_type(dimName, 0));

  if (!p.second) {
    msg << MSG::ERROR << "Already existing " + dimName << endreq;
    return std::pair<std::string, std::string> ("", "");
  }

  std::string dimSvcName = dimPrefix + m_utgid + "/"+dimName;

  return std::pair<std::string, std::string> (dimName, dimSvcName);
}

bool MonitorSvc::registerName(const std::string& name, const IInterface* owner) 
{
  MsgStream msg(msgSvc(),"MonitorSvc");

  //msg << MSG::DEBUG << "declareInfoMonObject: " << endreq;
  m_InfoNamesMapIt = m_InfoNamesMap.find(owner);
  std::string ownerName = infoOwnerName(owner);
  if( m_InfoNamesMapIt != m_InfoNamesMap.end()) {
    std::pair<std::set<std::string>::iterator,bool> p = (*m_InfoNamesMapIt).second.insert(name);
    if( p.second) msg << MSG::DEBUG << "Declaring info: Owner: " << ownerName << " Name: " << name << endreq;
    else 
    { // Insertion failed: Name already exists
      msg << MSG::ERROR << "Already existing info " << name << " from owner " << ownerName << " not published" << endreq;
      return false;
    }
  }
  else { // Create a new set for this algo and insert name
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(name);
  }
  return true;
}

void MonitorSvc::undeclareInfo( const std::string& name, const IInterface* owner )
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  std::set<std::string> * infoNamesSet = getInfos( owner );
  if( 0 == infoNamesSet ) {
    msg << MSG::DEBUG << "undeclareInfo: Info  " << name 
        << ": No info to undeclare for " << infoOwnerName(owner) 
        << ". Empty set" << endreq;
    return;
  }
  std::string ownerName = infoOwnerName( owner );
  if( (*infoNamesSet).find( name) !=  (*infoNamesSet).end() ){

    std::string dimName = name;
    if (dimName.find(ownerName) == std::string::npos) dimName = ownerName + "/" + dimName;
    undeclService( dimName ) ;
    undeclService( dimName + "/gauchocomment") ;
    (*infoNamesSet).erase(name);
    msg << MSG::DEBUG << "undeclareInfo: " << name << " from owner " 
        << ownerName  << " undeclared" << endreq;
  }  
  else{
    msg << MSG::DEBUG << "undeclareInfo: Info  " << name << " declared by " 
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
    msg << MSG::DEBUG << "update: Info  " << name << ": Nothing to update for " << infoOwnerName(owner) << ". Empty set" << endreq;
    return;
  }
  std::string ownerName = infoOwnerName( owner );
  if( (*infoNamesSet).find( name) !=  (*infoNamesSet).end() ){
    std::string dimName = name;
    if (dimName.find(ownerName) == std::string::npos) dimName = ownerName + "/" + dimName;
    updateService( dimName, endOfRun ) ;
    //msg << MSG::DEBUG << "update: " << name << " from owner " << ownerName  << " updated" << endreq;
  }  
  else{
    msg << MSG::DEBUG << "update: Info  " << name << " declared by " << infoOwnerName(owner) << " not found" << endreq;
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
      msg << MSG::DEBUG << "undeclareAll: No infos to  undeclare for " 
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
      undeclService( dimName ) ;
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
      msg << MSG::DEBUG << "updateAll: No infos to update for " << ownerName << endreq;
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
      updateService( dimName, endOfRun ) ;
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
      msg << MSG::DEBUG << "resethistos: No histograms to reset for " 
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

void MonitorSvc::undeclService(std::string infoName)
{
  MsgStream msg(msgSvc(),"MonitorSvc");

  for (m_dimSrvIt = m_dimSrv.begin(); m_dimSrvIt != m_dimSrv.end(); m_dimSrvIt++)
      msg << MSG::DEBUG << (*m_dimSrvIt).first << endreq;
  
  m_dimSrvIt = m_dimSrv.find(infoName);

  if(m_dimSrvIt != m_dimSrv.end()) {
    delete (*m_dimSrvIt).second;
    m_dimSrv.erase(m_dimSrvIt);
    msg << MSG::DEBUG << "undeclSvc: Service " + infoName + " undeclared" << endreq;
    return;
  }
  if (infoName.find("gauchocomment") == std::string::npos) return;

  msg << MSG::ERROR << "undeclSvc: No DimService found with the name:" + infoName << endreq;
}

void MonitorSvc::updateService(std::string infoName, bool endOfRun)
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  //for (m_dimSrvIt = m_dimSrv.begin(); m_dimSrvIt != m_dimSrv.end(); m_dimSrvIt++)
  //  mes << MSG::DEBUG << (*m_dimSrvIt).first << endreq;
  m_dimSrvIt = m_dimSrv.find(infoName);
  if(m_dimSrvIt != m_dimSrv.end()) {
    DimServiceMonObject* dimSvcMO = dynamic_cast<DimServiceMonObject*>((*m_dimSrvIt).second);
    if (0 != dimSvcMO) dimSvcMO->updateService(endOfRun);
    else (*m_dimSrvIt).second->updateService(); //THIS IS WRONG
    //msg << MSG::DEBUG << "updateSvc: Service " + infoName + " updated" << endreq;
    return;
  }
  msg << MSG::DEBUG << "updateSvc: No DimServiceMonObject found with the name:" + infoName << endreq;
}

