// $Id: UnpackMCParticle.cpp,v 1.2 2009-07-09 09:44:16 cattanem Exp $
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

DECLARE_ALGORITHM_FACTORY( UnpackMCParticle );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackMCParticle::UnpackMCParticle( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::PackedMCParticleLocation::Default );
  declareProperty( "OutputName", m_outputName = LHCb::MCParticleLocation::Default );
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

  LHCb::PackedMCParticles* dst = get<LHCb::PackedMCParticles>( m_inputName );
  debug() << "Size of PackedMCParticles = " << dst->end() - dst->begin() << endmsg;

  LHCb::MCParticles* newMCParticles = new LHCb::MCParticles();
  put( newMCParticles, m_outputName );

  StandardPacker pack;
  
  for ( std::vector<LHCb::PackedMCParticle>::const_iterator itS = dst->begin();
        dst->end() != itS; ++itS ) {
    const LHCb::PackedMCParticle& src = (*itS);

    LHCb::MCParticle* part = new LHCb::MCParticle( );
    newMCParticles->insert( part, src.key );

    double px = pack.energy( src.px );
    double py = pack.energy( src.py );
    double pz = pack.energy( src.pz );
    double mass = src.mass;
    double E = sqrt( px*px + py*py + pz*pz + mass*mass );
    Gaudi::LorentzVector p( px, py, pz , E );
    part->setMomentum( p );

    LHCb::ParticleID PID(src.PID);    
    part->setParticleID( PID );
    
    int hintID;
    int key;
    pack.hintAndKey( src.originVertex, dst, newMCParticles, hintID, key );
    SmartRef<LHCb::MCVertex> ref( newMCParticles, hintID, key );
    part->setOriginVertex( ref );

    std::vector<int>::const_iterator itI;
    for ( itI = src.endVertices.begin() ; src.endVertices.end() != itI ; ++itI ) {
      pack.hintAndKey( *itI, dst, newMCParticles, hintID, key );
      SmartRef<LHCb::MCVertex> ref( newMCParticles, hintID, key );
      part->addToEndVertices( ref );
    }
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
