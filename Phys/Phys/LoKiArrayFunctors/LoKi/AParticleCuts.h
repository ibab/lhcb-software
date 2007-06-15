// $Id: AParticleCuts.h,v 1.1.1.1 2007-06-15 08:04:18 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_APARTICLECUTS_H 
#define LOKI_APARTICLECUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AParticles.h"
// ============================================================================
namespace LoKi 
{
  namespace Cuts 
  {
    // ========================================================================
    /** @var ASIZE
     *  Simple functor  to get the size of the combinaton
     *
     *  @code 
     *  
     *    const LHCb::Particle::ConstVector& combination = ... ;
     * 
     *    const bool empty = 1 >  ASIZE ( combination ) ;
     *
     *  @endcode 
     *  @see LoKi::AParticles::Size  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09
     */
    const   LoKi::AParticles::Size                                    ASIZE   ;
     // ========================================================================    
    /** @typedef ANUM
     *
     *  Simple functor to count number of particles in a combination, 
     *  which satisfy certain combination.
     *  It uses the algorithm LoKi::Algs::count_if 
     *
     *  @code 
     *
     *    const LHCb::Particle::ConstVector& combination = ... ;
     *
     *    // count number of positive kaons: 1) create the functor:
     *    const AFun nK = ANUM ( "K+" == ID ) ;
     *    // use it!
     *    const doube nKplus = nK( combination ) ;
     * 
     *  @endcode 
     * 
     *  @see LoKi::AParticles::Count
     *  @see LoKi::Algs::count_if 
     *  @see LoKi::Cuts::ID 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09
     */
    typedef LoKi::AParticles::Count                                   ANUM    ;
    // ========================================================================
    /** @typedef AUNIQUE 
     *  Simple predicate to check the overlaps, using ICheckOverlap tool
     *
     *  @code 
     *
     *    const LHCb::Particle::ConstVector& combination = ... ;
     *    // get the tool:
     *    ICheckOverlap* tool = ... ;
     *    // create the predicate:
     *    const ACut unique = AUNIQUE( tool ) ;
     *
     *    // check the uniqueness:
     *    const bool OK = unique ( combination ) ;
     *
     *  @endcode 
     *
     *  @see ICheckOverlap
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09
     */
    typedef LoKi::AParticles::Unique                                  AUNIQUE ;
    // ========================================================================
    /** @typedef AMASS
     *  Simple evaluator of the invariant mass for the subcombination from the
     *  combination itself
     * 
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    // functor which evaluated the invariant mass of 1st and 2nd particles:
     *    const AFun M12 = AMASS(1,2) ;
     * 
     *    const double m12 = M12( comb ) ;
     *
     *  @endcode 
     *  
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::AParticles::InvariantMass
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    typedef LoKi::AParticles::InvariantMass                           AMASS ;
   // ========================================================================
    /** @var AM
     *
     *  Trivial functor to get the overall mass of the combination.
     *  It uses the function LoKi::Kinematics::addMomenta
     *
     *  @code 
     *
     *    const LHCb::Particle::ConstVector& combination = ... ;
     * 
     *    const double mass = AM  ( combination ) ;
     * 
     *  @endcode 
     * 
     *  @see LoKi::AParticles::InvarinatMass
     *  @see LoKi::Kinematics::addMomenta
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09
     */
    const   LoKi::AParticles::InvarinatMass                          AM(0)    ;
    // ========================================================================
    /** @var AM12
     *  Simple function , whoich evaluates the invariant mass of 1st and 2nd 
     *  particle
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double m12 = AM12( comb ) ;
     *
     *  @endcode 
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::Cuts::AM
     *  @see LoKi::AParticles::InvariantMass
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const LoKi::AParticles::InvariantMass                         AM12  (1,2) ;
    // ========================================================================
    /** @var AM13
     *  Simple function, which evaluates the invariant mass of 1st and 3rd 
     *  particle
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double m13 = AM13( comb ) ;
     *
     *  @endcode 
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::Cuts::AM
     *  @see LoKi::AParticles::InvariantMass
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const LoKi::AParticles::InvariantMass                         AM13  (1,3) ;
    // ========================================================================
    /** @var AM14
     *  Simple function, which evaluates the invariant mass of 1st and 4th 
     *  particle
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double m14 = AM14( comb ) ;
     *
     *  @endcode 
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::Cuts::AM
     *  @see LoKi::AParticles::InvariantMass
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const LoKi::AParticles::InvariantMass                         AM14  (1,4) ;
    // ========================================================================
    /** @var AM23
     *  Simple function , whoich evaluates the invariant mass of 2nd and 3rd 
     *  particle
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double m23 = AM23( comb ) ;
     *
     *  @endcode 
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::Cuts::AM
     *  @see LoKi::AParticles::InvariantMass
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const LoKi::AParticles::InvariantMass                         AM23  (2,3) ;
    // ========================================================================
    /** @var AM24
     *  Simple function , which evaluates the invariant mass of 2nd and 4th
     *  particle
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double m24 = AM24( comb ) ;
     *
     *  @endcode 
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::Cuts::AM
     *  @see LoKi::AParticles::InvariantMass
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const LoKi::AParticles::InvariantMass                         AM24  (2,4) ;
    // ========================================================================
    /** @var AM34
     *  Simple function, which evaluates the invariant mass of 3rd and 4th 
     *  particle
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double m34 = AM34( comb ) ;
     *
     *  @endcode 
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::Cuts::AM
     *  @see LoKi::AParticles::InvariantMass
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const LoKi::AParticles::InvariantMass                         AM34  (3,4) ;
    // ========================================================================
    /** @typedef AMOM
     *  Simple evaluator of momentum of the various subcombintions
     * 
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    // functor which evaluated the momentum of 1st and 2nd particles:
     *    const AFun   P12 = AMOM(1,2) ;
     *
     *    const double p12 = P12( comb ) ;
     *
     *  @endcode 
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::Cuts::AMOM
     *  @see LoKi::AParticles::Momentum
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    typedef LoKi::AParticles::Momentum                                   AMOM ;
    // ========================================================================
    /** @var AP
     *  Simple evaluator of the momentum fo the combination:
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double p = AP ( comb ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::AMOM
     *  @see LoKi::AParticles::Momentum
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const   LoKi::AParticles::Momentum                                   AP(0);
    // ========================================================================
    /** @typedef AMOMT
     *  Simple evaluator of transvese momentum of the various subcombintions
     * 
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    // functor which evaluated the transverse momentum of 1st and 2nd particles:
     *    const AFun   Pt12 = AMOMT(1,2) ;
     *
     *    const double pt12 = Pt12( comb ) ;
     *
     *  @endcode 
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::AParticles::TransverseMomentum
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    typedef LoKi::AParticles::Momentum                                   AMOMT;
    // ========================================================================
    /** @var APT
     *  Simple evaluator of the transverse momentum fo the combination:
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double pt = APT ( comb ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::AMOMT
     *  @see LoKi::AParticles::TransverseMomentum
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const   LoKi::AParticles::TransverseMomentum                        APT(0);
    // ========================================================================
    /** @typedef AETAP
     *  Simple evaluator of pseudorapidity of the various subcombintions
     * 
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    // functor which evaluated the pseudorapiditi of 1st and 2nd particles:
     *    const AFun   eta12 = AETAPT(1,2) ;
     *
     *    const double eta = eta12( comb ) ;
     *
     *  @endcode 
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::AParticles::Eta
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    typedef LoKi::AParticles::Eta                                        AETAP;
    // ========================================================================
    /** @var AETA
     *  Simple evaluator of the pseudorapidity of the combination
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double eta = AETA ( comb ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::AETAP
     *  @see LoKi::AParticles::Eta
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const   LoKi::AParticles::Eta                                      AETA(0);
    // ========================================================================
  } // end of namespace LoKi::Cuts 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_APARTICLECUTS_H
// ============================================================================
