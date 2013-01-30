// $Id: AddUTClusterTool.h,v 1.3 2010-02-08 17:55:53 decianm Exp $
#ifndef ADDUTCLUSTERTOOL_H
#define ADDUTCLUSTERTOOL_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"

// for std::auto_ptr
#include <boost/shared_ptr.hpp>

// From DigiEvent
#include "Event/STCluster.h"

// from TrackInterfaces
#include "TrackInterfaces/IAddUTClusterTool.h"

// from TrackEvent
#include "Event/Track.h"

#include <string>

class DeUTDetector;
class ITrajPoca;
class ITrackExtrapolator;
class ISTClusterPosition;
class IMagneticFieldSvc;

/** @class AddUTClusterTool AddUTClusterTool.h  
 *
 *  Tool to add UT hits to a standard track. It requires as input a Track
 *  which has at least one State near the UT.
 *
 *  @author:   Jeroen van Tilburg Jeroen.van.Tilburg@cern.nl
 *  @date:     12-05-2006
 */

class AddUTClusterTool : public GaudiTool,
                         virtual public IAddUTClusterTool,
                         virtual public IIncidentListener {
public: 
  /// Standard constructor
  AddUTClusterTool( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent );

  virtual ~AddUTClusterTool( ); ///< Destructor

  StatusCode initialize();    ///< Algorithm initialization

  /// Add UT clusters to matched tracks
  StatusCode addUTClusters( LHCb::Track& track );

  /// Add UT clusters to matched tracks
  StatusCode addUTClusters( LHCb::Track& track,
                            std::vector<LHCb::STCluster*>& ttClusters,
                            std::vector<double>& ttChi2s );

  /// Get the distance to the strip
  double distanceToStrip( const LHCb::Track& track, 
                          const LHCb::STCluster& ttCluster );

  /// Method not implemented
  StatusCode returnUTClusters( LHCb::State& /*state*/, std::vector<PatTTHit*>& /*ttHits*/, double& /*finalChi2*/, double /*p*/){
    return StatusCode::SUCCESS;
  };

private:

  /// Load the UT clusters and link to trajectories
  void loadUTClusters( );

  /** Implement the handle method for the Incident service.
  *  This is used to inform the tool of software incidents.
  *
  *  @param incident The incident identifier
  */
  void handle( const Incident& incident );

  typedef std::pair<LHCb::STCluster*, boost::shared_ptr<LHCb::Trajectory> >
    STClusterTrajectory;
  typedef std::vector<STClusterTrajectory> STClusterTrajectories;
  STClusterTrajectories m_clusterTrajectories;

  ITrackExtrapolator* m_extrapolator;  ///< The extrapolator
  DeUTDetector* m_utTracker;           ///< Pointer to the UT XML geom
  std::string   m_utTrackerPath;       ///< Name of the UT XML geom path
  ISTClusterPosition* m_utPositionTool;///< STClusterPosition tool for UT
  std::string m_utPositionToolName;    ///< STCluster position tool name for UT
  IMagneticFieldSvc* m_pIMF;           ///< Pointer to the magn. field service
  ITrajPoca*         m_poca;           ///< Pointer to the ITrajPoca interface
  bool m_utClustersLoaded;             ///< Flag to determine begin of event

  // Job options
  // -----------
  /// The extrapolator name
  std::string m_extrapolatorName;
  /// Chi2 cut to add the UT clusters (only the best in a wafer is taken)
  double m_utClusterCut;
  /// Minimum number of UT clusters
  unsigned int m_minUTHits;
  /// maximum distance difference between UT clusters of different stations
  double m_interStationCut;
  /// maximum distance difference between UT clusters of same station
  double m_intraStationCut;
  /** The quality is defined as: |distance| + m_spreadWeight*spread   
   *  The quality the the criterium to select the best UTCandidate (= set
   *  of UT clusters.)
   */
  double m_spreadWeight;
  bool m_addMeasurements;
  bool m_addLHCbIDs;

 
  double m_yTol;

};
#endif // ADDUTCLUSTERTOOL_H
