// $Id: CopyParticles.cpp,v 1.9 2007-10-30 18:10:48 jpalac Exp $
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


  const LHCb::Particle::Container* particles = 
    get<LHCb::Particle::Container>( inputTESLocation() );

  verbose() << "Found " << particles->size() << " particles" << endmsg;
  
  for (Particles::const_iterator iPart = particles->begin();
       iPart != particles->end();
       ++iPart) {
    verbose() << "Storing particle" << endmsg;
    LHCb::Particle* clone = storeParticle( *iPart);
    if (clone) {
      verbose() << "Cloned and stored particle!" << endmsg;
    } else {
      verbose() << "Particle cloning failed!" << endmsg;
    }
  }

  setFilterPassed(true);

  verbose() << "Going to store to TES" << endmsg;

  return copyLocalStoreToTES();
  

}
//=============================================================================
LHCb::Particle* CopyParticles::storeParticle(const LHCb::Particle* particle)
{
  StatusCode sc(StatusCode::SUCCESS);

  verbose() << "StoreParticles clone into local store"<< endmsg;

  LHCb::Particle* particleClone = 
    cloneKeyedItemToLocalStore<LHCb::Particle, ParticleItemCloner>(particle);

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
    cloneKeyedItemToLocalStore<LHCb::Vertex, VertexItemCloner>(vertex);

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

  return CopyAndStoreData::finalize();  // must be called after all other actions
}
//=============================================================================
