// $Id: P2MCRelatorAlg.cpp,v 1.6 2010-05-28 12:15:07 jpalac Exp $
// Include files


// local
#include "P2MCRelatorAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : P2MCRelatorAlg
//
// 2009-04-16 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
P2MCRelatorAlg::P2MCRelatorAlg( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_mcpLocation(LHCb::MCParticleLocation::Default),
    m_p2mcp(NULL),
    m_p2mcpType("MCMatchObjP2MCRelator")
{
  declareProperty( "ParticleLocations",  m_particleLocations );
  declareProperty( "MCParticleLocation", m_mcpLocation       );
  declareProperty( "IP2MCP",             m_p2mcpType         );
  //setProperty( "OutputLevel", 2 );
}

//=============================================================================
// Destructor
//=============================================================================
P2MCRelatorAlg::~P2MCRelatorAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode P2MCRelatorAlg::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  m_p2mcp = tool<IP2MCP>( m_p2mcpType, "MCMatcherForP2MCRelatorAlg" );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode P2MCRelatorAlg::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  m_mcParticles = getIfExists<LHCb::MCParticle::Container>( m_mcpLocation, false );
  if ( !m_mcParticles )
  {
    return Warning( "Found no MCParticles in "+ m_mcpLocation,
                    StatusCode::SUCCESS, 0 );
  }

  // Clear the tables
  m_tables.clear();

  // Create and fill the tables
  typedef std::vector<std::string> StringVector;
  for ( StringVector::const_iterator iLoc = m_particleLocations.begin();
        iLoc != m_particleLocations.end(); ++iLoc )
  {
    const LHCb::Particle::Range particles = getIfExists<LHCb::Particle::Range>(*iLoc);
    if ( !particles.empty() )
    {
      i_particleLoop( particles.begin(), particles.end() );
    }
  }

  // Loop over the final saved tables and save them
  for ( Tables::iterator iT = m_tables.begin(); iT != m_tables.end(); ++iT )
  {
    (*iT).second.i_sort();
    Particle2MCParticle::Table * table = new Particle2MCParticle::Table( (*iT).second );
    put( table, (*iT).first );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================

std::string P2MCRelatorAlg::relationsLocation( const DataObject * pObj ) const
{
  // Get the parent container TES location
  std::string pLoc = objectLocation( pObj );
  if ( pLoc.empty() ) { return pLoc; }

  // Form the relations TES location for this location
  boost::replace_all( pLoc, "/Particles", "/P2MCPRelations" );

  // return the new location
  return pLoc;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( P2MCRelatorAlg )

//=============================================================================
