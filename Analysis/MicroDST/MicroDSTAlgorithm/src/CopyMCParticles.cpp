// $Id: CopyMCParticles.cpp,v 1.1 2008-04-08 16:44:53 jpalac Exp $
// Include files 

// STL
#include <functional>
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include <Event/MCParticle.h>
#include <Event/Vertex.h>
// local
#include "CopyMCParticles.h"
//
//-----------------------------------------------------------------------------
// Implementation file for class : CopyMCParticles
//
// 2008-04-08 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyMCParticles );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyMCParticles::CopyMCParticles( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : 
  MicroDSTAlgorithm ( name , pSvcLocator ),
  m_particleCloner(0),
  m_particleClonerName("MCParticleCloner")
{
  declareProperty( "IMCCloneParticle", m_particleClonerName );
}
//=============================================================================
// Destructor
//=============================================================================
CopyMCParticles::~CopyMCParticles() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyMCParticles::initialize() {

  StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "Going to initialise MCParticleCloner" << endmsg;

  m_particleCloner = tool<ICloneMCParticle>(m_particleClonerName, this);

  if (m_particleCloner) {
    debug() << "Found ICloneMCParticle" << endmsg;
  } else {
    error() << "Failed to find ICloneMCParticle" << endmsg;
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
StatusCode CopyMCParticles::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(true);

  verbose() << "Going to store MCParticles from " << inputTESLocation()
            << " into " << fullOutputTESLocation() << endmsg;

  const LHCb::MCParticle::Container* clones = 
    copyKeyedContainer<LHCb::MCParticle::Container, 
    ICloneMCParticle>( inputTESLocation(),
                     m_particleCloner    );
  
  if (!clones) return StatusCode::FAILURE;
  
  verbose() << "Found " << clones->size() << " MCParticles" << endmsg;

  return StatusCode::SUCCESS;

}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyMCParticles::finalize() {

  debug() << "==> Finalize" << endmsg;

  return MicroDSTAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
