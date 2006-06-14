// $Id: TrackMatchVeloSeed.h,v 1.11 2006-06-14 19:45:54 jvantilb Exp $
#ifndef TRACKMATCHVELOSEED_H 
#define TRACKMATCHVELOSEED_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

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

/** @class TrackMatchVeloSeed TrackMatchVeloSeed.h  
 *
 *  A TrackMatchVeloSeed is a Gaudi top level Algorithm that matches
 *  the tracks from the seeding with the ones from the Velo.
 *  The tracks are matched at a given z-position according to the lowest
 *  chi2-distance. Matched tracks with a chi2 higher than a certain maximum
 *  value are rejected.
 *
 *  @author:   Jeroen van Tilburg Jeroen.van.Tilburg@cern.nl
 *  @date:     16-05-2001
 *  @modified: 14-01-2006
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
  /// Match velo tracks with seed tracks
  StatusCode matchTracks( LHCb::Tracks* veloTracks,
                          LHCb::Tracks* seedTracks,
                          LHCb::Tracks* matchCont );

  /// Add TT clusters to matched tracks
  StatusCode addTTClusters( LHCb::Track* track );

  /// Store the new tracks made from the seed- and velo track segments
  StatusCode storeTracks( LHCb::Tracks* matchCont );

  /// Extrapolate a Track to a z-position starting 
  /// with the closest State
  StatusCode extrapolate( LHCb::Track* track,
                          ITrackExtrapolator* extrapolator,
                          double zpos,
                          Gaudi::TrackVector& trackVector,
                          Gaudi::TrackSymMatrix& trackCov );

  /// Calculate the new z
  double determineZ( const LHCb::Track* track );

private:
  ///Definition of vector of TrackMatch pointers
  typedef std::vector<TrackMatch*> TrackMatchVector;

  // Job options
  // -----------
  /// Input Velo tracks
  std::string m_veloTracks;
  /// Input Seed tracks
  std::string m_seedTracks;
  /// flag to add the XxxMeasurements to the track
  bool m_addMeasurements;
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
  /// Determine whether to add TT clusters or not
  bool m_addTTClusters;

  /// The extrapolators
  ITrackExtrapolator* m_extrapolatorVelo;
  ITrackExtrapolator* m_extrapolatorSeed;

  /// The chi2 calculator tool
  ITrackChi2Calculator* m_chi2Calculator;

  /// The measurement provider tool
  IMeasurementProvider* m_measProvider;

  // The tool to add TT clusters
  IAddTTClusterTool* m_addTTClusterTool;

};
#endif // TRACKMATCHVELOSEED_H
