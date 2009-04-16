// $Id: MCParticleCloner.cpp,v 1.7 2009-04-16 12:14:47 jpalac Exp $
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
  m_vertexClonerName("NONE") 
{
  declareInterface<ICloneMCParticle>(this);
  declareProperty("ICloneMCVertex", m_vertexClonerName);
}
//=============================================================================
StatusCode MCParticleCloner::initialize()
{

  debug() << "==> Initialize" << endmsg;

  StatusCode sc = MicroDSTTool::initialize();
  
  if (! sc.isSuccess() ) return sc;

  m_vertexCloner = m_vertexClonerName != "NONE"      ? 
    this->tool<ICloneMCVertex>( m_vertexClonerName, 
                                this->parent()    )  :
    0;

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
    LHCb::MCVertex* originVertexClone = getStoredClone<LHCb::MCVertex>(originVertex);
    if (!originVertexClone) {
      debug() << "Found origin vertex\n " << *originVertex << endmsg;
      originVertexClone = 
        cloneKeyedContainerItem<LHCb::MCVertex, BasicVtxCloner>(originVertex);
      debug() << "Cloned originVertex" << endmsg;
      originVertexClone->clearProducts();
      originVertexClone->addToProducts(clone);
      debug() << "Now clone mother" << endmsg;
      const LHCb::MCParticle* mother = mcp->mother();
      LHCb::MCParticle* motherClone = (mother) ? (*this)(mother) : 0;
      debug() << "Cloned mother" << endmsg;

      originVertexClone->setMother(motherClone);
    }
    clone->setOriginVertex( originVertexClone );
  } else {
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
  for (SmartRefVector<LHCb::MCVertex>::const_iterator iEndVtx = endVertices.begin(); 
       iEndVtx!=endVertices.end(); 
       ++iEndVtx) {
    if ((*iEndVtx)->isDecay() && !((*iEndVtx)->products().empty()) ) {
      if (m_vertexCloner) {
        verbose() << "Cloning Decay Vertex\n" << *(*iEndVtx) 
                  << "\n with " << (*iEndVtx)->products().size() 
                  << " products!"<< endmsg;
        LHCb::MCVertex* decayVertexClone = (*m_vertexCloner)(*iEndVtx);
        clonedParticle->addToEndVertices(decayVertexClone);
        verbose() << "Cloned it!\n" <<  *(*iEndVtx) << endmsg;
      } else {
        verbose() << "Copying decay vertex SmartRefs" << endmsg;
        clonedParticle->addToEndVertices(*iEndVtx);
      }      
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
