// $Id: CopyRelatedMCParticles.cpp,v 1.18 2009-03-20 16:11:08 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Linker/LinkedTo.h"
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
  m_cloner(0),
  m_mcParticleClonerName("MCParticleCloner")
{
  declareProperty( "ICloneMCParticle", m_mcParticleClonerName );
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

  const std::vector<std::string> location(1, inputTESLocation() );

  m_cloner = tool<ICloneMCParticle>(m_mcParticleClonerName, this);

  m_compositeLinker = new Particle2MCLinker(this,
                                            Particle2MCMethod::Composite,
                                            location );

  m_linksLinker     = new Particle2MCLinker(this,
                                            Particle2MCMethod::Links,
                                            location);
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

  m_relations = new MCRelations();

  verbose() << "Found " << particles->size() << " particles" << endmsg;

  verbose() << "Make Linker for " << inputTESLocation()  << endmsg;

  //  const std::string relationsLoc = fullOutputTESLocation() + "/RelatedMCParticles";

  const std::string relationsLoc = MicroDST::trunkLocation( fullOutputTESLocation() ) 
    + "/P2MCPRelations";

  verbose() << "Storing relations table at " << relationsLoc << endmsg;

  put(m_relations, relationsLoc);

  verbose() << "loopOnParticles" << endmsg;

  StatusCode sc = loopOnParticles( particles->begin(), particles->end() );

  setFilterPassed(true);  

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

  Particle2MCLinker::ToRange assocMCPs;

  const std::string location = MicroDST::objectLocation(particle->parent() );

  StatusCode sc = associatedMCParticles(particle, assocMCPs);

  debug() << "Going to clone " << assocMCPs.size() 
          << " associated MCParticles to particle from " 
          << location << endmsg;

//   debug() << "Make linker" << endmsg;

//   MCLinker linker(evtSvc(), msgSvc(), location );

//   debug() << "Linking "<<  assocMCPs.size()
//           <<" MCParticles to Particle with location "<< location << endmsg;

  for (Particle2MCLinker::ToRange::iterator iMCP = assocMCPs.begin();
       iMCP != assocMCPs.end();
       ++iMCP) {
    verbose() << "CLONING MCParticle!" << endmsg;
    const LHCb::MCParticle* clonedMCParticle = (*m_cloner)(iMCP->to());
    verbose() << "CLONED MCParticle\n " << *clonedMCParticle << endmsg;
    verbose() << "Link particle from "<< location 
              <<" to MCParticle from " 
              << MicroDST::objectLocation( clonedMCParticle->parent() ) 
              << endmsg;
//     linker.link(particle, clonedMCParticle);
    verbose() << "Linked cloned MCParticle" << endmsg;
    LHCb::Particle* clonedParticle = getStoredClone<LHCb::Particle>(particle);
    if (particle) {
      m_relations->relate(clonedParticle, clonedMCParticle, 1.);
    } else {
      error() << "Particle has not been cloned, make sure you clone them first!" << endmsg;   
    } 
  }

  return sc;

}
//=============================================================================
StatusCode 
CopyRelatedMCParticles::associatedMCParticles(const LHCb::Particle* particle,
                                              Particle2MCLinker::ToRange& assocMCPs)
{

  verbose() << "Choose linker" << endmsg;
  
  Particle2MCLinker* linker = (particle->isBasicParticle() ) ? m_linksLinker : m_compositeLinker;

  if ( !assocMCPs.empty() ) assocMCPs.clear();

  if ( linker->isAssociated(particle) ) {
    verbose() << "Found associations"<< endmsg;
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
