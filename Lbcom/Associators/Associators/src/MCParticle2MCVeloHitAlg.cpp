//
// This File contains the implementation of the MCParticle2MCVeloHitAlg class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Authors: Rutger van der Eijk
//   Created: 3-7-2002

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCbKernel
#include "Relations/Relation1D.h" 

// Event
#include "Event/MCParticle.h"
#include "Event/MCVeloHit.h"

// local
#include "Associators/MCParticle2MCVeloHitAlg.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<MCParticle2MCVeloHitAlg>          s_factory ;
const        IAlgFactory& MCParticle2MCVeloHitAlgFactory = s_factory ; 


MCParticle2MCVeloHitAlg::MCParticle2MCVeloHitAlg(const std::string& name,
                                         ISvcLocator* pSvcLocator):
  GaudiAlgorithm ( name , pSvcLocator ),
  m_inputData( MCVeloHitLocation::Default),
  m_outputData( "Rec/Relations/MCParticle2MCVeloHit" )
{
  // Standard constructor, initializes variables
  declareProperty( "MCHitPath", m_inputData );
  declareProperty( "OutputData", m_outputData );
}


MCParticle2MCVeloHitAlg::~MCParticle2MCVeloHitAlg()
{
  // Destructor
}; 


StatusCode MCParticle2MCVeloHitAlg::execute()
{
  // Main execution
  debug() << "==> Execute" << endmsg;

  // get MCVeloHits
  MCVeloHits* mcVeloHits = get<MCVeloHits>( m_inputData );

  /// typedef
  typedef Relation1D<MCParticle, MCVeloHit> LocalDirectType;

  // create an association table
  LocalDirectType* table = new LocalDirectType();

  // loop over MCVeloHits
  for(MCVeloHits::const_iterator itHit = mcVeloHits->begin();
      itHit != mcVeloHits->end(); itHit++) {
    // retrieve MCVeloHit
    MCVeloHit* mcHit = *itHit ;
    if (!mcHit) {
      return Error( "Failed retrieving MCHit" );
    }
    
    // retrieve associated MCParticle
    MCParticle* mcPart = mcHit->mcParticle();

    // relate in table 
    table->relate(mcPart, mcHit);
  }
  
  // Register the table on the TES
  put( table, outputData() );

  return StatusCode::SUCCESS ;
};

