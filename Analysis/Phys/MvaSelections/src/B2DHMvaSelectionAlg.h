// $Id: B2DHMvaSelectionAlg.h,v 1.1 2010/06/10 12:42:21 seaso Exp $
#ifndef B2DHMVASELECTIONALG_H 
#define B2DHMVASELECTIONALG_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"
#include "IBs2DsHMvaUtilityTool.h"
#include "IB2DHSelectionParamTool.h"
#include "IB2DHSelectionCutsTool.h"
#include "IB2DHMvaMonitorHistoTool.h"


/** @class B2DHMvaSelectionAlg B2DHMvaSelectionAlg.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2009-07-21
 */
class B2DHMvaSelectionAlg : public DVAlgorithm {
public: 
  /// Standard constructor
  B2DHMvaSelectionAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~B2DHMvaSelectionAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  const LHCb::Particle* performB2DHSelection ();
  const  LHCb::RecVertex* getBestRefittedPVForBs(const LHCb::Particle* b);
  const  LHCb::RecVertex* AcquirePVForBsFromStdPV ( const LHCb::Particle* b );
  const  LHCb::RecVertex* getBestRefittedPVForBsFromSortedRelations(const LHCb::Particle* b);
  
  void  resetEventSelectionParams();
  void resetSelectionCounters();
  void PrintEvSelCounters();
  bool getL0Decision ();
  
protected:

private:

  IBs2DsHMvaUtilityTool* m_Bs2DsHMvaUtilityTool;

  IB2DHSelectionParamTool* m_IB2DHSelectionParamTool;
  
  IB2DHMvaMonitorHistoTool * m_B2DHMvaMonitorHistoTool ;

  IB2DHSelectionCutsTool* m_B2DHSelectionCutsTool;
  

  int m_curDecayChannel;
  
  bool m_activateHistograms;
  bool m_activateL0Selection;


  std::vector<double> m_fProb;
  std::vector<double> m_fIps;

  std::vector<int> m_fCandIndex;
  
  int m_numCandInEvent;
  int m_curbestCandIndexInEvent;


  std::vector<int> m_fCandNoSel;
  std::vector<int> m_fCandPrelimSel;
  std::vector<int> m_fCandFisherSel;
  std::vector<int> m_fCandAuxSel;
  std::vector<int> m_fCandAuxMassSel;
  
  

  int m_PVSelCandInEv;
  int m_PreLimSelCandInEv;
  int m_FisherSelCandInEv;
  int m_AuxCutSelCandInEv;
  int m_AllSelCandBeforeL0InEv;
  int m_AllSelCandWithL0InEv;


  int m_numSelectedEvCounter;
  int m_numTotEvCounter;
  int m_numEvWithBs;
  int m_numEvWitbBsAndPV;

  int m_numEvPrelimSel;
  int m_numEvWithFisher;
  int m_numEvWithAuxCuts;
  int m_numEvAllSelBeforeL0;
  int m_numEvAllSelWithL0;
  int m_numNotSelectedEvCounter;
  
};
#endif // B2DHMVASELECTIONALG_H
