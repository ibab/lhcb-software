#ifndef MONOBJECTTEXTPRINTER_H
#define MONOBJECTTEXTPRINTER_H 1

// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "Gaucho/MonObject.h"
#include "TFile.h"
#include "dic.hxx"
#include "dis.hxx"
#include<time.h>
#include<map>
#include<set>

/** @class MonObjectTextPrinter


*/

// Forward declarations
class IIncidentSvc;
class DimInfoMonObject;
class MonObjectTextPrinter : public Algorithm {
public:
  /// Constructor of this form must be provided
  MonObjectTextPrinter(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~MonObjectTextPrinter();
  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
private:

  IHistogramSvc* histoSvc;
  std::string m_serverName; 
  std::vector<std::string> m_nodeName;
  std::vector<std::string> m_monObjectName; 
  std::vector<std::string> m_algorithmName;
  std::vector<std::string> m_taskName; 
  
  std::string m_svcname; 
  int m_refreshtime;
  std::string m_dimclientdns;
  DimBrowser   m_dimBrowser;
  DimInfoMonObject *m_dimInfoMonObject;
  bool m_monObjectCreated;
  static std::vector<std::string>  splitString(const std::string &str, const char *s);

};
#endif    // MONOBJECTTEXTPRINTER_H


