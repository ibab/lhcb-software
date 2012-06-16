// $Id$
// ============================================================================
#ifndef LOKI_MCCHILD_H 
#define LOKI_MCCHILD_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace MCChild 
  {
    // ========================================================================
    /** get the number of children for the given MC-particle
     *  @attention Only the particles from "decay" end-vertex are counted 
     *  @see LHCb::MCParticle
     *  @see LHCb::MCVertex::isDecay
     *  @param  mother pointer to MC-particle
     *  @return number of chilren fomr decay vertex 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-02
     */
    GAUDI_API
    unsigned int nChildren ( const LHCb::MCParticle*   mother ) ;
    // ========================================================================
    /** Trivial accessor to the daughter "decay" particles for the 
     *  given MC-particle
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *  @attention Only the particles from isDecay" end-vertex are considered
     *
     *  @see LHCb::MCParticle
     *  @see LHCb::MCVertex::isDecay
     *  @param  mother  (const) pointer to mother particle 
     *  @param  index    index   index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */    
    GAUDI_API
    const LHCb::MCParticle* child 
    ( const LHCb::MCParticle*   mother , 
      const unsigned int        index  ) ;    
    // ========================================================================
    /** Trivial accessor to the daughter "decay" particles for the given 
     *  MC-particle.
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *  @attention Only the particles from isDecay" end-vertex are considered
     *
     *  @see LHCb::MCParticle
     *  @see LHCb::MCVertex::isDecay
     *  @param  particle (const) pointer to mother particle 
     *  @param  index1   index   index of the child particle 
     *  @param  index2   index   index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */    
    GAUDI_API
    const LHCb::MCParticle* child
    ( const LHCb::MCParticle*  particle , 
      const unsigned int       index1   , 
      const unsigned int       index2   ) ;
    // ========================================================================
    /** Trivial accessor to the daughter "decay" particles for the 
     *  given MC-particle.
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *  @attention Only the particles from isDecay" end-vertex are considered
     *
     *  @see LHCb::MCParticle
     *  @see LHCb::MCVertex::isDecay
     *  @param  particle (const) pointer to mother particle 
     *  @param  index1   index   index of the child particle 
     *  @param  index2   index   index of the child particle 
     *  @param  index3   index   index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */    
    GAUDI_API
    const LHCb::MCParticle* child
    ( const LHCb::MCParticle*  particle , 
      const unsigned int       index1   , 
      const unsigned int       index2   ,
      const unsigned int       index3   ) ;
    // ========================================================================
    /** Trivial accessor to the daughter particles for the given MC-particle.
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *  @attention Only the particles from isDecay" end-vertex are considered
     *
     *  @see LHCb::MCParticle
     *  @see LHCb::MCVertex::isDecay
     *  @param  particle (const) pointer to mother particle 
     *  @param  index1   index   index of the child particle 
     *  @param  index2   index   index of the child particle 
     *  @param  index3   index   index of the child particle 
     *  @param  index4   index   index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */
    GAUDI_API
    const LHCb::MCParticle* child
    ( const LHCb::MCParticle*  particle , 
      const unsigned int       index1   , 
      const unsigned int       index2   ,
      const unsigned int       index3   ,
      const unsigned int       index4   ) ;
    // ========================================================================
    /** Trivial accessor to the daughter "decay" particles for the 
     *  given MC-particle
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *  @attention Only the particles from isDecay" end-vertex are considered
     *
     *  @see LHCb::MCParticle
     *  @see LHCb::MCVertex::isDecay
     *  @param  mother  (const) pointer to mother particle 
     *  @param  indices vector of indices of the child particles
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */    
    GAUDI_API
    const LHCb::MCParticle* child 
    ( const LHCb::MCParticle*          mother , 
      const std::vector<unsigned int>& indices ) ;    
    // ========================================================================
    /** get all daughters for the given MCparticle 
     *  @param particle  MC-particle  
     *  @param output    the vector of daughetr particles 
     *  @param decayOnly flag to consider only particles from the decay
     *  @return the size of daughter vector 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */
    GAUDI_API
    unsigned int daughters 
    ( const LHCb::MCParticle*        particle          , 
      LHCb::MCParticle::ConstVector& output            , 
      const bool                     decayOnly = false ) ;
    // ========================================================================
    /** get all daughters for the given MCparticle 
     *  @param particle  MC-particle 
     *  @param decayOnly flag to consider only particles from the decay
     *  @return vector of daughetr particles 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */
    GAUDI_API 
    LHCb::MCParticle::ConstVector
    children 
    ( const LHCb::MCParticle* particle          , 
      const bool              decayOnly = false ) ;
    // ========================================================================
    /** get all descendants for the given MCparticle 
     *  @param particle  MC-particle 
     *  @param decayOnly flag to consider only particles from the decay
     *  @return vector of descendants particles 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */
    GAUDI_API
    LHCb::MCParticle::ConstVector
    descendants
    ( const LHCb::MCParticle* particle          , 
      const bool              decayOnly = false ) ;
    // ========================================================================
    /** get all ancestors for the given MCparticle 
     *  @param particle  MC-particle 
     *  @param decayOnly flag to consider only particles from the decay
     *  @return vector of ancestors particles 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */
    GAUDI_API
    LHCb::MCParticle::ConstVector
    ancestors 
    ( const LHCb::MCParticle* particle  ) ;
    // ========================================================================
    /** get all (0 or 1) parents for the given particle 
     *  @param particle  MC-particle 
     *  @return vector of parent particles (1or 2)
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */
    GAUDI_API
    LHCb::MCParticle::ConstVector
    parents
    ( const LHCb::MCParticle* particle  ) ;
    // ========================================================================
    /** get the mother particle  (just for completeness of interface)
     *  @param particle  MC-particle 
     *  @return mother particle 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */
    GAUDI_API 
    const LHCb::MCParticle* 
    mother 
    ( const LHCb::MCParticle* particle  ) ;
    // ========================================================================
    /// forward decalration
    class Selector ;                                     // forward decalration
    // ========================================================================
    /** Trivial accessor to the daughter particles for the given particle.
     *  @param  particle (const) pointer to mother particle 
     *  @param  selector the selector 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-05-29
     */
    GAUDI_API 
    const LHCb::MCParticle* child 
    ( const LHCb::MCParticle*        particle , 
      const LoKi::MCChild::Selector& selector ) ;
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
    ( const LHCb::MCParticle*        particle , 
      const LoKi::MCChild::Selector& selector , 
      LHCb::MCParticle::ConstVector& result   ) ;
    /** accessor to certain children particles for the given particle 
     *  @param  particle (INPUT) pointer to mother particle 
     *  @param  selector (INPUT) the selector 
     *  @return the container of found particles
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-05-29
     */
    GAUDI_API 
    LHCb::MCParticle::ConstVector
    children 
    ( const LHCb::MCParticle*        particle , 
      const LoKi::MCChild::Selector& selector ) ;
    // ========================================================================
  } //                                           end of namespace LoKi::MCChild
  // ==========================================================================
  namespace Child
  {
    // ========================================================================
    using LoKi::MCChild::nChildren   ;
    using LoKi::MCChild::child       ;
    using LoKi::MCChild::daughters   ;
    using LoKi::MCChild::children    ;
    using LoKi::MCChild::descendants ;
    using LoKi::MCChild::ancestors   ;
    using LoKi::MCChild::parents     ;
    using LoKi::MCChild::mother      ;
    // ========================================================================
  } //                                             end of namespace LoKi::Child
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCCHILD_H
// ============================================================================
