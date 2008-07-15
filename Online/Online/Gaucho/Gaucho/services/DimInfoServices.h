#ifndef DimInfoServices_H
#define DimInfoServices_H 1

#include "dic.hxx"
#include <string>
#include <set>
#include <map>
#include <vector>

//  Author: jotalo, 19/06/2008

class ProcessMgr;
class DimInfoServices: public DimInfo
{
private:
  std::string m_name;
  ProcessMgr *m_processMgr;
  std::string name(){return m_name;}
  
  std::set<std::string> m_serviceSet;
  
  bool alreadyDone;
  
public:
  DimInfoServices(ProcessMgr *processMgr, std::string serverName);
  
  ~DimInfoServices();
  
  void infoHandler();
  
  std::set<std::string> serviceSet() { return m_serviceSet;}
  void print();
  
};
#endif    // DimInfoServices_H
