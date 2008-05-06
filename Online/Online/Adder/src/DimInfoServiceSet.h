#ifndef DimInfoServiceSet_H
#define DimInfoServiceSet_H 1

#include <iostream>
#include <string>
#include <set>
#include <map>
#include <vector>

#include "dic.hxx"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

//  Author: jotalo, 14/04/2008

class DimInfoServiceSet: public DimInfo
{
private:
  
  
  std::set<std::string> *m_serviceSet;
  bool *m_updateTimerAdder;
  std::string m_serverName;
  std::vector<std::string> m_taskName;
  std::vector<std::string> m_algorithmName;
  std::vector<std::string> m_objectName;
  IMessageSvc* m_msgSvc;

  std::string m_nodeName;
  std::string  m_name;
public:
  DimInfoServiceSet(  std::set<std::string> *serviceSet,
                      bool *updateTimerAdder,
                      std::string serverName,
                      std::vector<std::string> taskName,
                      std::vector<std::string> algorithmName,
                      std::vector<std::string> objectName,
                      IMessageSvc* msgSvc);

  ~DimInfoServiceSet();
  std::string serverName() {return m_serverName;}
  void infoHandler();
  IMessageSvc* msgSvc() {return m_msgSvc;};
  std::string name() {return m_name;};

};
#endif    // DimInfoServiceSet_H


