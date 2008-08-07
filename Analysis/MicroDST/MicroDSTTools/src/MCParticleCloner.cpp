// $Id: MCParticleCloner.cpp,v 1.5 2008-08-07 14:08:29 jpalac Exp $
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

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCParticleCloner );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCParticleCloner::MCParticleCloner( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : 
  MicroDSTTool ( type, name , parent ),
  m_vertexClonerName("MCVertexCloner"),
  m_cloneDecayVertices(false)
{
  declareInterface<ICloneMCParticle>(this);
  declareProperty("ICloneMCVertex", m_vertexClonerName);
  declareProperty("CloneDecayVertices", m_cloneDecayVertices);
  
}
//=============================================================================
StatusCode MCParticleCloner::initialize()
{

  debug() << "==> Initialize" << endmsg;

  StatusCode sc = MicroDSTTool::initialize();
  
  if (! sc.isSuccess() ) return sc;

  m_vertexCloner = this->tool<ICloneMCVertex>( m_vertexClonerName, 
                                               this->parent()    );

  return StatusCode::SUCCESS;
  
}
//=============================================================================
LHCb::MCParticle* MCParticleCloner::clone(const LHCb::MCParticle* mcp)
{

  if (0==mcp) return 0;

  debug() << "clone() called for\n " << *mcp << endmsg;

  LHCb::MCParticle* clone = 
    cloneKeyedContainerItem<LHCb::MCParticle, BasicMCPCloner>(mcp);

  const LHCb::MCVertex* originVertex = mcp->originVertex();

  typedef MicroDST::BasicItemCloner<LHCb::MCVertex> BasicVtxCloner;

  if ( cloneOriginVertex(originVertex) ) {
    LHCb::MCVertex* originVertexClone = getStoredClone(originVertex);
    if (!originVertexClone) {
      debug() << "Found origin vertex\n " << *originVertex << endmsg;
      originVertexClone = 
        cloneKeyedContainerItem<LHCb::MCVertex, BasicVtxCloner>(originVertex);
      debug() << "Cloned originVertex" << endmsg;
      originVertexClone->clearProducts();
      originVertexClone->addToProducts(clone);
      debug() << "Now clone mother" << endmsg;
      const LHCb::MCParticle* mother = mcp->mother();
      //      LHCb::MCParticle* motherClone = (mother) ? this->clone(mother) : 0;
      LHCb::MCParticle* motherClone = (mother) ? (*this)(mother) : 0;
      debug() << "Cloned mother" << endmsg;

      originVertexClone->setMother(motherClone);
    }
    clone->setOriginVertex( originVertexClone );
  } else {
    clone->setOriginVertex(0);
  }

  clone->clearEndVertices();

  if (m_cloneDecayVertices) cloneDecayVertices(mcp->endVertices(),
                                               clone);
  
  return clone;

}
//=============================================================================
void  MCParticleCloner::cloneDecayVertices(const SmartRefVector<LHCb::MCVertex>& endVertices,
                                           LHCb::MCParticle* clonedParticle)
{
  for (SmartRefVector<LHCb::MCVertex>::const_iterator iEndVtx = endVertices.begin(); 
       iEndVtx!=endVertices.end(); 
       ++iEndVtx) {
    if ((*iEndVtx)->isDecay() && !((*iEndVtx)->products().empty()) ) {
      verbose() << "Cloning Decay Vertex\n" << *(*iEndVtx) 
                << "\n with " << (*iEndVtx)->products().size() << " products!"<< endmsg;
      LHCb::MCVertex* decayVertexClone = (*m_vertexCloner)(*iEndVtx);
      clonedParticle->addToEndVertices(decayVertexClone);
      verbose() << "Cloned it!\n" <<  *(*iEndVtx) << endmsg;
    }
  }
}
//=============================================================================
LHCb::MCParticle* MCParticleCloner::operator() (const LHCb::MCParticle* mcp) 
{
  if (0==mcp) return 0;
  LHCb::MCParticle* clone = getStoredClone<LHCb::MCParticle>(mcp);
  return 0 != clone ? clone : this->clone(mcp);
}
//=============================================================================
StatusCode MCParticleCloner::finalize() 
{
  return MicroDSTTool::finalize();
}
//=============================================================================
// Destructor
//=============================================================================
MCParticleCloner::~MCParticleCloner() 
{
}
//=============================================================================
