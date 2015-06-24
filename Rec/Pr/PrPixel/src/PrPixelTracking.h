#ifndef PRPIXELTRACKING_H
#define PRPIXELTRACKING_H 1

// #define DEBUG_HISTO // fill some histograms while the algorithm runs.

// Gaudi
#ifdef DEBUG_HISTO
#include "GaudiAlg/GaudiTupleAlg.h"
#else
#include "GaudiAlg/GaudiAlgorithm.h"
#endif
#include "GaudiAlg/ISequencerTimerTool.h"

// Local
#include "PrKernel/IPrDebugTool.h"
#include "PrPixelHitManager.h"
#include "PrPixelTrack.h"

/** @class PrPixelTracking PrPixelTracking.h
 *  This is the main tracking for the Velo Pixel upgrade
 *
 *  @author Olivier Callot
 *  @date   2011-12-16
 */
#ifdef DEBUG_HISTO
class PrPixelTracking : public GaudiTupleAlg {
#else
class PrPixelTracking : public GaudiAlgorithm {
#endif

 public:
  /// Standard constructor
  PrPixelTracking(const std::string &name, ISvcLocator *pSvcLocator);
  /// Destructor
  virtual ~PrPixelTracking();

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

 private:
  /// Search for tracks starting from pair of hits on adjacent sensors
  void searchByPair();
  /// Extrapolate a seed track and try to add further hits.
  void extendTrack(const PrPixelHit *h1, const PrPixelHit *h2);
  /// Try to add a matching hit on a given module.
  PrPixelHit *bestHit(PrPixelModule* module, const float xTol, 
                      const float maxScatter,
                      const PrPixelHit* h1, const PrPixelHit* h2) const;
  /// Produce LHCb::Track list understandable to other LHCb applications.
  void makeLHCbTracks();

  /// Debugging methods
  bool matchKey(const PrPixelHit *hit) const {
    if (m_debugTool) {
      LHCb::LHCbID id = hit->id();
      return m_debugTool->matchKey(id, m_wantedKey);
    }
    return false;
  }
  void printHit(const PrPixelHit* hit, const std::string& title = "") const;
  void printTrack(PrPixelTrack& track) const;
  void printHitOnTrack(const PrPixelHit* hit, const bool ifMatch = true) const;

  /// Location of output container
  std::string m_outputLocation;
  /// Hit manager tool
  PrPixelHitManager *m_hitManager;

  /// Slope limits for seed pairs
  float m_maxXSlope;
  float m_maxYSlope;
  /// Parameters for track extrapolation
  float m_extraTol;
  unsigned int m_maxMissed;
  /// Criteria for adding hits to an existing track
  float m_maxScatter;
  /// Max. chi2 for 3-hit tracks
  float m_maxChi2Short;
  /// Min. fraction of unused hits
  float m_fractionUnused;

  bool m_clearHits;
  bool m_stateClosestToBeamKalmanFit;
  bool m_stateEndVeloKalmanFit;
  bool m_addStateFirstLastMeasurementKalmanFit;

  /// List of tracks found
  PrPixelTracks m_tracks;
  /// Current track being worked with
  PrPixelTrack m_track;

  /// Debug control
  std::string m_debugToolName;
  int m_wantedKey;
  IPrDebugTool *m_debugTool;
  bool m_debug;

  /// Use lite clusters or raw banks.
  bool m_runOnRawBanks;

  /// Maximum clusters size (no effect when running on lite cluster banks).
  unsigned int m_maxClusterSize;
  /// Are we running in the trigger?
  bool m_trigger;

  /// Timing measurement control
  bool m_doTiming;
  ISequencerTimerTool *m_timerTool;
  int m_timeTotal;
  int m_timePrepare;
  int m_timePairs;
  int m_timeFinal;
};
#endif  // PRPIXELTRACKING_H
