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
  m_runNb=0;
  m_firststart = true;
}

SaverSvc::~SaverSvc() {}

StatusCode SaverSvc::initialize() {
  StatusCode sc = Service::initialize(); // must be executed first
   MsgStream msg(msgSvc(), name()); 
   if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot initialize service base class." << endreq;
    return StatusCode::FAILURE;
  }
     dim_set_write_timeout(15);
   sc = service("IncidentSvc",m_incidentSvc,true);
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot access incident service." << endmsg;
    return StatusCode::FAILURE;
  }
  
    
  m_incidentSvc->addListener(this,"SAVE_HISTOS");
    
  sc = service("MonitorSvc",m_pGauchoMonitorSvc,true);
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot access monitoring service." << endmsg;
    return StatusCode::FAILURE;
  }
  
  
  //msg << MSG::DEBUG << "Finishing the initialize method." << endreq;

  return StatusCode::SUCCESS;
}

void SaverSvc::handle(const Event&  ev) {
  MsgStream msg(msgSvc(), name());
  /* 
  std::vector<ProcessMgr *>::iterator it;
  int i=0;
  msg << MSG::INFO << "Setting the runnumber service." << endreq;
  for (it = m_processMgr.begin(); it < m_processMgr.end(); it++){
   m_processMgr[i]->setrunNumber(m_runNbSvc);
    i++;
  }*/
  
  if (!m_enablePostEvents) return;
  
  if (s_saveHistos == ev.type) {
   ProcessMgr* processMgr = (ProcessMgr*) ev.data;    
    save(processMgr).ignore();    
    //stop the timer after saving (at end of run)
   // processMgr->dimTimerProcess()->stop();   
  }
  if(s_startTimer == ev.type) { 
    ProcessMgr* processMgr = (ProcessMgr*) ev.data;
    processMgr->dimTimerProcess()->start(m_refreshTime);   
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
   // processMgr->serviceMap()->printMap();

  }
  

}

//------------------------------------------------------------------------------
void SaverSvc::handle(const Incident& inc) {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "Incident " << inc.type() << " occured " << endreq;
 
  std::vector<ProcessMgr *>::iterator it;
  int i=0;
  for (it = m_processMgr.begin(); it < m_processMgr.end(); it++){
    if (inc.type()=="SAVE_HISTOS") IocSensor::instance().send(this, s_saveHistos, *it);
    i++;
  }
}

StatusCode SaverSvc::start() {
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO  << "SaverSvc is starting." << endreq;
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
     //wait until the Adder is RUNNING
     m_monitoringFarm = false;
  }
  else { //Monitoring Farm
     m_nodeName = utgidParts[1];
     verifName = utgidParts[2];
     m_monitoringFarm = true;
  }
 
  if ((verifName != "Saver")&&(verifName != "SAVER")&&(verifName != "RecSaver")&&(verifName != "RECSAVER")) {
     msg << MSG::ERROR << "Wrong utgid format for Saver !!" << endreq;
     msg << MSG::ERROR << "If you are in a EFF farm try this format: nodename_Saver_1" << endreq;
     msg << MSG::ERROR << "If you are in a Monitoring farm try: partition_nodename_MonSaver" << endreq;
     return StatusCode::FAILURE;
  }
  std::string utgidformat = "nodename_taskname_#";
  if (m_monitoringFarm){
     utgidformat = "partname_" + utgidformat;
  } 
    
  m_partName.push_back(m_tmpPart);    
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
     if (m_firststart) {
        if (m_monitoringFarm) processMgr->setPartVector(m_partName);
        else {
	   lib_rtl_sleep(44000);
	   processMgr->setPartName(m_tmpPart);
	}   
        processMgr->setTaskName(*it);
        processMgr->setAlgorithmVector(m_algorithmName);
        processMgr->setObjectVector(m_objectName);
        processMgr->setNodeName(m_nodeName);
        processMgr->setSaveDir(m_saveDir);
        processMgr->setSaveDiff(m_saveDiff);
        processMgr->createInfoServers();
        processMgr->createTimerProcess();
        processMgr->dimTimerProcess()->start(m_refreshTime);   
        processMgr->setMonitorSvc(m_pGauchoMonitorSvc);
	m_processMgr.push_back(processMgr);	  
        std::string serviceName="";
        if (utgidParts[1]=="MONA0901") {
           processMgr->setFarm("MF");
	  // serviceName = m_tmpPart+"_MONA0901_RecAdder_1/Brunel/MonitorSvc/monRate/RunNumber"; 
        }
        else {
           processMgr->setFarm("EFF");
	 //  serviceName = m_tmpPart+"_Adder_1/GauchoJob/MonitorSvc/monRate/RunNumber"; 
        }
	serviceName = m_tmpPart+"_Adder_1/GauchoJob/MonitorSvc/monRate/RunNumber"; 
 
       // msg << MSG::INFO << "Subscribing to runnumber svc" << endreq;
	//DimClient::setDnsNode("HLT01");

        m_runNbSvc = new DimInfoRunNb(serviceName.c_str(),utgidParts[0]);  
        processMgr->setrunNumber(m_runNbSvc);       
        std::string *fileName = processMgr->fileNamePointer();
        std::string name;
        name=*fileName;    
        std::string infoName = m_tmpPart+"/SaverSvc/SAVESETLOCATION";
        //DimClient::setDnsNode(m_dimClientDns.c_str());
        m_dimSvcSaveSetLoc = new DimService(infoName.c_str(),(char *) name.c_str());

     }  
  }
  m_firststart=false; 

  //always start the timer at start 
  std::vector<ProcessMgr *>::iterator itpmgr;
  int i=0;
  for (itpmgr = m_processMgr.begin(); itpmgr < m_processMgr.end(); itpmgr++){
    // Save histos
  //  IocSensor::instance().send(this, s_startTimer, *itpmgr); //start Timer*/
    i++;
  }  
  msg << MSG::INFO  << "SaverSvc has started." << endreq; 
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode SaverSvc::stop() {
//------------------------------------------------------------------------------
 MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG<< "SaverSvc is stopping." << endmsg;
  return Service::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode SaverSvc::finalize() {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG<< "SaverSvc is finalizing." << endmsg;
  return Service::SUCCESS;
}



//------------------------------------------------------------------------------
StatusCode SaverSvc::save(ProcessMgr* processMgr) {
//------------------------------------------------------------------------------
  MsgStream  msg( msgSvc(), name() );
  //this will only be called at the end of a run
    if (processMgr->serviceOwner().compare(s_Saver) != 0 ) {
      msg << MSG::WARNING << "Sorry Only Savers can save." << endreq;
      return StatusCode::SUCCESS;
    }
    std::string *fileName = processMgr->fileNamePointer();
    m_runNb=m_runNbSvc->getRunNb();

    //when the runnumber changes we should stop and restart the dim timer
    //only save if the runnumber !=0
    if (m_runNb>0) {
       if (processMgr->getrunNumber(false)!=0) {
       //remove the stop temporarily 21/04
          //processMgr->dimTimerProcess()->stop();
         processMgr->write();   
       }   
       else {
	 processMgr->write();  
       }	   
       msg << MSG::DEBUG << "Finished saving histograms in file "<< *fileName << endreq; 
    }
    else {
       msg << MSG::DEBUG << "Runnumber unknown. Can't save "<< m_runNb << endreq;   
    } 
  return StatusCode::SUCCESS;
}


