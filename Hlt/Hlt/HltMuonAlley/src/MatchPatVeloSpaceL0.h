// $Id: MatchPatVeloSpaceL0.h,v 1.2 2006-09-27 14:18:04 cattanem Exp $
#ifndef MATCHPATVELOSPACEL0_H 
#define MATCHPATVELOSPACEL0_H 1

// Include files
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

#include "PatTools/PatDataStore.h"
#include "Event/L0MuonCandidate.h"
#include "MuonDet/IMuonPosTool.h"


/** @class MatchPatVeloSpaceL0 MatchPatVeloSpaceL0.h
 *   
 *  Implementation of matching PatVeloSpace with L0MuonCandidates
 *  Based on the implementation by  Mariusz Witek
 *  This version uses the new track event model
 *  @author Sandra Amato
 *  @date   2006-02-14
 */
class MatchPatVeloSpaceL0 : public HltAlgorithm {
 public: 
  /// Standard constructor
  MatchPatVeloSpaceL0( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~MatchPatVeloSpaceL0( ); ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
  double eCorMuon(const int &istation,
                  const double &e,
                  double &de);
  
 protected:
  
  bool acceptTrack(const LHCb::Track& track);
  
 private:
  StatusCode match3dMuon(LHCb::Track* ptr, LHCb::MuonTileID &, float & chi );
   LHCb::L0MuonCandidates* m_inputL0Muon;
  
  IMuonPosTool      *m_iPosTool;
  
  double  m_ptkickConstant;
  double  m_m1EcorParams0;
  double  m_m1EcorParams1;
  double  m_m1EcorParams2;
  double  m_m1EresParams0;
  double  m_m1EresParams1;
  double  m_nom1EcorParams0;
  double  m_nom1EcorParams1;
  double  m_nom1EcorParams2;
  double  m_nom1EresParams0;
  double  m_nom1EresParams1;
  double  m_maxChi2MatchMuon3d; ///< Maximum chi2 to accept matching
  bool m_checkMC;
  double m_sqrt12;
  
  
  LHCb::State* m_myState;
  
  
  std::string m_outputMuonTracksName;   ///< output container name
  
  
//  PatDataStore*       m_store;     ///< Pointer to the PatDataStore
  
  PatTrackContainer*     
  m_outputMuonTracks;///< Pointer to the muon track containe
  PatStateContainer* m_states;

  HltHisto m_histoChi;
HltHisto h_patOutTrack; 
  
};
#endif // MATCHPATVELOSPACEL0_H
