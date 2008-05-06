#include "DimInfoServerStatus.h"
#include "Misc.h"

DimInfoServerStatus::DimInfoServerStatus(
                      std::map<std::string, bool, std::less<std::string> > *serverStatus, 
                      const std::string &nodeName, 
                      const std::vector<std::string> &taskName,
                      IMessageSvc* msgSvc):
  DimInfo("DIS_DNS/SERVER_LIST", -1),
  m_serverStatus(serverStatus),
  m_nodeName(nodeName),
  m_taskName(taskName),
  m_msgSvc(msgSvc)
{
  m_name = "DimInfoServerStatus";
  std::vector<std::string>::const_iterator it;
  for(it=taskName.begin(); it!=taskName.end(); ++it) m_taskNameSet.insert((*it));
}

DimInfoServerStatus::~DimInfoServerStatus(){
  
}

void DimInfoServerStatus::infoHandler(){
  MsgStream msg(msgSvc(), name());
  
  int tmpStringSize = -1;
  while ( (tmpStringSize = getSize()) <=0 ){usleep(10000);}
  
  msg << MSG::DEBUG << "*********************************************"<< endreq;
  msg << MSG::DEBUG << "Posible changes in Server List"<< endreq;
  
  std::string value = getString();
  std::string oper = value.substr(0, 1);
  if ((oper.compare("+") == 0)||(oper.compare("-") == 0)) value = value.substr(1);

  bool activeStatus = true;
  if (oper.compare("-") == 0) activeStatus = false;

  msg << MSG::DEBUG << "Verifying Server List"<< endreq;
  std::set<std::string> server = decodeServerList(value);
  std::set<std::string>::iterator serverIt;
  
  if (server.size() > 0){
    
    msg << MSG::DEBUG << "Detected changes in Server List"<< endreq;
  }
  else {
    msg << MSG::DEBUG << "Sorry no new Server List"<< endreq;
    msg << MSG::DEBUG << "*********************************************"<< endreq;
    
    return;
  }

  msg << MSG::DEBUG << "Updating Server Status"<< endreq;
  for(serverIt=server.begin(); serverIt!=server.end(); ++serverIt){
    (*m_serverStatus)[(*serverIt)] = activeStatus;
  }
 
  msg << MSG::DEBUG << "Printing Status"<< endreq;
  printServerStatus();
  
  msg << MSG::DEBUG << "*********************************************"<< endreq;
}

void DimInfoServerStatus::printServerStatus(){
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << m_serverStatus->size() << " Servers: " ;
  if (m_serverStatus->size() > 0){
    for (m_statusIt=m_serverStatus->begin() ; m_statusIt != m_serverStatus->end(); ++m_statusIt) {
      msg << m_statusIt->first ; if (m_statusIt->second) msg << "[active], "; else msg << "[inactive], ";}
  }
  msg << endreq;
}

std::set<std::string> DimInfoServerStatus::decodeServerList(const std::string &serverListS)
{
  std::set<std::string> serverList;
  std::vector<std::string> serverListTot = Misc::splitString(serverListS, "|");
  std::vector<std::string>::const_iterator serverListTotIt;
  std::set<std::string>::const_iterator setIt;
  
  for(serverListTotIt=serverListTot.begin(); serverListTotIt!=serverListTot.end(); ++serverListTotIt){
   // std::cout<<"server="<<(*serverListTotIt)<<std::endl;
    // checking the nodeName
    std::size_t first_us = (*serverListTotIt).find("_");
    if (first_us == std::string::npos) continue;
    std::string nodeName = (*serverListTotIt).substr(0, first_us);
    
    for(setIt=m_nodeNameSet.begin(); setIt!=m_nodeNameSet.end(); ++setIt)
      if (Misc::findCaseIns((*setIt), nodeName) == std::string::npos) continue;
    
    // checking the taskName
    std::size_t second_us = (*serverListTotIt).find("_", first_us + 1);
    if (second_us == std::string::npos) continue;
    std::string taskName = (*serverListTotIt).substr(first_us + 1, second_us - first_us - 1);
    
    for(setIt=m_taskNameSet.begin(); setIt!=m_taskNameSet.end(); ++setIt)
      if (Misc::findCaseIns((*setIt), taskName) == std::string::npos) continue;

    std::string serverName = (*serverListTotIt).substr(0, (*serverListTotIt).find("@"));
   // std::cout<<"consider server="<<serverName<<std::endl;
    serverList.insert(serverName);
  }
  return serverList;
}

bool DimInfoServerStatus::isServerActive (const std::string &serverName){
  m_statusIt = m_serverStatus->find(serverName);
  if (m_statusIt != m_serverStatus->end()) return m_statusIt->second;
  return false;  
}



