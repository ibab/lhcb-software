// $Id: ParticleCuts.h,v 1.4 2006-02-23 21:14:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
//  $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_PHYSPARTICLECUTS_H 
#define LOKI_PHYSPARTICLECUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/CoreCuts.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/Particles.h"
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
 *  @date 2006-01-28 
 */
// ============================================================================


namespace LoKi
{
  namespace Cuts 
  {    
    // ========================================================================
    /** @var ALL
     *  the trivial functor which always evaluates to "true"
     *  
     *  @see LoKi::BooleanConstant
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::BooleanConstant<const LHCb::Particle*>      ALL ( true ) ;
    // ========================================================================
    
    // ========================================================================
    /** @var PALL
     *  the trivial functor which always evaluates to "true"
     *  
     *  @see LoKi::BooleanConstant
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::ALL
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::BooleanConstant<const LHCb::Particle*>  PALL = ALL ;
    // ========================================================================
    
    // ========================================================================
    /** @var PTRUE
     *  the trivial functor which always evaluates to "true"
     *  
     *  @see LoKi::BooleanConstant
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::ALL
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::BooleanConstant<const LHCb::Particle*>  PTRUE = ALL ;
    // ========================================================================

    // ========================================================================
    /** @var NONE
     *  the trivial functor which always evaluates to "false"
     *  
     *  @see LoKi::BooleanConstant
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::BooleanConstant<const LHCb::Particle*>      NONE ( false ) ;
    // ========================================================================

    // ========================================================================
    /** @var PNONE
     *  the trivial functor which always evaluates to "false"
     *  
     *  @see LoKi::BooleanConstant
     *  @see LHCb::Particle
     *  @see LHCb::Cuts::NONE 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::BooleanConstant<const LHCb::Particle*>    PNONE = NONE ;
    // ========================================================================
    
    // ========================================================================
    /** @var PFALSE
     *  the trivial functor which always evaluates to "false"
     *  
     *  @see LoKi::BooleanConstant
     *  @see LHCb::Particle
     *  @see LHCb::Cuts::NONE 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::BooleanConstant<const LHCb::Particle*>    PFALSE = NONE ;
    // ========================================================================

    // ========================================================================
    /** @var ONE
     *  the trivial functor which always evaluates to 1
     *  
     *  @see LoKi::Constant
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Constant<const LHCb::Particle*>             ONE ( 1  ) ;
    // ========================================================================

    // ========================================================================
    /** @var PONE
     *  the trivial functor which always evaluates to 1
     *  
     *  @see LoKi::Cuts::ONE
     *  @see LoKi::Constant
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Constant<const LHCb::Particle*>           PONE = ONE  ;
    // ========================================================================

    // ========================================================================
    /** @var ZERO
     *  the trivial functor which always evaluates to 0
     *  
     *  @see LoKi::Constant
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Constant<const LHCb::Particle*>             ZERO ( 0 ) ;
    // ========================================================================
    
    // ========================================================================
    /** @var PZERO
     *  the trivial functor which always evaluates to 0
     *  
     *  @see LoKi::Cuts::ZERO
     *  @see LoKi::Constant
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Constant<const LHCb::Particle*>           PZERO = ZERO ;
    // ========================================================================

    // ========================================================================
    /** @var VALID
     *  the trivial functor which evaluayed the "validity" of the argument
     *  
     *  @see LoKi::Valid
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Valid<const LHCb::Particle*>                   VALID ;
    // ========================================================================
    
    // ========================================================================
    /** @var HASKEY
     *  The trivial predicate, it relies on Particle::hasKey method 
     *
     *  @code 
     *
     *  const LHCb::Particle* p =  ... ;
     *  const bool good = HASKEY( p ) ;
     *
     *  @endcode 
     * 
     *  @see LHCb::Particle
     *  @see Particle::hasKey
     *  @see KeyedObject::hasKey
     *  @see LoKi::Particles::HasKey
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23
     */   
    const LoKi::Particles::HasKey                              HASKEY ;
    // ========================================================================

    // ========================================================================
    /** The trivial function, it relies on Particle::key method 
     *
     *  @code 
     *
     *  const LHCb::Particle* p =  ... ;
     *  Fun key = PKEY( -1 , -3 ) ;
     *
     *  const bool good = 1335 == key( p ) ;
     *
     *  @endcode 
     * 
     *  @see LHCb::Particle
     *  @see Particle::key
     *  @see KeyedObject::key
     *  @see LoKi::Particles::Key
     *  @see LoKi::Cuts::KEY
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23
     */   
    typedef LoKi::Particles::Key                              PKEY ;
    // ========================================================================

    // ========================================================================
    /** @var KEY
     *  The trivial predicate, it relies on Particle::hasKey method 
     *
     *  @code 
     *
     *  const LHCb::Particle* p =  ... ;
     *  const bool good = 1335 == KEY( p ) ;
     *
     *  @endcode 
     * 
     *  @see LHCb::Particle
     *  @see Particle::key
     *  @see KeyedObject::key
     *  @see LoKi::Particles::Key
     *  @see LoKi::Cuts::PKEY
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23
     */   
    const LoKi::Particles::Key                              KEY ;
    // ========================================================================
    
    // ========================================================================
    /** simple predicate to check if the particle is 
     *  registered in TES at the certain location 
     *
     *  @code 
     *  
     *  const LHCb::Particle* p = ... ;
     *
     *  Cut ksLL = INTES( "LLKs2PiPi" , false )
     *
     *  const double llK0S = ksLL( p ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::InTES
     *  @see LoKi::Cuts::PINTES 
     *  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23
     */   
    typedef LoKi::Particles::InTES                               INTES ;
    // ========================================================================

    // ========================================================================
    /** simple predicate to check if the particle is 
     *  registered in TES at the certain location 
     *
     *  @code 
     *  
     *  const LHCb::Particle* p = ... ;
     *
     *  Cut ksLL = PINTES( "LLKs2PiPi" , false )
     *
     *  const double llK0S = ksLL( p ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::InTES
     *  @see LoKi::Cuts::INTES 
     *  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23
     */   
    typedef INTES                                               PINTES ;
    // ========================================================================
    
    // ========================================================================
    /** @var PVALID
     *  the trivial functor which evaluayed the "validity" of the argument
     *  
     *  @see LoKi::Cuts::VALID
     *  @see LoKi::Valid
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Valid<const LHCb::Particle*>           PVALID = VALID ;
    // ========================================================================

    // ========================================================================
    /** the trivial predicate which check the identity of 2 particles
     * 
     *  @code 
     *
     *  const LHCb::Particle* B1 = ... ;
     *  Cut same = SAME( B1 ) ;
     * 
     *  const LHCb::Particle* B2 = ... ;
     *  
     *  const bool theSame = same( B2 ) ;
     *
     *  @endcode 
     *  
     *  @see LHCb::Particle
     *  @see LoKi::TheSame 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    typedef LoKi::TheSame<const LHCb::Particle*>        SAME  ;
    // ========================================================================

    // ========================================================================
    /** the trivial predicate whoch check the identity of 2 particles
     * 
     *  @code 
     *
     *  const LHCb::Particle* B1 = ... ;
     *  Cut same = PSAME( B1 ) ;
     * 
     *  const LHCb::Particle* B2 = ... ;
     *  
     *  const bool theSame = same( B2 ) ;
     *
     *  @endcode 
     *  
     *  @see LHCb::Particle
     *  @see LoKi::TheSame 
     *  @see LoKi::Cuts::SAME
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    typedef SAME                                            PSAME ;
    // ========================================================================
    
    // ========================================================================
    /** @var P 
     *  Particle's momentum 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     const double    p        = P( particle ) ;
     *
     *  @endcode
     *
     *  @see LHCb::Particle
     *  @see LoKi::Particle::Momentum 
     *  @see LoKi::Function
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    const   LoKi::Particles::Momentum                         P ;
    // ========================================================================
    
    // ========================================================================
    /** @var P2
     *  Particle's momentum squared
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     const double    p2       = P2( particle ) ;
     *
     *  @endcode
     *
     *  @see LHCb::Particle
     *  @see LoKi::Particle::Momentum2 
     *  @see LoKi::Function
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    const   LoKi::Particles::Momentum2                        P2 ;
    // ========================================================================
    
    // ========================================================================
    /** @var E 
     *  Particle's energy
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     const double    e        = E( particle ) ;
     *
     *  @endcode
     *
     *  @see Particle
     *  @see LoKi::Particle::Energy
     *  @see LoKi::Function
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    const   LoKi::Particles::Energy                           E ;
    // ========================================================================
    
    // ========================================================================
    /** @var PT 
     *  Particle's transverse momentum 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     const double    pt       = PT( particle ) ;
     *
     *  @endcode
     *
     *  @see Particle
     *  @see LoKi::Particle::TransverseMomentum 
     *  @see LoKi::Function
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    const   LoKi::Particles::TransverseMomentum               PT ;
    // ========================================================================

    // ========================================================================
    /** @var PX 
     *  X-component of Particle's momentum 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     const double    px       = PX( particle ) ;
     *
     *  @endcode
     *
     *  @see Particle
     *  @see LoKi::Particle::MomentumX
     *  @see LoKi::Function
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    const   LoKi::Particles::MomentumX                        PX ;
    // ========================================================================

    // ========================================================================
    /** @var PY 
     *  Y-component of Particle's momentum 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     const double    py       = PY( particle ) ;
     *
     *  @endcode
     *
     *  @see Particle
     *  @see LoKi::Particle::MomentumY
     *  @see LoKi::Function
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    const   LoKi::Particles::MomentumY                        PY ;
    // ========================================================================

    // ========================================================================
    /** @var PZ 
     *  Z-component of Particle's momentum 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     const double    pz       = PZ( particle ) ;
     *
     *  @endcode
     *
     *  @see Particle
     *  @see LoKi::Particle::MomentumZ
     *  @see LoKi::Function
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    const   LoKi::Particles::MomentumZ                        PZ ;
    // ========================================================================

    // ========================================================================
    /** @var ETA
     *  Particle pseudorapidity 
     * 
     *  @code
     * 
     *   const Particle* p    = ...         ;
     *   const double    eta  = ETA(  p ) ;
     *
     *  @endcode 
     *
     *  @see Particle
     *  @see LoKi::Particles::PseudoRapidity           
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    const LoKi::Particles::PseudoRapidity                 ETA ;
    // ========================================================================

    // ========================================================================
    /** @var PHI
     *  Particle asimuthal angle 
     * 
     *  @code
     * 
     *   const Particle* p    = ...         ;
     *   const double      phi  = PHI(  p ) ;
     *
     *  @endcode 
     *
     *  @see Particle
     *  @see LoKi::Particles::Phi
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const LoKi::Particles::Phi                           PHI ;
    // ========================================================================
    
    // ========================================================================
    /** @var M 
     *  Particle's mass (as <tt>sqrt(E*E-P*P)</tt>) 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     const double    m        = M( particle ) ;
     *
     *  @endcode
     *
     *  @see Particle
     *  @see LoKi::Particle::Mass
     *  @see LoKi::Function
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    const   LoKi::Particles::Mass                             M ;
    // ========================================================================

    // ========================================================================
    /** @var MM 
     *  Particle's measured mass 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     const double    mm       = MM( particle ) ;
     *
     *  @endcode
     *
     *  @see Particle
     *  @see LoKi::Particle::MeasuredMass
     *  @see LoKi::Function
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    const   LoKi::Particles::MeasuredMass                     MM ;
    // ========================================================================
    
    // ========================================================================
    /** invariant mass of subcombination of dauhter particles 
     *  
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     // evaluate the invarinat mass of 1st and 2nd daugter particles 
     *     Fun m12 = MASS(1,2) 
     *     const double  mass  = m12( particle ) ;
     *
     *  @endcode
     * 
     *  Up to 3 indices could be used with explicit constructors 
     *  and unlimited number of indices could be used with 
     *  constructor from the vector of indices 
     * 
     *  @see Particle
     *  @see LoKi::Particle::InvariantMass
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    typedef LoKi::Particles::InvariantMass                    MASS  ;
    // ========================================================================
    
    // ========================================================================
    /** @var M12 
     *  invariant mass of the first and the second daughter particles 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     // evaluate the invarinat mass of 1st and 2nd daugter particles 
     *     const double  m12  = M12( particle ) ;
     *
     *  @endcode
     *  
     *  @see Particle
     *  @see LoKi::Particle::InvariantMass
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    //const LoKi::Particles::InvariantMass                      M12(1,2) ;
    // ========================================================================
    
    // ========================================================================
    /** @var M13 
     *  invariant mass of the first and the third daughter particles 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     // evaluate the invarinat mass of 1st and 3rd daugter particles 
     *     const double  m13  = M13( particle ) ;
     *
     *  @endcode
     *  
     *  @see Particle
     *  @see LoKi::Particle::InvariantMass
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    //const LoKi::Particles::InvariantMass                    M13(1.3) ;
    // ========================================================================

    // ========================================================================
    /** @var M14 
     *  invariant mass of the first and the fourth daughter particles 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     // evaluate the invarinat mass of 1st and 4th daugter particles 
     *     const double  m14  = M14( particle ) ;
     *
     *  @endcode
     *  
     *  @see Particle
     *  @see LoKi::Particle::InvariantMass
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    //const LoKi::Particles::InvariantMass                    M14(1.4)  ;
    // ========================================================================

    // ========================================================================
    /** @var M23 
     *  invariant mass of the second and the third daughter particles 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     // evaluate the invarinat mass of 2nd and 3rd daugter particles 
     *     const double  m23  = M23( particle ) ;
     *
     *  @endcode
     *  
     *  @see Particle
     *  @see LoKi::Particle::InvariantMass
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    //const LoKi::Particles::InvariantMass                    M23(2,3) ;
    // ========================================================================

    // ========================================================================
    /** @var M24 
     *  invariant mass of the second and the fourth daughter particles 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     // evaluate the invarinat mass of 2nd and 4th daugter particles 
     *     const double  m24  = M24( particle ) ;
     *
     *  @endcode
     *  
     *  @see Particle
     *  @see LoKi::Particle::InvariantMass
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    //const LoKi::Particles::InvariantMass                   M24(2,4) ;
    // ========================================================================

    // ========================================================================
    /** @var M34 
     *  invariant mass of the third and the fourth daughter particles 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     // evaluate the invarinat mass of 3rd and 4th daugter particles 
     *     const double  m34  = M34( particle ) ;
     *
     *  @endcode
     *  
     *  @see Particle
     *  @see LoKi::Particle::InvariantMass
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    //const LoKi::Particles::InvariantMass                   M34(3,4) ;
    // ========================================================================

    // ========================================================================
    /** Delta Mass 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     Fun dm = DMASS( 3.010 * GeV );
     *     const double    mm       = dm( particle ) ;
     *
     *  @endcode
     *
     *  Alternatively the function object could be created
     *  from Particle ID or particle name, in this case it requires
     *  to be supplied with pointer to <tt>IParticlePpopertySvc</tt>
     *  service. The reference to <tt>ParticleProperty</tt> object 
     *  also could be used for creation of functor: 
     *  
     *
     *  @code 
     *
     *     const Particle*       particle = ... ; // get the particle
     *     IParticlePropertySvc* ppsvc    = ... ; // get the service 
     *     const ParticleProperty& D0     = ... ; // get from the service 
     *
     *     Fun dm1 = DMASS( "D0" , ppsvc ) ;
     *     const double    dm1       = dm1 ( particle ) ;
     *     Fun dm2 = DMASS( 241  , ppsvc ) ;
     *     const double    dm2       = dm2 ( particle ) ;
     *     Fun dm3 = DMASS( D0 ) ;
     *     const double    dm3       = dm3 ( particle ) ;
     *     Fun dm4 = DMASS( "D0" ) ;
     *     const double    mm4       = dm4 ( particle ) ;
     *     Fun dm5 = DMASS( ParticleID( 421 )  ) ;
     *     const double    mm5       = dm5 ( particle ) ;
     *
     *  @endcode
     *
     *  @see Particle
     *  @see LoKi::Particle::MeasuredMass
     *  @see LoKi::Function
     *  @see LoKi::Cuts::Fun
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */        
    typedef LoKi::Particles::DeltaMass                        DMASS ;
    // ========================================================================
    
    // ========================================================================
    /** Absolute value for delta Mass 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     Fun dm = ADMASS( 3.010 * GeV );
     *     const double    mm       = dm( particle ) ;
     *
     *  @endcode
     *
     *  Alternatively the function object could be created
     *  from Particle ID or particle name, in this case it requires
     *  to be supplied with pointer to <tt>IParticlePpopertySvc</tt>
     *  service. The reference to <tt>ParticleProperty</tt> object 
     *  also could be used for creation of functor: 
     *  
     *
     *  @code 
     *
     *     const Particle*       particle = ... ; // get the particle
     *     IParticlePropertySvc* ppsvc    = ... ; // get the service 
     *     const ParticleProperty& D0     = ... ; // get from the service 
     *
     *     Fun dm1 = ADMASS( "D0" , ppsvc ) ;
     *     const double    dm1       = dm1 ( particle ) ;
     *     Fun dm2 = ADMASS( 241  , ppsvc ) ;
     *     const double    dm2       = dm2 ( particle ) ;
     *     Fun dm3 = ADMASS( D0 ) ;
     *     const double    dm3       = dm3 ( particle ) ;
     *     Fun dm4 = ADMASS( "D0" ) ;
     *     const double    mm4       = dm4 ( particle ) ;
     *     Fun dm5 = ADMASS( ParticleID( 421 )  ) ;
     *     const double    mm5       = dm5 ( particle ) ;
     *
     *  @endcode
     *
     *  @see Particle
     *  @see LoKi::Particle::AbsDeltaMass
     *  @see LoKi::Function
     *  @see LoKi::Cuts::Fun
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */        
    typedef LoKi::Particles::AbsDeltaMass                    ADMASS ;
    // ========================================================================
    
    // ========================================================================
    /** Delta Measured Mass 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     Fun dmm = DMMASS( 3.010 * GeV );
     *     const double    mm       = dmm( particle ) ;
     *
     *  @endcode
     *
     *  Alternatively the function object could be created
     *  from Particle ID or particle name, in this case it requires
     *  to be supplied with pointer to <tt>IParticlePpopertySvc</tt>
     *  service. The reference to <tt>ParticleProperty</tt> object 
     *  also could be used for creation of functor: 
     *       
     *  @code 
     *
     *     const Particle*       particle = ... ; // get the particle
     *     IParticlePropertySvc* ppsvc    = ... ; // get the service 
     *     const ParticleProperty& D0     = ... ; // get from the service 
     *
     *     Fun dmm1 = DMMASS( "D0" , ppsvc ) ;
     *     const double    mm1       = dmm1 ( particle ) ;
     *     Fun dmm2 = DMMASS( 241  , ppsvc ) ;
     *     const double    mm2       = dmm2 ( particle ) ;
     *     Fun dmm3 = DMMASS( D0 ) ;
     *     const double    mm3       = dmm3 ( particle ) ;
     *     Fun dmm4 = DMMASS( "D0" ) ;
     *     const double    mm4       = dmm4 ( particle ) ;
     *     Fun dmm5 = DMMASS( ParticleID( 421 )  ) ;
     *     const double    mm5       = dmm5 ( particle ) ;
     *
     *
     *  @endcode
     *
     *  @see Particle
     *  @see LoKi::Particle::MeasuredMass
     *  @see LoKi::Function
     *  @see LoKi::Cuts::Fun
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */        
    typedef LoKi::Particles::DeltaMeasuredMass                DMMASS ;
    // ========================================================================
    
    // ========================================================================
    /** Absolute value of delta Measured Mass 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     Fun dmm = ADMMASS( 3.010 * GeV );
     *     const double    mm       = dmm( particle ) ;
     *
     *  @endcode
     *
     *  Alternatively the function object could be created
     *  from Particle ID or particle name, in this case it requires
     *  to be supplied with pointer to <tt>IParticlePpopertySvc</tt>
     *  service. The reference to <tt>ParticleProperty</tt> object 
     *  also could be used for creation of functor: 
     *       
     *  @code 
     *
     *     const Particle*       particle = ... ; // get the particle
     *     IParticlePropertySvc* ppsvc    = ... ; // get the service 
     *     const ParticleProperty& D0     = ... ; // get from the service 
     *
     *     Fun dmm1 = ADMMASS( "D0" , ppsvc ) ;
     *     const double    mm1       = dmm1 ( particle ) ;
     *     Fun dmm2 = ADMMASS( 241  , ppsvc ) ;
     *     const double    mm2       = dmm2 ( particle ) ;
     *     Fun dmm3 = ADMMASS( D0  ) ;
     *     const double    mm3       = dmm3 ( particle ) ;
     *     Fun dmm4 = ADMMASS( "D0" ) ;
     *     const double    mm4       = dmm4 ( particle ) ;
     *     Fun dmm5 = ADMMASS( ParticleID( 421 )  ) ;
     *     const double    mm5       = dmm5 ( particle ) ;
     *
     *  @endcode
     *
     *  @see Particle
     *  @see LoKi::Particle::DeltaMeasuredMass
     *  @see LoKi::Particle::AbsDeltaMeasuredMass
     *  @see LoKi::Function
     *  @see LoKi::Cuts::Fun
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */        
    typedef LoKi::Particles::AbsDeltaMeasuredMass            ADMMASS ;    
    // ========================================================================

    // ========================================================================
    /** Delta Measured Mass in chi2 units 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     Fun dmm = CHI2M ( 3.010 * GeV );
     *     const double  chi2  = dmm( particle ) ;
     *
     *  @endcode
     *
     *  Alternatively the function object could be created
     *  from Particle ID or particle name, in this case it requires
     *  to be supplied with pointer to <tt>IParticlePpopertySvc</tt>
     *  service: 
     *
     *  @code 
     *
     *     const Particle*       particle = ... ; // get the particle
     *     IParticlePropertySvc* ppsvc    = ... ; // get the service 
     *     const ParticleProperty& D0     = ... ; // get from the service 
     *
     *     Fun dmm1 = CHI2M( "D0" , ppsvc ) ;
     *     const double    chi2_1       = dmm1 ( particle ) ;
     *     Fun dmm2 = CHI2M( 241  , ppsvc ) ;
     *     const double    chi2_2       = dmm2 ( particle ) ;
     *     Fun dmm3 = CHI2M( D0 ) ;
     *     const double    chi2_3       = dmm3 ( particle ) ;
     *
     *  @endcode
     *
     *  More simple ways are also valid:
     *
     *  @code 
     *
     *     const Particle*       particle = ... ; // get the particle
     *
     *     Fun dmm1 = CHI2M( LHCb::ParticleID( 241 ) ) ;
     *     const double    chi2_1       = dmm1 ( particle ) ;
     *     Fun dmm2 = CHI2M( "D0" ) ;
     *     const double    chi2_2       = dmm2 ( particle ) ;
     *
     *  @endcode
     *
     *  @see Particle
     *  @see LoKi::Particle::DeltaMeasuredMassChi2
     *  @see LoKi::Particle::DeltaMeasuredMass
     *  @see LoKi::Particle::MeasuredMass
     *  @see LoKi::Function
     *  @see LoKi::Cuts::Fun
     *  @attention LoKi::Particles::ErrorValue returned on error 
     */        
    typedef LoKi::Particles::DeltaMeasuredMassChi2            CHI2M ;
    // ========================================================================

    // ========================================================================
    /** Delta Measured Mass in chi2 units 
     *
     *  @code 
     *
     *     const Particle* particle = ... ; //  get the particle
     *     Fun dmm = CHI2MASS ( 3.010 * GeV );
     *     const double  chi2  = dmm( particle ) ;
     *
     *  @endcode
     *
     *  Alternatively the function object could be created
     *  from Particle ID or particle name, in this case it requires
     *  to be supplied with pointer to <tt>IParticlePpopertySvc</tt>
     *  service: 
     *
     *  @code 
     *
     *     const Particle*       particle = ... ; // get the particle
     *     IParticlePropertySvc* ppsvc    = ... ; // get the service 
     *     const ParticleProperty& D0     = ... ; // get from the service 
     *
     *     Fun dmm1 = CHI2MASS( "D0" , ppsvc ) ;
     *     const double    chi2_1       = dmm1 ( particle ) ;
     *     Fun dmm2 = CHI2MASS( 241  , ppsvc ) ;
     *     const double    chi2_2       = dmm2 ( particle ) ;
     *     Fun dmm3 = CHI2MASS( D0 ) ;
     *     const double    chi2_3       = dmm3 ( particle ) ;
     *
     *  @endcode
     *
     *  More simple ways are also valid:
     *
     *  @code 
     *
     *     const Particle*       particle = ... ; // get the particle
     *
     *     Fun dmm1 = CHI2MASS( LHCb::ParticleID( 241 ) ) ;
     *     const double    chi2_1       = dmm1 ( particle ) ;
     *     Fun dmm2 = CHI2MASS( "D0" ) ;
     *     const double    chi2_2       = dmm2 ( particle ) ;
     *
     *  @endcode
     *
     *  @see Particle
     *  @see LoKi::Particle::DeltaMeasuredMassChi2
     *  @see LoKi::Particle::DeltaMeasuredMass
     *  @see LoKi::Particle::MeasuredMass
     *  @see LoKi::Function
     *  @see LoKi::Cuts::Fun
     *  @attention LoKi::Particles::ErrorValue returned on error 
     */        
    typedef LoKi::Particles::DeltaMeasuredMassChi2            CHI2MASS ;
    // ========================================================================
    
    // ========================================================================
    /** @var CL
     *  Trivial function which evaluates "confidence level" for 
     *  the particle
     *
     *  It relies on the method LHCb::Particle::confLevel
     *
     *  @code 
     * 
     *   const LHCb::Particle* p = ... ;
     *   const double cl = CL( p ) ;
     *  
     *  @endcode 
     *
     *  @see LHCb::Particle
     *  @see LoKi::Particles::ConfidenceLevel 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const   LoKi::Particles::ConfidenceLevel                  CL ;
    // ========================================================================

    // ========================================================================
    /** @var CONFLEVEL
     *  Trivial function which evaluates "confidence level" for 
     *  the particle
     *
     *  It relies on the method LHCb::Particle::confLevel
     *
     *  @code 
     * 
     *   const LHCb::Particle* p = ... ;
     *   const double cl = CONFLEVEL ( p ) ;
     *  
     *  @endcode 
     *
     *  @see LHCb::Particle
     *  @see LoKi::Particles::ConfidenceLevel 
     *  @see LoKi::Cuts::CL
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const   LoKi::Particles::ConfidenceLevel               CONFLEVEL = CL ;
    // ========================================================================
    
    // ========================================================================
    /** @var CONFLEV
     *  Trivial function which evaluates "confidence level" for 
     *  the particle
     *
     *  It relies on the method LHCb::Particle::confLevel
     *
     *  @code 
     * 
     *   const LHCb::Particle* p = ... ;
     *   const double cl = CONFLEV ( p ) ;
     *  
     *  @endcode 
     *
     *  @see LHCb::Particle
     *  @see LoKi::Particles::ConfidenceLevel 
     *  @see LoKi::Cuts::CL
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const   LoKi::Particles::ConfidenceLevel               CONFLEV = CL ;
    // ========================================================================
    
    // ========================================================================
    /** @var WEIGHT
     *  Trivial function which evaluates "weight" for 
     *  the particle
     *
     *  It relies on the method LHCb::Particle::weight
     *
     *  @code 
     * 
     *   const LHCb::Particle* p = ... ;
     *   const double weight = WEIGHT( p ) ;
     *  
     *  @endcode 
     *
     *  @see LHCb::Particle
     *  @see LoKi::Particles::Weight
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const   LoKi::Particles::Weight                  WEIGHT;
    // ========================================================================


    // ========================================================================
    /** @var W
     *  Trivial function which evaluates "weight" for 
     *  the particle
     *
     *  It relies on the method LHCb::Particle::weight
     *
     *  @code 
     * 
     *   const LHCb::Particle* p = ... ;
     *   const double weight = W( p ) ;
     *  
     *  @endcode 
     *
     *  @see LoKi::Cuts::WEIGHT
     *  @see LHCb::Particle
     *  @see LoKi::Particles::Weight
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const   LoKi::Particles::Weight                  W = WEIGHT ;
    // ========================================================================
    
    // ========================================================================
    /** @var ISBASIC 
     *  Trivial function which evaluates "weight" for 
     *  Trivial predicate which evaluates to "true" for 
     *  the "basic" particles 
     *
     *  It relies on the method LHCb::Particle::isBasicParticle
     *
     *  @code 
     * 
     *   const LHCb::Particle* p = ... ;
     *   const bool basic = ISBASIC( p ) ;
     *  
     *  @endcode 
     *
     *  @see LHCb::Particle
     *  @see LoKi::Particles::IsBasic
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const   LoKi::Particles::IsBasic              ISBASIC ;
    // ========================================================================

    // ========================================================================
    /** @var BASIC 
     *  Trivial function which evaluates "weight" for 
     *  Trivial predicate which evaluates to "true" for 
     *  the "basic" particles 
     *
     *  It relies on the method LHCb::Particle::isBasicParticle
     *
     *  @code 
     * 
     *   const LHCb::Particle* p = ... ;
     *   const bool basic = BASIC( p ) ;
     *  
     *  @endcode 
     *
     *  @see LoKi::Cuts::ISBASIC
     *  @see LHCb::Particle
     *  @see LoKi::Particles::IsBasic
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const   LoKi::Particles::IsBasic              BASIC = ISBASIC ;
    // ========================================================================

    // ========================================================================
    /** @var NDAUGS 
     *  Trivial function which evaluates number of daughters for 
     *  the particle
     *
     *  It relies on the method LHCb::Particle::daughters().size()
     *
     *  @code 
     * 
     *   const LHCb::Particle* p = ... ;
     *   const int nDau = NDAUGS ( p ) ;
     *  
     *  @endcode 
     *
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::NumberOfDaughters
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Particles::NumberOfDaughters                 NDAUGS ;
    // ========================================================================
    
    // ========================================================================
    /** @var NDAUGHTERS 
     *  Trivial function which evaluates number of daughters for 
     *  the particle
     *
     *  It relies on the method LHCb::Particle::daughters().size()
     *
     *  @code 
     * 
     *   const LHCb::Particle* p = ... ;
     *   const int nDau = NDAUGHTERS  ( p ) ;
     *  
     *  @endcode 
     *
     *  @see LHCb::Cuts::NDAUGS 
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::NumberOfDaughters
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Particles::NumberOfDaughters    NDAUGHTERS = NDAUGS ;
    // ========================================================================
    
    // ========================================================================
    /** Trivial predicate which evaluates LHCb::Particle::hasInfo
     *  function
     *
     *  It relies on the method LHCb::Particle::hasInfo
     *
     *  @code 
     * 
     *  const LHCb::Particle* p = ... ;
     *  Cut hasWeight = HASINFO( LHCb::Particle::Weight ) ;
     * 
     *  const bool good = hasWeight( p ) ;
     *
     *  @endcode 
     *
     * 
     *  @see LHCb::Particle
     *  @see LoKi::Particles::HasInfo
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    typedef LoKi::Particles::HasInfo                         HASINFO ;
    // ========================================================================

    // ========================================================================
    /** Trivial predicate which evaluates LHCb::Particle::hasInfo
     *  function
     *
     *  It relies on the method LHCb::Particle::hasInfo
     *
     *  @code 
     * 
     *  const LHCb::Particle* p = ... ;
     *  Cut hasWeight = HASPINFO( LHCb::Particle::Weight ) ;
     * 
     *  const bool good = hasWeight( p ) ;
     *
     *  @endcode 
     *
     * 
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::HASINFO
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    typedef HASINFO                                          HASPINFO ;
    // ========================================================================
    
    // ========================================================================
    /** Trivial predicate which evaluates LHCb::Particle::hasInfo
     *  function
     *
     *  It relies on the method LHCb::Particle::hasInfo
     *
     *  @code 
     * 
     *  const LHCb::Particle* p = ... ;
     *  Cut hasWeight = PHASINFO( LHCb::Particle::Weight ) ;
     * 
     *  const bool good = hasWeight( p ) ;
     *
     *  @endcode 
     *
     * 
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::HASINFO
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    typedef HASINFO                                          PHASINFO ;
    // ========================================================================
   
    // ========================================================================
    /** Trivial function which evaluates LHCb::Particle::info
     *  
     *  It relies on the method LHCb::Particle::info
     *
     *  @code 
     * 
     *  const LHCb::Particle* p = ... ;
     *
     *  const int index = ... ;
     *
     *  Fun info = INFO( index , -1000 ) ;
     * 
     *  const double result = info( p ) ;
     *
     *  @endcode 
     *
     * 
     *  @see LHCb::Particle
     *  @see LoKi::Particles::Info
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    typedef LoKi::Particles::Info                                 INFO ;
    // ========================================================================

    // ========================================================================
    /** Trivial function which evaluates LHCb::Particle::info
     *  
     *  It relies on the method LHCb::Particle::info
     *
     *  @code 
     * 
     *  const LHCb::Particle* p = ... ;
     *
     *  const int index = ... ;
     *
     *  Fun info = PINFO( index , -1000 ) ;
     * 
     *  const double result = info( p ) ;
     *
     *  @endcode 
     *
     * 
     *  @see LoKi::Cuts::INFO
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    typedef INFO                                                  PINFO ;
    // ========================================================================
    
    // ========================================================================
    /** @var HASPROTO
     *  Trivial predicate which evaluates to true 
     *  for particles with the valid protoparticle 
     *
     *  It relies on the method LHCb::Particle::proto()
     *
     *  @code 
     * 
     *  const LHCb::Particle* p = ... ;
     * 
     *  const bool good = HASPROTO( p ) ;
     *
     *  @endcode 
     *
     * 
     *  @see LHCb::Particle
     *  @see LoKi::Particles::HasProto
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Particles::HasProto                            HASPROTO ;
    // ========================================================================
    

    // ========================================================================
    /** @var HASVERTEX
     *  Trivial predicate which evaluates to true 
     *  for particles with the valid endVertex 
     *
     *  It relies on the method LHCb::Particle::endVertex()
     *
     *  @code 
     * 
     *  const LHCb::Particle* p = ... ;
     * 
     *  const bool good = HASVERTEX( p ) ;
     *
     *  @endcode 
     *
     * 
     *  @see LHCb::Particle
     *  @see LoKi::Particles::HasVertex
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Particles::HasVertex                          HASVERTEX ;
    // ========================================================================
    
    // ========================================================================
    /** Evaluator of the GEOMETRY 
     *  distance between the particle 
     *  "endVertex" and "the vertex". 
     *
     *  @code 
     *
     *  const LHCb::Particle* p = ... ;
     * 
     *  const LHCb::PrimVertex* pv = ... ;
     * 
     *  Fun dist = VD( pv ) ;
     * 
     *  const double flight = dist( p ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::Particles::VertexDistance 
     *  @see LHCb::Vertex
     *  @see LHCb::Particle
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    typedef LoKi::Particles::VertexDistance      VD ;
    // ========================================================================
    
    // ========================================================================
    /** Evaluator of the GEOMETRY distance between the particle 
     *  "endVertex" and "the vertex". The distance is signed according 
     *  to the sign of (Zv-Z0)
     *
     *  @code 
     *
     *  const LHCb::Particle* p = ... ;
     * 
     *  const LHCb::PrimVertex* pv = ... ;
     * 
     *  Fun dist = VDSIGN( pv ) ;
     * 
     *  const double flight = dist( p ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::Particles::VertexSignedDistance 
     *  @see LHCb::Vertex
     *  @see LHCb::Particle
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    typedef LoKi::Particles::VertexSignedDistance      VDSIGN ;
    // ========================================================================
    
    // ========================================================================
    /** Evaluator of the distance between the particle 
     *  "endVertex" and "the vertex" along the particle momentum
     *
     *  @code 
     *
     *  const LHCb::Particle* p = ... ;
     * 
     *  const LHCb::PrimVertex* pv = ... ;
     * 
     *  Fun dist = VDDOT( pv ) ;
     * 
     *  const double flight = dist( p ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::Particles::VertexDotDistance 
     *  @see LHCb::Vertex
     *  @see LHCb::Particle
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    typedef LoKi::Particles::VertexDotDistance      VDDOT ;
    // ========================================================================

    // ========================================================================
    /** Evaluator of the chi2 of GEOMETRY distance between the particle 
     *  "endVertex" and "the vertex" 
     *
     *  @code 
     *
     *  const LHCb::Particle* p = ... ;
     * 
     *  const LHCb::PrimVertex* pv = ... ;
     * 
     *  Fun dist = VDCHI2( pv ) ;
     * 
     *  const double chi2 = dist( p ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::Particles::VertexChi2Distance 
     *  @see LHCb::Vertex
     *  @see LHCb::Particle
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    typedef LoKi::Particles::VertexChi2Distance      VDCHI2 ;
    // ========================================================================

    // ========================================================================
    /** Evaluator of the minimal GEOMETRY distance between the particle 
     *  "endVertex" and some set of vertices  
     *
     *  @code 
     *
     *  const LHCb::Particle* p = ... ;
     * 
     *  const LHCb::PrimVertex* pv = ... ;
     * 
     *  Fun dist = VDMIN( pv ) ;
     * 
     *  const double minDistance = dist( p ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::Particles::MinVertexDistance 
     *  @see LHCb::Vertex
     *  @see LHCb::Particle
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    typedef LoKi::Particles::VertexChi2Distance      VDMIN ;
    // ========================================================================
    
    // ========================================================================
    /** Evaluator of the minimal GEOMETRY distance between the particle 
     *  "endVertex" and some set of vertices  
     *
     *  @code 
     *
     *  const LHCb::Particle* p = ... ;
     * 
     *  const LHCb::PrimVertices* pv = ... ;
     * 
     *  Fun dist = MINVD( pv ) ;
     * 
     *  const double minDistance = dist( p ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi:Cuts::MinVertexDistance
     *  @see LHCb::Vertex
     *  @see LHCb::Particle
     *  @see LoKi:Cuts::VDMIN
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    typedef VDMIN                                          MINVD ;
    // ========================================================================

    // ========================================================================
    /** Evaluator of the minimal chi2 for distance between the particle 
     *  "endVertex" and some set of vertices  
     *
     *  @code 
     *
     *  const LHCb::Particle* p = ... ;
     * 
     *  const LHCb::PrimVertices* pv = ... ;
     * 
     *  Fun dist = VDMINCHI2( pv ) ;
     * 
     *  const double chi2  = dist( p ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::Particles::MinVertexChi2Distance 
     *  @see LHCb::Vertex
     *  @see LHCb::Particle
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    typedef LoKi::Particles::MinVertexChi2Distance      VDMINCHI2 ;
    // ========================================================================
    
    // ========================================================================
    /** Evaluator of the minimal chi2 for distance between the particle 
     *  "endVertex" and some set of vertices  
     *
     *  @code 
     *
     *  const LHCb::Particle* p = ... ;
     * 
     *  const LHCb::PrimVertices* pv = ... ;
     * 
     *  Fun dist = MINVDCHI2( pv ) ;
     * 
     *  const double chi2  = dist( p ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::Particles::MinVertexChi2Distance 
     *  @see LHCb::Vertex
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::VDMINCHI2 
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    typedef VDMINCHI2                          MINVDCHI2 ;
    // ========================================================================

    
    
  } ; // end of namespace LoKi::Cuts 
  
} ; // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSPARTICLECUTS_H
// ============================================================================
