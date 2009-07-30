// $Id: VertexCloner.cpp,v 1.4 2009-07-30 10:03:01 jpalac Exp $
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
  base_class ( type, name , parent ),
  m_particleCloner(0),
  m_particleClonerName("ParticleCloner")
{
  declareProperty("ICloneParticle", m_particleClonerName);
}
//=============================================================================
StatusCode VertexCloner::initialize() 
{

  debug() << "==> Initialize" << endmsg;

  StatusCode sc = base_class::initialize();
  
  if (! sc.isSuccess() ) return sc;

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

  if (0==vertex) return 0;
  
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
  return base_class::finalize();
}
//=============================================================================
// Destructor
//=============================================================================
VertexCloner::~VertexCloner() {} 

//=============================================================================
