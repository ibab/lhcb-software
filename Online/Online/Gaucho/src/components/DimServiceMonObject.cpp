#include "Gaucho/DimServiceMonObject.h"
#include "Gaucho/MonObject.h"
#include "GaudiKernel/StatusCode.h"
#include <iostream>
#include <string>
#include <sstream>

DimServiceMonObject::DimServiceMonObject(std::string svcName, MonObject *monObject)
{
  m_monObject = monObject;
  m_svcName = svcName;
  
  m_ss.str("");
  boost::archive::binary_oarchive m_oa(m_ss);
  m_monObject->save(m_oa, m_monObject->version());
  m_data = const_cast<void *>((const void*)m_ss.str().data());
  m_dimService = new DimService(svcName.c_str(), "", m_data, m_ss.str().length());
  m_dimService->setData(m_data, m_ss.str().length());
}

DimServiceMonObject::~DimServiceMonObject() {
  delete m_dimService; m_dimService = 0;
}

void DimServiceMonObject::updateServiceMonObject(bool endOfRun) {
   m_monObject->setEndOfRun(endOfRun);
   setDataFromMonObject();
}

void DimServiceMonObject::setDataFromMonObject() {
  m_ss.str("");
  boost::archive::binary_oarchive m_oa(m_ss);
  m_monObject->save(m_oa, m_monObject->version());
  
//   std::cout << "==============================================================" << std::endl;
//   std::cout << "========================MONOBJECT=============================" << std::endl;
//   std::cout << "==============================================================" << std::endl;
//   m_monObject->print();
//   std::cout << "==============================================================" << std::endl;

  m_data = const_cast<void *>((const void*)m_ss.str().data());
  m_dimService->setData(m_data, m_ss.str().length());
  //DimService::updateService(); // why it doesnt work
}

