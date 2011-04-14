// $Id: $for velo pix track fit
#ifndef VELOPIXPATLINEAR_H 
#define VELOPIXPATLINEAR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "TrackInterfaces/IVeloPixClusterPosition.h"
#include "VeloPixDet/DeVeloPix.h"
#include "Event/VeloPixLiteCluster.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "VeloPixTrack.h"
/** @class VeloPixPatLinear VeloPixPatLinear.h
 *  
 *
 *  @author Wenbin Qian
 *  @date   2011-02-11
 */
class VeloPixPatLinear : public GaudiAlgorithm {
public: 
  /// Standard constructor
  VeloPixPatLinear( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloPixPatLinear( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  void findQuadruplets(int);
  void findTriplets(int);
   void findCrossed(int);
  void extendTrack(VeloPixTrack& , int, bool);
  void makeLHCbTracks( LHCb::Tracks* output );
  void mergeClones();
  void addAnotherSideHits();
protected:

private:
    DeVeloPix*  m_veloPix;
    IVeloPixClusterPosition* m_positiontool;
    std::string m_clusterLocation;
    std::string m_outputTracksLocation;
    LHCb::VeloPixLiteCluster::VeloPixLiteClusters* m_clusters;
    int m_sensor;
    double m_chi2;
    VeloPixTracks m_tracks;
    std::vector<VeloPixHits> m_hits;
    double m_dist;	
    int m_maxMissed;
    bool m_stateAtBeam;
    double m_fractionForMerge;

    bool m_doTiming;
    ISequencerTimerTool* m_timerTool;
    int m_timeTotal;
    int m_timePrepare;
    int m_timeFind4;
    int m_timeFind3;
    int m_timeCross;
    int m_timeMerge;
    int m_timeFinal;

};
#endif // VELOPIXPATLINEAR_H
