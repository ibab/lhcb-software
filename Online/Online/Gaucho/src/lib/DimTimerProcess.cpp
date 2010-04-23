#include <iostream>
#include<time.h>

#include "Gaucho/DimTimerProcess.h"
#include "Gaucho/ProcessMgr.h"
#include "Gaucho/BaseServiceMap.h"
#include "dic.hxx"

DimTimerProcess::DimTimerProcess (ProcessMgr *processMgr, const int &refreshTime, IMessageSvc* msgSvc) : 
  DimTimer(refreshTime),
  m_name("DimTimerProcess"),
  m_processMgr(processMgr),
  m_refreshTime(refreshTime),
  m_msgSvc(msgSvc)
{
  stop();
}

DimTimerProcess::~DimTimerProcess(){
 
}

void DimTimerProcess::timerHandler() {
  time_t now;
  time(&now);
  bool endofrun=false;
  endofrun=m_processMgr->timerHandler();
  // don't restart the timer at the end of the run
  if (!endofrun) start(m_refreshTime);
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

