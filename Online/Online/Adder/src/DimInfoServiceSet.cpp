#include "DimInfoServiceSet.h"
#include "Misc.h"

DimInfoServiceSet::DimInfoServiceSet(
                      std::set<std::string> *serviceSet,
                      bool *updateTimerAdder,
                      std::string serverName,
                      std::vector<std::string> taskName,
                      std::vector<std::string> algorithmName,
                      std::vector<std::string> objectName,
		      IMessageSvc* msgSvc):
  DimInfo((serverName + "/SERVICE_LIST").c_str(), -1),
  m_serviceSet(serviceSet),
  m_updateTimerAdder(updateTimerAdder),
  m_serverName(serverName),
  m_taskName(taskName),
  m_algorithmName(algorithmName),
  m_objectName(objectName),
  m_msgSvc(msgSvc)
{
  m_name = "DimInfoServiceSet";
  MsgStream msg(msgSvc, name());
  //msg << MSG::DEBUG << "DimInfoServiceSet subscribed to "<< serverName << "/SERVICE_LIST" << endreq;
  std::size_t first_us = m_serverName.find("_");
  m_nodeName = m_serverName.substr(0, first_us);
}

DimInfoServiceSet::~DimInfoServiceSet(){
  
  
}

void DimInfoServiceSet::infoHandler()
{
  MsgStream msg(msgSvc(), name());
  //msg << MSG::DEBUG << "inside DimInfoServiceSet::infoHandler" << endreq;
 
  int tmpStringSize = -1;
  while ( (tmpStringSize = getSize()) <=0 ){usleep(10000);}

  msg << MSG::DEBUG << "*********************************************"<< endreq;
  msg << MSG::DEBUG << "Verifying changes in Service Set"<< endreq;
 
  msg << MSG::DEBUG << "DimService = " << m_serverName + "/SERVICE_LIST"<< endreq;
  std::string serviceListS = getString();
  //msg << MSG::DEBUG << "serviceListS = " << serviceListS<< endreq;
  std::vector<std::string> serviceList = Misc::splitString(serviceListS, "\n");
  std::vector<std::string>::const_iterator serviceListIt;

  for(serviceListIt=serviceList.begin(); serviceListIt!=serviceList.end(); ++serviceListIt){
    std::string serviceName = (*serviceListIt).substr(0, (*serviceListIt).find("|"));
    msg << MSG::DEBUG << "Verifying service = " << serviceName<< endreq;
    if (serviceName.find("/" + m_nodeName) == std::string::npos) continue;

    std::vector<std::string> serviceParts = Misc::splitString(serviceName, "/");

    //it should be possible to have an unlimited number of slashes im the histogram names
    //if (4 != serviceParts.size() ) continue; 

    std::string svctype = serviceParts[0];
    std::string utgid = serviceParts[1];
    std::string algo = serviceParts[2];
    std::string object = serviceParts[3];
    std::string task = Misc::splitString(utgid, "_")[1];

    for(unsigned int i = 0; i < m_objectName.size(); i++){
      //std::cout << "task match : " << m_taskName[i] << " vs " << task << std::endl;
      if (Misc::findCaseIns(m_taskName[i], task) == std::string::npos) continue;
      //std::cout << "algorithm match : " << m_algorithmName[i] << " vs " << algo  << std::endl;
      if (Misc::findCaseIns(m_algorithmName[i], algo) == std::string::npos) continue;
      //std::cout << "object match : " << m_objectName[i] << " vs " << object  << std::endl;
      if (m_objectName[i].find("*", 0) != std::string::npos) {
        std::vector<std::string> objectNameParts = Misc::splitString(m_objectName[i], "*");
        if (!Misc::matchStringCaseIns(object, objectNameParts)) continue;
      }
      else{// No wildcard, then object must be equal to m_objectName[i]
        if (Misc::findCaseIns(m_objectName[i], object) == std::string::npos) continue;
      }
      msg << MSG::DEBUG << "service included => " << serviceName << endreq;
      m_serviceSet->insert(serviceName);
      break;
    }
  }
  (*m_updateTimerAdder) = true;
  msg << MSG::DEBUG << "*********************************************"<< endreq;
}

