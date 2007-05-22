// $Id: SpaceMuonMatch.h,v 1.1.1.1 2007-05-22 10:20:32 asatta Exp $
#ifndef SPACEMUONMATCH_H 
#define SPACEMUONMATCH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

/** @class SpaceMuonMatch SpaceMuonMatch.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-02-21
 */
class SpaceMuonMatch : public HltAlgorithm {
public: 
  /// Standard constructor
  SpaceMuonMatch( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SpaceMuonMatch( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  StatusCode tryMatch(LHCb::Track* track, LHCb::Track*& Ttrack);
  float calcP(LHCb::Track* velotrack,LHCb::Track* Ttrack);
  
    
protected:


private:
  float m_x_cut;
  float m_y_cut;
  float m_space_cut;
  float z_magnet_center;
  float m_ptkickConstant;
  LHCb::State* m_myState;
  std::string m_input2TracksName; 
  std::string m_outputMuonTracksName;   ///< output container name 
  Hlt::TrackContainer* m_input2Tracks;
  PatTrackContainer*
  m_outputMuonTracks;///< Pointer to the muon track containe
  PatStateContainer* m_states;

};
#endif // SPACEMUONMATCH_H
