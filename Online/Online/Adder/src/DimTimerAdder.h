#ifndef DIMTIMERADDER_H
#define DIMTIMERADDER_H 1

#include <dis.hxx>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <set>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"


//  Author: jotalo, 14/04/2008


class DimInfoMonObject;
class DimServiceMonObject;
class MonObject;

class DimTimerAdder: public DimTimer {
private: 
  std::string  m_name;
  const std::map<std::string, bool, std::less<std::string> > *m_serverStatus;
  const std::set<std::string> *m_serviceSet;
  bool *m_updateTimerAdder;
  const std::string *m_choosedServerName;
  const std::string m_serverNameAdder;
  const int m_refreshTime;
  IMessageSvc* m_msgSvc;

  std::map<std::string, bool, std::less<std::string> > m_serverStatusLocal;
  std::set<std::string> m_serviceSetLocal;
  std::string m_choosedServerNameLocal;
  
  std::map<std::string, DimServiceMonObject*> m_dimSrv;
  std::map<std::string, DimServiceMonObject*>::iterator m_dimSrvIt;
  
  std::map<std::string, MonObject*> m_monObject;
  std::map<std::string, MonObject*>::iterator m_monObjectIt;
  
  std::map<std::string, std::map<std::string, DimInfoMonObject*> > m_dimInfo;
  std::map<std::string, std::map<std::string, DimInfoMonObject*> >::iterator m_dimInfoIt;
  
  std::map<std::string, DimInfoMonObject*>::iterator m_dimInfo2It;

//  bool m_handlerEnabled; 
  
  bool adderActive(const std::string &adderName);
  bool termActive(const std::string &termName);
  
  std::string createAdderName (const std::string &serviceName);
  std::string createTermServiceName (const std::string &serviceName, const std::string &serverName);
  void includeServiceInMaps(const std::string &serviceName);
  void includeServerInMaps(const std::string &serverName);
  void excludeServiceInMaps(const std::string &serviceName);
  void excludeServerInMaps(const std::string &serviceName, const std::string &serverName);
  void excludeServerInMaps(const std::string &serverName);
  IMessageSvc* msgSvc() {return m_msgSvc;};
  std::string name() {return m_name;};
public: 
  DimTimerAdder (const std::map<std::string, bool, std::less<std::string> > *serverStatus, 
                 const std::set<std::string> *serviceSet,
                 bool *updateTimerAdder,
                 const std::string *choosedServerName,
                 const std::string &serverNameAdder,
                 const int &refreshTime,
                 IMessageSvc* msgSvc);
  
  ~DimTimerAdder();
  
  void timerHandler();
  void printDimInfoMap();
  void printServerStatus(std::map<std::string, bool, std::less<std::string> > serverStatus);
  void printServiceSet(const std::set<std::string> serviceSet);
  void updateDimInfoServiceMap();
  bool statusDiscrepancy();
/*  void disableHandler(){m_handlerEnabled = false;}
  void enableHandler(){m_handlerEnabled = true;}*/
};   

#endif    // DIMTIMERADDER_H

