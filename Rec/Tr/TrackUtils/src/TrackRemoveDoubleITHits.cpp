// $Id: TrackRemoveDoubleITHits.cpp,v 1.1 2007-09-14 14:19:10 lnicolas Exp $
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
  
  // Location of the different objects in use
  this -> declareProperty ( "TracksLocation", m_tracksPath = "Rec/Track/Tsa" );
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

  // Loop over tracks - select some and make some plots
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
        // Doubled hits for IT (splitted clusters at beetles boundaries)
        //**********************************************************************
        // Two hits in same ladder
        if ( aLHCbID.stID().uniqueSector() == aLHCbID2.stID().uniqueSector() ) {
          Warning("Found a double hit in IT. Removing both LHCbIDs from track!!!",
                  StatusCode::SUCCESS, 1);
          lhcbIDsToRemove.push_back( aLHCbID );
          lhcbIDsToRemove.push_back( aLHCbID2 );
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
