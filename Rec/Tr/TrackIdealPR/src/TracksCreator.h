// $Id: TracksCreator.h,v 1.1.1.1 2005-03-03 14:36:10 erodrigu Exp $
#ifndef TRACKIDEALPR_TRACKSCREATOR_H
#define TRACKIDEALPR_TRACKSCREATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IHistogramSvc.h"

// from Event
#include "Event/TrFitTrack.h"
#include "Event/OTTime.h"
#include "Event/ITCluster.h"
#include "Event/VeloCluster.h"

// from XxxAssociators
#include "VeloAssociators/VeloCluster2MCParticleAsct.h"
#include "ITAssociators/ITCluster2MCParticleAsct.h"
#include "OTAssociators/OTTime2MCDepositAsct.h"

// Reconstructible tool
#include "TrAssociators/ITrReconstructible.h"

// True Track States tool
#include "TrCheck/ITrStateCreator.h"

// forward declarations
class MCParticle;

class IHistogram1D;

class DeOTDetector;
class DeSTDetector;
class DeVelo;

/** @class TracksCreator TracksCreator.h TracksCreator.h
 *  
 *
 *  @author Eduardo Rodrigues
 *  @date   2005-02-22
 *
 *  History:
 *   Author: M. Needham
 *   Created: 27-7-1999
 *   Modified: 28-6-2000: R.M. van der Eijk
 *   Modified: 28-5-2002: R.M. van der Eijk (adapt to xml-event model)
 *
 */
class TracksCreator : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TracksCreator( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TracksCreator( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  /// register tracks in Transient data store
  StatusCode registerTracks( TrFitTracks* tracksCont );

  /// add outer tracker times
  StatusCode addOTTimes( OTTimes* times, MCParticle* mcPart,
                         TrFitTrack* track );

  /// add inner tracker clusters
  StatusCode addITClusters( ITClusters* clusters, MCParticle* mcPart,
                            TrFitTrack* track );

  /// add velo clusters
  StatusCode addVeloClusters( VeloClusters* clusters, MCParticle* mcPart,
                              TrFitTrack* track );

  StatusCode initializeState( double z, TrFitTrack* track, MCParticle* mcPart );

  // Job options properties
  bool m_addOTTimes;       ///< true if OT times should be put on tracks
  bool m_addITClusters;    ///< true if IT clusters should be put on tracks
  bool m_addVeloClusters;  ///< true if Velo R clusters should be put on tracks
  bool m_produceHistogram; ///< true to produce monitoring histograms
  int m_minNHits;          ///< Minimum number of hits on the track

  /// Output tracks path in the transient event store
  std::string m_tracksTESPath;  ///< Tracks container path in TES

  // MC associators
  OTTime2MCDepositAsct::IAsct*       m_otTimeToMCDep;
  ITCluster2MCParticleAsct::IAsct*   m_itClusToMCP;
  VeloCluster2MCParticleAsct::IAsct* m_veloClusToMCP;

  // Geometry information
  DeOTDetector* m_otTracker;
  DeSTDetector* m_itTracker;
  DeVelo*       m_velo;
  std::string   m_otTrackerPath;   ///< Name of the OT XML geom path
  std::string   m_itTrackerPath;   ///< Name of the IT XML geom path
  std::string   m_veloPath;        ///< Name of the Velo XML geom path

  // Tools
  ITrReconstructible* m_trAcceptance;   ///< Acceptance judge
  ITrStateCreator*    m_stateCreator;   ///< True States creator

  // Histogram declarations
  StatusCode initHistograms();
  IHistogram1D* fNumTracksHisto;
  IHistogram1D* fNumHitsOnTrackHisto;

};
#endif // TRACKIDEALPR_TRACKSCREATOR_H 1
