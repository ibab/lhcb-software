#include "Gaucho/ProcessMgr.h"
#include "Gaucho/DimInfoServers.h"
#include "Gaucho/Misc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "Gaucho/BaseServiceMap.h"
#include "Gaucho/DimTimerProcess.h"
#include "Gaucho/DimInfoServices.h"
#include "Gaucho/AdderSvc.h"
#include "DimCmdServer.h"
#include "dic.hxx"

#include <ctime>

#include "RTL/rtl.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb, AdderSvc)

AdderSvc::AdderSvc(const std::string& name, ISvcLocator* ploc) : Service(name, ploc),/*m_monitorSvc(0), */m_incidentSvc(0)
{
  //declareProperty("partname",m_partName); adders doesn't consider services with partname
  declareProperty("taskname",m_taskName);
  declareProperty("subfarmname",m_subfarmName);
  declareProperty("algorithmname",m_algorithmName);
  declareProperty("objectname",m_objectName);
  declareProperty("partitionname",m_partitionName);
  declareProperty("refreshTime",  m_refreshTime=20);
  declareProperty("dimclientdns",m_dimClientDns);
  declareProperty("publishRates",m_publishRates=0);

  //declareProperty("savedir", m_saveDir);
  m_enablePostEvents = true;
  m_reconfigured = false;
}

AdderSvc::~AdderSvc() {}

StatusCode AdderSvc::initialize() {
  StatusCode sc = Service::initialize(); // must be executed first
  lib_rtl_sleep(2000);
  MsgStream msg(msgSvc(), name());
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot initialize service base class." << endmsg;
    return StatusCode::FAILURE;
  }

  m_utgid = RTL::processName();
  std::size_t first_us = m_utgid.find("_");
  std::size_t second_us = m_utgid.find("_", first_us + 1);
  std::size_t third_us = m_utgid.find("_", second_us + 1);
  std::string adderType;
  std::string taskName;
  if (third_us != std::string::npos) {
     // three underscores found in UTGID -> we're in the Monitoring Farm
     m_nodeName = m_utgid.substr(first_us+1,second_us-first_us-1);
     m_farm = "MF";
     taskName = m_utgid.substr(second_us + 1, third_us - second_us - 1);
  }   
  else {
     m_nodeName = m_utgid.substr(0, first_us);
     m_farm = "EFF";
     taskName = m_utgid.substr(first_us + 1, second_us - first_us - 1);
  }
  if (m_nodeName.size() == 8) adderType = "First Level"; 
  else if ((m_nodeName.size() == 6)&&((m_nodeName.substr(0,4)!="PART")||(m_farm=="MF"))) {
    lib_rtl_sleep(10000);
    adderType = "Second Level"; }
  else if ((m_nodeName.size() == 6)&&((m_nodeName.substr(0,4)=="PART")||(m_farm=="MF"))) {
    lib_rtl_sleep(20000);
  adderType = "Third Level"; }
  else {
    msg << MSG::ERROR << "This is not an Adder because the nodeName do not correspond at any case" << endreq;
    msg << MSG::ERROR << "1rst level ==> HLTA0101" << endreq;
    msg << MSG::ERROR << "2nd  level ==> HLTA01" << endreq;
    msg << MSG::ERROR << "3rd  level ==> PARTxx" << endreq;
    return StatusCode::FAILURE;
  }



  if (((taskName != "Adder")&&(taskName != "ADDER"))&&((taskName !="RecAdder")&&(taskName !="RECADDER"))) {
    msg << MSG::ERROR << "This is not an Adder !" << endreq;
    msg << MSG::ERROR << "Please try nodeName_Adder_1" << endreq;
    return StatusCode::FAILURE;
  }

  sc = service("IncidentSvc",m_incidentSvc,true);
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot access incident service." << endmsg;
    return StatusCode::FAILURE;
  }

  m_incidentSvc->addListener(this,"RECONFIGURE");

  m_dimcmdsvr = new DimCmdServer( (m_utgid+"/"), serviceLocator(), 0);

  DimClient::setDnsNode(m_dimClientDns.c_str());
  m_processMgr = new ProcessMgr (s_Adder, msgSvc(), this, m_refreshTime);
  m_processMgr->setSubFarmVector(m_subfarmName);
  m_processMgr->setTaskName(m_taskName);
  m_processMgr->setAlgorithmVector(m_algorithmName);
  m_processMgr->setObjectVector(m_objectName);
  m_processMgr->setUtgid(m_utgid);
  m_processMgr->setFarm(m_farm);
  m_processMgr->setPartitionName(m_partitionName);
  if (m_publishRates == 1) m_processMgr->setPublishRates(true);

  startUp();
 // msg << MSG::DEBUG << "Finishing the initialize method." << endreq;
  return StatusCode::SUCCESS;
}

void AdderSvc::startUp(){
  MsgStream msg(msgSvc(), name());
  m_processMgr->createInfoServers();
  m_processMgr->createTimerProcess();
  IocSensor::instance().send(this, s_startTimer, this); //start Timer*/
}

void AdderSvc::shutDown(){
  MsgStream msg(msgSvc(), name());
  m_enablePostEvents = false;
  m_processMgr->dimTimerProcess()->stop();
  m_processMgr->destroyTimerProcess();
  m_processMgr->destroyInfoServers();
  m_processMgr->destroyInfoServices();
  m_enablePostEvents = true;
 }

void AdderSvc::handle(const Event&  ev) {
  MsgStream msg(msgSvc(), name());
  
  if (!m_enablePostEvents) return;
    
  if (s_reconfigureAdder == ev.type) {
    shutDown();
    startUp();
    m_reconfigured = true;
  }
  if(s_startTimer == ev.type) {
    m_processMgr->dimTimerProcess()->start(m_refreshTime);
  }
  else if(s_stopTimer == ev.type) {
    m_processMgr->dimTimerProcess()->stop();
  }
  else if(s_createInfoServices == ev.type ){
    std::string serverChoosed;
    while (true) {
      m_processMgr->dimInfoServers()->chooseServer();
      serverChoosed = m_processMgr->dimInfoServers()->serverChoosed();
      if ("" != serverChoosed) break;      
    }
    m_processMgr->createInfoServices(serverChoosed);
  }
  else if(s_updateSvcMapFromInfoServer == ev.type) {

    std::map<std::string, bool, std::less<std::string> > serverMap = m_processMgr->dimInfoServers()->serverMap();
    m_processMgr->serviceMap()->updateMap(serverMap);
  }
  else if(s_updateSvcMapFromInfoService == ev.type) {

    std::set<std::string> serviceSet = m_processMgr->dimInfoServices()->serviceSet();
    std::map<std::string, bool, std::less<std::string> > serverMap = m_processMgr->dimInfoServers()->serverMap();
    m_processMgr->serviceMap()->setServiceSet(serviceSet);
    m_processMgr->serviceMap()->updateMap(serverMap);
  }
}

//------------------------------------------------------------------------------
void AdderSvc::handle(const Incident& inc) {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
    msg << MSG::DEBUG << "Got incident " << inc.type() << " from " << inc.source() <<endreq;
  IocSensor::instance().send(this, s_reconfigureAdder, this);
}

StatusCode AdderSvc::finalize() {
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize Adder..... " << endmsg;  
  //keep alive in time to save histos
   lib_rtl_sleep(5000);
//  m_processMgr->serviceMap()->add();
  m_enablePostEvents = false;
  return Service::finalize();
}

