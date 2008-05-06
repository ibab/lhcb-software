#ifndef Saver_Saver_H
#define Saver_Saver_H 1

// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TFile.h"
#include "dis.hxx"

#include<time.h>

class IIncidentSvc;
class DimInfoMonObject;
class DimInfoServerStatus;
class MonObject;

class Saver : public Algorithm, virtual public IIncidentListener {
public:
  Saver(const std::string& name, ISvcLocator* pSvcLocator); 
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  virtual void handle(const Incident& inc);

private:
  void WriteMonObjects();
  void CreateDimNames(std::string nodeName, std::string taskName, std::vector<std::string> algorithmName, std::vector<std::string> objectName);
  void CreateMonObjects(int refreshTime);
  void DeleteMonObjects();
  std::string getServerName(const std::string &serviceName);

  // Ask Eric if this service is necessary here...
  IHistogramSvc* histoSvc;
  std::string   m_source;
  std::vector<std::vector<MonObject*> >  m_monObjectVect2;
  std::vector<std::vector<std::string> >  m_dimInfoNamesVect2;
  std::vector<std::vector<DimInfoMonObject*> >  m_dimInfoMonObjectVect2;
  std::string m_nodeName; 
  std::vector<std::string> m_objectName; 
  std::vector<std::string> m_algorithmName; 
  std::vector<std::string> m_taskName;
  int m_refreshTime;
  std::string m_dimClientDns;
  std::string m_saveDir;
  std::string m_command;
  std::string m_fileName;
  int m_fileSize;
  DimInfoServerStatus *m_dimInfoServerStatus;
  std::map<std::string, bool, std::less<std::string> > *m_serverStatus;
  std::map<std::string, bool, std::less<std::string> >::iterator  m_serverIt;

};

#endif    // Saver_Saver_H
