// $Id: DecisionUnit.h,v 1.4 2002-04-05 15:20:44 ocallot Exp $
#ifndef L0DU_DECISIONUNIT_H
#define L0DU_DECISIONUNIT_H

// Include files
#include <string>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "Event/L0MuonCandidate.h"
#include "Event/L0Muon.h"
#include "Event/L0CaloCandidate.h"
#include "Event/L0DUReport.h"
  
/** @class DecisionUnit DecisionUnit.h
 *
 *  Algorithm which choice the type of L0 trigger from L0Calo and L0Muon data 
 *
 *  @author  Nicole Brun
 *  @author  Pascal Perret
 *  @date    15 June 2001
 */

class DecisionUnit : public Algorithm {
  
public:
  
  // Constructor

  DecisionUnit( const std::string& name, ISvcLocator* svcloc );

  // Destructor

  virtual ~DecisionUnit(){};

  // Override default behaviour

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

  void setTrig( L0Trig::Trig cand,
                double etCand,
                double eCut1 ); 
  void setTrig_down( L0Trig::Trig cand,
                     double etCand,
                     double eCut2,
                     int scal );

  std::string m_nameOfInputL0CaloCandidate;
  std::string m_nameOfInputL0MuonCandidate;
  std::string m_nameOfInputPileUpVeto;
  std::string m_nameOfOutputDecisionUnit;
 
  int              m_typeL0Trig;
  std::vector<int> m_nEtSupCut2;

  L0CaloCandidate* m_Electron;
  L0CaloCandidate* m_Photon;
  L0CaloCandidate* m_Hadron;
  L0CaloCandidate* m_SumEt;
  L0CaloCandidate* m_Pi0Local;
  L0CaloCandidate* m_Pi0Global;
  L0MuonCandidate* m_Muon1;
  
  // For El, Ph, Ha, SumEt, Pi0L and Pi0G ( candidates of L0Calo ),
  //  for 1 Muon by pt decreasing ( candidates of L0Muon )
  //    and Sum n muons,
  //   2 energies of cut and 1 data of scaling

  double m_eElCut1;
  double m_eElCut2;
  int    m_scalEl;
  
  double m_ePhCut1;
  double m_ePhCut2;
  int    m_scalPh;  
   
  double m_eHaCut1;
  double m_eHaCut2;
  int    m_scalHa;

  double m_eSumEtCut1;
  double m_eSumEtCut2;
  int    m_scalSumEt;

  double m_ePi0LCut1;
  double m_ePi0LCut2;
  int    m_scalPi0L;

  double m_ePi0GCut1;
  double m_ePi0GCut2;
  int    m_scalPi0G;

  double m_eMu1Cut1;
  double m_eMu1Cut2;
  int    m_scalMu1;

  double m_eSumMuCut1;
  double m_eSumMuCut2;
  int    m_scalSumMu;
  int    m_nMuSumMu;
  
};

#endif  // L0DU_DECISIONUNIT_H



















