#ifndef HLTGLOBALMONITORING_H 
#define HLTGLOBALMONITORING_H 1

// Include files
// from Gaudi


#include "Event/HltSummary.h"
#include "Event/L0DUReport.h"
//#include "Event/IL0Candidate.h"
#include "HltBase/HltBaseAlg.h"

/** @class HltGlobalMonitoring HltGlobalMonitoring.h
 *
 *  functionality:
 *        monitor the global Hlt results
 *        make histograms with Hlt diagnostics info
 *
 *  @author Arthur Maciel
 *  @date   2003-03-06
 */

class HltGlobalMonitoring : public HltBaseAlg {
//class HltGlobalMonitoring : public HltAlgorithm {
public: 
  /// Standard constructor
  HltGlobalMonitoring( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltGlobalMonitoring( ); ///< Destructor

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
  void monitorAlleysinput();
  void monitoracceptAlleys();
  void monitorHLT();
  void monitorAlleySpecific();
  void monitorMuonAlley();
//  void monitorMuonHadAlley(LHCb::HltSummary* sum);
  void monitorHadronAlley();
//  void monitorCaloAlley(LHCb::HltSummary* sum);
  void monitorGlobalEvent(){;}
//  bool CheckDecision(LHCb::HltSummary* sum, int sumid);

  void starthistos(){
//L0 output histograms
  m_histoL0 = initializeHisto("L0",0.,14.,14);
  m_histoL0corr = this->book2D("Correlated L0 bits",0.,14.,14,0.,14.,14);
//Alley specific histograms
  m_histomuonalley = initializeHisto("Muon Alley fonctioning",0.,12.,12);
  m_histoMuonTrigCorr = this->book2D("Correlated Muon Alley Triggers", 2.,12.,10,2.,12.,10);
  m_histoHadronTrigCorr = this->book2D("Correlated Hadron Alley Triggers", 2.,6.,4,2.,6.,4);
  m_histohadronalley = initializeHisto("Hadron Alley fonctioning",0.,6.,6);
  m_histomuonhadalley = initializeHisto("Muon+Hadron Alley fonctioning",0.,10.,10);
  m_histocaloalley = initializeHisto("Calo Alley fonctioning",0.,10.,10);
//Standard Bins
#define binL0entry 0.5  
#define binTrigOr 2.5 
//Muon Specific Bins
#define binMuonTrigS 3.5
#define binMuonTrigDi 4.5
#define binMuonTrigIPDi 5.5
//Hadron Specific Bins
#define binHadTrig 2.5

//Interalley overview
  m_histoalleycall = initializeHisto("Alleys Called",0.,6.,6);
  m_histoalleyacc = initializeHisto("Accepted by Alley",0.,6.,6);
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
  
  Hlt::Histo* m_histoL0;
  Hlt::Histo* m_histoalleycall;
  Hlt::Histo* m_histoalleyacc;
  Hlt::Histo* m_histomuonalley;
  Hlt::Histo* m_histomuonhadalley;
  Hlt::Histo* m_histocaloalley;
  Hlt::Histo* m_histohadronalley;

  AIDA::IHistogram2D* m_histoL0corr;
  AIDA::IHistogram2D* m_histoMuonTrigCorr;
  AIDA::IHistogram2D* m_histoHadronTrigCorr;
  AIDA::IHistogram2D* m_hcorrallcall;
  AIDA::IHistogram2D* m_hcorrallacc;
  std::vector<stringKey> setKey;
  std::vector<stringKey> setKeyHad;
  std::vector<stringKey>::iterator it;
  std::vector<stringKey>::iterator it2;
  stringKey m_keymu;
  stringKey m_keyhad;
   

 private:

  int _counter1;        // "All events"
  int _counter2;       // "L0 accepted evts"
  float _efficiency;  // "Ratio counter2/counter1"
  bool muonalleyacc;
  bool hadronalleyacc;


};
#endif // HLTGLOBALMONITOR_H
