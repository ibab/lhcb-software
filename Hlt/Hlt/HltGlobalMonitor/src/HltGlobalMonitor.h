#ifndef HLTGLOBALMONITOR_H 
#define HLTGLOBALMONITOR_H 1

// Include files
// from Gaudi


#include "Event/HltSummary.h"
#include "Event/L0DUReport.h"
//#include "Event/IL0Candidate.h"
#include "HltBase/HltMonitorAlgorithm.h"

/** @class HltGlobalMonitor HltGlobalMonitor.h
 *
 *  functionality:
 *        monitor the global Hlt results
 *        make histograms with Hlt diagnostics info
 *
 *  @author Arthur Maciel
 *  @date   2003-03-06
 */

class HltGlobalMonitor : public HltMonitorAlgorithm {
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
  //  std::string m_PileUpDataLocation;
  //  std::string m_L0DUChannelLocation;

  LHCb::L0DUReport* m_l0;

  // metodos
protected:
  
  void monitorL0();
  void monitorAlleysinput(LHCb::HltSummary* sum);
  void monitoracceptAlleys(LHCb::HltSummary* sum);
  void monitorHLT(LHCb::HltSummary* sum);
  void monitorAlleySpecific(LHCb::HltSummary* sum);
  void monitorMuonAlley(LHCb::HltSummary* sum);
  void monitorMuonHadAlley(LHCb::HltSummary* sum);
  void monitorHadronAlley(LHCb::HltSummary* sum);
  void monitorCaloAlley(LHCb::HltSummary* sum);
  void monitorGlobalEvent(){;}
  bool CheckDecision(LHCb::HltSummary* sum, int sumid);

  void starthistos(){
//L0 output histograms
  initializeHisto(m_histoL0,"L0",0.,14.,14);
  m_histoL0corr = this->book2D("Correlated L0 bits",0.,14.,14,0.,14.,14);
//Alley specific histograms
  initializeHisto(m_histomuonalley,"Muon Alley fonctioning",0.,10.,10);
  m_histoMuonTrigCorr = this->book2D("Correlated Muon Alley Triggers", 4.,10.,6,4.,10.,6);
  initializeHisto(m_histohadronalley,"Hadron Alley fonctioning",0.,10.,10);
  initializeHisto(m_histomuonhadalley,"Muon+Hadron Alley fonctioning",0.,10.,10);
  initializeHisto(m_histocaloalley,"Calo Alley fonctioning",0.,10.,10);
//Standard Bins
#define binL0entry 0.5  
#define binL0conf 1.5  
#define binTrigOr 3.5 
//Muon Specific Bins
#define binMuonTrigS 4.5
#define binMuonTrigDi 5.5
#define binMuonTrigIPDi 6.5
//Hadron Specific Bins
#define binHadTrig 4.5

//Interalley overview
  initializeHisto(m_histoalleycall,"Alleys Called",0.,6.,6);
  initializeHisto(m_histoalleyacc,"Accepted by Alley",0.,6.,6);
  m_hcorrallcall = this->book2D("Correlation Calls",0.,6.,6,0.,6.,6);
  m_hcorrallacc = this->book2D("Correlated Alleys",0.,6.,6,0.,6.,6);

//Bin assigned to each Alley
#define binAlleyOr 0.5
#define binMuonAlley 1.5
#define binMuonHadAlley 2.5
#define binHadronAlley 3.5
#define binCaloAlley 4.5
}

  // hists
protected:
  
  HltHisto m_histoL0;
  HltHisto m_histoalleycall;
  HltHisto m_histoalleyacc;
  HltHisto m_histomuonalley;
  HltHisto m_histomuonhadalley;
  HltHisto m_histocaloalley;
  HltHisto m_histohadronalley;

  AIDA::IHistogram2D* m_histoL0corr;
  AIDA::IHistogram2D* m_histoMuonTrigCorr;
  AIDA::IHistogram2D* m_hcorrallcall;
  AIDA::IHistogram2D* m_hcorrallacc;

 private:

  int _counter1;        // "All events"
  int _counter2;       // "L0 accepted evts"
  float _efficiency;  // "Ratio counter2/counter1"


};
#endif // HLTGLOBALMONITOR_H
