// $Id: P2MCRelatorAlg.cpp,v 1.6 2010-05-28 12:15:07 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

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
                                ISvcLocator* pSvcLocator)
  :
  GaudiAlgorithm ( name , pSvcLocator ),
  m_particleLocations(),
  m_mcpLocation(LHCb::MCParticleLocation::Default),
  m_p2mcp(0),
  m_p2mcpType("MCMatchObjP2MCRelator"),
  m_table()
{
  m_particleLocations.clear();
  declareProperty("ParticleLocations", m_particleLocations);
  declareProperty("MCParticleLocation", m_mcpLocation);
  declareProperty("IP2MCP", m_p2mcpType);
}
//=============================================================================
// Destructor
//=============================================================================
P2MCRelatorAlg::~P2MCRelatorAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode P2MCRelatorAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_p2mcp = tool<IP2MCP>( m_p2mcpType, this );

  return (m_p2mcp) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode P2MCRelatorAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  m_mcParticles = get<LHCb::MCParticle::Container>( m_mcpLocation );
  if (0==m_mcParticles) {
    return Warning("Found no MCParticles in "+ m_mcpLocation,
                   StatusCode::SUCCESS, 0);
  }
  typedef std::vector<std::string> StringVector;
  StringVector::const_iterator _begin = m_particleLocations.begin();
  StringVector::const_iterator _end = m_particleLocations.end();

  for (StringVector::const_iterator iLoc = _begin; iLoc!=_end; ++iLoc) {

    m_table.clear();
    if (exist<LHCb::Particle::Range>(*iLoc) ) {
      const LHCb::Particle::Range particles = get<LHCb::Particle::Range>(*iLoc);
      if (!particles.empty()) {
        i_particleLoop( particles.begin(), particles.end() );
        m_table.i_sort();
      } else {
        Warning("Empty Particle::Range in "+ *iLoc,
                StatusCode::SUCCESS, 0).ignore();
      }


      Particle2MCParticle::Table* table = new Particle2MCParticle::Table(m_table);
      const std::string outputLocation =
        trunkLocation(*iLoc) + "/P2MCPRelations";

      put(table, outputLocation);
    } else {
      Warning("Found no Particles in "+ *iLoc,
              StatusCode::SUCCESS, 0).ignore();
    }

  }

  return StatusCode::SUCCESS;
}
//=============================================================================
std::string P2MCRelatorAlg::trunkLocation(const std::string& location) const
{
  const std::string::size_type pos = location.rfind("/");
  if ( pos != std::string::npos ) {
    return std::string(location, 0, pos);
  } else {
    return std::string(location);
  }

}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode P2MCRelatorAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( P2MCRelatorAlg )
