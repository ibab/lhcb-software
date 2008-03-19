// $Id: CopyParticles.cpp,v 1.14 2008-03-19 13:18:12 jpalac Exp $
// Include files 

// STL
#include <functional>
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include <Event/Particle.h>
#include <Event/Vertex.h>
// local
#include "CopyParticles.h"
//
//-----------------------------------------------------------------------------
// Implementation file for class : CopyParticles
//
// 2007-10-15 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyParticles );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyParticles::CopyParticles( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : 
  MicroDSTAlgorithm ( name , pSvcLocator ),
  m_particleCloner(0),
  m_particleClonerName("ParticleCloner")
{
  std::cout << "CopyParticles constructor" << std::endl;
  
  declareProperty( "ICloneParticle", m_particleClonerName );
}
//=============================================================================
// Destructor
//=============================================================================
CopyParticles::~CopyParticles() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyParticles::initialize() {

  StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "Going to initialise ParticleCloner" << endmsg;

  m_particleCloner = tool<ICloneParticle>(m_particleClonerName, this);

  if (m_particleCloner) {
    debug() << "Found ParticleCloner" << endmsg;
  } else {
    error() << "Failed to find ParticleCloner" << endmsg;
  }

  if (inputTESLocation()=="")  {
    error() << "No TES location " << endmsg;
    return StatusCode::FAILURE;
  }
  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyParticles::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(true);

  verbose() << "Going to store Particles from " << inputTESLocation()
            << " into " << fullOutputTESLocation() << endmsg;

  typedef LHCb::Particle::Container Particles;

  const LHCb::Particle::Container* clones = 
    copyKeyedContainer<LHCb::Particle::Container, 
                       ICloneParticle>( inputTESLocation(),
                                        m_particleCloner    );
  
  if (!clones) return StatusCode::FAILURE;
  
  verbose() << "Found " << clones->size() << " particles" << endmsg;

  return StatusCode::SUCCESS;

}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyParticles::finalize() {

  debug() << "==> Finalize" << endmsg;

  return MicroDSTAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
