#include "Gaucho/services/ProcessMgr.h"
#include "Gaucho/services/DimInfoServers.h"
#include "Gaucho/services/DimInfoServices.h"
#include "Gaucho/services/BaseServiceMap.h"
#include "Gaucho/services/DimTimerProcess.h"
#include "Gaucho/services/Misc.h"
#include "CPP/Interactor.h"
#include "TFile.h"

ProcessMgr::ProcessMgr(IMessageSvc* msgSvc, Interactor *service):m_name("ProcessMgr"), m_msgSvc(msgSvc), m_service(service)
{
  m_isAdder = false;
  m_withPartitionName = false;
}

ProcessMgr::~ProcessMgr() {
  m_dimTimerProcess->stop();
  if (m_dimTimerProcess) {delete m_dimTimerProcess; m_dimTimerProcess=0;}
  if (m_dimInfoServices) {delete m_dimInfoServices; m_dimInfoServices=0;}
  if (m_dimInfoServers) {delete m_dimInfoServers; m_dimInfoServers=0;}
  if (m_serviceMap) {delete m_serviceMap; m_serviceMap=0;}
}

void ProcessMgr::updateMap(){
  m_serviceMap->updateMap(m_dimInfoServers->serverMap());
}

void ProcessMgr::createInfoServers() {
  MsgStream msg(msgSvc(), name());
  m_serviceMap = new BaseServiceMap(this);
  msg << MSG::DEBUG << "Creating Server Status"<< endreq;
  m_dimInfoServers = new DimInfoServers(this);
}

void ProcessMgr::createInfoServices(std::string serverName){
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Creating Service Status"<< endreq;
  m_dimInfoServices = new DimInfoServices(this, serverName);
}

void ProcessMgr::createTimerProcess(int refreshTime) {
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Creating DimTimerAdder"<< endreq;
  
  m_dimTimerProcess = new DimTimerProcess (this, refreshTime, msgSvc());
  
}

void ProcessMgr::timerHandler(){
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "inside timerHandler"<< endreq;

  if (isAdder()){
    msg << MSG::DEBUG << "isAdder"<< endreq;
    m_serviceMap->add();
  }
  else { //it's a saver May be we have to save every deltaT
    msg << MSG::DEBUG << "isSaver"<< endreq;
    //std::string fileName = "from timerHandler";
    //int fileSize = 0;
    m_serviceMap->write(m_saveDir, *m_pFile, *m_pFileSize);
    //m_pGauchoMonitorSvc->updateService(infoName, false);
    m_pGauchoMonitorSvc->updateAll(false);
  }
}

void ProcessMgr::setUtgid(const std::string &utgid)
{
  m_utgid = utgid;
  std::size_t first_us = m_utgid.find("_");
  std::size_t second_us = m_utgid.find("_", first_us + 1);
  m_nodeName = m_utgid.substr(0, first_us);
  std::string other = m_utgid.substr(first_us + 1, second_us - first_us - 1);
  if ((other.compare("Adder")==0)||(other.compare("ADDER")==0)) m_isAdder = true;
} 

void ProcessMgr::updateServiceSet(std::string &dimString, std::set<std::string> &serviceSet) {
  
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "*************************************************************************"<< endreq;
  msg << MSG::DEBUG << "*******Posible changes in ServiceSet. We will update the ServiceSet******"<< endreq;
 
  msg << MSG::DEBUG << "DimString = " << dimString << endreq;
  
  std::vector<std::string> serviceList = Misc::splitString(dimString, "\n");
  std::vector<std::string>::const_iterator serviceListIt;

  for(serviceListIt=serviceList.begin(); serviceListIt!=serviceList.end(); ++serviceListIt){
    
    std::string serviceName = (*serviceListIt).substr(0, (*serviceListIt).find("|"));
    std::string oper = serviceName.substr(0, 1);
    if ((oper.compare("+") == 0)||(oper.compare("-") == 0)) serviceName = serviceName.substr(1);
    
    bool includeService = true;
    if (oper.compare("-") == 0) includeService = false;
    
    msg << MSG::DEBUG << "Verifying service = " << serviceName<< endreq;
    msg << MSG::DEBUG << "Verifying service2 = " << serviceName<< endreq;

    std::vector<std::string>::const_iterator it;
    if (m_subfarmName.size() != 0) {
      bool chooseIt=false;
      for(it=m_subfarmName.begin(); it!=m_subfarmName.end(); ++it){
        msg << MSG::DEBUG << "verifying subfarmName in serviceName "<< serviceName << endreq;
        if (serviceName.find("/" + (*it)) == std::string::npos){
         chooseIt=true;
         break;
        }
      }
      if (!chooseIt) {
        msg << MSG::DEBUG << "REFUSED because subfarmName NOT OK" << endreq;
        continue;
      }
      else msg << MSG::DEBUG << "subfarmName OK" << endreq;
    }
    else {
      if (serviceName.find("/" + m_nodeName) == std::string::npos) continue;
    }

    std::vector<std::string> serviceParts = Misc::splitString(serviceName, "/");

    //it should be possible to have an unlimited number of slashes im the histogram names
    if (serviceParts.size() < 4 ) continue; 
    std::string svctype = serviceParts[0];
    msg << MSG::INFO << "svctype = " << svctype << endreq;

    std::string pfxSvcType = svctype.substr(0, 3);
    if ((pfxSvcType.compare("MON")!=0)&&(pfxSvcType.compare("mon")!=0)&&(pfxSvcType.compare("Mon")!=0)) {
      msg << MSG::INFO << "REFUSED because this service is not a MonObject: "<< pfxSvcType << endreq;
      continue;  
    }
    pfxSvcType = svctype.substr(3, 1);
    if ((pfxSvcType.compare("H")!=0)&&(pfxSvcType.compare("h")!=0)&&(pfxSvcType.compare("R")!=0)&&(pfxSvcType.compare("r")!=0)  ) {
      msg << MSG::INFO << "REFUSED because this MonObject Service is not a MonH nor a MonR: " << pfxSvcType << endreq;
      continue;  
    }
    
    std::string utgid = serviceParts[1];
    msg << MSG::DEBUG << "utgid = " << utgid << endreq;
    
    std::string task;
    std::string part;
    int index=2;
    if (m_withPartitionName) {
      part = Misc::splitString(utgid, "_")[0];
      task = Misc::splitString(utgid, "_")[2];
    }
    else{
      task = Misc::splitString(utgid, "_")[1];
      // we don't do nothing with Savers
      if ((task.compare("Saver")==0)||(task.compare("SAVER")==0)) continue; 

      if ((task.compare("Adder")==0)||(task.compare("ADDER")==0)) {
        if (serviceParts.size() < 5 ) continue; 
        task = serviceParts[index];
        index++;
      }
    }
    
    msg << MSG::DEBUG << "task = " << task << endreq;
    std::string algo = serviceParts[index];
    msg << MSG::DEBUG << "algo = " << algo << endreq;
    
    std::string object = serviceParts[index+1];
    for (unsigned int i = index+2; i < serviceParts.size(); ++i)
      object += "/" + serviceParts[i];
    
    msg << MSG::DEBUG << "object = " << object << endreq;
    
    // it is not necesary to verify the node name. It is done by the updateServerMap
    
    bool matched = true;
    if (m_taskName.size() > 0)  matched = false;
    for(unsigned int i = 0; i < m_taskName.size(); i++) {
      msg << MSG::DEBUG << "task match : " << m_taskName[i] << " vs " << task << endreq;
      if (Misc::findCaseIns(m_taskName[i], task) != std::string::npos) {
        matched = true;
        break;
      }
    }
    if (!matched) continue;
    
    matched = true;
    if (m_algorithmName.size() > 0)  matched = false;
    for(unsigned int i = 0; i < m_algorithmName.size(); i++) {
      msg << MSG::DEBUG << "algo match : " << m_algorithmName[i] << " vs " << algo << endreq;
      if (Misc::findCaseIns(m_algorithmName[i], algo) != std::string::npos) {
        matched = true;
        break;
      }
    }
    if (!matched) continue;

    matched = true;
    if (m_objectName.size() > 0)  matched = false;
    for(unsigned int i = 0; i < m_objectName.size(); i++) {
      msg << MSG::DEBUG <<  "object match : " << m_objectName[i] << " vs " << object  << endreq;
      if (m_objectName[i].find("*", 0) != std::string::npos) {
        std::vector<std::string> objectNameParts = Misc::splitString(m_objectName[i], "*");
        if (Misc::matchStringCaseIns(object, objectNameParts)) {
          matched = true;
          break;
        }
      }
      else{// No wildcard, then object must be equal to m_objectName[i]
        if (Misc::findCaseIns(m_objectName[i], object) != std::string::npos) {
          matched = true;
          break;
        }
      }
    }
    if (!matched) continue;
        
    if (includeService) {
      msg << MSG::DEBUG << "service INCLUDED => " << serviceName << endreq;
      serviceSet.insert(serviceName);
    }
    else {
      msg << MSG::DEBUG << "service EXCLUDED => " << serviceName << endreq;
      serviceSet.erase(serviceName);
    }
  }
  
  msg << MSG::DEBUG << "***************The process to update the ServiceSet is finished**********"<< endreq;
  msg << MSG::DEBUG << "*************************************************************************"<< endreq;
  
}

void ProcessMgr::updateServerMap(std::string &dimString, std::map<std::string, bool, std::less<std::string> > &serverMap) {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "*************************************************************************"<< endreq;
  msg << MSG::DEBUG << "*******Posible changes in ServerMap. We will update the ServerMap********"<< endreq;
  
  msg << MSG::DEBUG << "DimString = " << dimString << endreq;
  
  std::string value = dimString;
  std::string oper = value.substr(0, 1);
  if ((oper.compare("+") == 0)||(oper.compare("-") == 0)) value = value.substr(1);

  bool activeStatus = true;
  if (oper.compare("-") == 0) activeStatus = false;

  msg << MSG::DEBUG << "Verifying Server List"<< endreq;
  std::set<std::string> server = decodeServerList(value);
  std::set<std::string>::iterator it;
  
  if (server.size() > 0)
    msg << MSG::DEBUG << "Detected changes in Server List"<< endreq;
  else {
    msg << MSG::DEBUG << "Sorry no new Server"<< endreq;
    msg << MSG::DEBUG << "*********************************************"<< endreq;
    return;
  }

  msg << MSG::DEBUG << "Updating Server Status"<< endreq;
  
  for(it=server.begin(); it!=server.end(); ++it) serverMap[(*it)] = activeStatus;
  
  msg << MSG::DEBUG << "***************The process to update the ServerMap is finished***********"<< endreq;
  msg << MSG::DEBUG << "*************************************************************************"<< endreq;
  
}

std::set<std::string> ProcessMgr::decodeServerList(const std::string &serverListS)
{
  MsgStream msg(msgSvc(), name());
  std::set<std::string> serverList;
  std::vector<std::string> serverListTot = Misc::splitString(serverListS, "|");
  std::vector<std::string>::const_iterator serverListTotIt;
  std::vector<std::string>::const_iterator it;
  
  for(serverListTotIt = serverListTot.begin(); serverListTotIt!=serverListTot.end(); ++serverListTotIt) {
    msg << MSG::DEBUG << "checking server="<<(*serverListTotIt)<< endreq;
    
    std::size_t first_us = (*serverListTotIt).find("_");
    if (first_us == std::string::npos) continue;
    
    std::size_t second_us = (*serverListTotIt).find("_", first_us + 1);
    if (second_us == std::string::npos) continue;

    std::string partName;
    std::string nodeName;
    std::string taskName;
    
    if (m_withPartitionName){
      std::size_t third_us = (*serverListTotIt).find("_", second_us + 1);
      if (third_us == std::string::npos) continue;
      
      partName = (*serverListTotIt).substr(0, first_us);
      nodeName = (*serverListTotIt).substr(first_us + 1, second_us - first_us - 1);
      taskName = (*serverListTotIt).substr(second_us + 1, third_us - second_us - 1);
      if ((taskName.compare("Saver")==0)||(taskName.compare("SAVER")==0)||(taskName.compare("Adder")==0)||(taskName.compare("ADDER")==0)) {
        msg << MSG::DEBUG << "refused because we don't save ADDERS nor save SAVERS. "<< endreq;    
        continue; // we don't do nothing with Savers
      }
      if (nodeName.size() !=  m_nodeName.size()) { // If nodeName have smaller size 
        nodeName = nodeName.substr(0, m_nodeName.size()); // we resize the nodename to do the match.
        msg << MSG::DEBUG << "nodeName="<< nodeName << endreq;
      }
      
      // checking the partName
      bool chooseIt=true;
      if (m_partName.size()) chooseIt=false;
      for(it=m_partName.begin(); it!=m_partName.end(); ++it){
        msg << MSG::DEBUG << "comparing partName="<< partName << " with "<< (*it) << endreq;
        if (Misc::findCaseIns((*it), partName) != std::string::npos) {
          chooseIt=true;
          break;
        }
      }
      if (!chooseIt) {
        msg << MSG::DEBUG << "REFUSED because partName NOT OK" << endreq;
        continue; 
      }
      else msg << MSG::DEBUG << "partName OK" << endreq;
    }
    else {
      nodeName = (*serverListTotIt).substr(0, first_us);
      taskName = (*serverListTotIt).substr(first_us + 1, second_us - first_us - 1);
      if ((taskName.compare("Saver")==0)||(taskName.compare("SAVER")==0)) {
        msg << MSG::DEBUG << "refused because we don't add nor save SAVERS. "<< endreq;    
        continue; // we don't do nothing with Savers
      }
      if (isAdder()) { // This is an Adder
        msg << MSG::DEBUG << " We are inside an ADDER " << endreq;
        if (nodeName.compare("Bridge") != 0) {
          if ((taskName.compare("Adder")==0)||(taskName.compare("ADDER")==0)){
            if (nodeName.size() ==  m_nodeName.size()) {
              msg << MSG::DEBUG << "REFUSED because it is an adder with the same node name size. "<< endreq;
              continue; // The nodeName have to be smaller size if the Adder is adding Adders
            }
          }
          if (nodeName.size() !=  m_nodeName.size()) { // If nodeName have smaller size 
            nodeName = nodeName.substr(0, m_nodeName.size()); // we resize the nodename to do the match.
            msg << MSG::DEBUG << "nodeName="<< nodeName << endreq;
          }
        }
      }
      else { // This is a Saver
        msg << MSG::DEBUG << "checking taskname= "<< taskName << endreq;
        if ((taskName.compare("Adder")!=0)&&(taskName.compare("ADDER")!=0)) {
          msg << MSG::DEBUG << "REFUSED because savers can save only adders. Taskname= "<< taskName << endreq;
          continue; // Savers can save only one Adder results
        }
        if (nodeName.size() !=  m_nodeName.size()){
          //msg << MSG::DEBUG << "REFUSED because it is saving adders and the node name size has to be the same."<< endreq;
          //continue; // In the case of servers the nodeName has to be the same
        }
      }
    }

    if (nodeName.compare("Bridge")==0) {
      std::string subfarmName = taskName;
      bool chooseIt=true;
      if (m_subfarmName.size()) chooseIt=false;
      for(it=m_subfarmName.begin(); it!=m_subfarmName.end(); ++it){
        msg << MSG::DEBUG << "comparing subfarmName="<< subfarmName << " with "<< (*it) << endreq;
        if (Misc::findCaseIns((*it), subfarmName) != std::string::npos) {
          chooseIt=true;
          break;
        }
      }
      if (!chooseIt) {
        msg << MSG::DEBUG << "REFUSED because subfarmName NOT OK" << endreq;
        continue; 
      }
      else msg << MSG::DEBUG << "subfarmName OK" << endreq;
    }
    else {
      // checking the nodeName
      msg << MSG::DEBUG << "comparing nodeName="<< nodeName << " with "<< m_nodeName << endreq;    
      if (Misc::findCaseIns(m_nodeName, nodeName) == std::string::npos){
        msg << MSG::DEBUG << "REFUSED because nodeName NOT OK" << endreq;
        continue;
      }
      else msg << MSG::DEBUG << "nodeName OK" << endreq;


      if ((taskName.compare("Adder")!=0)&&(taskName.compare("ADDER")!=0)) {
        // when the server is not an adder we can check the task name here.

        bool chooseIt=true;
        if (m_taskName.size()) chooseIt=false;
        for(it=m_taskName.begin(); it!=m_taskName.end(); ++it){
          msg << MSG::DEBUG << "comparing taskName="<< taskName << " with "<< (*it) << endreq;
          if (Misc::findCaseIns((*it), taskName) != std::string::npos) {
            chooseIt=true;
            break;
          }
        }
        if (!chooseIt) {
          msg << MSG::DEBUG << "REFUSED because taskName NOT OK" << endreq;
          continue; 
        }
        else msg << MSG::DEBUG << "taskName OK" << endreq;
      }
    }
    std::string serverName = (*serverListTotIt).substr(0, (*serverListTotIt).find("@"));
    msg << MSG::DEBUG << "Server="<<serverName << " will be considered"<< endreq;
    serverList.insert(serverName);
  }
  
  return serverList;
}
