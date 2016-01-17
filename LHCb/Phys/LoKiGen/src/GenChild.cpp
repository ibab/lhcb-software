// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
#include <functional>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/GenTypes.h"
#include "LoKi/GenChild.h"
#include "LoKi/BuildGenTrees.h"
// ============================================================================
/** @file
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
 *  @date 2006-01-23 
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
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
size_t LoKi::GenChild::particles 
( const HepMC::GenVertex*                 vertex , 
  const HepMC::IteratorRange              range  , 
  std::vector<const HepMC::GenParticle*>& output ) 
{
  if ( !output.empty() ) { output.clear()       ; }
  if ( 0 == vertex     ) { return output.size() ; }
  LoKi::GenTypes::GenSet gset ;
  particles ( vertex , range , gset ) ;
  output.insert ( output.end() , gset.begin() , gset.end() ) ;
  return output.size() ;                
}
// ============================================================================
/*  get all particles form the given vertex form the given range 
 *  @see HepMC::GenVertex::particles_begin
 *  @see HepMC::GenVertex::particles_end
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */
// ============================================================================
size_t LoKi::GenChild::particles 
( const HepMC::GenVertex*                 vertex , 
  const HepMC::IteratorRange              range  , 
  LoKi::GenTypes::GenSet&                 output ) 
{
  if ( !output.empty() ) { output.clear() ; }
  if ( 0 == vertex ) { return output.size() ; }
  HepMC::GenVertex* _v = const_cast<HepMC::GenVertex*> ( vertex ) ;
  output.insert ( _v -> particles_begin ( range ) , 
                  _v -> particles_end   ( range ) ) ;
  return output.size() ;                
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
// ===========================================================================
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
size_t LoKi::GenChild::daughters 
( const HepMC::GenParticle*               particle , 
  std::vector<const HepMC::GenParticle*>& output   ) 
{
  if ( !output.empty() ) { output.clear() ; }
  if ( 0 == particle   ) { return output.size() ; }  // RETURN
  return daughters ( particle -> end_vertex() , output ) ;
}
// ============================================================================
/*  get all "children" particles form the given particle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-05-26
 */    
// ============================================================================
size_t LoKi::GenChild::daughters 
( const HepMC::GenParticle* particle , 
  LoKi::GenTypes::GenSet&   output   ) 
{
  if ( !output.empty() ) { output.clear() ; }
  if ( 0 == particle   ) { return output.size() ; }  // RETURN
  return daughters ( particle -> end_vertex() , output ) ;
}
// ============================================================================
/*  get all "ancestors" particles from the given particle 
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
/* get all independent decay trees from HepMC::GenEvent
 *  @see LoKi::GenTrees::buildTrees 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2016-01-17
 */
// ============================================================================
LoKi::GenTypes::ConstVector
LoKi::GenChild::trees 
( const HepMC::GenEvent* event )
{
  if ( 0 == event ) { return LoKi::GenTypes::ConstVector() ; }
  LoKi::GenTypes::ConstVector result ;
  result.reserve ( 128 ) ; 
  LoKi::GenTrees::buildTrees 
    ( event -> particles_begin ()   , 
      event -> particles_end   ()   , 
      std::back_inserter ( result ) ) ;
  return result ;  
}
// ========================================================================
/*  get all independent decay trees from LHCb::HepMCEvent
 *  @see LoKi::GenTrees::buildTrees 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2016-01-17
 */
// ============================================================================
LoKi::GenTypes::ConstVector
LoKi::GenChild::trees 
( const LHCb::HepMCEvent* event )
{
  if ( 0 == event ) { return LoKi::GenTypes::ConstVector() ; }
  return trees ( event -> pGenEvt() ) ;
}
// ========================================================================
/*  get all independent decay trees from LHCb::HepMCEvent::Container
 *  @see LoKi::GenTrees::buildTrees 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2016-01-17
 */
// ============================================================================
LoKi::GenTypes::ConstVector
LoKi::GenChild::trees 
( const LHCb::HepMCEvent::Container* events)
{
  if ( 0 == events ) { return LoKi::GenTypes::ConstVector() ; }
  LoKi::GenTypes::ConstVector result ;
  for ( LHCb::HepMCEvent::Container::const_iterator it = events->begin() ; 
        events->end() != it ; ++it ) 
  {
    LoKi::GenTypes::ConstVector r = trees ( *it ) ;
    result.insert( result.end() , r.begin() , r.end() ) ;
  }
  return result ;
}
// ========================================================================
/*  get all independent decay trees from container of particles
 *  @see LoKi::GenTrees::buildTrees 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2016-01-17
 */
// ============================================================================
LoKi::GenTypes::ConstVector
LoKi::GenChild::trees 
( const LoKi::GenTypes::ConstVector& particles ) 
{
  LoKi::GenTypes::ConstVector result ;
  result.reserve ( particles.size() ) ;
  LoKi::GenTrees::buildTrees 
    ( particles.begin () , 
      particles.end   () , 
      std::back_inserter ( result ) ) ;
  return result ;  
}
// ========================================================================
/*  get all independent decay trees from container of particles
 *  @see LoKi::GenTrees::buildTrees 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2016-01-17
 */
// ============================================================================
LoKi::GenTypes::ConstVector
LoKi::GenChild::trees 
( const LoKi::GenTypes::GRange& particles ) 
{
  LoKi::GenTypes::ConstVector result ;
  result.reserve ( particles.size() ) ;
  LoKi::GenTrees::buildTrees 
    ( particles.begin () , 
      particles.end   () , 
      std::back_inserter ( result ) ) ;
  return result ;  
}
// ========================================================================
namespace 
{
  // ==========================================================================
  template <class INDEX> 
  const HepMC::GenParticle* 
  _child_ 
  ( const HepMC::GenParticle* particle , 
    INDEX                     begin    , 
    INDEX                     end      ) 
  {
    //
    if ( 0 == particle ) { return        0 ; } // RETURN 
    if ( begin == end  ) { return particle ; } // RETURN 
    //
    const HepMC::GenParticle* daug = 
      LoKi::GenChild::child ( particle , *begin ) ;
    //
    if ( 0 == daug     ) { return        0 ; } // RETURN
    //
    return _child_ ( daug , begin + 1 , end ) ;
  }
  // ==========================================================================
}  
// ============================================================================
const HepMC::GenParticle* 
LoKi::GenChild::child
( const HepMC::GenParticle*        particle , 
  const std::vector<unsigned int>& indices  )
{ return _child_ ( particle , indices.begin () , indices.end () ) ; }
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
