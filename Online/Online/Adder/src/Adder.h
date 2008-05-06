#ifndef Adder_Adder_H
#define Adder_Adder_H 1

#include <set>
#include <map>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"
#include "Misc.h"

//  Author: jotalo, 14/04/2008

class DimInfoServerStatus;
class DimTimerAdder;

class Adder : public Algorithm {
public:
  Adder(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  std::vector<std::string> m_nodeName;
  std::vector<std::string> m_objectName;
  std::vector<std::string> m_algorithmName;
  std::vector<std::string> m_taskName;
  int m_refreshTime;
  std::string m_dimClientDns;
  std::string m_serverName;

  bool *m_updateTimerAdder;
  std::string *m_choosedServerName;
  
  std::map<std::string, bool, std::less<std::string> > *m_serverStatus;
  std::map<std::string, bool, std::less<std::string> >::iterator  m_serverIt;
  
  std::set<std::string> *m_serviceSet;
  std::set<std::string>::iterator m_serviceIt;
  
  DimInfoServerStatus *m_dimInfoServerStatus;
  DimTimerAdder* m_dimTimerAdder;

  bool m_firstExecute;
  
};

#endif    // Adder_Adder_H

