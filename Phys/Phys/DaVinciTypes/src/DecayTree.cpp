// $Id$
// ============================================================================
// Inclide file 
// ============================================================================
// PhysEvent
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// local
// ============================================================================
#include "Kernel/DecayTree.h"
#include "Kernel/TreeCloners.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/foreach.hpp"
// ============================================================================
/** @file
 *  Implementation of class LHCb::DecayTree
 *  @see LHcb::DecayTree
 */
// ============================================================================
/*  constructor from the decay head 
 *  @attention the decay tree is cloned!
 *  @param head the decay tree to be cloned
 */
// ============================================================================
LHCb::DecayTree::DecayTree
( const LHCb::Particle& head )
  : m_head     () 
  , m_clonemap () 
{ 
  m_head.reset ( DaVinci::cloneTree( &head , m_clonemap ) ) ; 
}
// ============================================================================
/*  copy constructor 
 *  @attention "destructive copy" semantics!
 *  @param right object to be copied ( and destroyed!!!) 
 */
// ============================================================================
LHCb::DecayTree::DecayTree  
( const LHCb::DecayTree& right ) 
  : m_head     ( right.m_head     ) 
  , m_clonemap ( right.m_clonemap ) 
{
  right.m_clonemap.clear() ;                       // NB: clear also m_clonemap
}
// ============================================================================
//  default constructor 
// ============================================================================
LHCb::DecayTree::DecayTree () 
  : m_head     ( 0 ) 
  , m_clonemap (   ) 
{}
// ============================================================================
/*  destructor
 *  @attention the cloned decay tree is delete!
 */
// ============================================================================
LHCb::DecayTree::~DecayTree () { DaVinci::deleteTree ( m_head.release() ) ; }
// ============================================================================  
// find clone of particle in original decay tree
// ============================================================================
const LHCb::Particle* 
LHCb::DecayTree::findClone ( const LHCb::Particle& orig ) const 
{
  CloneMap::const_iterator it = m_clonemap.find( &orig ) ;
  return it != m_clonemap.end() ? it->second : 0 ; 
}
// ============================================================================
// find a particle in a decay tree based on PID
// ============================================================================
void LHCb::DecayTree::findInTree
( const LHCb::Particle*        particle ,
  const LHCb::ParticleID&      pid      ,
  LHCb::Particle::ConstVector& result   ) 
{
  if ( 0 == particle ) { return ; }
  //
  if ( particle->particleID() == pid )  { result.push_back( particle ) ; }
  //
  typedef SmartRefVector<LHCb::Particle> SRVP ;
  const SRVP& daughters = particle->daughters() ;
  for ( SRVP::const_iterator idau = daughters.begin() ; 
        daughters.end() != idau ; ++idau ) 
  {
    const LHCb::Particle* dau = *idau ;
    if ( 0 == dau ) { continue ; }
    //
    findInTree ( dau , pid , result ) ;
  }
}
// ============================================================================
// find a particle in a decay tree based on PID
// ============================================================================
const LHCb::Particle* 
LHCb::DecayTree::findFirstInTree
( const LHCb::Particle*   particle ,
  const LHCb::ParticleID& pid      ) 
{
  if ( 0 == particle ) { return 0 ; }
  //
  if ( particle->particleID() == pid ) { return particle ; }
  //
  typedef SmartRefVector<LHCb::Particle> SRVP ;
  const SRVP& daughters = particle->daughters() ;
  for ( SRVP::const_iterator idau = daughters.begin() ; 
        daughters.end() != idau ; ++idau ) 
  {
    const LHCb::Particle* dau = *idau ;
    if ( 0 == dau ) { continue ; }
    //
    const LHCb::Particle* found = findFirstInTree ( dau , pid ) ;
    //
    if ( 0 != found ) { return found ; }
  }
  //
  return 0 ;
}
// ============================================================================
// find particle based on PID
// ============================================================================
const LHCb::Particle* LHCb::DecayTree::find 
( const LHCb::ParticleID& pid ) const 
{
  if ( valid() ) { return findFirstInTree( m_head.get() , pid ) ; }
  return 0 ;
}
// ============================================================================
// take ownership of all particles in decay treeL
// ============================================================================
LHCb::Particle* LHCb::DecayTree::release ()
{
  m_clonemap.clear() ;                                   //     clear the links 
  return m_head.release() ;                              // invalidate the head 
}
// ============================================================================
// The END 
// ============================================================================
