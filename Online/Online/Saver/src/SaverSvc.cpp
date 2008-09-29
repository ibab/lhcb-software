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
  declareProperty("refreshTime",  m_refreshTime=10);
  declareProperty("dimclientdns",m_dimClientDns);
  declareProperty("savedir", m_saveDir);
  m_monitoringFarm = true;
  m_enablePostEvents = true;
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
  
  std::vector<std::string> utgidParts = Misc::splitString(m_utgid, "_");

  if (utgidParts.size() != 3 ) {
     msg << MSG::ERROR << "Wrong utgid format for Saver !!" << endreq;
     msg << MSG::ERROR << "If you are in a EFF farm try this format: nodename_Saver_1" << endreq;
     msg << MSG::ERROR << "If you are in a Monitoring farm try: partition_nodename_MonSaver" << endreq;
     return StatusCode::FAILURE;
  }
  
  std::string verifName;
  if (utgidParts[2].compare("1")==0){ // EFF farm 
    m_nodeName = utgidParts[0];
    verifName = utgidParts[1];
    m_monitoringFarm = false;
  }
  else { //Monitoring Farm
    m_nodeName = utgidParts[1];
    verifName = utgidParts[2];
    m_monitoringFarm = true;
  }
  msg << MSG::DEBUG << "nodeName = " << m_nodeName << endreq;
  msg << MSG::DEBUG << "verifName = " << verifName << endreq;
  
  if ((verifName != "Saver")&&(verifName != "SAVER")&&(verifName != "MonSaver")&&(verifName != "MONSAVER")) {
     msg << MSG::ERROR << "Wrong utgid format for Saver !!" << endreq;
     msg << MSG::ERROR << "If you are in a EFF farm try this format: nodename_Saver_1" << endreq;
     msg << MSG::ERROR << "If you are in a Monitoring farm try: partition_nodename_MonSaver" << endreq;
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
  
  msg << MSG::DEBUG << "Consider node " << m_nodeName << " partition " << m_partName << endreq;

  std::string utgidformat = "nodename_taskname_#";
  if (m_monitoringFarm){
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
    
  //sc = service("MonitorSvc",m_monitorSvc,true);
  sc = service("MonitorSvc",m_pGauchoMonitorSvc,true);
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot access monitoring service." << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_pGauchoMonitorSvc->enableMonObjectsForString();
  
  std::vector<std::string>::iterator  it;
  int i =0;
  
  m_file = new std::string[m_taskName.size()];
  
  for (it = m_taskName.begin(); it < m_taskName.end(); it++){
    msg << MSG::DEBUG << "creating ProcessMgr for taskName " << *it << endreq;
    ProcessMgr* processMgr = new ProcessMgr (msgSvc(), this, m_refreshTime);
    if (m_monitoringFarm) processMgr->setPartVector(m_partName);
    processMgr->setTaskName(*it);
    processMgr->setAlgorithmVector(m_algorithmName);
    processMgr->setObjectVector(m_objectName);
    processMgr->setNodeName(m_nodeName);
    processMgr->setSaveDir(m_saveDir);
    processMgr->createInfoServers();
    processMgr->createTimerProcess();
  
    processMgr->setMonitorSvc(m_pGauchoMonitorSvc);

    m_file[i] = "Waiting for command to save histograms............."; 
    // std::vector<std::string>::iterator it2 = m_file.end();
    // m_file.insert(it2,fileName);
    //m_file.push_back("Waiting for command to save histograms.............");
    
    //declareInfo("SAVESETLOCATION/"+*it, *it2,"Filename of latest saveset");
    //declareInfo("SAVESETLOCATION/"+*it, m_file[i],"Filename of latest saveset");
    
    std::string *fileName = processMgr->fileNamePointer();
    
    //declareInfo("SAVESETLOCATION/"+*it, *fileName,"Filename of latest saveset");
    std::string infoName = "SAVESETLOCATION/"+*it;
    
    declareInfo(infoName, *fileName, "Filename of latest saveset");
    
    //declareInfo("filesize",m_fileSize,"Filesize of latest saveset");
  
    //processMgr->setFileStaus(m_file[i]);
    //processMgr->setFileStaus(m_file[i]);
    //m_processMgr->setFileSizeStaus(m_fileSize);
  
    msg << MSG::DEBUG << "Activing PostEvent to StartTimer............." << endreq;
    IocSensor::instance().send(this, s_startTimer, processMgr); //start Timer*/
  
    m_processMgr.push_back(processMgr);
    msg << MSG::DEBUG << "Finishing the initialize ProcessMgr for taskName " << *it << endreq;
    i++;
  }
  msg << MSG::DEBUG << "Finishing the initialize method." << endreq;
  return StatusCode::SUCCESS;
}

void SaverSvc::handle(const Event&  ev) {
  MsgStream msg(msgSvc(), name());
  
  if (!m_enablePostEvents) return;
  
  if (s_saveHistos == ev.type) {
    msg << MSG::DEBUG << " We are inside a PostEvent to SaveHistos " << endreq;
    ProcessMgr* processMgr = (ProcessMgr*) ev.data;
    save(processMgr).ignore();
    
  }
  if(s_startTimer == ev.type) {
    msg << MSG::DEBUG << " We are inside a PostEvent to Start the Timer " << endreq;
    ProcessMgr* processMgr = (ProcessMgr*) ev.data;
    processMgr->dimTimerProcess()->start(m_refreshTime);
    msg << MSG::DEBUG << " End PostEvent to Start the Timer " << endreq;
  }
  else if(s_stopTimer == ev.type) {
    msg << MSG::DEBUG << " We are inside a PostEvent to Sop the Timer " << endreq;
    ProcessMgr* processMgr = (ProcessMgr*) ev.data;
    processMgr->dimTimerProcess()->stop();
    msg << MSG::DEBUG << " End PostEvent to Stop Timer " << endreq;
  }
  else if(s_createInfoServices == ev.type ){
    msg << MSG::DEBUG << " We are inside a PostEvent to Create the DimInfoServices " << endreq;
    msg << MSG::DEBUG << "Choosing Server to get ServicesSet.........." << endreq;
    ProcessMgr* processMgr = (ProcessMgr*) ev.data;
    std::string serverChoosed;
    while (true) {
      processMgr->dimInfoServers()->chooseServer();
      serverChoosed = processMgr->dimInfoServers()->serverChoosed();
      if ("" != serverChoosed) {
        msg << MSG::DEBUG << "Server Choosed = " << processMgr->dimInfoServers()->serverChoosed() << endreq;
        break;
      }
    }
    msg << MSG::DEBUG << "Before createInfoServices............." << endreq;
    processMgr->createInfoServices(serverChoosed);
   // IocSensor::instance().send(this, s_updateServiceMap, ev.data); //start Timer*/
    msg << MSG::DEBUG << " End PostEvent to Create the DimInfoServices " << endreq;
  }
  else if(s_updateSvcMapFromInfoServer == ev.type) {
    msg << MSG::DEBUG << " We are inside a PostEvent to UpdateServiceMapFromInfoServer " << endreq;
    ProcessMgr* processMgr = (ProcessMgr*) ev.data;
    std::map<std::string, bool, std::less<std::string> > serverMap = processMgr->dimInfoServers()->serverMap();

/*    std::pair<ProcessMgr*, std::map<std::string, bool, std::less<std::string> > >* data = (std::pair<ProcessMgr*, std::map<std::string, bool, std::less<std::string> > >*) ev.data;
    ProcessMgr* processMgr = data->first;
    std::map<std::string, bool, std::less<std::string> > serverMap = data->second;*/
    
    processMgr->serviceMap()->updateMap(serverMap);
    processMgr->serviceMap()->printMap();
    msg << MSG::DEBUG << " End PostEvent to UpdateServiceMap " << endreq;
    
  }
  else if(s_updateSvcMapFromInfoService == ev.type) {
    msg << MSG::DEBUG << " We are inside a PostEvent to UpdateServiceMapFromInfoService " << endreq;
    ProcessMgr* processMgr = (ProcessMgr*) ev.data;
    std::set<std::string> serviceSet = processMgr->dimInfoServices()->serviceSet();
    processMgr->serviceMap()->setServiceSet(serviceSet);
    processMgr->updateMap();

    processMgr->serviceMap()->printMap();

    msg << MSG::DEBUG << " End PostEvent to UpdateServiceMapFromInfoService " << endreq;
  }
}

//------------------------------------------------------------------------------
void SaverSvc::handle(const Incident& inc) {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Got incident " << inc.type() << " from " << inc.source() <<endreq;
  //IocSensor::instance().send(this, s_saveHistos, this);
  std::vector<ProcessMgr *>::iterator it;
  for (it = m_processMgr.begin(); it < m_processMgr.end(); it++){
    IocSensor::instance().send(this, s_saveHistos, *it);
  }
}


//------------------------------------------------------------------------------
StatusCode SaverSvc::finalize() {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  m_enablePostEvents = false;
  msg << MSG::INFO<< "Save historgams on finalized..... " << endmsg;
  // No linger accept incidents!
  if ( m_incidentSvc ) {
    m_incidentSvc->removeListener(this);
    m_incidentSvc->release();
    m_incidentSvc = 0;
  }
  
  std::vector<ProcessMgr *>::iterator it;
  for (it = m_processMgr.begin(); it < m_processMgr.end(); it++){
    // Save histos
    save(*it).ignore();
    // Disconnect NOW from monitoring service
  }
  m_processMgr.clear();
 
  if ( m_pGauchoMonitorSvc ) {
    m_pGauchoMonitorSvc->undeclareAll(this);
    m_pGauchoMonitorSvc->release();
    m_pGauchoMonitorSvc = 0;
  }
   
  
//  if (*it) {delete *it; *it=0;}
  
  return Service::finalize();
}

//------------------------------------------------------------------------------
StatusCode SaverSvc::save(ProcessMgr* processMgr) {
//------------------------------------------------------------------------------
  MsgStream  msg( msgSvc(), name() );
    
    msg << MSG::DEBUG << "save_histos command received." << endreq;
    if (processMgr->isAdder()) {
      msg << MSG::INFO << "Sorry Adders can't save." << endreq;
      return StatusCode::SUCCESS;
    }
    std::string *fileName = processMgr->fileNamePointer();
    
    msg << MSG::INFO << "Before saving histograms in file "<< *fileName << endreq;
    
    processMgr->write();
    
//    processMgr->serviceMap()->write(m_saveDir, m_file);
/*    for (unsigned int i = 0; i < m_taskName.size(); i++){
      if (processMgr->taskName().compare(m_taskName[i]) == 0){
        //processMgr->serviceMap()->write(m_saveDir, m_file[i]);
      }
    }*/
    
    
    
    msg << MSG::INFO << "Finished saving histograms in file "<< *fileName << endreq;
  
  return StatusCode::SUCCESS;
}

