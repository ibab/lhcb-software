#include "Gaucho/services/ProcessMgr.h"
#include "Gaucho/services/DimInfoServers.h"
#include "Gaucho/services/Misc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "Gaucho/services/BaseServiceMap.h"
#include "Gaucho/services/DimTimerProcess.h"
#include "SaverSvc.h"

#include <ctime>

#include "RTL/rtl.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb, SaverSvc)

SaverSvc::SaverSvc(const std::string& name, ISvcLocator* ploc) : Service(name, ploc),/*m_monitorSvc(0), */m_incidentSvc(0)
{
  declareProperty("partname",m_partName);
  declareProperty("taskname",m_taskName);
  declareProperty("algorithmname",m_algorithmName);
  declareProperty("objectname",m_objectName);
  declareProperty("refreshTime",  m_refreshTime=5);
  declareProperty("dimclientdns",m_dimClientDns);
  declareProperty("savedir", m_saveDir);
  
}

SaverSvc::~SaverSvc() {}

StatusCode SaverSvc::initialize() {
  StatusCode sc = Service::initialize(); // must be executed first
  
  MsgStream msg(msgSvc(), name());
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot initialize service base class." << endreq;
    return StatusCode::FAILURE;
  }
  
  m_utgid = RTL::processName();
  std::size_t first_us = m_utgid.find("_");
  std::size_t second_us = m_utgid.find("_", first_us + 1);
  m_nodeName = m_utgid.substr(0, first_us);
  std::string verifName = m_utgid.substr(first_us + 1, second_us - first_us - 1);
  
  if ((verifName != "Saver")&&(verifName != "SAVER")) {
    msg << MSG::ERROR << "This is not a Saver !" << endreq;
    msg << MSG::ERROR << "Pplease try nodeName_Saver_1" << endreq;
    return StatusCode::FAILURE;
  }

  sc = service("IncidentSvc",m_incidentSvc,true);
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot access incident service." << endmsg;
    return StatusCode::FAILURE;
  }
  
  msg << MSG::DEBUG << "***************************************************** " << endreq;
  msg << MSG::DEBUG << "******************Welcome to Saver test******************* " << endreq;
  msg << MSG::DEBUG << "***************************************************** " << endreq;
  msg << MSG::DEBUG << "This Saver will save data published in : " << m_dimClientDns << endreq;
  
  msg << MSG::DEBUG << "Consider node " << m_nodeName << endreq;

  if ((0 == m_partName.size())&&(0 == m_taskName.size())){
    msg << MSG::ERROR << "In your options file you should to specify the tasks names OR the partition names." << endreq;
    return StatusCode::FAILURE;
  }
  
  m_withPartitionName = false;
  std::string utgidformat = "nodename_taskname_X";
  
  if (m_partName.size() > 0){
    m_withPartitionName = true;
    utgidformat = "partname_" + utgidformat;
  } 
    
  msg << MSG::DEBUG << "This Saver will save services with UTGID format " << utgidformat << endreq;
  
  if (m_partName.size() > 0) msg << MSG::DEBUG << "Partition Names: " << endreq;
  for(unsigned int i=0; i < m_partName.size();++i) {
     msg << MSG::DEBUG << "         partName: " << m_partName[i] << endreq;
     msg << MSG::DEBUG << endreq;
  }
    
  if (m_taskName.size() > 0) msg << MSG::DEBUG << "Task Names: " << endreq;
  for(unsigned int i=0; i < m_taskName.size();++i) {
    msg << MSG::DEBUG << "         taskName: " << m_taskName[i] << endreq;
    msg << MSG::DEBUG << endreq;
  }
  
  if (m_objectName.size() > 0) msg << MSG::DEBUG << "Properties: " << endreq;
  for(unsigned int i=0; i < m_objectName.size();++i) {
    msg << MSG::DEBUG << "         objectName: " << m_objectName[i] << endreq;
    msg << MSG::DEBUG << "         algorithmName: " << m_algorithmName[i] << endreq;
    msg << MSG::DEBUG << endreq;
  }
  
  m_incidentSvc->addListener(this,"SAVE_HISTOS");

  msg << MSG::DEBUG << "Saver will save data every " << m_refreshTime << " seconds"<< endreq;
  msg << MSG::DEBUG << "***************************************************** " << endreq;
  msg << MSG::DEBUG << "***************************************************** " << endreq;
    
  msg << MSG::DEBUG << "creating ProcessMgr" << endreq;
  m_processMgr = new ProcessMgr (msgSvc(), this);
  if (m_withPartitionName) m_processMgr->setPartVector(m_partName);
  m_processMgr->setTaskVector(m_taskName);
  m_processMgr->setAlgorithmVector(m_algorithmName);
  m_processMgr->setObjectVector(m_objectName);
  m_processMgr->setUtgid(m_utgid);
  m_processMgr->setSaveDir(m_saveDir);

  m_processMgr->createInfoServers();

  m_processMgr->createTimerProcess(m_refreshTime);

  m_file="Waiting for command to save histograms............."; 
  m_fileSize=0;

  //sc = service("MonitorSvc",m_monitorSvc,true);
  sc = service("MonitorSvc",m_pGauchoMonitorSvc,true);
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot access monitoring service." << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_processMgr->setMonitorSvc(m_pGauchoMonitorSvc);
  declareInfo("file",m_file,"Filename of latest saveset");
  //declareInfo("filesize",m_fileSize,"Filesize of latest saveset");

  m_processMgr->setFileStaus(m_file);
  m_processMgr->setFileSizeStaus(m_fileSize);
  
  msg << MSG::DEBUG << "Activing PostEvent to StartTimer............." << endreq;
  IocSensor::instance().send(this, s_startTimer, this); //start Timer*/
  
  msg << MSG::DEBUG << "Finishing the initialize method." << endreq;
  return StatusCode::SUCCESS;
}

void SaverSvc::handle(const Event&  ev) {
  MsgStream msg(msgSvc(), name());

  if (s_saveHistos == ev.type) {
    save().ignore();
  }
  else if(s_startTimer == ev.type) {
    msg << MSG::DEBUG << " We are inside a PostEvent to Start the Timer " << endreq;
    m_processMgr->dimTimerProcess()->start(m_refreshTime);
    msg << MSG::DEBUG << " End PostEvent to Start the Timer " << endreq;
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
    std::map<std::string, bool, std::less<std::string> >* serverMap = (std::map<std::string, bool, std::less<std::string> >*) ev.data;
    m_processMgr->serviceMap()->updateMap(*serverMap);
    m_processMgr->serviceMap()->printMap();
    msg << MSG::DEBUG << " End PostEvent to UpdateServiceMap " << endreq;
  }
  else if(s_updateSvcMapFromInfoService == ev.type) {
    msg << MSG::DEBUG << " We are inside a PostEvent to UpdateServiceMapFromInfoService " << endreq;
    std::set<std::string>* serviceSet = (std::set<std::string>*) ev.data;
    m_processMgr->serviceMap()->setServiceSet(*serviceSet);
    m_processMgr->updateMap();
    m_processMgr->serviceMap()->printMap();
    msg << MSG::DEBUG << " End PostEvent to UpdateServiceMapFromInfoService " << endreq;
  }
}

//------------------------------------------------------------------------------
void SaverSvc::handle(const Incident& inc) {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Got incident " << inc.type() << " from " << inc.source() <<endreq;
  IocSensor::instance().send(this, s_saveHistos, this);
  
}


//------------------------------------------------------------------------------
StatusCode SaverSvc::finalize() {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO<< "Save historgams on finalized..... " << endmsg;
  // No linger accept incidents!
  if ( m_incidentSvc ) {
    m_incidentSvc->removeListener(this);
    m_incidentSvc->release();
    m_incidentSvc = 0;
  }
  // Save histos
  save().ignore();
  // Disconnect NOW from monitoring service
  if ( m_pGauchoMonitorSvc ) {
    m_pGauchoMonitorSvc->undeclareAll(this);
    m_pGauchoMonitorSvc->release();
    m_pGauchoMonitorSvc = 0;
  }
  
  if (m_processMgr) {delete m_processMgr; m_processMgr=0;}
  return Service::finalize();
}

//------------------------------------------------------------------------------
StatusCode SaverSvc::save() {
//------------------------------------------------------------------------------
  MsgStream  msg( msgSvc(), name() );
  //msg << MSG::INFO << "executing Saver....command " << m_command << endreq;

  //if (m_command=="SAVE_HISTOS") {
    msg << MSG::DEBUG << "save_histos command received." << endreq;
    if (m_processMgr->isAdder()) {
      msg << MSG::INFO << "Sorry Adders can't save." << endreq;
      return StatusCode::SUCCESS;
    }
    m_processMgr->serviceMap()->write(m_saveDir, m_file, m_fileSize);
    msg << MSG::INFO << "Finished saving histograms. "<< endreq;
  //}
  
  return StatusCode::SUCCESS;
}

