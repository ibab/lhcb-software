// $Id: Particle2MCChi2.cpp,v 1.1 2002-05-17 17:07:51 phicharp Exp $
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
  : Algorithm ( name , pSvcLocator )
  , m_inputData( ParticleLocation::Production )
  , m_outputData( Particle2MCAsctLocation )
  , m_chi2( 100. )
{
  declareProperty( "InputData", m_inputData );
  declareProperty( "OutputData", m_outputData );
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

  // Thsi is the standard Particle2MCChi2 tool
  StatusCode sc = toolSvc()->retrieveTool( "Particle2MCWithChi2Asct" , 
                                           m_pAsctChi2);
  if( sc.isFailure() || 0 == m_pAsctChi2) {
    log << MSG::FATAL << "    Unable to retrieve Chi2 Associator tool" 
        << endreq;
    return sc;
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2MCChi2::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  // get Particles and MCParticles
  SmartDataPtr<Particles> parts (eventSvc(), m_inputData);
  if( 0 != parts ) {
    log << MSG::DEBUG << "    Particles retrieved" << endreq;
  }
  else {
    log << MSG::FATAL 
        << "    *** Could not retrieve Particles from " << m_inputData
        << endreq;
    return StatusCode::FAILURE;
    
  }
  
  SmartDataPtr<MCParticles> 
    mcParts( eventSvc(), MCParticleLocation::Default );
  if( 0 != mcParts ) {
    log << MSG::DEBUG << "    MCPart retrieved" << endreq;
  }
  else {
    log << MSG::FATAL << "    *** Could not retrieve MCPart from " 
        << MCParticleLocation::Default << endreq;
    return StatusCode::FAILURE;
    
  }

  Particle2MCAsct::Table* table = new Particle2MCAsct::Table();

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
    log << MSG::DEBUG << "Chi2Asct table not found" << endreq;
  }
      
  // Now register the table in the TES
  StatusCode sc = eventSvc()->registerObject( outputData(), table);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "     *** Could not register table " << outputData()
        << endreq;
    delete table;
    return sc;
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
