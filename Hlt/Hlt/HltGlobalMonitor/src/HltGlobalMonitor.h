#ifndef HLTGLOBALMONITOR_H 
#define HLTGLOBALMONITOR_H 1

// Include files
// from Gaudi


#include "Event/HltSummary.h"
#include "Event/L0DUReport.h"
//#include "Event/IL0Candidate.h"
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
//class HltGlobalMonitor : public HltAlgorithm {
public: 
  /// Standard constructor
  HltGlobalMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltGlobalMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  //  sao enderecos na transient store
  std::string m_L0DUReportLocation;
  std::vector<std::string> m_Hlt1Lines;
  //  std::string m_PileUpDataLocation;
  //  std::string m_L0DUChannelLocation;

  LHCb::L0DUReport* m_l0;

  // metodos
  
  void monitorL0();
  void monitorAlleysinput();
//  void monitoracceptAlleys(LHCb::HltSummary* sum);
  void monitorHLT();
  void monitorAlleySpecific(LHCb::HltSummary* sum);
  void monitorGlobalEvent(){;}
//  bool CheckDecision(LHCb::HltSummary* sum, int sumid);

  void starthistos(){
//L0 output histograms
//  m_histoL0 = initializeHisto("L0 bits",0.,14.,14);
  m_histoL0corr = this->book2D("Correlated L0 bits",0.,14.,14,0.,14.,14);

}

  // hists
protected:
  
  AIDA::IHistogram1D* m_histoL0;
  AIDA::IHistogram1D* m_histoalleycall; 
  AIDA::IHistogram1D* m_histoodintype; 
  AIDA::IHistogram1D* m_histoodinentry; 
//  Hlt::Histo* m_histoalleyacc;
//  Hlt::Histo* m_histomuonalley;
//  Hlt::Histo* m_histomuonhadalley;
//  Hlt::Histo* m_histocaloalley;
//  Hlt::Histo* m_histohadronalley;

  AIDA::IHistogram2D* m_histoL0corr;
//  AIDA::IHistogram2D* m_histoMuonTrigCorr;
//  AIDA::IHistogram2D* m_hcorrallcall;
//  AIDA::IHistogram2D* m_hcorrallacc;

 private:

  std::string m_location;
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
