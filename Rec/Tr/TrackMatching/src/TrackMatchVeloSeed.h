// $Id: TrackMatchVeloSeed.h,v 1.4 2006-01-27 12:57:20 erodrigu Exp $
#ifndef TRACKMATCHING_TRACKMATCHVELOSEED_H 
#define TRACKMATCHING_TRACKMATCHVELOSEED_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from STDet
#include "STDet/DeSTDetector.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackChi2Calculator.h"
#include "TrackInterfaces/IMeasurementProvider.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/Measurement.h"

// local
#include "TrackMatch.h"

/** @class TrackMatchVeloSeed TrackMatchVeloSeed.h TrackMatching/TrackMatchVeloSeed.h
 *  
 *
 *  A TrackMatchVeloSeed is a Gaudi top level Algorithm that matches
 *  the tracks from the seeding with the ones from the Velo.
 *  The tracks are matched at a given z-position according to the lowest
 *  chi2-distance. Matched tracks with a chi2 higher than a certain maximum
 *  value are rejected.
 *
 *  @author:  Jeroen van Tilburg jtilburg@nikhef.nl
 *  @date:    16-05-2001
 */
class TrackMatchVeloSeed : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TrackMatchVeloSeed( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackMatchVeloSeed( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  ///Defiintion of vector of TrackMatch pointers
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
  /// True momentum cut for specified efficiency calculations
  double m_trueMomentumCut;
  /// Cuts on sigma's track parameters to cut away badly reconstructed tracks
  double m_veloXCut;
  double m_veloYCut;
  double m_veloTxCut;
  double m_veloTyCut;
  double m_seedXCut;
  double m_seedYCut;
  double m_seedTxCut;
  double m_seedTyCut;
  /// Z-position to match the 2 tracks
  double m_matchAtZPosition;
  /// Allow Z to depend on the value of the pT-kick
  bool m_variableZ;
  /// Define the parameters of the Z dependance
  std::vector<double> m_varZParameters;
  /// The extrapolators name
  std::string m_extrapolatorVeloName;
  std::string m_extrapolatorSeedName;
  /// define the new covariance matrix of the matched tracks for the fit
  double m_errorX2;
  double m_errorY2;
  double m_errorTx2;
  double m_errorTy2;
  double m_errorP;
  /// Determine whether to add TT clusters or not
  bool m_addTTClusters;
  /// Chi2 cut to add the TT clusters (only the best in a wafer is taken)
  double m_ttClusterCut;
  /// Minimum number of TT clusters
  unsigned int m_minTTHits;
  /// Number of TT layers
  unsigned int m_numTTLayers;
  /// maximum distance difference between TT clusters of different stations
  double m_interStationCut;
  /// maximum distance difference between TT clusters of same station
  double m_intraStationCut;
  /** The quality is defined as: |distance| + m_spreadWeight*spread   
   *  The quality the the criterium to select the best TTCandidate (= set
   *  of TT clusters.)
   */
  double m_spreadWeight;

  /// The extrapolators
  ITrackExtrapolator* m_extrapolatorVelo;
  ITrackExtrapolator* m_extrapolatorSeed;

  /// The chi2 calculator tool
  ITrackChi2Calculator* m_chi2Calculator;

  /// The measurement provider tool
  IMeasurementProvider* m_measProvider;

  // ST geometry
  DeSTDetector* m_itTracker;
  std::string   m_itTrackerPath;   ///< Name of the IT XML geom path

  /// use a fixed particle ID for the extrapolator
  ParticleID  m_particleID;

  /// match velo tracks with seed tracks
  StatusCode matchTracks( Tracks* veloTracks,
                          Tracks* seedTracks,
                          TrackMatches*& matchCont );

  /// Add TT clusters to matched tracks
  StatusCode addTTClusters( TrackMatches*& matchCont );

  /// Store the new tracks made from the seed- and velo track segments
  StatusCode storeTracks( TrackMatches*& matchCont );

  /// Extrapolate a TrTrack to a z-position starting 
  /// with the closest TrState.
  StatusCode extrapolate( Track* track,
                          ITrackExtrapolator* extrapolator,
                          double zpos,
                          HepVector& trackVector,
                          HepSymMatrix& trackCov );

  /// Calculate the new z
  StatusCode determineZ( double tX,
                         double& zNew );

};
#endif // TRACKMATCHING_TRACKMATCHVELOSEED_H
