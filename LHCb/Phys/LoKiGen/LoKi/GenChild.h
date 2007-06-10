// $Id: GenChild.h,v 1.2 2007-06-10 19:56:03 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_GENCHILD_H 
#define LOKI_GENCHILD_H 1
// ============================================================================
// Include files
// ============================================================================
// HepMC 
// ============================================================================
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"
// ============================================================================
namespace LoKi
{
  namespace GenChild 
  {
    // ========================================================================
    /** get the number of children for the given HepMC-particle
     *  @see HepMC::GenParticle
     *  @param  mother pointer to HepMC-particle
     *  @return number of chilren
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-02
     */
    std::size_t nChildren ( const HepMC::GenParticle*   mother ) ;     
    // ========================================================================
    /** Trivial accessor to the daughter "decay" particles for 
     *  the given HepMC-particle
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
    const HepMC::GenParticle* child 
    ( const HepMC::GenParticle* mother , 
      const size_t              index  ) ;    
    // ========================================================================
    /** Trivial accessor to the daughter "decay" particles for 
     *  the given HepMC-particle
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *
     *  @param  particle (const) pointer to mother particle 
     *  @param  index1   index   index of the child particle 
     *  @param  index2   index   index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */    
    inline 
    const HepMC::GenParticle* child
    ( const HepMC::GenParticle*  particle , 
      const size_t               index1   , 
      const size_t               index2   )
    { return child ( child ( particle , index1 ) , index2 ) ; }
    // ========================================================================
    /** Trivial accessor to the daughter "decay" particles for 
     *  the given HepMC-particle
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *
     *  @param  particle (const) pointer to mother particle 
     *  @param  index1   index   index of the child particle 
     *  @param  index2   index   index of the child particle 
     *  @param  index3   index   index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */    
    inline 
    const HepMC::GenParticle* child
    ( const HepMC::GenParticle*  particle , 
      const size_t               index1   , 
      const size_t               index2   ,
      const size_t               index3   )
    { return child ( child ( particle , index1 ) , index2 , index3 ) ; }
    // ========================================================================
    /** Trivial accessor to the daughter "decay" particles for 
     *  the given HepMC-particle
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
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */    
    inline 
    const HepMC::GenParticle* child
    ( const HepMC::GenParticle*  particle , 
      const size_t               index1   , 
      const size_t               index2   ,
      const size_t               index3   ,
      const size_t               index4   )
    { return child ( child ( particle , index1 ) , index2 , index3 , index4 ) ; }
    // ========================================================================
    /*  get all "in"-particles for the given vertex 
     *  @see HepMC::GenVertex::particles_in_const_begin() 
     *  @see HepMC::GenVertex::particles_in_const_end()
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */
    std::vector<const HepMC::GenParticle*>
    particles_in  ( const HepMC::GenVertex* vertex ) ;
    // ========================================================================
    /*  get all "out"-particles for the given vertex  
     *  @see HepMC::GenVertex::particles_in_const_begin() 
     *  @see HepMC::GenVertex::particles_in_const_end()
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */
    std::vector<const HepMC::GenParticle*>
    particles_out ( const HepMC::GenVertex* vertex ) ;
    // ========================================================================
    /*  get all particles form the given event 
     *  @see HepMC::GenEvent::particles_begin
     *  @see HepMC::GenEvent::particles_end
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */
    std::vector<const HepMC::GenParticle*> 
    particles_all ( const HepMC::GenEvent* event )  ;
    // ========================================================================
    /** get all vertices form the given event 
     *  @see HepMC::GenEvent::vertices_begin
     *  @see HepMC::GenEvent::vertices_end
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */
    std::vector<const HepMC::GenVertex*> 
    vertices_all ( const HepMC::GenEvent* event )  ;
    // ========================================================================
    /** get all particles form the given vertex form the given range 
     *  @see HepMC::GenVertex::particles_begin
     *  @see HepMC::GenVertex::particles_end
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */
    std::vector<const HepMC::GenParticle*> 
    particles 
    ( const HepMC::GenVertex*    vertex , 
      const HepMC::IteratorRange range  ) ;
    // ========================================================================
    /** get all "parents" particles form the given vertex
     *  @see LoKi::Child::particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */
    inline 
    std::vector<const HepMC::GenParticle*> 
    parents  ( const HepMC::GenVertex*    vertex ) 
    { return particles ( vertex , HepMC::parents ) ; }
    // ========================================================================
    /** get all "children" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    inline 
    std::vector<const HepMC::GenParticle*> 
    children ( const HepMC::GenVertex*    vertex ) 
    { return particles ( vertex , HepMC::children ) ; }
    // ========================================================================
    /** get all "children" particles form the given particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    std::vector<const HepMC::GenParticle*> 
    children ( const HepMC::GenParticle* particle ) ;
    // ========================================================================
    /** get all "family" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    inline 
    std::vector<const HepMC::GenParticle*> 
    family   ( const HepMC::GenVertex*    vertex ) 
    { return particles ( vertex , HepMC::family ) ; }    
    // ========================================================================
    /** get all "ancestors" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    inline 
    std::vector<const HepMC::GenParticle*> 
    ancestors  ( const HepMC::GenVertex*    vertex ) 
    { return particles ( vertex , HepMC::ancestors ) ; }
    // ========================================================================
    /** get all "ancestors" particles form the givel particlle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    std::vector<const HepMC::GenParticle*> 
    ancestors  ( const HepMC::GenParticle* particle ) ;
    // ========================================================================
    /** get all "descendants" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    inline 
    std::vector<const HepMC::GenParticle*> 
    descendants ( const HepMC::GenVertex*    vertex ) 
    { return particles ( vertex , HepMC::descendants ) ; }    
    // ========================================================================
    /** get all "descendant" particles form the given particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    std::vector<const HepMC::GenParticle*> 
    descendants ( const HepMC::GenParticle* particle ) ;
    // ========================================================================
    /** get all "relatives" particles form the given vertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-26
     */    
    inline 
    std::vector<const HepMC::GenParticle*> 
    relatives  ( const HepMC::GenVertex*    vertex ) 
    { return particles ( vertex , HepMC::relatives ) ; }    
    // ========================================================================
    /// helper structure to force the loading of functions 
    struct GenChild     
    {
      /// ficvtive contructor
      GenChild() ; ///< fictive  constructor
    } ;
    // ========================================================================
  } // end of namespace LoKi::GenChild 
  namespace Child 
  {
    using namespace LoKi::GenChild ;
  } // end of namespace LoKi::Child
} // end of namespace LoKi 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENCHILD_H
// ============================================================================
