//
// This File contains the implementation of the MCParticle2MCVeloHitAlg class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Authors: Rutger van der Eijk
//   Created: 3-7-2002

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// LHCbKernel
#include "Relations/Associator.h" 

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
  Algorithm ( name , pSvcLocator ),
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


StatusCode MCParticle2MCVeloHitAlg::initialize()
{
  // Initialisation. Check parameters
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  return StatusCode::SUCCESS;
};


StatusCode MCParticle2MCVeloHitAlg::execute()
{
  // Main execution

  // Initialise
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  // get MCVeloHits
  SmartDataPtr<MCVeloHits> mcVeloHits (eventSvc(), m_inputData);
  if(mcVeloHits) {
    log << MSG::DEBUG << "MCVeloHits retrieved from " << m_inputData<< endreq;
  }
  else {
    log << MSG::FATAL << "*** Could not retrieve MCVeloHits from "
        << m_inputData<< endreq;
    return StatusCode::FAILURE;
  }

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
      log << MSG::ERROR << "Failed retrieving MCHit" << endreq;
      return StatusCode::FAILURE;
    }
    
    // retrieve associated MCParticle
    MCParticle* mcPart = mcHit->mcParticle();

    // relate in table 
    table->relate(mcPart, mcHit);
  }
  
  // Register the table on the TES
  StatusCode sc = eventSvc()->registerObject( outputData(), table);
  if(sc.isFailure()) {
    log << MSG::FATAL << "     *** Could not register table " << outputData()
        << endreq;
    delete table;
    return sc;
  }
  return StatusCode::SUCCESS ;
};


StatusCode MCParticle2MCVeloHitAlg::finalize()
{
  //  Finalize

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  // end
  return StatusCode::SUCCESS;
}

