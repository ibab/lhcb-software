// $Id: GenKinematics.h,v 1.1.1.1 2006-01-24 09:54:23 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_GENKINEMATICS_H 
#define LOKI_GENKINEMATICS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Kinematics.h"
// ============================================================================
// HepMC 
// ============================================================================
#include "HepMC/GenParticle.h"
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
 *  with the campain of Dr.O.Callot at al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

// ============================================================================
namespace LoKi 
{
  namespace Kinematics 
  {

    /** @fn mass 
     *  trivial function to evaluate the mass HepMC::GenParticle
     *  @param  p particle 
     *  @return invariant mass 
     *  @see HepMC::GenParticle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const HepMC::GenParticle* p ) ;
    
    /** @fn mass 
     *  trivial function to evaluate the mass HepMC::GenParticle
     *  @param  p1 the first particle 
     *  @param  p2 the second particle 
     *  @return invariant mass 
     *  @see HepMC::GenParticle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const HepMC::GenParticle* p1 , 
      const HepMC::GenParticle* p2 ) ;

    /** @fn mass 
     *  trivial function to evaluate the mass HepMC::GenParticle
     *  @param  p1 the first particle 
     *  @param  p2 the third particle 
     *  @param  p3 the second particle 
     *  @return invariant mass 
     *  @see HepMC::GenParticle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const HepMC::GenParticle* p1 , 
      const HepMC::GenParticle* p2 ,
      const HepMC::GenParticle* p3 ) ;
    
    /** @fn mass 
     *  trivial function to evaluate the mass HepMC::GenParticle
     *  @param  p1 the first particle 
     *  @param  p2 the second particle 
     *  @param  p3 the third  particle 
     *  @param  p3 the fourth  particle 
     *  @return invariant mass 
     *  @see HepMC::GenParticle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const HepMC::GenParticle* p1 , 
      const HepMC::GenParticle* p2 ,
      const HepMC::GenParticle* p3 ,
      const HepMC::GenParticle* p4 ) ;

  } ; // end of namespace LoKi::Kinematics 
  
} ; // end of namespace LoKi 
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENKINEMATICS_H
// ============================================================================
