// $Id: MCChild.cpp,v 1.1 2007-06-03 20:41:11 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
 *  Implementation file for functions form namespace LoKi::Child 
 *  @date 2007-06-02 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
// get the number of children
// ============================================================================
std::size_t LoKi::Child::nChildren
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
const LHCb::MCParticle* LoKi::Child::child 
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
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
