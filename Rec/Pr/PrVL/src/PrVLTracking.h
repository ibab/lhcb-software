#ifndef PRVL_TRACKING_H 
#define PRVL_TRACKING_H 1

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
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

class PrVLTracking : public GaudiAlgorithm {
public: 
  /// Constructor
  PrVLTracking(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~PrVLTracking() {}

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalization

private:

  void findQuadruplets(DeVLRSensor* sensor0, const bool forward);
  void findTriplets(DeVLRSensor* sensor0, const bool forward);
  int extendTrack(PrVLTrack& track, unsigned int sensor,
                  const unsigned int zone, const bool forward, 
                  const bool unused);
  void extendTrackOtherSide(PrVLTrack& track, unsigned int sensor,
                            unsigned int zone, const bool forward,
                            const bool unused);
  void findSpaceTracks(PrVLTrack& input);
  void makeLHCbTracks(LHCb::Tracks* tracks);
  void mergeClones(PrVLTracks& tracks);
  void mergeSpaceClones();
  void findUnusedPhi();
  void buildHits();
  PrVLHit* closestHit(PrVLHits& hits, const double r, const double tol, 
                      const bool unused); 
  bool addPhiHit(PrVLHits& hits, PrVLTrack& track);
  bool addOtherSidePhiHit(PrVLHits& hits, PrVLTrack& track, 
                          unsigned int sensor);
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
  /// Maximal Z of a vertex for defining last sensor in backward tracks
  double m_zVertexMax;      
  double m_zVertexMaxBack;  
  /// Maximum RZ slope considered
  double m_maxRSlope;       
  /// R match tolerance in a quadruplet, in units of strip pitch
  double m_rTol4;      
  /// R match tolerance in a triplet, in units of strip pitch
  double m_rTol3;      
  /// R match tolerance for extending tracks
  double m_rTolExtra; 
  /// R match tolerance for other side hits
  double m_rTolOverlap; 
  /// Maximum number of consecutive missed sensors
  int m_maxMissed;       
  /// Minimum number of hits on track to tag as used
  unsigned int m_minToTag;  

  double m_phiTol;
  double m_phiTolOverlap;
  double m_d2Max;
  double m_fractionFound;
  double m_maxChi2PerHit;
  double m_maxChi2ToAdd;
  double m_maxQFactor;
  double m_maxQFactor3;
  double m_deltaQuality;
  double m_fractionForMerge;

  double m_maxDistToMerge;
  double m_maxDeltaSlopeToMerge;

  unsigned int m_maxRZForExtra;
  double m_phiUnusedFirstTol;
  double m_phiUnusedSecondTol;

  /// Pointer to detector element
  DeVL* m_det;
  /// Number of zones in an R sensor
  unsigned int m_nZonesR;
  /// Number of zones in a Phi sensor
  unsigned int m_nZonesPhi;
  
  /// Hits of an event grouped by sensor and zone
  std::vector<std::vector<PrVLHits> > m_hits;
  /// Vector of hit objects (to avoid newing)
  std::vector<PrVLHit> m_pool;
  std::vector<PrVLHit>::iterator m_nextInPool;  

  bool m_debug;
  std::string m_debugToolName;
  IPrDebugTool* m_debugTool;
  int m_wantedKey;

  bool m_doTiming;
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
