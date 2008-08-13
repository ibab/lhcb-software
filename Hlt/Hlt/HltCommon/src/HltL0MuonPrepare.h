// $Id: HltL0MuonPrepare.h,v 1.3 2008-08-13 07:19:27 graven Exp $
#ifndef HLTCOMMON_HLTL0MUONPREPARE_H 
#define HLTCOMMON_HLTL0MUONPREPARE_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/IMuonSeedTool.h"
#include "Event/L0MuonCandidate.h"

/** @class HltHadAlleyPreTrigger HltHadAlleyPreTrigger.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-28
 */
class HltL0MuonPrepare : public HltAlgorithm {
public: 
  /// Standard constructor
  HltL0MuonPrepare( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltL0MuonPrepare( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution


private:

  double m_PtMin;
  std::string m_outputMuonTracksName; 
  IMuonSeedTool* m_maker;

  bool checkClone(LHCb::L0MuonCandidate* muon);
  Hlt::SelectionContainer2<LHCb::Track,LHCb::L0MuonCandidate> m_selections;

};
#endif // HLTHADALLEYPRETRIGGER_H
