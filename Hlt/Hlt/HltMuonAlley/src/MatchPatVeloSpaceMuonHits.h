// $Id: MatchPatVeloSpaceMuonHits.h,v 1.2 2006-09-27 14:18:04 cattanem Exp $
#ifndef MATCHPATVELOSPACEMUONHITS_H 
#define MATCHPATVELOSPACEMUONHITS_H 1

// Include files
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

#include "PatTools/PatDataStore.h"
#include "Event/L0MuonCandidate.h"
#include "MuonDet/IMuonPosTool.h"


/** @class MatchPatVeloSpaceMuonHits MatchPatVeloSpaceMuonHits.h
 *   
 *
 *  @author Sandra Amato 
 *  @date   2006-03-07
 */
class MatchPatVeloSpaceMuonHits : public HltAlgorithm {
public: 
  /// Standard constructor
  MatchPatVeloSpaceMuonHits( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MatchPatVeloSpaceMuonHits( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


protected:

  bool acceptTrack(const LHCb::Track& track);

private:
  StatusCode muonMatch();
  

private:
  double  m_ptkickConstant; ///< ptkickConstant option

  std::string m_outputMuonTracksName;   ///< output container name

  bool m_checkMC;

  PatTrackContainer* m_outputMuonTracks;///< Pointer to the muon track container
  PatStateContainer* m_states;
  LHCb::State* m_myState;
 
  LHCb::L0MuonCandidates* m_inputL0Muon;
  
  std::vector<float> m_x_cut_track;
  std::vector<float> m_y_cut_track;
  
  float z_magnet_center;

  HltHisto m_histoYdist;
  HltHisto m_histoXdist;
  HltHisto h_patOutTrack; 
  IMuonPosTool      *m_iPosTool;

};
#endif // MATCHPATVELOSPACEMUONHITS_H
