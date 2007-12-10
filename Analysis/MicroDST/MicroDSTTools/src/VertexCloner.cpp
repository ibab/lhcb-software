// $Id: VertexCloner.cpp,v 1.1.1.1 2007-12-10 09:32:24 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from LHCb
#include "Event/Particle.h"
#include "Event/Vertex.h"

// local
#include "VertexCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VertexCloner
//
// 2007-11-30 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VertexCloner );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VertexCloner::VertexCloner( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : 
  MicroDSTTool ( type, name , parent ),
  m_particleCloner(0),
  m_particleClonerName("ParticleCloner")
{
  declareInterface<ICloneVertex>(this);

  declareProperty("ICloneParticle", m_particleClonerName);

}
//=============================================================================
StatusCode VertexCloner::initialize() 
{

  debug() << "==> Initialize" << endmsg;

  debug() << "Going to get ParticleCloner" << endmsg;
  
  m_particleCloner = tool<ICloneParticle>(m_particleClonerName, 
                                          this->parent() );

  if (m_particleCloner) {
    debug() << "Found ParticleCloner " << m_particleCloner->name() << endmsg;
  } else {
    error() << "Failed to find ParticleCloner" << endmsg;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
LHCb::Vertex* VertexCloner::operator() (const LHCb::Vertex* vertex)
{
  return this->clone(vertex);
}
//=============================================================================
//=============================================================================
LHCb::Vertex* VertexCloner::clone(const LHCb::Vertex* vertex)
{
  
  LHCb::Vertex* vertexClone = 
    cloneKeyedContainerItem<LHCb::Vertex, BasicVertexCloner>(vertex);
  
  vertexClone->clearOutgoingParticles();

  const SmartRefVector< LHCb::Particle > &outParticles = 
    vertex->outgoingParticles();

  for (SmartRefVector<LHCb::Particle>::const_iterator iPart = outParticles.begin();
       iPart != outParticles.end();
       ++iPart) {
    LHCb::Particle* particleClone = (*m_particleCloner)(*iPart);
    vertexClone->addToOutgoingParticles(particleClone);
  }

  return vertexClone;
  
}
//=============================================================================
StatusCode VertexCloner::finalize() 
{
  return MicroDSTTool::finalize();
}
//=============================================================================
// Destructor
//=============================================================================
VertexCloner::~VertexCloner() {} 

//=============================================================================
