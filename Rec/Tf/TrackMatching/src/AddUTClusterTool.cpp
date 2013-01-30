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
#include "STDet/DeUTDetector.h"

// from STEvent
#include "Event/STCluster.h"

// from TrackEvent
#include "Event/State.h"

// from TrackFitEvent
#include "Event/STMeasurement.h"
#include "TrackKernel/StateTraj.h"

// local
#include "AddUTClusterTool.h"
#include "UTCandidate.h"

// from LHCbKernel
#include "Kernel/ITrajPoca.h"
#include "Kernel/Trajectory.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ISTClusterPosition.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AddUTClusterTool
//
// 2006-05-15 : J. van Tilburg Jeroen.van.Tilburg@cern.ch
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( AddUTClusterTool )

using namespace Gaudi;
using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AddUTClusterTool::AddUTClusterTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent)
  : GaudiTool( type, name, parent)
  , m_extrapolator(0)
  , m_utTracker(0)
{
  declareInterface<IAddUTClusterTool>( this );

  declareProperty( "UTClusterCut",     m_utClusterCut = 7.0 );
  declareProperty( "MinUTHits",        m_minUTHits = 3 );
  declareProperty( "InterStationCut",  m_interStationCut = 2.0 );
  declareProperty( "IntraStationCut",  m_intraStationCut = 1.0 );
  declareProperty( "SpreadWeight",     m_spreadWeight = 7.0 );
  declareProperty( "AddLHCbIDs",       m_addLHCbIDs = true );
  declareProperty( "Extrapolator",
                   m_extrapolatorName = "TrackHerabExtrapolator" );
  declareProperty( "UTGeometryPath",
                   m_utTrackerPath = DeSTDetLocation::location("UT") );
  declareProperty( "UTClusterPositionTool",
                   m_utPositionToolName = "STOfflinePosition" );

  declareProperty("yTol", m_yTol = 20.0 * Gaudi::Units::mm);
}
//=============================================================================
// Destructor
//=============================================================================
AddUTClusterTool::~AddUTClusterTool() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode AddUTClusterTool::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  // Add incident to load the UT clusters for each event
  incSvc()->addListener( this, IncidentType::BeginEvent );

  // Access the extrapolator tools
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName );

  // Get UT silicon tracker geometry
  m_utTracker = getDet<DeUTDetector>( m_utTrackerPath );

  // Retrieve the STClusterPosition tool for UT
  m_utPositionTool = tool<ISTClusterPosition>( m_utPositionToolName );

  // Magnetic field
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );

  // Poca tool to calculate distance of closest approach
  m_poca = tool<ITrajPoca>( "TrajPoca" );

  return StatusCode::SUCCESS;
}

void AddUTClusterTool::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { 
    m_utClustersLoaded = false; 
  }
}

//=============================================================================
// Load all the UT cluster of the event (executed automatically)
//=============================================================================
void AddUTClusterTool::loadUTClusters( ) 
{ 
  m_clusterTrajectories.clear();
  
  // Get the UT Clusters
  STClusters* clusters = get<STClusters>( STClusterLocation::UTClusters );
  m_clusterTrajectories.reserve(clusters->size());

  // Loop over the clusters and make a trajectory for each cluster
  // TODO: maybe put this functionality in STMeasurement and use projectors
  STClusters::iterator iClus = clusters -> begin();
  for ( ; iClus != clusters -> end(); ++iClus ) {

    // Get the cluster and STChannelID
    STCluster* stCluster = (*iClus) ;
    STChannelID stChan = stCluster -> channelID() ; 
    ISTClusterPosition::Info measVal =
      m_utPositionTool -> estimate( stCluster );

    // Create a trajectory for this cluster and add combination to the vector
    STClusterTrajectory thisClusTraj;    
    thisClusTraj.first  = stCluster ;
    // explicitly transfer ownership from the auto_ptr returned by 'trajectory'
    // to the shared_ptr thisClusTraj.second
    thisClusTraj.second.reset( m_utTracker -> trajectory( LHCbID( stChan ),
                                       measVal.fractionalPosition).release() );
    m_clusterTrajectories.push_back( thisClusTraj ) ;
  }
}

//=============================================================================
// Add UT clusters to a track
//=============================================================================
StatusCode AddUTClusterTool::addUTClusters( Track& track ) 
{
  std::vector<double> ttChi2s; ttChi2s.reserve(8);
  std::vector<STCluster*> ttClusters; ttClusters.reserve(8);
  return this -> addUTClusters( track, ttClusters, ttChi2s );
}

//=============================================================================
// Add UT clusters to a track
//=============================================================================
StatusCode AddUTClusterTool::addUTClusters( Track& track, 
                                            std::vector<STCluster*>& ttClusters,
                                            std::vector<double>& ttChi2s ) 
{
  if ( !m_utClustersLoaded ) loadUTClusters();

  // Get the z -position of first layer
  const DeSTDetector::Layers& ttLayers = m_utTracker->layers();
  const unsigned int numUTLayers = ttLayers.size();
  DeSTDetector::Layers::const_iterator iLayer = ttLayers.begin();
  double zUT = ((*iLayer)->globalCentre()).z() ;

  // Make a new UT state
  State ttState = track.closestState( zUT );
  //State ttState = aState;

  // The vector of pointers to UT candidate hit-clusters
  UTCandidates candidates; 

  // loop over layers in UT and find which sector is hit
  for ( ; iLayer != ttLayers.end(); ++iLayer) {

    // For this UT layer: extrapolate the new State to the z of the layer
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
        Warning("TrajPoca minimize failed in addUTClusters", StatusCode::SUCCESS,0).ignore();
        if(msgLevel(MSG::DEBUG)) debug() << "TrajPoca minimize failed in addUTClusters" << endmsg;
      }
      int signDist = ( distance3D.x() > 0.0 ) ? 1 : -1 ;
      double distance = signDist * distance3D.R();
      
      if ( fabs( distance ) < m_utClusterCut ) {
        bool clusterUsed = false;
        unsigned int iCand = 0;
        unsigned int numCandidates = candidates.size();
        while ( iCand < numCandidates ) {
          UTCandidate* cand = candidates[iCand];
          if ( cand != 0 && cand->lastLayer() < layerNum ) {
            bool interStation = layerNum == 2 || 
              (layerNum == 3 && cand->lastLayer() != 2 );
            double delta = fabs(cand->lastDistance() - distance);
            if ( (interStation && delta < m_interStationCut) ||
                 (!interStation && delta < m_intraStationCut) ) {
              // if cluster compatible: make new set of clusters
              // using the old one and flag the old as dead.
              UTCandidate* newCand = new UTCandidate( cand, (*iClusTraj).first,
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
          UTCandidate* newCand = new UTCandidate( (*iClusTraj).first, distance,
                                                  layerNum,
                                                  ttState.stateVector() );
          candidates.push_back( newCand );
        }
      }
    } // loop iClusTraj

    // delete all dead candidates and those with too few hits
    UTCandidates::iterator iCand = candidates.begin();
    while ( iCand != candidates.end() ) {
      bool tooFewHits = int( (*iCand)->numUTClusters() - m_minUTHits + 
                             numUTLayers - layerNum ) < 1;
      if ( (*iCand)->isDead() || tooFewHits ) {
        delete *iCand;
        iCand = candidates.erase( iCand );
      } else ++iCand;
    }
  } // loop iLayer

  // find the best UT candidate
  std::vector<TrackVector> refVectors; refVectors.reserve(10);
  double bestQuality = 2.*m_utClusterCut;
  UTCandidates::const_iterator iCand = candidates.begin();
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
      debug() << "Added " << ttClusters.size() << " UT hits as LHCbIDs to the track." << endmsg; 
  }
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Get the distance to the strip
//=============================================================================
double AddUTClusterTool::distanceToStrip( const Track& track, 
                                          const STCluster& ttCluster ) 
{
  if ( !m_utClustersLoaded ) loadUTClusters();

  // Get the UT layer
  DeSTLayer* ttLayer = m_utTracker->findLayer( ttCluster.channelID() );
  double zLayer = (ttLayer->globalCentre()).z() ;

  // Make a new UT state
  const State& aState = track.closestState( zLayer );
  State ttState = aState;

  // For this UT layer: extrapolate the new State to the z of the layer
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
