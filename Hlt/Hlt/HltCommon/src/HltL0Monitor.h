// $Id: HltL0Monitor.h,v 1.1 2006-10-24 09:44:03 hernando Exp $
#ifndef HLTL0MONITOR_H 
#define HLTL0MONITOR_H 1

// Include files
// from Gaudi


#include "Event/HltSummary.h"
#include "Event/L0DUReport.h"
#include "Event/IL0Candidate.h"
#include "HltBase/HltAlgorithm.h"

/** @class HltL0Monitor HltL0Monitor.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-01-16
 */
class HltL0Monitor : public HltAlgorithm {
public: 
  /// Standard constructor
  HltL0Monitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltL0Monitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  std::string m_CaloDataLocation;
  std::string m_MuonDataLocation;
  std::string m_PileUpDataLocation;
  std::string m_L0DUReportLocation;
  std::string m_L0DUChannelLocation;

protected:

  LHCb::L0DUReport* m_l0;
  bool m_veto;

protected:

  bool m_ADCconvert;
  
  IL0Candidate* m_CaloCandidate;
  IL0Candidate* m_MuonCandidate;
  IL0Candidate* m_PileUpCandidate;

protected:
  
  void monitorL0();
  void monitorGlobalEvent();
  void monitorCalo();
  void monitorMuon();

protected:
  
  HltHisto m_histoL0;

  HltHisto m_histoEt;  
  HltHisto m_histoSPDMult;  

  HltHisto m_histoPUHits;  
  HltHisto m_histoPUPeak2;

  HltHisto m_histoEtHad;
  HltHisto m_histoEtEle;
  HltHisto m_histoEtGamma;
  HltHisto m_histoEtPi0L;
  HltHisto m_histoEtPi0G;

  HltHisto m_histoPtMu;  
  HltHisto m_histoPtDiMu;

  HltHisto m_histoPtMu1;
  HltHisto m_histoPtDiMu1;
  HltHisto m_histoEtHad1;

};
#endif // L0ENTRY_H
