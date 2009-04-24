
#ifndef DIMSERVICEMONOBJECT_H
#define DIMSERVICEMONOBJECT_H 1

#include <string>
#include <iostream>
#include "dis.hxx"
#include <TStopwatch.h>
#include <sstream>
#include "Gaucho/MonObject.h"

//  Author: jotalo, 22/10/2007
// class MonObject;
class DimServiceMonObject:public DimService {

private:
  //void*         m_data;
  MonObject*    m_monObject;
  std::string   m_name;
  //std::string   m_svcName;
  //std::stringstream m_ss;
  IMessageSvc* m_msgSvc;
  //DimService    *m_dimService;
  //boost::archive::binary_oarchive* m_oa;
public: 
  DimServiceMonObject(const std::string& svcName, MonObject *monObject);
  virtual ~DimServiceMonObject();
  IMessageSvc* msgSvc() const {return m_msgSvc;}
  const std::string& name() const {return m_name;}
  void updateService(bool endOfRun);
  void setDataFromMonObject();
  MonObject* monObject()  const {return m_monObject;};
};

#endif // DIMSERVICEMONOBJECT_H
