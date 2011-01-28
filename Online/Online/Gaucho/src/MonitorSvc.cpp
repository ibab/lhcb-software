// Include files
//#include "/usr/include/valgrind/callgrind.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
//#include "Gaucho/DimServiceMonObject.h"
//#include "Gaucho/Misc.h"
//#include "Gaucho/MonObject.h"
//#include "Gaucho/MonInt.h"
//#include "Gaucho/MonDouble.h"
//#include "Gaucho/MonLong.h"
//#include "Gaucho/MonFloat.h"
//#include "Gaucho/MonBool.h"
//#include "Gaucho/MonPairDD.h"
//#include "Gaucho/MonPairDI.h"
//#include "Gaucho/MonPairII.h"
//#include "Gaucho/MonString.h"
//#include "Gaucho/MonProfile.h"
//#include "Gaucho/MonH1D.h"
//#include "Gaucho/MonH2D.h"
//#include "Gaucho/MonH1F.h"
//#include "Gaucho/MonH2F.h"
//#include "Gaucho/MonVectorI.h"
//#include "Gaucho/MonVectorD.h"
//#include "Gaucho/MonObjectCreator.h"
#include "Gaucho/CntrMgr.h"
//#include "Gaucho/MonStatEntity.h"
#include "Gaucho/MonHist.h"
#include "MonitorSvc.h"
#include "Gaucho/MonSys.h"
#include "Gaucho/MonSubSys.h"
#include "Gaucho/MonCounter.h"
//#include "DimPropServer.h"
//#include "DimRpcGaucho.h"
//#include "DimCmdServer.h"
#include "RTL/rtl.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

#include "TH1D.h"
#include "TProfile.h"
#include "TH2D.h"

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
  m_started = false;
  m_MonSys =   &MonSys::m_instance();
  m_HistSubSys = 0;
  this->m_CntrSubSys = 0;
  m_CntrMgr = 0;
  declareProperty("UniqueServiceNames", m_uniqueServiceNames = 0);
  declareProperty("disableMonRate", m_disableMonRate = 0);
  declareProperty("disableDimPropServer", m_disableDimPropServer = 0);
  declareProperty("disableDimCmdServer", m_disableDimCmdServer = 0);
  // declareProperty("disableDimRcpGaucho", m_disableDimRcpGaucho = 0);

  declareProperty("disableMonObjectsForBool", m_disableMonObjectsForBool = 1);
  declareProperty("disableMonObjectsForInt", m_disableMonObjectsForInt = 0);
  declareProperty("disableMonObjectsForLong", m_disableMonObjectsForLong = 0);
  declareProperty("disableMonObjectsForDouble", m_disableMonObjectsForDouble = 0);
  declareProperty("disableMonObjectsForString", m_disableMonObjectsForString = 1);
  declareProperty("disableMonObjectsForPairs", m_disableMonObjectsForPairs = 1);
  declareProperty("disableMonObjectsForHistos", m_disableMonObjectsForHistos = 0);

  declareProperty("disableDeclareInfoBool", m_disableDeclareInfoBool = 1);
  declareProperty("disableDeclareInfoInt", m_disableDeclareInfoInt = 0);
  declareProperty("disableDeclareInfoLong", m_disableDeclareInfoLong = 0);
  declareProperty("disableDeclareInfoDouble", m_disableDeclareInfoDouble = 0);
  declareProperty("disableDeclareInfoString", m_disableDeclareInfoString = 1);
  declareProperty("disableDeclareInfoPair", m_disableDeclareInfoPair = 1);
  declareProperty("disableDeclareInfoFormat", m_disableDeclareInfoFormat = 1);
  declareProperty("disableDeclareInfoHistos", m_disableDeclareInfoHistos = 0);
  declareProperty("maxNumCountersMonRate", m_maxNumCountersMonRate = 1000);
  declareProperty("DimUpdateInterval", m_updateInterval = 10);
  TH1D::SetDefaultSumw2();
  TH2D::SetDefaultSumw2();
  TProfile::SetDefaultSumw2();
//   declareProperty("teste", m_teste);

//  MsgStream msg(msgSvc(),"MonitorSvc");
}


MonitorSvc::~MonitorSvc()
{
  if (m_CntrMgr != 0)
  {
    delete m_CntrMgr;
    m_CntrMgr = 0;
  }
}
//Query interfaces of Interface
// @param riid       ID of Interface to be retrieved
// @param ppvUnknown Pointer to Location for interface pointer
StatusCode MonitorSvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
  if(IMonitorSvc::interfaceID().versionMatch(riid)) {
    *ppvIF = dynamic_cast<IMonitorSvc*> (this);
  }
  else if (IGauchoMonitorSvc::interfaceID().versionMatch(riid)) {
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
  StatusCode sc = Service::initialize();

 // msg << MSG::DEBUG << "Initialize=====>m_disableDeclareInfoHistos : " << m_disableDeclareInfoHistos << endreq;
  msg << MSG::INFO << "Initialize=====>m_uniqueServiceNames : " << m_uniqueServiceNames << endreq;

  //const std::string& utgid = RTL::processName();
//  m_utgid = RTL::processName();
//  m_MonSys->setup((char*)m_utgid.c_str());
//  m_MonSys->start();
//  msg << MSG::DEBUG << "initialize: Setting up DIM for UTGID " << m_utgid << endreq;

/*  if ( 0 == m_disableDimRcpGaucho) {
    m_dimRpcGaucho = new DimRpcGaucho(m_utgid+"/GauchoRpc", serviceLocator());
    m_dimRpcGaucho->setUtgId(m_utgid);
    msg << MSG::DEBUG << "DimRpcGaucho created with name " << m_utgid+"/GauchoRpc" << endreq;
  }
  else msg << MSG::DEBUG << "DimRpcGaucho process is disabled." << endreq;*/


//  if ( 0 == m_disableDimPropServer) {
//    m_dimpropsvr= new DimPropServer(m_utgid, serviceLocator());
//    msg << MSG::DEBUG << "DimPropServer created with name " << m_utgid << endreq;
//  }
//  else msg << MSG::DEBUG << "DimPropServer process is disabled." << endreq;
//
//  if ( 0 == m_disableDimCmdServer) {
//    m_dimcmdsvr = new DimCmdServer( (m_utgid+"/"), serviceLocator(), this);
//    msg << MSG::DEBUG << "DimCmdServer created with name " << (m_utgid+"/") << endreq;
//  }
//  else msg << MSG::DEBUG << "DimCmdServer process is disabled." << endreq;

  if ( 0 == m_disableMonRate) {
//    msg << MSG::DEBUG << "new MonRate " << endreq;
//    m_monRate = new MonRate(msgSvc(), "MonitorSvc", 0);
//    m_monRate->setMaxNumCounters(m_maxNumCountersMonRate);
//    m_monRateDeclared = false;
//    msg << MSG::DEBUG << "End of new MonRate Information" << endreq;
  }
  else  msg << MSG::DEBUG << "MonRate process is disabled." << endreq;

  //DimServer::start(m_utgid.c_str());

  return StatusCode::SUCCESS;
}


StatusCode MonitorSvc::finalize()
{

  //dim_lock();
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::DEBUG << "MonitorSvc Destructor" << endreq;
  if (m_HistSubSys != 0)
  {
    delete m_HistSubSys;
    m_HistSubSys = 0;
  }
  if (m_CntrSubSys!=0)
  {
    delete m_CntrSubSys;
    m_CntrSubSys = 0;
  }
  if (m_CntrMgr !=0)
  {
    delete m_CntrMgr;
    this->m_CntrMgr = 0;
  }
//  m_InfoNamesMap.clear();
/*  if ( 0 == m_disableDimRcpGaucho){
    msg << MSG::DEBUG << "delete m_dimRcpGaucho" << endreq;
    if (m_dimRpcGaucho) delete m_dimRpcGaucho;  m_dimRpcGaucho = 0;
  }*/
//  if ( 0 == m_disableDimCmdServer){
//    msg << MSG::DEBUG << "delete m_dimcmdsvr" << endreq;
//    if (m_dimcmdsvr) delete m_dimcmdsvr;  m_dimcmdsvr = 0;
//  }
//  if ( 0 == m_disableDimPropServer){
//    msg << MSG::DEBUG << "delete m_dimpropsvr" << endreq;
//    if (m_dimpropsvr) delete m_dimpropsvr; m_dimpropsvr = 0;
//  }
//
//  if ( 0 == m_disableMonRate){
//    msg << MSG::DEBUG << "delete m_monRate" << endreq;
//    //if (m_monRate) delete m_monRate; m_monRate = 0;
//  }
  //dim_unlock();
  msg << MSG::DEBUG << "finalized successfully" << endreq;

  return StatusCode::SUCCESS;
}

void MonitorSvc::declareInfo(const std::string& name, const bool&  ,
                             const std::string& , const IInterface* )
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::ERROR << "declareInfo bool not implemented " << name << endreq;
  return;
//  if (0 != m_disableDeclareInfoBool) return;
//
//  MsgStream msg(msgSvc(),"MonitorSvc");
//  MonObject *monObject = 0;
//  bool isMonObject = false;
//  std::string prefix = "";
//  if (m_disableMonObjectsForBool == 0) {
//    isMonObject = true;
//    monObject = MonObjectCreator::createMonObject(s_monBool, msgSvc(), "MonitorSvc");
//    monObject->setComments(desc);
//    ((MonBool*) monObject)->setValue(var);
//    prefix = monObject->dimPrefix() + "/";
//  }
//
//  if (!registerName(name, owner)) return;
//  std::pair<std::string, std::string> dimSvcName = registerDimSvc(name, prefix, owner, false);
//  if (dimSvcName.second.compare("") == 0) return;
//
//  if (isMonObject) m_dimSrv[dimSvcName.first]=new DimServiceMonObject(dimSvcName.second, monObject);
//  else {
//    std::string formatstr="C:1";
//    m_dimSrv[dimSvcName.first]= new DimService((char*)dimSvcName.second.c_str(), (char*)formatstr.c_str(), (void*)&var, sizeof(bool));
//    std::pair<std::string, std::string> dimSvcNameComment = registerDimSvc(name, prefix, owner, true);
//    if ("" != dimSvcName.second) m_dimSrv[dimSvcNameComment.first]= new DimService((char*)dimSvcNameComment.second.c_str(),(char*)desc.c_str());
//  }
//  msg << MSG::DEBUG << "New DimService: " + dimSvcName.second << endreq;
}
//
void MonitorSvc::declareInfo(const std::string& name, const int&  var,
                             const std::string& desc, const IInterface* )
{
  if (m_started)
  {
//    printf("Delcare Info called after start for Name %s\n",name.c_str());
  }
  if (0 != m_disableDeclareInfoInt) return;
  MsgStream msg(msgSvc(),"MonitorSvc");
  if (name.find("COUNTER_TO_RATE") != std::string::npos)
  {
    std::string newName = extract("COUNTER_TO_RATE", name);
    if ( 0 == m_disableMonRate)
    {
      if (m_CntrMgr == 0)
      {
        m_CntrMgr = new CntrMgr(msgSvc(),"MonitorSvc",0);
      }
      m_CntrMgr->addCounter(newName,desc,var);
      if (m_HistSubSys == 0)
      {
        m_HistSubSys = new MonSubSys(m_updateInterval);
//        m_MonSys->addSubSys(m_HistSubSys);
//        m_HistSubSys->setup((char*)"Histos");
//        m_HistSubSys->start();
      }
      addRate(m_HistSubSys,m_CntrMgr);
    }
    else
    {
      msg << MSG::INFO << "Counter "<< newName << " can not be declared because MonRate process is disabled." << endreq;
    }
    return;
  }
  MonCounter *cnt = new MonCounter((char*)name.c_str(),(char*)desc.c_str(),(int*)&var);
  if (m_CntrSubSys == 0)
  {
    m_CntrSubSys = new MonSubSys(m_updateInterval);
//    m_MonSys->addSubSys(m_CntrSubSys);
//    m_CntrSubSys->setup((char*)"Counter");
//    m_CntrSubSys->start();

  }
  m_CntrSubSys->addObj(cnt);
}

void MonitorSvc::declareInfo(const std::string& name, const long&  var,
                             const std::string& desc, const IInterface* )
{
  if (m_started)
  {
//    printf("Delcare Info called after start for Name %s\n",name.c_str());
  }
  if (0 != m_disableDeclareInfoLong) return;
  MsgStream msg(msgSvc(),"MonitorSvc");
  if (name.find("COUNTER_TO_RATE") != std::string::npos)
  {
    std::string newName = extract("COUNTER_TO_RATE", name);
    if ( 0 == m_disableMonRate)
    {
      if (m_CntrMgr == 0)
      {
        m_CntrMgr = new CntrMgr(msgSvc(),"MonitorSvc",0);
      }
      m_CntrMgr->addCounter(newName,desc,var);
      if (m_HistSubSys == 0)
      {
        m_HistSubSys = new MonSubSys(m_updateInterval);
//        m_MonSys->addSubSys(m_HistSubSys);
//        m_HistSubSys->setup((char*)"Histos");
//        m_HistSubSys->start();
      }
      addRate(m_HistSubSys,m_CntrMgr);
    }
    else
    {
      msg << MSG::INFO << "Counter "<< newName << " can not be declared because MonRate process is disabled." << endreq;
    }
    return;
  }

  MonCounter *cnt = new MonCounter((char*)name.c_str(),(char*)desc.c_str(),(long long*)&var);
  if (m_CntrSubSys == 0)
  {
    m_CntrSubSys = new MonSubSys(m_updateInterval);
//    m_MonSys->addSubSys(m_CntrSubSys);
//    m_CntrSubSys->setup((char*)"Counter");
//    m_CntrSubSys->start();
  }
  m_CntrSubSys->addObj(cnt);
}
void MonitorSvc::declareInfo(const std::string& name, const double& var,
                             const std::string& desc, const IInterface* )
{
  if (m_started)
  {
//    printf("Delcare Info called after start for Name %s\n",name.c_str());
  }
  if (0 != m_disableDeclareInfoDouble) return;
  MsgStream msg(msgSvc(),"MonitorSvc");
  if (name.find("COUNTER_TO_RATE") != std::string::npos)
  {
    std::string newName = extract("COUNTER_TO_RATE", name);
    if ( 0 == m_disableMonRate)
    {
      if (m_CntrMgr == 0)
      {
        m_CntrMgr = new CntrMgr(msgSvc(),"MonitorSvc",0);
      }
      m_CntrMgr->addCounter(newName,desc,var);
      if (m_HistSubSys == 0)
      {
        m_HistSubSys = new MonSubSys(m_updateInterval);
//        m_MonSys->addSubSys(m_HistSubSys);
//        m_HistSubSys->setup((char*)"Histos");
//        m_HistSubSys->start();
      }
      addRate(m_HistSubSys,m_CntrMgr);
    }
    else
    {
      msg << MSG::INFO << "Counter "<< newName << " can not be declared because MonRate process is disabled." << endreq;
    }
    return;
  }
  MonCounter *cnt = new MonCounter((char*)name.c_str(),(char*)desc.c_str(),(double*)&var);
  if (m_CntrSubSys == 0)
  {
    m_CntrSubSys = new MonSubSys(m_updateInterval);
//    m_MonSys->addSubSys(m_CntrSubSys);
//    m_CntrSubSys->setup((char*)"Counter");
//    m_CntrSubSys->start();
  }
  m_CntrSubSys->addObj(cnt);
}
void MonitorSvc::addRate(MonSubSys *ss, CntrMgr *cm)
{
//  MsgStream msg(msgS(),"MonitorSvc");
  MonHist *h = (MonHist*)ss->findobj("COUNTERRATE");
  if (h == 0)
  {
    h = new MonHist(msgSvc(),"COUNTERRATE",cm);
    ss->addObj(h);
  }
  return;
}
void MonitorSvc::declareInfo(const std::string& name, const std::string& ,
                             const std::string& , const IInterface* )
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::ERROR << "declareInfo for strings not implemented" + name << endreq;
  return;
}
//  MonObject *monObject=0;
//  bool isMonObject = false;
//  std::string prefix = "";
//  if (m_disableMonObjectsForString == 0) {
//    isMonObject = true;
//    monObject = MonObjectCreator::createMonObject(s_monString, msgSvc(), "MonitorSvc");
//    monObject->setComments(desc);
//    ((MonString*) monObject)->setValue(var);
//    prefix = monObject->dimPrefix() + "/";
//  }
//
//  if (!registerName(name, owner)) return;
//  std::pair<std::string, std::string> dimSvcName = registerDimSvc(name, prefix, owner, false);
//  if (dimSvcName.second.compare("") == 0) return;
//
//  if (isMonObject) m_dimSrv[dimSvcName.first]=new DimServiceMonObject(dimSvcName.second, monObject);
//  else {
//    m_dimSrv[dimSvcName.first]= new DimService((char*)dimSvcName.second.c_str(),(char*)var.c_str());
//    std::pair<std::string, std::string> dimSvcNameComment = registerDimSvc(name, prefix, owner, true);
//      if (dimSvcName.second.compare("") != 0) m_dimSrv[dimSvcNameComment.first]= new DimService((char*)dimSvcNameComment.second.c_str(),(char*)desc.c_str());
//  }
//  msg << MSG::DEBUG << "New DimService: " + dimSvcName.second << endreq;
//}
//
void MonitorSvc::declareInfo(const std::string& name, const std::pair<double,double>& ,
                             const std::string& , const IInterface* )
{
  if (0 != m_disableDeclareInfoPair) {};

  if (m_started)
  {
//    printf("Delcare Info called after start for Name %s\n",name.c_str());
  }
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::ERROR << "MonitorSvc doesn't have support for declaring pairs " << name << endreq;
  return;
//  MonObject *monObject=0;
//  bool isMonObject = false;
//  std::string prefix = "";
//  if (m_disableMonObjectsForPairs == 0) {
//    isMonObject = true;
//    monObject = MonObjectCreator::createMonObject(s_monPairDD, msgSvc(), "MonitorSvc");
//    monObject->setComments(desc);
//    ((MonPairDD*) monObject)->setValue(var);
//    prefix = monObject->dimPrefix() + "/";
//  }else {
//    msg << MSG::ERROR << "MonitorSvc doesn't have support for declaring pairs without using MonObjects " << endreq;
//    return;
//  }
//
//  if (!registerName(name, owner)) return;
//  std::pair<std::string, std::string> dimSvcName = registerDimSvc(name, prefix, owner, false);
//  if (dimSvcName.second.compare("") == 0) return;
//
//  m_dimSrv[dimSvcName.first]=new DimServiceMonObject(dimSvcName.second, monObject);
}
//
void MonitorSvc::declareInfo(const std::string& name, const std::string& , const void* ,
                             int , const std::string& , const IInterface* )
{
//  if (0 != m_disableDeclareInfoFormat) return;

  if (m_started)
  {
//    printf("Delcare Info called after start for Name %s\n",name.c_str());
  }
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::ERROR << "Declare Format not implemented " << name << endreq;
  return;

}
//
void MonitorSvc::declareInfo(const std::string& name, const StatEntity& var,
                             const std::string& desc, const IInterface* )
{
  if (m_started)
  {
//    printf("Delcare Info called after start for Name %s\n",name.c_str());
  }
  MsgStream msg(msgSvc(),"MonitorSvc");
   msg << MSG::INFO << "=========================== DeclareInfo for StatEntity "<< name << endreq;
  if (name.find("COUNTER_TO_RATE") != std::string::npos)
  {
    std::string newName = extract("COUNTER_TO_RATE", name);
    //make newName unique!
    if ( 0 == m_disableMonRate)
    {
      if (m_CntrMgr == 0)
      {
        m_CntrMgr = new CntrMgr(msgSvc(),"MonitorSvc",0);
      }
      m_CntrMgr->addCounter(newName,desc,var);
      if (m_HistSubSys == 0)
      {
        m_HistSubSys = new MonSubSys(m_updateInterval);
//        m_MonSys->addSubSys(m_HistSubSys);
//        m_HistSubSys->setup((char*)"Histos");
//        m_HistSubSys->start();
      }
      addRate(m_HistSubSys,m_CntrMgr);
    }
    else
    {
      msg << MSG::INFO << "Counter "<< newName << " can not be declared because MonRate process is disabled." << endreq;
    }
    return;
  }
  else
  {
    if (m_HistSubSys == 0)
    {
      m_HistSubSys = new MonSubSys(m_updateInterval);
  //    m_MonSys->addSubSys(m_HistSubSys);
//      m_HistSubSys->setup((char*)"Histos");
  //    m_HistSubSys->start();
    }
    MonHist *m = new MonHist(msgSvc(),name,desc,&var);
    m_HistSubSys->addObj(m);
  }
  return;
}


void MonitorSvc::declareInfo(const std::string& name, const AIDA::IBaseHistogram* var,
                             const std::string& , const IInterface* )
{
  MsgStream msg(msgSvc(),"MonitorSvc");
 // msg << MSG::INFO << "m_disableDeclareInfoHistos : " << m_disableDeclareInfoHistos << endreq;

  if (m_started)
  {
//    printf("Declare Info (Histogram) called after start for Name %s\n",name.c_str());
  }
  if (0 != m_disableDeclareInfoHistos)
  {
    msg << MSG::INFO << "m_disableDeclareInfoHistos DISABLED " << endreq;
    return;
  }
  if( 0 != dynamic_cast<const AIDA::IProfile1D* >(var) )
  {
  }
  else if( 0 != dynamic_cast<const AIDA::IHistogram1D * >(var) )
  {
  }
  else if( 0 != dynamic_cast<const AIDA::IHistogram2D * >(var) )
  {
  }
  else
  {
    msg << MSG::ERROR << "Unknown histogram type. Source " << name << endreq;
    return;
  }
  if (m_HistSubSys == 0)
  {
    m_HistSubSys = new MonSubSys(m_updateInterval);
//    m_MonSys->addSubSys(m_HistSubSys);
//    m_HistSubSys->setup((char*)"Histos");
//    m_HistSubSys->start();
  }

  MonHist *mhist;
  if (m_disableMonObjectsForHistos == 0)
  {
//    isMonObject = true;
    mhist = new MonHist(msgSvc(),name,var);
    m_HistSubSys->addObj(mhist);
  }
  else
  {
    msg << MSG::ERROR << "MonitorSvc doesn't have support for declaring histograms/profiles without using MonObjects " << endreq;
    return;
  }

}

void MonitorSvc::declareMonRateComplement( int& runNumber, unsigned int& tck, int& cycleNumber, double& deltaT, double& offsetTimeFirstEvInRun, double& offsetTimeLastEvInCycle, double& offsetGpsTimeLastEvInCycle)
{
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::DEBUG << "Inside declareMonRateComplement" << endreq;

  if ( 0 == m_disableMonRate)
  {
    if (m_CntrMgr == 0)
    {
      m_CntrMgr = new CntrMgr(msgSvc(),"MonitorSvc",0);
    }
//    printf("Complement Addresses: %X %X %X %X %X %X %X\n",
//        &runNumber, &tck, &cycleNumber, &deltaT, &offsetTimeFirstEvInRun, &offsetTimeLastEvInCycle, &offsetGpsTimeLastEvInCycle);
    m_CntrMgr->addComplement(&runNumber, &tck, &cycleNumber, &deltaT, &offsetTimeFirstEvInRun, &offsetTimeLastEvInCycle, &offsetGpsTimeLastEvInCycle);
    if (m_HistSubSys == 0)
    {
      m_HistSubSys = new MonSubSys(m_updateInterval);
  //    m_MonSys->addSubSys(m_HistSubSys);
//      m_HistSubSys->setup((char*)"Histos");
  //    m_HistSubSys->start();
    }
    m_HistSubSys->m_updateTimer->setExtLastDelta((unsigned long long *)&deltaT);
  }
  else  msg << MSG::INFO << "Complement of MonRate was not declared because MonRate process is disabled." << endreq;

}

std::string MonitorSvc::extract(const std::string mascara, std::string value){
  if (value.find(mascara) != std::string::npos) {
    value.erase(0, mascara.size()+1);
    value.erase(value.size() - 1, 1);
  }
  return value;
}

//std::pair<std::string, std::string> MonitorSvc::registerDimSvc(const std::string& name, const std::string& dimPrefix, const IInterface* owner, bool isComment)
//{
//  MsgStream msg(msgSvc(),"MonitorSvc");
//  std::string ownerName = infoOwnerName(owner);
//  std::string dimName = name;
//  if (dimName.find(ownerName) == std::string::npos) dimName = ownerName + "/" + dimName;
//  if (isComment) dimName = dimName + "/gauchocomment";
//  std::pair<DimServiceMapIt,bool> p = m_dimSrv.insert(DimServiceMap::value_type(dimName, 0));
//
//
//
//  if (!p.second) {
//    msg << MSG::ERROR << "Already existing " + dimName << endreq;
//    return std::pair<std::string, std::string> ("", "");
//  }
//  //when in monitoring farm, we replace the nodename in the utgid by x if dimPrefix=='' (counters only)
//  //this is so that the trendtool stay subscribed to the same dimservice
//  std::string dimSvcName ="";
//  std::vector<std::string> utgidParts = Misc::splitString(m_utgid, "_");
//  if ((utgidParts.size() == 4)) {
//    if (m_uniqueServiceNames==1) {
//      //this is for the storage system
//      dimSvcName = dimPrefix + m_utgid + "/"+dimName;
//    }
//    else { if (dimPrefix=="") dimSvcName = utgidParts[0]+"_x_"+utgidParts[2] +"_"+utgidParts[3]+ "/"+dimName;
//           else  dimSvcName = dimPrefix + m_utgid + "/"+dimName;
//    }
//  }
//  else {
//    if (utgidParts[0]=="CALD0701") {
//       //calibrationfarm - add partition
//       if (dimPrefix!="") {
//          if (utgidParts[2]=="1") dimSvcName = dimPrefix+"LHCb_CALD0701_CaloDAQCalib_1/"+dimName;
//	  else  dimSvcName = dimPrefix+"LHCb_"+m_utgid + "/"+dimName;
//       }
//       else dimSvcName = dimPrefix + m_utgid + "/"+dimName;
//    }
//    else {
//       dimSvcName = dimPrefix + m_utgid + "/"+dimName;
//    }
//  }
//
//  //msg << MSG::INFO << " register========>dimSvcName="<< dimSvcName << endreq;
//
//  return std::pair<std::string, std::string> (dimName, dimSvcName);
//}

bool MonitorSvc::registerName(const std::string& name, const IInterface* owner)
{
  MsgStream msg(msgSvc(),"MonitorSvc");

  msg << MSG::DEBUG << "registerName: " << name << endreq;
  m_InfoNamesMapIt = m_InfoNamesMap.find(owner);
  std::string ownerName = infoOwnerName(owner);
  if( m_InfoNamesMapIt != m_InfoNamesMap.end()) {
    std::pair<std::set<std::string>::iterator,bool> p = (*m_InfoNamesMapIt).second.insert(name);
    if( p.second) {
      msg << MSG::DEBUG << "Declaring info: Owner: " << ownerName << " Name: " << name << endreq;
    }
    else
    { // Insertion failed: Name already exists
      msg << MSG::ERROR << "Already existing info " << name << " from owner " << ownerName << " not published" << endreq;
      return false;
    }
  }
  else { // Create a new set for this algo and insert name
    msg << MSG::DEBUG << "registerName: creating new map for owner: "<< ownerName << " name: " << name << endreq;
    m_InfoNamesMap[owner]=std::set<std::string>();
    m_InfoNamesMap[owner].insert(name);
  }
  return true;
}

//void MonitorSvc::undeclareInfo( const std::string& name, const IInterface* owner )
//{
//  MsgStream msg(msgSvc(),"MonitorSvc");
//  std::set<std::string> * infoNamesSet = getInfos( owner );
//  if( 0 == infoNamesSet ) {
//    msg << MSG::DEBUG << "undeclareInfo: Info  " << name
//        << ": No info to undeclare for " << infoOwnerName(owner)
//        << ". Empty set" << endreq;
//    return;
//  }
//  std::string ownerName = infoOwnerName( owner );
//  if( (*infoNamesSet).find( name) !=  (*infoNamesSet).end() ){
//
//    std::string dimName = name;
//    if (dimName.find(ownerName) == std::string::npos) dimName = ownerName + "/" + dimName;
//    undeclService( dimName ) ;
//    undeclService( dimName + "/gauchocomment") ;
//    (*infoNamesSet).erase(name);
//    msg << MSG::DEBUG << "undeclareInfo: " << name << " from owner "
//        << ownerName  << " undeclared" << endreq;
//  }
//  else{
//    msg << MSG::DEBUG << "undeclareInfo: Info  " << name << " declared by "
//        << infoOwnerName(owner) << " not found" << endreq;
//    msg << MSG::DEBUG << infoOwnerName(owner) << " infoNames: " << endreq;
//    for( std::set<std::string>::iterator infoNamesIt = (*infoNamesSet).begin();
//         infoNamesIt!=(*infoNamesSet).end();++infoNamesIt)
//      msg << MSG::DEBUG << "\t" <<  (*infoNamesIt) << endreq;
//  }
//}

//std::set<std::string> * MonitorSvc::getInfos(const IInterface* owner )
//{
//  m_InfoNamesMapIt = m_InfoNamesMap.find( owner );
//  if( m_InfoNamesMapIt != m_InfoNamesMap.end() )
//    return &(m_InfoNamesMapIt->second);
//  else {
//    return 0;
//  }
//}

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

StatusCode MonitorSvc::start()
{
//  CALLGRIND_START_INSTRUMENTATION
  MsgStream msg(msgSvc(),"MonitorSvc");
  msg << MSG::INFO << "======== MonitorSvc start() called ============= " << endreq;
  if (m_CntrMgr != 0)
  {
//    printf("In STARTS Method... Counter Manager present... Closing it...\n");
    this->m_CntrMgr->close();
  }
  m_utgid = RTL::processName();
  if (m_MonSys != 0) m_MonSys->setup((char*)m_utgid.c_str());
  if (m_CntrSubSys != 0) m_CntrSubSys->setup((char*)"Counter");
  if (m_HistSubSys != 0) m_HistSubSys->setup((char*)"Histos");
  if (this->m_HistSubSys != 0)
  {
    if (m_CntrMgr != 0)
    {
      MonHist *h = (MonHist*)m_HistSubSys->findobj("COUNTERRATE");
      if (h == 0)
      {
        h = new MonHist(msgSvc(),"COUNTERRATE",m_CntrMgr);
        m_HistSubSys->addObj(h);
      }
      h->makeCounters();
    }
  }
  this->m_MonSys->List();
  this->m_MonSys->start();
  m_started = true;
  return StatusCode::SUCCESS;
}
StatusCode MonitorSvc::stop()
{
//  CALLGRIND_STOP_INSTRUMENTATION
//  printf("+++++++++++++++++++++++++++ Monitor Service STOP called\n");
  m_started = false;
//  updateSvc("",m_runno,0);
  if (m_CntrMgr != 0)
  {
    this->m_CntrMgr->open();
  }
  m_MonSys->stop();
  return StatusCode::SUCCESS;
}

//updateSvc and resetHistos methods are for fast run changes
void MonitorSvc::setRunNo(int runno)
{
  m_runno = runno;
  this->m_MonSys->setRunNo(runno);
  return;
}

void MonitorSvc::updateSvc( const std::string& , int runno, const IInterface*  )
{
//  printf("Updating EOR service....");
  this->m_MonSys->EORUpdate(runno);
}
//
//void MonitorSvc::undeclareAll( const IInterface* owner)
//{
//  MsgStream msg(msgSvc(),"MonitorSvc");
//  if( 0!=owner ){
//    std::string ownerName = infoOwnerName( owner );
//    std::set<std::string> * infoNamesSet = getInfos( owner );
//    if( 0 == infoNamesSet ) {
//      msg << MSG::DEBUG << "undeclareAll: No infos to  undeclare for "
//          << ownerName << endreq;
//      return;
//    }
//    std::set<std::string>::iterator infoNamesIt;
//    msg << MSG::DEBUG << "undeclareAll: List of services published by "
//        << ownerName << endreq;
//    for( infoNamesIt = (*infoNamesSet).begin();
//         infoNamesIt!=(*infoNamesSet).end();++infoNamesIt)
//      msg << MSG::DEBUG << (*infoNamesIt) << " ";
//    msg << MSG::DEBUG << endreq;
//    for( infoNamesIt = (*infoNamesSet).begin();
//         infoNamesIt!=(*infoNamesSet).end();++infoNamesIt){
//      std::string dimName = (*infoNamesIt);
//      if (dimName.find(ownerName) == std::string::npos) dimName = ownerName + "/" + dimName;
//      undeclService( dimName ) ;
//      undeclService( dimName + "/gauchocomment") ;
//      msg << MSG::DEBUG << "undeclareAll: Undeclared info " << (*infoNamesIt)
//          << " from owner " << ownerName << endreq;
//    }
//    m_InfoNamesMap.erase(owner );
//  } else { // Null pointer. Undeclare for all owners
//    for(m_InfoNamesMapIt = m_InfoNamesMap.begin();
//        m_InfoNamesMapIt != m_InfoNamesMap.end();++m_InfoNamesMapIt)
//      undeclareAll( m_InfoNamesMapIt->first );
//  }
//  undeclareAll(this);
//}
//
//void MonitorSvc::updateAll( bool endOfRun, const IInterface* owner)
//{
//  MsgStream msg(msgSvc(),"MonitorSvc");
//  msg << MSG::DEBUG << " inside updateAll" << endreq;
//  if( 0!=owner ){
//    std::string ownerName = infoOwnerName( owner );
//    std::set<std::string> * infoNamesSet = getInfos( owner );
//    if( 0 == infoNamesSet ) {
//      msg << MSG::DEBUG << "updateAll: No infos to update for " << ownerName << endreq;
//      return;
//    }
//    std::set<std::string>::iterator infoNamesIt;
//    msg << MSG::DEBUG << "updateAll: List of services published by " << ownerName << endreq;
//    for( infoNamesIt = (*infoNamesSet).begin();
//         infoNamesIt!=(*infoNamesSet).end();++infoNamesIt) {
//      msg << MSG::DEBUG << (*infoNamesIt) << " ";
//      msg << MSG::DEBUG << endreq;
//    }
//    for( infoNamesIt = (*infoNamesSet).begin();
//         infoNamesIt!=(*infoNamesSet).end();++infoNamesIt){
//      std::string dimName = (*infoNamesIt);
//      if (dimName.find(ownerName) == std::string::npos) dimName = ownerName + "/" + dimName;
//      updateService( dimName, endOfRun ) ;
//  //    msg << MSG::INFO << "updateAll: Updated info " << (*infoNamesIt) << " from owner " << ownerName << endreq;
//    }
//  } else { // Null pointer. Update for all owners
//    for(m_InfoNamesMapIt = m_InfoNamesMap.begin();
//        m_InfoNamesMapIt != m_InfoNamesMap.end();++m_InfoNamesMapIt)
//      updateAll(endOfRun, m_InfoNamesMapIt->first );
//  }
//}

 void MonitorSvc::resetHistos( const IInterface*  )
 {
   this->m_MonSys->Clear();
 }
//  MsgStream msg(msgSvc(),"MonitorSvc");
//  if( 0!=owner ){
//    std::string ownerName = infoOwnerName( owner );
//    std::set<std::string> * infoNamesSet = getInfos( owner );
//    if( 0 == infoNamesSet ) {
//      msg << MSG::INFO << "resethistos: No histograms to reset for " << ownerName << endreq;
//      return;
//    }
//    msg << MSG::INFO << "reset histogram for " << ownerName << endreq;
//    ownerName = ownerName + "/";
//    DimRpcInfo rpc(ownerName.c_str(),"");
//
//    //DimRpcInfo rpc(m_dimpropsvr->getName(),"Not found RCP Service");
//    rpc.setData("resethistos");
//  }
//  else{ // Null pointer. reset for all owners
//     for(m_InfoNamesMapIt = m_InfoNamesMap.begin();
//         m_InfoNamesMapIt != m_InfoNamesMap.end();++m_InfoNamesMapIt)
//       resetHistos(m_InfoNamesMapIt->first);
//  }
// }

/*void MonitorSvc::resetHistos(bool saveHistos) {
  MsgStream msg(msgSvc(),"MonitorSvc");
  DimRpcInfo rpc(m_dimRpcGaucho->getName(),"Not found RCP Service");

  if (saveHistos) rpc.setData("reset_save_histos");
  else   rpc.setData("reset_histos");

}*/

//void MonitorSvc::undeclService(std::string infoName)
//{
//  MsgStream msg(msgSvc(),"MonitorSvc");
//   msg << MSG::DEBUG << "undeclSvc: trying to undeclare Service " + infoName  << endreq;
//
////  for (m_dimSrvIt = m_dimSrv.begin(); m_dimSrvIt != m_dimSrv.end(); m_dimSrvIt++)
////      msg << MSG::DEBUG << (*m_dimSrvIt).first << endreq;
//
//  m_dimSrvIt = m_dimSrv.find(infoName);
//
//  if(m_dimSrvIt != m_dimSrv.end()) {
//    delete (*m_dimSrvIt).second;
//    m_dimSrv.erase(m_dimSrvIt);
//    msg << MSG::DEBUG << "undeclSvc: Service " + infoName + " undeclared" << endreq;
//    return;
//  }
//
//  if (infoName.find("gauchocomment") == std::string::npos) return;
//
//  msg << MSG::DEBUG << "undeclSvc: No DimService found with the name:" + infoName << endreq;
//}

//void MonitorSvc::updateService(std::string infoName, bool endOfRun)
//{
//  MsgStream msg(msgSvc(),"MonitorSvc");
////  for (m_dimSrvIt = m_dimSrv.begin(); m_dimSrvIt != m_dimSrv.end(); m_dimSrvIt++)
////  msg << MSG::DEBUG << (*m_dimSrvIt).first << endreq;
//  m_dimSrvIt = m_dimSrv.find(infoName);
//  if(m_dimSrvIt != m_dimSrv.end()) {
//    if (m_dimSrvIt->second != 0){
//      //msg << MSG::INFO << "svcName="<< m_dimSrvIt->second->getName() << endreq;
//      std::string svcName = m_dimSrvIt->second->getName();
//      if ((svcName.compare(0, 3, "Mon") == 0 )||(svcName.compare(0, 3, "MON") == 0 )){
//        DimServiceMonObject* dimSvcMO = static_cast<DimServiceMonObject*>(m_dimSrvIt->second);
//	//msg << MSG::INFO << "updating monobject service " << infoName << " endofrun " << endOfRun << endreq;
//        dimSvcMO->updateService(endOfRun);
//      }
//      else {
//        //msg << MSG::INFO << "name="<< m_dimSrvIt->first << endreq;
//      //  msg << MSG::INFO << "updating second service"<< endreq;
//        m_dimSrvIt->second->updateService(); //THIS IS WRONG
//      }
//      //msg << MSG::DEBUG << "updateSvc: Service " + infoName + " updated" << endreq;
//    }
//    else {
//      // msg << MSG::DEBUG << "updateSvc:service "<< infoName << "is been processed." << endreq;
//    }
//  }
//  //else  msg << MSG::DEBUG << "updateSvc:service "<< infoName << " was not processed yet." << endreq;
//}


 void MonitorSvc::Lock(void)
  {
//   printf("Monitor System Locking\n");
    this->m_MonSys->Lock();
  }
 void MonitorSvc::UnLock(void)
  {
//   printf("Monitor System Un-Locking\n");
    this->m_MonSys->unLock();
  }
