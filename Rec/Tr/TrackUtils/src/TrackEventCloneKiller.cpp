// $Id: TrackEventCloneKiller.cpp,v 1.11 2008-05-06 10:01:14 aperiean Exp $
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
// 2008-04-15 : Adrian Perieanu
// Update for speed and clone rate
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackEventCloneKiller );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackEventCloneKiller::TrackEventCloneKiller( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  // default list of input containers
  m_tracksInContainers.push_back( LHCb::TrackLocation::Forward    );
  m_tracksInContainers.push_back( LHCb::TrackLocation::Match      );
  m_tracksInContainers.push_back( LHCb::TrackLocation::VeloTT     );
  m_tracksInContainers.push_back( LHCb::TrackLocation::Downstream );
  m_tracksInContainers.push_back( LHCb::TrackLocation::Tsa        );

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
  declareProperty( "CompareInSameContainerForwardUpstream",
                   m_compareInSameContainerForwardUpstream = false);
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
  getInputTracks( allTracks);
  
  // Find the clones and flag them
  // -----------------------------
  for( unsigned int i1 = 0; i1 < allTracks.size(); ++i1){

    // second loop over input tracks
    for( unsigned int i2 = i1+1; i2 < allTracks.size(); ++i2){

      if( ( allTracks[i1]->checkFlag( LHCb::Track::Clone) &&   
	    (allTracks[i1]->history() != LHCb::Track::PatForward)) ||   
	  allTracks[i2]->checkFlag( LHCb::Track::Clone))continue;  
      
      if( m_skipSameContainerTracks &&
	  !m_compareInSameContainerForwardUpstream){
	if( allTracks[i1]->parent()->name() == 
	    allTracks[i2]->parent()->name())continue;
      }else if( m_skipSameContainerTracks &&
		m_compareInSameContainerForwardUpstream){
	if( ( allTracks[i1]->parent()->name() == 
	      allTracks[i2]->parent()->name()) &&
	    ( !( allTracks[i1]->type() == LHCb::Track::Upstream &&
		 allTracks[i2]->type() == LHCb::Track::Upstream)) &&
	    ( !( allTracks[i1]->history() == LHCb::Track::PatForward &&
		 allTracks[i2]->history() == LHCb::Track::PatForward))
	    )continue;
      }
      
      // clones are flagged by the tool
      m_cloneFinder->areClones( *allTracks[i1], *allTracks[i2],
				true);
    } // loop i2 over tracks
  } // loop i1 over tracks

  // Make the output container
  // -------------------------
  LHCb::Tracks* tracksOutCont = new LHCb::Tracks();
  tracksOutCont->reserve( allTracks.size());

  // Fill the output container
  // -------------------------
  for( unsigned int i = 0; i < allTracks.size(); ++i){
    bool toStore = true;
    for( unsigned int k = 0 ; k < m_ignoredTrackTypes.size(); ++k){
      if( m_ignoredTrackTypes[k] == allTracks[i]->type()) toStore = false;
    }
    if( toStore &&
	( m_storeCloneTracks ||
	  !allTracks[i]->checkFlag( LHCb::Track::Clone))){
      tracksOutCont->add( allTracks[i]->clone());
    }
  }
  // Store the tracks in the TES
  // ---------------------------
  put( tracksOutCont, m_tracksOutContainer);

  if( m_debugLevel){
    unsigned int nUnique   = 0;
    unsigned int nFiltered = 0;
    for( unsigned int i = 0; i < allTracks.size(); ++i){
      if( !allTracks[i]->checkFlag( LHCb::Track::Clone)) ++nUnique;
      bool toStore = true;
      for ( unsigned int k = 0 ; m_ignoredTrackTypes.size() > k ; ++k ) {
	if ( m_ignoredTrackTypes[k] == allTracks[i]->type()) toStore = false;
      }
      if(!( toStore &&
	    ( m_storeCloneTracks ||
	      !allTracks[i] -> checkFlag( LHCb::Track::Clone)))){
	++nFiltered;
      }
    }
    debug() << "Stored " << tracksOutCont -> size() 
            << " tracks, identified "
            << ( allTracks.size()  - nUnique ) << " clones of which " 
            << nFiltered << " were not stored."
            << endreq;
  }

  return StatusCode::SUCCESS;
};
//=============================================================================
// Retrieve the input tracks from all the user-specified containers
// Note: are only taken into account Valid and Fitted tracks!
//=============================================================================
void TrackEventCloneKiller::getInputTracks( std::vector<LHCb::Track*>&
					    allTracks)
{
  std::vector<std::string>::const_iterator 
    itInCont = m_tracksInContainers.begin();

  LHCb::Tracks::const_iterator iTrack;
  SmartRefVector<LHCb::Track>::iterator it;
  while( itInCont != m_tracksInContainers.end()){

    LHCb::Tracks* inTracks = get<LHCb::Tracks>( *itInCont);

    if( m_debugLevel) debug() << "# Tracks in " << *itInCont
			      << " = " << inTracks -> size() << endreq;

    allTracks.reserve( allTracks.size() + inTracks->size());
    // loop over container
    for( iTrack = inTracks->begin(); iTrack != inTracks->end(); ++iTrack){
      // label ancestors as clones 
      if( *itInCont != "LHCb::TrackLocation::Tsa"){
 	SmartRefVector<LHCb::Track>& ancestors = (*iTrack)->ancestors();
	for ( it = ancestors.begin(); it != ancestors.end(); ++it){
	  if( (*it)->checkFlag( LHCb::Track::Clone))continue;
	  (*it)->setFlag( LHCb::Track::Clone, true);
	}
      }
      if( !m_storeCloneTracks &&
	  (*iTrack)->checkFlag( LHCb::Track::Clone))continue;
      
      if( !(*iTrack)->checkFlag( LHCb::Track::Invalid))
	allTracks.push_back( *iTrack);
    }// end loop over container
    ++itInCont;
  }
  if ( m_debugLevel ) debug() << "-> total # of tracks retrieved = "
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
