// $Id: TrackRemoveDoubleITHits.cpp,v 1.2 2007-09-20 15:14:40 lnicolas Exp $
//

//-----------------------------------------------------------------------------
// Implementation file for class : TrackRemoveDoubleITHits
//
// Louis Nicolas
// Started: 14.09.2007
///-----------------------------------------------------------------------------

//===========================================================================
// Includes
//===========================================================================
// Local
#include "TrackRemoveDoubleITHits.h"
//===========================================================================

//===========================================================================
// Declare
//===========================================================================
// Declaration of the algorithm factory
DECLARE_ALGORITHM_FACTORY ( TrackRemoveDoubleITHits );
//===========================================================================

//===========================================================================
// Constructor
//===========================================================================
TrackRemoveDoubleITHits::TrackRemoveDoubleITHits ( const std::string& name,
                                                   ISvcLocator* pSvcLocator ):
  GaudiAlgorithm ( name, pSvcLocator ),
  m_itTracker ( 0 ) {

  // Location of the IT geometries
  this -> declareProperty ( "ITGeometryPath",
                            m_itTrackerPath = DeSTDetLocation::IT );
  
  // Location of the tracks container
  this -> declareProperty ( "TracksLocation", m_tracksPath = "Rec/Track/Tsa" );

  // Location of the IT clusters container
  this -> declareProperty ( "ITLiteClustersLocation",
                            m_itLiteClustersPath =
                            LHCb::STLiteClusterLocation::ITClusters );
  this -> declareProperty ( "ITClustersLocation",
                            m_itClustersPath = LHCb::STClusterLocation::ITClusters );
  

  // Do we want to keep the strip with the highest charge of a double hit?
  // OR
  // Do we want to keep the strip with a high threshold of a double hit?
  this -> declareProperty ( "KeepHighCharge", m_keepHighCharge = false );
  this -> declareProperty ( "KeepHighThreshold", m_keepHighThreshold = false );
}
//===========================================================================


//===========================================================================
// Destructor
//===========================================================================
TrackRemoveDoubleITHits::~TrackRemoveDoubleITHits ( ) {
}
//===========================================================================


//===========================================================================
// Initialize
//===========================================================================
StatusCode TrackRemoveDoubleITHits::initialize ( ) {

  if ( msgLevel( MSG::DEBUG ) )
    debug() << "==> Initialize" << endmsg;

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return Error( "Failed to initialize" );

  // Get IT geometry
  m_itTracker = getDet<DeITDetector>( m_itTrackerPath );

  if ( m_keepHighThreshold && m_keepHighCharge ) {
    warning() << "Cannot keep strip with highest charge AND strip with high threshold!"
              << endmsg
              << "User has to set one of the two properties to false!"
              << endmsg
              << "Will remove both LHCbIDs of the track in case of an IT double hit!"
              << endmsg;
    m_keepHighThreshold = false;
    m_keepHighCharge = false;
  } else if ( m_keepHighThreshold )
    info() << "In case of a double IT hit, will remove one LHCbID from the track"
           << " if one of the two only has a high threshold." << endmsg;
  else if ( m_keepHighCharge )
    info() << "In case of a double IT hit, will remove from the track the LHCbID"
           << " of the strip that has the lowest charge." << endmsg;
  else
    info() << "In case of a double IT hit, will remove both LHCbIDs from the track."
           << endmsg;

  debug() << "TrackRemoveDoubleITHits initialized successfully" << endmsg;    

  return StatusCode::SUCCESS;
}
//===========================================================================


//===========================================================================
// Execute
//===========================================================================
StatusCode TrackRemoveDoubleITHits::execute ( ) {

  debug() << "TrackRemoveDoubleITHits starting execution" << endmsg;

  // Get the tracks
  m_tracks = get<LHCb::Tracks>( m_tracksPath );  

  if ( m_keepHighThreshold )
    m_itLiteClusters = get<STLiteClusters>( m_itLiteClustersPath );
  else if ( m_keepHighCharge )
    m_itClusters = get<LHCb::STCluster::Container>( m_itClustersPath );

  // Loop over tracks
  LHCb::Tracks::const_iterator iTracks = m_tracks->begin();
  for ( ; iTracks != m_tracks->end(); ++iTracks ) {
    LHCb::Track& aTrack = *(*iTracks);

    std::vector<LHCb::LHCbID> lhcbIDsToRemove;

    // Loop over the lhcbIDs to check if
    // there are two of the same track in the same IT ladder
    std::vector<LHCb::LHCbID>::const_iterator iLHCbIDs = aTrack.lhcbIDs().begin();
    for ( ; iLHCbIDs != aTrack.lhcbIDs().end(); ++iLHCbIDs ) {
      
      const LHCb::LHCbID& aLHCbID = *iLHCbIDs;
      
      // Only loop on hits in IT
      if ( !aLHCbID.isIT() ) continue;
      
      std::vector<LHCb::LHCbID>::const_iterator iLHCbIDs2 = iLHCbIDs+1;
      for ( ; iLHCbIDs2 != aTrack.lhcbIDs().end(); ++iLHCbIDs2 ) {
        
        const LHCb::LHCbID& aLHCbID2 = *iLHCbIDs2;
        
        // Only loop on hits in IT
        if ( !aLHCbID2.isIT() ) continue;
        
        //**********************************************************************
        // Doubled hits for IT
        // (splitted clusters at beetles boundaries or simulated dead strips)
        //**********************************************************************
        // Two hits in same ladder
        if ( aLHCbID.stID().uniqueSector() == aLHCbID2.stID().uniqueSector() ) {
          
          if ( m_keepHighThreshold ) {
            if ( isHighThreshold ( aLHCbID ) &&
                 !isHighThreshold ( aLHCbID2 ) ) {
              Warning("Found a double hit in IT. Removing from track LHCbID of strip with low threshold!!!",
                      StatusCode::SUCCESS, 1);
              lhcbIDsToRemove.push_back( aLHCbID2 );
            }
            else if ( isHighThreshold ( aLHCbID2 ) &&
                      !isHighThreshold ( aLHCbID ) ) {
              Warning("Found a double hit in IT. Removing from track LHCbID of strip with low threshold!!!",
                      StatusCode::SUCCESS, 1);
              lhcbIDsToRemove.push_back( aLHCbID );
            }
            else {
              Warning("Found a double hit in IT. Both high or low thresholds. Removing both LHCbIDs from track!!!",
                      StatusCode::SUCCESS, 1);
              lhcbIDsToRemove.push_back( aLHCbID );
              lhcbIDsToRemove.push_back( aLHCbID2 );
            }
          }
          else if ( m_keepHighCharge ) {
            if ( charge ( aLHCbID ) > charge ( aLHCbID2 ) ) {
              Warning("Found a double hit in IT. Removing from track LHCbID of strip with less charge!!!",
                      StatusCode::SUCCESS, 1);
              lhcbIDsToRemove.push_back( aLHCbID2 );
            }
            else if ( charge ( aLHCbID2 ) > charge ( aLHCbID ) ) {
              Warning("Found a double hit in IT. Removing from track LHCbID of strip with less charge!!!",
                      StatusCode::SUCCESS, 1);
              lhcbIDsToRemove.push_back( aLHCbID );
            }
            else {
              Warning("Found a double hit in IT. None with higher charge. Removing both LHCbIDs from track!!!",
                      StatusCode::SUCCESS, 1);
              lhcbIDsToRemove.push_back( aLHCbID );
              lhcbIDsToRemove.push_back( aLHCbID2 );
            }
          }
          else {
            Warning("Found a double hit in IT. Removing both LHCbIDs from track!!!",
                    StatusCode::SUCCESS, 1);
            lhcbIDsToRemove.push_back( aLHCbID );
            lhcbIDsToRemove.push_back( aLHCbID2 );
          }
          break;
        }
        //**********************************************************************
      }
    }

    std::vector<LHCb::LHCbID>::const_iterator rLHCbIDs = lhcbIDsToRemove.begin();
    for ( ; rLHCbIDs != lhcbIDsToRemove.end(); ++rLHCbIDs )
      aTrack.removeFromLhcbIDs( *rLHCbIDs );
  }
  
  return StatusCode::SUCCESS;  
}
//===========================================================================

  
//===========================================================================
// Finalize
//===========================================================================
StatusCode TrackRemoveDoubleITHits::finalize ( ) {
  
  if ( msgLevel( MSG::DEBUG ) )
    debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize ( );   // Must be called after all other actions
};
//===========================================================================


//===========================================================================
// Get the high threshold bit of the lite cluster associated to an LHCbID
//===========================================================================
bool TrackRemoveDoubleITHits::isHighThreshold ( const LHCb::LHCbID& theLHCbID ) {

  STLiteClusters::const_iterator iLiteClus =
    m_itLiteClusters->find<LHCb::STLiteCluster::findPolicy>( theLHCbID.stID() );

  if ( iLiteClus != m_itLiteClusters->end() )
    return (*iLiteClus).highThreshold();

  return false;
};
//===========================================================================


//===========================================================================
// Get the charge of the cluster associated to an LHCbID
//===========================================================================
double TrackRemoveDoubleITHits::charge ( const LHCb::LHCbID& theLHCbID ) {

  LHCb::STCluster* theClus =
    m_itClusters->object ( theLHCbID.stID() );

  if ( theClus != 0 )
    return theClus->totalCharge();

  return 0.0;
};
//===========================================================================
