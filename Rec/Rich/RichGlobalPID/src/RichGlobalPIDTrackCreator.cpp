
//--------------------------------------------------------------------------
/** @file RichGlobalPIDTrackCreator.cpp
 *
 *  Implementation file for RICH Global PID tool : Rich::Rec::GlobalPID::TrackCreator
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
DECLARE_TOOL_FACTORY( TrackCreator )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackCreator::TrackCreator( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : ToolBase( type, name, parent )
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
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichExpectedTrackSignal", m_tkSignal );

  return sc;
}

LHCb::RichGlobalPIDTrack * TrackCreator::createTrack( LHCb::RichRecTrack * track ) const
{
  // new object
  auto * pidTrack = new LHCb::RichGlobalPIDTrack();

  // give to container
  gpidTracks()->insert( pidTrack, track->key() );

  // Make new PID result and save
  auto * newPID = new LHCb::RichGlobalPID();
  gpidPIDs()->insert( newPID, track->key() );
  
  // give PID to track
  pidTrack->setGlobalPID( newPID );

  // Set Track reference
  const auto * trtrack = dynamic_cast<const LHCb::Track *>(track->parentTrack());
  if ( !trtrack ) 
  {
    Warning( "Input track type is not Track -> RichPID has null track reference" ).ignore();
  }
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
  auto * rRTrack = track->richRecTrack();
  auto * pid     = track->globalPID();

  // Set best PID
  pid->setBestParticleID( rRTrack->currentHypothesis() );

  // store used radiator information
  pid->setUsedAerogel  ( rRTrack->inAerogel() );
  pid->setUsedRich1Gas ( rRTrack->inGas1()    );
  pid->setUsedRich2Gas ( rRTrack->inGas2()    );

  // -------------------------------------------------------------------------------
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
    Warning("non-zero deltaLL value").ignore();
    _ri_debug << "PID " << pid->key() 
              << " best ID " << pid->bestParticleID()
              << " has non-zero deltaLL value! " << deltaLLs[pid->bestParticleID()] 
              << endmsg;
  }
  // Internally, the Global PID normalises the DLL values to the best hypothesis
  // and also works in "-loglikelihood" space.
  // For final storage, renormalise the DLLS w.r.t. the pion hypothesis and
  // invert the values
  for ( const auto & hypo : pidTypes() )
  {
    if ( deltaLLs[hypo] < 0 ) { deltaLLs[hypo] = 0; }
    deltaLLs[hypo] = (float) ( pionDLL - deltaLLs[hypo] );
  }
  // final update DLL values in stored RichPID data object
  pid->setParticleLLValues(deltaLLs);
  // -------------------------------------------------------------------------------

  // -------------------------------------------------------------------------------
  // Final checks
  // -------------------------------------------------------------------------------
  if ( !pid->isAboveThreshold(pidTypes().front()) )
  {
    warning() << "Lowest active mass hypothesis '" << pidTypes().front() 
              << "' is below threshold ..." << endmsg;
    warning() << *pid << endmsg;
  }
  // -------------------------------------------------------------------------------

}
