// $Id: TrackEventCloneKiller.cpp,v 1.5 2006-08-14 15:21:19 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// TrackEvent
#include "Event/Track.h"

// local
#include "TrackEventCloneKiller.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackEventCloneKiller
//
// 2006-03-01 : Eduardo Rodrigues
// Based on the clone killer algorithm of Rutger van der Eijk (2002-06-17)
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<TrackEventCloneKiller>          s_factory ;
const        IAlgFactory& TrackEventCloneKillerFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackEventCloneKiller::TrackEventCloneKiller( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  // default list of input containers
  m_tracksInContainers.push_back( LHCb::TrackLocation::VeloTT  );
  m_tracksInContainers.push_back( LHCb::TrackLocation::Forward );
  m_tracksInContainers.push_back( LHCb::TrackLocation::Match   );
  m_tracksInContainers.push_back( LHCb::TrackLocation::Tsa     );

  declareProperty( "TracksInContainers",
                   m_tracksInContainers );
  declareProperty( "TracksOutContainer", 
                   m_tracksOutContainer = LHCb::TrackLocation::Default );
  declareProperty( "IgnoredTrackTypes",
                   m_ignoredTrackTypes );
  declareProperty( "StoreCloneTracks",
                   m_storeCloneTracks = false );
  declareProperty( "SkipSameContainerTracks",
                   m_skipSameContainerTracks = true );
  declareProperty( "CloneFinderTool",
                   m_cloneFinderName = "TrackCloneFinder" );
}

//=============================================================================
// Destructor
//=============================================================================
TrackEventCloneKiller::~TrackEventCloneKiller() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackEventCloneKiller::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  debug() << "==> Initialize" << endmsg;
  
  m_debugLevel = msgLevel( MSG::DEBUG );
  
  // Retrieve the clone finder tool
  // ------------------------------
  m_cloneFinder = tool<ITrackCloneFinder>( m_cloneFinderName,
                                           "CloneFinderTool", this );
  
  // Print out the user-defined settings
  // -----------------------------------
  printProps( MSG::INFO );
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackEventCloneKiller::execute() {
  
  if ( m_debugLevel ) debug() << "==> Execute" << endmsg;

  // Put all the input tracks into a temporary vector of pointers
  // ------------------------------------------------------------
  std::vector<LHCb::Track*> allTracks = getAllInputTracks();
  
  // Remove first all ancestors of tracks on this list!
  // --------------------------------------------------
  removeAncestors( allTracks );
  
  // Find the clones and flag them
  // -----------------------------
  unsigned int nClonesFlagged = 0;
  bool cloneFound = false;

  for ( unsigned int i1 = 0; i1 < allTracks.size(); ++i1 ) {
    for ( unsigned int i2 = i1+1; i2 < allTracks.size(); ++i2 ) {

      if ( allTracks[i1] -> checkFlag( LHCb::Track::Clone ) ||
           allTracks[i2] -> checkFlag( LHCb::Track::Clone ) ) continue;
      if ( m_skipSameContainerTracks &&
           allTracks[i1]->parent()->name() == allTracks[i2]->parent()->name() )
        continue;
      
      // clones are flagged by the tool!
      cloneFound = m_cloneFinder -> areClones( *allTracks[i1], *allTracks[i2],
                                               true );
      if ( cloneFound ) {
        ++nClonesFlagged;
        if ( m_debugLevel ) {
          debug() << "-> tracks " << allTracks[i1] -> key()
                  << " in " << allTracks[i1] -> parent() -> name() << " and "
                  << allTracks[i2] -> key()
                  << " in " << allTracks[i2] -> parent() -> name()
                  << " are clones" << endreq;
        }
      } // if ( cloneFound )
    } // loop i2 over tracks
  } // loop i1 over tracks

  if ( m_debugLevel )
    debug() << "Flagged " << nClonesFlagged
            << " tracks as clones out of " << allTracks.size() << endreq;

  // Make the output container
  // -------------------------
  LHCb::Tracks* tracksOutCont = new LHCb::Tracks();

  // Fill the output container
  // -------------------------
  unsigned int nUnique   = 0;
  unsigned int nFiltered = 0;
  for ( unsigned int i = 0; i < allTracks.size(); ++i ) {
    if ( !allTracks[i] -> checkFlag( LHCb::Track::Clone ) ) ++nUnique;
    bool toStore = true;
    for ( unsigned int k = 0 ; m_ignoredTrackTypes.size() > k ; ++k ) {
      if ( m_ignoredTrackTypes[k] == allTracks[i] -> type() ) toStore = false;
    }
    if ( toStore &&
         ( m_storeCloneTracks ||
           !allTracks[i] -> checkFlag( LHCb::Track::Clone ) ) ) {
      tracksOutCont -> add( allTracks[i] -> clone() );
    }
    else {
      ++nFiltered;
    }
  }

  if ( m_debugLevel )
    debug() << "Stored " << tracksOutCont -> size() 
            << " tracks, identified "
            << ( allTracks.size()  - nUnique ) << " clones of which " 
            << nFiltered << " were not stored."
            << endreq;
  
  // Store the tracks in the TES
  // ---------------------------
  put( tracksOutCont, m_tracksOutContainer );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Retrieve the input tracks from all the user-specified containers
// Note: are only taken into account Valid and Fitted tracks!
//=============================================================================
std::vector<LHCb::Track*> TrackEventCloneKiller::getAllInputTracks()
{
  std::vector<LHCb::Track*> allTracks;
 
  std::vector<std::string>::const_iterator itInCont =
    m_tracksInContainers.begin();

  while ( itInCont != m_tracksInContainers.end() ) {

    LHCb::Tracks* inTracks = get<LHCb::Tracks>( *itInCont );
    if ( m_debugLevel ) debug() << "# Tracks in " << *itInCont
                                << " = " << inTracks -> size() << endreq;

    LHCb::Tracks::const_iterator iTrack = inTracks -> begin();
    for( ; iTrack != inTracks -> end(); ++iTrack ) {
      if ( toBeUsed( *iTrack ) ) allTracks.push_back( *iTrack );
    }
    ++itInCont;
  }

  if ( m_debugLevel ) debug() << "-> total # of tracks retrieved = "
                              << allTracks.size() << endreq;

  return allTracks;
};

//=============================================================================
//  Include this track in the list of input tracks to be considered
//=============================================================================
bool TrackEventCloneKiller::toBeUsed( const LHCb::Track* track )
{
  // discard invalid tracks
  if ( track -> checkFlag( LHCb::Track::Invalid ) ) return false;
  
  return true;
}

//=============================================================================
//  Remove ancestor tracks from the list of input tracks to be considered
//=============================================================================
void
TrackEventCloneKiller::removeAncestors( std::vector<LHCb::Track*>& allTracks )
{
  unsigned int nAncestors = 0;
  for( std::vector<LHCb::Track*>::iterator iTrack = allTracks.begin();
       iTrack != allTracks.end(); ++iTrack ) {
    // Check for ancestors and remove them from the vector of input tracks!
    SmartRefVector<LHCb::Track>& ancestors = (*iTrack) -> ancestors();
    for ( SmartRefVector<LHCb::Track>::iterator it = ancestors.begin();
          it != ancestors.end(); ++it ) {
      std::vector<LHCb::Track*>::iterator iAncTrack =
        std::remove( allTracks.begin(), allTracks.end(), *it );
      if ( iAncTrack != allTracks.end() ) {
        ++nAncestors;
        (*it) -> setFlag( LHCb::Track::Clone, true );
        allTracks.erase( iAncTrack, allTracks.end() );
        if ( m_debugLevel )
          debug() << (*iTrack) -> parent() -> name() << " track with key "
                  << (*iTrack) -> key() << " has ancestor "
                  << (*it) -> parent() -> name() << " track with key "
                  << (*it) -> key() << " -> ancestor removed"
                  << endreq;
      }
    } // loop over ancestors
  } // loop over allTracks
  
  debug() << "Removed " << nAncestors
          << " ancestor tracks from the list of input tracks" << endmsg
          << "-> total # of tracks to be considered = "
          << allTracks.size() << endreq;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackEventCloneKiller::finalize()
{
  debug() << "==> Finalize" << endmsg;
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
};

//=============================================================================
