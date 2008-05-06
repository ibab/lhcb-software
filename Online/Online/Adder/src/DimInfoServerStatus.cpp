#include "DimInfoServerStatus.h"
#include "DimInfoServiceSet.h" 
#include "Misc.h"

DimInfoServerStatus::DimInfoServerStatus(
                      std::map<std::string, bool, std::less<std::string> > *serverStatus, 
                      std::set<std::string> *serviceSet,
                      bool *updateTimerAdder,
                      std::string *choosedServerName,
                      const std::vector<std::string> &nodeName, 
                      const std::vector<std::string> &taskName,
                      const std::vector<std::string> &algorithmName,
                      const std::vector<std::string> &objectName,
		      IMessageSvc* msgSvc):
  DimInfo("DIS_DNS/SERVER_LIST", -1),
  m_serverStatus(serverStatus),
  m_serviceSet(serviceSet),
  m_updateTimerAdder(updateTimerAdder),
  m_choosedServerName(choosedServerName),
  m_taskName(taskName),
  m_algorithmName(algorithmName),
  m_objectName(objectName),
  m_msgSvc(msgSvc)
{
  
  m_dimInfoServiceSet = 0;
  
  m_name = "DimInfoServerStatus";
  std::vector<std::string>::const_iterator it;
  for(it=nodeName.begin(); it!=nodeName.end(); ++it) m_nodeNameSet.insert((*it));
  for(it=taskName.begin(); it!=taskName.end(); ++it) m_taskNameSet.insert((*it));
}

DimInfoServerStatus::~DimInfoServerStatus(){
  if (m_dimInfoServiceSet != 0) {delete m_dimInfoServiceSet; m_dimInfoServiceSet = 0;}
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

  (*m_updateTimerAdder) = true;
  
  msg << MSG::DEBUG << "Updating Server Status"<< endreq;
  for(serverIt=server.begin(); serverIt!=server.end(); ++serverIt)
    (*m_serverStatus)[(*serverIt)] = activeStatus;

  printServerStatus();
  
  if ((*m_choosedServerName).compare("") != 0){
    if ((*m_serverStatus)[(*m_choosedServerName)]) {
      msg << MSG::DEBUG << "The choosed Server Name is the same, no changes in Service Set"<< endreq;
      msg << MSG::DEBUG << "*********************************************"<< endreq;
    
      return;
    }
  }
  
  msg << MSG::DEBUG << "Choosing one Server to create Service Set"<< endreq;
  chooseServer();
  msg << MSG::DEBUG << "New Server choosed:" << (*m_choosedServerName) << endreq;
  msg << MSG::DEBUG << "Updating Service Set"<< endreq;
  updateServiceSet();
  // printServiceSet(); we can not print at this point the serviceset because it needs a time to be updated (asynchronus)
  
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


void DimInfoServerStatus::printServiceSet(){
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << m_serviceSet->size() << " Services in " <<  (*m_choosedServerName) << endreq;
  if (m_serviceSet->size() > 0){
     std::set<std::string>::iterator serviceIt;
     for (serviceIt=m_serviceSet->begin() ; serviceIt != m_serviceSet->end(); ++serviceIt)
       msg << MSG::DEBUG << " service: " <<  (*serviceIt) << endreq;
  }
}

void DimInfoServerStatus::chooseServer() {
  // if there is no server we have to choose someone
  // else we verify that the old server is still active and
  // if the server is not active we have to change it
  if (m_choosedServerName->compare("") == 0){
    for(m_statusIt=m_serverStatus->begin(); m_statusIt!=m_serverStatus->end(); ++m_statusIt)
      if (m_statusIt->second) (*m_choosedServerName) = m_statusIt->first;
  }
  else{
    // if the server is not active we have to change it
    if (!(*m_serverStatus)[(*m_choosedServerName)]){
      (*m_choosedServerName) = "";
      for(m_statusIt=m_serverStatus->begin(); m_statusIt!=m_serverStatus->end(); ++m_statusIt)
        if (m_statusIt->second) (*m_choosedServerName) = m_statusIt->first;
    }
  }
}

void DimInfoServerStatus::updateServiceSet(){
   MsgStream msg(msgSvc(), name());
  // if no server then we clear the service set
  // but firstly we have to kill the DimInfo service 
  // to avoid DIM modifications in service set list
  if (m_choosedServerName->compare("") == 0){
    msg << MSG::DEBUG << "no server choosed"<< endreq;
    delete m_dimInfoServiceSet;
    m_dimInfoServiceSet = 0;
    m_serviceSet->clear();
    return;
  }
  // if no DimInfo service status then we define one, join
  // to it the service status list and let DIM to fill it
  if (m_dimInfoServiceSet == 0) {
    msg << MSG::DEBUG << "ServiceSet is Null, Creating one"<< endreq;
    m_serviceSet->clear();
    m_dimInfoServiceSet = new DimInfoServiceSet(m_serviceSet, 
                                                m_updateTimerAdder,
                                                (*m_choosedServerName), // <---  
                                                m_taskName,
                                                m_algorithmName,
                                                m_objectName, 
                                                msgSvc());
    
//     int tmpStringSize = -1;
//     while ( (tmpStringSize = m_serviceSet->size()) <=0 ){
//       msg << MSG::DEBUG << "*********************************************"<< endreq;
//       msg << MSG::DEBUG << "Waiting for changes in ServiceSet"<< endreq;
//       msg << MSG::DEBUG << "*********************************************"<< endreq;
//       sleep(2); 
//     }
    
    return;
  }
  // if the server changed we have to kill the old DimInfo service status,
  // define a new one, join to it the service status list and let DIM to fill it
  if (!(m_dimInfoServiceSet->serverName().compare((*m_choosedServerName)) == 0)){
    msg << MSG::DEBUG << "server coosed different from the older, creating ServiceSet"<< endreq;
    delete m_dimInfoServiceSet;
    m_dimInfoServiceSet = 0;
    m_serviceSet->clear();
    m_dimInfoServiceSet = new DimInfoServiceSet(m_serviceSet, 
                                                m_updateTimerAdder,
                                                (*m_choosedServerName), // <---  
                                                m_taskName,
                                                m_algorithmName,
                                                m_objectName,
                                                msgSvc());
/*    int tmpStringSize = -1;
    while ( (tmpStringSize = m_serviceSet->size()) <=0 ){
      msg << MSG::DEBUG << "*********************************************"<< endreq;
      msg << MSG::DEBUG << "Waiting for changes in ServiceSet"<< endreq;
      msg << MSG::DEBUG << "*********************************************"<< endreq;
      sleep(2); 
    }*/
    return;
  }
  
  msg << MSG::DEBUG << "No changes in ServiceSet"<< endreq;
  
}

std::set<std::string> DimInfoServerStatus::decodeServerList(const std::string &serverListS)
{
  std::set<std::string> serverList;
  std::vector<std::string> serverListTot = Misc::splitString(serverListS, "|");
  std::vector<std::string>::const_iterator serverListTotIt;
  std::set<std::string>::const_iterator setIt;
  
  for(serverListTotIt=serverListTot.begin(); serverListTotIt!=serverListTot.end(); ++serverListTotIt){
    //std::cout<<"server="<<(*serverListTotIt)<<std::endl;
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
    //std::cout<<"consider server="<<serverName<<std::endl;
    serverList.insert(serverName);
  }
  return serverList;
}





