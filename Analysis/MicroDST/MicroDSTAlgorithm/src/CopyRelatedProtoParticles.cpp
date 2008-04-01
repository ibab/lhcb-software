// $Id: CopyRelatedProtoParticles.cpp,v 1.1 2008-04-01 15:43:57 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/Particle.h"
// from MicroDST
#include "MicroDST/ICloneProtoParticle.h"
// local
#include "CopyRelatedProtoParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyRelatedProtoParticles
//
// 2007-10-15 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyRelatedProtoParticles );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyRelatedProtoParticles::CopyRelatedProtoParticles( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : 
  MicroDSTAlgorithm ( name , pSvcLocator ),
  m_cloner(0),
  m_clonerName("ProtoParticleCloner")
{
  declareProperty( "ICloneProtoParticle", m_clonerName );
}
//=============================================================================
// Destructor
//=============================================================================
CopyRelatedProtoParticles::~CopyRelatedProtoParticles() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyRelatedProtoParticles::initialize() {

  StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  m_cloner = tool<ICloneProtoParticle>(m_clonerName, this);

  if (m_cloner) {
    debug() << "Found ICloneProtoParticle" << endmsg;
  } else {
    error() << "Failed to find IClonerProtoParticle" << endmsg;
  }

  return StatusCode::SUCCESS;

}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyRelatedProtoParticles::execute() {

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
void CopyRelatedProtoParticles::scanParticleTree(const LHCb::Particle* particle) 
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
void CopyRelatedProtoParticles::scanParticleSet(typename T::const_iterator begin,
                                         typename T::const_iterator end) 
{

  if ( begin==end ) return;

  for (typename T::const_iterator iPart = begin; iPart != end; ++iPart ) {
    scanParticleTree(*iPart);
  }

}
//=============================================================================
const LHCb::ProtoParticle* CopyRelatedProtoParticles::storeProtoParticle(const LHCb::Particle* particle)
{

  const LHCb::ProtoParticle* proto = particle->proto();

  LHCb::Particle* particleClone = getStoredClone<LHCb::Particle>(particle);

  if ( proto && particleClone ) {

    const LHCb::ProtoParticle* ppClone = (*m_cloner)( particle->proto() );

    particleClone->setProto(ppClone);

    return ppClone;

  } else {
    return 0;
  }
   
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyRelatedProtoParticles::finalize() {

  debug() << "==> Finalize" << endmsg;

  return MicroDSTAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
