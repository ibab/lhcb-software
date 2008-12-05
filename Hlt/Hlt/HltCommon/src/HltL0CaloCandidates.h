// $Id: HltL0CaloCandidates.h,v 1.1 2008-12-05 19:11:39 graven Exp $
#ifndef HLTCOMMON_HLTLOCALOPREPARE_H 
#define HLTCOMMON_HLTL0CALOPREPARE_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/ICaloSeedTool.h"
#include "GaudiKernel/Property.h"

/** @class HltHadAlleyPreTrigger HltHadAlleyPreTrigger.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-28
 */
class HltL0CaloCandidates : public HltAlgorithm {
public: 
  /// Standard constructor
  HltL0CaloCandidates( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltL0CaloCandidates( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution


private:

  void makeTrack(const LHCb::L0CaloCandidate& calo, LHCb::Track& track);
  void addExtras(const LHCb::L0CaloCandidate& calo, LHCb::Track& track);


  Hlt::SelectionContainer2<LHCb::Track,LHCb::L0CaloCandidate> m_selection; 

  std::string m_l0Location;  
  std::string m_l0Channel;  
  std::string m_caloMakerName;  
  ICaloSeedTool* m_caloMaker;

  Hlt::Histo* m_histoEt;
  Hlt::Histo* m_histoEtBest;


};
#endif // HLTHADALLEYPRETRIGGER_H
