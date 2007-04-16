// $Id: PhysKinematics.h,v 1.4 2007-04-16 16:16:26 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/11/27 12:01:32  ibelyaev
//  prepare for LoKi v4r3
//
// Revision 1.2  2006/02/18 18:09:14  ibelyaev
//  fix a typo
//
// Revision 1.1.1.1  2006/02/17 19:17:26  ibelyaev
// New package: "Phys"-part of restructurized LoKi project  
// 
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
  namespace Kinematics 
  {
    /** @fn mass 
     *  trivial function to evaluate the mass LHCb::Particle
     *  @param  p particle 
     *  @return invariant mass 
     *  @see LHCb::Particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const LHCb::Particle* p ) ;
    
    /** @fn mass 
     *  trivial function to evaluate the mass LHCb::Particle
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
    
    /** @fn mass 
     *  trivial function to evaluate the mass LHCb::Particle
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
    
    /** @fn mass 
     *  trivial function to evaluate the mass LHCb::Particle
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

  } // end of namespace LoKi::Kinematics 

} // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSKINEMATICS_H
// ============================================================================
