// $Id: MCVertexCloner.cpp,v 1.3 2008-04-23 13:00:31 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// From LHCb
#include <Event/MCVertex.h>
// local
#include "MCVertexCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCVertexCloner
//
// 2007-11-30 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCVertexCloner );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCVertexCloner::MCVertexCloner( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : 
  MicroDSTTool ( type, name , parent ),
  m_particleClonerName("MCParticleCloner")
{
  declareInterface<ICloneMCVertex>(this);
  declareProperty("ICloneMCParticle", m_particleClonerName);

}
//=============================================================================
LHCb::MCVertex* MCVertexCloner::operator() (const LHCb::MCVertex* vertex)
{
  if (0==vertex) return 0;
  LHCb::MCVertex* clone = getStoredClone<LHCb::MCVertex>(vertex);
  return 0 != clone ? clone : this->clone(vertex);

}
//=============================================================================
LHCb::MCVertex* MCVertexCloner::clone(const LHCb::MCVertex* vertex)
{

  LHCb::MCVertex* clone = 
    cloneKeyedContainerItem<LHCb::MCVertex, BasicCloner>(vertex);

  clone->setMother((*m_particleCloner)(vertex->mother()));

  clone->clearProducts();

  cloneDecayProducts(vertex->products(), clone);

  return clone;

}
//=============================================================================
void MCVertexCloner::cloneDecayProducts(const SmartRefVector<LHCb::MCParticle>& products,
                                        LHCb::MCVertex* clonedVertex) 
{
  for (SmartRefVector<LHCb::MCParticle>::const_iterator iProd = products.begin();
       iProd != products.end();
       ++iProd) {
    LHCb::MCParticle* productClone = (*m_particleCloner)(*iProd);
    productClone->setOriginVertex( clonedVertex );
    if (productClone) clonedVertex->addToProducts(productClone );
  }
}
//=============================================================================
StatusCode MCVertexCloner::initialize()
{

  StatusCode sc = MicroDSTTool::initialize();
  
  if (! sc.isSuccess() ) return sc;

  m_particleCloner = tool<ICloneMCParticle>( m_particleClonerName,
                                             this->parent()       );

  return StatusCode::SUCCESS;
  
}
//=============================================================================
//=============================================================================
StatusCode MCVertexCloner::finalize() 
{
  return MicroDSTTool::finalize();
}
//=============================================================================
// Destructor
//=============================================================================
MCVertexCloner::~MCVertexCloner() {} 

//=============================================================================
