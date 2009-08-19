// $Id: DecayTree.cpp,v 1.1 2009-08-19 11:54:05 ibelyaev Exp $
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
// ============================================================================
// Boost
// ============================================================================
#include <boost/foreach.hpp>
// ============================================================================
/** @file
 *  Implementation of class LHCb::DecyaTree
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
  m_head.reset ( cloneTree( head , m_clonemap ) ) ; 
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
/* destructor
 *  @attention the cloned decay tree is deleted!
 */
// ============================================================================
LHCb::DecayTree::~DecayTree () { if ( valid() ) deleteTree ( *m_head ) ; }  
// ============================================================================
/*  the actual cloning of the decay tree 
 *  @param original the original particle
 *  @param cloneMap the helper map to keep the links netween original and clone
 *  @return the cloned tree
 */
// ============================================================================
LHCb::Particle* LHCb::DecayTree::cloneTree 
( const LHCb::Particle&      original ,
  LHCb::DecayTree::CloneMap& clonemap )
{
  LHCb::Particle* clone = new LHCb::Particle(original) ;
  clonemap[&original] = clone ;
  // copy the daughters
  SmartRefVector<LHCb::Particle> origdaughters = clone->daughters() ;
  // clear the daughters
  clone->setDaughters(SmartRefVector<LHCb::Particle>() ) ;
  for(SmartRefVector<LHCb::Particle>::const_iterator idau = origdaughters.begin() ;
      idau != origdaughters.end() ; ++idau ) 
    clone->addToDaughters( cloneTree( **idau, clonemap ) ) ;
  // clone the vertex, make sure to set daughter pointers correctly
  if( original.endVertex() ) {
    LHCb::Vertex* vertex = original.endVertex()->clone() ;
    clone->setEndVertex( vertex ) ;
    vertex->clearOutgoingParticles() ;
    BOOST_FOREACH( const LHCb::Particle* daughter, original.endVertex()->outgoingParticles() ) {
      CloneMap::const_iterator it = clonemap.find( daughter ) ;
      if( it != clonemap.end() ) vertex->addToOutgoingParticles( it->second ) ;
      else std::cout << "DecayTreeFitter::Tree::cloneTree: cannot find particle in vertex" << std::endl ;
    }
  }
  return clone ;
}
// ============================================================================
// delete the tree 
// ============================================================================  
void LHCb::DecayTree::deleteTree ( LHCb::Particle& head ) 
{
  // delete the daughters
  BOOST_FOREACH( const LHCb::Particle* daughter, head.daughters() ) 
    if( daughter->parent() == 0 ) {
      deleteTree( const_cast<LHCb::Particle&>(*daughter) ) ;
      delete daughter ;
    }
  head.setDaughters(SmartRefVector<LHCb::Particle>() ) ;
  // delete the end vertex
  LHCb::Vertex* vertex = head.endVertex() ;
  if( vertex && vertex->parent()==0 ) {
    delete vertex ;
    head.setEndVertex(0) ;
  }
}
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
( const LHCb::Particle&        particle ,
  const LHCb::ParticleID&      pid      ,
  LHCb::Particle::ConstVector& result   ) 
{
  if( particle.particleID() == pid )  result.push_back( &particle ) ;
  BOOST_FOREACH( const LHCb::Particle* daughter, particle.daughters() ) 
    findInTree( *daughter, pid, result ) ;
}
// ============================================================================
// find a particle in a decay tree based on PID
// ============================================================================
const LHCb::Particle* 
LHCb::DecayTree::findFirstInTree
( const LHCb::Particle&   particle ,
  const LHCb::ParticleID& pid      ) 
{
  const LHCb::Particle* rc(0) ;
  if( particle.particleID() == pid ) 
    rc = &particle ;
  else 
    BOOST_FOREACH( const LHCb::Particle* daughter, particle.daughters() ) {
    if( (rc = findFirstInTree(*daughter,pid) ) ) break ;
  }
  return rc ;
}
// ============================================================================
// find particle based on PID
// ============================================================================
const LHCb::Particle* LHCb::DecayTree::find 
( const LHCb::ParticleID& pid ) const 
{
  if ( valid() ) { return findFirstInTree( *m_head, pid ) ; }
  return 0 ;
}

// ============================================================================
// The END 
// ============================================================================
