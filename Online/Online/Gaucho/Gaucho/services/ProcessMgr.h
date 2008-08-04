#ifndef SaverSvc_ProcessMgr_H
#define SaverSvc_ProcessMgr_H 1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include <string>
#include <set>
#include <map>
#include <vector>

static const int s_saveHistos(123);
static const int s_updateSvcMapFromInfoService(124);
static const int s_updateSvcMapFromInfoServer(125);
static const int s_startTimer(126);
static const int s_createInfoServices(127);

//  Author: jotalo, 19/06/2008

class DimInfoServers;
class DimInfoServices;
class BaseServiceMap;
class Interactor;
class DimTimerProcess;
class ProcessMgr {
public:
  ProcessMgr(std::string name, IMessageSvc* msgSvc, Interactor *service); 
  virtual ~ProcessMgr();
  DimInfoServers*  dimInfoServers() {return m_dimInfoServers;}
  DimInfoServices* dimInfoServices(){return m_dimInfoServices;}
  BaseServiceMap*      serviceMap()     {return m_serviceMap;}
  Interactor* service() {return m_service;} 
  DimTimerProcess* dimTimerProcess(){return m_dimTimerProcess;}

  std::string utgid() {return m_utgid;}
  
  bool isAdder() {return m_isAdder;}
  
  bool withPartitionName() {return m_withPartitionName;}
  
  std::string name() {return m_name;};
  IMessageSvc* msgSvc() {return m_msgSvc;};  
      
  void updateServiceSet(std::string &dimString, std::set<std::string> &serviceSet);
  void updateServerMap(std::string &dimString, std::map<std::string, bool, std::less<std::string> > &serverMap);
  
  void createInfoServers();
  void createInfoServices(std::string serverName);
  void createTimerProcess(int refreshTime);
  void updateMap(); //this method update data when we dont have the ServerMap 

  void setPartVector(const std::vector<std::string> &partName) {m_partName = partName; m_withPartitionName = true;}
  void setTaskVector(const std::vector<std::string> &taskName) {m_taskName = taskName;}
  void setAlgorithmVector(const std::vector<std::string> &algorithmName) {m_algorithmName = algorithmName;}
  void setObjectVector(const std::vector<std::string> &objectName) {m_objectName = objectName;}
  void setUtgid(const std::string &utgid);
  void setSaveDir(const std::string &saveDir){m_saveDir = saveDir;}
  void timerHandler();

private:
  std::set<std::string> decodeServerList(const std::string &serverListS);

protected:
  std::string m_name;
  IMessageSvc* m_msgSvc;
  Interactor* m_service;
  bool m_isAdder;
  
  std::string m_utgid;
  std::string m_nodeName;
  
  bool m_withPartitionName;
  std::vector<std::string> m_partName;
  std::vector<std::string> m_taskName;
  std::vector<std::string> m_algorithmName;
  std::vector<std::string> m_objectName;
  std::string m_saveDir;
  
  DimInfoServers*  m_dimInfoServers;
  DimInfoServices* m_dimInfoServices;
  BaseServiceMap*  m_serviceMap;
  DimTimerProcess* m_dimTimerProcess;
};

#endif // SaverSvc_ProcessMgr_H
