// $Id: PhysKinematics.h,v 1.6 2007-11-28 14:39:30 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PHYSKINEMATICS_H 
#define LOKI_PHYSKINEMATICS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Kinematics.h"
// ============================================================================
namespace LHCb { class Particle ; }
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
 *  @date 2006-02-09 
 */
// ============================================================================
namespace LoKi
{
  namespace PhysKinematics 
  {
    // ========================================================================
    /** trivial function to evaluate the mass LHCb::Particle
     *  @param  p particle 
     *  @return invariant mass 
     *  @see LHCb::Particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const LHCb::Particle* p ) ;
    // ========================================================================    
    /** trivial function to evaluate the mass LHCb::Particle
     *  @param  p1 the first particle 
     *  @param  p2 the second particle 
     *  @return invariant mass 
     *  @see LHCb::Particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const LHCb::Particle* p1 , 
      const LHCb::Particle* p2 ) ;
    // ========================================================================    
    /** trivial function to evaluate the mass LHCb::Particle
     *  @param  p1 the first particle 
     *  @param  p2 the third particle 
     *  @param  p3 the second particle 
     *  @return invariant mass 
     *  @see LHCb::Particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const LHCb::Particle* p1 , 
      const LHCb::Particle* p2 ,
      const LHCb::Particle* p3 ) ;
    // ========================================================================    
    /** trivial function to evaluate the mass LHCb::Particle
     *  @param  p1 the first particle 
     *  @param  p2 the second particle 
     *  @param  p3 the third  particle 
     *  @param  p3 the fourth  particle 
     *  @return invariant mass 
     *  @see LHCb::Particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const LHCb::Particle* p1 , 
      const LHCb::Particle* p2 ,
      const LHCb::Particle* p3 ,
      const LHCb::Particle* p4 ) ;
    // ========================================================================
  } // end of namespace LoKi::PhysKinematics
  namespace Kinematics 
  {
    /// import the namespace into the proper scope 
    using namespace LoKi::PhysKinematics ;
  } // end of namespace LoKi::Kinematics
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSKINEMATICS_H
// ============================================================================
