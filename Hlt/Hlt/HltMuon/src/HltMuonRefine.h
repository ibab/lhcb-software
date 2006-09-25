// $Id: HltMuonRefine.h,v 1.1.1.1 2006-09-25 16:14:59 asatta Exp $
#ifndef HLTMUON_HLTMUONREFINE_H 
#define HLTMUON_HLTMUONREFINE_H 1

#include "HltCommon/HltAlgorithm.h"
#include "HltCommon/HltFunctions.h"


// Include files
// from Gaudi
//#include "GaudiAlg/GaudiAlgorithm.h"
//#include "Event/EventHeader.h"
//#include "Event/L0DUReport.h"
//#include "Event/L1Score.h"
//#include "Event/TrgTrack.h"
//#include "Event/TrgVertex.h"
//#include "TrgTools/TrgDataProvider.h"
//#include "Event/Track.h"
//#include "PatTools/PatDataStore.h"
#include "Event/L0MuonCandidate.h"
//#include "TrgMuon/StoredMuonTrack.h"
#include "MuonTools/IMuonPosTool.h"
//#include "TrgTools/TrgL0MuonProvider.h"

/** @class TrgMuonrefine TrgMuonrefine.h TrgMuon/TrgMuonrefine.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2004-11-24
 */
class HltMuonRefine : public HltAlgorithm {
public: 
  /// Standard constructor
  HltMuonRefine( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltMuonRefine( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  bool acceptMatch(float dist,float momentum);
  float calcDist(float slX,float slY,float X,float Y,
                 float Z,float XMuon,float YMuon,float ZMuon);
  bool acceptMatchL0(float slX,float slY,float X,float Y,float Z,
                     double XMuon,double YMuon,double ZMuon,int region);
  

protected:

private:

  IMuonPosTool      *m_iPosTool;
  LHCb::L0MuonCandidates* m_inputL0Muon;
  
  //  std::string m_inputLongTracksName;
  // std::string m_inputMuonTracksName;
  //std::string m_outputTracksName;

  //  TrgL0MuonProvider*  m_l0MuonProvider; ///<Pointer to the muons container tool
  //TrgL0MuonContainer* m_inputL0Muons;   ///< Pointer to the L0 muons



  float m_par1;
  float m_par2;
  float m_par3;
  float m_par4;
  float m_L0X[4];
  float m_L0Y[4];
  float m_dxR1;
  float m_dxR2;
  float m_dxR3;
  float m_dxR4;
  float m_dyR1;
  float m_dyR2;
  float m_dyR3;
  float m_dyR4;
  
  bool               m_onTES; ///< Should create States ?
  //  PatTrackContainer* m_muonTracksContainer;
  //PatTrackContainer* m_tracksInput;
  //PatTrackContainer* m_tracksOutput;
  //PatStateContainer* m_states;

  // Counters
  int m_countEvents;
  int m_countRemoved;

};
#endif // HLTMUON_HLTMUONREFINE_H
