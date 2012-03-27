// $Id: VertexCloner.cpp,v 1.5 2010-08-11 12:52:52 jpalac Exp $
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

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VertexCloner::VertexCloner( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : base_class           ( type, name, parent ),
    m_particleCloner     ( NULL ),
    m_particleClonerName ( "ParticleCloner" )
{
  declareProperty("ICloneParticle", m_particleClonerName);
  //setProperty( "OutputLevel", 2 );
}

//=============================================================================

StatusCode VertexCloner::initialize()
{
  const StatusCode sc = base_class::initialize();
  if ( sc.isFailure() ) return sc;

  m_particleCloner = tool<ICloneParticle>(m_particleClonerName,
                                          this->parent() );

  if (m_particleCloner)
  {
    debug() << "Found ParticleCloner " << m_particleCloner->name() << endmsg;
  }
  else
  {
    error() << "Failed to find ParticleCloner" << endmsg;
  }

  return sc;
}

//=============================================================================

LHCb::Vertex* VertexCloner::operator() (const LHCb::Vertex* vertex)
{
  return this->clone(vertex);
}

//=============================================================================

LHCb::Vertex* VertexCloner::clone(const LHCb::Vertex* vertex)
{
  if ( !vertex ) return NULL;

  LHCb::Vertex* vertexClone =
    cloneKeyedContainerItem<BasicVertexCloner>(vertex);

  vertexClone->clearOutgoingParticles();

  const SmartRefVector<LHCb::Particle> & outParticles = vertex->outgoingParticles();
  for ( SmartRefVector<LHCb::Particle>::const_iterator iPart = outParticles.begin();
        iPart != outParticles.end(); ++iPart )
  {
    LHCb::Particle* particleClone = (*m_particleCloner)(*iPart);
    if (particleClone) { vertexClone->addToOutgoingParticles(particleClone); }
  }

  return vertexClone;
}

//=============================================================================
// Destructor
//=============================================================================
VertexCloner::~VertexCloner() {}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VertexCloner )
