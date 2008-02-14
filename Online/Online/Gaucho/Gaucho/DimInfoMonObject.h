#include "dic.hxx"
#include <string>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include <sstream>
#include <TStopwatch.h>


//  Author: jotalo, 22/10/2007

class MonObject;
class DimMonObjectManager;


class DimInfoMonObject : public DimInfo {
private:
  DimMonObjectManager*   m_dimMonObjectManager;
  int     m_rtime;
  bool    m_hasData;
  MonObject*  m_monObject;
  int     m_StringSize;
  std::string  m_svcname;
  std::string  m_name;
  
public : 
  DimInfoMonObject(DimMonObjectManager* manager, std::string svcName);
  DimInfoMonObject(DimMonObjectManager* manager, std::string svcName, int rTime);
  virtual ~DimInfoMonObject();
  virtual void infoHandler();
  void createMonObject();
  void deleteMonObject();
  void loadMonObject();
  MonObject *monObject();
  int stringSize() { return m_StringSize;};
  std::string svcName(){return m_svcname;};
};
