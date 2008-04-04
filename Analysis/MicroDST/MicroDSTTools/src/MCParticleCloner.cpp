// $Id: MCParticleCloner.cpp,v 1.3 2008-04-04 14:07:31 jpalac Exp $
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
LHCb::MCParticle* MCParticleCloner::clone(const LHCb::MCParticle* mcp,
                                          bool cloneOriginVertex=true)
{

  debug() << "clone() called for\n " << *mcp << endmsg;
  

  LHCb::MCParticle* clone = 
    cloneKeyedContainerItem<LHCb::MCParticle, BasicMCPCloner>(mcp);

  const LHCb::MCParticle* mother = mcp->mother();

  LHCb::MCParticle* motherClone = (mother) ? 
    cloneKeyedContainerItem<LHCb::MCParticle, BasicMCPCloner>(mother) : 0;

  if (cloneOriginVertex) {

    const LHCb::MCVertex* originVertex = mcp->originVertex();  

    if (originVertex) {
      debug() << "Found origin vertex\n " << *originVertex << endmsg;
      LHCb::MCVertex* originVertexClone = cloneVertexTree(originVertex);
      clone->setOriginVertex(originVertexClone);
      originVertexClone->setMother(motherClone);  
    } else {
      debug() << "No originVertex found"<< endmsg;
    }
  }

  clone->clearEndVertices();

  const SmartRefVector< LHCb::MCVertex >& endVtx = mcp->endVertices();

  for (SmartRefVector<LHCb::MCVertex>::const_iterator iEndVtx =endVtx.begin(); 
       iEndVtx!=endVtx.end(); 
       ++iEndVtx) {
    if (m_cloneDecayVertices && (*iEndVtx)->isDecay() ) {
      LHCb::MCVertex* decayVertexClone = (*m_vertexCloner)(*iEndVtx);
      clone->addToEndVertices(decayVertexClone);
    } else {
      clone->addToEndVertices(*iEndVtx);
    }
    
  }
  


  return clone;

}
//=============================================================================
LHCb::MCVertex* MCParticleCloner::cloneVertexTree(const LHCb::MCVertex* mcVertex)
{
  LHCb::MCVertex* clone = (*m_vertexCloner)(mcVertex);
 
  clone->clearProducts();
  const SmartRefVector<LHCb::MCParticle>& products = mcVertex->products();
  for (SmartRefVector<LHCb::MCParticle>::const_iterator iProd = products.begin();
       iProd != products.end();
       ++iProd) {
    debug() << "cloning Vertex Tree product\n" << *(*iProd)<< endmsg;
    // somehow have to switch off the storing of the origin vertex in
    // this cloning process!
    LHCb::MCParticle* productClone = this->clone(*iProd, false);
    debug() << "cloned Vertex Tree product" << endmsg;
    if (productClone) clone->addToProducts(productClone);
  }

  return clone;

}
//=============================================================================
LHCb::MCParticle* MCParticleCloner::operator() (const LHCb::MCParticle* mcp) 
{
  return this->clone(mcp);
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
