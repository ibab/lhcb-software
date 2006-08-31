// $Id: TrackEventCloneKiller.cpp,v 1.8 2006-08-31 17:56:05 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// TrackEvent
#include "Event/Track.h"
#include "Event/TrackFunctor.h"

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
  m_tracksInContainers.push_back( LHCb::TrackLocation::KsTrack );

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
  std::vector<LHCb::Track*> allTracks;
  getAllInputTracks( allTracks );
  
  // Remove first all ancestors and/or clone tracks on this list
  // (takes into account the "StoreCloneTracks" property!)
  // ------------------------------------------------------------
  if ( !m_storeCloneTracks ) removeAncestorsAndClones( allTracks );
  
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
void TrackEventCloneKiller::getAllInputTracks( std::vector<LHCb::Track*>&
                                               allTracks )
{
  std::vector<std::string>::const_iterator itInCont =
    m_tracksInContainers.begin();

  while ( itInCont != m_tracksInContainers.end() ) {

    LHCb::Tracks* inTracks = get<LHCb::Tracks>( *itInCont );

    if ( m_debugLevel ) debug() << "# Tracks in " << *itInCont
                                << " = " << inTracks -> size() << endreq;

    allTracks.reserve( allTracks.size() + inTracks -> size() );
    LHCb::Tracks::const_iterator iTrack = inTracks -> begin();
    for( ; iTrack != inTracks -> end(); ++iTrack ) {
      if ( toBeUsed( *iTrack ) ) allTracks.push_back( *iTrack );
    }
    ++itInCont;
  }
  if ( m_debugLevel ) debug() << "-> total # of tracks retrieved = "
                              << allTracks.size() << endreq;
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
//  Remove ancestor and/or clone tracks from the list of input tracks
//=============================================================================
void
TrackEventCloneKiller::removeAncestorsAndClones( std::vector<LHCb::Track*>&
                                                 allTracks )
{
  // First remove all the clone tracks from the list
  // (only needed for couting in debug mode - all clones are removed hereafter)
  unsigned int nClonesRemoved = allTracks.size();
  if ( m_debugLevel ) {
    std::vector<LHCb::Track*>::iterator iCloneTracks =
      std::remove_if( allTracks.begin(), allTracks.end(),
                      TrackFunctor::HasKey<LHCb::Track,const LHCb::Track::Flags&>
                      (&LHCb::Track::checkFlag,LHCb::Track::Clone) );
    allTracks.erase( iCloneTracks, allTracks.end() );
    nClonesRemoved -= allTracks.size();
  }
  
  // Flag the ancestor tracks as clones!
  for( std::vector<LHCb::Track*>::iterator iTrack = allTracks.begin();
       iTrack != allTracks.end(); ++iTrack ) {
    SmartRefVector<LHCb::Track>& ancestors = (*iTrack) -> ancestors();
    for ( SmartRefVector<LHCb::Track>::iterator it = ancestors.begin();
          it != ancestors.end(); ++it ) {
      if ( std::find( allTracks.begin(), allTracks.end(), *it )
           != allTracks.end() ) {
        (*it) -> setFlag( LHCb::Track::Clone, true );
        if ( m_debugLevel )
          debug() << (*iTrack) -> parent() -> name() << " track with key "
                  << (*iTrack) -> key() << " has ancestor "
                  << (*it) -> parent() -> name() << " track with key "
                  << (*it) -> key() << " -> ancestor to be removed"
                  << endreq;
      }
    } // loop over ancestors
  } // loop over allTracks
  
  // Remove the ancestor tracks from list
  unsigned int nAncRemoved = allTracks.size();
  std::vector<LHCb::Track*>::iterator iAncTracks =
    std::remove_if( allTracks.begin(), allTracks.end(),
                    TrackFunctor::HasKey<LHCb::Track,const LHCb::Track::Flags&>
                    (&LHCb::Track::checkFlag,LHCb::Track::Clone) );
  allTracks.erase( iAncTracks, allTracks.end() );
  nAncRemoved -= allTracks.size();
  
  if ( m_debugLevel )
    debug() << "Removed " << nAncRemoved
            << " ancestor tracks and " << nClonesRemoved
            << " clone tracks from the list of input tracks"
            << endmsg
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
