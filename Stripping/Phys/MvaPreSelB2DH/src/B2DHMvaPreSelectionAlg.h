// $Id: $
#ifndef B2DHMVAPRESELECTIONALG_H 
#define B2DHMVAPRESELECTIONALG_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciHistoAlgorithm.h"
#include "IB2DHPreselMvaUtilityTool.h"
#include "IB2DHMvaPreSelectionParamTool.h"
#include "IB2DHMvaPreSelectionCutsTool.h"
#include "IB2DHPreselMvaFisherDMonitorHistoTool.h"


/** @class B2DHMvaPreSelectionAlg B2DHMvaPreSelectionAlg.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2010-07-19
 */
class B2DHMvaPreSelectionAlg : public DaVinciHistoAlgorithm {
public: 
  /// Standard constructor
  B2DHMvaPreSelectionAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~B2DHMvaPreSelectionAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  const LHCb::Particle* performB2DHSelection (const LHCb::Particles * partsBs);

  const  LHCb::RecVertex* getBestRefittedPVForBs(const LHCb::Particle* b,LHCb::RecVertex::Container* aStdPVs);
  const  LHCb::RecVertex* AcquirePVForBsFromStdPV ( const LHCb::Particle* b,LHCb::RecVertex::Container* aStdPVs );
  const  LHCb::RecVertex* getBestRefittedPVForBsFromSortedRelations(const LHCb::Particle* b,LHCb::RecVertex::Container* aStdPVs);
  void BumpMvaStrEvCounters();
  
  void  resetEventSelectionParams(int aNumCandSize);
  void resetSelectionCounters();
  void PrintEvSelCounters();
  bool getL0Decision ();

protected:

private:
  IB2DHPreselMvaUtilityTool * m_B2DHPreselMvaUtilityTool;

  IB2DHMvaPreSelectionParamTool * m_B2DHMvaPreSelectionParamTool;
  
  IB2DHPreselMvaFisherDMonitorHistoTool * m_B2DHPreselMvaFisherDMonitorHistoTool ;

  IB2DHMvaPreSelectionCutsTool * m_B2DHMvaPreSelectionCutsTool;
  int m_curDecayChannel;
  
  bool m_activateHistograms;
  bool m_activateL0Selection;
  bool m_activatedPVRefit;
  bool m_searchBestCandInEv;
  
  

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
  
  
  int m_BCandinEv;
  int m_PVSelCandInEv;
  int m_PreLimSelCandInEv;
  int m_FisherSelCandInEv;
  int m_AuxCutSelCandInEv;
  int m_AllSelCandBeforeL0InEv;
  int m_AllSelCandWithL0InEv;


  int m_numSelectedEvCounter;
  int m_numTotEvCounter;
  int m_numEvWithBs;

  int m_numEvWitbBsAllChannelsInput;
  int m_numEvWitbBsAndPV;

  int m_numEvPrelimSel;
  int m_numEvWithFisher;
  int m_numEvWithAuxCuts;
  int m_numEvAllSelBeforeL0;
  int m_numEvAllSelWithL0;
  int m_numNotSelectedEvCounter;

  bool m_ActivateStrippingSelection;

    
   
};
#endif // B2DHMVAPRESELECTIONALG_H
