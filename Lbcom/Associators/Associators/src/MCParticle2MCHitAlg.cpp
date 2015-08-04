//
// This File contains the implementation of the MCParticle2MCHitAlg interface
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Authors: Rutger van der Eijk
//   Created: 3-7-2002

// from Event/MCEvent
#include "Event/MCParticle.h"
#include "Event/MCHit.h"

// from Event/LinkerEvent
#include "Linker/LinkerWithKey.h"

// local
#include "MCParticle2MCHitAlg.h"

DECLARE_ALGORITHM_FACTORY( MCParticle2MCHitAlg )

using namespace LHCb;

MCParticle2MCHitAlg::MCParticle2MCHitAlg(const std::string& name,
                                         ISvcLocator* pSvcLocator):
  GaudiAlgorithm ( name , pSvcLocator ),
  m_inputData(),
  m_outputData( LHCb::MCParticleLocation::Default + "2MCHits" )
{
  // Standard constructor, initializes variables
  declareProperty( "MCHitPath",   m_inputData );
  declareProperty( "OutputData",  m_outputData );
}


MCParticle2MCHitAlg::~MCParticle2MCHitAlg()
{
  // Destructor
}


StatusCode MCParticle2MCHitAlg::execute()
{
  // Main execution
  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // get MCHits
  MCHits* mcHits = get<MCHits>( m_inputData );

  LinkerWithKey<MCParticle,MCHit> myLink( eventSvc(), msgSvc(), m_outputData );
  // loop over MCHits
  for(MCHits::const_iterator itHit = mcHits->begin();
      itHit != mcHits->end(); ++itHit) {
    // retrieve MCHit
    MCHit* mcHit = *itHit ;
    if (!mcHit) {
      return Error( "Failed retrieving MCHit" );
    }
    // relate to associated MCParticle
    myLink.link( mcHit, mcHit->mcParticle() );
  }
  
  if( msgLevel(MSG::DEBUG) ) debug() << "Linker table stored at " << m_outputData << endmsg;

  return StatusCode::SUCCESS;
}
