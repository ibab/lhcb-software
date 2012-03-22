// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/Stat.h"

// Event 
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

// local 
#include "GiGaGetEventAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GiGaGetEvent
// 
// 2004-02-20 : Vanya Belyaev 
// 2005-02-02 : Gloria Corti   
// 2006-01-16 : Gloria Corti
// 2007-01-11 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GiGaGetEventAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaGetEventAlg::GiGaGetEventAlg(const std::string& name, 
                                 ISvcLocator* pSvcLocator) 
  : GaudiAlgorithm( name , pSvcLocator) 
{ 
  declareProperty( "Particles", m_particles = LHCb::MCParticleLocation::Default); 
  declareProperty( "Vertices" , m_vertices = LHCb::MCVertexLocation::Default); 
}

//=============================================================================
// Destructor
//=============================================================================
GiGaGetEventAlg::~GiGaGetEventAlg(){};

//=============================================================================
// Main execution
//=============================================================================
StatusCode GiGaGetEventAlg::execute() 
{
  if ( !m_particles.empty() )
  {
    const LHCb::MCParticles* particles = get<LHCb::MCParticles>( m_particles );
    info() << "Number of extracted MCParticles '"
           << m_particles << "' \t"
           << particles -> size() 
           << endmsg ;
    Stat stat( chronoSvc(), "#MCParticles", particles->size() );
  }
  
  if( !m_vertices.empty() )
  {
    const LHCb::MCVertices* vertices = get<LHCb::MCVertices>( m_vertices ) ;
    info() << "Number of extracted MCVertices  '"
           << m_vertices << "'  \t" 
           << vertices -> size() 
           << endmsg ;
    Stat stat( chronoSvc(), "#MCVertices", vertices->size() );
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
