// $Id: RichGlobalPIDFinalize.cpp,v 1.4 2003-08-06 10:02:10 jonrob Exp $
// Include files

// local
#include "RichGlobalPIDFinalize.h"

//--------------------------------------------------------------------------
// Implementation file for class : RichGlobalPIDFinalize
//
// 017/04/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichGlobalPIDFinalize>          s_factory ;
const        IAlgFactory& RichGlobalPIDFinalizeFactory = s_factory ;

// Standard constructor, initializes variables
RichGlobalPIDFinalize::RichGlobalPIDFinalize( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : RichGlobalPIDAlgBase ( name, pSvcLocator ) {

  declareProperty( "ProcStatusLocation",
                   m_procStatLocation = ProcStatusLocation::Default );

}

// Destructor
RichGlobalPIDFinalize::~RichGlobalPIDFinalize() {}

// Initialize
StatusCode RichGlobalPIDFinalize::initialize() {

  // Sets up various tools and services
  if ( !RichRecAlgBase::initialize() ) return StatusCode::FAILURE;

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Initialize" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichGlobalPIDFinalize::execute() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Execute" << endreq;

  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // update pointers to global PID working objects
  if ( !gpidPIDs() || !gpidTracks() ) return StatusCode::FAILURE;

  // Iterate over working tracks and keep/delete PID results
  for ( RichGlobalPIDTracks::iterator track = m_GPIDtracks->begin();
        track != m_GPIDtracks->end();
        ++track ) {
    RichRecTrack * rRTrack = (*track)->richRecTrack();

    if ( msgLevel(MSG::VERBOSE) ) {
      msg << MSG::VERBOSE << "PID'ed Track "
          << (*track)->key() << " (" << (*track)->trQuality()
          << "), as " << rRTrack->currentHypothesis() << endreq;
    }

    RichGlobalPID * pid = (*track)->globalPID();

    // Only store results for physics quality tracks
    if ( (*track)->trQuality() != Rich::GlobalPID::Physics ) {
      m_GPIDs->erase( pid );
      continue;
    }

    // Set best PID
    pid->setBestParticleID( rRTrack->currentHypothesis() );

    // Finalise delta LL and probability values
    std::vector<float> & deltaLLs = pid->particleLLValues();
    if ( deltaLLs[pid->bestParticleID()] > 1e-10 ) {
      msg << MSG::WARNING << "PID " << pid->key() << " best ID " << pid->bestParticleID()
          << " has non-zero deltaLL value! " << deltaLLs[pid->bestParticleID()] << endreq;
    }
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
      if ( deltaLLs[iHypo] < 0 ) {
        if ( fabs(deltaLLs[iHypo]) > 1e-5 ) {
          msg << MSG::WARNING << "PID " << pid->key() << " ID " << (Rich::ParticleIDType)iHypo
              << " negative deltaLL " << deltaLLs[iHypo] << " reset to 0" << endreq;
        }
        deltaLLs[iHypo] = 0.0;
      }
      double prob = 1.0 - gsl_sf_erf( sqrt(deltaLLs[iHypo]) );
      pid->setParticleRawProb( (Rich::ParticleIDType)iHypo, prob );
    }

  }

  // All OK - Update ProcStatus with number of PIDs
  SmartDataPtr<ProcStatus> procStat( eventSvc(), m_procStatLocation );
  if ( !procStat ) {
    msg << MSG::WARNING << "Failed to locate ProcStatus at "
        << m_procStatLocation << endreq;
    return StatusCode::FAILURE;
  }
  procStat->addAlgorithmStatus( m_richGPIDName, m_GPIDs->size() );

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichGlobalPIDFinalize::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Execute base class method
  return RichRecAlgBase::finalize();
}
