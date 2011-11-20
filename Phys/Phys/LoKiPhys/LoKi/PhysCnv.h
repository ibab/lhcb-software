// $Id$
// ============================================================================
#ifndef LOKI_PHYSCNV_H 
#define LOKI_PHYSCNV_H 1
// ============================================================================
// GaudiKernl
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysRangeTypes.h"
#include "LoKi/PhysRangeTypes.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16 
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::Cnv PhysCnv.h LoKi/PhysCnv.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-17
   */
  namespace Cnv
  { 
    // ========================================================================
    /** Trivial conversion function 
     *  @param vct vector of non-const particles 
     *  @return vector of const particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    inline 
    LHCb::Particle::ConstVector 
    convert 
    ( const LHCb::Particle::Vector&      vct ) 
    { return LHCb::Particle::ConstVector( vct.begin() , vct.end() ) ; }
    // ========================================================================    
    /** Trivial conversion function 
     *  @param vct vector of const particles 
     *  @return vector of non-const particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    GAUDI_API 
    LHCb::Particle::Vector 
    convert 
    ( const LHCb::Particle::ConstVector& vct ) ;
    // ========================================================================
    /** Trivial conversion function 
     *  @param vct vector of const particles 
     *  @return vector of non-const particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    GAUDI_API
    LHCb::Particle::Vector 
    convert 
    ( SmartRefVector<LHCb::Particle>& vct ) ;
    // ========================================================================    
    /** Trivial conversion function 
     *  @param vct vector of const particles 
     *  @return vector of non-const particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    GAUDI_API 
    LHCb::Particle::Vector 
    convert 
    ( const SmartRefVector<LHCb::Particle>& vct ) ;
    // ========================================================================    
    /** Trivial conversion function 
     *  @param vct vector of non-const vertices 
     *  @return vector of const vertices 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    inline 
    LHCb::Vertex::ConstVector 
    convert 
    ( const LHCb::Vertex::Vector&      vct ) 
    { return LHCb::Vertex::ConstVector( vct.begin() , vct.end() ) ; }
    // ========================================================================    
    /** Trivial conversion function 
     *  @param vct vector of const vertices 
     *  @return vector of non-const vertices 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    GAUDI_API
    LHCb::Vertex::Vector 
    convert 
    ( const LHCb::Vertex::ConstVector& vct ) ;
    // ========================================================================
    /** Trivial conversion function 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    GAUDI_API 
    LHCb::Particle::Vector 
    convert 
    ( const LoKi::Types::Range& vct ) ;
    // ========================================================================    
    /** Trivial conversion function 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    GAUDI_API
    LHCb::Vertex::Vector 
    convert 
    ( SmartRefVector<LHCb::Vertex>& vct ) ;
    // ========================================================================    
    /** Trivial conversion function 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    GAUDI_API 
    LHCb::Vertex::Vector 
    convert 
    ( const SmartRefVector<LHCb::Vertex>& vct ) ;
    // ========================================================================
  } //                                               end of namespace LoKi::Cnv 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSCNV_H
// ============================================================================
