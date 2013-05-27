#ifndef ADDTTCLUSTERTOOL_H
#define ADDTTCLUSTERTOOL_H 1

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
#include "TrackInterfaces/IAddTTClusterTool.h"

// from TrackEvent
#include "Event/Track.h"

#include <string>

class DeTTDetector;
class ITrajPoca;
class ITrackExtrapolator;
class ISTClusterPosition;
class IMagneticFieldSvc;

/** @class AddTTClusterTool AddTTClusterTool.h  
 *
 *  Tool to add TT hits to a standard track. It requires as input a Track
 *  which has at least one State near the TT.
 *
 *  @author:   Jeroen van Tilburg Jeroen.van.Tilburg@cern.nl
 *  @date:     12-05-2006
 */

class AddTTClusterTool : public GaudiTool,
                         virtual public IAddTTClusterTool,
                         virtual public IIncidentListener {
public: 
  /// Standard constructor
  AddTTClusterTool( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent );

  virtual ~AddTTClusterTool( ); ///< Destructor

  StatusCode initialize();    ///< Algorithm initialization

  /// Add TT clusters to matched tracks
  StatusCode addTTClusters( LHCb::Track& track );

  /// Add TT clusters to matched tracks
  StatusCode addTTClusters( LHCb::Track& track,
                            std::vector<LHCb::STCluster*>& ttClusters,
                            std::vector<double>& ttChi2s );

  /// Get the distance to the strip
  double distanceToStrip( const LHCb::Track& track, 
                          const LHCb::STCluster& ttCluster );

  /// Method not implemented
  StatusCode returnTTClusters( LHCb::State& /*state*/, std::vector<PatTTHit*>& /*ttHits*/, double& /*finalChi2*/, double /*p*/){
    return StatusCode::SUCCESS;
  };

private:

  /// Load the TT clusters and link to trajectories
  void loadTTClusters( );

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
  DeTTDetector* m_ttTracker;           ///< Pointer to the TT XML geom
  std::string   m_ttTrackerPath;       ///< Name of the TT XML geom path
  ISTClusterPosition* m_ttPositionTool;///< STClusterPosition tool for TT
  std::string m_ttPositionToolName;    ///< STCluster position tool name for TT
  IMagneticFieldSvc* m_pIMF;           ///< Pointer to the magn. field service
  ITrajPoca*         m_poca;           ///< Pointer to the ITrajPoca interface
  bool m_ttClustersLoaded;             ///< Flag to determine begin of event

  // Job options
  // -----------
  /// The extrapolator name
  std::string m_extrapolatorName;
  /// Chi2 cut to add the TT clusters (only the best in a wafer is taken)
  double m_ttClusterCut;
  /// Minimum number of TT clusters
  unsigned int m_minTTHits;
  /// maximum distance difference between TT clusters of different stations
  double m_interStationCut;
  /// maximum distance difference between TT clusters of same station
  double m_intraStationCut;
  /** The quality is defined as: |distance| + m_spreadWeight*spread   
   *  The quality the the criterium to select the best TTCandidate (= set
   *  of TT clusters.)
   */
  double m_spreadWeight;
  bool m_addLHCbIDs;

 
  double m_yTol;

};
#endif // ADDTTCLUSTERTOOL_H
