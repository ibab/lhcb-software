// $Id: Particle2MCLinks.cpp,v 1.2 2002-07-12 15:27:11 phicharp Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "Particle2MCLinks.h"

// from event model
#include "Event/AxPartCandidate.h"

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
  : Algorithm ( name , pSvcLocator )
  , m_outputTable( "Phys/Relations/Part2MCfromLinks" )
{
  m_inputData.push_back( ParticleLocation::Production );
  declareProperty( "InputData", m_inputData );
  declareProperty( "OutputTable", m_outputTable );
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

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2MCLinks::execute() {

  static bool notFromAxPart = false;
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  SmartDataPtr<MCParticles> 
    mcParts( eventSvc(), MCParticleLocation::Default );
  if( 0 != mcParts ) {
    log << MSG::DEBUG << "    MCPart retrieved" << endreq;
  }
  else {
    log << MSG::ERROR << "    *** Could not retrieve MCPart" << endreq;
  }

  // create an association table and register it in the TES
  Particle2MCAsct::Table* table = new Particle2MCAsct::Table();

  // First loop on MCParticles to count how many there are for each collision
  Collision* currentColl = 0 ;
  std::vector<long> nbMCPartIntegrated ;
  int nbMCPart = 0 ;
  
  for( MCParticles::const_iterator mcIt=mcParts->begin();
       mcParts->end() != mcIt; mcIt++, nbMCPart++) {
    MCParticle* mcPart = *mcIt ;
    Collision* coll = mcPart->collision() ;
    if(  currentColl != coll ) {
      nbMCPartIntegrated.push_back( nbMCPart ) ;
      currentColl = coll ;
    }
  }
  log << MSG::DEBUG << "       Nb of collisions found : " 
      << nbMCPartIntegrated.size() << " for " << nbMCPart 
      <<" MCParticles" << endreq ;
  
  //////////////////////////////////
  // Loop on Particles containers //
  //////////////////////////////////

  int nrel = 0;
  for( vector<std::string>::iterator inp = m_inputData.begin(); 
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
    for( Particles::const_iterator pIt=parts->begin() ;
         parts->end() != pIt; pIt++) {
      // Follow links from this particle in the ZEBRA bank...
      AxPartCandidate* axPart = dynamic_cast<AxPartCandidate*> 
        ( (*pIt)->origin() ) ;
      if( 0 == axPart ) {
        if( false == notFromAxPart ) {
          log << MSG::WARNING 
              << " Particles not originating from AxPartCandidate" << endreq ;
          notFromAxPart = true ;
        }
      } else {
        long truth = axPart->addrMCTruth() ;
        long interaction = truth/10000 ;
        long posInInter = truth % 10000 ;
        
        int mcPartSeq = nbMCPartIntegrated[interaction-1] + posInInter - 1 ;

        notFromAxPart = false ;
        MCParticles::const_iterator mcIt=mcParts->begin() ;
        mcIt += mcPartSeq;
        MCParticle* mcPart = *mcIt ;
        table->relate( *pIt, mcPart);
        nrel++;
      }
    }
  
    log << MSG::DEBUG
        << parts->end() - parts->begin() << " Parts associated with "
        << mcParts->end() - mcParts->begin() << " MCParts" << endreq;
  }
  
  // Register the table on the TES
  if( !notFromAxPart && nrel ) {
    StatusCode sc = eventSvc()->registerObject( outputTable(), table);
    if( sc.isFailure() ) {
      log << MSG::FATAL << "     *** Could not register table " 
          << outputTable()
          << endreq;
      delete table;
      return sc;
    }
  } else {
    delete table;
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
