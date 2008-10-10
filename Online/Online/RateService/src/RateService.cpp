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
#include "RateService.h"

#include <ctime>

#include "RTL/rtl.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb, RateService)

RateService::RateService(const std::string& name, ISvcLocator* ploc) : Service(name, ploc)
{
  declareProperty("refreshTime",  m_refreshTime=10);
  declareProperty("dimclientdns", m_dimClientDns);
  declareProperty("partName", m_partName);
  declareProperty("taskName", m_taskName="Adder");
  declareProperty("monRateCounterNumber", m_nbCounterInMonRate=30);  
  m_enablePostEvents = true;
}

RateService::~RateService() {}

StatusCode RateService::initialize() {
  StatusCode sc = Service::initialize(); // must be executed first
  
  MsgStream msg(msgSvc(), name());
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot initialize service base class." << endreq;
    return StatusCode::FAILURE;
  }
  
  m_utgid = RTL::processName();
  std::size_t first_us = m_utgid.find("_");
  std::size_t second_us = m_utgid.find("_", first_us + 1);
  std::string partName = m_utgid.substr(0, first_us);
  std::string taskName = m_utgid.substr(first_us + 1, second_us - first_us - 1);

  if (m_partName.compare("") == 0)  m_partName = partName;
  
  if ((taskName != "RateSvc")&&(taskName != "RATESVC")) {
    msg << MSG::ERROR << "This is not a MonRateService !" << endreq;
    msg << MSG::ERROR << "Please try partName_RateSvc_1" << endreq;
    return StatusCode::FAILURE;
  }

  msg << MSG::DEBUG << "***************************************************** " << endreq;
  msg << MSG::DEBUG << "******************Welcome to MonRateService********** " << endreq;
  msg << MSG::DEBUG << "***************************************************** " << endreq;
  
  msg << MSG::DEBUG << "***************************************************** " << endreq;
  msg << MSG::DEBUG << "***************************************************** " << endreq;
    
  sc = service("MonitorSvc",m_pGauchoMonitorSvc,true);
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot access monitoring service." << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_pGauchoMonitorSvc->enableMonObjectsForString();
  

  msg << MSG::DEBUG << "creating ProcessMgr " << endreq;
  m_processMgr = new ProcessMgr (s_MonRateService, msgSvc(), this, m_refreshTime);
  m_processMgr->setPartName(m_partName);
  m_processMgr->setTaskName(m_taskName);
  m_processMgr->setUtgid(m_utgid);
  m_processMgr->setNbCounterInMonRate(m_nbCounterInMonRate);
  
  m_processMgr->createInfoServers();
  m_processMgr->createTimerProcess();
  m_processMgr->setMonitorSvc(m_pGauchoMonitorSvc);

  msg << MSG::DEBUG << "Activing PostEvent to StartTimer............." << endreq;
  IocSensor::instance().send(this, s_startTimer, m_processMgr); //start Timer
  
  msg << MSG::DEBUG << "Finishing the initialize method." << endreq;
  return StatusCode::SUCCESS;
}

void RateService::handle(const Event&  ev) {
  MsgStream msg(msgSvc(), name());
  
  if (!m_enablePostEvents) return;
  
  if(s_startTimer == ev.type) {
     msg << MSG::DEBUG << " We are inside a PostEvent to Start the Timer " << endreq;
     m_processMgr->dimTimerProcess()->start(m_refreshTime);
     msg << MSG::DEBUG << " End PostEvent to Start the Timer " << endreq;
  }
  else if(s_stopTimer == ev.type) {
     msg << MSG::DEBUG << " We are inside a PostEvent to Sop the Timer " << endreq;
     m_processMgr->dimTimerProcess()->stop();
     msg << MSG::DEBUG << " End PostEvent to Stop Timer " << endreq;
  }
  else if(s_createInfoServices == ev.type ){
    msg << MSG::DEBUG << " We are inside a PostEvent to Create the DimInfoServices " << endreq;
    msg << MSG::DEBUG << "Choosing Server to get ServicesSet.........." << endreq;
    
    std::string serverChoosed;
    while (true) {
      m_processMgr->dimInfoServers()->chooseServer();
      serverChoosed = m_processMgr->dimInfoServers()->serverChoosed();
      if ("" != serverChoosed) {
        msg << MSG::DEBUG << "Server Choosed = " << m_processMgr->dimInfoServers()->serverChoosed() << endreq;
        break;
      }
    }
    msg << MSG::DEBUG << "Before createInfoServices............." << endreq;
    m_processMgr->createInfoServices(serverChoosed);
   // IocSensor::instance().send(this, s_updateServiceMap, ev.data); //start Timer*/
    msg << MSG::DEBUG << " End PostEvent to Create the DimInfoServices " << endreq;
  }
  else if(s_updateSvcMapFromInfoServer == ev.type) {
    msg << MSG::DEBUG << " We are inside a PostEvent to UpdateServiceMapFromInfoServer " << endreq;
    
    std::map<std::string, bool, std::less<std::string> > serverMap = m_processMgr->dimInfoServers()->serverMap();

    m_processMgr->serviceMap()->updateMap(serverMap);
    m_processMgr->serviceMap()->printMap();
    msg << MSG::DEBUG << " End PostEvent to UpdateServiceMap " << endreq;
    
  }
  else if(s_updateSvcMapFromInfoService == ev.type) {
    msg << MSG::DEBUG << " We are inside a PostEvent to UpdateServiceMapFromInfoService " << endreq;
    
    std::set<std::string> serviceSet = m_processMgr->dimInfoServices()->serviceSet();
    m_processMgr->serviceMap()->setServiceSet(serviceSet);
    m_processMgr->updateMap();

    m_processMgr->serviceMap()->printMap();

    msg << MSG::DEBUG << " End PostEvent to UpdateServiceMapFromInfoService " << endreq;
  }
}

//------------------------------------------------------------------------------
StatusCode RateService::finalize() {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  m_enablePostEvents = false;
  
  if (m_processMgr) {delete m_processMgr; m_processMgr=0;}
 
  if ( m_pGauchoMonitorSvc ) {
    m_pGauchoMonitorSvc->undeclareAll(this);
    m_pGauchoMonitorSvc->release();
    m_pGauchoMonitorSvc = 0;
  }
   
  return Service::finalize();
}

