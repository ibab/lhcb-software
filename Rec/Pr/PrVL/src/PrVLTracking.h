#ifndef PRVL_TRACKING_H 
#define PRVL_TRACKING_H 1

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiTupleAlg.h"
#include "PrKernel/IPrDebugTool.h"
#include "GaudiAlg/ISequencerTimerTool.h"
// Det/VLDet
#include "VLDet/DeVL.h"
// Local
#include "PrVLHit.h"
#include "PrVLTrack.h"

/** @class PrVLTracking PrVLTracking.h
 *  VeloLite pattern recognition (based on FastVelo)
 *
 */

//class PrVLTracking : public GaudiAlgorithm {
class PrVLTracking : public GaudiTupleAlg {
public: 
  /// Constructor
  PrVLTracking(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~PrVLTracking() {}

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution

private:

  void findQuadruplets(DeVLRSensor* sensor0, const bool forward);
  void findTriplets(DeVLRSensor* sensor0, const bool forward);
  void extendTrack(PrVLTrack& track, unsigned int sensor,
                   const unsigned int zone, const bool forward);
  void extendTrackOtherSide(PrVLTrack& track, unsigned int sensor,
                            unsigned int zone, const bool forward,
                            const bool next);
  void cleanupRZ();
  void findPhiHits(PrVLTrack& seed, const bool unused);
  void findPhiHitsOverlap(PrVLTrack& seed, const bool unused);
  int findSpaceTracksPhi(PrVLTrack& seed);
  bool findSpaceTracksXY(PrVLTrack& seed);
  void makeLHCbTracks();
  void mergeClones(PrVLTracks& tracks);
  void mergeSpaceClones(const bool forward);
  void findUnusedR(DeVLRSensor* sensor0, const bool forward);
  void findUnusedPhi();
  void buildHits();
  PrVLHit* closestRHit(PrVLHits& hits, const double r, const double tol, 
                       const bool unused); 
  PrVLHit* closestPhiHit(PrVLHits& hits, const double phi, const double tol, 
                         const bool unused); 
  PrVLHit* closestPhiHitOtherSide(PrVLHits& hits, const double phi, 
                                  const double tol, const bool unused,
                                  const unsigned int sensor); 
  PrVLHit* closestPhiHit(PrVLHits& hits, const double x, const double y, 
                         const double tol, const bool unused); 
  bool addPhiHit(PrVLHits& hits, PrVLTrack& track, const double maxChi2);
  bool fitProtoTrack(PrVLHits& hits, const unsigned int nMin,
                     const unsigned int nR, double& q,
                     double& x0, double& y0, double& tx, double& ty);

  bool matchKey(const PrVLHit* hit) {
    bool ok = false;
    if (m_debugTool) {
      LHCb::LHCbID id = hit->lhcbID();
      ok = m_debugTool->matchKey(id, m_wantedKey);
    }
    return ok;
  }
  void printHit(const PrVLHit* hit, const std::string title);
  void printRZTrack(PrVLTrack& track);
  void printTrack(PrVLTrack& track);

  std::string m_trackLocation;

  bool m_forward;
  bool m_backward;
 
  /// Minimal Z of a vertex for forward tracks
  double m_zVertexMin;
  /// Maximal Z of a vertex for backward tracks
  double m_zVertexMax; 
  
  /// Parameters for RZ tracking
  double m_maxRSlope4;
  double m_maxRSlope3;       
  double m_rTol4;      
  double m_rTol3;
  double m_rTolExtra; 
  double m_rTolOverlap; 
  int m_maxMissed;       
  unsigned int m_minToTag;  

  /// Parameters for space tracking
  double m_xTolOverlap;
  double m_phiTol;
  double m_xyTol;
  double m_fractionPhi;
  double m_fractionShared;
  double m_fractionUsed; 
  double m_maxChi2PerHit;
  double m_maxChi2ToAdd;
  double m_maxQXy;
  double m_maxQXyFull;
  //  double m_maxQPhi;
  double m_maxQRPhi;

  /// Parameters for clone merging
  double m_fractionForMerge;
  double m_maxDistToMerge;
  double m_maxDeltaSlopeToMerge;

  /// Parameters for recovery
  unsigned int m_maxRZForExtra;
  double m_phiUnusedFirstTol;
  double m_phiUnusedSecondTol;

  /// Pointer to detector element
  DeVL* m_det;
  /// Number of stations
  int m_nStations;
  /// List of possible Phi hits matching an RZ seed track
  std::vector<PrVLHits> m_phiHits;
  /// Number of zones in an R sensor
  unsigned int m_nZonesR;
  /// Number of zones in a Phi sensor
  unsigned int m_nZonesPhi;
  /// Bisectors and phi ranges of R zones
  std::vector<double> m_ctrPhi;
  std::vector<double> m_tolPhi;
  /// Hits of an event grouped by sensor and zone
  std::vector<std::vector<PrVLHits> > m_hits;
  /// Vector of hit objects (to avoid newing)
  std::vector<PrVLHit> m_pool;
  std::vector<PrVLHit>::iterator m_nextInPool;  

  bool m_debug;
  std::string m_debugToolName;
  IPrDebugTool* m_debugTool;
  int m_wantedKey;

  bool m_timing;
  ISequencerTimerTool* m_timerTool;
  int m_timeTotal;
  int m_timePrepare;
  int m_timeFwd4;
  int m_timeBwd4;
  int m_timeFwd3;
  int m_timeBwd3;
  int m_timeSpace;
  int m_timeUnused;
  int m_timeFinal;

  PrVLTracks m_rzTracks; 
  PrVLTracks m_spaceTracks;  

};

#endif
