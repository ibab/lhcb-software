// $Id: CopyParticles.cpp,v 1.12 2008-02-26 15:27:54 jpalac Exp $
// Include files 

// STL
#include <functional>
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
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

  const LHCb::Particle::Container* clones = 
    copyKeyedContainer<Particles, ICloneParticle>( inputTESLocation(),
                                                   m_particleCloner    );
  
  if (!clones) return StatusCode::FAILURE;
  
  verbose() << "Found " << clones->size() << " particles" << endmsg;

  return StatusCode::SUCCESS;

}
//=============================================================================
LHCb::Particle* CopyParticles::storeParticle(const LHCb::Particle* particle)
{
  StatusCode sc(StatusCode::SUCCESS);

  verbose() << "StoreParticles clone into local store"<< endmsg;

  LHCb::Particle* particleClone = 
    cloneKeyedContainerItem<LHCb::Particle, ParticleItemCloner>(particle);

  verbose() << "StoreParticles cloned into local store!"<< endmsg;

  if (particle->endVertex() && particleClone) {
    const LHCb::Vertex* vertexClone = storeVertex(particle->endVertex() );
    if (vertexClone) particleClone->setEndVertex(vertexClone);
    storeDaughters( particleClone, particle->daughters() );
  }



  return particleClone;

}
//=============================================================================
const LHCb::Vertex* CopyParticles::storeVertex(const LHCb::Vertex* vertex)
{
  StatusCode sc(StatusCode::SUCCESS);

  LHCb::Vertex* vertexClone = 
    cloneKeyedContainerItem<LHCb::Vertex, VertexItemCloner>(vertex);

  storeOutgoingParticles(vertexClone, vertex->outgoingParticles());

  return vertexClone;
  
}
//=============================================================================
void CopyParticles::storeDaughters(LHCb::Particle* particleClone,
                                   const SmartRefVector<LHCb::Particle>& daughters) 
{

  particleClone->clearDaughters();

  for (SmartRefVector<LHCb::Particle>::const_iterator dau = daughters.begin();
       dau != daughters.end();
       ++dau ) {
    LHCb::Particle* daughterClone = storeParticle( *dau);
    if (daughterClone) particleClone->addToDaughters(daughterClone);
  }
}
//=============================================================================
void CopyParticles::storeOutgoingParticles(LHCb::Vertex* vertexClone,
                                           const SmartRefVector<LHCb::Particle>& outParticles) 
{
 
  vertexClone->clearOutgoingParticles();

  for (SmartRefVector<LHCb::Particle>::const_iterator part =
         outParticles.begin();
       part != outParticles.end();
       ++part) {
    LHCb::Particle* particleClone = storeParticle( *part );
    vertexClone->addToOutgoingParticles(particleClone);
  }
 
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyParticles::finalize() {

  debug() << "==> Finalize" << endmsg;

  return MicroDSTAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
