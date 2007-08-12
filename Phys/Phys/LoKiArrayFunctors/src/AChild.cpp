// $Id: AChild.cpp,v 1.2 2007-08-12 18:57:09 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Child.h"
#include "LoKi/AChild.h"
// ============================================================================
/** @file
 *  The Implementation file for function fomr the filke LoKi/AChild.h
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-07-21 
 */
// ============================================================================
/* Trivial accessor to the daughter particles for the given particle.
 *
 *  @attention index starts with 1 , null index corresponds 
 *             to the original particle 
 *
 *  @param  vct  "pseudo-particle"
 *  @param  index the index of the daughter particle 
 *  @return daughter particle with given index 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-02-11
 */
// ============================================================================
LHCb::Particle* LoKi::AChild::child
( const LHCb::Particle::ConstVector& vct   , 
  const size_t                       index ) 
{
  if ( 0 == index || vct.size() < index ) { return 0 ; }
  const LHCb::Particle* res = vct[index-1] ;
  return const_cast<LHCb::Particle*> ( res ) ;
}
// ============================================================================
/** Trivial accessor to the daughter particles for the given particle.
 *
 *  @attention index starts with 1 , null index corresponds 
 *             to the original particle 
 *
 *  @param  vct  "pseudo-particle"
 *  @param  index1   the index of the daughter particle 
 *  @param  index2   the index of the daughter particle 
 *  @return daughter particle with given index 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-02-11
 */
// ========================================================================
LHCb::Particle* LoKi::AChild::child
( const LHCb::Particle::ConstVector& vct    , 
  const size_t                       index1 , 
  const size_t                       index2 ) 
{ return LoKi::Child::child ( child ( vct , index1 ) , index2 ) ; }
// ========================================================================
/* Trivial accessor to the daughter particles for the given particle.
 *
 *  @attention index starts with 1 , null index corresponds 
 *             to the original particle 
 *
 *  @param  vct  "pseudo-particle"
 *  @param  index1   the index of the daughter particle 
 *  @param  index2   the index of the daughter particle 
 *  @param  index3   the index of the daughter particle 
 *  @return daughter particle with given index 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-02-11
 */
// ========================================================================
LHCb::Particle* LoKi::AChild::child
( const LHCb::Particle::ConstVector& vct    , 
  const size_t                       index1 , 
  const size_t                       index2 ,
  const size_t                       index3 ) 
{ return LoKi::Child::child ( child ( vct , index1 ) , index2 , index3 ) ; }
// ========================================================================
/* Trivial accessor to the daughter particles for the given particle.
 *
 *  @attention index starts with 1 , null index corresponds 
 *             to the original particle 
 *
 *  @param  vct  "pseudo-particle"
 *  @param  index1   the index of the daughter particle 
 *  @param  index2   the index of the daughter particle 
 *  @param  index3   the index of the daughter particle 
 *  @param  index4   the index of the daughter particle 
 *  @return daughter particle with given index 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-02-11
 */
// ========================================================================
LHCb::Particle* LoKi::AChild::child
( const LHCb::Particle::ConstVector& vct    , 
  const size_t                       index1 , 
  const size_t                       index2 ,
  const size_t                       index3 ,
  const size_t                       index4 ) 
{ return LoKi::Child::child ( child ( vct , index1 ) , index2 , index3 , index4 ) ; }
// ========================================================================
/*  trivial function to access all descendants particles 
 *  @param vct  "pseudo-particle"
 *  @param return vector of descendants 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-06-04
 */
// ========================================================================
LHCb::Particle::ConstVector 
LoKi::AChild::descendants
( const LHCb::Particle::ConstVector& vct ) 
{
  typedef LHCb::Particle::ConstVector VCT ;
  VCT result ;
  result.reserve ( 4 * vct.size() ) ;
  for ( VCT::const_iterator i = vct.begin() ; vct.end() != i ; ++i )
  {
    const VCT& _r = LoKi::Child::descendants ( *i ) ;
    result.insert ( result.end() , _r.begin() , _r.end() ) ;
  } 
  return result ;
}
// ============================================================================

// ============================================================================
// The END
// ============================================================================
