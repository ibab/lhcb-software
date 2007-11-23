// $Id: CopyRelatedMCParticles.cpp,v 1.5 2007-11-23 17:59:19 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

// local
#include "CopyRelatedMCParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyRelatedMCParticles
//
// 2007-11-02 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyRelatedMCParticles );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

CopyRelatedMCParticles::CopyRelatedMCParticles( const std::string& name,
                                                ISvcLocator* pSvcLocator )
  : 
  CopyAndStoreData ( name , pSvcLocator ),
  m_deepCloneMCVertex(false)
{
  declareProperty("DeepCloneMCVertex", m_deepCloneMCVertex);
}

//=============================================================================
// Destructor
//=============================================================================

CopyRelatedMCParticles::~CopyRelatedMCParticles() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyRelatedMCParticles::initialize() {

  StatusCode sc = CopyAndStoreData::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (inputTESLocation()=="")  {
    error() << "No TES location " << endmsg;
    return StatusCode::FAILURE;
  }
  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  m_compositeLinker = new Particle2MCLinker(this,
                                            Particle2MCMethod::Composite,"")  ;

  m_linksLinker     = new Particle2MCLinker(this,
                                            Particle2MCMethod::Links,"")  ;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================

StatusCode CopyRelatedMCParticles::execute() {

  debug() << "==> Execute" << endmsg;
  verbose() << "Going to store MCParticles related to Particles from " 
            << inputTESLocation() << endmsg;

  const LHCb::Particle::Container* particles = 
    get<LHCb::Particle::Container>( inputTESLocation() );

  verbose() << "Found " << particles->size() << " particles" << endmsg;

  // Actually this needs to be iterative, so we store the MCParticles for
  // everything in the tree.

  StatusCode sc = loopOnParticles( particles->begin(),
                                   particles->end()    );
  
  setFilterPassed(true);

  verbose() << "Going to store to TES" << endmsg;

  return copyLocalStoreToTES();


}
//=============================================================================
template <class IT>
StatusCode CopyRelatedMCParticles::loopOnParticles(IT begin, IT end) 
{

  StatusCode sc(StatusCode::SUCCESS);
  
  for (IT iPart = begin,;
       sc == StatusCode::SUCCESS && iPart != end;
       ++iPart) {
    if ( storeAssociatedMCParticles( *iPart) ) {
      const SmartRefVector<LHCb::Particle>& daughters = (*iPart)->daughters();
      sc = loopOnParticles(daughters.begin(), daughters.end());
    }
    
  }

  return sc;
  
}
//=============================================================================
StatusCode
CopyRelatedMCParticles::storeAssociatedMCParticles(const LHCb::Particle* particle)
{

  // Get a vector of associated particles

  LHCb::MCParticle::ConstVector assocMCPs;

  StatusCode sc = associatedMCParticles(particle, assocMCPs);

  for (LHCb::MCParticle::ConstVector::const_iterator iMCP = assocMCPs.begin();
       iMCP != assocMCPs.end();
       ++iMCP) {
    storeMCParticle(*iMCP);
    
  }

  // Store clone of mother if available

  // Store origin vertex if desireable.
  
  return sc;

}
//=============================================================================
LHCb::MCParticle* CopyRelatedMCParticles::storeMCParticle(const LHCb::MCParticle* mcp,
                                                          bool storeOriginVertex )
{
  // Store MCParticle clones
  LHCb::MCParticle* mcpClone = 
    cloneKeyedItemToLocalStore<LHCb::MCParticle, MCParticleItemCloner>(mcp);
  // Store clone of mother if available
  const LHCb::MCParticle* mother = mcp->mother();
  LHCb::MCParticle* motherClone = 0;
  
  if (mother) motherClone = 
                cloneKeyedItemToLocalStore<LHCb::MCParticle,MCParticleItemCloner>(mother);
  
  if (storeOriginVertex) {
    // Store origin vertex if desireable.
    const LHCb::MCVertex* originVertex = mcp->originVertex();
    if (originVertex) {
      LHCb::MCVertex* originVertexClone = storeMCVertex(originVertex);
      mcpClone->setOriginVertex(originVertexClone);
      originVertexClone->setMother(motherClone);
    
    }
  } else {
    mcpClone->setOriginVertex( mcp->originVertex() );
  }
  
  
}
//=============================================================================
StatusCode CopyRelatedMCParticles::associatedMCParticles(const LHCb::Particle* particle,
                                                         LHCb::MCParticle::ConstVector&) 
{

  Particle2MCLinker* linker = (particle->isBasicParticle() ) ? m_linksLinker : m_compositeLinker;

  return StatusCode::SUCCESS;
  
}
//=============================================================================
LHCb::MCVertex* CopyRelatedMCParticles::storeMCVertex(const LHCb::MCVertex* vertex) 
{

  LHCb::MCVertex* clone = cloneKeyedItemToLocalStore<LHCb::MCVertex,MCVertexItemCloner>(vertex);

  if ( deepCloneMCVertex() ) {

    clone->clearProducts();    
    const SmartRefVector<LHCb::MCParticle>& products = vertex->products();
    for (SmartRefVector<LHCb::MCParticle>::const_iterator iProd = products.begin();
         iProd != products.end();
         ++iProd) {

      LHCb::MCParticle* productClone = storeMCParticle(*iProd, false);
      if (productClone) clone->addToProducts(productClone);
    }
  }

  return clone;
}
//=============================================================================
//  Finalize
//=============================================================================

StatusCode CopyRelatedMCParticles::finalize() {

  debug() << "==> Finalize" << endmsg;

  return CopyAndStoreData::finalize();  // must be called after all other actions
}
//=============================================================================
