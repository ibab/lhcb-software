#ifndef TRACKMATCHVELOSEED_H 
#define TRACKMATCHVELOSEED_H 1


#include <map>

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiAlg/ISequencerTimerTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackChi2Calculator.h"
#include "TrackInterfaces/IMeasurementProvider.h"
#include "TrackInterfaces/IAddTTClusterTool.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/Measurement.h"

// local
#include "TrackMatch.h"
#include "VeloCandidate.h"

/** @class TrackMatchVeloSeed TrackMatchVeloSeed.h  
 *
 *  A TrackMatchVeloSeed is a Gaudi top level Algorithm that matches
 *  the tracks from the seeding with the ones from the Velo.
 *  The tracks are matched at a given z-position according to the lowest
 *  chi2-distance. Matched tracks with a chi2 higher than a certain maximum
 *  value are rejected.
 *
 *  @author:   Jeroen van Tilburg Jeroen.van.Tilburg@cern.nl
 *  @date:     16-05-2001 modified: 14-01-2006
 */

class DeVelo;
class IVeloExpectation;

class TrackMatchVeloSeed : public GaudiHistoAlg {
public: 
  /// Standard constructor
  TrackMatchVeloSeed( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackMatchVeloSeed( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  /// Match velo tracks with seed tracks
  StatusCode matchTracks( VeloCandidates& veloTracks,
                          LHCb::Tracks* seedTracks,
                          LHCb::Tracks* matchCont );

  /// Add TT clusters to matched tracks
  StatusCode addTTClusters( LHCb::Track* track );

  /// Store the new tracks made from the seed- and velo track segments
  StatusCode storeTracks( LHCb::Tracks* matchCont );

  /// Calculate the new z
  double determineZ( const LHCb::Track* track );

  void createVeloCandidates(LHCb::Tracks* tracks, VeloCandidates& candidates);
  
  bool goodSeed(const LHCb::Track* aTrack) const;

  bool trackToTake(const TrackMatch* aMatch) const; 
  bool usedT(const LHCb::Track* aTrack) const;
  bool usedVelo(const LHCb::Track* aTrack) const;

  StatusCode extrapolateSeed(LHCb::State& aState, const double zMatch) const;

  ///Definition of vector of TrackMatch pointers
  typedef std::vector<TrackMatch*> TrackMatchVector;

  // Job options
  // -----------
  /// Input Velo tracks
  std::string m_veloTracks;
  /// Input Seed tracks
  std::string m_seedTracks;
  /// Chi2 cut to decide whether to match 2 tracks
  double m_chi2MatchingCut;
  /// Store all combinations smaller than chi2-cut or only the best ones
  bool m_allCombinations;
  /// Momentum cut from pT-kick (0 = no cut)
  double m_momentumCut;
  /// pt cut
  double m_pt2Cut;  
  /// Lik cut
  double m_likCut;

  /// Cuts on sigma's track parameters to cut away badly reconstructed tracks
  double m_veloXCut2;
  double m_veloYCut2;
  double m_veloTxCut2;
  double m_veloTyCut2;
  double m_seedXCut2;
  double m_seedYCut2;
  double m_seedTxCut2;
  double m_seedTyCut2;
  /// Z-position to match the 2 tracks
  double m_matchAtZPosition;
  /// Allow Z to depend on the value of the pT-kick
  bool m_variableZ;
  /// Define the parameters of the Z dependance
  std::vector<double> m_varZParameters;
  
  /// The extrapolators name
  std::string m_extrapolatorVeloName;
  std::string m_extrapolatorSeedName;
  /// Determine whether to add TT clusters or not
  bool m_addTTClusters;

  double resT(const double p, const std::vector<double>& params) const ;

  /// The extrapolators
  ITrackExtrapolator* m_extrapolatorVelo;
  ITrackExtrapolator* m_extrapolatorSeed;

  /// The chi2 calculator tool
  ITrackChi2Calculator* m_chi2Calculator;

  /// The measurement provider tool
  IMeasurementProvider* m_measProvider;

  // The tool to add TT clusters
  IAddTTClusterTool* m_addTTClusterTool;

  double m_chi2SeedCut;
  std::string m_ttClusterToolName;
  std::string m_outputLocation;

  typedef std::map<const LHCb::Track*,unsigned int> UsedMap;
  mutable UsedMap m_usedT;
  mutable UsedMap m_usedVelo;

  unsigned int m_nUseT;
  unsigned int m_nUseVelo;

  double m_refT;
  double m_maxStepSize;
  double m_chi2TCut;

  std::vector<double> m_yTParams;
  std::vector<double> m_tyTParams; 

  bool m_omitSeedFitOutliers; /// Omit outliers of SeedFit when creating new track

  bool m_writeNNVariables; /*switch on or off NN var. writing*/

  bool m_discardUsedVelo;
  double m_discardChi2;

  unsigned int m_maxNSeed;
  unsigned int m_maxNVelo;

   ISequencerTimerTool* m_timerTool;
   int  m_matchTime;
   bool m_doTiming;

};

inline bool TrackMatchVeloSeed::usedT(const LHCb::Track* aTrack) const{
  UsedMap::iterator iter = m_usedT.find(aTrack);
  return( iter != m_usedT.end() ? iter->second > m_nUseT : false); 
}

inline bool TrackMatchVeloSeed::usedVelo(const LHCb::Track* aTrack) const{
  UsedMap::iterator iter = m_usedVelo.find(aTrack);
  return( iter != m_usedVelo.end() ? iter->second > m_nUseVelo : false); 
}

inline double TrackMatchVeloSeed::resT(const double p, const std::vector<double>& params) const{
  double t1 = params[0] ;
  if (params.size() > 1u) t1+= params[1]/gsl_pow_2(p);
  return 1.0/t1;
}


#endif // TRACKMATCHVELOSEED_H
