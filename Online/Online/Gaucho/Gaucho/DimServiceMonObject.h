#ifndef DIMSERVICEMONOBJECT_H
#define DIMSERVICEMONOBJECT_H 1

#include <string>
#include <iostream>
#include "dis.hxx"
#include <TStopwatch.h>
#include <sstream>
#include "Gaucho/MonObject.h"

//  Author: jotalo, 22/10/2007

class DimServiceMonObject:public DimService {

private:
  MonObject*    m_monObject;
  std::string   m_name;
  std::string m_archiveType;
  IMessageSvc*  m_msgSvc;
public: 
  DimServiceMonObject(const std::string& svcName, std::string archiveType, MonObject *monObject);
  virtual ~DimServiceMonObject();
  IMessageSvc* msgSvc() const {return m_msgSvc;}
  const std::string&  name() const {return m_name;}
  void updateService(bool endOfRun);
  void setDataFromMonObject();
  MonObject* monObject() const {return m_monObject;};
};

#endif // DIMSERVICEMONOBJECT_H
