#include "DimMonObjectManager.h"
#include "DimTimerMonObjectAdder.h"
#include "DimServiceMonObject.h"
#include "DimInfoMonObject.h"
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
#include "Gaucho/MonPair.h"
#include "Gaucho/MonProfile.h"
#include "Gaucho/MonHitMap2D.h"

DimMonObjectManager::DimMonObjectManager(IMessageSvc* msgSvc, const std::string& source)
{
  m_msgSvc=msgSvc;
  m_name="DimMOManager";
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
void DimMonObjectManager::setDimBrowser(DimBrowser* dimBrowser){
  m_dimBrowser = dimBrowser;
}

void DimMonObjectManager::dimWait(){
  dim_wait();
}

void DimMonObjectManager::msg(std::string msg, std::string name, int msgLevel){
    if (0==m_msgSvc) {
      std::cout << msg << std::endl;
    }
    else{
      MsgStream msgStream(m_msgSvc, name);
      msgStream << msgLevel << msg << endreq;
    }
}

MonObject* DimMonObjectManager::createMonObject(std::string  monObjectTypeName){
  MonObject* monObject=0;
  if (monObjectTypeName == "MonObject") monObject = new MonObject(m_msgSvc, m_source);
  if (monObjectTypeName == "MonH1F") monObject = new MonH1F(m_msgSvc, m_source);
  if (monObjectTypeName == "MonH1D") monObject = new MonH1D(m_msgSvc, m_source);
  if (monObjectTypeName == "MonH2F") monObject = new MonH2F(m_msgSvc, m_source);
  if (monObjectTypeName == "MonH2D") monObject = new MonH2D(m_msgSvc, m_source);
  if (monObjectTypeName == "MonProfile") monObject = new MonProfile(m_msgSvc, m_source);
  if (monObjectTypeName == "MonInt") monObject = new MonInt(m_msgSvc, m_source);
  if (monObjectTypeName == "MonDouble") monObject = new MonDouble(m_msgSvc, m_source);
  if (monObjectTypeName == "MonFloat") monObject = new MonFloat(m_msgSvc, m_source);
  if (monObjectTypeName == "MonString") monObject = new MonString(m_msgSvc, m_source);
  if (monObjectTypeName == "MonLong") monObject = new MonLong(m_msgSvc, m_source);
  if (monObjectTypeName == "MonBool") monObject = new MonBool(m_msgSvc, m_source);
  if (monObjectTypeName == "MonPair") monObject = new MonPair(m_msgSvc, m_source);
  return monObject;
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

    msg("New DimMonObject: " + dimSvcName, m_name, MSG::INFO);

  }
  else {
    msg("Already exist DimMonObject: " + dimSvcName, m_name, MSG::ERROR);
  }
}

//MonitorSvc
void DimMonObjectManager::undeclServiceMonObject(std::string infoName){

  for (m_dimMonObjectsIt = m_dimMonObjects.begin(); m_dimMonObjectsIt != m_dimMonObjects.end(); m_dimMonObjectsIt++)
    msg((*m_dimMonObjectsIt).first, m_name, MSG::DEBUG);

  m_dimMonObjectsIt = m_dimMonObjects.find(infoName);  

  if(m_dimMonObjectsIt != m_dimMonObjects.end()) {
    delete (*m_dimMonObjectsIt).second;
    m_dimMonObjects.erase(m_dimMonObjectsIt);
    msg("undeclSvc: Service " + infoName + " undeclared", m_name, MSG::INFO);
    return;
  }

  msg("undeclSvc: No DimServiceMonObject found with the name:" + infoName, m_name, MSG::ERROR);

}

//MonitorSvc
void DimMonObjectManager::updateServiceMonObject(std::string infoName){
  for (m_dimMonObjectsIt = m_dimMonObjects.begin(); m_dimMonObjectsIt != m_dimMonObjects.end(); m_dimMonObjectsIt++)
    msg((*m_dimMonObjectsIt).first, m_name, MSG::DEBUG);
  m_dimMonObjectsIt = m_dimMonObjects.find(infoName);  
  if(m_dimMonObjectsIt != m_dimMonObjects.end()) {
    (*m_dimMonObjectsIt).second->updateService();
    msg("updateSvc: Service " + infoName + " updated", m_name, MSG::INFO);    
    return;
  }
  msg("updateSvc: No DimServiceMonObject found with the name:" + infoName, m_name, MSG::WARNING);
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

    msg("svcName=" + svcName, m_name, MSG::INFO);

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
        msg("Service found: " + serviceStr, m_name, MSG::INFO);
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
/*  std::cout << "PRINTING DIM NAMES : " << std::endl;
  for (int k = 0; k < (int) m_dimInfoNamesVect2.size(); k++) {
    for (int j = 0; j < (int) m_dimInfoNamesVect2[k].size(); j++) {
      std::cout << "Info : " << m_dimInfoNamesVect2[k][j] << std::endl;
    }
  }*/
}

//Saver
void DimMonObjectManager::saverCreateMonObjects(int refreshTime) {
  for (int k = 0; k < (int) m_dimInfoNamesVect2.size(); k++) {
    std::vector<MonObject*> tmpMonObjectVect1;
    for (int i = 0; i < (int) m_dimInfoNamesVect2[k].size(); i++) {
      
      DimInfoMonObject* tmpDimInfoMonObject = new DimInfoMonObject(this, m_dimInfoNamesVect2[k][i], refreshTime);
      
      MonObject *tmpMonObject;
      int ntries=0;
      while (1)
      {
        tmpMonObject = tmpDimInfoMonObject->monObject();
        if (tmpMonObject != 0) break;
        ntries++;

        MsgStream mes(m_msgSvc, m_name);
        mes << MSG::ERROR <<"No MonObject found after " << ntries << " attempts." << endreq;
        if (0==m_msgSvc) {
          mes.activate();
          mes.doOutput();
        }

        if(ntries==10) break;
        lib_rtl_sleep(500); 
      }
      tmpMonObjectVect1.push_back(tmpMonObject);
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

// Adder
int DimMonObjectManager::serviceParts(std::string serviceStr, std::string &svctype, std::string &task, std::string &utgid, std::string &algo, std::string &object){

  std::string::size_type first = serviceStr.find("/");
  std::string::size_type second = serviceStr.find("/",first+1);
  std::string::size_type third = serviceStr.find("/",second+1);
  std::string::size_type first_us = serviceStr.find("_",first+1);
  std::string::size_type second_us = serviceStr.find("_",first_us+1);
  
  if (first == std::string::npos) return 0;
  if ((second <= first)||(third <= second )||(first_us == std::string::npos)||(second_us == std::string::npos)) return -1;
  
  svctype  = serviceStr.substr(0, first_us - 1);
  task  = serviceStr.substr(first_us + 1, second_us - first_us - 1);
  utgid  = serviceStr.substr(first_us, second_us - first_us);
  algo  = serviceStr.substr(second + 1, third - second - 1);
  object = serviceStr.substr(third + 1);
  return 1;
}

// Adder
void DimMonObjectManager::adderCreateDimNames(std::string nodeName, std::vector<std::string> taskName, std::vector<std::string> algorithmName, std::vector<std::string> objectName, std::string serverName){
  
  MsgStream msgStream(m_msgSvc, m_name);
  std::string svcName = "*/" + nodeName + "*";
  m_dimBrowser->getServices(svcName.c_str());
  
  char *service;
  char *format;
  int type;
  
  std::vector<std::string> svcFoundVect1;
  std::vector<std::string> objectUniqueVect1; 
  std::vector<std::string> objectFoundVect1; 
  std::vector<std::string> algoFoundVect1; 
  std::vector<std::string> taskFoundVect1; 
  std::vector<std::string> svctypeFoundVect1; 
  
  while (type = m_dimBrowser->getNextService(service, format))
  {
    std::string serviceStr = service; //Ex--> MON/node00101_Test_1/GaudiExample/ExMonH1D
    
    if (serviceStr.find(nodeName+"_Adder_",0) != std::string::npos ) continue;
   
    std::string svctype;
    std::string task;
    std::string utgid;
    std::string algo;
    std::string object;
    
    int matchRes = DimMonObjectManager::serviceParts(serviceStr, svctype, task, utgid, algo, object);
    
    if (matchRes <= 0){ 
      if (matchRes < 0)
        msg("Service: " + serviceStr + " doesn't match whit MON/node_task_x/algoritm/histo and. Ignoring.", m_name, MSG::ERROR);
      continue;
    }
    
    for (int i = 0;i < (int) objectName.size(); i++) {//Ex --> ExMonH1D* or ExMonH1D
    
      if (task != taskName[i]) continue;
      if (algo != algorithmName[i]) continue;
    
      std::string::size_type foundStar = objectName[i].find("*", 0);
    
      if (foundStar == std::string::npos ) { // No wildcard, then histo must be equal to m_histogramname[i] ? ask Eric
        if (objectName[i] != object) continue;
      }
      else { // Wildcard, it works only when * is at the end of objectName[i]
        std::string::size_type objectfound = object.find(objectName[i].substr(0, foundStar));
        if (objectfound == std::string::npos) continue;
      }
      
      objectFoundVect1.push_back(object);
      algoFoundVect1.push_back(algo);
      taskFoundVect1.push_back(task); 
      svctypeFoundVect1.push_back(svctype); 
      svcFoundVect1.push_back(serviceStr);
    
      // After that, there are breaks. ask Eric if one service belong to only one histo. If not, break must be changed to continue
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
      dimInfoNamesVect1.push_back(svcFoundVect1[i]);
    }
    std::string dimSvcNameAdder = svctype + "/" + serverName + "_Adder_1/" + task + "/" + algo + "/" + objectUniqueVect1[k];
    
    /************************************************/
    m_dimInfoNamesVect2.push_back(dimInfoNamesVect1);
    m_dimSvcNamesAdderVect1.push_back(dimSvcNameAdder);
    /************************************************/
  }
  
  // print for test
  // std::cout << "PRINTING DIM NAMES : " << std::endl;
  // for (int k = 0; k < (int) m_dimInfoNamesVect2.size(); k++) {
  //   for (int j = 0; j < (int) m_dimInfoNamesVect2[k].size(); j++) {
  //     std::cout << "Info : " << m_dimInfoNamesVect2[k][j] << std::endl;
  //   }
  // }
  // for (int k = 0; k < (int) m_dimSvcNamesAdderVect1.size(); k++) {
  //   std::cout << "Service : " << m_dimSvcNamesAdderVect1[k] << std::endl;
  // }
}

// Adder
void DimMonObjectManager::adderCreateDimMonObjects(std::string procName, int refreshTime){
  
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

// Presenter 
TH1* DimMonObjectManager::presenterInitHistogram(DimInfoMonObject* dimInfoMonObject, int localInstance){
  dimInfoMonObject->createMonObject();
  std::string  monObjectTypeName = dimInfoMonObject->monObject()->typeName();
  TString histoRootName;
  TString histoRootTitle;

  if (monObjectTypeName == "MonH1F"){
    MonH1F* monTH1F = (MonH1F*) dimInfoMonObject->monObject();
    histoRootName = TString(Form("%s__instance__%i", monTH1F->histName().c_str(), localInstance));
    histoRootTitle = TString(Form("%s", monTH1F->histTitle().c_str()));
    monTH1F->createObject(histoRootName.Data());
    monTH1F->hist()->SetTitle(histoRootTitle);
    return monTH1F->hist();
  }
  if (monObjectTypeName == "MonH2F"){
    MonH2F* monTH2F = (MonH2F*) dimInfoMonObject->monObject();
    histoRootName = TString(Form("%s__instance__%i", monTH2F->histName().c_str(), localInstance));
    histoRootTitle = TString(Form("%s", monTH2F->histTitle().c_str()));
    monTH2F->createObject(histoRootName.Data());
    monTH2F->hist()->SetTitle(histoRootTitle);
    return monTH2F->hist();
  }
  if (monObjectTypeName == "MonH1D"){
    MonH1D* monTH1D = (MonH1D*) dimInfoMonObject->monObject();
    histoRootName = TString(Form("%s__instance__%i", monTH1D->histName().c_str(), localInstance));
    histoRootTitle = TString(Form("%s", monTH1D->histTitle().c_str()));
    monTH1D->createObject(histoRootName.Data());
    monTH1D->hist()->SetTitle(histoRootTitle);
    return monTH1D->hist();
  }
  if (monObjectTypeName == "MonH2D"){
    MonH2D* monTH2D = (MonH2D*) dimInfoMonObject->monObject();
    histoRootName = TString(Form("%s__instance__%i", monTH2D->histName().c_str(), localInstance));
    histoRootTitle = TString(Form("%s", monTH2D->histTitle().c_str()));
    monTH2D->createObject(histoRootName.Data());
    monTH2D->hist()->SetTitle(histoRootTitle);
    return monTH2D->hist();
  }
  return 0;
}

void DimMonObjectManager::presenterFillHistogram(DimInfoMonObject* dimInfoMonObject){
       
  dimInfoMonObject->loadMonObject();
  
  std::string  monObjectTypeName = dimInfoMonObject->monObject()->typeName();
  
  if (monObjectTypeName == "MonH1F"){
    MonH1F* monTH1F = (MonH1F*) dimInfoMonObject->monObject();
    monTH1F->loadObject();
  }
  if (monObjectTypeName == "MonH2F"){
    MonH2F* monTH2F = (MonH2F*) dimInfoMonObject->monObject();
    monTH2F->loadObject();
  }
  if (monObjectTypeName == "MonH1D"){
    MonH1D* monTH1D = (MonH1D*) dimInfoMonObject->monObject();
    monTH1D->loadObject();
  }
  if (monObjectTypeName == "MonH2D"){
    MonH2D* monTH2D = (MonH2D*) dimInfoMonObject->monObject();
    monTH2D->loadObject();
  }
}

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
          dimMonObjectServicesList.push_back(std::string(dimService));
        }
      }
    }
        }
  return nDimServers;
}

