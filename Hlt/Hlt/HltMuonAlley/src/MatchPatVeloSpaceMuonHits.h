// $Id: MatchPatVeloSpaceMuonHits.h,v 1.1.1.1 2006-09-25 16:29:11 asatta Exp $
#ifndef MATCHPATVELOSPACEMUONHITS_H 
#define MATCHPATVELOSPACEMUONHITS_H 1

// Include files
// from Gaudi

#include "HltCommon/HltAlgorithm.h"
#include "HltCommon/HltFunctions.h"


#include "PatTools/PatDataStore.h"
#include "Event/L0MuonCandidate.h"
#include "MuonTools/IMuonPosTool.h"


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

  

  
  // PatDataStore*       m_store;     ///< Pointer to the PatDataStore
    bool m_checkMC;

  PatTrackContainer*     m_outputMuonTracks;///< Pointer to the muon track containe
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
