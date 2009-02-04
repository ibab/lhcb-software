#include "Gaucho/DimServiceMonObject.h"
#include "GaudiKernel/StatusCode.h"
#include "Gaucho/Misc.h"
#include <iostream>
#include <string>
#include <sstream>

DimServiceMonObject::DimServiceMonObject(std::string svcName, MonObject *monObject):DimService(svcName.c_str(), "C", m_data, 0)
{
  m_monObject = monObject;
  m_svcName = svcName;
  m_ss.str("");
  boost::archive::binary_oarchive oa(m_ss);
  m_monObject->save(oa, m_monObject->version());
  m_data = const_cast<void *>((const void*)m_ss.str().data());
  //m_dimService = new DimService(svcName.c_str(), "", m_data, m_ss.str().length());
  
  setData(m_data, m_ss.str().length());
  DimService::updateService();
  m_msgSvc = 0;
  m_name = "DimServiceMonObject";
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
  MsgStream msg(msgSvc(), name());
  m_ss.str("");
//  delete m_oa;
//  m_oa = new boost::archive::binary_oarchive(m_ss);
  boost::archive::binary_oarchive oa(m_ss);
  m_monObject->save(oa, m_monObject->version());

  const char* c = m_ss.str().data();
  try {
    //m_data = const_cast<void *>((const void*)m_ss.str().data());
    m_data = const_cast<void *>((const void*) c);
    setData(m_data, m_ss.str().length());
    DimService::updateService();
  }
  catch (...) {
    msg << MSG::WARNING << "Unable setdata to DimService " << endreq;
    try {
      char* c2 =  const_cast<char *> (c);
      Misc::printSerializedString(c2, m_ss.str().size());
    }
    catch (const std::exception &ex){
      msg << MSG::WARNING << "std::exception: " << ex.what() << endreq;
    }
    catch (...){
      msg << MSG::WARNING << "unrecognized exception. "<< endreq;
    }
  }
//DimService::updateService(m_data,m_ss.str().length()); // why it doesnt work
}

