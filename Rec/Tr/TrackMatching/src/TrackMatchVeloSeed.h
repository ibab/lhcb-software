// $Id: TrackMatchVeloSeed.h,v 1.8 2006-05-09 08:48:12 erodrigu Exp $
#ifndef TRACKMATCHVELOSEED_H 
#define TRACKMATCHVELOSEED_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"

// from LHCbDefinitions
#include "Kernel/TrackTypes.h"

// from LHCbKernel
#include "Kernel/ISTClusterPosition.h"

// from STDet
#include "STDet/DeTTDetector.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackChi2Calculator.h"
#include "TrackInterfaces/IMeasurementProvider.h"
#include "TrackInterfaces/ITrajPoca.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/Measurement.h"

// local
#include "TrackMatch.h"

using namespace Gaudi;
using namespace LHCb;

/** @class TrackMatchVeloSeed TrackMatchVeloSeed.h  
 *
 *  A TrackMatchVeloSeed is a Gaudi top level Algorithm that matches
 *  the tracks from the seeding with the ones from the Velo.
 *  The tracks are matched at a given z-position according to the lowest
 *  chi2-distance. Matched tracks with a chi2 higher than a certain maximum
 *  value are rejected.
 *
 *  @author:  Jeroen van Tilburg Jeroen.van.Tilburg@cern.nl
 *  @date:    16-05-2001
 *  @modified:14-01-2006
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
  StatusCode matchTracks( Tracks* veloTracks,
                          Tracks* seedTracks,
                          TrackMatches*& matchCont );

  /// Add TT clusters to matched tracks
  StatusCode addTTClusters( TrackMatches*& matchCont );

  /// Store the new tracks made from the seed- and velo track segments
  StatusCode storeTracks( TrackMatches*& matchCont );

  /// Extrapolate a Track to a z-position starting 
  /// with the closest State
  StatusCode extrapolate( Track* track,
                          ITrackExtrapolator* extrapolator,
                          double zpos,
                          TrackVector& trackVector,
                          TrackSymMatrix& trackCov );

  /// Calculate the new z
  StatusCode determineZ( double tX,
                         double& zNew );

private:
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

  // TT geometry
  DeTTDetector* m_ttTracker;             ///< Pointer to the TT XML geom
  std::string   m_ttTrackerPath;         ///< Name of the TT XML geom path
  ISTClusterPosition* m_ttPositionTool;  ///< STClusterPosition tool for TT
  std::string m_ttPositionToolName;  ///< ST cluster position tool name for TT
  IMagneticFieldSvc* m_pIMF;             ///< Pointer to the magn. field service
  ITrajPoca*         m_poca;             ///< Pointer to the ITrajPoca interface
};
#endif // TRACKMATCHVELOSEED_H
