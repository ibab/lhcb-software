// $Id: HltTrackUpgrade.h,v 1.3 2008-01-22 10:04:25 hernando Exp $
#ifndef HLTTRACKING_HLTTRACKUPGRADE_H 
#define HLTTRACKING_HLTTRACKUPGRADE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/ISequencerTimerTool.h"
#include "HltBase/HltAlgorithm.h"
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
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  
  void saveConfiguration();

protected:

  std::string m_recoName;

  HltTrackUpgradeTool* m_tool;

protected:

  // already in HltAlgorithm 
  // Hlt::TrackSelection* m_inputTracks;
  // Hlt::TrackSelection* m_outputTracks;

};
#endif // HLTTRACKING_H
