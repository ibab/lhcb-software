// $Id: RichGlobalPIDFinalize.cpp,v 1.1.1.1 2003-06-30 16:10:54 jonesc Exp $
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

#ifdef WIN32
extern "C" float __stdcall FREQ( const float& );
#else
#define FREQ freq_
extern "C" float FREQ( const float& );
#endif

// Standard constructor, initializes variables
RichGlobalPIDFinalize::RichGlobalPIDFinalize( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : RichGlobalPIDAlgBase ( name, pSvcLocator ) { }

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
  if ( !gpidPIDs() || !gpidTracks()  ) return StatusCode::FAILURE;

  // Iterate over working tracks and keep/delete PID results
  for ( RichGlobalPIDTracks::iterator track = m_GPIDtracks->begin();
        track != m_GPIDtracks->end();
        ++track ) {
    RichRecTrack * rRTrack = (*track)->richRecTrack();

    if ( msgLevel(MSG::VERBOSE) ) {
      msg << MSG::VERBOSE << "finaliseEvent : PID'ed Track "
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
      msg << MSG::WARNING << "PID " << pid->key() << " PID " << pid->bestParticleID()
          << " has non-zero deltaLL value! " << deltaLLs[pid->bestParticleID()] << endreq;
    }
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
      double dll =  deltaLLs[iHypo];
      if ( dll < 0 ) {
        msg << MSG::WARNING << "Negative deltaLL " << dll << " reset to 0" << endreq;
        dll = 0.0;
      }
      double prob = 2.0*( 1.0 - FREQ(sqrt(2.0*dll)) );
      pid->setParticleRawProb( (Rich::ParticleIDType)iHypo, prob );
    }

  }

  // All OK - Update ProcStatus with number of PIDs
  procStatus()->addAlgorithmStatus( m_richGPIDName, m_GPIDs->size() );

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichGlobalPIDFinalize::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Execute base class method
  return RichRecAlgBase::finalize();
}
