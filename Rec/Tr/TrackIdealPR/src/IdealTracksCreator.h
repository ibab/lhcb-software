#ifndef TRACKIDEALPR_IDEALTRACKSCREATOR_H
#define TRACKIDEALPR_IDEALTRACKSCREATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from TrackEvent
#include "Event/Track.h"

// from STTools
#include "STTools/ISTClusterPosition.h"

// from XxxAssociators
#include "OTAssociators/OTTime2MCHitAsct.h"
#include "ITAssociators/ITCluster2MCParticleAsct.h"
#include "VeloAssociators/VeloCluster2MCParticleAsct.h"

// from TrackInterfaces
#include "TrackInterfaces/IIdealStateCreator.h"
#include "TrackInterfaces/ITrackCriteriaSelector.h"

// Forward declarations
class DeOTDetector;
class DeSTDetector;
class DeVelo;

/** @class IdealTracksCreator IdealTracksCreator.h
 *
 *  A IdealTracksCreator is an Algorithm that creates tracks from MCParticles.
 *  Clusters which are associated to a single MCParticle are added to the 
 *  'true' track. The relation table from Tracks to MCParticles is 
 *  automatically filled.
 *  Currently the following types of measurements can be added:
 *   @li Outer Tracker OTTimes (OTMeasurement)
 *   @li Inner Tracker clusters (ITMeasurement)
 *   @li Velo r clusters (VeloRMeasurement)
 *   @li Velo phi clusters (VeloPhiMeasurement)
 *  For the OTTimes the relation table with MCHits is needed, since the
 *  left/right ambiguity is needed. For the other measurement, the associtors
 *  with MCParticles are used.
 *
 *  With the track selection tool (@em select), one can put selection
 *  criteria for the MCParticles (e.g. select only tracks reconstrucible as 
 *  long tracks).
 *
 *  The true-state creator tool creates states from MCParticles and their 
 *  MCHits.
 *
 *  @author Eduardo Rodrigues (adaptations to new track event model, 18-04-2005)
 *  @author M. Needham
 *  @author R.M. van der Eijk (28-5-2002) 
 *  @author J. van Tilburg (02-07-2003)
 *  @date   27-07-1999
 */

class IdealTracksCreator: public GaudiAlgorithm {

public:

  /// Standard constructor
  IdealTracksCreator( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~IdealTracksCreator( );      ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  void sortMeasurements( Track* track ) {
    std::sort( track->measurements().begin(),
               track->measurements().end(),
               TrackFunctor::increasingByZ<Measurement>() );
  };

private:
  /// Add outer tracker clusters
  StatusCode addOTTimes( OTTimes* times,
                         MCParticle* mcPart,
                         Track* track );

  /// Add inner tracker clusters
  StatusCode addITClusters( MCParticle* mcPart, Track* track );

  /// Add velo r and phi clusters
  StatusCode addVeloClusters( MCParticle* mcPart, Track* track );

  /// Initialize seed state
  StatusCode initializeState( double z, Track* track, MCParticle* mcPart );

  /// Delete all states on the track
  StatusCode deleteStates( Track* track );

  // MC associators
  OTTime2MCHitAsct::IAsct*           m_otTim2MCHit;
  ITCluster2MCParticleAsct::IAsct*   m_itClus2MCP;
  VeloCluster2MCParticleAsct::IAsct* m_veloClus2MCP;

  // Geometry information
  DeOTDetector* m_otTracker;       ///< Pointer to OT detector element
  DeSTDetector* m_itTracker;       ///< Pointer to ST detector element
  DeVelo*       m_velo;            ///< Pointer to VELO detector element
  std::string   m_otTrackerPath;   ///< Name of the OT XML geom path
  std::string   m_itTrackerPath;   ///< Name of the IT XML geom path
  std::string   m_veloPath;        ///< Name of the Velo XML geom path

  // Interfaces
  ISTClusterPosition*     m_stPositionTool;     ///< ST cluster position tool
  ITrackCriteriaSelector* m_trackSelector;      ///< Track selection tool
  IIdealStateCreator*     m_stateCreator;       ///< Create 'seed' state at last measurement

  // job options
  bool m_addOTTimes;               ///< true if OT clusters should be put on track
  bool m_addITClusters;            ///< true if IT clusters should be put on track
  bool m_addVeloClusters;          ///< true if Velo R clusters should be put on track
  bool m_initState;                ///< initialize seed state
  bool m_initStateUpstream;        ///< seed state created upstream/downstream
  bool m_trueStatesAtMeas;         ///< Store true states at each measurement position
  std::string m_tracksTESPath;     ///< TrTracks container path in EvDS
  std::string m_relationTablePath; ///< Location of the associator table
  int m_minNHits;         ///< Minimum number of hits on the track
  double m_errorX2;       ///< Error^2 on x
  double m_errorY2;       ///< Error^2 on y
  double m_errorTx2;      ///< Error^2 on slope x
  double m_errorTy2;      ///< Error^2 on slope y
  double m_errorP;        ///< dp/p
};

#endif // TRACKIDEALPR_IDEALTRACKSCREATOR_H
