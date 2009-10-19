// $Id: TTGenericTracking.h,v 1.3 2009-10-19 16:32:12 jvantilb Exp $
#ifndef TTGenericTracking_H
#define TTGenericTracking_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "STDet/DeSTDetector.h"

/** @class TTGenericTracking TTGenericTracking.h
 *
 *  Algorithm to find tracks in TT. This algorithm is mainly useful for finding
 *  cosmic tracks, since for regular collision data the occupancy is too high.
 *  Keep in mind that the TT has only 4 layers, so a typical TT track will have
 *  only 4 hits, having 0 degrees of freedom left for the linear track fit (4 
 *  parameters). A presentation on this algorithm was giving on 19.10.2009 in
 *  the T-Rec meeting: http://indico.cern.ch/event/48032 .
 *  The algorithm has three stages:
 *  - <b>Track finding</b>: Loops over the hits in the x layers to make all
 *    possible combinations of 2d tracks out of a hit in an x1 layer and a
 *    hit in an x2 layer. Then, it searches for hits in the u and v layers to
 *    include information on the y position. The intersection of the 2d track
 *    with the stereo hit has to be compatible in y with the begin and end
 *    points of the strip. When it has found two stereo hits, it also checks if
 *    the 3d track is compatible in y with the x hits.
 *  - <b>Adding additional hits</b>: Due to the staggered layout of a
 *    layer, there are overlaps between the sensors within a single layer.
 *    Therefore there can sometimes be more than 4 hits on a track. In this
 *    step, a search is performed on additional hits in these staggered
 *    sensors.
 *  - <b>Removing clones</b>: Also due to these overlaps, there can be clones
 *    (tracks that share most of their hits). In this step all tracks that are a
 *    subset of another track are removed.
 *  
 *  These are the options that can be set:
 *  - <b>ClusterLocation</b>: Input location in the TES of the TT clusters.
 *  - <b>OutputLocation</b>: Output location in the TES of the TT tracks.
 *  - <b>MaxNumClusters</b>: Cut on busy events. Very useful, since for some
 *    (busy) events the required CPU time can be too high. Default value is now
 *    set to 1000 (somewhat arbitrary).
 *  - <b>ChargeCut</b>: Cut on the total charge (in ADC counts) of the
 *    STClusters.
 *  - <b>MergeNeighbours</b>: Boolean flag to merge neighbouring clusters.
 *    Default set to true. Useful for steep tracks (cosmics) in case the cluster
 *    size is large (>4). When set to true, the algorithm only considers the
 *    cluster with the highest charge, thereby avoiding clone tracks.
 *  - <b>ToleranceY</b>: Tolerance for compatibility in y of the hit with the
 *    strip.
 *  - <b>DistCut</b>: Maximum distance in mm of the additional hits to the
 *     track.
 *  - <b>WindowCenter</b>: Vector of three doubles that represents the central
 *    3d point (in mm) of the window through which the tracks are required to
 *    pass.
 *  - <b>HalfWindowXSize and HalfWindowYSize</b>: Size in mm of the half window
 *    in x and y through which the tracks are required to pass. For example,
 *    when the WindowCenter is set to (0,0,880) and the HalfWindowXSize is set
 *    to 500 mm, the tracks are first extrapolated to z=880. Only tracks that
 *    are then within a window of +- 500 mm in x are kept.
 *  - <b>AddRich1State</b>: Boolean flag to determine whether a state at the
 *    entrance of RICH1 is added to the track.
 *
 *  @author J. van Tilburg
 *  @date   25/09/2009
 */

class TTGenericTracking : public GaudiAlgorithm {

public:
 
  /// constructer
  TTGenericTracking( const std::string& name, ISvcLocator *svcloc );

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

  /// finalize
  StatusCode finalize();

private:

  /// Make all possible combinations with 4 hits in 4 different layers
  void findTTtracks(LHCb::Tracks* tracks, Tf::STHits& hits );

  /// Add additional hits to the track
  void addAdditionalHits( LHCb::Tracks* tracks, Tf::STHits& hits );

  /// Find and remove the clones in the container of tracks
  void removeClones( LHCb::Tracks* tracks );
  

  // Internal counters
  int m_nEvent;           ///< Counts the number of events
  int m_nTriggered;       ///< Counts the number of events with TT tracks
  int m_nTrackCandidates; ///< Counts the total number of TT tracks

  /// Pointer to the TT detector element
  DeSTDetector* m_tracker;
  
  // job options
  std::string m_clusterLocation;  ///< Location of the TT clusters
  std::string m_outputLocation;   ///< Location of the TT tracks
  unsigned int m_maxNumClusters;  ///< Cut on busy events
  double m_chargeCut;             ///< Cut on the total charge of the STClusters
  bool m_mergeNeighbours;         ///< Flag to merge neighbouring clusters
  double m_toleranceY;            ///< Tolerance for hit compatibility in y
  double m_distCut;               ///< Cut on distance track to additional hits
  std::vector<double> m_winCenter;///< Central point (x,y,z) of allowed window
  double m_windowX;               ///< Half the size of the window in x
  double m_windowY;               ///< Half the size of the window in y
  bool m_addRich1State;           ///< Flag to add a state at RICH1 entrance

};

#endif // TTGenericTracking_H
