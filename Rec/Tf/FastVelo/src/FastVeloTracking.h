// $Id: $
#ifndef FASTVELOTRACKING_H 
#define FASTVELOTRACKING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "PatKernel/IPatDebugTool.h"
#include "GaudiAlg/ISequencerTimerTool.h"

#include "FastVeloHitManager.h"
#include "FastVeloTrack.h"

/** @class FastVeloTracking FastVeloTracking.h
 *  Fast implementation of teh Velo tracking
 *
 *  @author Olivier Callot
 *  @date   2010-09-09
 */
class FastVeloTracking : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FastVeloTracking( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FastVeloTracking( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  void findQuadruplets( unsigned int sens0, bool forward );
  
  void findUnusedTriplets( unsigned int sens0, bool forward );
  
  FastVeloHit* closestHit( FastVeloHits& hits, double coord, double tol ); 

  FastVeloHit* closestUnusedHit( FastVeloHits& hits, double coord, double tol ); 

  int extendTrack( FastVeloTrack& newTrack, FastVeloSensor* sensor, 
                   unsigned int zone, bool forward );
  
  void addOppositeSideRHits( FastVeloTrack& newTrack, const int &zone,
                             FastVeloSensor* sensor, bool forward );
  
  void makeLHCbTracks( LHCb::Tracks* output );
 
  void makeSpaceTracks( FastVeloTrack& input );

  void mergeSpaceClones();
  
  void beamSplashSpaceMerge(); ///< Merge "space" tracks if beam splash event is detected

  void findUnusedPhi();

  void mergeClones( FastVeloTracks& tracks );

  ///  Remove tracks with phi clusters used too many times on average
  void phiReuseKill( ); 

  //== Debugging methods

  bool matchKey( const FastVeloHit* hit ) {
    if ( m_debugTool ) {
      LHCb::LHCbID id = hit->lhcbID();
      return m_debugTool->matchKey( id, m_wantedKey );
    }
    return false;
  }

  void printCoord( const FastVeloHit* hit, std::string title );

  void printTrack( FastVeloTrack& track, std::string header = "" );

  void printRTrack( FastVeloTrack& track, std::string header = "" );

  void printCoords( FastVeloHits& hits, std::string header = "" );

protected:

private:
  std::string m_outputLocation;
  std::string m_hitManagerName;
  
  //== Overall control parameters

  bool   m_onlyForward;
  bool   m_onlyBackward;
  bool   m_HLT1Only;
  bool   m_HLT2Complement;
  unsigned int m_maxRZForExtra;
  bool   m_stateAtBeam;
  bool   m_resetUsedFlags;

  //== Paramaters for RZ search  
  double m_zVertexMin;      ///< Minimal Z of a vertex for forward tracks
  double m_zVertexMax;      ///< Maximal Z of a vertex for defining last sensor in backward tracks
  double m_zVertexMaxBack;  ///< Maximal Z of a vertex for backward tracks
  double m_maxRSlope;       ///< Maximum RZ slope considered
  double m_rMatchTol4;      ///< R match tolerance in a quadruplet, in pitch unit
  double m_rMatchTol3;      ///< R match tolerance in a triplet, in pitch unit
  double m_rExtraTol;       ///< R match tol. extrapolating, in pitch unit
  double m_rOverlapTol;     ///< R match in overlap, in pitch unit
  int    m_maxMissed;       ///< Maximum number of consecutive missed sensors
  unsigned int m_minToTag;  ///< Minimum number of hit on track to tag as used

  double m_phiMatchZone;
  double m_phiCentralZone;
  double m_maxDelta2;
  double m_fractionFound;
  double m_maxChi2PerHit;
  double m_maxChi2ToAdd;
  double m_maxQFactor;
  double m_maxQFactor3;
  double m_deltaQuality;
  double m_fractionForMerge;

  double m_maxDistToMerge;
  double m_maxDeltaSlopeToMerge;
  
  double m_phiUnusedFirstTol;
  double m_phiUnusedSecondTol;

  double m_maxTrackClusterFrac; ///< fraction of tracks allowed per cluster (check for splash events)
  double m_maxMergeManyTracks; ///< fraction of clusters to remove clone in splash event

  double m_PhiReuseLimit; ///< if on average phi clusters used more than this kill the track

  bool   m_useKalmanFit ;
  double m_kalmanPtForScattering ;
  bool   m_kalmanStateLastMeasurement ;
  bool   m_kalmanStateEndVelo ;
  std::vector<double>  m_kalmanScatteringNoiseParameters;
  
  //== Debugging controls
  std::string      m_debugToolName;
  int              m_wantedKey;
  IPatDebugTool*   m_debugTool;
  bool             m_isDebug;
  bool             m_debug;
  bool             m_doTiming;
  ISequencerTimerTool* m_timerTool;
  int   m_timeTotal;
  int   m_timePrepare;
  int   m_timeFwd4;
  int   m_timeBkwd4;
  int   m_timeFwd3;
  int   m_timeBkwd3;
  int   m_timeSpace;
  int   m_timeUnused;
  int   m_timeFinal;

  //== Working variables
  FastVeloHitManager* m_hitManager;
  FastVeloTracks  m_tracks;  ///< Vector of tracks used during the processing
  FastVeloTracks  m_spaceTracks;  ///< Vector of tracks used during the processing
  double          m_cosPhi;
  double          m_sinPhi;
};
#endif // FASTVELOTRACKING_H
