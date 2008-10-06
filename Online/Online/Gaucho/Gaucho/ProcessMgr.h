#ifndef SaverSvc_ProcessMgr_H
#define SaverSvc_ProcessMgr_H 1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include <string>
#include <set>
#include <map>
#include <vector>

static const int s_saveHistos(123);
static const int s_updateSvcMapFromInfoService(124);
static const int s_updateSvcMapFromInfoServer(125);
static const int s_startTimer(126);
static const int s_stopTimer(127);
static const int s_createInfoServices(128);

static const std::string s_Adder("Adder");
static const std::string s_Saver("Saver");
static const std::string s_MonRateService("MonRateService");


//  Author: jotalo, 19/06/2008

class DimInfoServers;
class DimInfoServices;
class BaseServiceMap;
class Interactor;
class DimTimerProcess;
class ProcessMgr {
public:
  ProcessMgr(std::string serviceOwner, IMessageSvc* msgSvc, Interactor *service, const int &m_refreshTime);
  virtual ~ProcessMgr();
  DimInfoServers*  dimInfoServers() {return m_dimInfoServers;}
  DimInfoServices* dimInfoServices(){return m_dimInfoServices;}
  BaseServiceMap*      serviceMap()     {return m_serviceMap;}
  Interactor* service() {return m_service;} 
  DimTimerProcess* dimTimerProcess(){return m_dimTimerProcess;}
  std::string utgid() {return m_utgid;}

  int refreshTime(){return m_refreshTime;}
  
  bool isMonitoringFarm() {return m_monitoringFarm;}
  
  std::string name() {return m_name;};
  IMessageSvc* msgSvc() {return m_msgSvc;};  
      
  void updateServiceSet(std::string &dimString, std::set<std::string> &serviceSet);
  void updateServerMap(std::string &dimString, std::map<std::string, bool, std::less<std::string> > &serverMap);
  
  void createInfoServers();
  void createInfoServices(std::string serverName);
  void createTimerProcess();
  void updateMap(); //this method update data when we dont have the ServerMap 

  void setPartVector(const std::vector<std::string> &partName) {m_partName = partName; m_monitoringFarm = true;}
  void setTaskName(const std::string &taskName) {m_taskName = taskName;}
  void setSubFarmVector(const std::vector<std::string> &subfarmName) {m_subfarmName = subfarmName;}
  void setAlgorithmVector(const std::vector<std::string> &algorithmName) {m_algorithmName = algorithmName;}
  void setObjectVector(const std::vector<std::string> &objectName) {m_objectName = objectName;}
  void setUtgid(const std::string &utgid);
  void setNodeName(const std::string &nodeName){m_nodeName=nodeName;}

  void setSaveDir(const std::string &saveDir){m_saveDir = saveDir;}
  void setSaveDiff(const int &saveDiff){m_saveDiff = saveDiff;}
  bool saveDiff () {if (0 == m_saveDiff) return false; else return true;}
  void timerHandler();
  void write();
  std::string taskName() {return m_taskName;}
  
  void setMonitorSvc(IGauchoMonitorSvc* pGauchoMonitorSvc){m_pGauchoMonitorSvc = pGauchoMonitorSvc;}
  //void setFileStaus(std::string& file) {m_pFile = &file;}
  //void fileName() {}
  std::string* fileNamePointer(){return &m_fileName;}
  bool isAdder () {if (s_Adder == m_serviceOwner) return true;else return false;}
  
private:
  std::set<std::string> decodeServerList(const std::string &serverListS);

protected:
  std::string m_serviceOwner;
  std::string m_name;
  IMessageSvc* m_msgSvc;
  Interactor* m_service;
  int m_refreshTime;
    
  std::string m_utgid;
  std::string m_nodeName;
  
  //std::string *m_pFile;
  std::string m_fileName;
  
  IGauchoMonitorSvc* m_pGauchoMonitorSvc; ///< Online Gaucho Monitoring Service
  
  bool m_monitoringFarm;
  std::vector<std::string> m_partName;
  std::string m_taskName;
  std::vector<std::string> m_subfarmName;
  std::vector<std::string> m_algorithmName;
  std::vector<std::string> m_objectName;
  std::string m_saveDir;
  int m_saveDiff;
  
  DimInfoServers*  m_dimInfoServers;
  DimInfoServices* m_dimInfoServices;
  BaseServiceMap*  m_serviceMap;
  DimTimerProcess* m_dimTimerProcess;
  
  // this variable is used only in the case of savers to exclude multiple servers with the same taskname
  std::string m_serverNameChoosen;
  
};

#endif // SaverSvc_ProcessMgr_H
