// $Id: Particle2MCChi2.cpp,v 1.7 2003-06-05 17:42:22 phicharp Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "Particle2MCChi2.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle2MCChi2
//
// 17/05/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<Particle2MCChi2>          s_factory ;
const        IAlgFactory& Particle2MCChi2Factory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2MCChi2::Particle2MCChi2( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : AsctAlgorithm ( name , pSvcLocator )
  , m_chi2( 100. )
{
  m_inputData.push_back( ParticleLocation::Production );
  m_outputTable = Particle2MCAsctLocation ;
  
  declareProperty( "Chi2Cut", m_chi2 );
}

//=============================================================================
// Destructor
//=============================================================================
Particle2MCChi2::~Particle2MCChi2() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Particle2MCChi2::initialize() {
 
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  // Retrieve a weighted associator on which a threshold will be applied
  // Its InputData must be set identical as this one's (true parameter)
  StatusCode sc =  retrievePrivateAsct( "Particle2MCWithChi2Asct", 
                                       true, m_pAsctChi2) ;

  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2MCChi2::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  // Create an association table and register it in the TES
  Particle2MCAsct::Table* table = new Particle2MCAsct::Table();

  // NOTE: there is no need to access the MCParticle container, 
  // since we retrieve them through the relation table

  //////////////////////////////////
  // Loop on Particles containers //
  //////////////////////////////////

  for( std::vector<std::string>::iterator inp = m_inputData.begin(); 
       m_inputData.end()!= inp; inp++) {
    // Get Particles
    SmartDataPtr<Particles> parts (eventSvc(), *inp);
    if( 0 != parts ) {
      log << MSG::VERBOSE << "    Particles retrieved from " << *inp 
          << endreq;
    }
    else {
      log << MSG::INFO
          << "    *** Could not retrieve Particles from " << *inp
          << endreq;
      continue;
    }

    if( m_pAsctChi2->tableExists() ) {
      for( Particles::const_iterator pIt=parts->begin();
           parts->end() != pIt; pIt++) {
        MCsFromParticleChi2 mcRange = 
          m_pAsctChi2->rangeWithHighCutFrom( *pIt, m_chi2) ;
        if( !mcRange.empty() ) {
          for( MCsFromParticleChi2Iterator mcIt = mcRange.begin();
               mcRange.end() != mcIt; mcIt++ ) {
            table->relate( *pIt, mcIt->to());
          }
        }
      }
    } else {
      log << MSG::INFO << "Chi2Asct table not found" << endreq;
    }
  }
      
  // Now register the table in the TES
  StatusCode sc = eventSvc()->registerObject( outputTable(), table);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "     *** Could not register table " << outputTable()
        << endreq;
    delete table;
    return sc;
  } else {
    log << MSG::VERBOSE << "     Registered table " << outputTable() << endreq;
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Particle2MCChi2::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
