//
// This File contains the implementation of the MCParticle2MCHitAlg interface
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
#include "Event/MCHit.h"

// local
#include "Associators/MCParticle2MCHitAlg.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<MCParticle2MCHitAlg>          s_factory ;
const        IAlgFactory& MCParticle2MCHitAlgFactory = s_factory ; 


MCParticle2MCHitAlg::MCParticle2MCHitAlg(const std::string& name,
                                         ISvcLocator* pSvcLocator):
  Algorithm ( name , pSvcLocator ),
  m_inputData( MCHitLocation::OTHits),
  m_outputData( "Rec/Relations/MCParticle2MCHit" )
{
  // Standard constructor, initializes variables
  declareProperty( "MCHitPath", m_inputData );
  declareProperty( "OutputData", m_outputData );
}


MCParticle2MCHitAlg::~MCParticle2MCHitAlg()
{
  // Destructor
}; 


StatusCode MCParticle2MCHitAlg::initialize()
{
  // Initialisation. Check parameters
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  return StatusCode::SUCCESS;
};


StatusCode MCParticle2MCHitAlg::execute()
{
  // Main execution

  // Initialise
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  // get MCHits
  SmartDataPtr<MCHits> mcHits (eventSvc(), m_inputData);
  if(mcHits) {
    log << MSG::DEBUG << "MCHits retrieved from " << m_inputData<< endreq;
  }
  else {
    log << MSG::FATAL << "*** Could not retrieve MCHits from "
        << m_inputData<< endreq;
    return StatusCode::FAILURE;
  }

  /// typedef
  typedef Relation1D<MCParticle, MCHit> LocalDirectType;

  // create an association table
  LocalDirectType* table = new LocalDirectType();

  // loop over MCHits
  for(MCHits::const_iterator itHit = mcHits->begin();
      itHit != mcHits->end(); itHit++) {
    // retrieve MCHit
    MCHit* mcHit = *itHit ;
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


StatusCode MCParticle2MCHitAlg::finalize()
{
  //  Finalize

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  // end
  return StatusCode::SUCCESS;
}

