#ifndef Saver_Saver_H
#define Saver_Saver_H 1

// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TProfile.h"
#include "GaudiKernel/IIncidentListener.h"
#include<time.h>

/** @class Saver


*/

// Forward declarations
class IIncidentSvc;
class Saver : public Algorithm, virtual public IIncidentListener {
public:
  /// Constructor of this form must be provided
  Saver(const std::string& name, ISvcLocator* pSvcLocator); 

  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  virtual void handle(const Incident& inc);
private:

  IHistogramSvc* histoSvc;
 // TFile *f;
  std::string m_nodeName; 
  std::vector<std::string> m_histogramName; 
  std::vector<std::string> m_algorithmName; 
  int m_refreshTime;
  std::string m_dimClientDns;
  std::string m_taskName;
  std::string m_saveDir;
  std::string command;
  

};

#endif    // Saver_Saver_H
