// Include files

// local
#include "TrackUseCaloMomentumAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackUseCaloMomentumAlg
//
// 2008-05-27 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackUseCaloMomentumAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackUseCaloMomentumAlg::TrackUseCaloMomentumAlg( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputTracks",
                   m_inTracksLoc  = LHCb::TrackLocation::Default );
  declareProperty( "OutputTracks",
                   m_outTracksLoc = LHCb::TrackLocation::Default+"WithCALOInfo" );
}

//=============================================================================
// Destructor
//=============================================================================
TrackUseCaloMomentumAlg::~TrackUseCaloMomentumAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackUseCaloMomentumAlg::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  // Get the Track -> CALO tool
  m_trackenergy = tool<ITrackCaloMatch>( "TrackCaloMatch" );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackUseCaloMomentumAlg::execute()
{

  // Load input tracks
  const auto * in_tracks = get<LHCb::Tracks>( m_inTracksLoc );
  debug() << "Found " << in_tracks->size() << " Tracks at '"
          << m_inTracksLoc << "'" << endmsg;

  // Create output tracks
  auto * out_tracks = new LHCb::Tracks();
  put( out_tracks, m_outTracksLoc );

  // Loop over tracks and clone those with CALO info
  for ( const auto tk : *in_tracks )
  {
    // Get CALO info ?
    const auto caloEn = m_trackenergy->energy( *tk );
    if ( caloEn > 0 )
    {
      // Found CALO
      verbose() << " -> Track " << tk->key()
                << " has CALO energy " << caloEn << endmsg;

      // Clone track and save in output container
      auto * newTk = tk->clone();
      out_tracks->insert( newTk, tk->key() ); 

      // Update momentum values in all attached states
      for ( const auto state : newTk->states() )
      {
        verbose() << "  -> Updating State at z = " << state->z()
                  << " : Ptot " << state->p() << " -> ";
        // CRJ : ??? Need to correct energy to momentum using pion mass ???
        state->setQOverP( state->qOverP() * state->p() / caloEn );
        verbose() << state->p() << endmsg;
      }

    } // Track has CALO energy

  } // loop over input tracks

  debug() << "Created " << out_tracks->size() << " Tracks at '"
          << m_outTracksLoc << "'" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
