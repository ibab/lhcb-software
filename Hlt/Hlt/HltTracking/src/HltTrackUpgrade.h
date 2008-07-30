// $Id: HltTrackUpgrade.h,v 1.5 2008-07-30 13:39:39 graven Exp $
#ifndef HLTTRACKING_HLTTRACKUPGRADE_H 
#define HLTTRACKING_HLTTRACKUPGRADE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/ISequencerTimerTool.h"
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
#include "HltTrackUpgradeTool.h"

/** @class HltTrackUpgrade HltTrackUpgrade.h
 *  
 *  functionality:
 *  Options:
 *        RecoName: name of the reconstruction to be done,
 *           valid names: "Velo","VeloTT","Forward","MuonForward"
 *        OrderByPt: true/false order by Pt output tracks in HltDataStore
 *
 *  Note:

 *  @author Jose Angel Hernando Morata
 *  @date   2006-08-28
 */
class HltTrackUpgrade : public HltAlgorithm {
public: 
  /// Standard constructor
  HltTrackUpgrade( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltTrackUpgrade( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  void saveConfiguration();

  std::string m_recoName;
  HltTrackUpgradeTool* m_tool;

  // 1 input: Track
  // 1 output: Track
  Hlt::SelectionContainer2<LHCb::Track,LHCb::Track> m_selections;
};
#endif // HLTTRACKING_H
