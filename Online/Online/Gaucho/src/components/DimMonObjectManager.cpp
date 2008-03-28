#include "Gaucho/DimMonObjectManager.h"
#include "Gaucho/DimTimerMonObjectAdder.h"
#include "Gaucho/DimServiceMonObject.h"
#include "Gaucho/DimInfoMonObject.h"
#include "Gaucho/MonH1F.h"
#include "Gaucho/MonH1D.h"
#include "Gaucho/MonH2F.h"
#include "Gaucho/MonH2D.h"
#include "Gaucho/MonBool.h"
#include "Gaucho/MonInt.h"
#include "Gaucho/MonLong.h"
#include "Gaucho/MonFloat.h"
#include "Gaucho/MonString.h"
#include "Gaucho/MonDouble.h"
#include "Gaucho/MonPairDD.h"
#include "Gaucho/MonPairII.h"
#include "Gaucho/MonPairDI.h"
#include "Gaucho/MonProfile.h"
#include "Gaucho/MonHitMap2D.h"
#include "Gaucho/MonStatEntity.h"
#include "Gaucho/MonVectorI.h"
#include "Gaucho/MonVectorD.h"

DimMonObjectManager::DimMonObjectManager(IMessageSvc* msgSvc, const std::string& source)
{
  m_msgSvc=msgSvc;
  m_name="DimMonObjectManager";
  m_source=source;
  int outputLevel = m_msgSvc->outputLevel(m_source);
  m_msgSvc->setOutputLevel(m_name, outputLevel); 
  MsgStream msgStream(m_msgSvc, m_name);
  //I dont know why code in setOutputLevel is comment;
}

DimMonObjectManager::~DimMonObjectManager(){}

void DimMonObjectManager::setDimClientDns(std::string dimClientDns){
  DimClient::setDnsNode(dimClientDns.c_str());
  m_dimBrowser = new DimBrowser();
}

void DimMonObjectManager::deleteDimBrowser(){
  if (0!=m_dimBrowser) delete m_dimBrowser;
}

void DimMonObjectManager::setDimBrowser(DimBrowser* dimBrowser){
  m_dimBrowser = dimBrowser;
}

void DimMonObjectManager::dimWait(){
  dim_wait();
}

MonObject* DimMonObjectManager::createMonObject(std::string  monObjectTypeName){
  MonObject* monObject=0;
  if (monObjectTypeName == s_monObject) monObject = new MonObject(m_msgSvc, m_source);
  if (monObjectTypeName == s_monH1F) monObject = new MonH1F(m_msgSvc, m_source);
  if (monObjectTypeName == s_monH1D) monObject = new MonH1D(m_msgSvc, m_source);
  if (monObjectTypeName == s_monH2F) monObject = new MonH2F(m_msgSvc, m_source);
  if (monObjectTypeName == s_monH2D) monObject = new MonH2D(m_msgSvc, m_source);
  if (monObjectTypeName == s_monProfile) monObject = new MonProfile(m_msgSvc, m_source);
  if (monObjectTypeName == s_monInt) monObject = new MonInt(m_msgSvc, m_source);
  if (monObjectTypeName == s_monDouble) monObject = new MonDouble(m_msgSvc, m_source);
  if (monObjectTypeName == s_monFloat) monObject = new MonFloat(m_msgSvc, m_source);
  if (monObjectTypeName == s_monString) monObject = new MonString(m_msgSvc, m_source);
  if (monObjectTypeName == s_monLong) monObject = new MonLong(m_msgSvc, m_source);
  if (monObjectTypeName == s_monBool) monObject = new MonBool(m_msgSvc, m_source);
  if (monObjectTypeName == s_monPairDD) monObject = new MonPairDD(m_msgSvc, m_source);
  if (monObjectTypeName == s_monPairII) monObject = new MonPairII(m_msgSvc, m_source);
  if (monObjectTypeName == s_monPairDI) monObject = new MonPairDI(m_msgSvc, m_source);
  //  if (monObjectTypeName == s_monHitMap2D) monObject = new MonHitMap2D(m_msgSvc, m_source);
  if (monObjectTypeName == s_monStatEntity) monObject = new MonStatEntity(m_msgSvc, m_source);
  if (monObjectTypeName == s_monVectorI) monObject = new MonVectorI(m_msgSvc, m_source);
  if (monObjectTypeName == s_monVectorD) monObject = new MonVectorD(m_msgSvc, m_source);
  return monObject;
}

void DimMonObjectManager::deleteMonObject(MonObject* monObject){
  delete monObject;
}


int DimMonObjectManager::monObjectSize(MonObject* monObject){
  std::stringstream m_ss;
  m_ss.str("");
  boost::archive::binary_oarchive oa(m_ss);
  monObject->save(oa, monObject->version());
  return m_ss.str().length();
}

void DimMonObjectManager::loadMonObject(char* c, int size, MonObject *monObject){
  std::stringstream is;
  is.rdbuf()->pubsetbuf(c, size);
  boost::archive::binary_iarchive *ia = new boost::archive::binary_iarchive(is);
  monObject->load(*ia, 0);
  delete ia;
}

void DimMonObjectManager::saveMonObject(char* c, int& size, MonObject *monObject){
  std::stringstream m_ss("");
  boost::archive::binary_oarchive oa(m_ss);
  monObject->save(oa, monObject->version());
  c = const_cast<char *>(m_ss.str().c_str());
  size = m_ss.str().length();
}

//MonitorSvc
void DimMonObjectManager::declServiceMonObject(std::string infoName, const MonObject* monObject){

  MsgStream mes(m_msgSvc, m_name);
  std::string typeName = monObject->typeName();
  std::string dimPrefix = monObject->dimPrefix();

  std::string dimSvcName = dimPrefix + "/"+m_utgid+"/"+infoName;

  std::pair<DimServiceMonObjectMapIt,bool> p = m_dimMonObjects.insert(DimServiceMonObjectMap::value_type(infoName,0));

  if (p.second) {

    MonObject* monObjectAux = const_cast<MonObject *>(monObject);
    std::stringstream ss; 
    boost::archive::binary_oarchive oa(ss);
    monObjectAux->save(oa, 0);
    int size = ss.str().length();
    // Maybe here we only have to do: (Not time to test it now....Nov 13) 
    // int size = monObjectSize(monObjectAux);
    m_dimMonObjects[infoName]=new DimServiceMonObject(this, dimSvcName, monObjectAux, size);

    mes << MSG::DEBUG << "New DimMonObject: " + dimSvcName << endreq;

  }
  else {
    mes << MSG::ERROR << "Already existing DimMonObject: " + dimSvcName << endreq;
  }
}

//MonitorSvc
void DimMonObjectManager::undeclServiceMonObject(std::string infoName){

  MsgStream mes(m_msgSvc, m_name);
  for (m_dimMonObjectsIt = m_dimMonObjects.begin(); m_dimMonObjectsIt != m_dimMonObjects.end(); m_dimMonObjectsIt++)
    mes << MSG::DEBUG << (*m_dimMonObjectsIt).first << endreq;

  m_dimMonObjectsIt = m_dimMonObjects.find(infoName);  

  if(m_dimMonObjectsIt != m_dimMonObjects.end()) {
    (*m_dimMonObjectsIt).second->deleteMonObject();
    delete (*m_dimMonObjectsIt).second;
    m_dimMonObjects.erase(m_dimMonObjectsIt);
    mes << MSG::DEBUG << "undeclSvc: Service " + infoName + " undeclared" << endreq;
    return;
  }

  mes << MSG::ERROR << "undeclSvc: No DimServiceMonObject found with the name:" + infoName << endreq;

}

//MonitorSvc
void DimMonObjectManager::updateServiceMonObject(std::string infoName, bool endOfRun){
  MsgStream mes(m_msgSvc, m_name);
  //for (m_dimMonObjectsIt = m_dimMonObjects.begin(); m_dimMonObjectsIt != m_dimMonObjects.end(); m_dimMonObjectsIt++)
  //  mes << MSG::DEBUG << (*m_dimMonObjectsIt).first << endreq;
  m_dimMonObjectsIt = m_dimMonObjects.find(infoName);  
  if(m_dimMonObjectsIt != m_dimMonObjects.end()) {
    (*m_dimMonObjectsIt).second->updateService(endOfRun);
    mes << MSG::DEBUG << "updateSvc: Service " + infoName + " updated" << endreq;
    return;
  }
  mes << MSG::WARNING << "updateSvc: No DimServiceMonObject found with the name:" + infoName << endreq;
}

//Saver
void DimMonObjectManager::saverCreateDimNames(std::string nodeName, std::vector<std::string> taskName, std::vector<std::string> algorithmName, std::vector<std::string> objectName){

  MsgStream mes(m_msgSvc, m_name);
  mes << MSG::DEBUG << "size : " << objectName.size() << endreq;
  if (0==m_msgSvc) {
    mes.activate();
    mes.doOutput();
  }

  for (int j=0; j<= (int)objectName.size()-1;j++) {
    //Ex--> MON/node00101_Test_1/GaudiExample/ExMonH1D
    std::string svcName = "*/" + nodeName+"*"+taskName[j] +"*"+algorithmName[j]+"/"+objectName[j];

    mes << MSG::INFO << "svcName=" + svcName  << endreq;
    m_dimBrowser->getServices(svcName.c_str());

    char *service;
    char *format;
    int type;
    int count=0;
    std::vector<std::string> dimInfoNamesVect1;

    while (count==0)
    {

      while (type = m_dimBrowser->getNextService(service, format))
      {
        std::string  serviceStr = service;//Ex--> MON/node00101_Test_1/GaudiExample/ExMonH1D
        mes << MSG::INFO << "Service found: " + serviceStr << endreq;
        dimInfoNamesVect1.push_back(serviceStr);
        count++;
      } 
      if (count>0) break;
      else lib_rtl_sleep(500);
    }

    /************************************************/
    m_dimInfoNamesVect2.push_back(dimInfoNamesVect1);
    /************************************************/
  }
  // print for test
  if(m_msgSvc->outputLevel(m_name) >= MSG::DEBUG){
    mes << MSG::DEBUG << "PRINTING DIM NAMES : " << endreq;
    for (int k = 0; k < (int) m_dimInfoNamesVect2.size(); k++) {
      for (int j = 0; j < (int) m_dimInfoNamesVect2[k].size(); j++) {
        mes << MSG::DEBUG << "	Info : " << m_dimInfoNamesVect2[k][j] << endreq;
      }
    }
  }
}

//Saver
void DimMonObjectManager::saverCreateMonObjects(int refreshTime) {
  MsgStream mes(m_msgSvc, m_name);
  for (int k = 0; k < (int) m_dimInfoNamesVect2.size(); k++) {
    std::vector<MonObject*> tmpMonObjectVect1;
    for (int i = 0; i < (int) m_dimInfoNamesVect2[k].size(); i++) {
      DimInfoMonObject* tmpDimInfoMonObject = new DimInfoMonObject(this, m_dimInfoNamesVect2[k][i], refreshTime);

      MonObject *tmpMonObject;
      int ntries=0;
      while (1)
      {
        if (tmpDimInfoMonObject->monObject() != 0){
          //tmpMonObject = tmpDimInfoMonObject->monObject();
          tmpMonObject = createMonObject(tmpDimInfoMonObject->monObject()->typeName());
	  tmpMonObject->copyFrom(tmpDimInfoMonObject->monObject());
          break;
        }
        ntries++;

        mes << MSG::WARNING <<"No MonObject found after " << ntries << " attempts." << endreq;
         if (0==m_msgSvc) {
           mes.activate();
           mes.doOutput();
         }

        if(ntries==10) break;
        lib_rtl_sleep(500); 
      }
      tmpMonObjectVect1.push_back(tmpMonObject);

      tmpDimInfoMonObject->deleteMonObject();
      delete tmpDimInfoMonObject;
    }
    m_monObjectVect2.push_back(tmpMonObjectVect1);
  }
}

//Saver
void DimMonObjectManager::saverWriteMonObjects(){
  for (int k = 0; k < (int) m_monObjectVect2.size(); k++) {
    for (int i = 0; i < (int) m_monObjectVect2[k].size(); i++) {
      m_monObjectVect2[k][i]->write();
    }
  }
}

//Saver
void DimMonObjectManager::saverDeleteMonObjects(){
  for (int k = 0; k < (int) m_monObjectVect2.size(); k++) {
    for (int i = 0; i < (int) m_monObjectVect2[k].size(); i++) {
      deleteMonObject(m_monObjectVect2[k][i]);
    }
  }
  m_monObjectVect2.clear();
}

// Adder
int DimMonObjectManager::serviceParts(std::string serviceStr, std::string &svctype, std::string &task,                                          std::string &utgid, std::string &algo, std::string &object){
  std::string::size_type first = serviceStr.find("/");
  std::string::size_type second = serviceStr.find("/",first+1);
  std::string::size_type third = serviceStr.find("/",second+1);
  std::string::size_type first_us = serviceStr.find("_",first+1);
  std::string::size_type second_us = serviceStr.find("_",first_us+1);

  if (first == std::string::npos) return 0;
  if ((first_us == std::string::npos)||(second_us == std::string::npos)) return -1;

  svctype = serviceStr.substr(0, first);
  task    = serviceStr.substr(first_us + 1, second_us - first_us - 1);
  if (second <= first)  return - 1;

  utgid   = serviceStr.substr(first + 1, second - first - 1);
  if (third <= second) return - 1;

  algo    = serviceStr.substr(second + 1, third - second - 1);
  object  = serviceStr.substr(third + 1);

  return 1;
}

// Adder
int DimMonObjectManager::adderCreateDimNames(std::vector<std::string> nodeNames,
                                              std::vector<std::string> taskName, 
                                              std::vector<std::string> algorithmName,
                                              std::vector<std::string> objectName, 
                                              std::string serverName){
  MsgStream mes(m_msgSvc, m_name);

  char *service;
  char *format;
  int type;

  std::vector<std::string> svcFoundVect1;
  std::vector<std::string> objectUniqueVect1; 
  std::vector<std::string> objectFoundVect1; 
  std::vector<std::string> algoFoundVect1; 
  std::vector<std::string> taskFoundVect1; 
  std::vector<std::string> svctypeFoundVect1; 

  for(int iNode=0; iNode!=(int)nodeNames.size();++iNode){  
    std::string svcName = "*/" + nodeNames[iNode] + "*";
    m_dimBrowser->getServices(svcName.c_str());
    while (type = m_dimBrowser->getNextService(service, format))
    {
      std::string serviceStr = service; //Ex--> MON/node00101_Test_1/GaudiExample/ExMonH1D

      mes << MSG::DEBUG << "iNode : " << iNode << " nodename: " << nodeNames[iNode] 
          << " serviceStr : " << serviceStr << endreq;

      if (serviceStr.find(nodeNames[iNode]+"_Adder_",0) != std::string::npos ) continue;

      std::string svctype;
      std::string task;
      std::string utgid;
      std::string algo;
      std::string object;
      
      int matchRes = DimMonObjectManager::serviceParts(serviceStr, svctype, task, utgid, algo, object);

      mes << MSG::DEBUG << "	svctype    : " << svctype << endreq;
      mes << MSG::DEBUG << "	task       : " << task << endreq;
      mes << MSG::DEBUG << "	utgid      : " << utgid << endreq;
      mes << MSG::DEBUG << "	algo       : " << algo << endreq;
      mes << MSG::DEBUG << "	object     : " << object << endreq;

      if (matchRes <= 0){ 
        if (matchRes < 0)
          mes << MSG::ERROR << "Service: " + serviceStr + " doesn't match whit MON/node_task_x/algoritm/histo and. Ignoring." 
              << endreq;
        continue;
      }

      for (int i = 0;i < (int) objectName.size(); i++) {//Ex --> ExMonH1D* or ExMonH1D
        mes << MSG::DEBUG << "	adderCreateDimNames: objectName " << i << " Request " + taskName[i] + " " +  algorithmName[i] + " " + objectName[i] + " " + serverName << endreq;

        if (task != taskName[i]) continue;
        
        if ("" != algo) {
          if (algo != algorithmName[i]) continue;
        }
        
        std::string::size_type foundStar = objectName[i].find("*", 0);
        if (foundStar == std::string::npos ) { // No wildcard, then histo must be equal to m_histogramname[i] ? ask Eric
          if (objectName[i] != object) {
            mes << MSG::DEBUG  << "              Don't find object " << objectName[i] << " in " << object << endreq;
            continue;
          }
        }
        else { // Wildcard, it works only when * is at the end of objectName[i]
          std::string::size_type objectfound = object.find(objectName[i].substr(0, foundStar));
          if (objectfound == std::string::npos) {
            mes << MSG::DEBUG  << "		Don't find object " << objectName[i].substr(0, foundStar) << " in " << object << endreq;
            continue;
          }
        }
        
        mes << MSG::DEBUG  << "		objectFoundVect1: Adding object " + object << endreq;
        objectFoundVect1.push_back(object);
        algoFoundVect1.push_back(algo);
        taskFoundVect1.push_back(task); 
        svctypeFoundVect1.push_back(svctype); 
        svcFoundVect1.push_back(serviceStr);
        
        // After that, there are breaks. ask Eric if one service belong to only one histo. 
        // If not, break must be changed to continue
        bool unique = true;
        for (int k = 0; k < (int) objectUniqueVect1.size(); k++){
          if (object == objectUniqueVect1[k]){
            unique = false;
            break; 
          }
        }
        if (unique) objectUniqueVect1.push_back(object);
        break; 
      }
    }
    //mes << MSG::DEBUG << "objectUniqueVect1[" << k << "] = " << objectUniqueVect1[k] << endreq;
  }
  
  //creating DimInfoNames and DimServiceNamesAdder Arrays
  for (int k = 0; k < (int) objectUniqueVect1.size(); k++) {
    std::vector<std::string> dimInfoNamesVect1;
    std::string svctype;
    std::string task;
    std::string algo;
    for (int i = 0; i < (int) objectFoundVect1.size(); i++) {
      if (objectFoundVect1[i] != objectUniqueVect1[k]) continue;
      svctype = svctypeFoundVect1[i];
      task = taskFoundVect1[i];
      algo = algoFoundVect1[i];
      // Add only if new (unique)
      bool unique = true;
      for(unsigned int j=0; j!=dimInfoNamesVect1.size(); j++){
        if(dimInfoNamesVect1[j]==svcFoundVect1[i]) {
          unique = false;
          break;
        }          
      }
      if (unique)dimInfoNamesVect1.push_back(svcFoundVect1[i]);    
    }
    std::string dimSvcNameAdder = svctype + "/" + serverName + "_Adder_1/" + task + "/";

    if (("MonH1F"==svctype)||("MonH2F"==svctype)||("MonH1D"==svctype)||("MonH2D"==svctype)||("MonP1"==svctype))
      dimSvcNameAdder = dimSvcNameAdder + algo + "/" + objectUniqueVect1[k];
    else
      dimSvcNameAdder = dimSvcNameAdder + objectUniqueVect1[k];

    /************************************************/
    m_dimInfoNamesVect2.push_back(dimInfoNamesVect1);
    m_dimSvcNamesAdderVect1.push_back(dimSvcNameAdder);
    /************************************************/
    mes << MSG::DEBUG << "Added " << dimSvcNameAdder << " to m_dimSvcNamesAdderVect1" << endreq;
  }
  
  //print for test
  mes << MSG::DEBUG << endreq;
  mes << MSG::DEBUG  << "PRINTING DIM NAMES : " << endreq;
  for (int k = 0; k < (int) m_dimInfoNamesVect2.size(); k++) {
    for (int j = 0; j < (int) m_dimInfoNamesVect2[k].size(); j++) {
      mes << MSG::DEBUG << "\tInfo :    " << m_dimInfoNamesVect2[k][j] << endreq;
    }
  }
  for (int k = 0; k < (int) m_dimSvcNamesAdderVect1.size(); k++) {
    mes << MSG::DEBUG << "\tService : " << m_dimSvcNamesAdderVect1[k] << endreq;
  }

  return m_dimSvcNamesAdderVect1.size();
}

// Adder
int DimMonObjectManager::adderCreateDimMonObjects(std::string procName, int refreshTime){
  int contAddersOk = 0;
  MsgStream msgStream(m_msgSvc, m_name);
  //update the DNS only once, when all new DimServices have been made
  DimServer::autoStartOff();

  for (int k = 0; k < (int) m_dimInfoNamesVect2.size(); k++) {
    std::vector<DimInfoMonObject*> tmpDimInfoMonObjectVect1;
    DimInfoMonObject* tmpDimInfoMonObject;
    for (int i = 0; i < (int) m_dimInfoNamesVect2[k].size(); i++) {
      tmpDimInfoMonObject = new DimInfoMonObject(this, m_dimInfoNamesVect2[k][i], refreshTime);
      tmpDimInfoMonObjectVect1.push_back(tmpDimInfoMonObject);
    }
    
    /***********************************************************/
    m_dimInfoMonObjectVect2.push_back(tmpDimInfoMonObjectVect1);
    /***********************************************************/
      
    MonObject *monObjectTest;
    int ntries=0;
    while (1)
    {  
      monObjectTest = m_dimInfoMonObjectVect2[k][0]->monObject();
      if (monObjectTest != 0) break;
      ntries++;
      MsgStream mes(m_msgSvc, m_name);
      mes << MSG::INFO <<"No MonObject found after " << ntries << " attempts." << endreq;
      if (0==m_msgSvc) {
        mes.activate();
        mes.doOutput();
      }
      if(ntries==10) break;
      lib_rtl_sleep(500); 
    }
    
    if (ntries==10) {
      MsgStream mes(m_msgSvc, m_name);
      mes << MSG::ERROR <<"After " << ntries << " attempts we didn't find the Service:"<<m_dimInfoNamesVect2[k][0]<< ". It might to be crashed." << endreq;
      continue;
    }

    contAddersOk ++ ;

    std::string monObjectTypeName = monObjectTest->typeName();

    MonObject *monObjectAdder = createMonObject(monObjectTypeName);
    monObjectAdder->copyFrom(monObjectTest);

    //int monObjectStringSize = m_dimInfoMonObjectVect2[k][0]->monObject()->stringSize();
    int tsize = monObjectSize(monObjectAdder);

    DimServiceMonObject* tmpDimServiceMonObjectAdder=new DimServiceMonObject(this,m_dimSvcNamesAdderVect1[k],monObjectAdder,tsize);

    /***********************************************************/
    m_dimSvcMonObjectAdderVect1.push_back(tmpDimServiceMonObjectAdder);
    /***********************************************************/
  }

  DimServer::start(procName.c_str());
  return contAddersOk;
}

// Adder
void DimMonObjectManager::adderDeleteDimMonObjects(){
  for (int k = 0; k < (int) m_dimInfoMonObjectVect2.size(); k++) {
    for (int i = 0; i < (int) m_dimInfoNamesVect2[k].size(); i++) {
      m_dimInfoMonObjectVect2[k][i]->deleteMonObject();
      delete m_dimInfoMonObjectVect2[k][i];
    }
    m_dimSvcMonObjectAdderVect1[k]->deleteMonObject();
    delete m_dimSvcMonObjectAdderVect1[k];
  }
}

// Adder
void DimMonObjectManager::adderCreateDimTimerMonObject(int refreshTime){
  MsgStream mes(m_msgSvc, m_name);
  mes << MSG::INFO << "Init. completed. Starting DimTimerMonObjectAdder to add " << m_dimInfoMonObjectVect2.size() << " objects" << endreq;
  if (0==m_msgSvc) {
    mes.activate();
    mes.doOutput();
  }
  m_dimTimerMonObjectAdder= new DimTimerMonObjectAdder(this, refreshTime, m_dimInfoMonObjectVect2, m_dimSvcMonObjectAdderVect1);
}

// Adder
void DimMonObjectManager::adderDeleteDimTimerMonObject(){
  if (0==m_dimTimerMonObjectAdder) return;
  delete m_dimTimerMonObjectAdder;
}

// Presenter 
TH1* DimMonObjectManager::presenterInitHistogram(DimInfoMonObject* dimInfoMonObject, int localInstance){
  dimInfoMonObject->createMonObject();
  std::string  monObjectTypeName = dimInfoMonObject->monObject()->typeName();
  TString histoRootName;
  TString histoRootTitle;

  if (monObjectTypeName == s_monH1F){
    MonH1F* monTH1F = (MonH1F*) dimInfoMonObject->monObject();
    histoRootName = TString(Form("%s__instance__%i", monTH1F->histName().c_str(), localInstance));
    histoRootTitle = TString(Form("%s", monTH1F->histTitle().c_str()));
    monTH1F->createObject(histoRootName.Data());
    monTH1F->hist()->SetTitle(histoRootTitle);
    return monTH1F->hist();
  }
  if (monObjectTypeName == s_monH2F){
    MonH2F* monTH2F = (MonH2F*) dimInfoMonObject->monObject();
    histoRootName = TString(Form("%s__instance__%i", monTH2F->histName().c_str(), localInstance));
    histoRootTitle = TString(Form("%s", monTH2F->histTitle().c_str()));
    monTH2F->createObject(histoRootName.Data());
    monTH2F->hist()->SetTitle(histoRootTitle);
    return monTH2F->hist();
  }
  if (monObjectTypeName == s_monH1D){
    MonH1D* monTH1D = (MonH1D*) dimInfoMonObject->monObject();
    histoRootName = TString(Form("%s__instance__%i", monTH1D->histName().c_str(), localInstance));
    histoRootTitle = TString(Form("%s", monTH1D->histTitle().c_str()));
    monTH1D->createObject(histoRootName.Data());
    monTH1D->hist()->SetTitle(histoRootTitle);
    return monTH1D->hist();
  }
  if (monObjectTypeName == s_monH2D){
    MonH2D* monTH2D = (MonH2D*) dimInfoMonObject->monObject();
    histoRootName = TString(Form("%s__instance__%i", monTH2D->histName().c_str(), localInstance));
    histoRootTitle = TString(Form("%s", monTH2D->histTitle().c_str()));
    monTH2D->createObject(histoRootName.Data());
    monTH2D->hist()->SetTitle(histoRootTitle);
    return monTH2D->hist();
  }
  return 0;
}

// Presenter 
void DimMonObjectManager::presenterFillHistogram(DimInfoMonObject* dimInfoMonObject){
       
  dimInfoMonObject->loadMonObject();
  
  std::string  monObjectTypeName = dimInfoMonObject->monObject()->typeName();
  
  if (monObjectTypeName == s_monH1F){
    MonH1F* monTH1F = (MonH1F*) dimInfoMonObject->monObject();
    monTH1F->loadObject();
  }
  if (monObjectTypeName == s_monH2F){
    MonH2F* monTH2F = (MonH2F*) dimInfoMonObject->monObject();
    monTH2F->loadObject();
  }
  if (monObjectTypeName == s_monH1D){
    MonH1D* monTH1D = (MonH1D*) dimInfoMonObject->monObject();
    monTH1D->loadObject();
  }
  if (monObjectTypeName == s_monH2D){
    MonH2D* monTH2D = (MonH2D*) dimInfoMonObject->monObject();
    monTH2D->loadObject();
  }
}

// Presenter 
int DimMonObjectManager::presenterGetDimMonObjectSvcList(std::vector<std::string> &dimMonObjectServicesList)
{
  char* dimDnsServerNode;

  const int nDimServers = m_dimBrowser->getServers();

  MsgStream mes(m_msgSvc, m_name);
  mes << MSG::INFO << "nDimServers:\t" << nDimServers << endreq;
  if (0==m_msgSvc) {
    mes.activate();
    mes.doOutput();
  }

  if (nDimServers <= 0) return nDimServers;
  
  TString dimServerName;
  TString dimServerNodeName;
  TString dimServiceName;
  TString statusMessage;

  // First look for a DNS
  dimDnsServerNode = DimClient::getDnsNode();

  mes << MSG::INFO << "DNS: " << dimDnsServerNode << endreq;
  if (0==m_msgSvc) {
    mes.activate();
    mes.doOutput();
  }

  char* dimServer;
  char* dimServerNode;
  char* dimService;
  char* dimFormat;
  int   dimType;

  while (m_dimBrowser->getNextServer(dimServer, dimServerNode)) {
    m_dimBrowser->getServerServices(dimServer);
    dimServerName =  TString(dimServer);
    dimServerNodeName =  TString(dimServerNode);

    mes << MSG::INFO << "\t|_ " << dimServerNodeName << "\t-\t" << dimServerName << endreq;
    if (0==m_msgSvc) {
      mes.activate();
      mes.doOutput();
    }

    if (!dimServerName.BeginsWith("DIS_DNS")) {
      while(dimType = m_dimBrowser->getNextServerService(dimService, dimFormat)) {
        if (TString(dimService).BeginsWith("Mon")) {
          mes << MSG::DEBUG << "presenterGetDimMonObjectSvcList: Adding dimService " << dimService 
              << " from server " << dimServerName << " to list" << endreq;
          dimMonObjectServicesList.push_back(std::string(dimService));
        }
      }
    }
  }
  return nDimServers;
}

//MonObjectTextPrinter 
MonObject *DimMonObjectManager::monObjectTextPrinterGetMonObject(int refreshTime, std::string svcName) {
  MsgStream mes(m_msgSvc, m_name);
  if (0==m_msgSvc) {
     mes.activate();
     mes.doOutput();
  }
  DimInfoMonObject* tmpDimInfoMonObject = new DimInfoMonObject(this, svcName, refreshTime);
  MonObject *tmpMonObject=0;
  int ntries=0;
  while (1)
  {
    if (tmpDimInfoMonObject->monObject() != 0){
      tmpMonObject = createMonObject(tmpDimInfoMonObject->monObject()->typeName());
      tmpMonObject->copyFrom(tmpDimInfoMonObject->monObject());
      tmpMonObject->print();
      break;
    }
    ntries++;

    mes << MSG::WARNING <<"No MonObject found after " << ntries << " attempts." << endreq;
    if (0==m_msgSvc) {
      mes.activate();
      mes.doOutput();
    }
    if(ntries==10) break;
    lib_rtl_sleep(500); 
  }
  mes << MSG::DEBUG <<"Deleting MonObjects." << endreq; 
  tmpDimInfoMonObject->deleteMonObject();
  mes << MSG::DEBUG <<"Deleting DimInfo." << endreq; 
  delete tmpDimInfoMonObject;
  return tmpMonObject;
}
