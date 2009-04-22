
#ifndef DIMSERVICEMONBUFFER_H
#define DIMSERVICEMONBUFFER_H 1

#include <string>
#include "dis.hxx"
#include <TStopwatch.h>
#include <TBufferFile.h>
#include "Gaucho/MonBuffer.h"

/** @class ClassName MonBufferObject.h 
 *
 *  DimServiceMonBuffer class
 *
 *  @author Juan Otalora
 *  @date 16/02/2009
 */
class DimServiceMonBuffer:public DimService {

private:
  void*         m_data;
  MonBuffer*    m_monBuffer;
  std::string   m_name;
  std::string   m_svcName;
  TBufferFile*   m_bufferFile;
  IMessageSvc* m_msgSvc;
  //DimService    *m_dimService;
  //boost::archive::binary_oarchive* m_oa;
public: 
  DimServiceMonBuffer(std::string svcName, MonBuffer *monBuffer);
  virtual ~DimServiceMonBuffer();
  IMessageSvc* msgSvc(){return m_msgSvc;}
  std::string  name(){return m_name;}
  void updateService(bool endOfRun);
  void setDataFromMonBuffer();
  MonBuffer* monBuffer(){return m_monBuffer;};
};

#endif // DIMSERVICEMONBUFFER_H
