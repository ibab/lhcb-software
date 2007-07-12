// $Id: L0Muon2Tracks.h,v 1.1 2007-07-12 18:03:15 asatta Exp $
#ifndef L0MUON2TRACKS_H 
#define L0MUON2TRACKS_H 1

// Include files
// from Gaudi

#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

#include "Event/L0MuonCandidate.h"
#include "MuonDet/IMuonPosTool.h"
#include "PatTools/PatDataStore.h"
/** @class L0Muon2Tracks L0Muon2Tracks.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-01-03
 */
class L0Muon2Tracks :  public HltAlgorithm {
public: 
  /// Standard constructor
  L0Muon2Tracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0Muon2Tracks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  StatusCode fillSingle() ;
  StatusCode fillDiMuon();
  StatusCode  fillMuon( LHCb::L0MuonCandidate* muon);
  StatusCode fillVertex( LHCb::Track* muonone, LHCb::Track* muontwo);
  bool checkClone(LHCb::L0MuonCandidate* muon);
  bool isIn(LHCb::Track* track);
  
protected:

private:
  std::string m_L0MuonType;
  float m_L0MuonThreshold;
  std::string  m_outputL0MuonTracksName;
  std::string  m_outputL0MuonVerticesName;
  
  LHCb::L0MuonCandidates* m_inputL0Muon;
  LHCb::State* m_myState;  
  LHCb::State* m_vertexState;
  PatTrackContainer*
  m_outputL0MuonTracks;///< Pointer to the L0muon track containe
  PatStateContainer* m_states;
  IMuonPosTool      *m_iPosTool;
  PatVertexContainer* m_patVertexBank;
  bool m_rejClone;
  

};
#endif // L0MUON2TRACKS_H
