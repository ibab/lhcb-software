#ifndef HLTGLOBALMONITOR_H 
#define HLTGLOBALMONITOR_H 1

#include "Event/ODIN.h"
#include "Event/L0DUReport.h"
#include "Event/HltDecReports.h"

#include "HltBase/HltBaseAlg.h"

/** @class HltGlobalMonitor HltGlobalMonitor.h
 *
 *  functionality:
 *        monitor the global Hlt results
 *        make histograms with Hlt diagnostics info
 *
 *  @author Arthur Maciel
 *  @date   2003-03-06
 */

class HltGlobalMonitor : public HltBaseAlg {
public: 
  /// Standard constructor
  HltGlobalMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltGlobalMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  std::string m_ODINLocation;
  std::string m_L0DUReportLocation;
  std::string m_HltDecReportsLocation;
  std::vector<std::string>  m_Hlt1Lines;
  
  void monitorODIN(const LHCb::ODIN&);
  void monitorL0DU(const LHCb::L0DUReport&);
  void monitorHLT(const LHCb::HltDecReports&);


  // hists
private:
  
  AIDA::IHistogram1D* m_histoL0;
  AIDA::IHistogram1D* m_histoalleycall; 
  AIDA::IHistogram1D* m_histoodintype; 
  AIDA::IHistogram1D* m_histoodinentry; 

  AIDA::IHistogram2D* m_histoL0corr;

 private:

  int m_hlt1allcall;        // "All events"
  int m_counter2;       // "L0 accepted evts"
  double m_efficiency;  // "Ratio counter2/hlt1allcall"
  std::vector<double> m_allAcc;
  std::vector<double> m_allCall;
  double m_physallcall;
  double m_randallcall;
  double m_orallacc;
  unsigned long long gpstime;
  int gpstimesec;
};
#endif // HLTGLOBALMONITOR_H
