#include "Gaucho/DimServiceMonObject.h"
#include "Gaucho/MonObject.h"
#include "GaudiKernel/StatusCode.h"
#include <iostream>
#include <string>
#include <sstream>

DimServiceMonObject::DimServiceMonObject(std::string svcName, MonObject *monObject, int size) : DimService(svcName.c_str(),"C",m_data,size), m_data(0) {

  m_monObject = monObject;
  m_svcName = svcName;
  enableHandler();
  setDataFromMonObject();
}

DimServiceMonObject::~DimServiceMonObject() {

}

void DimServiceMonObject::updateService(bool endOfRun) {
  m_monObject->setEndOfRun(endOfRun);
  DimService::updateService();
}

void DimServiceMonObject::serviceHandler() {
  if (!m_enableHandler) return;
  setDataFromMonObject();
  return;
}

void DimServiceMonObject::setDataFromMonObject() {
  m_ss.str("");
  boost::archive::binary_oarchive oa(m_ss);
  m_monObject->save(oa, m_monObject->version());
  setData(const_cast<void *>((const void*)m_ss.str().c_str()), m_ss.str().length());
}

