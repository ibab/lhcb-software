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
#include "Gaucho/SaverSvc.h"
#include "Gaucho/DimInfoRunNb.h"


#include <ctime>

#include "RTL/rtl.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"


DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb, SaverSvc)

SaverSvc::SaverSvc(const std::string& name, ISvcLocator* ploc) : Service(name, ploc),/*m_monitorSvc(0), */m_incidentSvc(0)
{
  declareProperty("partname",m_tmpPart="LHCb");
  declareProperty("taskname",m_taskName);
  declareProperty("algorithmname",m_algorithmName);
  declareProperty("objectname",m_objectName);
  declareProperty("refreshTime",  m_refreshTime=20);
  declareProperty("dimclientdns",m_dimClientDns);
  declareProperty("savedir", m_saveDir);
  declareProperty("saveDiff", m_saveDiff=0);
  m_monitoringFarm = true;
  m_enablePostEvents = true;
  m_finalizing = false;
  m_runNb=0;
  m_firstsave = true;
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

  if ((utgidParts.size() != 3 )&&(utgidParts[1]!="MONA0901")) {
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
//  msg << MSG::DEBUG << "nodeName = " << m_nodeName << endreq;
//  msg << MSG::DEBUG << "verifName = " << verifName << endreq;
  
  if ((verifName != "Saver")&&(verifName != "SAVER")&&(verifName != "RecSaver")&&(verifName != "RECSAVER")) {
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
  
//  msg << MSG::DEBUG << "***************************************************** " << endreq;
//  msg << MSG::DEBUG << "******************Welcome to Saver test******************* " << endreq;
//  msg << MSG::DEBUG << "***************************************************** " << endreq;
//  msg << MSG::DEBUG << "This Saver will save data published in : " << m_dimClientDns << endreq;
  
//  msg << MSG::DEBUG << "Consider node " << m_nodeName << endreq;

  std::string utgidformat = "nodename_taskname_#";
  if (m_monitoringFarm){
    utgidformat = "partname_" + utgidformat;
  } 
    
//  msg << MSG::DEBUG << "This Saver will save services with UTGID format " << utgidformat << endreq;
  
  m_partName.push_back(m_tmpPart);
 /* if (m_partName.size() > 0) msg << MSG::DEBUG << "Partition Names: " << endreq;
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
  } */
  
  m_incidentSvc->addListener(this,"SAVE_HISTOS");

//  msg << MSG::DEBUG << "Saver will save data every " << m_refreshTime << " seconds"<< endreq;
//  msg << MSG::DEBUG << "***************************************************** " << endreq;
//  msg << MSG::DEBUG << "***************************************************** " << endreq;
    
  sc = service("MonitorSvc",m_pGauchoMonitorSvc,true);
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot access monitoring service." << endmsg;
    return StatusCode::FAILURE;
  }

  
  std::vector<std::string>::iterator  it;
  
  char timestr[64];
  char year[5]; 
  time_t rawTime=time(NULL);
  struct tm* timeInfo = localtime(&rawTime);
  ::strftime(timestr, sizeof(timestr),"%Y%m%dT%H%M%S", timeInfo);
  ::strftime(year, sizeof(year),"%Y", timeInfo);

  m_saveDir=m_saveDir+"/"+year+"/"+m_tmpPart+"/";
        
  for (it = m_taskName.begin(); it < m_taskName.end(); it++){
    ProcessMgr* processMgr = new ProcessMgr (s_Saver, msgSvc(), this, m_refreshTime);
    if (m_monitoringFarm) processMgr->setPartVector(m_partName);
    else processMgr->setPartName(m_tmpPart);
    processMgr->setTaskName(*it);
    processMgr->setAlgorithmVector(m_algorithmName);
    processMgr->setObjectVector(m_objectName);
    processMgr->setNodeName(m_nodeName);
    processMgr->setSaveDir(m_saveDir);
    processMgr->setSaveDiff(m_saveDiff);
    processMgr->createInfoServers();
    processMgr->createTimerProcess();
  
    processMgr->setMonitorSvc(m_pGauchoMonitorSvc);
    std::string serviceName="";
    if (utgidParts[1]=="MONA0901") {
       processMgr->setFarm("MF");
    }
    else {
       processMgr->setFarm("EFF");
    }
    serviceName = m_tmpPart+"_Adder_1/GauchoJob/MonitorSvc/monRate/RunNumber"; 
    m_runNbSvc = new DimInfoRunNb(serviceName.c_str(),utgidParts[0]);  
  //  msg << MSG::DEBUG << "created runnumber service " << serviceName.c_str() << endreq;
   // m_runNb = processMgr->getrunNumber(false);
       
    std::string *fileName = processMgr->fileNamePointer();
    std::string name;
    name=*fileName;
    
    std::string infoName = m_tmpPart+"/SaverSvc/SAVESETLOCATION";
    m_dimSvcSaveSetLoc = new DimService(infoName.c_str(),(char *) name.c_str());
  
    msg << MSG::DEBUG << "Activing PostEvent to StartTimer............." << endreq;
    IocSensor::instance().send(this, s_startTimer, processMgr); //start Timer*/
  
    m_processMgr.push_back(processMgr);
    msg << MSG::DEBUG << "Finishing the initialize ProcessMgr for taskName " << *it << endreq;
  }


  
  msg << MSG::DEBUG << "Finishing the initialize method." << endreq;

  return StatusCode::SUCCESS;
}

void SaverSvc::handle(const Event&  ev) {
  MsgStream msg(msgSvc(), name());
   
  std::vector<ProcessMgr *>::iterator it;
  int i=0;
  for (it = m_processMgr.begin(); it < m_processMgr.end(); it++){
   m_processMgr[i]->setrunNumber(m_runNbSvc);
    i++;
  }
  
  if (!m_enablePostEvents) return;
  
  if (s_saveHistos == ev.type) {
    ProcessMgr* processMgr = (ProcessMgr*) ev.data;    
    save(processMgr).ignore();    
    //update run number for next timer save
    //processMgr->getrunNumber(true);
  }
  if(s_startTimer == ev.type) { 
    ProcessMgr* processMgr = (ProcessMgr*) ev.data;
    if (m_firstsave == true) {
       processMgr->dimTimerProcess()->start(120);
       }
    else {
       processMgr->dimTimerProcess()->start(m_refreshTime);}   
  }
  else if(s_stopTimer == ev.type) {
   ProcessMgr* processMgr = (ProcessMgr*) ev.data;
    processMgr->dimTimerProcess()->stop();
  }
  else if(s_createInfoServices == ev.type ){
    ProcessMgr* processMgr = (ProcessMgr*) ev.data;
    std::string serverChoosed;
    while (true) {
      processMgr->dimInfoServers()->chooseServer();
      serverChoosed = processMgr->dimInfoServers()->serverChoosed();
      if ("" != serverChoosed) {
        break;
      }
    }
    processMgr->createInfoServices(serverChoosed);
  }
  else if(s_updateSvcMapFromInfoServer == ev.type) {
    ProcessMgr* processMgr = (ProcessMgr*) ev.data;
    std::map<std::string, bool, std::less<std::string> > serverMap = processMgr->dimInfoServers()->serverMap();
   
    processMgr->serviceMap()->updateMap(serverMap);
    
  }
  else if(s_updateSvcMapFromInfoService == ev.type) {
    ProcessMgr* processMgr = (ProcessMgr*) ev.data;
    std::set<std::string> serviceSet = processMgr->dimInfoServices()->serviceSet();
    processMgr->serviceMap()->setServiceSet(serviceSet);
    processMgr->updateMap();

    processMgr->serviceMap()->printMap();

  }
  

}

//------------------------------------------------------------------------------
void SaverSvc::handle(const Incident& inc) {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Got incident " << inc.type() << " from " << inc.source() <<endreq;

  std::vector<ProcessMgr *>::iterator it;
  int i=0;
  for (it = m_processMgr.begin(); it < m_processMgr.end(); it++){
    m_processMgr[i]->setrunNumber(m_runNbSvc);
    IocSensor::instance().send(this, s_saveHistos, *it);
    i++;
  }
}


//------------------------------------------------------------------------------
StatusCode SaverSvc::finalize() {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  m_enablePostEvents = false;
  m_finalizing = true;
  msg << MSG::INFO<< "Saving histograms on finalize..... " << endmsg;
  // No longer accept incidents!
 // if ( m_incidentSvc ) {
 //   m_incidentSvc->removeListener(this);
 //   m_incidentSvc->release();
 //   m_incidentSvc = 0;
//  }

  std::vector<ProcessMgr *>::iterator it;
  int i=0;
  for (it = m_processMgr.begin(); it < m_processMgr.end(); it++){
    // Save histos
    save(*it).ignore();
    i++;
    // Disconnect NOW from monitoring service
  }
  m_processMgr.clear();
 
 // if ( m_pGauchoMonitorSvc ) {
 //   m_pGauchoMonitorSvc->undeclareAll(this);
 //   m_pGauchoMonitorSvc->release();
//    m_pGauchoMonitorSvc = 0;
//  }
  if (m_runNbSvc) { delete m_runNbSvc; }
  return Service::finalize();
}

//------------------------------------------------------------------------------
StatusCode SaverSvc::save(ProcessMgr* processMgr) {
//------------------------------------------------------------------------------
  MsgStream  msg( msgSvc(), name() );
 
    if (processMgr->serviceOwner().compare(s_Saver) != 0 ) {
      msg << MSG::WARNING << "Sorry Only Savers can save." << endreq;
      return StatusCode::SUCCESS;
    }
    std::string *fileName = processMgr->fileNamePointer();

 
    if (!m_finalizing) { processMgr->setrunNumber(m_runNbSvc);
        if (m_runNb == 0) m_runNb=m_runNbSvc->getRunNb() ;
           }

     msg << MSG::INFO << "saving histograms in "<< *fileName << " runnb " << m_runNb << endreq;     
     //if the runnumber is 0 here, try to get it 
    //when the runnumber changes we should stop and restart the dim timer
    //only save if the runnumber !=0
    if (m_runNb!=0) {
       if (processMgr->getrunNumber(false)!=0) {
          processMgr->dimTimerProcess()->stop();
          processMgr->write(); 
          //set the runnumber of the next run
          if (!m_finalizing) {
             m_runNb=processMgr->getrunNumber(true);        
             processMgr->dimTimerProcess()->start(m_refreshTime);
          }	  
       }   
       else processMgr->write();   
       m_firstsave=false;  
       msg << MSG::INFO << "Finished saving histograms in file "<< *fileName << endreq;
    }
    else {
       msg << MSG::INFO << "Runnumber unknown. Can't save "<< m_runNb << endreq;    
    } 
  return StatusCode::SUCCESS;
}

