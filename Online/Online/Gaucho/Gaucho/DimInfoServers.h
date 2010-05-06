#ifndef DimInfoServers_H
#define DimInfoServers_H 1

#include "dic.hxx"
#include <string>
#include <set>
#include <map>
#include <vector>

//  Author: jotalo, 19/06/2008

class ProcessMgr;
class DimInfoServers: public DimInfo
{
private:
  std::string m_name;

  std::string m_serverChoosed;

  ProcessMgr *m_processMgr;
  std::string name(){return m_name;}

  std::map<std::string, bool, std::less<std::string> > m_serverMap;
  std::map<std::string, bool, std::less<std::string> >::iterator m_it;
  int m_maxNbOfServers;

public:
  DimInfoServers(ProcessMgr *processMgr, int maxNbOfServers=999);

  ~DimInfoServers();

  void infoHandler();

  std::string serverChoosed() {return m_serverChoosed;}
  std::map<std::string, bool, std::less<std::string> > serverMap(){return m_serverMap;}
  void chooseServer();
  bool isActive(std::string serverName);

  void print();

};
#endif    // DimInfoServers_H
