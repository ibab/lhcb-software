// $Id: Particle2MCLinks.cpp,v 1.6 2002-09-03 09:19:19 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "Particle2MCLinks.h"

// from event model
#include "Event/ProtoParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle2MCLinks
//
// 10/05/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<Particle2MCLinks>          s_factory ;
const        IAlgFactory& Particle2MCLinksFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2MCLinks::Particle2MCLinks( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : AsctAlgorithm ( name , pSvcLocator )
{
  m_outputTable = "Phys/Relations/Part2MCfromLinks" ;
  m_inputData.push_back( ParticleLocation::Production );
}

//=============================================================================
// Destructor
//=============================================================================
Particle2MCLinks::~Particle2MCLinks() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Particle2MCLinks::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  StatusCode sc = retrievePrivateAsct( "ProtoParticle2MCAsct", 
                                       "Proto", false, m_pAsctProto) ;

  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2MCLinks::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  // create an association table and register it in the TES
  Particle2MCAsct::Table* table = new Particle2MCAsct::Table();
  
  //////////////////////////////////
  // Loop on Particles containers //
  //////////////////////////////////

  for( std::vector<std::string>::iterator inp = m_inputData.begin(); 
       m_inputData.end()!= inp; inp++) {
    // Get Particles
    SmartDataPtr<Particles> parts (eventSvc(), *inp);
    if( 0 != parts ) {
      log << MSG::DEBUG << "    Particles retrieved from " << *inp 
          << endreq;
    }
    else {
      log << MSG::FATAL << "    *** Could not retrieve Particles" << endreq;
      continue;
    }
    
    // loop on Parts and MCParts to match them
    if( m_pAsctProto->tableExists() ) {
      for( Particles::const_iterator pIt=parts->begin() ;
           parts->end() != pIt; pIt++) {
        log << MSG::DEBUG << "    Particle " << (*pIt)->key();
        ProtoParticle* protoPart = dynamic_cast<ProtoParticle*>
          ( (*pIt)->origin() ) ;
        if( protoPart ) {
          log << " from ProtoParticle " << protoPart->key();
          MCParticle* mcPart = m_pAsctProto->associatedFrom( protoPart );
          if( mcPart ) {
            log << " associated to MCPart " << mcPart->key();
            table->relate( *pIt, mcPart);
          }
        }
        log << endreq;
      }
      
    } else {
      delete table;
      table = 0;
    }
  }
  if (table!=0) {
      // Register the table on the TES
      StatusCode sc = eventSvc()->registerObject( outputTable(), table);
      if( sc.isFailure() ) {
        log << MSG::FATAL << "     *** Could not register table " 
            << outputTable()
            << endreq;
        delete table;
        return sc;
      } else {
        log << MSG::DEBUG << "     Registered table " 
            << outputTable() << endreq;
      }
  } else {
      log << MSG::FATAL << "     *** Could not create table " 
            << outputTable()
            << endreq;

  }
    
  return StatusCode::SUCCESS ;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Particle2MCLinks::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
