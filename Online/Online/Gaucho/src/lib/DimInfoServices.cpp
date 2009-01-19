#include "Gaucho/DimInfoServices.h"
#include "Gaucho/ProcessMgr.h"
#include "Gaucho/BaseServiceMap.h"
#include "CPP/IocSensor.h"


DimInfoServices::DimInfoServices(ProcessMgr *processMgr, std::string serverName):
  DimInfo((serverName + "/SERVICE_LIST").c_str(), -1)
 ,m_name("DimInfoServices")
 ,m_processMgr(processMgr)
{
 alreadyDone = false;
}

DimInfoServices::~DimInfoServices()
{
}

void DimInfoServices::infoHandler()
{
  if (alreadyDone) return;

  int tmpStringSize = -1;
  while ( (tmpStringSize = getSize()) <=0 ){usleep(10000);}
  
  std::string value = getString();
  m_processMgr->updateServiceSet(value, m_serviceSet);
  //IocSensor::instance().send(m_processMgr->service(), s_createServiceMap , m_processMgr->service());
  //IocSensor::instance().send(m_processMgr->service(), s_updateServiceMap, m_processMgr->dimInfoServers()); //start Timer*/
  if (m_serviceSet.size() <= 0) return;
  IocSensor::instance().send(m_processMgr->service(), s_stopTimer, (void *) m_processMgr); //stop Timer*/
//  std::pair<ProcessMgr*, std::set<std::string> > data = std::pair<ProcessMgr*, std::set<std::string> > (m_processMgr, m_serviceSet);
  //IocSensor::instance().send(m_processMgr->service(), s_updateSvcMapFromInfoService, (void *) &m_serviceSet);
  IocSensor::instance().send(m_processMgr->service(), s_updateSvcMapFromInfoService, (void *) m_processMgr);
  IocSensor::instance().send(m_processMgr->service(), s_startTimer,  (void *) m_processMgr); //start Timer*/
  alreadyDone = true;
}

void DimInfoServices::print() {
 MsgStream msg(m_processMgr->msgSvc(), name());
  
 std::set<std::string>::iterator it;
 
 for (it = m_serviceSet.begin(); it != m_serviceSet.end(); ++it){
   msg << MSG::DEBUG << "Service " << *it << endreq;    
  }
}
