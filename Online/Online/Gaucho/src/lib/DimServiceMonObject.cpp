#include "Gaucho/DimServiceMonObject.h"
#include "Gaucho/MonObject.h"
#include "GaudiKernel/StatusCode.h"
#include <iostream>
#include <string>
#include <sstream>

DimServiceMonObject::DimServiceMonObject(std::string svcName, MonObject *monObject):DimService(svcName.c_str(), "C", m_data, 0)
{
  m_monObject = monObject;
  m_svcName = svcName;
  std::stringstream m_ss;
  m_ss.str("");
  boost::archive::binary_oarchive m_oa(m_ss);
  m_monObject->save(m_oa, m_monObject->version());
  m_data = const_cast<void *>((const void*)m_ss.str().data());
  //m_dimService = new DimService(svcName.c_str(), "", m_data, m_ss.str().length());
  
  setData(m_data, m_ss.str().length());
  DimService::updateService();
  //DimService::updateService(m_data,m_ss.str().length()); // why it doesnt work
}

DimServiceMonObject::~DimServiceMonObject() {
  //delete m_dimService; m_dimService = 0;
}

void DimServiceMonObject::updateService(bool endOfRun) {
   m_monObject->setEndOfRun(endOfRun);
   setDataFromMonObject();
}

void DimServiceMonObject::setDataFromMonObject() {
  std::stringstream m_ss;
  m_ss.str("");
  boost::archive::binary_oarchive m_oa(m_ss);
  m_monObject->save(m_oa, m_monObject->version());
  
//   std::cout << "==============================================================" << std::endl;
//   std::cout << "========================MONOBJECT=============================" << std::endl;
//   std::cout << "==============================================================" << std::endl;
//   m_monObject->print();
//   std::cout << "==============================================================" << std::endl;

  m_data = const_cast<void *>((const void*)m_ss.str().data());
  setData(m_data, m_ss.str().length());
  DimService::updateService();
  //DimService::updateService(m_data,m_ss.str().length()); // why it doesnt work
}

