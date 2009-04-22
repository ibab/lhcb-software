#include "Gaucho/DimServiceMonBuffer.h"
#include "GaudiKernel/StatusCode.h"
#include "Gaucho/Misc.h"
#include <string>

DimServiceMonBuffer::DimServiceMonBuffer(std::string svcName, MonBuffer *monBuffer):DimService(svcName.c_str(), "C", m_data, 0)
{
  m_monBuffer = monBuffer;
  m_svcName = svcName;
  m_bufferFile = new TBufferFile(TBuffer::kWrite,256*1024); // verify those numbers
  m_monBuffer->save(m_bufferFile, m_monBuffer->version());
  const char* chardata = m_bufferFile->Buffer();
  m_data = const_cast<void *>((const void*) chardata);
  setData(m_data, m_bufferFile->Length());
  DimService::updateService();
  m_msgSvc = 0;
  m_name = "DimServiceMonBuffer";
  //DimService::updateService(m_data, m_bufferFile->Length());
}

DimServiceMonBuffer::~DimServiceMonBuffer() {
  //delete m_dimService; m_dimService = 0;
}

void DimServiceMonBuffer::updateService(bool endOfRun) {
   m_monBuffer->setEndOfRun(endOfRun);
   setDataFromMonBuffer();
}

void DimServiceMonBuffer::setDataFromMonBuffer() {
  MsgStream msg(msgSvc(), name());

  m_bufferFile->Reset();
  m_monBuffer->save(m_bufferFile, m_monBuffer->version());
  const char* c = m_bufferFile->Buffer();

  try {
    //m_data = const_cast<void *>((const void*)m_ss.str().data());
    m_data = const_cast<void *>((const void*) c);
    setData(m_data, m_bufferFile->Length());
    DimService::updateService();
  }
  catch (...) {
    msg << MSG::WARNING << "Unable setdata to DimService " << endreq;
    try {
      char* c2 =  const_cast<char *> (c);
      Misc::printSerializedString(c2, m_bufferFile->Length());
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

