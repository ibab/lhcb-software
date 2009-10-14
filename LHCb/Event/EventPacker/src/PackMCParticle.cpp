// $Id: PackMCParticle.cpp,v 1.4 2009-10-14 16:22:02 cattanem Exp $
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

DECLARE_ALGORITHM_FACTORY( PackMCParticle );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PackMCParticle::PackMCParticle( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::MCParticleLocation::Default );
  declareProperty( "OutputName", m_outputName = LHCb::PackedMCParticleLocation::Default );
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

  LHCb::MCParticles* parts = getOrCreate<LHCb::MCParticles,LHCb::MCParticles>( m_inputName );
  debug() << m_inputName << " contains " << parts->size()
          << " MCParticles to convert." << endmsg;
  
  StandardPacker pack;
  
  LHCb::PackedMCParticles* out = new LHCb::PackedMCParticles();
  put( out, m_outputName );

  for ( LHCb::MCParticles::const_iterator itP = parts->begin();
        parts->end() != itP; ++itP ) {
    LHCb::MCParticle* part = (*itP);
    LHCb::PackedMCParticle newPart;
    
    newPart.key  = part->key();
    newPart.px   = pack.energy( part->momentum().px() );
    newPart.py   = pack.energy( part->momentum().py() );
    newPart.pz   = pack.energy( part->momentum().pz() );
    newPart.mass = (float)part->virtualMass();
    newPart.PID  = part->particleID().pid();
    newPart.originVertex = pack.reference( out, 
                                           part->originVertex()->parent(), 
                                           part->originVertex()->key() );
    for ( SmartRefVector<LHCb::MCVertex>::const_iterator itV = part->endVertices().begin();
          part->endVertices().end() != itV; ++itV ) {
      int ref = pack.reference( out, (*itV)->parent(), (*itV)->key() );
      newPart.endVertices.push_back( ref );
      if ( MSG::VERBOSE >= msgLevel() ) {
        verbose() << "Reference to endVertex       " 
                  << format( "%8x",  ref ) << endmsg;
      }
    }
    out->addEntry( newPart );
  }

  // Clear the registry address of the unpacked container, to prevent reloading
  parts->registry()->setAddress( 0 );
  
  return StatusCode::SUCCESS;
};

//=============================================================================
