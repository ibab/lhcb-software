// $Id: MCParticleCloner.cpp,v 1.9 2010-08-11 12:52:52 jpalac Exp $
// Include files

// from Gaudi
#include <GaudiKernel/ToolFactory.h>

// LHCb
#include "Event/MCParticle.h"

// MicroDST
#include <MicroDST/Functors.hpp>
// local
#include "MCParticleCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCParticleCloner
//
// 2007-11-30 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCParticleCloner::MCParticleCloner( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : base_class ( type, name , parent ),
    m_vertexClonerName("NONE")
{
  declareProperty("ICloneMCVertex", m_vertexClonerName);
}

//=============================================================================

StatusCode MCParticleCloner::initialize()
{
  const StatusCode sc = base_class::initialize();
  if ( sc.isFailure() ) return sc;

  m_vertexCloner = ( m_vertexClonerName != "NONE"      ?
                     this->tool<ICloneMCVertex>( m_vertexClonerName,
                                                 this->parent()    )  :
                     NULL );

  return sc;
}

//=============================================================================

LHCb::MCParticle* MCParticleCloner::clone(const LHCb::MCParticle* mcp)
{
  if (!mcp) return NULL;

  debug() << "clone() called for\n " << *mcp << endmsg;

  LHCb::MCParticle* clone =
    cloneKeyedContainerItem<BasicMCPCloner>(mcp);

  const LHCb::MCVertex* originVertex = mcp->originVertex();

  typedef MicroDST::BasicItemCloner<LHCb::MCVertex> BasicVtxCloner;

  if ( cloneOriginVertex(originVertex) )
  {
    LHCb::MCVertex* originVertexClone = getStoredClone<LHCb::MCVertex>(originVertex);
    if (!originVertexClone)
    {
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Found origin vertex\n " << *originVertex << endmsg;
      originVertexClone =
        cloneKeyedContainerItem<BasicVtxCloner>(originVertex);
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Cloned originVertex" << endmsg;
      originVertexClone->clearProducts();
      originVertexClone->addToProducts(clone);
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Now clone mother" << endmsg;
      const LHCb::MCParticle* mother = mcp->mother();
      LHCb::MCParticle* motherClone = (mother) ? (*this)(mother) : 0;
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Cloned mother" << endmsg;
      originVertexClone->setMother(motherClone);
    }
    clone->setOriginVertex( originVertexClone );
  }
  else
  {
    clone->setOriginVertex(0);
  }

  clone->clearEndVertices();

  cloneDecayVertices(mcp->endVertices(), clone);

  return clone;
}

//=============================================================================

void  MCParticleCloner::cloneDecayVertices(const SmartRefVector<LHCb::MCVertex>& endVertices,
                                           LHCb::MCParticle* clonedParticle)
{
  for ( SmartRefVector<LHCb::MCVertex>::const_iterator iEndVtx = endVertices.begin();
        iEndVtx!=endVertices.end(); ++iEndVtx )
  {
    if ((*iEndVtx)->isDecay() && !((*iEndVtx)->products().empty()) )
    {
      if (m_vertexCloner)
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "Cloning Decay Vertex\n" << *(*iEndVtx)
                    << "\n with " << (*iEndVtx)->products().size()
                    << " products!"<< endmsg;
        LHCb::MCVertex* decayVertexClone = (*m_vertexCloner)(*iEndVtx);
        clonedParticle->addToEndVertices(decayVertexClone);
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "Cloned it!\n" <<  *(*iEndVtx) << endmsg;
      }
      else
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "Copying decay vertex SmartRefs" << endmsg;
        clonedParticle->addToEndVertices(*iEndVtx);
      }
    }
  }
}

//=============================================================================

LHCb::MCParticle* MCParticleCloner::operator() (const LHCb::MCParticle* mcp)
{
  if (!mcp) return NULL;
  LHCb::MCParticle* clone = getStoredClone<LHCb::MCParticle>(mcp);
  return clone ? clone : this->clone(mcp);
}

//=============================================================================
// Destructor
//=============================================================================
MCParticleCloner::~MCParticleCloner() { }

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCParticleCloner )
