// $Id: FilterByBestParticles.cpp,v 1.1 2004-07-16 11:20:48 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/MsgStream.h" 

// local
#include "FilterByBestParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterByBestParticles
//
// 2004-07-15 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<FilterByBestParticles>          s_factory ;
const        IAlgFactory& FilterByBestParticlesFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterByBestParticles::FilterByBestParticles( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
    , m_best()
{
  declareProperty( "BestFilterName", m_bestFilterName );
}
//=============================================================================
// Destructor
//=============================================================================
FilterByBestParticles::~FilterByBestParticles() {}; 


//=============================================================================
// Main execution
//=============================================================================
StatusCode FilterByBestParticles::execute() {

  debug() << "==> Execute" << endmsg;
  verbose() << "Getting particles" << endreq;

  const ParticleVector parts = desktop()->particles();
  setFilterPassed(false);

  verbose() << "Filtering " << parts.size() << " particles" << endreq;
  ParticleVector filtered;
  StatusCode sc = m_best->Candidates(parts,filtered);
  if ( !sc){
    err() << "Could not filter particles" << endreq;
    return sc;  
  }
  
  verbose() << "Filtered  " << filtered.size() << " particles" << endreq;
  if ( filtered.size()> 0 ) setFilterPassed(true);

  sc = desktop()->saveTrees(filtered);

  return sc;
};

//=============================================================================
// Initialization
//=============================================================================
StatusCode FilterByBestParticles::initialize() {
  StatusCode sc = StatusCode::SUCCESS ; // must be executed first

  debug() << "==> Initialize" << endmsg;

  sc = toolSvc()->retrieveTool(m_bestFilterName,m_best,this);
  if (!sc) {
    err() << "Could not retrieve BestParticlesFilter '" << m_bestFilterName
          << "'" << endreq;
    return sc;
  } else verbose() << "Retrieved BestParticlesFilter '" << m_bestFilterName
                   << "'" << endreq;
  sc = toolSvc()->retrieveTool("ParticleMakerSeq",m_pm,this);

  return StatusCode::SUCCESS;
};
//=============================================================================
//  Finalize
//=============================================================================
StatusCode FilterByBestParticles::finalize() {

  debug() << "==> Finalize" << endmsg;
  if( m_best ) {
    debug() << "Releasing Best Particles tool" << endreq;
    toolSvc()->releaseTool( m_best );
  }
  toolSvc()->releaseTool( m_pm );
  
  return StatusCode::SUCCESS;
  
}

//=============================================================================
