// $Id: MCParticleCloner.cpp,v 1.1.1.1 2007-12-10 09:32:24 jpalac Exp $
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
  MicroDSTTool ( type, name , parent )
{
  declareInterface<ICloneMCParticle>(this);

}
//=============================================================================
StatusCode MCParticleCloner::initialize()
{

  m_vertexCloner = this->tool<ICloneMCVertex>( "MCVertexCloner", 
                                               this->parent()    );

  return StatusCode::SUCCESS;
  
}
//=============================================================================
LHCb::MCParticle* MCParticleCloner::clone(const LHCb::MCParticle* mcp)
{
  //  LHCb::MCParticle* clone = m_mcpCloner(mcp);
  LHCb::MCParticle* clone = 
    cloneKeyedContainerItem<LHCb::MCParticle, BasicMCPCloner>(mcp);

  const LHCb::MCParticle* mother = mcp->mother();

  LHCb::MCParticle* motherClone = (mother) ? 
    cloneKeyedContainerItem<LHCb::MCParticle, BasicMCPCloner>(mother) : 0;

    const LHCb::MCVertex* originVertex = mcp->originVertex();  

    if (originVertex) {
      LHCb::MCVertex* originVertexClone = cloneVertexTree(originVertex);
      clone->setOriginVertex(originVertexClone);
      originVertexClone->setMother(motherClone);  
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

    LHCb::MCParticle* productClone = this->clone(*iProd);
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
// Destructor
//=============================================================================
MCParticleCloner::~MCParticleCloner() 
{
}
//=============================================================================
