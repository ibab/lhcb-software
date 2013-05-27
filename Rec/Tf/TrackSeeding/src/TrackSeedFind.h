#ifndef TRACKSEEDING_SEEDFind_H 
#define TRACKSEEDING_SEEDFind_H 1

// Include files
// from Gaudi
#include <GaudiAlg/GaudiAlgorithm.h>
#include "STDet/DeSTDetector.h"
#include "OTDet/DeOTDetector.h"
#include <utility>

//tools
#include "TrackInterfaces/ITrackMomentumEstimate.h"
#include "TrackInterfaces/ISTClusterPosition.h"

/** @class TrackSeedFind TrackSeedFind.h
 *  
 *
 *  @author HJ Bulten, henkjan@nikhef.nl
 *  @date   2005-12-15
 */

// TrackSeedHit: helper class to calculate positions of hits in (rotated/translated) planes 
// quickly. The class deals with IT and OT hits in a transparant way
// TrackCand: helper structure for storage of temporary Seed candidates

class TrackSeedHit;
class TrackCand;

class TrackSeedFind : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TrackSeedFind( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackSeedFind(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


private:

  //methods

  StatusCode getData(); // fill the hit arrays
  StatusCode findSeeds(); // do the work
  StatusCode maketracks(); // make tracks
  StatusCode pileUpKiller(); // kill spill-over tracks.
  StatusCode cloneKiller(); // kill clones

  //Geometry
  std::string m_itTrackerPath; ///< Name of the IT XML geom path
  DeSTDetector* m_itTracker;
  std::string m_otTrackerPath; ///< Name of the OT XML geom path
  DeOTDetector* m_otTracker;
  std::string m_ITclusterPath; // location of the STLiteClusters
  std::string m_OTtimePath; // location ot the OTTimes
  std::string m_trackLocation; // output track location
 
    //Job Options
  bool m_usePtKick; // use B-field info
  bool m_useSpilloverKiller; // test for spill-over tracks
  double m_maxXSlope; // maximal accepted slope
  double m_sigmaOutlierX;  // reject hits XX sigma away
  double m_sigmaOutlierY; // reject hits XX sigma away
  double m_YSlopDev; // maximal deviation in Y-slope , for particles coming from IP
  double m_OTDriftSpeed; // for r-t relation OT straws
  double m_OTWireDriftTime; // for r-t relation OT straws, ns/m
  double m_OTCellRadius; // for r-t relation OT
  double m_otTimeAv; // "average" calibrated OT time for a valid track, to distinguish pile-up hits
  double m_otTimeWindow; // width of calibrated OT time in which the average value should fall.
  bool m_debug;   // for debugging, loop counting etc.
  bool m_verbose; // for extra debugging
  bool m_useIT; // for debugging, switch on/off IT hits
  bool m_useOT; // for debugging, switch on/off OT hits
  int m_maxOTHits; // to limit the occupancy
// for the following minimum hits: an IT hit counts double
  int m_minX13; // minimum X hits in one station in the track candidates
  int m_minY13; // minimum Y hits in one station in the track candidates
  int m_minT1T3; // minimum X hits/Y hits on a straight line through chamber 1 and 3
  int m_minUV13; // minimum number of U hits and of V hits in a T1-T3 straight candidate
  int m_minUV123; // minimum number of U and V hits in a track candidate
  int m_minX123; // minimum number of X hits in a track candidate
  int m_minY123; // minimum number of Y hits in a track candidate
  int m_minT123; // minimum number of total hits in a track candidate
  double m_maxChi2; // maximum allowed chi2 for all fits

  
  ITrackMomentumEstimate* m_fCalcMomentum; // momentum tool
  ISTClusterPosition* m_itPositionTool; // IT cluster position tool; necessary to create measurements
  
// an average displacement for a curved track will be calculated in order to set a window
// around x-hits in station 2. Therefore, the magnetic mid-z and the ratio
// for the integrals between the origin  and station 1 and the origin and station 3 are used
  double m_zMagnet; // midplane 
  double m_zRotPoint; // z where the effective angle change between T1 and T3 takes place
  double m_BAngleChange;  // fractional change of track angle between T1 and T3 with respect to T1 and the origin 
  double m_zmin[3]; // minimum z positions of T1,T2,T3 planes
  double m_zmax[3]; // maximum z positions of T1,T2,T3 planes
  double m_minstereo; // smallest absolute value of the stereo angle in the U,V planes
  double m_maxstereo; // largest absolute value of the stereo angle in the U,V planes

// Hit searching: TrackSeedHits
  std::multimap<double, SeedHit> T1X; // contains IT and OT hits
  std::multimap<double, SeedHit> T1Y;
  std::multimap<double, SeedHit> T2X;
  std::multimap<double, SeedHit> T2Y;
  std::multimap<double, SeedHit> T3X;
  std::multimap<double, SeedHit> T3Y;

  std::list<TrackCand> trackCands; // candidates.

};
#endif 
