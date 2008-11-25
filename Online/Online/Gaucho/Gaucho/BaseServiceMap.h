#ifndef SaverSvc_BaseServiceMap_H
#define SaverSvc_BaseServiceMap_H 1


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include <string>
#include <set>
#include <map>
#include <vector>

//  Author: jotalo, 19/06/2008

class ProcessMgr;
class MonObject;
class DimInfoMonObject;
class DimServiceMonObject;
class DimInfoMonRate;
class RatePublisher;
class MonRateDecoder;
class BaseServiceMap {
public:
  BaseServiceMap(ProcessMgr *processMgr); 
  virtual ~BaseServiceMap();
  void removeMap();


  IMessageSvc* msgSvc();
  std::string name() { return m_name;}
  
  //void createMap(std::map<std::string, bool, std::less<std::string> > serverMap, std::set<std::string> serviceSet);
  void updateMap(std::map<std::string, bool, std::less<std::string> > serverMap);
  
  void includeServerInMaps(const std::string &serverName);
  void excludeServerInMaps(const std::string &serverName);
  
  void insertDimInfo(const std::string &serviceName, const std::string &serverName);
  void loadDimInfo(const std::string &serviceName, const std::string &serverName);
  int deleteDimInfo(const std::string &serviceName, const std::string &serverName);
  
  void insertDimService(const std::string &serviceName, const std::string &serverName);
  void deleteDimService(const std::string &serviceName);
  
  std::string createTermServiceName (const std::string &serviceName, const std::string &serverName);
  std::string createAdderName (const std::string &serviceName);
  std::string createSaverName (const std::string &serviceName);

  void printMap();
  void printServiceSet();
  
  void write(std::string saveDir, std::string &fileName);
  void add();
  void updateNumberOfMonRates();
  
  void setServiceSet(const std::set<std::string>& serviceSet){m_serviceSet = serviceSet;}
  void setNbCounterInMonRate(int nbCounterInMonRate){m_nbCounterInMonRate =
  nbCounterInMonRate;}
  int dimInfoMonRateSize(){return m_dimInfoMR.size();}
  
private:
  std::string m_name;
  ProcessMgr *m_processMgr;

  std::map<std::string, bool, std::less<std::string> > m_serverMap;
  std::set<std::string> m_serviceSet;
  
protected:
  
  std::map<std::string, std::pair<DimServiceMonObject*, MonObject*>, std::less<std::string> > m_dimSrv;
  std::map<std::string, std::pair<DimServiceMonObject*, MonObject*>, std::less<std::string> >::iterator m_dimSrvIt;
  
  std::map<std::string, std::pair<bool, std::string>, std::less<std::string> > m_dimSrvStatus;
  std::map<std::string, std::pair<bool, std::string>, std::less<std::string> >::iterator m_dimSrvStatusIt;

  std::map<std::string, std::map<std::string, DimInfoMonObject*>, std::less<std::string> > m_dimInfo;
  std::map<std::string, std::map<std::string, DimInfoMonObject*>, std::less<std::string> >::iterator m_dimInfoIt;
  
  // Objects for MonRateService 
  std::map<std::string, DimInfoMonRate*, std::less<std::string> > m_dimInfoMR;
  RatePublisher * m_numberOfMonRatesPublisher;
  int m_nbCounterInMonRate;
  MonRateDecoder *m_monRateDecoder;

};

#endif    // SaverSvc_BaseServiceMap_H
