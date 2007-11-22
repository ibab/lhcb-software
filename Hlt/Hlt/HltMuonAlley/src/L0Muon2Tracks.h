// $Id: L0Muon2Tracks.h,v 1.2 2007-11-22 10:59:09 sandra Exp $
#ifndef L0MUON2TRACKS_H 
#define L0MUON2TRACKS_H 1

// Include files
// from Gaudi

#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

#include "Event/L0MuonCandidate.h"
#include "MuonDet/IMuonPosTool.h"
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
  StatusCode fillSingle(LHCb::Tracks* muontracks, LHCb::Tracks* muonSelected) ;
  StatusCode fillDiMuon(LHCb::Tracks* muontracks, LHCb::RecVertices* muonpairs);
  StatusCode  fillMuon( LHCb::L0MuonCandidate* muon, LHCb::Track& outTr);
  StatusCode fillVertex( LHCb::Track* muonone, LHCb::Track* muontwo,LHCb::RecVertices* muonpairs);
  bool checkClone(LHCb::L0MuonCandidate* muon, LHCb::Tracks* muontracks);
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
  IMuonPosTool      *m_iPosTool;
  bool m_rejClone;
  

};
#endif // L0MUON2TRACKS_H
