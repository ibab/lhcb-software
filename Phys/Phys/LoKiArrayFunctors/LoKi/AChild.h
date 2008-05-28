// $Id: AChild.h,v 1.4 2008-05-28 11:39:20 cattanem Exp $
// ============================================================================
#ifndef LOKI_ACHILD_H 
#define LOKI_ACHILD_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ATypes.h"
// ============================================================================
namespace LoKi
{
  namespace AChild
  {
    // ========================================================================
    /** get the number of children 
     *  @param  vct  "pseudo-particle"
     *  @return number of daughter particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-11
     */
    inline std::size_t 
    nChildren 
    ( const LoKi::ATypes::Combination& vct ) { return vct.size() ; }
    // ========================================================================
    /** Trivial accessor to the daughter particles for the given particle.
     *
     *  @attention index starts with 1 
     *
     *  @param  vct  "pseudo-particle"
     *  @param  index the index of the daughter particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-11
     */
    LHCb::Particle* child
    ( const LoKi::ATypes::Combination& vct   , 
      const size_t                       index ) ;
    // ========================================================================
    /** Trivial accessor to the daughter particles for the given particle.
     *
     *  @attention index starts with 1 
     *
     *  @param  vct  "pseudo-particle"
     *  @param  index1   the index of the daughter particle 
     *  @param  index2   the index of the daughter particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-11
     */
    LHCb::Particle* child
    ( const LoKi::ATypes::Combination& vct    , 
      const size_t                       index1 , 
      const size_t                       index2 ) ;
    // ========================================================================
    /** Trivial accessor to the daughter particles for the given particle.
     *
     *  @attention index starts with 1
     *
     *  @param  vct  "pseudo-particle"
     *  @param  index1   the index of the daughter particle 
     *  @param  index2   the index of the daughter particle 
     *  @param  index3   the index of the daughter particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-11
     */
    LHCb::Particle* child
    ( const LoKi::ATypes::Combination& vct    , 
      const size_t                       index1 , 
      const size_t                       index2 ,
      const size_t                       index3 ) ;
    // ========================================================================
    /** Trivial accessor to the daughter particles for the given particle.
     *
     *  @attention index starts with 1
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
    LHCb::Particle* child
    ( const LoKi::ATypes::Combination& vct    , 
      const size_t                       index1 , 
      const size_t                       index2 ,
      const size_t                       index3 ,
      const size_t                       index4 ) ;
    // ========================================================================
    /** trivial function to access all children particles 
     *  @param vct  "pseudo-particle"
     *  @return vector of children
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-04
     */
    inline LHCb::Particle::ConstVector
    children
    ( const LoKi::ATypes::Combination& vct ) 
    { return LHCb::Particle::ConstVector ( vct.begin() , vct.end() ) ; }
    // ========================================================================
    /** trivial function to access all descendants particles 
     *  @param vct  "pseudo-particle"
     *  @return vector of descendants 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-04
     */
    LHCb::Particle::ConstVector
    descendants
    (  const LoKi::ATypes::Combination& vct ) ;
    // ========================================================================
  } // end of the namespace LoKi:AChild 
  namespace Child
  {
    using namespace LoKi::AChild ;
  }
} // end of the namespace LoKi
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_ACHILD_H
// ============================================================================
