// $Id: CopyParticles.cpp,v 1.3 2007-10-22 13:34:40 jpalac Exp $
// Include files 

// STL
#include <functional>
// BOOST
#include <boost/function.hpp>
#include <boost/bind.hpp>
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// local
#include "CopyParticles.h"

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
  : CopyAndStoreData ( name , pSvcLocator )
{
  //  declareProperty( "StoreTracks", m_storeTracks );
}
//=============================================================================
// Destructor
//=============================================================================
CopyParticles::~CopyParticles() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyParticles::initialize() {

  StatusCode sc = CopyAndStoreData::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (inputTESLocation()=="")  {
    error() << "changing input TES location " << endmsg;
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
  verbose() << "Going to store Particles from " << inputTESLocation()
            << " into " << fullOutputTESLocation() << endmsg;


  StatusCode sc = storeMothers();
  return sc;
  

}
//=============================================================================
StatusCode CopyParticles::storeMothers() 
{
  const Particles* particles =
    copyKeyedContainer<Particles, ParticleCloner>(inputTESLocation(),
                                                  fullOutputTESLocation());
  
  StatusCode sc(StatusCode::SUCCESS);

  for (Particles::const_iterator it = particles->begin();
       sc.isSuccess() && it != particles->end();
       ++it) {
    storeDecay(*it);
  }

  return sc;

}
//=============================================================================
StatusCode CopyParticles::storeDecay(LHCb::Particle* particleClone)
{
  StatusCode sc(StatusCode::SUCCESS);

  if (particleClone->endVertex()) {
    const LHCb::Vertex* vertexClone = 
      storeVertex(particleClone->endVertex());
    if (vertexClone) particleClone->setEndVertex(vertexClone);
  }
  return sc;
}
//=============================================================================
const LHCb::Vertex* CopyParticles::storeVertex(const LHCb::Vertex* vertex)
{
  StatusCode sc(StatusCode::SUCCESS);
  std::string from = objectLocation(vertex->parent());
  const std::string to = outputTESLocation(from);

  LHCb::Vertex* vertexClone = cloneKeyedContainerItem<LHCb::Vertex, VertexItemCloner>(vertex, to);

  storeOutgoingParticles(vertexClone, vertex->outgoingParticles());

  return vertexClone;
  
}
//=============================================================================
LHCb::Particle* CopyParticles::storeParticle(const LHCb::Particle* particle)
{
  StatusCode sc(StatusCode::SUCCESS);
  std::string from = objectLocation(particle->parent());
  const std::string to = outputTESLocation(from);

  LHCb::Particle* particleClone = 
    cloneKeyedContainerItem<LHCb::Particle, ParticleCloneFunctor>(particle, to);

  if (particle->endVertex() && particleClone) {
    const LHCb::Vertex* vertexClone = storeVertex(particle->endVertex() );
    if (vertexClone) particleClone->setEndVertex(vertexClone);
  }

  storeDaughters( particleClone, particle->daughters() );

  return particleClone;

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

  return CopyAndStoreData::finalize();  // must be called after all other actions
}
//=============================================================================
