#include "Gaucho/services/ProcessMgr.h"
#include "Gaucho/services/DimInfoServers.h"
#include "Gaucho/services/Misc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "Gaucho/services/BaseServiceMap.h"
#include "Gaucho/services/DimTimerProcess.h"
#include "AdderSvc.h"

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
  declareProperty("refreshTime",  m_refreshTime=5);
  declareProperty("dimclientdns",m_dimClientDns);
  //declareProperty("savedir", m_saveDir);

}

AdderSvc::~AdderSvc() {}

StatusCode AdderSvc::initialize() {
  StatusCode sc = Service::initialize(); // must be executed first
  lib_rtl_sleep(1000);
  MsgStream msg(msgSvc(), name());
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot initialize service base class." << endmsg;
    return StatusCode::FAILURE;
  }

  m_utgid = RTL::processName();
  std::size_t first_us = m_utgid.find("_");
  std::size_t second_us = m_utgid.find("_", first_us + 1);
  m_nodeName = m_utgid.substr(0, first_us);
  std::string taskName = m_utgid.substr(first_us + 1, second_us - first_us - 1);

  if ((taskName != "Adder")&&(taskName != "ADDER")) {
    msg << MSG::ERROR << "This is not an Adder !" << endreq;
    msg << MSG::ERROR << "Please try nodeName_Adder_1" << endreq;
    return StatusCode::FAILURE;
  }

  sc = service("IncidentSvc",m_incidentSvc,true);
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot access incident service." << endmsg;
    return StatusCode::FAILURE;
  }

  msg << MSG::INFO << "***************************************************** " << endreq;

  msg << MSG::INFO << "****************** Welcome to Adder Test************* " << endreq;
  msg << MSG::INFO << "***************************************************** " << endreq;
  msg << MSG::INFO << "This Adder will add data published in : " << m_dimClientDns << endreq;

  msg << MSG::INFO << "Consider node " << m_nodeName << endreq;

  msg << MSG::INFO << "TaskNames: " << endreq;

  std::vector<std::string>::iterator it;

  for(it = m_taskName.begin(); it != m_taskName.end(); ++it) {
    msg << MSG::INFO << "         taskName: " << *it << endreq;
  }

  msg << MSG::DEBUG << "Properties: " << endreq;
  for(unsigned int i=0; i < m_objectName.size();++i) {
    msg << MSG::DEBUG << "         objectName: " << m_objectName[i] << endreq;
    msg << MSG::DEBUG << "         algorithmName: " << m_algorithmName[i] << endreq;
    msg << MSG::DEBUG << endreq;
  }

  msg << MSG::DEBUG << "Adder will publish data every " << m_refreshTime << " seconds"<< endreq;
  msg << MSG::DEBUG << "***************************************************** " << endreq;
  msg << MSG::DEBUG << "***************************************************** " << endreq;

  msg << MSG::DEBUG << "creating ProcessMgr" << endreq;
  m_processMgr = new ProcessMgr (msgSvc(), this);
  m_processMgr->setSubFarmVector(m_subfarmName);
  m_processMgr->setTaskVector(m_taskName);
  m_processMgr->setAlgorithmVector(m_algorithmName);
  m_processMgr->setObjectVector(m_objectName);
  m_processMgr->setUtgid(m_utgid);

  m_processMgr->createInfoServers();

  m_processMgr->createTimerProcess(m_refreshTime);

  msg << MSG::INFO << "Activing PostEvent to StartTimer............." << endreq;
  IocSensor::instance().send(this, s_startTimer, this); //start Timer*/

  msg << MSG::INFO << "Finishing the initialize method." << endreq;
  return StatusCode::SUCCESS;
}

void AdderSvc::handle(const Event&  ev) {
  MsgStream msg(msgSvc(), name());

  if(s_startTimer == ev.type) {
    msg << MSG::INFO << " We are inside a PostEvent to Start the Timer " << endreq;
    m_processMgr->dimTimerProcess()->start(m_refreshTime);
    msg << MSG::INFO << " End PostEvent to Start the Timer " << endreq;
  }
  else if(s_createInfoServices == ev.type ){
    msg << MSG::INFO << " We are inside a PostEvent to Create the DimInfoServices " << endreq;
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
    msg << MSG::INFO << " End PostEvent to Create the DimInfoServices " << endreq;
  }
  else if(s_updateSvcMapFromInfoServer == ev.type) {
    msg << MSG::INFO << " We are inside a PostEvent to UpdateServiceMapFromInfoServer " << endreq;
    std::map<std::string, bool, std::less<std::string> >* serverMap = (std::map<std::string, bool, std::less<std::string> >*) ev.data;
    m_processMgr->serviceMap()->updateMap(*serverMap);
    m_processMgr->serviceMap()->printMap();
    msg << MSG::INFO << " End PostEvent to UpdateServiceMap " << endreq;
  }
  else if(s_updateSvcMapFromInfoService == ev.type) {
    msg << MSG::INFO << " We are inside a PostEvent to UpdateServiceMapFromInfoService " << endreq;
    std::set<std::string>* serviceSet = (std::set<std::string>*) ev.data;
    m_processMgr->serviceMap()->setServiceSet(*serviceSet);
    m_processMgr->updateMap();
    m_processMgr->serviceMap()->printMap();
    msg << MSG::INFO << " End PostEvent to UpdateServiceMapFromInfoService " << endreq;
  }
}

void AdderSvc::handle(const Incident& inc) {
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "Got incident " << inc.type() << " from " << inc.source() <<endreq;
  //IocSensor::instance().send(this, s_COMMAND_IN_PROCESSMGR, this);
}

StatusCode AdderSvc::finalize() {
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "Finalize Adder..... " << endmsg;
  if (m_processMgr) {delete m_processMgr; m_processMgr=0;}
  return Service::finalize();
}

