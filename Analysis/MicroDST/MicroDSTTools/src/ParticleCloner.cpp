// $Id: ParticleCloner.cpp,v 1.8 2009-08-25 07:03:37 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from LHCb
#include "Event/Particle.h"
#include "Event/Vertex.h"

// From MicroDST
#include <MicroDST/ICloneVertex.h>
#include <MicroDST/ICloneProtoParticle.h>

// local
#include "ParticleCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleCloner
//
// 2007-11-30 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ParticleCloner );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleCloner::ParticleCloner( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : 
  //  base_class ( type, name , parent ),
  base_class( type, name, parent),
  m_vertexCloner(0),
  m_vertexClonerName("VertexCloner"),
  m_ppCloner(0),
  m_ppClonerName("ProtoParticleCloner")
{

  declareProperty("ICloneVertex", m_vertexClonerName);
  declareProperty("ICloneProtoParticle", m_ppClonerName);

}
//=============================================================================
StatusCode ParticleCloner::initialize() 
{

  debug() << "==> Initialize" << endmsg;

  StatusCode sc = base_class::initialize();

  debug() << "Going to initialise ICloneProtoParticle and ICloneVertex" << endmsg;
  
  if (! sc.isSuccess() ) return sc;
  
  m_vertexCloner = (m_vertexClonerName!="NONE") ?  
    tool<ICloneVertex>(m_vertexClonerName, this->parent() ) : 0;

  m_ppCloner = (m_ppClonerName!="NONE") ? 
    tool<ICloneProtoParticle>(m_ppClonerName, this->parent() ) : 0;

  if (m_ppCloner) {
    verbose() << "Found ICloneProtoParticle " << m_ppClonerName << endmsg;
  } else {
    warning() << "Did not find ICloneProtoParticle " << m_ppClonerName 
              <<". ProtoParticle cloning de-activated."<< endmsg;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
LHCb::Particle* ParticleCloner::operator() (const LHCb::Particle* particle)
{
  return this->clone(particle);
}
//=============================================================================
LHCb::Particle* ParticleCloner::clone(const LHCb::Particle* particle)
{

  if (0==particle) return 0;

  LHCb::Particle* particleClone = 
    cloneKeyedContainerItem<LHCb::Particle, BasicParticleCloner>(particle);

  if ( 0 == particleClone ) return 0;

  if (m_vertexCloner) {
    particleClone->setEndVertex( (*m_vertexCloner)(particle->endVertex()) );
  } else {
    particleClone->setEndVertex(SmartRef<LHCb::Vertex>(particle->endVertex()));
  }

  storeDaughters( particleClone, particle->daughters() );

  if (m_ppCloner) {
    particleClone->setProto( (*m_ppCloner)( particle->proto() ) );
  } //   else {
//     particleClone->setProto(SmartRef<LHCb::ProtoParticle>(particle->proto()));
//   }

  return particleClone;
  
}
//=============================================================================
void ParticleCloner::storeDaughters(LHCb::Particle* particleClone,
                                    const SmartRefVector<LHCb::Particle>& daughters) 
{

  particleClone->clearDaughters();

  for (SmartRefVector<LHCb::Particle>::const_iterator dau = daughters.begin();
       dau != daughters.end();
       ++dau ) {
    LHCb::Particle* daughterClone = this->clone( *dau);
    if (daughterClone) particleClone->addToDaughters(daughterClone);
  }

}
//=============================================================================
// Destructor
//=============================================================================
ParticleCloner::~ParticleCloner() {} 

//=============================================================================
