// $Id: CopyRelatedMCParticles.cpp,v 1.9 2008-02-12 13:12:51 jpalac Exp $
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
  m_deepCloneMCVertex(false),
  m_cloner(0),
  m_linker(0)
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

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (inputTESLocation()=="")  {
    error() << "No TES location " << endmsg;
    return StatusCode::FAILURE;
  }
  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  m_cloner = tool<ICloneMCParticle>("MCParticleCloner", this);

   m_compositeLinker = new Particle2MCLinker(this,
                                             Particle2MCMethod::Composite,
                                             std::vector<std::string>(1,"") );
  m_linksLinker     = new Particle2MCLinker(this,
                                            Particle2MCMethod::Links,
                                            std::vector<std::string>(1,""))  ;

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

  m_linker = new MCLinker(evtSvc(),
                          msgSvc(),
                          fullOutputTESLocation());

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

  //  LHCb::MCParticle::ConstVector assocMCPs;

  Particle2MCLinker::ToRange assocMCPs;

  StatusCode sc = associatedMCParticles(particle, assocMCPs);

  debug() << "Going to clone " << assocMCPs.size() 
          << " associated MCParticles" << endmsg;

  for (Particle2MCLinker::ToRange::iterator iMCP = assocMCPs.begin();
       iMCP != assocMCPs.end();
       ++iMCP) {
    debug() << "CLONING MCParticle!" << endmsg;
    const LHCb::MCParticle* clonedMCParticle = (*m_cloner)(iMCP->to());
    m_linker->link(particle, clonedMCParticle);
    debug() << "CLONED MCParticle!" << endmsg;
    //
  }

  return sc;

}
//=============================================================================
StatusCode 
CopyRelatedMCParticles::associatedMCParticles(const LHCb::Particle* particle,
                                              Particle2MCLinker::ToRange& assocMCPs)

                                              //LHCb::MCParticle::ConstVector& assocMCPs) 
{

  Particle2MCLinker* linker = (particle->isBasicParticle() ) ? m_linksLinker : m_compositeLinker;

  if ( !assocMCPs.empty() ) assocMCPs.clear();

  if ( linker->isAssociated(particle) ) {
    
  } else {
    debug() << "Found no associated MCParticles for Particle\n" 
            << *particle << endmsg;
    return StatusCode::SUCCESS;
  }

  Particle2MCLinker::ToRange range = linker->rangeFrom(particle);
  
  debug() << "Found " << range.size() 
          << " associated MCParticles for Particle\n" 
          << *particle << endmsg;

  for (Particle2MCLinker::ToRange::iterator toIter = range.begin();
       toIter != range.end();
       ++toIter) {
    //    assocMCPs.push_back( toIter->to() ); 
    assocMCPs.push_back(*toIter);
  }
  
  return StatusCode::SUCCESS;
  
}
//=============================================================================
//  Finalize
//=============================================================================

StatusCode CopyRelatedMCParticles::finalize() {

  debug() << "==> Finalize" << endmsg;

//   if (0!=m_cloner) {
//     delete m_cloner;
//     m_cloner = 0;
//   }
  

  return MicroDSTAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
