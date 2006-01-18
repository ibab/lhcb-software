// $Id: GiGaGetEventAlg.cpp,v 1.3 2006-01-18 09:12:46 gcorti Exp $ 
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
// 2004-02-20 : I.Belyaev 
// 2005-02-02 : G.Corti   
// 2006-01-16 : G.Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GiGaGetEventAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaGetEventAlg::GiGaGetEventAlg(const std::string& name, 
                                 ISvcLocator* pSvcLocator) 
  ///
  : GaudiAlgorithm( name , pSvcLocator) 
  , m_particles   ( LHCb::MCParticleLocation::Default )
  , m_vertices    ( LHCb::MCVertexLocation::Default )
{ 
  declareProperty( "Particles", m_particles ); 
  declareProperty( "Vertices" , m_vertices ); 
};

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
    info() << " Number of extracted MCParticles '"
           << m_particles << "' \t"
           << particles -> size() 
           << endreq ;
    Stat stat( chronoSvc(), "#MCParticles", particles->size() );
  }
  
  if( !m_vertices.empty() )
  {
    const LHCb::MCVertices* vertices = get<LHCb::MCVertices>( m_vertices ) ;
    info() << " Number of extracted MCvertices  '"
           << m_vertices << "'  \t" 
           << vertices -> size() 
           << endreq ;
    Stat stat( chronoSvc(), "#MCVertices", vertices->size() );
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================








