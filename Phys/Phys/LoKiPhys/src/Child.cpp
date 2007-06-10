// $Id: Child.cpp,v 1.5 2007-06-10 20:07:17 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Objects.h"
#include "LoKi/Child.h"
#include "LoKi/PhysExtract.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-10 
 */
// ========================================================================
/*  get the number of children 
 *  @param mother poiunter to mother particle 
 *  @return number of daugthter particles 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-02-11
 */
// ============================================================================
std::size_t LoKi::Child::nChildren ( const LHCb::Particle* mother ) 
{
  if ( 0 == mother ) { return 0 ;}
  return mother->daughters().size() ;
}
// ============================================================================
/*  Trivial accessor to the daughter particles for the given particle.
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
/*  trivial function to access all children particles 
 *  @see LHCb::Particle::daughters
 *  @param particle pointer to particle 
 *  @param return vector of children
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-06-04
 */
// ============================================================================
LHCb::Particle::ConstVector 
LoKi::Child::children 
( const LHCb::Particle* particle ) 
{
  if ( 0 == particle ) { return LHCb::Particle::ConstVector() ; } /// RETURN 
  return LHCb::Particle::ConstVector ( particle -> daughters() . begin () , 
                                       particle -> daughters() . end   () ) ;
}
// ============================================================================
/*  trivial function to access all descendants particles 
 *  @param particle pointer to particle 
 *  @param return vector of descendants 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-06-04
 */
// ========================================================================
LHCb::Particle::ConstVector 
LoKi::Child::descendants
( const LHCb::Particle* particle ) 
{
  LHCb::Particle::ConstVector result ;
  if ( 0 == particle ) { return result ; }
  /// get all particles (skip NULLs only)
  LoKi::Extract::getParticles 
    ( particle , std::back_inserter ( result ) , LoKi::Objects::_VALID_ ) ;   
  /// remove self
  LHCb::Particle::ConstVector::iterator self = 
    std::remove ( result.begin() , result.end () , particle ) ;
  result.erase ( self , result.end() ) ;
  return result ;
}
// ========================================================================
// constructor 
// ============================================================================
LoKi::Child::PhysChild::PhysChild(){}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

