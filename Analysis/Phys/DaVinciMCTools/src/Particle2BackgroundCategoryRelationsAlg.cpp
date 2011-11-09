// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Relations/Relations.h"
//LHCb
#include "Event/Particle.h"
// DaVinci
#include "Kernel/IBackgroundCategory.h"
#include "Kernel/DaVinciStringUtils.h"
// local
#include "Particle2BackgroundCategoryRelationsAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle2BackgroundCategoryRelationsAlg
//
// 2009-11-30 : V. Gligorov
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Particle2BackgroundCategoryRelationsAlg );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2BackgroundCategoryRelationsAlg::Particle2BackgroundCategoryRelationsAlg( const std::string& name,
                                                                                  ISvcLocator* pSvcLocator)
  : 
  GaudiAlgorithm ( name , pSvcLocator ),
  m_particleLocations(),
  m_bkg(0)  
{
  declareProperty("InputLocations", m_particleLocations);
}
//=============================================================================
// Destructor
//=============================================================================
Particle2BackgroundCategoryRelationsAlg::~Particle2BackgroundCategoryRelationsAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Particle2BackgroundCategoryRelationsAlg::initialize() 
{

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_bkg = tool<IBackgroundCategory>( "BackgroundCategory", this );

  return (0!=m_bkg) ? StatusCode::SUCCESS : StatusCode::FAILURE;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2BackgroundCategoryRelationsAlg::execute() 
{

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  //Check that we have an input location
  if (m_particleLocations.empty()) {
    return Error ( "No particle location provided" ) ;
  }

  std::vector<std::string>::const_iterator _begin = m_particleLocations.begin();

  StatusCode sc = StatusCode::SUCCESS;
  for (std::vector<std::string>::const_iterator iLoc = _begin; iLoc != m_particleLocations.end(); ++iLoc) {
    sc = backCategoriseParticles(*iLoc);
    if (sc.isFailure()) return Error("Problem BackgroundCategorizing "+ *iLoc);
  }
  
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode Particle2BackgroundCategoryRelationsAlg::backCategoriseParticles(const std::string& location) const 
{
  // CRJ : Allow for the possibility there is no data at a given TES location
  //     : Possible when running on uDSTs
  if ( !exist<LHCb::Particle::Range>(location) ) return StatusCode::SUCCESS;
  
  //Check that we have an input location
  if (location == "") 
  {
    return Error ( "No particle location provided" ) ;
  }
  //Get the input particles
  const LHCb::Particle::Range myParticles = get<LHCb::Particle::Range>(location);
  //Check that this returns something 
  if (myParticles.empty()) {
    return Error ( "No particles at the location provided" );
  }

  //Make the relations table
  LHCb::Relation1D<LHCb::Particle, int>* catRelations =
    new LHCb::Relation1D<LHCb::Particle, int>( myParticles.size() );

  for(LHCb::Particle::Range::const_iterator iP = myParticles.begin(); 
      iP != myParticles.end(); ++iP ){
    int thisCat = static_cast<int>(m_bkg->category(*iP));
    catRelations->i_relate(*iP,thisCat);
  }

  std::string outputLocation = location;
  DaVinci::StringUtils::removeEnding(outputLocation, "/Particles");

  put(catRelations, outputLocation+"/P2BCRelations");

  return StatusCode::SUCCESS;
  
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Particle2BackgroundCategoryRelationsAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
