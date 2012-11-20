// $Id: MCVertexCloner.cpp,v 1.6 2010-08-11 12:52:52 jpalac Exp $
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

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCVertexCloner::MCVertexCloner( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : base_class ( type, name , parent ),
    m_particleCloner( NULL )
{
  declareProperty("ICloneMCParticle", m_particleClonerName = "MCParticleCloner" );
  //setProperty( "OutputLevel", 1 );
}

//=============================================================================

StatusCode MCVertexCloner::initialize()
{
  const StatusCode sc = base_class::initialize();
  if ( sc.isFailure() ) return sc;

  m_particleCloner = tool<ICloneMCParticle>( m_particleClonerName,
                                             this->parent() );

  return sc;
}

//=============================================================================

LHCb::MCVertex* MCVertexCloner::operator() (const LHCb::MCVertex* vertex)
{
  if ( !vertex ) return NULL;
  LHCb::MCVertex* clone = getStoredClone<LHCb::MCVertex>(vertex);

  const size_t nProd      = vertex->products().size();
  const size_t nCloneProd = ( clone ? clone->products().size() : 0 );

  return ( clone && (nProd == nCloneProd) ? clone : this->clone(vertex) );
}

//=============================================================================

LHCb::MCVertex* MCVertexCloner::clone(const LHCb::MCVertex* vertex)
{
  LHCb::MCVertex* clone =
    cloneKeyedContainerItem<BasicCloner>(vertex);

  clone->setMother((*m_particleCloner)(vertex->mother()));

  clone->clearProducts();

  cloneDecayProducts( vertex->products(), clone );

  return clone;
}

//=============================================================================

void 
MCVertexCloner::cloneDecayProducts(const SmartRefVector<LHCb::MCParticle>& products,
                                   LHCb::MCVertex* clonedVertex)
{
  for ( SmartRefVector<LHCb::MCParticle>::const_iterator iProd = products.begin();
        iProd != products.end(); ++iProd )
  {
    LHCb::MCParticle* productClone = (*m_particleCloner)(*iProd);
    if ( productClone )
    {
      productClone->setOriginVertex( clonedVertex );
      clonedVertex->addToProducts( productClone );
    }
  }
}

//=============================================================================
// Destructor
//=============================================================================
MCVertexCloner::~MCVertexCloner() {}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCVertexCloner )

//=============================================================================

