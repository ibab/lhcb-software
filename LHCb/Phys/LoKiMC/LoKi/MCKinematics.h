// $Id: MCKinematics.h,v 1.4 2007-04-16 16:16:08 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/11/27 11:58:37  ibelyaev
//  prepare for LoKi v4r3
//
// Revision 1.2  2006/02/18 18:10:57  ibelyaev
//  fix a typo
//
// Revision 1.1.1.1  2006/01/26 16:13:39  ibelyaev
// New Packaage: MC-dependent part of LoKi project 
// 
// ============================================================================
#ifndef LOKI_MCKINEMATICS_H 
#define LOKI_MCKINEMATICS_H 1
// ============================================================================
// Include files
// ============================================================================/
// LoKiCore 
// ============================================================================/
#include "LoKi/Kinematics.h"
// ============================================================================
// forward declarations 
// ============================================================================
namespace LHCb { class MCParticle ; }
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
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi
{
  /** @namespace LoKi::Kinematics 
   *  collection varios kinematic-related functions 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2001-01-23 
   */
  namespace Kinematics
  {
    /** @fn mass 
     *  trivial function to evaluate the mass MCParticle
     *  @param  p particle 
     *  @return invariant mass 
     *  @see LHCb::MCParticle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const LHCb::MCParticle* p ) ;
    
    /** @fn mass 
     *  trivial function to evaluate the mass MCParticle
     *  @param  p1 the first particle 
     *  @param  p2 the second particle 
     *  @return invariant mass 
     *  @see LHCb::MCParticle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const LHCb::MCParticle* p1 , 
      const LHCb::MCParticle* p2 ) ;
    
    /** @fn mass 
     *  trivial function to evaluate the mass MCParticle
     *  @param  p1 the first particle 
     *  @param  p2 the second particle 
     *  @param  p3 the third particle 
     *  @return invariant mass 
     *  @see LHCb::MCParticle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const LHCb::MCParticle* p1 , 
      const LHCb::MCParticle* p2 ,
      const LHCb::MCParticle* p3 ) ;

    /** @fn mass 
     *  trivial function to evaluate the mass MCParticle
     *  @param  p1 the first particle 
     *  @param  p2 the second particle 
     *  @param  p3 the third particle 
     *  @param  p4 the fourth particle 
     *  @return invariant mass 
     *  @see LHCb::MCParticle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const LHCb::MCParticle* p1 , 
      const LHCb::MCParticle* p2 ,
      const LHCb::MCParticle* p3 ,
      const LHCb::MCParticle* p4 ) ;
    
  }  // end of namespace LoKi::Kinematics 
}  // end of namespace LoKi 

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCKINEMATICS_H
// ============================================================================
