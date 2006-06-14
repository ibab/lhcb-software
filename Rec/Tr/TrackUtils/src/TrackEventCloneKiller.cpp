// $Id: TrackEventCloneKiller.cpp,v 1.3 2006-06-14 21:15:46 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// TrackEvent
#include "Event/Track.h"

// local
#include "TrackEventCloneKiller.h"

using namespace LHCb;

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
  m_tracksInContainers.push_back( TrackLocation::Seed );
  m_tracksInContainers.push_back( TrackLocation::Velo );
  m_tracksInContainers.push_back( TrackLocation::Forward );
  m_tracksInContainers.push_back( TrackLocation::VeloTT );
  m_tracksInContainers.push_back( TrackLocation::Match );
  m_tracksInContainers.push_back( TrackLocation::KsTrack );

  declareProperty( "TracksInContainers",
                   m_tracksInContainers );
  declareProperty( "TracksOutContainer", 
                   m_tracksOutContainer = TrackLocation::Default );
  declareProperty( "IgnoredTrackTypes",
                   m_ignoredTrackTypes );
  declareProperty( "StoreCloneTracks",
                   m_storeCloneTracks = false );
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
StatusCode TrackEventCloneKiller::initialize() {
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

  debug() << "==> Execute" << endmsg;

  // Put all the input tracks into a temporary vector of pointers
  // ------------------------------------------------------------
  std::vector<Track*> allTracks = getAllInputTracks();

  // Find the clones and flag them
  // -----------------------------
  unsigned int nClonesFlagged = 0;
  bool cloneFound = false;
  for ( unsigned int i1 = 0; i1 < allTracks.size(); ++i1 ) {
    for ( unsigned int i2 = i1+1; i2 < allTracks.size(); ++i2 ) {
      if ( allTracks[i1] -> checkFlag( Track::Clone ) ||
           allTracks[i2] -> checkFlag( Track::Clone ) ) continue;
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
      }
    }
  }

  if ( m_debugLevel )
    debug() << "Flagged " << nClonesFlagged
            << " tracks out of " << allTracks.size() << endreq;

  // Make the output container
  // -------------------------
  Tracks* tracksOutCont = new Tracks();

  // Fill the output container
  // -------------------------
  unsigned int nUnique   = 0;
  unsigned int nFiltered = 0;
  for ( unsigned int i = 0; i < allTracks.size(); ++i ) {
    if ( !allTracks[i] -> checkFlag( Track::Clone ) ) ++nUnique;
    bool toStore = true;
    for ( unsigned int k = 0 ; m_ignoredTrackTypes.size() > k ; ++k ) {
      if ( m_ignoredTrackTypes[k] == allTracks[i] -> type() ) toStore = false;
    }
    if ( toStore &&
         ( m_storeCloneTracks ||
           !allTracks[i] -> checkFlag( Track::Clone ) ) ) {
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
std::vector<Track*> TrackEventCloneKiller::getAllInputTracks()
{
  std::vector<Track*> allTracks;
 
  std::vector<std::string>::const_iterator itInCont =
    m_tracksInContainers.begin();

  while ( itInCont != m_tracksInContainers.end() ) {

    Tracks* inTracks = get<Tracks>( *itInCont );
    if ( m_debugLevel ) debug() << "# Tracks in " << *itInCont
                                << " = " << inTracks -> size() << endreq;

    Tracks::const_iterator iTrack = inTracks -> begin();
    for( ; iTrack != inTracks -> end(); ++iTrack ) {

      if ( !(*iTrack) -> checkFlag( Track::Invalid ) )
        allTracks.push_back( *iTrack );
    }
    ++itInCont;
  }

  if ( m_debugLevel ) debug() << "-> total # of Tracks retrieved = "
                              << allTracks.size() << endreq;

  return allTracks;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackEventCloneKiller::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
