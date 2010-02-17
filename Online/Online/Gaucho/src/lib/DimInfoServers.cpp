#include "Gaucho/DimInfoServers.h"
#include "Gaucho/ProcessMgr.h"
#include "Gaucho/BaseServiceMap.h"
#include "CPP/IocSensor.h"


DimInfoServers::DimInfoServers(ProcessMgr *processMgr):
  DimInfo("DIS_DNS/SERVER_LIST", -1),
  m_name("DimInfoServers"),
  m_processMgr(processMgr)
{
  m_serverChoosed = "";
}

DimInfoServers::~DimInfoServers() {
  
}

void DimInfoServers::infoHandler() {

  int tmpStringSize = -1;
  while ( (tmpStringSize = getSize()) <=0 ){usleep(10000);}

  std::string value = getString();

  m_processMgr->updateServerMap(value, m_serverMap);

  if ("" == m_serverChoosed) {
    if (0 != m_serverMap.size()){
      //IocSensor::instance().send(m_processMgr->service(), s_createInfoServices, (void *) &m_serverMap); // this command also updateServiceMap
      IocSensor::instance().send(m_processMgr->service(), s_createInfoServices, (void *) m_processMgr); // this command also updateServiceMap
    }
  }
  else {
    IocSensor::instance().send(m_processMgr->service(), s_stopTimer, (void *) m_processMgr); //stop Timer*/
    //IocSensor::instance().send(m_processMgr->service(), s_updateSvcMapFromInfoServer, (void *) &m_serverMap);
    IocSensor::instance().send(m_processMgr->service(), s_updateSvcMapFromInfoServer, (void *) m_processMgr);
    IocSensor::instance().send(m_processMgr->service(), s_startTimer, (void *) m_processMgr); //start Timer*/
  }

  //m_processMgr->serviceMap()->updateMap(m_serverMap);
}

void DimInfoServers::chooseServer() {
  
   
  for (m_it = m_serverMap.begin(); m_it != m_serverMap.end(); ++m_it){
    
  
    if (m_it->second) {
      m_serverChoosed = m_it->first;
      return ;
    }
  }
}

bool DimInfoServers::isActive(std::string serverName) {
  return m_serverMap.find(serverName)->second;
}


void DimInfoServers::print() {
 MsgStream msg(m_processMgr->msgSvc(), name());
 for (m_it = m_serverMap.begin(); m_it != m_serverMap.end(); ++m_it){
   std::string status = "inactive" ;
   if (m_it->second) status = "active" ;
   msg << MSG::DEBUG << "The server " << m_it->first << " is " << status << endreq;    
  }
}

