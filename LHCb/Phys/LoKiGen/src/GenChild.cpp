// $Id: GenChild.cpp,v 1.2 2007-06-10 19:56:03 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
#include <functional>
// ============================================================================
// HepMC 
// ============================================================================
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/GenChild.h"
// ============================================================================
/*  get the number of children for the given HepMC-particle
 *  @see HepMC::GenParticle
 *  @param  mother pointer to HepMC-particle
 *  @return number of chilren
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-06-02
 */
// ============================================================================
std::size_t LoKi::GenChild::nChildren 
( const HepMC::GenParticle*   mother ) 
{
  if ( 0 == mother     ) { return 0 ; }                           // RETURN 
  HepMC::GenVertex* ev = mother->end_vertex() ;
  if ( 0 == ev         ) { return 0 ; }                           // RETURN 
  return std::distance 
    ( ev->particles_begin ( HepMC::children ) , 
      ev->particles_end   ( HepMC::children ) ) ;
}
// ============================================================================
/*  Trivial accessor to the daughter "decay" particles for the given 
 *   HepMC-particle
 *
 *  @attention index starts with 1 , null index corresponds 
 *             to the original particle 
 *
 *  @param  particle (const) pointer to mother particle 
 *  @param  index    index   index of the child particle 
 *  @return daughter particle with given index 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-06-02
 */    
// ============================================================================
const HepMC::GenParticle* LoKi::GenChild::child 
( const HepMC::GenParticle* mother , 
  const size_t              index  ) 
{
  if ( 0 == mother     ) { return 0      ; }                       // RETURN 
  if ( 0 == index      ) { return mother ; }                       // RETURN 
  HepMC::GenVertex* ev = mother->end_vertex() ;
  if ( 0 == ev         ) { return 0      ; }                      // RETURN 
  typedef HepMC::GenVertex::particle_iterator IT ;
  IT begin = ev -> particles_begin ( HepMC::children ) ;
  IT end   = ev -> particles_end   ( HepMC::children ) ;
  size_t curr    = index - 1 ;
  while ( begin != end && 0 <  curr ) {  ++begin ;  --curr ; }
  if    ( begin != end && 0 == curr ) { return *begin ; }          // REUTRN
  return 0 ;                                                       // RETURN 
}
// ============================================================================
/*  get all particles form the given vertex form the given range 
 *  @see HepMC::GenVertex::particles_begin
 *  @see HepMC::GenVertex::particles_end
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GenChild::particles 
( const HepMC::GenVertex*    vertex , 
  const HepMC::IteratorRange range  ) 
{
  if ( 0 == vertex ) { return std::vector<const HepMC::GenParticle*>() ; }
  HepMC::GenVertex* _v = const_cast<HepMC::GenVertex*> ( vertex ) ;
  return std::vector<const HepMC::GenParticle*>
    ( _v -> particles_begin ( range ) , 
      _v -> particles_end   ( range ) ) ;
} 
// ============================================================================
/*  get all "in"-particles for the given vertex 
 *  @see HepMC::GenVertex::particles_in_const_begin() 
 *  @see HepMC::GenVertex::particles_in_const_end()
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */
// ============================================================================
std::vector<const HepMC::GenParticle*>
LoKi::GenChild::particles_in 
( const HepMC::GenVertex* vertex ) 
{
  if ( 0 == vertex ) { return std::vector<const HepMC::GenParticle*>() ; }
  return std::vector<const HepMC::GenParticle*>
    ( vertex -> particles_in_const_begin () , 
      vertex -> particles_in_const_end   () ) ;
}
// ============================================================================
/*  get all "out"-particles for the given vertex  
 *  @see HepMC::GenVertex::particles_in_const_begin() 
 *  @see HepMC::GenVertex::particles_in_const_end()
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */
// ============================================================================
std::vector<const HepMC::GenParticle*>
LoKi::GenChild::particles_out
( const HepMC::GenVertex* vertex ) 
{
  if ( 0 == vertex ) { return std::vector<const HepMC::GenParticle*>() ; }
  return std::vector<const HepMC::GenParticle*>
    ( vertex -> particles_out_const_begin () , 
      vertex -> particles_out_const_end   () ) ;
}
// ============================================================================
/*  get all particles form the given event 
 *  @see HepMC::GenEvent::particles_begin
 *  @see HepMC::GenEvent::particles_end
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GenChild::particles_all 
( const HepMC::GenEvent* event )  
{
  if ( 0 == event ) { return std::vector<const HepMC::GenParticle*>() ; }
  return std::vector<const HepMC::GenParticle*>
    ( event -> particles_begin () , 
      event -> particles_end   () ) ;
}
// ============================================================================
/*  get all vertices form the given event 
 *  @see HepMC::GenEvent::vertices_begin
 *  @see HepMC::GenEvent::vertices_end
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */
// ============================================================================
std::vector<const HepMC::GenVertex*> 
LoKi::GenChild::vertices_all 
( const HepMC::GenEvent* event )  
{
  if ( 0 == event ) { return std::vector<const HepMC::GenVertex*>() ; }
  return std::vector<const HepMC::GenVertex*>
    ( event -> vertices_begin () , 
      event -> vertices_end   () ) ;
}
// ============================================================================
/*  get all "children" particles form the given particle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GenChild::children ( const HepMC::GenParticle* particle ) 
{
  if ( 0 == particle ){ return std::vector<const HepMC::GenParticle*>() ; }
  return LoKi::GenChild::children ( particle->end_vertex() ) ;
}
// ============================================================================
/*  get all "ancestors" particles form the givel particlle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GenChild::ancestors  ( const HepMC::GenParticle* particle ) 
{
  if ( 0 == particle ){ return std::vector<const HepMC::GenParticle*>() ; }
  return LoKi::GenChild::ancestors ( particle->production_vertex() ) ;
} 
// ============================================================================
/*  get all "descendant" particles form the given particle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
std::vector<const HepMC::GenParticle*> 
LoKi::GenChild::descendants ( const HepMC::GenParticle* particle ) 
{
  if ( 0 == particle ){ return std::vector<const HepMC::GenParticle*>() ; }
  return LoKi::GenChild::descendants ( particle->end_vertex() ) ;
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
