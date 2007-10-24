// $Id: CopyProtoParticles.cpp,v 1.3 2007-10-24 15:38:35 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/RecVertex.h"
// local
#include "CopyProtoParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyProtoParticles
//
// 2007-10-15 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyProtoParticles );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyProtoParticles::CopyProtoParticles( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : CopyAndStoreData ( name , pSvcLocator )
{
}
//=============================================================================
// Destructor
//=============================================================================
CopyProtoParticles::~CopyProtoParticles() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyProtoParticles::initialize() {

  StatusCode sc = CopyAndStoreData::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyProtoParticles::execute() {

  debug() << "==> Execute" << endmsg;
  verbose() << "Going to store ProtoParticles starting from " 
            << inputTESLocation() << endmsg;

  const LHCb::Particle::Container* particles = 
    get<LHCb::Particle::Container>( inputTESLocation() );

  scanParticleSet<LHCb::Particle::Container>( particles->begin(), 
                                              particles->end()   );

  return StatusCode::SUCCESS;

}
//=============================================================================
void CopyProtoParticles::scanParticleTree(const LHCb::Particle* particle) 
{

  storeProtoParticle(particle);

  typedef SmartRefVector<LHCb::Particle> ParticleVector;

  const ParticleVector& daughters = particle->daughters();

  scanParticleSet<ParticleVector>( daughters.begin(), daughters.end() );
  
  if ( ! particle->endVertex() ) return;
  
  const ParticleVector& outParticles = 
    particle->endVertex()->outgoingParticles();

  scanParticleSet<ParticleVector>( outParticles.begin(), outParticles.end() );
  
}
//=============================================================================
template <class T>
void CopyProtoParticles::scanParticleSet(typename T::const_iterator begin,
                                         typename T::const_iterator end) 
{

  if ( begin==end ) return;

  for (typename T::const_iterator iPart = begin; iPart != end; ++iPart ) {
    scanParticleTree(*iPart);
  }

}
//=============================================================================
const LHCb::ProtoParticle* CopyProtoParticles::storeProtoParticle(const LHCb::Particle* particle)
{
  // JPP. Here we make the assumption that something might be wrong with the
  // ProtoParticle of the cloned Particle, so we get it from the original,
  // clone it, and then pass it to the cloned Particle. 
  // Should check if this is really necessary.

  const LHCb::ProtoParticle* proto = particle->proto();
  LHCb::Particle* particleClone = getStoredClone<LHCb::Particle>(particle);

  if ( proto && particleClone ) {

    const std::string from = objectLocation(proto->parent());
    const std::string to = outputTESLocation(from);

    const LHCb::ProtoParticle* ppClone = 
      cloneKeyedContainerItem<LHCb::ProtoParticle, PPCloneFunctor>(particle->proto(), to );
    
    particleClone->setProto(ppClone);

    // loop over daughters.

    return ppClone;

  } else {
    return 0;
  }
   
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyProtoParticles::finalize() {

  debug() << "==> Finalize" << endmsg;

  return CopyAndStoreData::finalize();  // must be called after all other actions
}
//=============================================================================
