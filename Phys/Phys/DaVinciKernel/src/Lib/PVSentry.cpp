// $Id:$ 
// ============================================================================
// Include files 
// ============================================================================
// PhysEvent 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// DaVinciIntefraces
// ============================================================================
#include "Kernel/IDVAlgorithm.h"
// ============================================================================
// local
// ============================================================================
#include "Kernel/PVSentry.h"
// ============================================================================
/** @file 
 *  Implementation file for class DaVinci::PVSentry
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2009-05-05
 *  @see CombineParticles 
 *
 *                    $Revision:$
 *  Last modification $Date:$
 *                 by $Author:$
 */
// ============================================================================
namespace
{
  // ==========================================================================
  inline std::size_t _insert_
  ( std::vector<const LHCb::Particle*>& particles         , 
    const LHCb::Particle*               particle          , 
    const bool                          tree      = false )
  {
    //
    if ( nullptr == particle ) { return 0 ; }    // RETURN 
    //
    const std::size_t old_size = particles.size() ;
    particles.push_back ( particle )  ;
    //
    if ( tree ) 
    {
      typedef SmartRefVector<LHCb::Particle> DAUGHTERS ;
      const DAUGHTERS& daughters = particle->daughters() ;
      for ( DAUGHTERS::const_iterator id = daughters.begin() ; 
            daughters.end() != id ; ++id ) 
      { _insert_ ( particles , *id , tree ) ; } 
    }
    return particles.size() - old_size ;
  } 
  // ==========================================================================
}
// ============================================================================
// Constructor 
// ============================================================================
DaVinci::PVSentry::PVSentry
( const IDVAlgorithm*   parent   ,
  const LHCb::Particle* particle ,
  const bool            tree     )
  : m_parent    ( parent   )
  , m_particles () 
{
  m_particles.reserve ( 10 ) ;
  if ( parent ) { _insert_ ( m_particles , particle , tree ) ; }
}
// ============================================================================
// destructor
// ============================================================================
DaVinci::PVSentry::~PVSentry() 
{ 
  if ( 0 != m_parent && !m_particles.empty() ) 
  {
    for ( const LHCb::Particle* p : m_particles ) 
    { if ( p ) { m_parent->unRelatePV ( p ) ; } } 
  }
  m_particles.clear() ;
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
