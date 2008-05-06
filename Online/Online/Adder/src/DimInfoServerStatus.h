#ifndef DimInfoServerStatus_H
#define DimInfoServerStatus_H 1

#include "dic.hxx"
#include <string>
#include <set>
#include <map>
#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

//  Author: jotalo, 14/04/2008

class DimInfoServiceSet; 
class DimInfoServerStatus: public DimInfo
{
private:
  
  std::map<std::string, bool, std::less<std::string> >::iterator m_statusIt;
  
  std::map<std::string, bool, std::less<std::string> > *m_serverStatus;
  std::set<std::string> *m_serviceSet;
  bool *m_updateTimerAdder;
  std::string *m_choosedServerName;
  std::vector<std::string> m_taskName;
  std::vector<std::string> m_algorithmName;
  std::vector<std::string> m_objectName;
  IMessageSvc* m_msgSvc;
  
  DimInfoServiceSet* m_dimInfoServiceSet;

  std::set<std::string> m_nodeNameSet;
  std::set<std::string> m_taskNameSet;

  
  std::string m_name;
  
  
  IMessageSvc* msgSvc() {return m_msgSvc;};
  std::string name() {return m_name;};

public:
  DimInfoServerStatus(std::map<std::string, bool, std::less<std::string> > *serverStatus, 
                      std::set<std::string> *serviceSet,
                      bool *updateTimerAdder,
                      std::string *choosedServerName,
                      const std::vector<std::string> &nodeName, 
                      const std::vector<std::string> &taskName,
                      const std::vector<std::string> &algorithmName,
                      const std::vector<std::string> &objectName,
                      IMessageSvc* msgSvc);

  ~DimInfoServerStatus();
  
  void printServerStatus();
  void printServiceSet();
  
  
  void infoHandler();
  void chooseServer();
  void updateServiceSet();
  std::set<std::string> decodeServerList(const std::string &serverListS);
};
#endif    // DimInfoServerStatus_H
