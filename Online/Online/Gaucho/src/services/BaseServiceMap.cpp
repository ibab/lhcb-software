#include "Gaucho/DimInfoMonObject.h"
#include "Gaucho/DimServiceMonObject.h"
#include "Gaucho/MonObjectCreator.h"
#include "Gaucho/MonObject.h"
#include "Gaucho/MonH1F.h"
#include "Gaucho/MonH1D.h"
#include "Gaucho/MonH2F.h"
#include "Gaucho/MonH2D.h"
#include "Gaucho/MonProfile.h"
#include "Gaucho/services/DimInfoServers.h"
#include "Gaucho/services/ProcessMgr.h"
#include "Gaucho/services/BaseServiceMap.h"
#include "Gaucho/services/Misc.h"
#include "TFile.h"


BaseServiceMap::BaseServiceMap(std::string name, ProcessMgr *processMgr): 
  m_name("BaseServiceMap"),
  m_processMgr(processMgr)
{
 
}

BaseServiceMap::~BaseServiceMap() {

}

IMessageSvc* BaseServiceMap::msgSvc() 
{
  return m_processMgr->msgSvc();
}

void BaseServiceMap::printServiceSet() {
 MsgStream msg(m_processMgr->msgSvc(), name());
 std::set<std::string>::iterator it;
 for (it = m_serviceSet.begin(); it != m_serviceSet.end(); ++it){
   msg << MSG::DEBUG << "Service " << *it << endreq;    
  }
}

void BaseServiceMap::updateMap(std::map<std::string, bool, std::less<std::string> > serverMap)
{
  MsgStream msg(msgSvc(), name());
  
  msg << MSG::DEBUG << "verifying if we have the ServiceSet seted" << endreq;
  if (0 == m_serviceSet.size()) return;
  msg << MSG::DEBUG << "verifying if current Map is equal to local map " << endreq;
  if (serverMap == m_serverMap) return;
  
  std::map<std::string, bool, std::less<std::string> >::iterator svrMapIt;
  
  msg << MSG::DEBUG << "printing local set services " << endreq;
  printServiceSet();
    
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  msg << MSG::DEBUG << " Excluding Servers from Maps" << endreq;
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  //printMap();
  for (svrMapIt=m_serverMap.begin() ; svrMapIt != m_serverMap.end(); ++svrMapIt){
    if (!svrMapIt->second) continue; // is already inactive in local dimInfo
    if (serverMap.find(svrMapIt->first)->second) continue; // Server active, we don't have to exclude
    msg << MSG::DEBUG << "   Server: " << svrMapIt->first << endreq;
    excludeServerInMaps(svrMapIt->first);
  }
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  msg << MSG::DEBUG << " Including Servers in Maps" << endreq;
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  for (svrMapIt=serverMap.begin() ; svrMapIt != serverMap.end(); ++svrMapIt){
    if (!svrMapIt->second) continue; // Server inactive, we don't have to include
    if (m_serverMap.find(svrMapIt->first) != m_serverMap.end()) // if it is in local we verify if its active
      if (m_serverMap.find(svrMapIt->first)->second) continue; // is already active in local dimInfo  
   
    msg << MSG::DEBUG << "   Server: " << svrMapIt->first << endreq;
    includeServerInMaps(svrMapIt->first);
  }
  
  m_serverMap = serverMap;
}

void BaseServiceMap::includeServerInMaps(const std::string &serverName) {
  MsgStream msg(msgSvc(), name());
  
  std::set<std::string>::iterator svcSetIt;  
  
  if (m_dimInfo.size() <= 0) {
    for (svcSetIt = m_serviceSet.begin(); svcSetIt != m_serviceSet.end(); ++svcSetIt) {
      msg << MSG::DEBUG << "Service " << *svcSetIt << endreq;    
      insertDimInfo(*svcSetIt, serverName);
      if (m_processMgr->isAdder()) {
        msg << MSG::DEBUG << "creating the Adder Service " << endreq;    
        insertDimService(*svcSetIt, serverName);
      }
    }
  }
  else {
    for (svcSetIt = m_serviceSet.begin(); svcSetIt != m_serviceSet.end(); ++svcSetIt) {
      msg << MSG::DEBUG << "Service " << *svcSetIt << endreq;    
      insertDimInfo(*svcSetIt, serverName);
    }
  }
} 

void BaseServiceMap::printMap() {
  MsgStream msg(msgSvc(), name());
  std::map<std::string, DimInfoMonObject*>::iterator it; 
  
  for (m_dimInfoIt = m_dimInfo.begin(); m_dimInfoIt != m_dimInfo.end(); ++m_dimInfoIt){
    msg << MSG::DEBUG << "Grupo: " << m_dimInfoIt->first  << endreq;
    for (it = m_dimInfoIt->second.begin(); it != m_dimInfoIt->second.end(); ++it) {
      msg << MSG::DEBUG << "         Elemento: " << it->first << endreq;
    }
  }
}

void BaseServiceMap::excludeServerInMaps(const std::string &serverName) {
  MsgStream msg(msgSvc(), name());

  std::set<std::string>::iterator svcSetIt;  
  
  for (svcSetIt = m_serviceSet.begin(); svcSetIt != m_serviceSet.end(); ++svcSetIt) {
    int numLastInfos = deleteDimInfo(*svcSetIt, serverName);
    if ( 0 == numLastInfos) {
      if (m_processMgr->isAdder()) deleteDimService(*svcSetIt);
    }
  }
} 

void BaseServiceMap::insertDimInfo(const std::string &serviceName, const std::string &serverName)
{
  MsgStream msg(msgSvc(), name());
  
  std::string termSvcName = createTermServiceName (serviceName, serverName);

  std::string groupName = "";
  std::string elementName = "";
  
  if (m_processMgr->isAdder()) {
    groupName = createAdderName (serviceName);
    elementName = serverName;
  }
  else {
    groupName = serverName;
    elementName = termSvcName;
  }
  
  msg << MSG::DEBUG << "creating DimInfoMonObject " << termSvcName << endreq;
  //msg << MSG::DEBUG << "groupName =  " << groupName << endreq;
  //msg << MSG::DEBUG << "elementName =  " << elementName << endreq;
  
  m_dimInfo[groupName][elementName] = new DimInfoMonObject(termSvcName, 5); 
  //msg << MSG::DEBUG << "setSourceName : " << name() << endreq;
  m_dimInfo[groupName][elementName]->setSourceName(name());
  //msg << MSG::DEBUG << "setMsgSvc" << endreq;
  m_dimInfo[groupName][elementName]->setMsgSvc(msgSvc());
      
  msg << MSG::DEBUG << "creating MonObject in DimInfoMonObject " << endreq;
  m_dimInfo[groupName][elementName]->createMonObject();
  if (!m_processMgr->isAdder()) {// If it's a Saver we also create the object
    if (0 != m_dimInfo[groupName][elementName]->monObject())
      m_dimInfo[groupName][elementName]->monObject()->createObject();
  }
}

int BaseServiceMap::deleteDimInfo(const std::string &serviceName, const std::string &serverName)
{
  MsgStream msg(msgSvc(), name());

  std::string termSvcName = createTermServiceName (serviceName, serverName);

  std::string groupName = "";
  std::string elementName = "";

  if (m_processMgr->isAdder()) {
    groupName = createAdderName (serviceName);
    elementName = serverName;
  }
  else {
    groupName = serverName;
    elementName = termSvcName;
  }

  if (m_dimInfo.find(groupName) == m_dimInfo.end()){
    msg << MSG::DEBUG << " sorry there is no group " << groupName << " in the map" << endreq;    
    return 1;
  }

  if (m_dimInfo[groupName].find(elementName) == m_dimInfo[groupName].end()){
    msg << MSG::DEBUG << " sorry there is no element " << elementName << "in group " << groupName << endreq;    
    return 1;
  }

  msg << MSG::DEBUG << " erasing element "<< elementName <<" from group "<< groupName <<endreq;
  msg << MSG::DEBUG << " the serviceName was: "<< m_dimInfo[groupName][elementName]->dimInfo()->getName() <<endreq;
  delete m_dimInfo[groupName][elementName]; m_dimInfo[groupName][elementName]=0; m_dimInfo[groupName].erase(elementName);

  if (m_dimInfo[groupName].size()!=0)  return 1;

  msg << MSG::DEBUG << " The group is empty after erasing element "<< elementName << ". We have to erase the group too." <<endreq;
  msg << MSG::DEBUG << " erasing group "<< groupName << " from map "<< endreq;  
  m_dimInfo.erase(groupName);

  return 0;
}

void BaseServiceMap::insertDimService(const std::string &serviceName, const std::string &serverName)
{

  MsgStream msg(msgSvc(), name());

  std::string adderName = createAdderName (serviceName);

  std::string groupName = "";
  std::string elementName = "";

  if (m_processMgr->isAdder()) {
    groupName = createAdderName (serviceName);
    elementName = serverName;
  }
  else {
    msg << MSG::WARNING << "This is not an Adder !!!!!!!!. You can not add DimServices." << endreq;
    return;  
  }
    
  MonObject *monObjectTmp = m_dimInfo[groupName][elementName]->monObject();
  //msg << MSG::DEBUG << "printing MonObjectTest: " << endreq;
  //monObjectTest->print();
  msg << MSG::DEBUG << "creating MonObject for Adder" << endreq;
  
  MonObject *monObjectAdder = MonObjectCreator::createMonObject(monObjectTmp->typeName(), msgSvc(), name());
  monObjectAdder->copyFrom(monObjectTmp);
  //monObjectAdder->print();
      
  msg << MSG::DEBUG << "creating DimServiceMonObject for Adder : " << groupName << endreq;
  DimServiceMonObject *dimServiceMonObjectAdder = new DimServiceMonObject(groupName, monObjectAdder);
      
  m_dimSrv[groupName] = std::pair<DimServiceMonObject*, MonObject*> (dimServiceMonObjectAdder, monObjectAdder);
}

void BaseServiceMap::deleteDimService(const std::string &serviceName){
  MsgStream msg(msgSvc(), name());
  std::string adderName = createAdderName (serviceName);
  
  std::string groupName = "";
  
  if (m_processMgr->isAdder()) groupName = createAdderName (serviceName);
  else {
    msg << MSG::WARNING << "This is not an Adder !!!!!!!!. You can not delete DimServices." << endreq;
    return;  
  }
  
  msg << MSG::DEBUG << " erasing published MonObject from map" <<endreq;  
  delete m_dimSrv[groupName].second; 
  m_dimSrv[groupName].second = 0;
  msg << MSG::DEBUG << " erasing DimService " << groupName << " from map" <<endreq;  
  delete m_dimSrv[groupName].first; 
  m_dimSrv[groupName].first = 0; 
  m_dimSrv.erase(groupName);
}


std::string BaseServiceMap::createTermServiceName (const std::string &serviceName, const std::string &serverName)
{
  MsgStream msg(msgSvc(), name());

  std::size_t first_slash = serviceName.find("/");
  std::size_t second_slash = serviceName.find("/", first_slash + 1);
  if (serverName.find("Bridge")!= std::string::npos) {
    std::vector<std::string> parts = Misc::splitString(serviceName, "/");
    std::vector<std::string> parts2 = Misc::splitString(parts[1], "_");
    std::string subfarmName = parts2[0];
    msg << MSG::DEBUG << " subfarmName = " << subfarmName <<endreq;  
    return serviceName.substr(0, first_slash + 1) + subfarmName + "_Adder_1" + serviceName.substr(second_slash);
  }
  else{
    return serviceName.substr(0, first_slash + 1) + serverName + serviceName.substr(second_slash);
  }

}    
    
std::string BaseServiceMap::createAdderName (const std::string &serviceName){
  // Creating the service name for the adder
  std::vector<std::string> serviceParts = Misc::splitString(serviceName, "/");
  std::string svctype = serviceParts[0];
  std::string utgid = serviceParts[1];
  std::string task = Misc::splitString(utgid, "_")[1];
    
  int index = 2;

  if ((task == "Adder")||(task == "ADDER")) {
    task = serviceParts[index];
    index++;
  }
      
  std::string algo = serviceParts[index];
  std::string object = serviceParts[index+1];
  for (unsigned int i = index+2; i < serviceParts.size(); ++i)
    object += "/" + serviceParts[i];

  
  std::string adderName = svctype + "/" + m_processMgr->utgid()  + "/" + task + "/";
  
  if ((s_pfixMonH1F==svctype)||(s_pfixMonH2F==svctype)||(s_pfixMonH1D==svctype)||(s_pfixMonH2D==svctype)||(s_pfixMonProfile==svctype)||
      (s_pfixMonRate==svctype))
    adderName = adderName + algo + "/" + object;
  else
    adderName = adderName + object;
  
  return adderName;
}

void BaseServiceMap::write(std::string saveDir, std::string &fileName, int &fileSize)
{
  MsgStream msg(msgSvc(), name());

  if ((0 == m_serverMap.size())||(0 == m_serviceSet.size())||(0 == m_dimInfo.size())) {
    msg << MSG::INFO << " Writer can't write because ServerMap, ServiceMap or DimInfoMap is empty " << endreq;
    return;
  }

  std::map<std::string, bool, std::less<std::string> > serverMap = m_processMgr->dimInfoServers()->serverMap();
  std::map<std::string, bool, std::less<std::string> >::iterator it;
  std::map<std::string, DimInfoMonObject*>::iterator it2;

  TFile *f=0;

  char timestr[64];
  time_t rawTime=time(NULL);
  struct tm* timeInfo = localtime(&rawTime);
  ::strftime(timestr, sizeof(timestr),"%Y%m%dT%H%M%S", timeInfo);

  for (it = serverMap.begin(); it != serverMap.end(); ++it){
    msg << MSG::INFO << "Server : " << it->first << endreq;

    if (!it->second) continue;

    fileSize=0;
    std::string tmpfile = saveDir + "/" +  it->first + "-" + timestr + ".root"; 
    fileName.replace(0, fileName.length(), tmpfile);
    msg << MSG::INFO << "SaverSvc will save histograms in file " << fileName << endreq;

    f = new TFile(fileName.c_str(),"create");
    //TFile f(fileName.c_str(),"create");

    msg << MSG::DEBUG << "Writing MonObjects"<< endreq;

    std::string groupName = it->first;

    for (it2 = m_dimInfo[groupName].begin(); it2 != m_dimInfo[groupName].end(); ++it2)
    {
      if(0 == it2->second->monObject()) continue;
      it2->second->loadMonObject();
      std::string type = it2->second->monObject()->typeName();
      msg << MSG::DEBUG << " Service " << it2->first << " is a " << type <<endreq;
      if ((s_monH1F == type)||(s_monH1D == type)||(s_monH1F == type)||(s_monH2D == type)||(s_monProfile == type)) {
        it2->second->monObject()->loadObject();
        it2->second->monObject()->write();
      }
      else {
        msg << MSG::ERROR << "MonObject of type " << type << " can not be writed."<< endreq; 
      }
    }
    fileSize=(int)f->GetSize();
    f->Close();
    delete f;f=0;
  }
}


void BaseServiceMap::add() {
  MsgStream msg(msgSvc(), name());
  
  std::map<std::string, DimInfoMonObject*>::iterator it;

  if ((0 == m_serverMap.size())||(0 == m_serviceSet.size())||(0 == m_dimInfo.size())){
    msg << MSG::INFO << " Adder can't add because the ServerMap, ServiceMap or DimInfoMap is empty " << endreq;
    return;
  }

  for (m_dimInfoIt=m_dimInfo.begin(); m_dimInfoIt!=m_dimInfo.end(); ++m_dimInfoIt) 
  {
    std::cout << "========================ADDER=================================" << std::endl;
    msg << MSG::DEBUG << " Adder : " << m_dimInfoIt->first << endreq;
    // reset MonObject
    if (m_dimSrv.find(m_dimInfoIt->first) == m_dimSrv.end()) { 
      msg << MSG::DEBUG << "No Adder Found " << m_dimInfoIt->first << endreq;
      continue;
    }
    
    m_dimSrv[m_dimInfoIt->first].second->reset();
    
    // checking endOfRun Flag
    msg << MSG::DEBUG << "Adder EndOfRun Flag=  " << m_dimSrv[m_dimInfoIt->first].second->endOfRun() << endreq;
    
    for (it=m_dimInfoIt->second.begin(); it!=m_dimInfoIt->second.end(); ++it) {
      if (!m_processMgr->dimInfoServers()->isActive(it->first)) continue;
      if(0 == it->second->monObject()) continue;
      //msg << MSG::DEBUG << "Term : " << it->first << endreq;
      msg << MSG::DEBUG << "Term : " << it->second->dimInfo()->getName() << endreq;

      it->second->loadMonObject();
      // it->second->createMonObject();
      msg << MSG::DEBUG << "Term EndOfRun Flag=  " << it->second->monObject()->endOfRun() << endreq;
      // if (m_dimSrv[m_dimInfoIt->first].second->endOfRun() != it->second->monObject()->endOfRun()) continue; 
      // std::cout << "==============================================================" << std::endl;
      // std::cout << "==============================================================" << std::endl;
      // it->second->monObject()->print();
      // std::cout << "==============================================================" << std::endl;
      m_dimSrv[m_dimInfoIt->first].second->combine(it->second->monObject());
    }
    //m_dimSrv[m_dimInfoIt->first].second->print();
    m_dimSrv[m_dimInfoIt->first].first->updateServiceMonObject(m_dimSrv[m_dimInfoIt->first].second->endOfRun());
    
    std::cout << "==============================================================" << std::endl;
    
/*  std::cout << "==============================================================" << std::endl;
  std::cout << "========================ADDER=================================" << std::endl;
  std::cout << "==============================================================" << std::endl;
  m_dimSrv[m_dimInfoIt->first].second->print();
  std::cout << "==============================================================" << std::endl;
  std::cout << "========================ADDERB=================================" << std::endl;
  std::cout << "==============================================================" << std::endl;
  m_dimSrv[m_dimInfoIt->first].first->monObject()->print();
  std::cout << "==============================================================" << std::endl;*/
  }
}

/*
void BaseServiceMap::createMap(std::map<std::string, bool, std::less<std::string> > serverMap, std::set<std::string> serviceSet) 
{
  MsgStream msg(msgSvc(), name());
  
  m_serverMap = serverMap;
  m_serviceSet = serviceSet;
  
  printLocalSet();
  
  std::map<std::string, bool, std::less<std::string> >::iterator svrMapIt;
  
  for (svrMapIt = m_serverMap.begin(); svrMapIt != m_serverMap.end(); ++svrMapIt) {  
    if (!svrMapIt->second) continue; // inactive Server
    includeServerInMaps(svrMapIt->first);
  }
}
*/
