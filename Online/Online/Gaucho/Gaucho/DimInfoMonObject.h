#ifndef DimInfoMonObject_H
#define DimInfoMonObject_H 1

#include "dic.hxx"
#include <string>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include <sstream>
#include <TStopwatch.h>


//  Author: jotalo, 22/10/2007

class MonObject;
class DimInfo;

class DimInfoMonObject {
private:
  bool         m_hasData;
  std::string  m_name;
  std::string  m_svcName;
  std::string  m_source;
  MonObject*   m_monObject;
  IMessageSvc* m_msgSvc;
  int          m_StringSize;
  DimInfo*     m_dimInfo;
  
public : 
  DimInfoMonObject(std::string svcName, int refreshTime);
  DimInfoMonObject(std::string svcName, int refreshTime, std::string source);
  virtual ~DimInfoMonObject();
  void infoHandler();
  void setSourceName(std::string source) {m_source = source;}
  void setMsgSvc(IMessageSvc* msgSvc){m_msgSvc = msgSvc;}
  IMessageSvc* msgSvc(){return m_msgSvc;}
  std::string  name(){return m_name;}  
  bool createMonObject();
  bool loadMonObject();
  MonObject *monObject();
  DimInfo*  dimInfo(){ return m_dimInfo;};
  int stringSize() const { return m_StringSize;};
  std::string svcName() const {return m_svcName;};
};  
#endif    // DimInfoMonObject_H

