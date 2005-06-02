// $Id: PackMCParticle.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/MCParticle.h"
#include "Kernel/StandardPacker.h"
#include "Event/PackedMCParticle.h"
// local
#include "PackMCParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackMCParticle
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PackMCParticle>          s_factory ;
const        IAlgFactory& PackMCParticleFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PackMCParticle::PackMCParticle( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = MCParticleLocation::Default );
  declareProperty( "OutputName", m_outputName = PackedMCParticleLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
PackMCParticle::~PackMCParticle() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackMCParticle::execute() {

  debug() << "==> Execute" << endmsg;

  MCParticles* parts = get<MCParticles>( m_inputName );
  debug() << m_inputName << " contains " << parts->size()
          << " MCParticles to convert." << endreq;
  
  StandardPacker pack;
  
  PackedMCParticles* out = new PackedMCParticles();
  put( out, m_outputName );

  for ( MCParticles::const_iterator itP = parts->begin();
        parts->end() != itP; ++itP ) {
    MCParticle* part = (*itP);
    PackedMCParticle newPart;
    
    newPart.key  = part->key();
    newPart.px   = pack.energy( part->momentum().px() );
    newPart.py   = pack.energy( part->momentum().py() );
    newPart.pz   = pack.energy( part->momentum().pz() );
    newPart.mass = part->momentum().m();
    newPart.PID  = part->particleID().pid();
    if ( part->hasOscillated() ) {
      newPart.oscillated = 1;
    } else {
      newPart.oscillated = 0;
    }
    newPart.helicity = part->helicity();
    if ( 0 !=  part->helicity()  ) {
      info() << "MCPart " << newPart.key 
             << " oscillated " << int(newPart.oscillated)
             << " helicity " <<  part->helicity() << endreq;
    }
    
    newPart.originVertex = pack.reference( out, 
                                           part->originVertex()->parent(), 
                                           part->originVertex()->key() );
    for ( SmartRefVector<MCVertex>::const_iterator itV = part->endVertices().begin();
          part->endVertices().end() != itV; ++itV ) {
      int ref = pack.reference( out, (*itV)->parent(), (*itV)->key() );
      newPart.endVertices.push_back( ref );
      if ( MSG::VERBOSE >= msgLevel() ) {
        verbose() << "Reference to endVertex       " 
                  << format( "%8x",  ref ) << endreq;
      }
    }
    out->addEntry( newPart );
  }

  // Clear the registry address of the unpacked container, to prevent reloading
  parts->registry()->setAddress( 0 );
  
  return StatusCode::SUCCESS;
};

//=============================================================================
