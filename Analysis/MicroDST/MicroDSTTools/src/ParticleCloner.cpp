// $Id: ParticleCloner.cpp,v 1.3 2008-02-15 13:05:57 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from LHCb
#include "Event/Particle.h"
#include "Event/Vertex.h"

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
  MicroDSTTool ( type, name , parent ),
  m_vertexCloner(0),
  m_vertexClonerName("VertexCloner")
{

  declareInterface<ICloneParticle>(this);

  declareProperty("ICloneVertex", m_vertexClonerName);

}
//=============================================================================
StatusCode ParticleCloner::initialize() 
{

  debug() << "==> Initialize" << endmsg;

  StatusCode sc = MicroDSTTool::initialize();
  
  if (! sc.isSuccess() ) return sc;
  
  m_vertexCloner = tool<ICloneVertex>(m_vertexClonerName, this->parent() );

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
  LHCb::Particle* particleClone = 
    cloneKeyedContainerItem<LHCb::Particle, BasicParticleCloner>(particle);

  if (particle->endVertex() && particleClone) {
    LHCb::Vertex* vertexClone = (*m_vertexCloner)( particle->endVertex() );
    if (vertexClone) particleClone->setEndVertex(vertexClone);
    storeDaughters( particleClone, particle->daughters() );
  }
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
