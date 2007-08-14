// $Id: LoopChild.cpp,v 1.4 2007-08-14 17:12:31 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Child.h"
#include "LoKi/Loop.h"
#include "LoKi/LoopObj.h"
#include "LoKi/LoopChild.h"
#include "LoKi/Algo.h"
// ============================================================================
/*  get the number of children 
 *  @param mother poiunter to mother particle 
 *  @return number of daugthter particles 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-06-02
 */   
// ============================================================================
std::size_t LoKi::LoopChild::nChildren 
( const LoKi::Loop& particle )
{
  return !particle.valid() ? 0 : particle->dim() ;
}
// ============================================================================
/*  Trivial accessor to the dauthter particles for the given "particle"
 *
 *  @attention index starts with 1 , null index corresponds 
 *             to the original particle 
 *
 *  @param  particle (const) pointer to mother particle 
 *  @param  index    index   index of the child particle 
 *  @return daughter particle with given index 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-06-02
 */
// ============================================================================
LHCb::Particle* LoKi::LoopChild::child
( const LoKi::Loop& particle , 
  const size_t      index    ) 
{
  const LHCb::Particle* p = particle.child( index ) ;
  if ( 0 == p ) { return  0 ; }                                   // RETURN 
  return const_cast<LHCb::Particle*> ( p ) ;
}
// ============================================================================
/*  Trivial accessor to the dauthter particles for the given "particle"
 *
 *  @attention index starts with 1 , null index corresponds 
 *             to the original particle 
 *
 *  @param  particle (const) pointer to mother particle 
 *  @param  index1   index   index of the child particle 
 *  @param  index2   index   index of the child particle 
 *  @return daughter particle with given index 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-06-02
 */    
// ============================================================================
LHCb::Particle* LoKi::LoopChild::child
( const LoKi::Loop& particle , 
  const size_t      index1   , 
  const size_t      index2   )
{ return LoKi::Child::child ( child ( particle , index1 ) , index2 ) ; }    
// ============================================================================
/*  Trivial accessor to the dauthter particles for the given "particle"
 *
 *  @attention index starts with 1 , null index corresponds 
 *             to the original particle 
 *
 *  @param  particle (const) pointer to mother particle 
 *  @param  index1   index   index of the child particle 
 *  @param  index2   index   index of the child particle 
 *  @param  index3   index   index of the child particle 
 *  @return daughter particle with given index 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-06-02
 */    
// ============================================================================
LHCb::Particle* LoKi::LoopChild::child
( const LoKi::Loop& particle , 
  const size_t      index1   , 
  const size_t      index2   ,
  const size_t      index3   )
{ return LoKi::Child::child ( child ( particle , index1 ) , index2 , index3 ) ; }
// ============================================================================
/*  Trivial accessor to the dauthter particles for the given "particle"
 *
 *  @attention index starts with 1 , null index corresponds 
 *             to the original particle 
 *
 *  @param  particle (const) pointer to mother particle 
 *  @param  index1   index   index of the child particle 
 *  @param  index2   index   index of the child particle 
 *  @param  index3   index   index of the child particle 
 *  @param  index4   index   index of the child particle 
 *  @return daughter particle with given index 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-06-02
 */    
// ============================================================================
LHCb::Particle* LoKi::LoopChild::child
( const LoKi::Loop&  particle , 
  const size_t       index1   , 
  const size_t       index2   ,
  const size_t       index3   ,
  const size_t       index4   )
{ return LoKi::Child::child ( child ( particle , index1 ) , 
                              index2 , index3 , index4 ) ; }
// ============================================================================
/* get the whole vector of daughters (for completeness)
 * @author Vanya BELYAEV ibelyaev@physics.syr.edu
 * @date   2007-06-02
 */    
// ============================================================================
LHCb::Particle::ConstVector
LoKi::LoopChild::children 
( const LoKi::Loop& particle ) 
{
  if ( !particle) { return LHCb::Particle::ConstVector() ; }    // RETURN
  LHCb::Particle::ConstVector result ;
  result.reserve ( particle->dim() ) ;
  const LoKi::LoopObj::Select& current = particle->current() ;
  for ( LoKi::LoopObj::Select::const_iterator ip = current.begin() ; 
        current.end() != ip ; ++ip ) 
  {
    const LHCb::Particle* p = **ip ;
    if ( 0 == p ) { continue ; }
    result.push_back ( p ) ;
  }
  return result ;
} 
// ============================================================================
/*  get all descendants (for completeness)
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-06-02
 */    
// ============================================================================
LHCb::Particle::ConstVector
LoKi::LoopChild::descendants 
( const LoKi::Loop& particle ) 
{
  if ( !particle ) { return LHCb::Particle::ConstVector() ; }     // RETURN 
  LHCb::Particle::ConstVector result ;
  result.reserve ( particle->dim() ) ;
  const LoKi::LoopObj::Select& current = particle->current() ;
  for ( LoKi::LoopObj::Select::const_iterator ip = current.begin() ; 
        current.end() != ip ; ++ip ) 
  {
    const LHCb::Particle* p = **ip ;
    if ( 0 == p ) { continue ; }
    const LHCb::Particle::ConstVector& tmp = LoKi::Child::descendants ( p ) ;
    result.insert ( result.end() , tmp.begin() , tmp.end() ) ;
  }
  return result ;
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
