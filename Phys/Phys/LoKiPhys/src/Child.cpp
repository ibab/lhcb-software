// $Id: Child.cpp,v 1.2 2006-02-18 18:09:15 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $ 
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Child.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for fcuntions form namespace  LoKi::Child
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-10 
 */
// ============================================================================

// ============================================================================
/** @fn child 
 *  Trivial accessor to the daughter particles for the given particle.
 *  It is not very useful in a"stand-alone-mode", but it coudl be very useful 
 *  for non-trivial access:
 *
 *  @code 
 *
 *  for( Loop phi = loop( "kaon+ kaon-" , "pho(1020)" ) ; phi ; ++phi )
 *   {
 *    if( ..... ) { phi->save("phi") ; }
 *   }
 *  
 *  for( Loop Bs = loop( "phi gamma" ) ; Bs ; ++Bs )
 *   {
 *       const Particle* kaon_plus  = child( Bs(1) , 1 ) ; 
 *       const Particle* kaon_minus = child( Bs(1) , 2 ) ; 
 *   }
 *
 *  @endcode 
 *
 *  @attention index starts with 1 , null index corresponds 
 *             to the original particle 
 *
 *  @param  particle (const) pointer to mother particle 
 *  @param  index    index   index of the child particle 
 *  @return daughter particle with given index 
 */
// ============================================================================
LHCb::Particle* LoKi::Child::child
( const LHCb::Particle*  particle , 
  const size_t           index    ) 
{
  if ( 0 == particle      ) { return  0 ; }           // RETURN 
  // trivial case 
  if ( 0 == index ) { return const_cast<LHCb::Particle*> ( particle ) ; }
  const SmartRefVector<LHCb::Particle>& daugs = particle->daughters() ;
  // ATTENTION: index starts from ONE 
  if ( index > daugs.size() ) { return 0 ; }            // RETURN
  // ATTENTION: index starts from ONE 
  const LHCb::Particle* d = *(daugs.begin()+index-1) ;
  return const_cast<LHCb::Particle*>( d ) ;
} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

