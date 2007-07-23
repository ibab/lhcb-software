// $Id: PhysRangeTypes.h,v 1.8 2007-07-23 17:35:48 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PHYSRANGETYPES_H 
#define LOKI_PHYSRANGETYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/NamedRange.h"
#include "LoKi/Selected.h"
#include "LoKi/RangeList.h"
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
 *  @date 2006-02-16 
 */
// ============================================================================
namespace LoKi
{
  /** @namespace LoKi::PhysTypes LoKi/PhysRangeTypes.h
   *  useful "physics types"
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-16 
   */   
  namespace PhysTypes
  {
    /// the actual type of the container of particles  
    typedef LHCb::Particle::ConstVector          Container ;
    /// the actual storage  of containers of particles  
    typedef LoKi::Selected_<Container>            Selected ;
    /// the actual type of range of particles 
    typedef Selected::Range   Range ;
    /// the actual type of the container of vertices 
    typedef LHCb::VertexBase::ConstVector         VContainer ;
    /// the actual storagf  of containers of particles  
    typedef LoKi::Selected_<VContainer>           VSelected  ;
    /// the actual type of range of particles 
    typedef VSelected::Range                         VRange  ;
  } // end of namespace LoKi::PhysTypes  
  namespace Types
  {
    /// the actual range of particles 
    typedef LoKi::PhysTypes::Range                  Range   ;
    /// the actual range of particles 
    typedef LoKi::PhysTypes::VRange                 VRange  ;
    /// the actual range of range list particles 
    typedef LoKi::RangeList_<Range>                 RangeL  ;
    /// the actual range of range list particles 
    typedef LoKi::RangeList_<VRange>                VRangeL ;
  } // end of namespace LoKi::Types  
  /** Trivial conversion function 
   *  @param vct vector of noon-const particles 
   *  @param return vector of const particles 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-16 
   */
  inline 
  LHCb::Particle::ConstVector 
  convert ( LHCb::Particle::Vector&      vct ) 
  { return LHCb::Particle::ConstVector ( vct.begin() , vct.end() ) ; }
}  // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSRANGETYPES_H
// ============================================================================
