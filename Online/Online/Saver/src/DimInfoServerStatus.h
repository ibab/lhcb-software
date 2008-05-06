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
  std::string m_nodeName;
  std::vector<std::string> m_taskName;
  IMessageSvc* m_msgSvc;
  
  DimInfoServiceSet* m_dimInfoServiceSet;

  std::set<std::string> m_taskNameSet;
  std::set<std::string> m_nodeNameSet;

  std::string m_name;
  
  IMessageSvc* msgSvc() {return m_msgSvc;};
  std::string name() {return m_name;};

public:
  DimInfoServerStatus(std::map<std::string, bool, std::less<std::string> > *serverStatus, 
                      const std::string &nodeName, 
                      const std::vector<std::string> &taskName,
                      IMessageSvc* msgSvc);

  ~DimInfoServerStatus();
  
  void printServerStatus();
  
  void infoHandler();
  std::set<std::string> decodeServerList(const std::string &serverListS);
  bool isServerActive (const std::string &serverName);
  
};
#endif    // DimInfoServerStatus_H
