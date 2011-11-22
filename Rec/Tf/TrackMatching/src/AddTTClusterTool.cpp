// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

// from GSL
#include "gsl/gsl_math.h"

// from STDet
#include "STDet/DeSTDetector.h"
#include "STDet/DeTTDetector.h"

// from STEvent
#include "Event/STCluster.h"

// from TrackEvent
#include "Event/State.h"

// from TrackFitEvent
#include "Event/STMeasurement.h"
#include "TrackKernel/StateTraj.h"

// local
#include "AddTTClusterTool.h"
#include "TTCandidate.h"

// from LHCbKernel
#include "Kernel/ITrajPoca.h"
#include "Kernel/Trajectory.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ISTClusterPosition.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AddTTClusterTool
//
// 2006-05-15 : J. van Tilburg Jeroen.van.Tilburg@cern.ch
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( AddTTClusterTool )

using namespace Gaudi;
using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AddTTClusterTool::AddTTClusterTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent)
  : GaudiTool( type, name, parent)
  , m_extrapolator(0)
  , m_ttTracker(0)
{
  declareInterface<IAddTTClusterTool>( this );

  declareProperty( "TTClusterCut",     m_ttClusterCut = 7.0 );
  declareProperty( "MinTTHits",        m_minTTHits = 3 );
  declareProperty( "InterStationCut",  m_interStationCut = 2.0 );
  declareProperty( "IntraStationCut",  m_intraStationCut = 1.0 );
  declareProperty( "SpreadWeight",     m_spreadWeight = 7.0 );
  declareProperty( "AddLHCbIDs",       m_addLHCbIDs = true );
  declareProperty( "Extrapolator",
                   m_extrapolatorName = "TrackHerabExtrapolator" );
  declareProperty( "TTGeometryPath",
                   m_ttTrackerPath = DeSTDetLocation::location("TT") );
  declareProperty( "TTClusterPositionTool",
                   m_ttPositionToolName = "STOfflinePosition" );

  declareProperty("yTol", m_yTol = 20.0 * Gaudi::Units::mm);
}
//=============================================================================
// Destructor
//=============================================================================
AddTTClusterTool::~AddTTClusterTool() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode AddTTClusterTool::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  // Add incident to load the TT clusters for each event
  incSvc()->addListener( this, IncidentType::BeginEvent );

  // Access the extrapolator tools
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName );

  // Get TT silicon tracker geometry
  m_ttTracker = getDet<DeTTDetector>( m_ttTrackerPath );

  // Retrieve the STClusterPosition tool for TT
  m_ttPositionTool = tool<ISTClusterPosition>( m_ttPositionToolName );

  // Magnetic field
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );

  // Poca tool to calculate distance of closest approach
  m_poca = tool<ITrajPoca>( "TrajPoca" );

  return StatusCode::SUCCESS;
}

void AddTTClusterTool::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { 
    m_ttClustersLoaded = false; 
  }
}

//=============================================================================
// Load all the TT cluster of the event (executed automatically)
//=============================================================================
void AddTTClusterTool::loadTTClusters( ) 
{ 
  m_clusterTrajectories.clear();
  
  // Get the TT Clusters
  STClusters* clusters = get<STClusters>( STClusterLocation::TTClusters );
  m_clusterTrajectories.reserve(clusters->size());

  // Loop over the clusters and make a trajectory for each cluster
  // TODO: maybe put this functionality in STMeasurement and use projectors
  STClusters::iterator iClus = clusters -> begin();
  for ( ; iClus != clusters -> end(); ++iClus ) {

    // Get the cluster and STChannelID
    STCluster* stCluster = (*iClus) ;
    STChannelID stChan = stCluster -> channelID() ; 
    ISTClusterPosition::Info measVal =
      m_ttPositionTool -> estimate( stCluster );

    // Create a trajectory for this cluster and add combination to the vector
    STClusterTrajectory thisClusTraj;    
    thisClusTraj.first  = stCluster ;
    // explicitly transfer ownership from the auto_ptr returned by 'trajectory'
    // to the shared_ptr thisClusTraj.second
    thisClusTraj.second.reset( m_ttTracker -> trajectory( LHCbID( stChan ),
                                       measVal.fractionalPosition).release() );
    m_clusterTrajectories.push_back( thisClusTraj ) ;
  }
}

//=============================================================================
// Add TT clusters to a track
//=============================================================================
StatusCode AddTTClusterTool::addTTClusters( Track& track ) 
{
  std::vector<double> ttChi2s; ttChi2s.reserve(8);
  std::vector<STCluster*> ttClusters; ttClusters.reserve(8);
  return this -> addTTClusters( track, ttClusters, ttChi2s );
}

//=============================================================================
// Add TT clusters to a track
//=============================================================================
StatusCode AddTTClusterTool::addTTClusters( Track& track, 
                                            std::vector<STCluster*>& ttClusters,
                                            std::vector<double>& ttChi2s ) 
{
  if ( !m_ttClustersLoaded ) loadTTClusters();

  // Get the z -position of first layer
  const DeSTDetector::Layers& ttLayers = m_ttTracker->layers();
  const unsigned int numTTLayers = ttLayers.size();
  DeSTDetector::Layers::const_iterator iLayer = ttLayers.begin();
  double zTT = ((*iLayer)->globalCentre()).z() ;

  // Make a new TT state
  State ttState = track.closestState( zTT );
  //State ttState = aState;

  // The vector of pointers to TT candidate hit-clusters
  TTCandidates candidates; 

  // loop over layers in TT and find which sector is hit
  for ( ; iLayer != ttLayers.end(); ++iLayer) {

    // For this TT layer: extrapolate the new State to the z of the layer
    DeSTLayer* aLayer = *iLayer;
    double zLayer = (aLayer->globalCentre()).z() ;
    StatusCode sc = m_extrapolator -> propagate( ttState, zLayer );
    if ( sc.isFailure() ) { 
      debug() << "Extrapolation of state to z=" << zLayer << " failed" <<endmsg;
      continue;
    }

    // calculate the layer number (0-3)
    unsigned int layerNum = iLayer - ttLayers.begin();

    // Get the state trajectory
    XYZVector bfield(0,0,0);
    m_pIMF -> fieldVector( ttState.position(), bfield ).ignore();
    StateTraj stateTraj = StateTraj( ttState.stateVector(), zLayer, bfield );

    // Loop over the clusters      
    STClusterTrajectories::iterator iClusTraj = m_clusterTrajectories.begin();
    for ( ; iClusTraj != m_clusterTrajectories.end(); ++iClusTraj ) {
      
      // Only consider the clusters in this layer
      if ( !(*iLayer)->contains(((*iClusTraj).first)->channelID()) ) continue;
      
      // Get the Trajectory pointer from the ClusterTrajectory
      Trajectory* measTraj = ((*iClusTraj).second).get();
      
      // check there is a physical overlap with the track
      double yState = ttState.y();

      double yMin = measTraj->beginPoint().y();
      double yMax = measTraj->endPoint().y();
      if (yMin > yMax) std::swap(yMin, yMax);

      if ((yState > yMax + m_yTol) || (yState < yMin - m_yTol)) continue; 

      // Determine the distance between track state and measurement
      double s1, s2;
      XYZVector distance3D;
      s1 = 0.0;
      s2 = measTraj->arclength( stateTraj.position(s1) );
      StatusCode msc = m_poca -> minimize( stateTraj, s1, *measTraj, s2,
                                           distance3D, 50*Gaudi::Units::mm );
      if( msc.isFailure() ) {
        Warning("TrajPoca minimize failed in addTTClusters", StatusCode::SUCCESS,0).ignore();
        if(msgLevel(MSG::DEBUG)) debug() << "TrajPoca minimize failed in addTTClusters" << endmsg;
      }
      int signDist = ( distance3D.x() > 0.0 ) ? 1 : -1 ;
      double distance = signDist * distance3D.R();
      
      if ( fabs( distance ) < m_ttClusterCut ) {
        bool clusterUsed = false;
        unsigned int iCand = 0;
        unsigned int numCandidates = candidates.size();
        while ( iCand < numCandidates ) {
          TTCandidate* cand = candidates[iCand];
          if ( cand != 0 && cand->lastLayer() < layerNum ) {
            bool interStation = layerNum == 2 || 
              (layerNum == 3 && cand->lastLayer() != 2 );
            double delta = fabs(cand->lastDistance() - distance);
            if ( (interStation && delta < m_interStationCut) ||
                 (!interStation && delta < m_intraStationCut) ) {
              // if cluster compatible: make new set of clusters
              // using the old one and flag the old as dead.
              TTCandidate* newCand = new TTCandidate( cand, (*iClusTraj).first,
                                                      distance, layerNum,
                                                      ttState.stateVector() );
              candidates.push_back( newCand );
              cand->setDead(true);
              clusterUsed = true;
            } 
          }
          ++iCand;
        } 
        if ( !clusterUsed ) {
          TTCandidate* newCand = new TTCandidate( (*iClusTraj).first, distance,
                                                  layerNum,
                                                  ttState.stateVector() );
          candidates.push_back( newCand );
        }
      }
    } // loop iClusTraj

    // delete all dead candidates and those with too few hits
    TTCandidates::iterator iCand = candidates.begin();
    while ( iCand != candidates.end() ) {
      bool tooFewHits = int( (*iCand)->numTTClusters() - m_minTTHits + 
                             numTTLayers - layerNum ) < 1;
      if ( (*iCand)->isDead() || tooFewHits ) {
        delete *iCand;
        iCand = candidates.erase( iCand );
      } else ++iCand;
    }
  } // loop iLayer

  // find the best TT candidate
  std::vector<TrackVector> refVectors; refVectors.reserve(10);
  double bestQuality = 2.*m_ttClusterCut;
  TTCandidates::const_iterator iCand = candidates.begin();
  for ( ; iCand != candidates.end(); ++iCand ) {
    // the quality is defined as: |<distance>| + m_spreadWeight*spread 
    double quality = fabs((*iCand)->averageDistance()) + 
      m_spreadWeight * (*iCand)->spread();
    if ( quality < bestQuality ) {
      bestQuality = quality;
      ttClusters = (*iCand)->ttClusters();
      ttChi2s = (*iCand)->distances();
      refVectors = (*iCand)->refVectors();
    }
    delete *iCand;
  }
  candidates.clear();
  
  // Add the LHCbIDs and Measurements to the track
  if ( m_addLHCbIDs ) {
    LHCbID lhcbID;
    std::vector<STCluster*>::const_iterator iClus = ttClusters.begin();
    //    std::vector<TrackVector>::const_iterator iRef = refVectors.begin();
    while ( iClus != ttClusters.end() ) {
      const STCluster* aCluster = *iClus;
      lhcbID = LHCbID( aCluster -> channelID() );
      track.addToLhcbIDs( lhcbID );
      ++iClus;
    }
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Added " << ttClusters.size() << " TT hits as LHCbIDs to the track." << endmsg; 
  }
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Get the distance to the strip
//=============================================================================
double AddTTClusterTool::distanceToStrip( const Track& track, 
                                          const STCluster& ttCluster ) 
{
  if ( !m_ttClustersLoaded ) loadTTClusters();

  // Get the TT layer
  DeSTLayer* ttLayer = m_ttTracker->findLayer( ttCluster.channelID() );
  double zLayer = (ttLayer->globalCentre()).z() ;

  // Make a new TT state
  const State& aState = track.closestState( zLayer );
  State ttState = aState;

  // For this TT layer: extrapolate the new State to the z of the layer
  StatusCode sc = m_extrapolator -> propagate( ttState, zLayer );
  if ( sc.isFailure() ) { 
    debug() << "Extrapolation of state to z=" << zLayer << " failed." <<endmsg;
    return 0.0;
  }

  // Get the state trajectory
  XYZVector bfield(0,0,0);
  m_pIMF -> fieldVector( ttState.position(), bfield );
  StateTraj stateTraj = StateTraj( ttState.stateVector(), zLayer, bfield );

  // Find the trajectory      
  STClusterTrajectories::iterator iClusTraj = m_clusterTrajectories.begin();
  while ( iClusTraj != m_clusterTrajectories.end() && 
          ((*iClusTraj).first)->channelID()!=ttCluster.channelID()) ++iClusTraj;
  if ( iClusTraj == m_clusterTrajectories.end() ) {
    Warning("A Trajectory could not be found corresponding to th cluster.",StatusCode::SUCCESS,0).ignore() ;
    
    if(msgLevel(MSG::DEBUG)) debug() << "A Trajectory could not be found corresponding to th cluster." << endmsg;
    return 0.0;    
  }
  Trajectory* measTraj = ((*iClusTraj).second).get();  

  // Determine the distance between track state and measurement
  double s1, s2;
  XYZVector distance3D;
  s1 = 0.0;
  s2 = measTraj->arclength( stateTraj.position(s1) );
  StatusCode asc = m_poca -> minimize( stateTraj, s1, *measTraj, s2,
                                       distance3D, 50*Gaudi::Units::mm );
  if( asc.isFailure() ) {
    Warning("TrajPoca minimize failed in distanceToStrip",StatusCode::SUCCESS,0).ignore() ;
    if(msgLevel(MSG::DEBUG)) debug() << "TrajPoca minimize failed in distanceToStrip" << endreq;
  }
  int signDist = ( distance3D.x() > 0.0 ) ? 1 : -1 ;
  return signDist * distance3D.R(); 
}

//=============================================================================
