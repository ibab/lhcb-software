// $Id: $for velo pix track fit
#ifndef VPPATLINEAR_H 
#define VPPATLINEAR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "TrackInterfaces/IVPClusterPosition.h"
#include "VPDet/DeVP.h"
#include "Event/VPLiteCluster.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "VPTrack.h"
/** @class VPPatLinear VPPatLinear.h
 *  
 *
 *  @author Wenbin Qian
 *  @date   2011-02-11
 */
class VPPatLinear : public GaudiAlgorithm {
public: 
  /// Standard constructor
  VPPatLinear( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VPPatLinear( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  void findQuadruplets(int);
  void findTriplets(int);
   void findCrossed(int);
  void extendTrack(VPTrack& , int, bool);
  void makeLHCbTracks( LHCb::Tracks* output );
  void mergeClones();
  void addAnotherSideHits();
protected:

private:
    DeVP*  m_vP;
    IVPClusterPosition* m_positiontool;
    std::string m_clusterLocation;
    std::string m_outputTracksLocation;
    LHCb::VPLiteCluster::VPLiteClusters* m_clusters;
    int m_sensor;
    double m_chi2;
    VPTracks m_tracks;
    std::vector<VPHits> m_hits;
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
#endif // VPPATLINEAR_H
