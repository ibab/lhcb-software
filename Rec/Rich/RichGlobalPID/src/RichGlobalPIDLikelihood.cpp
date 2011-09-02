
//--------------------------------------------------------------------------
/** @file RichGlobalPIDLikelihood.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::Likelihood
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichGlobalPIDLikelihood.h"

// namespaces
using namespace Rich::Rec::GlobalPID;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Likelihood )

// Standard constructor, initializes variables
Likelihood::Likelihood( const std::string& name,
                        ISvcLocator* pSvcLocator )
  : AlgBase   ( name, pSvcLocator ),
    m_pidTool ( NULL              )
{
  declareProperty( "SingleTrackMode", m_oneAtATime = false );
}

// Destructor
Likelihood::~Likelihood() {}

//  Initialize
StatusCode Likelihood::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire tools
  acquireTool( "GPIDLikelihoodTool", m_pidTool, this );

  // pre-cache tools
  statusCreator();
  photonCreator();

  if ( m_oneAtATime ) info() << "Will operate in single track 'local' mode" << endmsg;

  return sc;
}

// Main execution
StatusCode Likelihood::execute()
{

  // ------------------------------ General RICH reco stuff -------------------------------------

  // Update RichRecEvent pointers
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // how many tracks ?
  if ( gpidTracks()->empty() ) return StatusCode::SUCCESS;

  // Reconstruct all photons
  if ( !richPhotons() ) return StatusCode::FAILURE;
  //if ( richPhotons()->empty() )
  {
    if ( !photonCreator()->reconstructPhotons() ) return StatusCode::FAILURE;
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Reconstructed " << richPhotons()->size() << " photon candidates" << endmsg;
    if ( richPhotons()->empty() )
      return Warning("No reconstructed photons -> Abort",StatusCode::SUCCESS,0);
  }

  // ------------------------------ General RICH reco stuff -------------------------------------

  // Now start Global PID :

  if ( m_oneAtATime )
  {
    // Cache selected tracks
    LHCb::RichGlobalPIDTrack::Vector gtracks;
    gtracks.reserve(gpidTracks()->size());
    for ( LHCb::RichGlobalPIDTracks::const_iterator iT = gpidTracks()->begin();
          iT != gpidTracks()->end(); ++iT ) 
    {
      if ( (*iT)->richRecTrack()->inUse() ) gtracks.push_back(*iT);
    }

    // Turn all selected tracks 'off'
    for ( LHCb::RichGlobalPIDTrack::Vector::iterator track = gtracks.begin();
          track != gtracks.end(); ++track ) 
    {
      (*track)->richRecTrack()->setInUse(false);
    }

    // Finally, loop over all tracks and PID them
    for ( LHCb::RichGlobalPIDTrack::Vector::iterator track = gtracks.begin();
          track != gtracks.end(); ++track ) 
    {
      (*track)->richRecTrack()->setInUse(true);
      if ( msgLevel(MSG::DEBUG) ) debug() << "PID'ing track " << (*track)->key() << endmsg;
      m_pidTool->pid( *track );
      (*track)->richRecTrack()->setInUse(false);
    }

    // Turn all selected tracks 'on'
    for ( LHCb::RichGlobalPIDTrack::Vector::iterator track = gtracks.begin();
          track != gtracks.end(); ++track ) 
    {
      (*track)->richRecTrack()->setInUse(true);
    }

  }
  else
  {
    // Do all selected tracks at once
   
    LHCb::RichGlobalPIDTrack::Vector gtracks;
    gtracks.reserve(gpidTracks()->size());
    for ( LHCb::RichGlobalPIDTracks::const_iterator iT = gpidTracks()->begin();
          iT != gpidTracks()->end(); ++iT ) 
    { 
      gtracks.push_back(*iT); 
    }
    m_pidTool->pids( gtracks );

  }

  return StatusCode::SUCCESS;
}
