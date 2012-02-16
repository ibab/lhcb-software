// $Id$
// ============================================================================
#ifndef LOKI_LOOPCHILD_H 
#define LOKI_LOOPCHILD_H 1
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
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
namespace LoKi 
{
  // ==========================================================================
  class Loop ;
  // ==========================================================================
  namespace LoopChild 
  {
    // ========================================================================
    /** get the number of children 
     *  @param  particle (const) pointer to mother particle 
     *  @return number of daugthter particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-02
     */   
    GAUDI_API 
    std::size_t nChildren ( const LoKi::Loop& particle ) ;
    // ========================================================================
    /** Trivial accessor to the dauthter particles for the given "particle"
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
    GAUDI_API 
    LHCb::Particle* child
    ( const LoKi::Loop& particle , 
      const size_t      index    ) ;
    // ========================================================================
    /** Trivial accessor to the dauthter particles for the given "particle"
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
    GAUDI_API 
    LHCb::Particle* child
    ( const LoKi::Loop& particle , 
      const size_t      index1   , 
      const size_t      index2   ) ;
    // ========================================================================
    /** Trivial accessor to the dauthter particles for the given "particle"
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
    GAUDI_API 
    LHCb::Particle* child
    ( const LoKi::Loop& particle , 
      const size_t      index1   , 
      const size_t      index2   ,
      const size_t      index3   ) ;
    // ========================================================================
    /** Trivial accessor to the dauthter particles for the given "particle"
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
    GAUDI_API 
    LHCb::Particle* child
    ( const LoKi::Loop&  particle , 
      const size_t       index1   , 
      const size_t       index2   ,
      const size_t       index3   ,
      const size_t       index4   ) ;
    // ========================================================================
    /** get the whole vector of daughters (for completeness)
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-02
     */    
    GAUDI_API 
    LHCb::Particle::ConstVector
    children ( const LoKi::Loop& particle ) ;
    // ========================================================================
    /** get all descendants (for completeness)
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-02
     */    
    GAUDI_API 
    LHCb::Particle::ConstVector
    descendants ( const LoKi::Loop& particle ) ;
    // ========================================================================
  } //                                         end of namespace LoKi::LoopChild 
  // ==========================================================================
  namespace Child 
  {
    // ========================================================================
    using namespace LoKi::LoopChild ;
    // ========================================================================
  } //                                             end of namespace LoKi::Child 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_LOOPCHILD_H
// ============================================================================
