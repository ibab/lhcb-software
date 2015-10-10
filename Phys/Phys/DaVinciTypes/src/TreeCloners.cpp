// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL
// ============================================================================
#include <set>
// ============================================================================
// local
// ============================================================================
#include "Kernel/TreeCloners.h"
// ============================================================================
/** @file
 *  implementation of clone functions
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-06-09
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /** check if the object is registered in TES 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-06-09
   */
  inline bool inTES ( const ContainedObject* o )
  { return ( 0 != o ) && ( 0 != o->parent () ) ; }
  // ==========================================================================
  /** @class _Counter
   *  simple local counter
   */
  class _Counter
  {
  public:
    // ========================================================================
    // constructor
    _Counter () : m_particles ( 0 ) , m_vertices  ( 0 ) {};
    // destructor
    ~_Counter() 
    {
#ifdef COUNT_INSTANCES 
      //
      if ( 0 != m_particles ) 
      {
        std::cout 
          << " LHCb::DecayTree WARNING misbalance #particles " 
          << m_particles  << std::endl ;
      }
      //
      if ( 0 != m_vertices ) 
      {
        std::cout 
          << " LHCb::DecayTree WARNING misbalance #vertices  " 
          << m_vertices  << std::endl ;
      }
#endif 
      //
    }
    // make the increment
    long incParticles () { return ++m_particles ; }
    long incVertices  () { return ++m_vertices  ; }
    long decParticles () { return --m_particles ; }
    long decVertices  () { return --m_vertices  ; }
    // ========================================================================
  private:
    // ========================================================================
    /// counter of cloned particles 
    long  m_particles ;                          // counter of cloned particles 
    /// counter of cloned vertices  
    long  m_vertices  ;                         //   counter of cloned vertices 
    // ========================================================================
  };
  // ==========================================================================
  /// the local instance counter 
  _Counter s_COUNTER ;                            // the local instance counter 
  // ==========================================================================
} //                                                 end of anonymous namespace 
// ============================================================================
/*  clone the decay tree using helper clone-map
 *  @param head     (INPUT)  the decay tree to be cloned 
 *  @param clonemap (UPDATE) the clonemap to be filled 
 *  @return the cloned tree 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-06-09
 */
// ============================================================================
LHCb::Particle* DaVinci::cloneTree 
( const LHCb::Particle* original , 
  DaVinci::CloneMap&    clonemap ) 
{
  //
  if ( 0 == original        ) { return           0 ; }           // RETURN 
  CloneMap::const_iterator i1 = clonemap.find ( original ) ;
  if ( clonemap.end() != i1 ) { return i1->second ; }            // RETURN 
  //
  // clone it!
  LHCb::Particle*    np = original -> clone() ;
  s_COUNTER.incParticles() ;
  clonemap[ original ]  = np  ;
  //
  np -> clearDaughters (   ) ;
  np -> setEndVertex   ( 0 ) ;
  // 
  typedef SmartRefVector<LHCb::Particle> SRVP ;
  //
  // clone daughters 
  const SRVP& daughters = original->daughters() ;
  for ( SRVP::const_iterator idau = daughters.begin() ; 
        daughters.end() != idau ; ++idau ) 
  {
    const LHCb::Particle* dau = *idau ;
    if ( 0 == dau ) { return 0 ; }                                  // CONTINUE 
    //
    const LHCb::Particle* dau_clone = 0 ;
    CloneMap::const_iterator i2 = clonemap.find ( dau ) ;
    if ( clonemap.end() != i2 ) { dau_clone = i2->second                   ; }
    else                        { dau_clone = cloneTree ( dau , clonemap ) ; }
    //
    if ( 0 != dau_clone ) { np -> addToDaughters ( dau_clone ) ; }  // NB!
    //
  }
  //
  const LHCb::Vertex* ov = original -> endVertex() ;
  if ( 0 == ov ) { return np ; }                                      // RETURN
  //
  LHCb::Vertex* nv = ov -> clone() ;
  s_COUNTER.incVertices () ;
  //
  np -> setEndVertex           ( nv ) ;
  nv -> clearOutgoingParticles (    ) ;
  //
  const SRVP& outgoing = ov->outgoingParticles() ;
  for ( SRVP::const_iterator io = outgoing.begin() ; 
        outgoing.end() != io ; ++io ) 
  {
    const LHCb::Particle* out = *io ;
    if ( 0 == out ) { continue ; }                                  // CONTINUE 
    //
    const LHCb::Particle* out_clone = 0 ;
    CloneMap::const_iterator i3 = clonemap.find ( out ) ;
    if ( clonemap.end() != i3 ) { out_clone = i3->second                   ; }
    else                        { out_clone = cloneTree ( out , clonemap ) ; }
    //
    if ( 0 != out_clone ) { nv -> addToOutgoingParticles ( out_clone ) ; } // NB!
    //
  }
  // 
  return np ;                                                         // RETURN
  // ==========================================================================
}
// ============================================================================
/*  clone the decay tree using helper clone-map
 *  @attention only non-TES particles are cloned!
 *  
 *  @param head     (INPUT)  the decay tree to be cloned 
 *  @param clonemap (UPDATE) the clonemap to be filled 
 *  @param newp     (UPDATE) the vector of new particles 
 *  @param newv     (UPDATE) the vector of new vertices  
 *  @return the cloned tree 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-06-09
 */
// ============================================================================
LHCb::Particle* DaVinci::cloneTree 
( const LHCb::Particle*        original , 
  DaVinci::CloneMap&           clonemap , 
  LHCb::Particle::ConstVector& newp     , 
  LHCb::Vertex::ConstVector&   newv     ) 
{
  if ( 0 ==    original     ) { return         0 ; }                  // RETURN 
  // in TES ? 
  if ( inTES ( original )   ) { return const_cast<LHCb::Particle*> ( original ) ; }
  // already cloned ?
  CloneMap::const_iterator i1 = clonemap.find ( original ) ;
  if ( clonemap.end() != i1 ) { return i1->second ; }                 // RETURN 
  // 
  // clone it!
  LHCb::Particle* np = original->clone() ;
  // update clonemap! 
  clonemap[ original ] = np ;
  // add into the output container 
  newp.push_back ( np ) ;
  //
  np->clearDaughters() ;
  //
  // clone daughters 
  typedef SmartRefVector<LHCb::Particle> SRVP ;
  const SRVP& daughters = original->daughters() ;
  for ( SRVP::const_iterator id = daughters.begin() ; 
        daughters.end() != id ; ++id )
  {
    const LHCb::Particle* d = *id ;
    if ( 0 == d ) { continue ; }
    //
    const LHCb::Particle* d_clone = 0 ;
    //
    if ( inTES ( d ) ) { d_clone = d ; }
    else 
    {
      CloneMap::const_iterator ii = clonemap.find ( d ) ;
      if ( clonemap.end() != ii ) { d_clone = ii->second ; }
      else { d_clone = cloneTree ( d , clonemap  , newp , newv ) ; }
      //
    }
    //
    np->addToDaughters ( d_clone ) ;
  }
  //
  const LHCb::Vertex* v = original->endVertex() ;
  if ( 0 == v      ) { return np ; }             // RETURN
  //
  if ( inTES ( v ) ) { return np ; }             // RETURN 
  //
  LHCb::Vertex* nv = v->clone() ;
  newv.push_back ( nv ) ;
  //
  np->setEndVertex ( nv ) ;
  nv->clearOutgoingParticles() ;
  //
  const SRVP& outgoing = v->outgoingParticles() ;
  for ( SRVP::const_iterator io = outgoing.begin() ; outgoing.end() != io ; ++io ) 
  {
    const LHCb::Particle* o = *io ;
    if ( 0 == o ) { continue ; }                // CONITNUE 
    //
    const LHCb::Particle* o_clone = 0 ;
    if ( inTES ( o ) ) { o_clone = o ; }
    else 
    {
      CloneMap::const_iterator ii = clonemap.find ( o ) ;
      if ( clonemap.end() != ii ) { o_clone = ii->second ; }
      else { o_clone = cloneTree ( o , clonemap , newp , newv ) ; }
      //
    }
    //
    nv->addToOutgoingParticles ( o_clone ) ;
  }
  //
  return np ;
}
// ============================================================================
namespace 
{
  // ==========================================================================
  unsigned int _deleteTree 
  ( LHCb::Particle*                   tree   , 
    std::set<const ContainedObject*>& killed )  
  {
    //
    if ( 0 == tree                            ) { return  0 ; }  // RETURN 
    if ( killed.end() != killed.find ( tree ) ) { return  0 ; }  // RETURN 
    if ( inTES ( tree )                       ) { return  0 ; }  // RETURN 
    // 
    typedef SmartRefVector<LHCb::Particle> SRVP ;
    //
    unsigned int result = 0 ;
    //
    // delete daughters
    //
    const SRVP& daughters = tree->daughters() ;
    for ( SRVP::const_iterator idau = daughters.begin() ; 
          daughters.end() != idau ; ++idau ) 
    {
      const LHCb::Particle* dau = *idau ;
      if ( 0 == dau ) { continue ; }
      //
      result += _deleteTree ( const_cast<LHCb::Particle*>( dau ) , killed ) ;
    } 
    tree->clearDaughters() ;
    //
    // delete end-vertex 
    //
    LHCb::Vertex* ev = tree->endVertex() ;
    if ( 0 != ev && killed.end() == killed.find ( ev ) )
    {
      const SRVP& outgoing = ev->outgoingParticles() ;
      for ( SRVP::const_iterator io = outgoing.begin() ; 
            outgoing.end() != io ; ++io ) 
      {
        const LHCb::Particle* out = *io ;
        if ( 0 == out ) { continue ; }
        //
        result += _deleteTree ( const_cast<LHCb::Particle*>( out ) , killed ) ;
      }
      ev->clearOutgoingParticles() ;
      //
      // delete vertex!
      //
      killed.insert ( ev ) ;
      delete ev ;
      //
      s_COUNTER.decVertices () ;
      ++result  ;
    }
    //
    // delete particle 
    //
    killed.insert ( tree ) ;
    delete tree ;
    //
    s_COUNTER.decParticles () ;
    //
    return ++result ;                                                // RETURN
  }
  // ==========================================================================
}
// ============================================================================
/** delete the tree 
 *  @param tree (INPUT) the tree to be deleted 
 *  @return the total number of deleetc objects (particles & vertices)
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-06-09
 */   
// ============================================================================
unsigned int DaVinci::deleteTree ( LHCb::Particle* tree ) 
{
  if ( 0 == tree      ) { return 0 ; }                          // RETURN 
  if ( inTES ( tree ) ) { return 0 ; }                          // RETURN 
  //
  std::set<const ContainedObject*> killed ;
  return _deleteTree ( tree , killed ) ;                        // RETURN 
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
