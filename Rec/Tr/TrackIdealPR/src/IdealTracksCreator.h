#ifndef TRACKIDEALPR_IDEALTRACKSCREATOR_H
#define TRACKIDEALPR_IDEALTRACKSCREATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"

// from Event
#include "Event/MCParticle.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/TrackFunctor.h"
#include "Event/Measurement.h"

// from MCInterfaces
#include "MCInterfaces/IIdealStateCreator.h"

#include "Event/OTTime.h"
#include "Event/STCluster.h"
#include "Event/VeloCluster.h"
#include "Event/MuonCoord.h"
#include "Linker/LinkedFromKey.h"

// Forward declarations
class ITrajPoca;
class IMCReconstructible;
class IMeasurementProvider;

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

  /// Add muons
  StatusCode addMuonHits( const LHCb::MCParticle* mcPart, LHCb::Track* track ) const ;

  /// Initialize seed state
  StatusCode initializeState( const LHCb::MCParticle* mcPart, LHCb::Track* track ) const ;


  StatusCode initEvent() const;

  bool wanted(const LHCb::Track::Types& aType) const;

  void printMCParticle(const LHCb::MCParticle* mcParticle) const;

  void printTrack(const LHCb::Track* aTrack) const;

  void smearQOverP(LHCb::State& aState) const;

  IMCReconstructible* m_trackSelector; ///< Track selection tool
  IIdealStateCreator*     m_stateCreator;  ///< Create 'seed'state at last meas

  // job options
  bool m_addOTTimes;      ///< true if OT clusters should be put on track
  bool m_addTTClusters;   ///< true if ST clusters in TT should be put on track
  bool m_addITClusters;   ///< true if ST clusters in IT should be put on track
  bool m_addVeloClusters; ///< true if Velo R clusters should be put on track
  bool m_addMuonHits;
  bool m_initState;       ///< initialize seed state
  bool m_initStateUpstreamFit; ///< seed state created upstream/downstream
  bool m_trueStatesAtMCHits;  ///< Store true states at each MCHit position
 
  std::string m_selectorToolName;     ///< Track selector tool name
  std::string m_tracksOutContainer;///< Tracks output container path in the TES
  int    m_minNHits;      ///< Minimum number of hits on the track
  std::vector<int> m_tracktypes;    ///< Track types of the monitored tracks
  
  typedef LinkedFrom<LHCb::STCluster,LHCb::MCParticle> STLinker;
  typedef LinkedFromKey<LHCb::MCParticle, LHCb::OTChannelID> OTLinker ;
  typedef LinkedFrom<LHCb::VeloCluster,LHCb::MCParticle> VeloLinker;
  typedef LinkedFrom<LHCb::MuonCoord,LHCb::MCParticle> MuonLinker;

  mutable STLinker m_itLinker;
  mutable STLinker m_ttLinker;
  mutable OTLinker m_otLinker;
  mutable VeloLinker m_veloLinker;
  mutable MuonLinker m_muonLinker;

  // smart interface to generators
  SmartIF<IRndmGen> m_uniformDist;
  SmartIF<IRndmGen> m_gaussDist;

  double m_otEff;
  double m_itEff;
  double m_ttEff;
  double m_veloEff;
  double m_smearP;

};

#endif // TRACKIDEALPR_IDEALTRACKSCREATOR_H
