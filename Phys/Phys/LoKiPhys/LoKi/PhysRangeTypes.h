// $Id: PhysRangeTypes.h,v 1.1.1.1 2006-02-17 19:17:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef LOKI_PHYSRANGETYPES_H 
#define LOKI_PHYSRANGETYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/NamedRange.h"
// ============================================================================

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
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16 
 */
// ============================================================================


namespace LoKi
{
  namespace PhysTypes
  {
    /// the actual range of particles 
    typedef LoKi::NamedRange_<LHCb::Particle::ConstVector>   Range ;
    /// the actual range of particles 
    typedef LoKi::NamedRange_<LHCb::Vertex::ConstVector>    VRange ;
  } ;
  namespace Types
  {
    /// the actual range of particles 
    typedef LoKi::NamedRange_<LHCb::Particle::ConstVector>   Range ;
    /// the actual range of particles 
    typedef LoKi::NamedRange_<LHCb::Vertex::ConstVector>    VRange ;
  } ;
  
  /** @fn convert 
   *  Trivial conversion function 
   *  @param vct vector of noon-const particles 
   *  @param return vector of const particles 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-16 
   */
  inline 
  LHCb::Particle::ConstVector 
  convert ( LHCb::Particle::Vector&      vct ) 
  { return LHCb::Particle::ConstVector( vct.begin() , vct.end() ) ; }
  
  /** @fn convert 
   *  Trivial conversion function 
   *  @param vct vector of const particles 
   *  @param return vector of non-const particles 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-16 
   */
  LHCb::Particle::Vector 
  convert ( LHCb::Particle::ConstVector& vct ) ;

} // end of namespace LoKi 

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSRANGETYPES_H
// ============================================================================
