#ifndef DimInfoMonBuffer_H
#define DimInfoMonBuffer_H 1

#include "dic.hxx"
#include <string>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include <TStopwatch.h>
#include <TBufferFile.h>


/** @class ClassName DimInfoMonBuffer.h 
 *
 *  DimInfoMonBuffer class
 *
 *  @author Juan Otalora
 *  @date 16/02/2009
 */

class MonBuffer;
class DimInfo;

class DimInfoMonBuffer {
private:
  bool         m_hasData;
  std::string  m_name;
  std::string  m_svcName;
  std::string  m_source;
  std::string  m_noValidMonBuffer;
  MonBuffer*   m_monBuffer;
  IMessageSvc* m_msgSvc;
  int          m_StringSize;
  DimInfo*     m_dimInfo;

public : 
  DimInfoMonBuffer(const std::string svcName, int refreshTime);
  DimInfoMonBuffer(const std::string svcName, int refreshTime, std::string source);
  virtual ~DimInfoMonBuffer();
  void infoHandler();
  void setSourceName(std::string source) {m_source = source;}
  void setMsgSvc(IMessageSvc* msgSvc){m_msgSvc = msgSvc;}
  IMessageSvc* msgSvc(){return m_msgSvc;}
  std::string  name() {return m_name;}  
  bool createMonBuffer();
  bool loadMonBuffer();
  MonBuffer *monBuffer();
  DimInfo*  dimInfo(){ return m_dimInfo;};
  int stringSize() const { return m_StringSize;};
  std::string svcName() {return m_svcName;};
};
#endif    // DimInfoMonBuffer_H

