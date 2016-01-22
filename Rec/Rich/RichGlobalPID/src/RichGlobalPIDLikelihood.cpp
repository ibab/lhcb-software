
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
  : AlgBase( name, pSvcLocator )
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
    for ( auto * tk : *gpidTracks() )
    {
      if ( tk->richRecTrack()->inUse() ) gtracks.push_back(tk);
    }

    // Turn all selected tracks 'off'
    for ( auto * tk : gtracks ) { tk->richRecTrack()->setInUse(false); }

    // Finally, loop over all tracks and PID them
    for ( auto * tk : gtracks )
    {
      tk->richRecTrack()->setInUse(true);
      if ( msgLevel(MSG::DEBUG) ) debug() << "PID'ing track " << tk->key() << endmsg;
      m_pidTool->pid( tk );
      tk->richRecTrack()->setInUse(false);
    }

    // Turn all selected tracks 'on'
    for ( auto * tk : gtracks ) { tk->richRecTrack()->setInUse(true); }

  }
  else
  {
    // Do all selected tracks at once
   
    LHCb::RichGlobalPIDTrack::Vector gtracks;
    gtracks.reserve(gpidTracks()->size());
    for ( auto * tk : *gpidTracks() ) { gtracks.push_back(tk); }
    m_pidTool->pids( gtracks );

  }

  return StatusCode::SUCCESS;
}
