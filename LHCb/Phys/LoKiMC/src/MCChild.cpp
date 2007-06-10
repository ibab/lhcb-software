// $Id: MCChild.cpp,v 1.3 2007-06-10 19:59:05 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL
// ============================================================================
#include <algorithm>
#include <functional>
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MCChild.h"
#include "LoKi/MCDecayVertex.h"
// ============================================================================
/** @file
 *  Implementation file for functions form namespace LoKi::MCChild 
 *  @date 2007-06-02 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
// get the number of children
// ============================================================================
std::size_t LoKi::MCChild::nChildren
( const LHCb::MCParticle*   mother ) 
{
  if ( 0 == mother       ) { return 0 ; }                           // RETURN 
  typedef SmartRefVector<LHCb::MCVertex> EVs ;
  const EVs& evs = mother->endVertices () ;
  // find "LHCb::MCParticle::isDecay" vertex: 
  EVs::const_iterator iev = 
    std::find_if ( evs.begin() , evs.end() , LoKi::MCVertices::IsDecay  ) ;
  if ( evs.end () == iev ) { return 0 ; }                         // RETURN 
  const LHCb::MCVertex* ev = *iev ;
  return ev->products().size() ;                                  // RETURN  
}
// ============================================================================
/* Trivial accessor to the dautgher particles for the given MC-particle.
 *
 *  @attention index starts with 1 , null index corresponds 
 *             to the original particle 
 *
 *  @param  particle (const) pointer to mother particle 
 *  @param  index    index   index of the child particle 
 *  @return daughter particle with given index 
 */
// ============================================================================
const LHCb::MCParticle* LoKi::MCChild::child 
( const LHCb::MCParticle* mother , 
  const size_t            index  ) 
{
  if ( 0 == mother                    ) { return 0      ; }       // RETURN 
  if ( 0 == index                     ) { return mother ; }       // RETURN 
  typedef SmartRefVector<LHCb::MCVertex> EVs ;
  const EVs& evs = mother->endVertices() ;
  // find "LHCb::MCParticle::isDecay" vertex: 
  EVs::const_iterator iev = 
    std::find_if ( evs.begin() , evs.end() , LoKi::MCVertices::IsDecay ) ;
  if ( evs.end () == iev              ) { return 0 ; }            // RETURN 
  const LHCb::MCVertex* ev = *iev ;
  if ( index > ev->products().size()  ) { return 0 ; }            // RETURN 
  // get the proper particle:
  return ev->products()[index-1] ;                                // RETURN 
}
// ========================================================================
/*  get all daughter for th egiven MCparticle 
 *  @param particle  MC-particle 
 *  @param decayOnly flag to consider only particles from the decay
 *  @return vector of daughetr particles 
 *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
 *  @date 2007-06-02
 */
// ========================================================================
LHCb::MCParticle::ConstVector
LoKi::MCChild::children 
( const LHCb::MCParticle* particle  , 
  const bool              decayOnly ) 
{
  LHCb::MCParticle::ConstVector result ;
  if ( 0 == particle                ) { return result  ;}  // RETURN 
  typedef SmartRefVector<LHCb::MCVertex> EVs ;
  const EVs& evs = particle->endVertices () ;
  for ( EVs::const_iterator iev = evs.begin() ; evs.end() != iev ; ++iev ) 
  {
    const LHCb::MCVertex* ev = *iev ;
    if ( 0 == ev                     ) { continue ; }    // CONTINUE 
    if ( decayOnly && !ev->isDecay() ) { continue ; }    // CONTINUE 
    const SmartRefVector<LHCb::MCParticle>& products = ev->products() ;
    result.insert( result.end () , products.begin () , products.end () ) ;
  }
  return result ;
} 
// ============================================================================
/** get all descendants for the given MCparticle 
 *  @param particle  MC-particle 
 *  @param decayOnly flag to consider only particles from the decay
 *  @return vector of descendants particles 
 *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
 *  @date 2007-06-02
 */
// ============================================================================
LHCb::MCParticle::ConstVector
LoKi::MCChild::descendants
( const LHCb::MCParticle* particle  , 
  const bool              decayOnly ) 
{
  LHCb::MCParticle::ConstVector result ;
  if ( 0 == particle                ) { return result  ;}  // RETURN 
  typedef SmartRefVector<LHCb::MCVertex> EVs ;
  const EVs& evs = particle->endVertices () ;
  for ( EVs::const_iterator iev = evs.begin() ; evs.end() != iev ; ++iev ) 
  {
    const LHCb::MCVertex* ev = *iev ;
    if ( 0 == ev                     ) { continue ; }    // CONTINUE 
    if ( decayOnly && !ev->isDecay() ) { continue ; }    // CONTINUE 
    typedef SmartRefVector<LHCb::MCParticle> Ps ;
    const Ps& ps = ev->products() ;
    for ( Ps::const_iterator ip = ps.begin() ; ps.end() != ip ; ++ip ) 
    {
      if ( 0 == *ip ) { continue ; }
      // insert the daughter: 
      result.push_back( *ip ) ;
      const LHCb::MCParticle::ConstVector& v = descendants ( *ip , decayOnly ) ;
      // insert all its descendants:
      result.insert( result.end () , v.begin () , v.end () ) ;
    } // loop over daughter particles 
  } // loop over end-vertices 
  return result ;
}
// ============================================================================
/** get all ancestors for the given MCparticle 
 *  @param particle  MC-particle 
 *  @param decayOnly flag to consider only particles from the decay
 *  @return vector of ancestors particles 
 *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
 *  @date 2007-06-02
 */
// ============================================================================
LHCb::MCParticle::ConstVector
LoKi::MCChild::ancestors 
( const LHCb::MCParticle* particle  ) 
{
  LHCb::MCParticle::ConstVector result ;
  if ( 0 == particle ) { return result ; }                         // RETURN 
  const LHCb::MCParticle* mother = particle->mother() ;
  while ( 0 != mother ) 
  {
    result.push_back ( mother ) ;
    mother = mother->mother() ;
  }
  return result ;
}
// ============================================================================
/*  get all (0 or 1) parents for the given particle 
 *  @param particle  MC-particle 
 *  @return vector of parent particles (1or 2)
 *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
 *  @date 2007-06-02
 */
// ============================================================================
LHCb::MCParticle::ConstVector
LoKi::MCChild::parents
( const LHCb::MCParticle* particle  )
{
  LHCb::MCParticle::ConstVector result ;
  if ( 0 == particle ) { return result ; }                      // RETURN 
  const LHCb::MCParticle* mother = particle->mother() ;  
  if ( 0 == mother   ) { return result ; }                      // RETURN 
  result.push_back ( mother ) ;
  return result ;                                               // RETURN 
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
