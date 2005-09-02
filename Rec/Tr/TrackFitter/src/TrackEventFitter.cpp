// $Id: TrackEventFitter.cpp,v 1.2 2005-09-02 17:05:03 erodrigu Exp $
// Include files
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/TrackKeys.h"

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
                   m_tracksInContainer = TrackLocation::Default );
  declareProperty( "TracksOutContainer", 
                   m_tracksOutContainer = "Rec/Track/FitIdeal" );
  declareProperty( "FitterName"      , m_fitterName = "TrackKalmanFilter" );
  declareProperty( "FitUpstream"     , m_fitUpstream   = false );
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

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackEventFitter::execute() {

  debug() << "==> Execute" << endmsg;

  StatusCode sc = StatusCode::SUCCESS;

  debug() << "-> getting input Tracks container ..." << endmsg;
  // Retrieve the Tracks container
  // -----------------------------
  Tracks* tracksCont = get<Tracks>( m_tracksInContainer );

  debug() << "-> creating output Tracks container ..." << endmsg;
  // Make container for tracks
  // -------------------------
  Tracks* tracksNewCont = new Tracks();

  // Loop over the tracks and fit them
  // ---------------------------------
  Tracks::const_iterator iTrack = tracksCont -> begin();
  unsigned int nFitFail = 0;

  debug() << "-> starting loop over input Tracks ..." << endmsg;
  for ( iTrack; iTrack != tracksCont->end(); ++iTrack ) {

    // Make a new track keeping the same key
    Track& track = *( (*iTrack) -> cloneWithKey() );

    // check if it is needed to populate the track with measurements
    if ( track.checkFlag( TrackKeys::PatRecIDs ) ) {
      m_measProvider -> load();    
      sc = m_measProvider -> load( track );
      if ( sc.isFailure() )
        return Error( "Unable to load measurements!", StatusCode::FAILURE); 
    }

    // get the seed state
    if ( track.nStates() == 0 )
      return Error( "Track has no state! Can not fit.", StatusCode::FAILURE);

    State& seed = seedState( track );

    debug() << "#### Fitting Track # " << track.key() << " ####" << endreq;
    debug() << "# of states before fit:" << track.nStates() << endreq;
    debug() << "States at z-positions: ";
    const std::vector<State*>& allstates = track.states();
    for ( unsigned int it = 0; it < allstates.size(); it++ ) {
      debug() << allstates[it]->z() << " ";
    }
    debug() << endreq;

    if ( m_fitUpstream ) {  // fit upstream from last measurement
      //sc = m_tracksFitter -> fitReverse( track );
    } 
    else {                       // fit downstream from first measurement
      sc = m_tracksFitter -> fit( track, seed );
    }

    if ( sc.isSuccess() ) {
      debug() << "Fitted successfully track # " << track.key() << endreq;
      debug() << "# of states after fit:" << track.nStates() << endreq;
      debug() << "States are: ";
      const std::vector<State*>& allstates = track.states();
      for ( unsigned int it2 = 0; it2 < allstates.size(); it2++ ) {
        debug() << "-- at z = " << allstates[it2]->z() << endreq
                << "state vector:" << allstates[it2]->stateVector() << endreq
                << "covariance:" << allstates[it2]->covariance() << endreq;
      }
      debug() << endreq;
      track.setStatus( TrackKeys::Fitted );
      // Add the track to the new Tracks container
      // -----------------------------------------
      tracksNewCont -> add( &track );
    }
    else {
      debug() << "Unable to fit the track # " << track.key() << endreq;
      track.setFlag( TrackKeys::Invalid, true );
      track.setStatus( TrackKeys::FitFailed );
      nFitFail++;
    }

  } // loop over input Tracks
  
  if ( nFitFail == 0 )
    info() << "All tracks fitted succesfully." << endreq;
  else
    info() << "Fitted successfully " << (tracksCont->size()-nFitFail)
           << " out of " << tracksCont->size() << endreq;

  // Store the Tracks in the TES
  // ---------------------------
  sc = registerTracks( tracksNewCont );
  if( sc.isFailure() ) return sc;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackEventFitter::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
// Get a seed State from the Track
//=============================================================================
State& TrackEventFitter::seedState( Track& track )
{
  if ( m_fitUpstream )  // fit upstream
    return *( *(track.states().end()-1) );
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

  debug() << "Tracks container stored in the TES" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
