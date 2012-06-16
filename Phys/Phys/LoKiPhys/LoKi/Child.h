// $Id$
// ============================================================================
#ifndef LOKI_CHILD_H 
#define LOKI_CHILD_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
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
 *  @date 2006-02-10 
 */
namespace LoKi 
{
  // ===========================================================================
  /** @namespace LoKi::Child LoKi/Child.h
   *  Namespace with collection of accessr to children  
   *  @see LHCb::Particle
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-11
   */
  namespace Child
  {
    // ========================================================================
    /** get the number of children 
     *  @param mother pointer to the mother particle 
     *  @return number of the daughter particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-11
     */
    GAUDI_API
    unsigned int nChildren ( const LHCb::Particle* mother ) ;
    // ========================================================================
    /** Trivial accessor to the daughter particles for the given particle.
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *
     *  @param  particle (const) pointer to mother particle 
     *  @param  index    the index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-11
     */
    GAUDI_API
    LHCb::Particle* child
    ( const LHCb::Particle*  particle , 
      const unsigned int     index    ) ;
    // ========================================================================
    /** Trivial accessor to the daughter particles for the given particle.
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *
     *  @param  particle (const) pointer to mother particle 
     *  @param  index1   the index of the child particle 
     *  @param  index2   the index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-11
     */    
    GAUDI_API
    LHCb::Particle* child
    ( const LHCb::Particle*  particle , 
      const unsigned int     index1   , 
      const unsigned int     index2   ) ;
    // ========================================================================
    /** Trivial accessor to the daughter particles for the given particle.
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *
     *  @param  particle (const) pointer to mother particle 
     *  @param  index1   the index of the child particle 
     *  @param  index2   the index of the child particle 
     *  @param  index3   the index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-11
     */    
    GAUDI_API
    LHCb::Particle* child
    ( const LHCb::Particle*  particle , 
      const unsigned int     index1   , 
      const unsigned int     index2   ,
      const unsigned int     index3   ) ;
    // ========================================================================
    /** Trivial accessor to the daughter particles for the given particle.
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *
     *  @param  particle (const) pointer to mother particle 
     *  @param  index1   the index of the child particle 
     *  @param  index2   the index of the child particle 
     *  @param  index3   the index of the child particle 
     *  @param  index4   the index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-11
     */    
    GAUDI_API 
    LHCb::Particle* child
    ( const LHCb::Particle*  particle , 
      const unsigned int     index1   , 
      const unsigned int     index2   ,
      const unsigned int     index3   ,
      const unsigned int     index4   ) ;
    // ========================================================================
    /** Trivial accessor to the daughter particles for the given particle.
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *
     *  @param  particle (const) pointer to mother particle 
     *  @param  indices the vector of indices 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-07-09
     */
    GAUDI_API
    LHCb::Particle* child 
    ( const LHCb::Particle*            particle , 
      const std::vector<unsigned int>& indices  ) ;
    // ========================================================================
    /** trivial function to access all children particles 
     *  @see LHCb::Particle::daughters
     *  @param particle pointer to particle 
     *  @return vector of children
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-04
     */
    GAUDI_API
    LHCb::Particle::ConstVector 
    children 
    ( const LHCb::Particle* particle ) ;
    // ========================================================================
    /** trivial function to access all children particles at the given level 
     *  @attention: level 0 corresponds to the particle itself 
     *              level 1 corresponds to daughters , etc... 
     *  @see LHCb::Particle::daughters
     *  @param particle pointer to particle 
     *  @param level  the level
     *  @return vector of children at the given level 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-04
     */
    GAUDI_API
    LHCb::Particle::ConstVector 
    children 
    ( const LHCb::Particle* particle , 
      const unsigned int    level    ) ;
    // ========================================================================
    /** get all daughters for the given Particle 
     *  @param particle  particle  
     *  @param output    the vector of daughetr particles 
     *  @return the size of daughter vector 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */
    GAUDI_API
    unsigned int daughters 
    ( const LHCb::Particle*        particle , 
      LHCb::Particle::ConstVector& output   ) ;
    // ========================================================================
    /** trivial function to access all children particles at the given level 
     *  @attention: level 0 corresponds to the particle itself 
     *              level 1 corresponds to daughters , etc... 
     *  @see LHCb::Particle::daughters
     *  @param particle pointer to particle 
     *  @param level  the level
     *  @param result vector of particles 
     *  @return number of children found 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-04
     */
    GAUDI_API
    unsigned int children 
    ( const LHCb::Particle*        particle , 
      const unsigned int           level    , 
      LHCb::Particle::ConstVector& result   ) ;
    // ========================================================================
    /** trivial function to access all descendants particles 
     *  @param particle pointer to particle 
     *  @return vector of descendants 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-04
     */
    GAUDI_API
    LHCb::Particle::ConstVector 
    descendants
    ( const LHCb::Particle* particle ) ;
    // ========================================================================
    /// forward decalration
    class Selector ;                                    // forward decalrations 
    // ========================================================================
    /** Trivial accessor to the daughter particles for the given particle.
     *  @param  particle (const) pointer to mother particle 
     *  @param  selector the selector 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-05-29
     */
    GAUDI_API 
    const LHCb::Particle* child 
    ( const LHCb::Particle*        particle , 
      const LoKi::Child::Selector& selector ) ;
    // ========================================================================
    /** accessor to certain children particles for the given particle 
     *  @param  particle (INPUT) pointer to mother particle 
     *  @param  selector (INPUT) the selector 
     *  @param  result   (OUTPUT) the container of found particles
     *  @return number of found particles 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-05-29
     */
    GAUDI_API 
    unsigned int children 
    ( const LHCb::Particle*        particle , 
      const LoKi::Child::Selector& selector , 
      LHCb::Particle::ConstVector& result   ) ;
    // ========================================================================
    /** accessor to certain children particles for the given particle 
     *  @param  particle (INPUT) pointer to mother particle 
     *  @param  selector (INPUT) the selector 
     *  @return the container of found particles
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-05-29
     */
    GAUDI_API 
    LHCb::Particle::ConstVector
    children 
    ( const LHCb::Particle*        particle , 
      const LoKi::Child::Selector& selector ) ;
    // ========================================================================
  } //                                          end of the namespace LoKi:Child 
  // ==========================================================================
} //                                                  end of the namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CHILD_H
// ============================================================================

