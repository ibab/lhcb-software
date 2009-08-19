#include "Gaucho/DimInfoMonObject.h"
#include "Gaucho/DimServiceMonObject.h"
#include "Gaucho/MonObjectCreator.h"
#include "Gaucho/MonObject.h"
#include "Gaucho/MonH1F.h"
#include "Gaucho/MonH1D.h"
#include "Gaucho/MonH2F.h"
#include "Gaucho/MonH2D.h"
#include "Gaucho/MonProfile.h"
#include "Gaucho/MonRate.h"
#include "Gaucho/MonStatEntity.h"
#include "Gaucho/DimInfoServers.h"
#include "Gaucho/ProcessMgr.h"
#include "Gaucho/BaseServiceMap.h"
#include "Gaucho/Misc.h"
#include "Gaucho/MonRateDecoder.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TSystem.h"

BaseServiceMap::BaseServiceMap(ProcessMgr *processMgr): 
  m_name("BaseServiceMap"),
  m_processMgr(processMgr)
{

}

BaseServiceMap::~BaseServiceMap() {
  removeMap();
}

void BaseServiceMap::removeMap() {
  MsgStream msg(m_processMgr->msgSvc(), name());
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  msg << MSG::DEBUG  << " Removing Servers from maps because destructor called"      << endreq;
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  std::map<std::string, bool, std::less<std::string> >::iterator svrMapIt;
  for (svrMapIt=m_serverMap.begin() ; svrMapIt != m_serverMap.end(); ++svrMapIt){
    if (!svrMapIt->second) continue; // is already inactive in local dimInfo
    msg << MSG::DEBUG << "   Removing Server : " << svrMapIt->first << "." << endreq;
    excludeServerInMaps(svrMapIt->first);
  }
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
  msg << MSG::DEBUG << "m_dimInfo.size() = " << m_dimInfo.size()<< endreq;    

  bool insertDimSvc = false;

  if (m_processMgr->serviceOwner() == s_Adder) {
    if (m_dimInfo.size() <= 0) insertDimSvc = true;
  }

  for (svcSetIt = m_serviceSet.begin(); svcSetIt != m_serviceSet.end(); ++svcSetIt) {
    msg << MSG::DEBUG << "Service " << *svcSetIt << endreq;    
    insertDimInfo(*svcSetIt, serverName);
  }

  for (svcSetIt = m_serviceSet.begin(); svcSetIt != m_serviceSet.end(); ++svcSetIt) {
    msg << MSG::DEBUG << "Service " << *svcSetIt << endreq;    
    loadDimInfo(*svcSetIt, serverName);
    if (insertDimSvc) {
      msg << MSG::DEBUG << "creating the Adder Service " << endreq;    
      insertDimService(*svcSetIt, serverName);
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
      if (m_processMgr->serviceOwner() == s_Adder) deleteDimService(*svcSetIt);
    }
  }
} 

void BaseServiceMap::insertDimInfo(const std::string &serviceName, const std::string &serverName)
{
  MsgStream msg(msgSvc(), name());

  std::string termSvcName = createTermServiceName (serviceName, serverName);

  std::string groupName = "";
  std::string elementName = "";
  
  if (m_processMgr->serviceOwner() == s_Adder){
    groupName = createAdderName (serviceName);
    elementName = serverName;
  }
  else {
    termSvcName = serviceName;
    groupName = createSaverName (termSvcName);// in the case of Saving Adders it should return "serverName"
    elementName = termSvcName;
  }
  
  msg << MSG::DEBUG << "creating DimInfoMonObject " << termSvcName << endreq;
  msg << MSG::DEBUG << "groupName =  " << groupName << endreq;
  msg << MSG::DEBUG << "elementName =  " << elementName << endreq;
  
  m_dimInfo[groupName][elementName] = new DimInfoMonObject(termSvcName, m_processMgr->refreshTime()); 
  //msg << MSG::DEBUG << "setSourceName : " << name() << endreq;
  m_dimInfo[groupName][elementName]->setSourceName(name());
  //msg << MSG::DEBUG << "setMsgSvc" << endreq;
  m_dimInfo[groupName][elementName]->setMsgSvc(msgSvc());

}

void BaseServiceMap::loadDimInfo(const std::string &serviceName, const std::string &serverName)
{
  MsgStream msg(msgSvc(), name());
  
  std::string termSvcName = createTermServiceName (serviceName, serverName);

  std::string groupName = "";
  std::string elementName = "";
  
  if (m_processMgr->serviceOwner() == s_Adder) {
    groupName = createAdderName (serviceName);
    elementName = serverName;
  }
  else {
    termSvcName = serviceName;
    groupName = createSaverName (termSvcName);// in the case of Saving Adders it should return "serverName"
    elementName = termSvcName;
  }
  
  msg << MSG::DEBUG << "creating MonObject in DimInfoMonObject " << termSvcName << endreq;
  
  bool created = false;  
  int maxtries=100;
  int tries=0;
  while (!created) {
    created = m_dimInfo[groupName][elementName]->createMonObject();  
    tries++;
    if (tries > maxtries) break;
  }
  
  msg << MSG::DEBUG << "after creating MonObject in DimInfoMonObject " << endreq;
  if (m_processMgr->serviceOwner() == s_Saver)
  {// If it's a Saver we also create the object
    if (0 != m_dimInfo[groupName][elementName]->monObject()){ 
      
      std::vector<std::string> serviceParts = Misc::splitString(termSvcName, "/");
      std::string newName="";
      int n = serviceParts.size();

      newName = serviceParts[2];
      for (int i=3; i < n; i++) {
        newName = newName + "/" + serviceParts[i];
      }

      std::string svctype = serviceParts[0];

      if (svctype.compare(s_pfixMonH1F) == 0){
       // newName = ((MonH1F*) (m_dimInfo[groupName][elementName]->monObject()))->histName();
        // newName = m_processMgr->taskName() + "/"+ newName;
          ((MonH1F*) (m_dimInfo[groupName][elementName]->monObject()))->createObject(newName);
      } else if (svctype.compare(s_pfixMonH1D) == 0){
       // newName = ((MonH1D*) (m_dimInfo[groupName][elementName]->monObject()))->histName();
       // newName = m_processMgr->taskName() + "/"+ newName;
        ((MonH1D*) (m_dimInfo[groupName][elementName]->monObject()))->createObject(newName);
      } else if (svctype.compare(s_pfixMonH2F) == 0){
       // newName = ((MonH2F*) (m_dimInfo[groupName][elementName]->monObject()))->histName();
       // newName = m_processMgr->taskName() + "/"+ newName;
       ((MonH2F*) (m_dimInfo[groupName][elementName]->monObject()))->createObject(newName);
      } else if (svctype.compare(s_pfixMonH2D) == 0){
       // newName = ((MonH2D*) (m_dimInfo[groupName][elementName]->monObject()))->histName();
       // newName = m_processMgr->taskName() + "/"+ newName;
        ((MonH2D*) (m_dimInfo[groupName][elementName]->monObject()))->createObject(newName);
      } else if (svctype.compare(s_pfixMonProfile) == 0){
      // newName = ((MonProfile*) (m_dimInfo[groupName][elementName]->monObject()))->profileName();
       // newName = m_processMgr->taskName() + "/"+ newName;
       ((MonProfile*) (m_dimInfo[groupName][elementName]->monObject()))->createObject(newName);
      } else {
        msg << MSG::WARNING << "MonObject not included in the Saver: " << svctype << endreq;
      }
    }
  }
}

int BaseServiceMap::deleteDimInfo(const std::string &serviceName, const std::string &serverName)
{
  MsgStream msg(msgSvc(), name());

  std::string termSvcName = createTermServiceName (serviceName, serverName);

  std::string groupName = "";
  std::string elementName = "";

  if (m_processMgr->serviceOwner() == s_Adder) {
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

  if (m_processMgr->serviceOwner() == s_Adder) {
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
  msg << MSG::DEBUG << "creating MonObject for Adder" << groupName << endreq;

  std::vector<std::string> serviceParts = Misc::splitString(serviceName, "/");
  std::string svctype = serviceParts[0];

  MonObject *monObjectAdder = MonObjectCreator::createMonObjectWithPrefix(svctype, msgSvc(), name());

  msg << MSG::DEBUG << "copy MonObject for Adder" << groupName << endreq;

  bool isCopied = false;
  if (monObjectTmp !=0) {
    monObjectAdder->copyFrom(monObjectTmp);
    isCopied = true;
  }

  //monObjectAdder->print();
  msg << MSG::DEBUG << "creating DimServiceMonObject for Adder : " << groupName << endreq;
  DimServiceMonObject *dimServiceMonObjectAdder = new DimServiceMonObject(groupName, monObjectAdder);

  m_dimSrv[groupName] = std::pair<DimServiceMonObject*, MonObject*> (dimServiceMonObjectAdder, monObjectAdder);
  m_dimSrvStatus[groupName] = std::pair<bool, std::string> (isCopied, elementName);

  if (svctype.compare (s_pfixMonRate) == 0) {
    if (m_processMgr->publishRates())  {
      m_monRateDecoder = new MonRateDecoder(m_processMgr->msgSvc(), m_processMgr->utgid(), groupName);
    }
  }
}

void BaseServiceMap::deleteDimService(const std::string &serviceName){
  MsgStream msg(msgSvc(), name());
  std::string adderName = createAdderName (serviceName);
  
  std::string groupName = "";
  
  if (m_processMgr->serviceOwner() == s_Adder) groupName = createAdderName (serviceName);
  else {
    msg << MSG::WARNING << "This is not an Adder !!!!!!!!. You can not delete DimServices." << endreq;
    return;  
  }
  
  if (m_dimSrv[groupName].second->typeName().compare (s_monRate) == 0) {
    if (m_processMgr->publishRates())  {
      msg << MSG::DEBUG << " erasing MonRateDecoder" <<endreq;
      if (m_monRateDecoder != 0) {
        delete m_monRateDecoder; m_monRateDecoder = 0;
      }
    }
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
    //std::vector<std::string> parts = Misc::splitString(serviceName, "/");
    //std::vector<std::string> parts2 = Misc::splitString(parts[1], "_");
    //std::string subfarmName = parts2[0];
    std::vector<std::string> parts = Misc::splitString(serverName, "_");
    std::string subfarmName = Misc::stringToUpper(parts[1]);
    
   // subfarmName=m_processMgr->getPartitionName(); 
    msg << MSG::DEBUG << " subfarmName = " << subfarmName <<endreq; 

    return serviceName.substr(0, first_slash + 1) + subfarmName + "_Adder_1" + serviceName.substr(second_slash);
  }
  else{
   return serviceName.substr(0, first_slash + 1) + serverName + serviceName.substr(second_slash);
   // for saver the partition is in the utgid position
   // return serviceName;
  }

}    
    
std::string BaseServiceMap::createAdderName (const std::string &serviceName){
  // Creating the service name for the adder
  std::vector<std::string> serviceParts = Misc::splitString(serviceName, "/");
  std::string svctype = serviceParts[0];
  std::string utgid = serviceParts[1];
  std::string task = Misc::splitString(utgid, "_")[1];
  std::string farm=m_processMgr->getFarm();
  if (farm=="MF") task = "RecBrunel" ;
    
  int index = 2;

  if ((task == "Adder")||(task == "ADDER")) {
    task = serviceParts[index];
    index++;
  }
      
  std::string algo = serviceParts[index];
  std::string object = serviceParts[index+1];
  for (unsigned int i = index+2; i < serviceParts.size(); ++i)
    object += "/" + serviceParts[i];

  std::string partitionName = Misc::splitString(m_processMgr->utgid(),"_")[0];
  std::string adderName="";
  if (partitionName.find("PART")!= std::string::npos) {
    adderName = svctype + "/" + m_processMgr->getPartitionName() + "_Adder_1/" + task + "/";
  }
  else adderName = svctype + "/" + m_processMgr->utgid()  + "/" + task + "/";
  
  if ((s_pfixMonH1F==svctype)||(s_pfixMonH2F==svctype)||(s_pfixMonH1D==svctype)||(s_pfixMonH2D==svctype)||(s_pfixMonProfile==svctype)||(s_pfixMonRate==svctype)||(s_pfixMonStatEntity==svctype))
    adderName = adderName + algo + "/" + object;
  else
    adderName = adderName + object;
  
  return adderName;
}

std::string BaseServiceMap::createSaverName (const std::string &serviceName){
  // Creating the task name for the saver
  std::vector<std::string> serviceParts = Misc::splitString(serviceName, "/");
  std::string svctype = serviceParts[0];
  std::string utgid = serviceParts[1];
//  std::string utgid = serviceParts[0];
  std::vector<std::string> utgidParts = Misc::splitString(utgid, "_");
  if (utgidParts.size() == 4) {
    std::string partName = utgidParts[0];
    std::string taskName = utgidParts[2];
    return partName + "-" + taskName;
  }
 // if (utgidParts[0].find("PART")!= std::string::npos) {
 //   return m_processMgr->getPartitionName() + "_Saver_1/";
 // }
  return utgid; // it should be equal to the serverName
}



void BaseServiceMap::write(std::string saveDir, std::string &fileName)
{
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << " We will try to Write " << endreq;

  if ((0 == m_serverMap.size())||(0 == m_serviceSet.size())||(0 == m_dimInfo.size())) {
    msg << MSG::DEBUG << " Writer can't write because ServerMap, ServiceMap or DimInfoMap is empty " << endreq;
    return;
  }

  std::map<std::string, DimInfoMonObject*>::iterator it;

  TFile *f=0;

  char timestr[64];
  time_t rawTime=time(NULL);
  struct tm* timeInfo = localtime(&rawTime);
  ::strftime(timestr, sizeof(timestr),"%Y%m%dT%H%M%S", timeInfo);

  for (m_dimInfoIt=m_dimInfo.begin(); m_dimInfoIt!=m_dimInfo.end(); ++m_dimInfoIt) 
  {
    
     std::vector<std::string> serviceParts = Misc::splitString(m_dimInfoIt->first, "_");
     std::string farm=m_processMgr->getFarm();
     std::string taskName="Moore";
     if (farm=="MF") {
       taskName = "RecBrunel";
     }  
     
 /*    if (3 == serviceParts.size()) {
       taskName = "Moore";
     }
     else if (4 == serviceParts.size()) {
       taskName = serviceParts[2];
     }*/

    
   // std::string tmpfile = saveDir + m_dimInfoIt->first + "-" + timestr + ".root";
    std::string tmpfile = saveDir + taskName + "/" + taskName + "-" + timestr + ".root";
    fileName.replace(0, fileName.length(), tmpfile);
    msg << MSG::INFO << "SaverSvc will save histograms in file " << fileName << endreq;

/*    std::string dirName = saveDir + taskName + "/" + taskName;  
    void *hdir = gSystem->OpenDirectory(dirName.c_str());
    if (hdir == 0) {
     gSystem->mkdir(dirName.c_str(),true);
    }*/

    f = new TFile(fileName.c_str(),"create");
    
    for (it=m_dimInfoIt->second.begin(); it!=m_dimInfoIt->second.end(); ++it) {
      std::string serverName = Misc::splitString(it->first, "/")[1];
      msg << MSG::DEBUG << "Term : " << it->first << " servername " << serverName << endreq;
      //EvH I commented this line out for the moment because the servername is now the partitionname
      //(for the presenter) - not that slice name PARTxx
      //if (!m_processMgr->dimInfoServers()->isActive(serverName)) continue;
     if(0 == it->second->monObject()) continue;

      msg << MSG::DEBUG << "Term being saved: " << it->second->dimInfo()->getName() << endreq;

      std::string type = it->second->monObject()->typeName();
      std::vector<std::string> HistoFullName = Misc::splitString(it->second->dimInfo()->getName(), "/");  
  
      if ((s_monH1F == type)||(s_monH1D == type)||(s_monH2F == type)||(s_monH2D == type)||(s_monProfile == type)) {
          TDirectory *dir=0; 
          it->second->loadMonObject();
          it->second->monObject()->loadObject();
	 // std::vector<std::string> HistoDirName;  
	  for (int i=3;i<(int)HistoFullName.size()-1;i++) {
	     //recreate the directory structure inside the root file before saving
             //HistoDirName keeps track of the directory names, to avoid rewriting them
	        if (i>3) {   
		    if (dir->GetDirectory(HistoFullName[i].c_str())) { 
		          msg << MSG::DEBUG << "cding 1 into " << HistoFullName[i].c_str()<< endreq; 
		          dir->cd(HistoFullName[i].c_str());}
		    else {
		       dir=dir->mkdir(HistoFullName[i].c_str(),TString::Format("subsubdir %02d",i)); 
		       dir->cd();
		    }   	
	        }   		    		 
	        else {
		  if (dir) {
		      dir=f->GetDirectory(HistoFullName[i].c_str());
		      msg << MSG::DEBUG << "cding 2 into " << HistoFullName[i].c_str()<< endreq; 
		      dir->cd(HistoFullName[i].c_str());		       
		  }    
		  else { 
		      if (dir=f->GetDirectory(HistoFullName[i].c_str())) {}		         
		      else {
		      msg << MSG::DEBUG << "Making directory HistoFullName[" << i << "]= " << HistoFullName[i].c_str() <<endreq; 		      
		      dir=f->mkdir(HistoFullName[i].c_str(),TString::Format("subdir %02d",i));
		      }
		  }   
	        }
         }   
	 if (dir) { 	  
	     dir->cd();	 
	 }	
	 else {	  
	     f->cd();
	 }
	  msg << MSG::DEBUG << "writing " << endreq; 
	
          it->second->monObject()->write();

      }
      else {
        msg << MSG::ERROR << "MonObject of type " << type << " can not be written."<< endreq; 
      }
    }
  
    f->Close();
    delete f;f=0;
  }
  
  msg << MSG::INFO << "SaverSvc saved histograms in file " << fileName << endreq;
  
}


void BaseServiceMap::add() {
  MsgStream msg(msgSvc(), name());
  
  std::map<std::string, DimInfoMonObject*>::iterator it;

  if (0 == m_serverMap.size()){
    msg << MSG::DEBUG << " Adder can't add because the ServerMap is empty " << endreq;
    return;
  }
  if (0 == m_serviceSet.size()) {
    msg << MSG::DEBUG << " Adder can't add because the ServiceSet is empty " << endreq;
    return;
  }
  if (0 == m_dimInfo.size()){
    msg << MSG::DEBUG << " Adder can't add because the DimInfoMap is empty " << endreq;
    return;
  }

  for (m_dimInfoIt=m_dimInfo.begin(); m_dimInfoIt!=m_dimInfo.end(); ++m_dimInfoIt) 
  {
    msg << MSG::DEBUG << " Adder : " << m_dimInfoIt->first << endreq;

    if (m_dimSrv.find(m_dimInfoIt->first) == m_dimSrv.end()) { 
      msg << MSG::DEBUG << "No Adder Found " << m_dimInfoIt->first << endreq;
      continue;
    }

    if (!m_dimSrvStatus[m_dimInfoIt->first].first){
      MonObject *monObjectTmp = m_dimInfo[m_dimInfoIt->first][m_dimSrvStatus[m_dimInfoIt->first].second]->monObject();
      if (monObjectTmp !=0) {
        m_dimSrv[m_dimInfoIt->first].second->copyFrom(monObjectTmp);
        m_dimSrvStatus[m_dimInfoIt->first].first = true;
      }
      else {
        msg << MSG::DEBUG << "Adder MonObject still unloaded for " << m_dimInfoIt->first << endreq;
        continue;
      }
    }

    m_dimSrv[m_dimInfoIt->first].second->reset();
    //msg << MSG::DEBUG << "Adder EndOfRun Flag=  " << m_dimSrv[m_dimInfoIt->first].second->endOfRun() << endreq;
    for (it=m_dimInfoIt->second.begin(); it!=m_dimInfoIt->second.end(); ++it) {
      if (!m_processMgr->dimInfoServers()->isActive(it->first)) continue;

      if(0 == it->second->monObject()) continue;
      msg << MSG::DEBUG << "Loading term : " << it->first << endreq;
      bool isLoaded = it->second->loadMonObject();
      if (!isLoaded){
        msg << MSG::DEBUG << "Term : " << it->second->dimInfo()->getName() << " unloaded" << endreq;
        continue;
      }
      msg << MSG::DEBUG << "Term : " << it->second->dimInfo()->getName() << " ok" << endreq;

      if (m_dimSrv[m_dimInfoIt->first].second->typeName().compare(s_monRate) == 0){
        int countersAdder = ((MonRate*) (m_dimSrv[m_dimInfoIt->first].second))->numCounters();
        int countersTerm = ((MonRate*) it->second->monObject())->numCounters();
        if ( countersAdder < countersTerm) {
          ((MonProfile*) (m_dimSrv[m_dimInfoIt->first].second))->synchronizeLabelNames(it->second->monObject());
          ((MonRate*) (m_dimSrv[m_dimInfoIt->first].second))->setNumCounters(((MonRate*)it->second->monObject())->numCounters());
        }
      }
      //msg << MSG::DEBUG << "Combining Service : " << endreq;
      m_dimSrv[m_dimInfoIt->first].second->combine(it->second->monObject());
    }
    //msg << MSG::DEBUG << "Updating Service : " << endreq;
    m_dimSrv[m_dimInfoIt->first].first->updateService(m_dimSrv[m_dimInfoIt->first].second->endOfRun());

    if (m_dimSrv[m_dimInfoIt->first].second->typeName().compare(s_monRate) == 0){
      if (m_processMgr->publishRates()) m_monRateDecoder->update((MonRate*)(m_dimSrv[m_dimInfoIt->first].second));
    }
  }
}

