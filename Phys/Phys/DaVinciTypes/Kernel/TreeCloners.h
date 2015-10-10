// $Id$
// ============================================================================
#ifndef KERNEL_PARTICLECLONERS_H 
#define KERNEL_PARTICLECLONERS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD& STL 
// ============================================================================
#include <map>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
/** @file 
 *  collection of useful utilities to clone decay trees 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-06-09
 */
// ============================================================================
namespace DaVinci
{
  // ==========================================================================
  /** the actual type for the clone-map
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-06-09
   */
  typedef std::map<const LHCb::Particle*,LHCb::Particle*>            CloneMap ;
  // ==========================================================================
  /** clone the decay tree using helper clone-map
   *  @param head     (INPUT)  the decay tree to be cloned 
   *  @param clonemap (UPDATE) the clonemap to be filled 
   *  @return the cloned tree 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-06-09
   */
  GAUDI_API
  LHCb::Particle* cloneTree 
  ( const LHCb::Particle* original , 
    CloneMap&             clonemap ) ;
  // =========================================================================
  /** clone the decay tree using helper clone-map
   *  @attention only non-TES particles are cloned!
   *  
   *  @param head     (INPUT)  the decay tree to be cloned 
   *  @param clonemap (UPDATE) the clonemap to be filled 
   *  @param newp     (UPDATE) the vector of new particles 
   *  @param newv     (UPDATE) the vector of new vertices  
   *  @return the cloned tree 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-06-09
   */
  GAUDI_API
  LHCb::Particle* cloneTree 
  ( const LHCb::Particle*        original , 
    CloneMap&                    clonemap , 
    LHCb::Particle::ConstVector& newp     , 
    LHCb::Vertex::ConstVector&   newv     ) ;
  // ==========================================================================
  /** delete the tree 
   *  @param tree (INPUT) the tree to be deleted 
   *  @return the total number of deleted objects (particles & vertices)
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-06-09
   */   
  GAUDI_API
  unsigned int deleteTree ( LHCb::Particle* tree ) ;
  // ==========================================================================
} //                                               the end of namespace DaVinci
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // KERNEL_PARTICLECLONERS_H
// ============================================================================
