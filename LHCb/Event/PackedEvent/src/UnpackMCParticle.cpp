// $Id: UnpackMCParticle.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/MCParticle.h"
#include "Kernel/StandardPacker.h"
#include "Event/PackedMCParticle.h"
// local
#include "UnpackMCParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackMCParticle
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<UnpackMCParticle>          s_factory ;
const        IAlgFactory& UnpackMCParticleFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackMCParticle::UnpackMCParticle( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = PackedMCParticleLocation::Default );
  declareProperty( "OutputName", m_outputName = MCParticleLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
UnpackMCParticle::~UnpackMCParticle() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackMCParticle::execute() {

  debug() << "==> Execute" << endmsg;

  PackedMCParticles* dst = get<PackedMCParticles>( m_inputName );
  debug() << "Size of PackedMCParticles = " << dst->end() - dst->begin() << endreq;

  MCParticles* newMCParticles = new MCParticles();
  put( newMCParticles, m_outputName );

  StandardPacker pack;
  
  for ( std::vector<PackedMCParticle>::const_iterator itS = dst->begin();
        dst->end() != itS; ++itS ) {
    const PackedMCParticle& src = (*itS);

    MCParticle* part = new MCParticle( );
    newMCParticles->insert( part, src.key );

    Hep3Vector p( pack.energy( src.px ),pack.energy( src.py ), pack.energy( src.pz ) );
    part->momentum().setVectM( p, src.mass );

    part->particleID().setPid( src.PID );
    part->setHasOscillated( src.oscillated == 1 );
    part->setHelicity( src.helicity );
    int hintID;
    int key;
    pack.hintAndKey( src.originVertex, dst, newMCParticles, hintID, key );
    SmartRef<MCVertex> ref( newMCParticles, hintID, key );
    part->setOriginVertex( ref );

    std::vector<int>::const_iterator itI;
    for ( itI = src.endVertices.begin() ; src.endVertices.end() != itI ; ++itI ) {
      pack.hintAndKey( *itI, dst, newMCParticles, hintID, key );
      SmartRef<MCVertex> ref( newMCParticles, hintID, key );
      part->addToEndVertices( ref );
    }
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
