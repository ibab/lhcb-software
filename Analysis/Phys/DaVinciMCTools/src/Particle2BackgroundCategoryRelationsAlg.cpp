
// local
#include "Particle2BackgroundCategoryRelationsAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle2BackgroundCategoryRelationsAlg
//
// 2009-11-30 : V. Gligorov
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2BackgroundCategoryRelationsAlg::
Particle2BackgroundCategoryRelationsAlg( const std::string& name,
                                         ISvcLocator* pSvcLocator )
  : GaudiAlgorithm      ( name , pSvcLocator ),
    m_bkg               ( NULL               )
{
  declareProperty( "Inputs", m_particleLocations );
  declareProperty( "FullTree", m_fullTree = false );
  //setProperty( "OutputLevel", 2 );
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

  m_bkg = tool<IBackgroundCategory>( "BackgroundCategory", this );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2BackgroundCategoryRelationsAlg::execute()
{
  //Check that we have an input location
  if ( m_particleLocations.empty() )
  {
    return Error ( "No particle location(s) provided" ) ;
  }

  // Clear the map of relations
  m_catMap.clear();

  for ( std::vector<std::string>::const_iterator iLoc = m_particleLocations.begin();
        iLoc != m_particleLocations.end(); ++iLoc )
  {
    const StatusCode sc = backCategoriseParticles(*iLoc);
    if ( sc.isFailure() )
    {
      return Error("Problem BackgroundCategorizing '" + *iLoc + "'",sc );
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode
Particle2BackgroundCategoryRelationsAlg::
backCategoriseParticles( const std::string& location ) const
{
  // Check that we have an input location
  if ( location.empty() )
  {
    return Error ( "No Particle TES location provided" ) ;
  }

  // Get the input particles
  const LHCb::Particle::Range myParticles = getIfExists<LHCb::Particle::Range>(location);
  // Check that this returns something
  if ( myParticles.empty() ) { return StatusCode::SUCCESS; }

  // Loop over the Particles and save the relations
  StatusCode sc = StatusCode::SUCCESS;
  for ( LHCb::Particle::Range::const_iterator iP = myParticles.begin();
        iP != myParticles.end(); ++iP )
  {
    sc = sc && backCategoriseParticle( *iP );
  }

  // return
  return sc;
}

//=============================================================================

StatusCode
Particle2BackgroundCategoryRelationsAlg::
backCategoriseParticle( const LHCb::Particle * particle,
                        const unsigned int     recurCount ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  // protect against too many recursive calls...
  // (should never ever get this deep, but to just to be safe ...)
  if ( recurCount > 99999 )
  {
    Warning( "Recursive limit in backCategoriseParticle reached. Aborting Particle tree scan" );
    return sc;
  }

  // protect against null pointers
  if ( particle )
  {

    // relate this particle
    CatRelations * catRel = catRelations(particle);
    if ( catRel )
    {
      // Get the background category
      const int thisCat = static_cast<int>(m_bkg->category(particle));

      // Save it
      catRel->i_relate( particle, thisCat );

      // printout
      if ( msgLevel(MSG::VERBOSE) )
      { verbose() << "Particle key=" << particle->key() 
                  << " PID=" << particle->particleID() 
                  << " " << objectLocation(particle->parent())
                  << " BackCat=" << thisCat << endmsg; }
    }

    // if requested, scan the daughters as well
    if ( m_fullTree )
    {
      for ( SmartRefVector<LHCb::Particle>::const_iterator iD = particle->daughters().begin();
            iD != particle->daughters().end(); ++iD )
      {
        sc = sc && backCategoriseParticle( *iD, recurCount+1 );
        if ( sc.isFailure() ) break;
      }
    }

  }

  // return
  return sc;
}

//=============================================================================

Particle2BackgroundCategoryRelationsAlg::CatRelations * 
Particle2BackgroundCategoryRelationsAlg::
catRelations( const LHCb::Particle * particle ) const
{
  // TES location for the Particle
  const std::string pLocation = objectLocation( particle->parent() );

  // Does a relations object already exist ?
  CatMap::const_iterator iM = m_catMap.find(pLocation);
  if ( iM == m_catMap.end() )
  {
    // Make a new relations object
    CatRelations * catRelations = new CatRelations();

    // Form TES location for the new object
    std::string rLoc = pLocation;
    boost::replace_all( rLoc, "/Particles", "/P2BCRelations" );

    // Save to the TES
    put( catRelations, rLoc );

    // Update the map
    m_catMap[pLocation] = catRelations;

    // return the new object
    return catRelations;
  }

  // Return the saved object
  return iM->second;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Particle2BackgroundCategoryRelationsAlg )

//=============================================================================
