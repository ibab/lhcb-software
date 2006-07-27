// $Id: TrackEventFitter.cpp,v 1.10 2006-07-27 06:35:47 cattanem Exp $
// Include files
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from TrackEvent
#include "Event/Track.h"

// local
#include "TrackEventFitter.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TrackEventFitter
//
// 2005-05-30 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackEventFitter );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackEventFitter::TrackEventFitter( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_tracksFitter(0)
  , m_makeNewContainer(true)
{
  declareProperty( "TracksInContainer", 
                   m_tracksInContainer  = TrackLocation::Default );
  declareProperty( "TracksOutContainer", 
                   m_tracksOutContainer = ""                  );
  declareProperty( "Fitter",
                   m_fitterName         = "TrackMasterFitter" );
}

//=============================================================================
// Destructor
//=============================================================================
TrackEventFitter::~TrackEventFitter() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackEventFitter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_tracksFitter = tool<ITrackFitter>( m_fitterName, "Fitter", this );

  if ( m_tracksOutContainer == "" ) {
    m_tracksOutContainer = m_tracksInContainer;
    m_makeNewContainer   = false;
  }

  // Print out the user-defined settings
  // -----------------------------------
  info()
    << " " << endmsg
    << "=========== TrackEventFitter Settings ============"
    << endmsg
    << "  Tracks input container   : " << m_tracksInContainer << endmsg
    << "  Tracks output container  : " << m_tracksOutContainer << endmsg
    << "  Fitter name              : " << m_fitterName << endmsg
    << "=================================================="
    << endmsg
    << " " << endmsg;


  // Initialize global counters
  // --------------------------
  m_nTracks       = 0;
  m_nFittedTracks = 0;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackEventFitter::execute() {

  debug() << "==> Execute" << endmsg;

  StatusCode sc = StatusCode::SUCCESS;

  // Retrieve the Tracks container
  // -----------------------------
  Tracks* tracksCont = get<Tracks>( m_tracksInContainer );

  // Make container for tracks
  // -------------------------
  Tracks* tracksNewCont = 0;
  if ( m_makeNewContainer ) tracksNewCont = new Tracks();

  // Loop over the tracks and fit them
  // ---------------------------------
  unsigned int nFitFail = 0;
  Tracks::const_iterator iTrack = tracksCont -> begin();
  for ( ; iTrack != tracksCont->end(); ++iTrack ) {

    // If needed make a new track keeping the same key
    Track& track = ( m_makeNewContainer ) ?
      *( (*iTrack) -> cloneWithKey() ) :  *(*iTrack);

    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << "#### Fitting Track # " << track.key() << " ####" << endmsg
              << "  # of states before fit:" << track.nStates() << endmsg
              << "  States are: " << endreq;
      const std::vector<State*>& allstates = track.states();
      for ( unsigned int it = 0; it < allstates.size(); it++ ) {
        debug() << "  - z = " << allstates[it]->z() << endreq
                << "  - stateVector = "
                << allstates[it]->stateVector() << endreq
                << "  - covariance = " << endreq
                << allstates[it]->covariance() << endreq;
      }
    }

    sc = m_tracksFitter -> fit( track );

    if ( sc.isSuccess() ) {
      track.setFitStatus( Track::Fitted );
      // Add the track to the new Tracks container
      // -----------------------------------------
      if ( m_makeNewContainer ) tracksNewCont -> add( &track );
      if ( msgLevel( MSG::DEBUG ) ) {
        debug() << "Fitted successfully track # " << track.key() << endmsg
                << "  # of states after fit:" << track.nStates() << endmsg;
      }
    }
    else {
      track.setFlag( Track::Invalid, true );
      track.setFitStatus( Track::FitFailed );
      ++nFitFail;
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Unable to fit the track # " << track.key() << endmsg;
    }
  } // loop over input Tracks

  // Update counters
  // ---------------
  unsigned int nTracks = tracksCont -> size();
  m_nTracks       += nTracks;
  m_nFittedTracks += ( nTracks - nFitFail );

  if ( msgLevel( MSG::DEBUG ) ) {
    if ( nFitFail == 0 )
      debug() << "All " << nTracks << " tracks fitted succesfully." << endmsg;
    else
      debug() << "Fitted successfully " << (nTracks-nFitFail)
              << " out of " << nTracks << endmsg;
  }

  // Store the Tracks in the TES
  // ---------------------------
  if ( m_makeNewContainer )
    put( tracksNewCont, m_tracksOutContainer );
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackEventFitter::finalize() {

  debug() << "==> Finalize" << endmsg;

  float perf = 0.;
  if ( m_nTracks > 0 )
    perf = float(m_nFittedTracks) / float(m_nTracks) * 100.;

  info()
    << " " << endmsg
    << "====================== TrackEventFitter Summary ======================"
    << endmsg
    << "  Fitting performance   : "
    << format( " %7.2f %%", perf ) << endmsg
    << "                            ("
    << format( "%7d / %6d tracks )",
               m_nFittedTracks, m_nTracks ) << endmsg
    << "======================================================================"
    << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
