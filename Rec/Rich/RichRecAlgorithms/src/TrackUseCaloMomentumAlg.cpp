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
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_trackenergy  ( NULL )
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
  const LHCb::Tracks * in_tracks = get<LHCb::Tracks>( m_inTracksLoc );
  debug() << "Found " << in_tracks->size() << " Tracks at '"
          << m_inTracksLoc << "'" << endmsg;

  // Create output tracks
  LHCb::Tracks * out_tracks = new LHCb::Tracks();
  put( out_tracks, m_outTracksLoc );

  // Loop over tracks and clone those with CALO info
  for ( LHCb::Tracks::const_iterator iT = in_tracks->begin();
        iT != in_tracks->end(); ++iT )
  {
    // Get CALO info ?
    const double caloEn = m_trackenergy->energy( **iT );
    if ( caloEn > 0 )
    {
      // Found CALO
      verbose() << " -> Track " << (*iT)->key()
                << " has CALO energy " << caloEn << endmsg;

      // Clone track and save in output container
      LHCb::Track * newTk = (*iT)->clone();
      out_tracks->insert( newTk, (*iT)->key() ); 

      // Update momentum values in all attached states
      const LHCb::Track::StateContainer & states = newTk->states();
      for ( LHCb::Track::StateContainer::const_iterator iS = states.begin();
            iS != states.end(); ++iS )
      {
        verbose() << "  -> Updating State at z = " << (*iS)->z()
                  << " : Ptot " << (*iS)->p() << " -> ";
        // CRJ : ??? Need to correct energy to momentum using pion mass ???
        (*iS)->setQOverP( (*iS)->qOverP() * (*iS)->p() / caloEn );
        verbose() << (*iS)->p() << endmsg;
      }

    } // Track has CALO energy

  } // loop over input tracks

  debug() << "Created " << out_tracks->size() << " Tracks at '"
          << m_outTracksLoc << "'" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
