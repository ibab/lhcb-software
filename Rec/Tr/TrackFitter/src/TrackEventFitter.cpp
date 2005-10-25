// $Id: TrackEventFitter.cpp,v 1.3 2005-10-25 12:50:05 erodrigu Exp $
// Include files
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// from TrackEvent
#include "Event/Track.h"

// local
#include "TrackEventFitter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackEventFitter
//
// 2005-05-30 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<TrackEventFitter>          s_factory ;
const        IAlgFactory& TrackEventFitterFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackEventFitter::TrackEventFitter( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_tracksFitter(0)
  , m_measProvider(0)
{
  declareProperty( "TracksInContainer", 
                   m_tracksInContainer = "Rec/Track/Ideal" );
  declareProperty( "TracksOutContainer", 
                   m_tracksOutContainer = "Rec/Track/FitIdeal" );
  declareProperty( "FitterName"      , m_fitterName = "TrackKalmanFilter" );
  declareProperty( "FitUpstream"     , m_fitUpstream   = true );
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

  m_measProvider = tool<IMeasurementProvider>( "MeasurementProvider",
                                               "MeasProvider", this );

  // Print out the user-defined settings
  // -----------------------------------
  std::string fitType;
  if ( m_fitUpstream ) fitType = "upstream";
  else                 fitType = "downstream";

  info()
    << " " << endreq
    << "================ TrackEventFitter Settings ================"
    << endreq
    << "  Tracks input container   : " << m_tracksInContainer << endreq
    << "  Tracks output container  : " << m_tracksOutContainer << endreq
    << "  Fitter name              : " << m_fitterName << endreq
    << "  Fit type                 : " << fitType << endreq
    << "==========================================================="
    << endreq
    << " " << endreq;

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
  Tracks* tracksNewCont = new Tracks();

  // Loop over the tracks and fit them
  // ---------------------------------
  unsigned int nFitFail = 0;

  for ( Tracks::const_iterator iTrack = tracksCont -> begin();
        iTrack != tracksCont->end();
        ++iTrack ) {

    // Make a new track keeping the same key
    Track& track = *( (*iTrack) -> cloneWithKey() );

    // Check if it is needed to populate the track with measurements
    if ( track.checkFlag( Track::PatRecIDs ) ) {
      m_measProvider -> load();    
      sc = m_measProvider -> load( track );
      if ( sc.isFailure() )
        return Error( "Unable to load measurements!", StatusCode::FAILURE );
    }

    // Get the seed state
    if ( track.nStates() == 0 )
      return Error( "Track has no state! Can not fit.", StatusCode::FAILURE );

    State& seed = seedState( track );

    // Check that the number of measurements is enough
    unsigned int nMeas = track.nMeasurements();
    if ( nMeas < seed.nParameters() ) {
      error() << "Track has only " << nMeas
              << " measurements. Not enough to fit a "
              << seed.nParameters() << "D-state!" << endreq;
      return StatusCode::FAILURE;
    }

    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << "#### Fitting Track # " << track.key() << " ####" << endreq
              << "  # of states before fit:" << track.nStates() << endreq
              << "  States at z-positions: ";
      const std::vector<State*>& allstates = track.states();
      for ( unsigned int it = 0; it < allstates.size(); it++ ) {
        debug() << allstates[it]->z() << " ";
      }
      debug() << endreq;
    }

    if ( m_fitUpstream ) {  // fit upstream from last measurement
      sc = m_tracksFitter -> fitUpstream( track, seed );
    } 
    else {                  // fit downstream from first measurement
      sc = m_tracksFitter -> fitDownstream( track, seed );
    }

    if ( sc.isSuccess() ) {
      track.setStatus( Track::Fitted );
      // Add the track to the new Tracks container
      // -----------------------------------------
      tracksNewCont -> add( &track );
    }
    else {
      track.setFlag( Track::Invalid, true );
      track.setStatus( Track::FitFailed );
      nFitFail++;
      debug() << "Unable to fit the track # " << track.key() << endreq;
    }

    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << "Fitted successfully track # " << track.key() << endreq
              << "  # of states after fit:" << track.nStates() << endreq
              << "  States are: ";
      const std::vector<State*>& allstates = track.states();
      for ( unsigned int it2 = 0; it2 < allstates.size(); it2++ ) {
        debug() << "-- at z = " << allstates[it2]->z() << endreq
                << "state vector:" << allstates[it2]->stateVector() << endreq
                << "covariance:" << allstates[it2]->covariance() << endreq;
      }
      debug() << endreq;
    }

  } // loop over input Tracks

  // Update counters
  // ---------------
  unsigned int nTracks = tracksCont -> size();
  m_nTracks       += nTracks;
  m_nFittedTracks += ( nTracks - nFitFail );

  if ( msgLevel( MSG::DEBUG ) ) {
    if ( nFitFail == 0 )
      debug() << "All " << nTracks << " tracks fitted succesfully." << endreq;
    else
      debug() << "Fitted successfully " << (nTracks-nFitFail)
              << " out of " << nTracks << endreq;
  }

  // Store the Tracks in the TES
  // ---------------------------
  return registerTracks( tracksNewCont );
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
    << " " << endreq
    << "====================== TrackEventFitter Summary ======================"
    << endreq
    << "  Fitting performance   : "
    << format( " %7.2f %%", perf ) << endreq
    << "                            ("
    << format( "%7d / %6d tracks )",
               m_nFittedTracks, m_nTracks ) << endreq
    << "======================================================================"
    << endreq;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
// Get a seed State from the Track
//=============================================================================
State& TrackEventFitter::seedState( Track& track )
{
  if ( m_fitUpstream )  // fit upstream
    //return *( *(track.states().end()-1) );
    return *track.states().back();
  else                  // fit downstream
    return track.firstState();
}

//=============================================================================
// Register the tracks container in the TES
//=============================================================================
StatusCode TrackEventFitter::registerTracks( Tracks* tracksCont )
{
  StatusCode sc = put( tracksCont, m_tracksOutContainer );

  if ( sc.isFailure() )
    error() << "Unable to register the output container at "
            << m_tracksOutContainer << ". Status is " << sc << endreq;

  return sc;
}

//=============================================================================
