// $Id: ParticleCloner.cpp,v 1.10 2010-08-11 12:52:52 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "ParticleCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleCloner
//
// 2007-11-30 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================

ParticleCloner::ParticleCloner( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : base_class         ( type, name, parent ),
    m_vertexCloner     ( NULL               ),
    m_ppCloner         ( NULL               )
{
  declareProperty("ICloneVertex",        m_vertexClonerName="VertexCloner"   );
  declareProperty("ICloneProtoParticle", m_ppClonerName="ProtoParticleCloner");
}

//=============================================================================

ParticleCloner::~ParticleCloner() {}

//=============================================================================

StatusCode ParticleCloner::initialize()
{
  const StatusCode sc = base_class::initialize();
  if ( sc.isFailure() ) return sc;

  debug() << "Going to initialise ICloneProtoParticle and ICloneVertex" << endmsg;

  m_vertexCloner = ( m_vertexClonerName != "NONE" ?
                     tool<ICloneVertex>(m_vertexClonerName, this->parent() ) : NULL );

  m_ppCloner = ( m_ppClonerName != "NONE"  ?
                 tool<ICloneProtoParticle>(m_ppClonerName, this->parent() ) : NULL );

  if (m_ppCloner)
  {
    debug() << "Found ICloneProtoParticle " << m_ppClonerName << endmsg;
  }
  else
  {
    debug() << "Did not find ICloneProtoParticle '" << m_ppClonerName
            << "'. ProtoParticle cloning de-activated."<< endmsg;
  }

  return sc;
}

//=============================================================================

LHCb::Particle* ParticleCloner::operator() (const LHCb::Particle* particle)
{
  return this->clone(particle);
}

//=============================================================================

LHCb::Particle* ParticleCloner::clone(const LHCb::Particle* particle)
{
  if ( !particle ) 
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Particle pointer is NULL !" << endmsg;
    return NULL;
  }

  LHCb::Particle* particleClone =
    cloneKeyedContainerItem<BasicParticleCloner>(particle);

  if ( particleClone )
  {
    // vertex
    if ( m_vertexCloner )
    {
      particleClone->setEndVertex( (*m_vertexCloner)(particle->endVertex()) );
    }
    else
    {
      particleClone->setEndVertex( SmartRef<LHCb::Vertex>(particle->endVertex()) );
    }
    
    // Daughters
    storeDaughters( particleClone, particle->daughters() );
    
    // ProtoParticle
    const LHCb::ProtoParticle * protoToSet = NULL;
    if ( m_ppCloner && particle->proto() )
    {
      protoToSet = (*m_ppCloner)( particle->proto() );
    }
    if ( protoToSet ) { particleClone->setProto(protoToSet); }
    
  }

  return particleClone;
}

//=============================================================================

void 
ParticleCloner::storeDaughters( LHCb::Particle* particleClone,
                                const SmartRefVector<LHCb::Particle>& daughters )
{
  particleClone->clearDaughters();

  for ( SmartRefVector<LHCb::Particle>::const_iterator dau = daughters.begin();
        dau != daughters.end(); ++dau )
  {
    LHCb::Particle* daughterClone = this->clone( *dau );
    if ( daughterClone ) { particleClone->addToDaughters(daughterClone); }
  }
}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ParticleCloner )

//=============================================================================
