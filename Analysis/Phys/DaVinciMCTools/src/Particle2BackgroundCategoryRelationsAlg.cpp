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
DECLARE_ALGORITHM_FACTORY( Particle2BackgroundCategoryRelationsAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2BackgroundCategoryRelationsAlg::
Particle2BackgroundCategoryRelationsAlg( const std::string& name,
                                         ISvcLocator* pSvcLocator )
  : 
  GaudiAlgorithm      ( name , pSvcLocator ),
  m_particleLocations (                    ),
  m_bkg               ( NULL               )  
{
  declareProperty( "Inputs", m_particleLocations );
  declareProperty( "FullTree", m_fullTree = false );
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
  const StatusCode sc = GaudiAlgorithm::initialize();

  if ( sc.isFailure() ) return sc; 

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_bkg = tool<IBackgroundCategory>( "BackgroundCategory", this );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2BackgroundCategoryRelationsAlg::execute() 
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  //Check that we have an input location
  if ( m_particleLocations.empty())
  {
    return Error ( "No particle location provided" ) ;
  }

  for ( std::vector<std::string>::const_iterator iLoc = m_particleLocations.begin(); 
        iLoc != m_particleLocations.end(); ++iLoc )
  {
    const StatusCode sc = backCategoriseParticles(*iLoc);
    if ( sc.isFailure() ) 
      return Error("Problem BackgroundCategorizing "+ *iLoc,sc);
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode 
Particle2BackgroundCategoryRelationsAlg::
backCategoriseParticles(const std::string& location) const 
{

  //Check that we have an input location
  if ( location.empty() ) 
  {
    return Error ( "No particle location provided" ) ;
  }

  // CRJ : Allow for the possibility there is no data at a given TES location
  //     : Possible when running on uDSTs
  if ( !exist<LHCb::Particle::Range>(location) ) return StatusCode::SUCCESS;

  //Get the input particles
  const LHCb::Particle::Range myParticles = get<LHCb::Particle::Range>(location);
  //Check that this returns something 
  if (myParticles.empty()) 
  {
    return Error ( "No particles at the location provided" );
  }

  // Make the relations table
  LHCb::Relation1D<LHCb::Particle, int>* catRelations =
    new LHCb::Relation1D<LHCb::Particle,int>( myParticles.size() );

  // save
  std::string outputLocation = location;
  DaVinci::StringUtils::removeEnding(outputLocation, "/Particles");
  outputLocation += "/P2BCRelations";
  put( catRelations, outputLocation );

  StatusCode sc = StatusCode::SUCCESS;
  for ( LHCb::Particle::Range::const_iterator iP = myParticles.begin(); 
        iP != myParticles.end(); ++iP )
  {
    sc = sc && backCategoriseParticle( *iP, catRelations );
  }

  return sc;  
}

StatusCode 
Particle2BackgroundCategoryRelationsAlg::
backCategoriseParticle( const LHCb::Particle * particle,
                        LHCb::Relation1D<LHCb::Particle, int>* catRelations ) const
{
  StatusCode sc = StatusCode::SUCCESS;
  const int thisCat = static_cast<int>(m_bkg->category(particle));
  catRelations->i_relate( particle, thisCat );
  if ( m_fullTree )
  {
    const SmartRefVector<LHCb::Particle> & daus = particle->daughters();
    for ( SmartRefVector<LHCb::Particle>::const_iterator iD = daus.begin();
          iD != daus.end(); ++iD )
    {
      sc = sc && backCategoriseParticle( *iD, catRelations );
      if ( sc.isFailure() ) break;
    }
  }
  return sc;
}

