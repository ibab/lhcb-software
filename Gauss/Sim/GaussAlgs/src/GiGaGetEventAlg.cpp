// $Id: GiGaGetEventAlg.cpp,v 1.2 2005-02-02 15:05:54 gcorti Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2004/02/20 19:43:29  ibelyaev
// New package 
//
//  ===========================================================================
#define GIGACNV_GIGAMISCGETEVENTALG_CPP
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Stat.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// ============================================================================
// local 
// ============================================================================
#include "GiGaGetEventAlg.h"
// ============================================================================

// ============================================================================
/** @file
 *  
 *  implementation of class GiGaGetEvent 
 *
 *  @author Vany aBelyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
/// mandatory factory stuff
// ============================================================================
static const  AlgFactory<GiGaGetEventAlg>         s_Factory;
const        IAlgFactory&GiGaGetEventAlgFactory = s_Factory;

// ============================================================================
/** standard constructor
 *  @param name name of teh algorithm 
 *  @param SvcLoc pointer to service locator 
 */
// ============================================================================
GiGaGetEventAlg::GiGaGetEventAlg(const std::string& name, 
                                 ISvcLocator* pSvcLocator) 
  ///
  : GaudiAlgorithm( name , pSvcLocator) 
  , m_particles   ( MCParticleLocation::Default )
  , m_vertices    ( MCVertexLocation::Default   )
{ 
  declareProperty( "Particles" , m_particles  ) ; 
  declareProperty( "Vertices"  , m_vertices   ) ; 
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaGetEventAlg::~GiGaGetEventAlg(){};
// ============================================================================

// ============================================================================
/** the standard Algorithm execution method 
 *  @see Algorithm
 *  @return status code
 */
// ============================================================================
StatusCode GiGaGetEventAlg::execute() 
{
  if ( !m_particles.empty() )
  {
    const MCParticles* particles = get<MCParticles> ( m_particles ) ;
    info() << " Number of extracted MCParticles '"
           << m_particles << "' \t"
           << particles -> size() 
           << endreq ;
    Stat stat( chronoSvc(), "#MCParticles", particles->size() );
  }
  
  if( !m_vertices.empty() )
  {
    const MCVertices* vertices = get<MCVertices> ( m_vertices ) ;
    info() << " Number of extracted MCvertices  '"
           << m_vertices << "'  \t" 
           << vertices -> size() 
           << endreq ;
    Stat stat( chronoSvc(), "#MCVertices", vertices->size() );
  }
  
  return StatusCode::SUCCESS;
};
// ============================================================================


// ============================================================================
// The End 
// ============================================================================








