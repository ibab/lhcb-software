// $Id: Particle2MCLinks.cpp,v 1.10 2003-05-26 11:38:38 phicharp Exp $
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

  MsgStream msg(msgSvc(), name());
  msg << MSG::VERBOSE << "==> Initialise" << endreq;

  StatusCode sc = retrievePrivateAsct( "ProtoParticle2MCAsct", "ChargedPPAsct",
                                       false, m_pAsctCharged) ;
  if( sc.isSuccess() ) {
    sc = retrievePrivateAsct( "ProtoParticle2MCAsct", "NeutralPPAsct",
                              false, m_pAsctNeutral) ;
  }
  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2MCLinks::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::VERBOSE << "==> Execute" << endreq;

  // create an association table and register it in the TES
  Particle2MCLinksAsct::Table* table = new Particle2MCLinksAsct::Table();
  
  //////////////////////////////////
  // Loop on Particles containers //
  //////////////////////////////////

  for( std::vector<std::string>::iterator inp = m_inputData.begin(); 
       m_inputData.end()!= inp; inp++) {
    // Get Particles
    SmartDataPtr<Particles> parts (eventSvc(), *inp);
    if( 0 != parts ) {
      msg << MSG::VERBOSE << "    Particles retrieved from " << *inp 
        << endreq;
    }
    else {
      msg << MSG::INFO << "    *** Could not retrieve Particles from " << *inp
        << endreq;
      continue;
    }
    
    // loop on Parts and MCParts to match them
    if( m_pAsctCharged->tableExists() || m_pAsctNeutral->tableExists() ) {
      for( Particles::const_iterator pIt=parts->begin() ;
           parts->end() != pIt; pIt++) {
        msg << MSG::VERBOSE << "    Particle " << (*pIt)->key();
        ProtoParticle* protoPart = dynamic_cast<ProtoParticle*>
          ( (*pIt)->origin() ) ;
        if( protoPart ) {
          if( msg.level() <= MSG::VERBOSE ) {
            std::string strCharged = (*pIt)->charge() ? "Charged" : "Neutral";
            msg << " from " << strCharged << " ProtoParticle " << protoPart->key();
          }
          ProtoParticle2MCAsct::IAsct* pAsct =
            (*pIt)->charge() ? m_pAsctCharged : m_pAsctNeutral;
          MCsFromProtoParticle mcPartRange = 
            pAsct->rangeFrom( protoPart );
          if( !mcPartRange.empty() ) {
            msg << " associated to " << mcPartRange.end()-mcPartRange.begin()
                << " MCParts: ";
            MCsFromProtoParticleIterator mcPartIt;
            for( mcPartIt = mcPartRange.begin();
                 mcPartRange.end() != mcPartIt; mcPartIt++ ) {
              MCParticle* mcPart = mcPartIt->to();
              double weight = mcPartIt->weight();
              if( mcPart ) {
                msg  << mcPart->key() << " - ";
                table->relate( *pIt, mcPart, weight);
              }
            }
          } else {
            msg << " not associated to any MCPart";
          }
        } else {
            msg << " not from a ProtoParticle";
        }
        msg << endreq;
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
      msg << MSG::FATAL << "     *** Could not register table " 
          << outputTable()
          << endreq;
      delete table;
      return sc;
    } else {
      msg << MSG::VERBOSE << "     Registered table " 
          << outputTable() << endreq;
    }
  } else {
    msg << MSG::FATAL << "     *** Could not create table " 
        << outputTable()
        << endreq;
  }
    
  return StatusCode::SUCCESS ;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Particle2MCLinks::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::VERBOSE << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
