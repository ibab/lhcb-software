#ifndef Saver_Saver_H
#define Saver_Saver_H 1

// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "GaudiKernel/IIncidentListener.h"
#include<time.h>

/** @class Saver


*/

// Forward declarations
class IHistogram1D;
class IHistogram2D;
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
  std::string m_nodename; 
  std::vector<std::string> m_histogramname; 
  int m_refreshtime;
  std::string m_dimclientdns;
  std::string m_taskname;
  std::string m_savedir;
  std::string command;
  

};

#endif    // Saver_Saver_H
