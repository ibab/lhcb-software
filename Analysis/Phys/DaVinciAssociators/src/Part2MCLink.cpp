// $Id: Part2MCLink.cpp,v 1.1 2002-05-10 15:08:21 phicharp Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "Part2MCLink.h"

// from event model
#include "Event/AxPartCandidate.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Part2MCLink
//
// 10/05/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<Part2MCLink>          s_factory ;
const        IAlgFactory& Part2MCLinkFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Part2MCLink::Part2MCLink( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_inputData( ParticleLocation::Production )
  , m_outputData( "Phys/Relations/Part2MCfromLinks" )
{
  declareProperty( "InputData", m_inputData );
  declareProperty( "OutputData", m_outputData );
}

//=============================================================================
// Destructor
//=============================================================================
Part2MCLink::~Part2MCLink() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Part2MCLink::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode Part2MCLink::execute() {

  static bool notFromAxPart = false;
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  // get Particles and MCParticles
  SmartDataPtr<Particles> parts (eventSvc(), m_inputData);
  if( 0 != parts ) {
    log << MSG::DEBUG << "    Particles retrieved" << endreq;
  }
  else {
    log << MSG::FATAL << "    *** Could not retrieve Particles" << endreq;
  }
  
  SmartDataPtr<MCParticles> 
    mcParts( eventSvc(), MCParticleLocation::Default );
  if( 0 != mcParts ) {
    log << MSG::DEBUG << "    MCPart retrieved" << endreq;
  }
  else {
    log << MSG::FATAL << "    *** Could not retrieve MCPart" << endreq;
  }
  // create an association table and register it in the TES
  Part2MCPartAsct::Table* table = new Part2MCPartAsct::Table();
  StatusCode sc = eventSvc()->registerObject( outputData(), table);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "     *** Could not register " << outputData()
        << endreq;
    delete table;
    return sc;
  }

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
      return StatusCode::SUCCESS ;
    }
    
    long truth = axPart->addrMCTruth() ;
    long interaction = truth/10000 ;
    long posInInter = truth % 10000 ;
    
    int mcPartSeq = nbMCPartIntegrated[interaction-1] + posInInter ;
    
    MCParticles::const_iterator mcIt=mcParts->begin() ;
    mcIt += mcPartSeq;
    MCParticle* mcPart = *mcIt ;
    table->relate( *pIt, mcPart);
  }
  
  log << MSG::DEBUG
      << parts->end() - parts->begin() << " Parts associated with "
      << mcParts->end() - mcParts->begin() << " MCParts" << endreq;

  return StatusCode::SUCCESS ;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Part2MCLink::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
