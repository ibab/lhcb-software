#ifndef TRACKIDEALPR_IDEALTRACKSCREATOR_H
#define TRACKIDEALPR_IDEALTRACKSCREATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from LHCbKernel
#include "Kernel/ISTClusterPosition.h"

// from Event
#include "Event/MCParticle.h"
#include "Event/OTTime.h"
#include "Event/STCluster.h"
#include "Event/VeloCluster.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/TrackFunctor.h"
#include "Event/Measurement.h"

// from MCInterfaces
#include "MCInterfaces/IIdealStateCreator.h"

// Forward declarations
class DeVelo;
class DeOTDetector;
class DeSTDetector;
class IMagneticFieldSvc;
class ITrajPoca;
class IMCReconstructible;

/** @class IdealTracksCreator IdealTracksCreator.h
 *
 *  A IdealTracksCreator is an Algorithm that creates tracks from MCParticles.
 *  Clusters which are associated to a single MCParticle are added to the 
 *  'true' track. The relation table from Tracks to MCParticles is 
 *  automatically filled.
 *  Currently the following types of measurements can be added:
 *   @li Outer Tracker OTTimes (OTMeasurement)
 *   @li Silicon Tracker clusters in TT and IT (STMeasurement)
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

private:
  /// Add outer tracker clusters
  StatusCode addOTTimes( const LHCb::MCParticle* mcPart, LHCb::Track* track ) const ;

  /// Add silicon tracker clusters in TT
  StatusCode addTTClusters( const LHCb::MCParticle* mcPart, LHCb::Track* track ) const ;

  /// Add silicon tracker clusters in IT
  StatusCode addITClusters( const LHCb::MCParticle* mcPart, LHCb::Track* track ) const ;

  /// Add velo r and phi clusters
  StatusCode addVeloClusters( const LHCb::MCParticle* mcPart, LHCb::Track* track ) const ;

  /// Initialize seed state
  StatusCode initializeState( const LHCb::MCParticle* mcPart, LHCb::Track* track ) const ;

  // bool hasT(const LHCb::Track* aTrack) const;

  //  bool hasVelo(const LHCb::Track* aTrack) const;

  StatusCode initEvent() const;

  bool wanted(const LHCb::Track::Types& aType) const;

  void printMCParticle(const LHCb::MCParticle* mcParticle) const;

  void printTrack(const LHCb::Track* aTrack) const;

  // Geometry information
  DeVelo*       m_velo;            ///< Pointer to VELO detector element
  DeSTDetector* m_ttTracker;       ///< Pointer to TT detector element
  DeSTDetector* m_itTracker;       ///< Pointer to IT detector element
  DeOTDetector* m_otTracker;       ///< Pointer to OT detector element
  std::string   m_veloPath;        ///< Name of the Velo XML geom path
  std::string   m_ttTrackerPath;   ///< Name of the TT XML geom path
  std::string   m_itTrackerPath;   ///< Name of the IT XML geom path
  std::string   m_otTrackerPath;   ///< Name of the OT XML geom path

  // Interfaces
  ISTClusterPosition*     m_ttPositionTool;///< ST cluster position tool for TT
  ISTClusterPosition*     m_itPositionTool;///< ST cluster position tool for IT
  IVeloClusterPosition* m_veloPositionTool;///< Velo cluster position tool
  IMCReconstructible* m_trackSelector; ///< Track selection tool
  IIdealStateCreator*     m_stateCreator;  ///< Create 'seed'state at last meas
  IMagneticFieldSvc* m_pIMF;          ///< Pointer to the magn. field service
  ITrajPoca*         m_poca;          ///< Pointer to the ITrajPoca interface

  // job options
  bool m_addOTTimes;      ///< true if OT clusters should be put on track
  bool m_addTTClusters;   ///< true if ST clusters in TT should be put on track
  bool m_addITClusters;   ///< true if ST clusters in IT should be put on track
  bool m_addVeloClusters; ///< true if Velo R clusters should be put on track
  bool m_addMeasurements; ///< flag to add the XxxMeasurements to the track
  bool m_initState;       ///< initialize seed state
  bool m_initStateUpstreamFit; ///< seed state created upstream/downstream
  bool m_trueStatesAtMeas;  ///< Store true states at each measurement position
  std::string m_ttPositionToolName;
  std::string m_itPositionToolName;
  std::string m_veloPositionToolName; ///< Velo cluster position tool name
  std::string m_selectorToolName;     ///< Track selector tool name
  std::string m_tracksOutContainer;///< Tracks output container path in the TES
  int    m_minNHits;      ///< Minimum number of hits on the track
  std::vector<int> m_tracktypes;    ///< Track types of the monitored tracks
  
  typedef LinkedFrom<LHCb::STCluster,LHCb::MCParticle> STLinker;
  typedef LinkedFrom<LHCb::OTTime,LHCb::MCParticle> OTLinker;
  typedef LinkedFrom<LHCb::VeloCluster,LHCb::MCParticle> VeloLinker;

  mutable STLinker m_itLinker;
  mutable STLinker m_ttLinker;
  mutable OTLinker m_otLinker;
  mutable VeloLinker m_veloLinker;

};

#endif // TRACKIDEALPR_IDEALTRACKSCREATOR_H
