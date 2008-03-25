
//--------------------------------------------------------------------------
/** @file RichGlobalPIDTrackCreator.cpp
 *
 *  Implementation file for RICH Global PID tool : Rich::Rec::GlobalPID::TrackCreator
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDTrackCreator.cpp,v 1.1 2008-03-25 16:26:44 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2008-03-01
 */
//--------------------------------------------------------------------------

// local
#include "RichGlobalPIDTrackCreator.h"

// namespaces
using namespace Rich::Rec::GlobalPID;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrackCreator );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackCreator::TrackCreator( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : Rich::Rec::GlobalPID::ToolBase ( type, name, parent ),
    m_tkSignal ( NULL )
{
  declareInterface<ITrackCreator>(this);
}

//=============================================================================
// Destructor
//=============================================================================
TrackCreator::~TrackCreator() {}

//  Initialize
StatusCode TrackCreator::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::GlobalPID::ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichExpectedTrackSignal", m_tkSignal );

  return sc;
}

LHCb::RichGlobalPIDTrack * TrackCreator::createTrack( LHCb::RichRecTrack * track ) const
{
  // new object
  LHCb::RichGlobalPIDTrack * pidTrack = new LHCb::RichGlobalPIDTrack();

  // give to container
  gpidTracks()->insert( pidTrack, track->key() );

  // Make new PID result and save
  LHCb::RichGlobalPID * newPID = new LHCb::RichGlobalPID();
  gpidPIDs()->insert( newPID, track->key() );

  // give PID to track
  pidTrack->setGlobalPID( newPID );

  // Set Track reference
  const LHCb::Track * trtrack = dynamic_cast<const LHCb::Track *>(track->parentTrack());
  if ( !trtrack ) Warning( "Input track type is not Track -> RichPID has null track reference" );
  newPID->setTrack( trtrack );

  // Set its SmartRef to RichRecTrack
  pidTrack->setRichRecTrack( track );

  // Store threshold information
  m_tkSignal->setThresholdInfo( track, newPID );

  // return final track
  return pidTrack;
}

//=============================================================================

void TrackCreator::finaliseTrack( LHCb::RichGlobalPIDTrack * track ) const
{
  LHCb::RichRecTrack  * rRTrack = track->richRecTrack();
  LHCb::RichGlobalPID * pid     = track->globalPID();

  // Set best PID
  pid->setBestParticleID( rRTrack->currentHypothesis() );

  // store used radiator information
  pid->setUsedAerogel  ( rRTrack->inAerogel() );
  pid->setUsedRich1Gas ( rRTrack->inGas1()    );
  pid->setUsedRich2Gas ( rRTrack->inGas2()    );

  // Finalise delta LL values
  // -------------------------------------------------------------------------------
  // Make a working copy of the DLL values
  std::vector<float> deltaLLs = pid->particleLLValues();
  // Get the pion DLL
  double pionDLL = pid->particleDeltaLL(Rich::Pion);
  if ( pionDLL < 0 ) { pionDLL = 0; }
  // sanity check on best ID
  if ( deltaLLs[pid->bestParticleID()] > 1e-10 )
  {
    warning() << "PID " << pid->key() << " best ID " << pid->bestParticleID()
              << " has non-zero deltaLL value! " << deltaLLs[pid->bestParticleID()] << endreq;
  }
  // Internally, the Global PID normalises the DLL values to the best hypothesis
  // and also works in "-loglikelihood" space.
  // For final storage, renormalise the DLLS w.r.t. the pion hypothesis and
  // invert the values
  for ( Rich::Particles::const_iterator iHypo = pidTypes().begin();
        iHypo != pidTypes().end(); ++iHypo )
  {
    if ( deltaLLs[*iHypo] < 0 ) { deltaLLs[*iHypo] = 0; }
    deltaLLs[*iHypo] = pionDLL - deltaLLs[*iHypo];
  }
  // final update DLL values in stored RichPID data object
  pid->setParticleLLValues(deltaLLs);
  // -------------------------------------------------------------------------------

}
