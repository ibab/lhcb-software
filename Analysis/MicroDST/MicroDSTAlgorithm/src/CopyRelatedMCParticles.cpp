// $Id: CopyRelatedMCParticles.cpp,v 1.7 2007-12-20 16:19:06 hmdegaud Exp $
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
  MicroDSTAlgorithm ( name , pSvcLocator ),
  m_deepCloneMCVertex(false)
  //  m_cloner( &localDataStore(), 
  //            MCParticleCloner( &localDataStore() ) )
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

  StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  //  m_cloner = 
  //    new MicroDST::CloneKeyedContainerItemToStore<LHCb::MCParticle,MCParticleCloner>(  &localDataStore(),
  //                                                                                      MCParticleCloner( &localDataStore() ) );
  

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (inputTESLocation()=="")  {
    error() << "No TES location " << endmsg;
    return StatusCode::FAILURE;
  }
  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  m_cloner = tool<ICloneMCParticle>("MCParticleCloner", this);

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

  return StatusCode::SUCCESS;

}
//=============================================================================
template <class IT>
StatusCode CopyRelatedMCParticles::loopOnParticles(IT begin, IT end) 
{

  StatusCode sc(StatusCode::SUCCESS);
  
  for (IT iPart = begin;
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
    (*m_cloner)(*iMCP);
  }

  return sc;

}
//=============================================================================
StatusCode 
CopyRelatedMCParticles::associatedMCParticles(const LHCb::Particle* particle,
                                              LHCb::MCParticle::ConstVector&) 
{

  Particle2MCLinker* linker = (particle->isBasicParticle() ) ? m_linksLinker : m_compositeLinker;

  return StatusCode::SUCCESS;
  
}
//=============================================================================
//  Finalize
//=============================================================================

StatusCode CopyRelatedMCParticles::finalize() {

  debug() << "==> Finalize" << endmsg;

  if (0!=m_cloner) {
    delete m_cloner;
    m_cloner = 0;
  }
  

  return MicroDSTAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
