
//--------------------------------------------------------------------------
/** @file RichGlobalPIDFinalize.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::Finalize
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDFinalize.cpp,v 1.20 2007-12-14 14:21:18 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichGlobalPIDFinalize.h"

// namespaces
using namespace Rich::Rec::GlobalPID;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Finalize );

// Standard constructor, initializes variables
Finalize::Finalize( const std::string& name,
                    ISvcLocator* pSvcLocator )
  : AlgBase ( name, pSvcLocator ) { }

// Destructor
Finalize::~Finalize() {}

StatusCode Finalize::execute()
{
  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Iterate over working tracks and keep/delete PID results
  for ( LHCb::RichGlobalPIDTracks::iterator track = gpidTracks()->begin();
        track != gpidTracks()->end(); ++track )
  {
    LHCb::RichRecTrack * rRTrack = (*track)->richRecTrack();

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "PID'ed Track "
                << (*track)->key() << " (" << (*track)->trQuality()
                << "), as " << rRTrack->currentHypothesis() << endreq;
    }

    LHCb::RichGlobalPID * pid = (*track)->globalPID();

    // Only store results for physics quality tracks
    if ( (*track)->trQuality() != Rich::Rec::GlobalPID::Physics )
    {
      gpidPIDs()->erase( pid );
      continue;
    }

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

  // All OK - Update ProcStatus with number of PIDs
  procStatus()->addAlgorithmStatus( gpidName(), gpidPIDs()->size() );

  return StatusCode::SUCCESS;
}

