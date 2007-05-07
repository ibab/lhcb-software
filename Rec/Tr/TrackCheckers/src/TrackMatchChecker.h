// $Id: TrackMatchChecker.h,v 1.3 2007-05-07 09:38:40 cattanem Exp $
#ifndef TRACKMATCHING_TRACKMATCHCHECKER_H 
#define TRACKMATCHING_TRACKMATCHCHECKER_H 1

// from GaudiAlg
#include "GaudiAlg/GaudiTupleAlg.h"

// Event
#include "Event/Track.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackChi2Calculator.h"
#include "TrackInterfaces/IAddTTClusterTool.h"

// from TrackMCInterfaces
#include "TrackMCInterfaces/IIdealStateCreator.h"

/** @class TrackMatchChecker TrackMatchChecker.h
 *
 *  A TrackMatchChecker is a top level Algorithm that checks the TrackMatches 
 *  which are stored in the EvDS. The TrackMatches are compared with the
 *  cheated matching. The results are stored in an ntuple.
 *
 *  @author:   Jeroen van Tilburg Jeroen.van.Tilburg@cern.ch
 *  @date:     21-06-2002 modified: 15-05-2006
 */

class TrackMatchChecker: public GaudiTupleAlg {
public:

  /// Constructor
  TrackMatchChecker(const std::string& name, ISvcLocator* pSvcLocator); 

  /// destructor
  virtual ~TrackMatchChecker();  

  /// intialize
  virtual StatusCode initialize();

  /// execute
  virtual StatusCode execute();

  /// finalize
  virtual StatusCode finalize();

private:

  /// Write NTuples
  StatusCode writeNTuples();

  /// match velo tracks with tracker tracks using MC-information
  StatusCode matchMCTracks();

  /// monitor the matching efficiency and ghost rate
  StatusCode monitor();

  /// Extrapolate a TrTrack to a z-position starting 
  /// with the closest TrState.
  StatusCode extrapolate( const LHCb::Track* track,
                          ITrackExtrapolator* extrapolator,
                          double zpos,
                          Gaudi::TrackVector& trackVector,
                          Gaudi::TrackSymMatrix& trackCov );

  double determineZ( const LHCb::Track* track );
  
  /// The velo track extrapolator
  ITrackExtrapolator* m_extrapolatorVelo;

  /// The seed track extrapolator
  ITrackExtrapolator* m_extrapolatorSeed;

  /// The chi2 calculator tool
  ITrackChi2Calculator* m_chi2Calculator;

  /// Pointer to ideal State creator
  IIdealStateCreator* m_stateCreator;

  // The tool to add TT clusters
  IAddTTClusterTool* m_addTTClusterTool;

  // Global counters for efficiency and ghost rate
  int m_countTotal;    ///< total number of found matches
  int m_countMCTotal;  ///< total number of possible matches
  int m_countGood;     ///< number of correctly found matches
  int m_countTTGood;   ///< number of correctly added TT hits
  int m_countTTBad;    ///< number of wrong TT hits
  

  /// Matching result on ntpl
  int m_numMatches;
  int m_numMCMatches;
  int m_numGood;
  int m_numVeloClones;
  int m_numSeedClones;
  int m_numVeloGhost;
  int m_numSeedGhost;
  int m_numVeloTracks;
  int m_numSeedTracks;

  std::vector<bool> m_goodMatch;
  std::vector<float> m_chi2Match;
  std::vector<bool> m_electron;
  std::vector<float> m_matchZPos;
  std::vector<long> m_numOfHitsVelo;
  std::vector<long> m_numOfHitsSeed;
  std::vector<float> m_zPosLastVeloHit;
  std::vector<float> m_trueMomentum;
  std::vector<bool> m_veloMCMatch;
  std::vector<bool> m_seedMCMatch;
  std::vector<long> m_nTT;
  std::vector<float> m_purityTT;

  std::vector<float> m_xVelo;
  std::vector<float> m_yVelo;
  std::vector<float> m_txVelo;
  std::vector<float> m_tyVelo;
  std::vector<float> m_momentumVelo;
  std::vector<float> m_eXVelo;
  std::vector<float> m_eYVelo;
  std::vector<float> m_eTxVelo;
  std::vector<float> m_eTyVelo;
  std::vector<float> m_eMomentumVelo;

  std::vector<float> m_xSeed;
  std::vector<float> m_xRich2;
  std::vector<float> m_ySeed;
  std::vector<float> m_yRich2;
  std::vector<float> m_txSeed;
  std::vector<float> m_txRich2;
  std::vector<float> m_tySeed;
  std::vector<float> m_tyRich2;
  std::vector<float> m_momentumSeed;
  std::vector<float> m_eXSeed;
  std::vector<float> m_eYSeed;
  std::vector<float> m_eTxSeed;
  std::vector<float> m_eTySeed;
  std::vector<float> m_eMomentumSeed;

  std::vector<float> m_xTrue;
  std::vector<float> m_yTrue;
  std::vector<float> m_txTrue;
  std::vector<float> m_tyTrue;
  std::vector<float> m_pTrue;

  /// MC matching results
  std::vector<bool> m_mcElectron;
  std::vector<float> m_mcChi2;
  std::vector<float> m_mcTrueMomentum;
  std::vector<long> m_mcnTT;

  // job options:  
  double m_matchAtZPosition;            ///< z-position to match the 2 tracks
  bool m_variableZ;                     ///< Allow z to vary with tx
  std::vector<double> m_varZParameters; ///< Parameters of the Z dependance
  std::string m_extrapolatorVeloName;   ///< The velo extrapolator name
  std::string m_extrapolatorSeedName;   ///< The seed extrapolators name
  std::string m_veloAssociator;         ///< The velo associator name
  std::string m_seedAssociator;         ///< The seed associator name
  std::string m_veloLinker;             ///< The velo linker name
  std::string m_seedLinker;             ///< The seed linker name
  std::string m_veloTracks;             ///< The velo location path
  std::string m_seedTracks;             ///< The seed location path
  std::string m_matchTracks;            ///< The match location path
  std::string m_ttClusterToolName;      ///< The tool name to add TT clusters

};

#endif // TRACKMATCHING_TRACKMATCHCHECKER_H
