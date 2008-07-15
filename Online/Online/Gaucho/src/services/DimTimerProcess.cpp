#include <iostream>
#include<time.h>

#include "Gaucho/services/DimTimerProcess.h"
#include "Gaucho/services/ProcessMgr.h"
#include "Gaucho/services/BaseServiceMap.h"

DimTimerProcess::DimTimerProcess (ProcessMgr *processMgr, const int &refreshTime, IMessageSvc* msgSvc) : 
  DimTimer(refreshTime),
  m_name("DimTimerProcess"),
  m_processMgr(processMgr),
  m_refreshTime(refreshTime),
  m_msgSvc(msgSvc)
{
  MsgStream msg(msgSvc, name());
  msg << MSG::DEBUG << "Inside DimTimerProcess Creator." << endreq;
  stop();
}

DimTimerProcess::~DimTimerProcess(){
 
}

void DimTimerProcess::timerHandler() {
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "**********************************" << endreq;
  msg << MSG::DEBUG << "         timerHandler             " << endreq;
  time_t now;
  time(&now);
  m_processMgr->timerHandler();
  start(m_refreshTime);
  msg << MSG::DEBUG << "        end  timerHandler         " << endreq;
  msg << MSG::DEBUG << "**********************************" << endreq;
}

// bool DimTimerProcess::statusDiscrepancy() {
// 
//   std::map<std::string, bool, std::less<std::string> >::const_iterator itStat;
//   
//   if (m_choosedServerNameLocal.compare((*m_choosedServerName)) != 0 ) return true;
//   
//   if (m_serverStatusLocal.size() != m_serverStatus->size()) return true;
//   
//   for (itStat=m_serverStatusLocal.begin() ; itStat != m_serverStatusLocal.end(); ++itStat)
//     if (m_serverStatus->find(itStat->first)->second != itStat->second) return true;
//     
//   return false;
// }    

