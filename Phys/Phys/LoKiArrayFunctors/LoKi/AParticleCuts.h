// $Id$
// ============================================================================
#ifndef LOKI_APARTICLECUTS_H 
#define LOKI_APARTICLECUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AParticles.h"
#include "LoKi/Monitoring.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    // Suppress Intel compiler warnings about missing default constructor
    // In this case the compiler generated constructor is fine, since there are
    // no member data to be initialised
#ifdef __INTEL_COMPILER
#pragma warning(disable:854)
#pragma warning(push)
#endif
    // ========================================================================
    /** @typedef ACHI2DOCA 
     *  The evaluator of chi2-distance of the closest approach 
     *  for all two-particle subcombinations
     *  
     *  @code
     * 
     *  IDistanceCalculator* doca = ... ;
     *  AFun chi2_12 = ACHI2DOCA ( 1 , 2 , doca ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *
     *  const double chi2 = chi2_12 ( combination ) ;
     *
     *  @endcode
     *  
     *  @see IDistanceCalculator
     *  @see LoKi::AParticles::DOCAChi2
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::DOCAChi2                              ACHI2DOCA ;
    // ========================================================================
    /** @typedef ACHI2V
     *  Simple fuction which evaluates chi2 of the vertex fit for the 
     *  combination
     *
     *  @code
     * 
     *  IVertexFit* fitter = ... ;
     *  AFun vChi2 = ACHI2V ( fitter ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *
     *  const double chi2 = fun ( combination ) ;
     *
     *  @endcode
     * 
     *  @see IVertexFit
     *  @see LoKi::AParticles::VertexChi2
     *  @see LoKi::Cuts::AVCHI2
     *  @see LoKi::Cuts::ACHI2VX
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::VertexChi2                               ACHI2V ;
    // ========================================================================
    /** @typedef ACHI2VX
     *  Simple fuction which evaluates chi2 of the vertex fit for the 
     *  combination
     *
     *  @code
     * 
     *  IVertexFit* fitter = ... ;
     *  AFun vChi2 = ACHI2VX ( fitter ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *
     *  const double chi2 = fun ( combination ) ;
     *
     *  @endcode
     * 
     *  @see IVertexFit
     *  @see LoKi::AParticles::VertexChi2
     *  @see LoKi::Cuts::AVCHI2
     *  @see LoKi::Cuts::ACHI2V
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::VertexChi2                              ACHI2VX ;
    // ========================================================================
    /** @typedef ACHILD
     *  The function which evaluates another function using the 
     *  defined child particle
     *  
     *  Get the DLL(K-pi) for the first particle 
     *  @code 
     * 
     *  // Get the DLL(K-pi) for the first particle 
     *  AFun fun = ACHILD ( PIDK , 1 ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *  
     *  const double kaonDLL = fun ( combination ) ;
     *
     *  @endcode
     *  @see LoKi::Cuts::PIDK
     * 
     *  @see LoKi::AParticles::ChildFun
     *  @see LoKi::Cuts::ACHILDFUN
     *  @see LoKi::Cuts::AFUNCHILD
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::ChildFun                                 ACHILD ;
    // ========================================================================
    /** @typedef ACHILDCUT
     *  The predicate which evaluates another predicate using the 
     *  defined child particle
     *  
     *  check if the first daughter has good DLL(K-pi) 
     *  @code 
     * 
     *  // check for good tDLL(K-pi) for the first particle 
     *  ACut cut = ACHILDCUT ( PIDK > 0 , 1 ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *  
     *  const bool goodKaonDLL = cut  ( combination ) ;
     *
     *  @endcode
     *  @see LoKi::Cuts::PIDK
     * 
     *  @see LoKi::AParticles::ChildCut
     *  @see LoKi::Cuts::ACUTCHILD
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::ChildCut                              ACHILDCUT ;
    // ========================================================================
    /** @typedef ACHILDFUN
     *  The function which evaluates another function using the 
     *  defined child particle
     *  
     *  Get the DLL(K-pi) for the first particle 
     *  @code 
     * 
     *  // Get the DLL(K-pi) for the first particle 
     *  AFun fun = ACHILDFUN ( PIDK , 1 ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *  
     *  const double kaonDLL = fun ( combination ) ;
     *
     *  @endcode
     *  @see LoKi::Cuts::PIDK
     * 
     *  @see LoKi::AParticles::ChildFun
     *  @see LoKi::Cuts::ACHILD
     *  @see LoKi::Cuts::AFUNCHILD
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::ChildFun                              ACHILDFUN ;
    // ========================================================================
    /** @typedef ACOUNTER
     *  Simple monitoring counter for predicates
     *
     *  @code 
     *
     *  // some predicate to be monitored
     *  ACut cut = ... ;
     *
     *  // Create the monitored predicate using the generic counter
     *  // either local counter, or the counter from IStatSvc or ICounterSvc
     *  StatEntity* counter = ... ; ///< the generic counter
     *  ACut mon = COUNTER ( cut , counter ) ;
     *
     *  for ( ... ) 
     *    {
     *      ...
     *      const LHCb::Particle::ConstVector& p = ... ;
     *      const bool result = mon( p ) ; ///< use the predicate
     *      ...
     *    }
     *
     *  ...
     *  info () 
     *     << " Monitoring results : "                       << endmsg 
     *     << " NEntries:  #" << counter->entries ()         << endmsg 
     *     << " Efficiency:(" << counter->eff     ()   
     *     << "+="            << counter->effErr  () << ")%" << endmsg ;
     *
     *  @endcode
     *
     *  The alternative (recommended) way to create the monitored predicate:
     *  @code 
     *
     *  ACut cut = ... ;
     *
     *  ACut mon = monitor ( cut , counter("SomeEffCounter") ) ;
     *  
     *  @endcode 
     *
     *  The substitution of the predicate by monitored predicate 
     *  cound be done "on-flight" without the disturbing of the
     *  actual processing:
     *  @code
     *
     *  // some predicate to be monitored
     *  ACut cut = ... ;
     *
     *  if ( monitoring ) 
     *    {
     *       cut = monitor ( cut , "Efficiency1" ) ;
     *    }
     *
     *  @endcode
     *
     *  @attention The string representation of the object 
     *             is delegated to the underlying prediate,  
     *             therefore the object is NOT recontructable 
     *             from its string representations. It is done 
     *             on purpose to avoid the disturbing of ids. 
     * 
     *  @see LoKi::Monitoring::Counter
     *  @see LoKi::monitor
     *  @see StatEntity
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03 
     */        
    typedef LoKi::Monitoring::Counter<LoKi::ATypes::Combination,bool> ACOUNTER ;
    // ========================================================================
    /** @typedef ACUTCHILD
     *  The predicate which evaluates another predicate using the 
     *  defined child particle
     *  
     *  check if the first daughter has good DLL(K-pi) 
     *  @code 
     * 
     *  // check for good tDLL(K-pi) for the first particle 
     *  ACut cut = ACUTCHILD ( PIDK > 0 , 1 ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *  
     *  const bool goodKaonDLL = cut  ( combination ) ;
     *
     *  @endcode
     *  @see LoKi::Cuts::PIDK
     * 
     *  @see LoKi::AParticles::ChildCut
     *  @see LoKi::Cuts::ACHILDCUT
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::ChildCut                              ACUTCHILD ;
    // ========================================================================
    /** @typedef  ACUTDOCA 
     *  Simple predicate which check if all two-particle subcombinations
     *  have a distance of the closest approach below some threshold
     *  
     *  @code
     * 
     *  IDistanceCalculator* doca = ... ;
     *  ACut cut = ACUTDOCA ( doca , 0.2 * mm ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *
     *  const bool good = cut ( combination ) ;
     *
     *  @endcode
     *  
     *  It should be more efficient than a combination of 
     *   LoKi::AParticles::MaxDOCA and comparison.
     *
     *  @see IDistanceCalculator
     *  @see LoKi::AParticles::MaxDOCACut
     *  @see LoKi::Cuts::ADOCACUT
     *  @see LoKi::AParticles::MaxDOCA
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::MaxDOCACut                             ACUTDOCA ;
    // ========================================================================
    /** @typedef  ACUTDOCACHI2
     *  Simple predicate which check if all two-particle subcombinations
     *  have a chi2-distance of the closest approach below some threshold
     *  
     *  @code
     * 
     *  IDistanceCalculator* doca = ... ;
     *  ACut cut = ACUTDOCACHI2 ( doca , 3*3 ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *
     *  const bool good = cut ( combination ) ;
     *
     *  @endcode
     *  
     *  It should be more efficient than a combination of 
     *   LoKi::AParticles::MaxDOCAChi2 and comparison.
     *
     *  @see IDistanceCalculator
     *  @see LoKi::AParticles::MaxDOCAChi2Cut
     *  @see LoKi::Cuts::ADOCACHI2CUT
     *  @see LoKi::AParticles::MaxDOCAChi
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::MaxDOCAChi2Cut                     ACUTDOCACHI2 ;
    // ========================================================================
    /** @typedef ADAMASS
     *
     *  Trivial functor to get the absolute value for the 
     *   delta mass of the combination.
     *  It uses the function LoKi::Kinematics::addMomenta
     *
     *  @code 
     *
     *    const LHCb::Particle::ConstVector& combination = ... ;
     *
     *    
     *    const AFun adamass = ADAMASS ( "J/psi(1S)" )  
     *
     *    const double absdelta = adamass ( combination ) ;
     * 
     *  @endcode 
     * 
     *  @see LoKi::AParticles::AbsDeltaMass
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09
     */
    typedef LoKi::AParticles::AbsDeltaMass                          ADAMASS ;
    // ========================================================================    
    /** @typedef ADOCACHI2 
     *  The evaluator of the maximal chi2-distance of the closest approach 
     *  for all two-particle subcombinations
     *  
     *  @code
     * 
     *  IDistanceCalculator* doca = ... ;
     *  AFun chi2MAX = ADOCACHI2 ( doca ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *
     *  const double maxChi2 = fun ( combination ) ;
     *
     *  @endcode
     *  
     *  @see IDistanceCalculator
     *  @see LoKi::AParticles::MaxDOCAChi2
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::MaxDOCAChi2                           ADOCACHI2 ;
    // ========================================================================
    /** @typedef  ADOCACHI2CUT
     *  Simple predicate which check if all two-particle subcombinations
     *  have a chi2-distance of the closest approach below some threshold
     *  
     *  @code
     * 
     *  IDistanceCalculator* doca = ... ;
     *  ACut cut = ADOCACHI2CUT ( doca , 3*3 ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *
     *  const bool good = cut ( combination ) ;
     *
     *  @endcode
     *  
     *  It should be more efficient than a combination of 
     *   LoKi::AParticles::MaxDOCAChi2 and comparison.
     *
     *  @see IDistanceCalculator
     *  @see LoKi::AParticles::MaxDOCAChi2Cut
     *  @see LoKi::Cuts::ACUTDOCACHI2
     *  @see LoKi::AParticles::MaxDOCAChi
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::MaxDOCAChi2Cut                     ADOCACHI2CUT ;
    // ========================================================================    
    /** @typedef  ADOCACUT 
     *  Simple predicate which check if all two-particle subcombinations
     *  have a distance of the closest approach below some threshold
     *  
     *  @code
     * 
     *  IDistanceCalculator* doca = ... ;
     *  ACut cut = ADOCACUT ( doca , 0.2 * mm ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *
     *  const bool good = cut ( combination ) ;
     *
     *  @endcode
     *  
     *  It should be more efficient than a combination of 
     *   LoKi::AParticles::MaxDOCA and comparison.
     *
     *  @see IDistanceCalculator
     *  @see LoKi::AParticles::MaxDOCACut
     *  @see LoKi::Cuts::ACUTDOCA
     *  @see LoKi::AParticles::MaxDOCA
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::MaxDOCACut                             ADOCACUT ;
    // ========================================================================
    /** @typedef ADOCAMAX 
     *  The evaluator of the maximal distance of the closest approach 
     *  for all two-particle subcombinations
     *  
     *  @code
     * 
     *  IDistanceCalculator* doca = ... ;
     *  AFun docaMAX = ADOCAMAX ( doca ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *
     *  const double maxDOCA = fun ( combination ) ;
     *
     *  @endcode
     *  
     *  @see IDistanceCalculator
     *  @see LoKi::AParticles::MaxDOCA
     *  @see LoKi::Cuts::AMAXDOCA
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::MaxDOCA                                ADOCAMAX ;
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
    const   LoKi::AParticles::Eta                                  AETA ( 0 ) ;
    // ========================================================================
    /** @var AETA0
     *  Simple evaluator of the pseudorapidity of the combination
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double eta = AETA0 ( comb ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::AETAP
     *  @see LoKi::AParticles::Eta
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const   LoKi::AParticles::Eta                                 AETA0 ( 0 ) ;
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
    typedef LoKi::AParticles::Eta                                       AETAP ;
    // ========================================================================
    /** @typedef AFUNCHILD
     *  The function which evaluates another function using the 
     *  defined child particle
     *  
     *  Get the DLL(K-pi) for the first particle 
     *  @code 
     * 
     *  // Get the DLL(K-pi) for the first particle 
     *  AFun fun = AFUNCHILD ( PIDK , 1 ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *  
     *  const double kaonDLL = fun ( combination ) ;
     *
     *  @endcode
     *  @see LoKi::Cuts::PIDK
     * 
     *  @see LoKi::AParticles::ChildFun
     *  @see LoKi::Cuts::ACHILD
     *  @see LoKi::Cuts::ACHILDFUN
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::ChildFun                              AFUNCHILD ;
    // ========================================================================
    /** @typedef AHASCHILD
     *  Check the existence of daughter particle, which satisfy 
     *  certain criteria:
     *  
     *  Check the existnce of "badKaon" in the combination:
     *  @code 
     * 
     *  // check the existnce of "badKaon" in the combination:
     *  ACut cut = AHASCHILD ( "K+" = ABSID && PIDK < -5 ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *  
     *  const bool hasBadKaon = cut ( combination ) ;
     *
     *  @endcode
     *  @see LoKi::Cuts::ABSID
     *  @see LoKi::Cuts::PIDK
     * 
     *  Check the presence of photons, including teh photons,
     *  used for recontruction of daughter particles:
     *  @code 
     * 
     *  // Check the presence of photons, including teh photons,
     *  // used for recontruction of daughter particles:
     *  ACut cut = AHASCHILD ( INTREE ( "gamma" == ID )  ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *  
     *  const bool hasGamma = cut ( combination ) ;
     *
     *  @endcode
     *  @see LoKi::Cuts::INTREE
     *  @see LoKi::Cuts::ID
     *  
     *  @see LoKi::AParticles::HasChild
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::HasChild                              AHASCHILD ;
    // ========================================================================
    /** @typedef ALV0 
     *
     *  For partice, which has a some daughter particles, 
     *  it evaluates the cosine of the angle between daughter's 
     *  momentum and mother flight direction in the rest 
     *  system of mother particle. For 2-body decays it is just 
     *  a polarization angle of mother particle.
     *  
     *  The name comes from KAL language by H.Algrecht 
     *   used in ARGUS collaboaration
     *
     *  @see LoKi::Cuts::ALV01 
     *  @see LoKi::Cuts::ALV02 
     *  @see LoKi::Cuts::ALV03 
     *  @see LoKi::Cuts::ALV04 
     *  @see LoKi::AParticles::DecayAngle 
     *
     *  @author Vanya Belyaev ibelyaev@physics.syr.edu
     *  @date   2002-11-02
     */
    typedef LoKi::AParticles::DecayAngle                                 ALV0 ;
    // ========================================================================
    /** @var LV01
     *
     *  For partice, which has a some daughter particles, 
     *  it evaluates the cosine of the angle between the first 
     *  daughter's momentum and mother flight direction in the rest 
     *  system of mother particle. 
     *  
     *  The name comes from KAL language by H.Algrecht 
     *   used in ARGUS collaboaration
     *
     *  @see LoKi::Cuts::ALV0 
     *  @see LoKi::AParticles::DecayAngle 
     *
     *  @author Vanya Belyaev ibelyaev@physics.syr.edu
     *  @date   2002-11-02
     */
    const LoKi::AParticles::DecayAngle                            ALV01 ( 1 ) ;
    // ========================================================================
    /** @var ALV02
     *
     *  For partice, which has a some daughter particles, 
     *  it evaluates the cosine of the angle between the second
     *  daughter's momentum and mother flight direction in the rest 
     *  system of mother particle. 
     *  
     *  The name comes from KAL language by H.Algrecht 
     *   used in ARGUS collaboaration
     *
     *  @see LoKi::Cuts::ALV0  
     *  @see LoKi::AParticles::DecayAngle 
     *
     *  @author Vanya Belyaev ibelyaev@physics.syr.edu
     *  @date   2002-11-02
     */
    const LoKi::AParticles::DecayAngle                            ALV02 ( 2 ) ;
    // ========================================================================
    /** @var ALV03
     *
     *  For partice, which has a some daughter particles, 
     *  it evaluates the cosine of the angle between the third
     *  daughter's momentum and mother flight direction in the rest 
     *  system of mother particle. 
     *  
     *  The name comes from KAL language by H.Algrecht 
     *   used in ARGUS collaboaration
     *
     *  @see LoKi::Cuts::ALV0 
     *  @see LoKi::AParticles::DecayAngle 
     * 
     *  @author Vanya Belyaev ibelyaev@physics.syr.edu
     *  @date   2002-11-02
     */
    const LoKi::AParticles::DecayAngle                            ALV03 ( 3 ) ;
    // ========================================================================
    /** @var ALV04
     *
     *  For partice, which has a some daughter particles, 
     *  it evaluates the cosine of the angle between  the fourth
     *  daughter's momentum and mother flight direction in the rest 
     *  system of mother particle. 
     *  
     *  The name comes from KAL language by H.Algrecht 
     *   used in ARGUS collaboaration
     *
     *  @see LoKi::Cuts::ALV0 
     *  @see LoKi::AParticles::DecayAngle 
     *
     *  @author Vanya Belyaev ibelyaev@physics.syr.edu
     *  @date   2002-11-02
     */
    const LoKi::AParticles::DecayAngle                            ALV04 ( 4 ) ;
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
     *  @see LoKi::AParticles::InvariantMass
     *  @see LoKi::Kinematics::addMomenta
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09
     */
    const   LoKi::AParticles::InvariantMass                          AM ( 0 ) ;
    // ========================================================================
    /** @var AM0
     *
     *  Trivial functor to get the overall mass of the combination.
     *  It uses the function LoKi::Kinematics::addMomenta
     *
     *  @code 
     *
     *    const LHCb::Particle::ConstVector& combination = ... ;
     * 
     *    const double mass = AM0  ( combination ) ;
     * 
     *  @endcode 
     * 
     *  @see LoKi::AParticles::InvariantMass
     *  @see LoKi::Kinematics::addMomenta
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09
     */
    const   LoKi::AParticles::InvariantMass                         AM0 ( 0 ) ;
    // ========================================================================
    /** @var AM1
     *  Simple function which evaluates the invariant mass of 1st particle
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double m1 = AM1( comb ) ;
     *
     *  @endcode 
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::Cuts::AM
     *  @see LoKi::AParticles::InvariantMass
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const LoKi::AParticles::InvariantMass                           AM1 ( 1 ) ;
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
    const LoKi::AParticles::InvariantMass                      AM12 ( 1 , 2 ) ;
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
    const LoKi::AParticles::InvariantMass                      AM13 ( 1 , 3 ) ;
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
    const LoKi::AParticles::InvariantMass                      AM14 ( 1 , 4 ) ;
    // ========================================================================
    /** @var AM2
     *  Simple function which evaluates the invariant mass of the second particle
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double m2 = AM2( comb ) ;
     *
     *  @endcode 
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::Cuts::AM
     *  @see LoKi::AParticles::InvariantMass
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const LoKi::AParticles::InvariantMass                           AM2 ( 2 ) ;
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
    const LoKi::AParticles::InvariantMass                      AM23 ( 2 , 3 ) ;
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
    const LoKi::AParticles::InvariantMass                      AM24 ( 2 , 4 ) ;
    // ========================================================================
    /** @var AM3
     *  Simple function which evaluates the invariant mass of the third particle
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double m3 = AM3( comb ) ;
     *
     *  @endcode 
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::Cuts::AM
     *  @see LoKi::AParticles::InvariantMass
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const LoKi::AParticles::InvariantMass                           AM3 ( 3 ) ;
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
    const LoKi::AParticles::InvariantMass                     AM34  ( 3 , 4 ) ;
    // ========================================================================
    /** @var AM4
     *  Simple function which evaluates the invariant mass of the fourth particle
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double m4 = AM4( comb ) ;
     *
     *  @endcode 
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::Cuts::AM
     *  @see LoKi::AParticles::InvariantMass
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */   
    const LoKi::AParticles::InvariantMass                           AM4 ( 4 ) ;
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
    typedef LoKi::AParticles::InvariantMass                             AMASS ;
    // ========================================================================
    /** @typedef AMAXCHILD
     *  Simple function which evaluates the maximal value of 
     *  another function over the combination components
     * 
     *  Search for the maximal transverse momentum:
     *  @code
     *  
     *  AFun maxPT = AMAXCHILD(PT) ;
     * 
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *  
     *  cound double ptMax = maxPT ( combination ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::PT
     *
     *  Search for the maximal transverse momentum of pion:
     *  @code
     *  
     *  AFun maxPT = AMAXCHILD ( PT , "pi+" == ABSID ) ;
     * 
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *  
     *  cound double ptMax = maxPT ( combination ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::PT
     *  @see LoKi::Cuts::ABSID
     *
     *  Search for the maximal transverse momentum of pion including 
     *  the pions, used for recontruction of daughter particles:
     *  @code
     *  
     *  AFun maxPT = AMAXCHILD ( MAXTREE ( PT , "pi+" == ABSID )  ) ;
     * 
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *  
     *  cound double ptMax = maxPT ( combination ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::PT
     *  @see LoKi::Cuts::ABSID
     *  @see LoKi::Cuts::MAXTREE
     *
     *  @see LoKi::AParticles::MaxChild
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::MaxChild                              AMAXCHILD ;
    // ========================================================================
    /** @typedef AMAXDOCA 
     *  The evaluator of the maximal distance of the closest approach 
     *  for all two-particle subcombinations
     *  
     *  @code
     * 
     *  IDistanceCalculator* doca = ... ;
     *  AFun docaMAX = AMAXDOCA ( doca ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *
     *  const double maxDOCA = fun ( combination ) ;
     *
     *  @endcode
     *  
     *  @see IDistanceCalculator
     *  @see LoKi::AParticles::MaxDOCA
     *  @see LoKi::Cuts::ADOCAMAX
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::MaxDOCA                                AMAXDOCA ;
    // ========================================================================
    /** @typedef AMINCHILD
     *  Simple function which evaluates the minimal value of 
     *  another function over the combination components
     * 
     *  Search for the minimum transverse momentum:
     *  @code
     *  
     *  AFun minPT = AMINCHILD(PT) ;
     * 
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *  
     *  cound double ptMin = minPT ( combination ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::PT
     *
     *  Search for the minimum transverse momentum of pion:
     *  @code
     *  
     *  AFun minPT = AMINCHILD ( PT , "pi+" == ABSID ) ;
     * 
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *  
     *  cound double ptMin = minPT ( combination ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::PT
     *  @see LoKi::Cuts::ABSID
     *
     *  Search for the minimum transverse momentum of pion including 
     *  the pions, used for recontruction of daughter particles:
     *  @code
     *  
     *  AFun minPT = AMINCHILD ( MINTREE ( PT , "pi+" == ABSID )  ) ;
     * 
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *  
     *  cound double ptMin = minPT ( combination ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::PT
     *  @see LoKi::Cuts::ABSID
     *  @see LoKi::Cuts::MINTREE
     *
     *  @see LoKi::AParticles::MinChild
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::MinChild                              AMINCHILD ;
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
    typedef LoKi::AParticles::TransverseMomentum                        AMOMT ;
    // ========================================================================
    /** @typedef ANUM
     *
     *  Simple functor to count number of particles in a combination, 
     *  which satisfy certain criteria.
     *
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
    typedef LoKi::AParticles::Count                                      ANUM ;
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
    const   LoKi::AParticles::Momentum                               AP ( 0 ) ;
    // ========================================================================
    /** @var AP0
     *  Simple evaluator of the momentum of the combination:
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double p = AP0 ( comb ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::AMOM
     *  @see LoKi::AParticles::Momentum
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const   LoKi::AParticles::Momentum                              AP0 ( 0 ) ;
    // ========================================================================
    /** @var AP1
     *  Simple evaluator of the momentum of the first daughter 
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double p = AP1 ( comb ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::AMOM
     *  @see LoKi::AParticles::Momentum
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const   LoKi::AParticles::Momentum                              AP1 ( 1 ) ;
    // ========================================================================
    /** @var AP2
     *  Simple evaluator of the momentum of the second daughter 
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double p = AP0 ( comb ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::AMOM
     *  @see LoKi::AParticles::Momentum
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const   LoKi::AParticles::Momentum                              AP2 ( 2 ) ;
    // ========================================================================
    /** @var AP3
     *  Simple evaluator of the momentum of the third daughter 
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double p = AP0 ( comb ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::AMOM
     *  @see LoKi::AParticles::Momentum
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const   LoKi::AParticles::Momentum                              AP3 ( 3 ) ;
    // ========================================================================
    /** @var AP4
     *  Simple evaluator of the momentum of the fourth daughter
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double p = AP0 ( comb ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::AMOM
     *  @see LoKi::AParticles::Momentum
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const   LoKi::AParticles::Momentum                              AP4 ( 4 ) ;
    // ========================================================================
    /** @typedef PLOT
     *  Simple monitoring histogram for the functions
     *
     *  @code 
     *
     *  // some function to be monitored
     *  AFun fun = ... ;
     *
     *  // Create the monitored function using the histogram:
     *  AIDA::IHistogram1D* histo = ... ;
     *  AFun mon = PLOT ( fun , histo ) ;
     *
     *  for ( ... ) 
     *    {
     *      ...
     *      const LHCb::Particle::ConstVector& p = ... ;
     *      const double result = mon( p ) ; ///< use the function
     *      ...
     *    }
     *
     *  @endcode
     *
     *  The alternative (recommended) way to create the monitored function
     *  @code 
     *
     *  // some function to be monitored
     *  AFun fun = ... ;
     *
     *  AIDA::IHistogram1D* histo = ... ;
     *  AFun mon = monitor ( fun , histo ) ;
     *  
     *  @endcode 
     *
     *  The substitution of the function by monitored function 
     *  cound be done "on-flight" without the disturbing of the
     *  actual processing:
     *  @code
     *
     *  // some function to be monitored
     *  AFun fun = ... ;
     *
     *  if ( monitoring ) 
     *    {
     *       AIDA::IHistogram1D* histo = ... ;
     *       fun = monitor ( fun , histo ) ;
     *    }
     *
     *  @endcode
     * 
     *  @attention The string representation of the object 
     *             is delegated to the underlying function,  
     *             therefore the object is NOT recontructable 
     *             from its string representations. It is done 
     *             on purpose to avoid the disturbing of ids. 
     * 
     *  @see LoKi::Monitoring::Plot
     *  @see LoKi::monitor
     *  @see AIDA::IHistogram1D
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03 
     */        
    typedef LoKi::Monitoring::Plot<LoKi::ATypes::Combination,double>     APLOT ;
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
    const   LoKi::AParticles::TransverseMomentum                    APT ( 0 ) ;
    // ========================================================================
    /** @var APT0
     *  Simple evaluator of the transverse momentum of the combination:
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double pt = APT0 ( comb ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::AMOMT
     *  @see LoKi::AParticles::TransverseMomentum
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const   LoKi::AParticles::TransverseMomentum                   APT0 ( 0 ) ;
    // ========================================================================
    /** @var APT1
     *  Simple evaluator of the transverse momentum of the first daughter
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double pt1 = APT1 ( comb ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::AMOMT
     *  @see LoKi::AParticles::TransverseMomentum
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const   LoKi::AParticles::TransverseMomentum                   APT1 ( 1 ) ;
    // ========================================================================
    /** @var APT2
     *  Simple evaluator of the transverse momentum of the second daughter
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double pt2 = APT2 ( comb ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::AMOMT
     *  @see LoKi::AParticles::TransverseMomentum
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const   LoKi::AParticles::TransverseMomentum                   APT2 ( 2 ) ;
    // ========================================================================
    /** @var APT3
     *  Simple evaluator of the transverse momentum of the third daughter
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double pt = APT3 ( comb ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::AMOMT
     *  @see LoKi::AParticles::TransverseMomentum
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const   LoKi::AParticles::TransverseMomentum                   APT3 ( 3 ) ;
    // ========================================================================
    /** @var APT4
     *  Simple evaluator of the transverse momentum of the fourth daughter
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     * 
     *    const double pt = APT4 ( comb ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::AMOMT
     *  @see LoKi::AParticles::TransverseMomentum
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09     
     */
    const   LoKi::AParticles::TransverseMomentum                   APT4 ( 4 ) ;
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
    const   LoKi::AParticles::Size                                      ASIZE ;
    // ========================================================================
    /** @var ASSWITCH
     *
     *  Simple functor  which acts as switch:
     * 
     *   <b> result = condition ? constant1 : constant2 </b>
     *
     *  @code 
     *  
     *    const LHCb::Particle::ConstVector& combination = ... ;
     *
     *    AFun fun  = ASWITCH ( HASCHILD ( "gamma" == ID ) , 0 , 1 ) ;   
     *
     *  @endcode 
     *
     *  @see LoKi::AParticles::Size  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09
     */
    typedef LoKi::SimpleSwitch<LoKi::ATypes::Combination>            ASSWITCH ;
    // ========================================================================
    /** @typedef ASTAT
     *  Simple monitoring counter for the functions
     *
     *  @code 
     *
     *  // some function to be monitored
     *  AFun fun = ... ;
     *
     *  // Create the monitored function using the generic counter
     *  // either local counter, or the counter from IStatSvc or ICounterSvc
     *  StatEntity* counter = ... ; ///< the generic counter
     *  AFun mon = STAT ( fun , counter ) ;
     *
     *  for ( ... ) 
     *    {
     *      ...
     *      const LHCb::Particle::ConstVector& p = ... ;
     *      const double result = mon( p ) ; ///< use the function
     *      ...
     *    }
     *
     *  ...
     *  info () 
     *     << " Monitoring results : "                 << endmsg 
     *     << " NEntries:  #" << counter->entries  ()  << endmsg 
     *     << " TotalSum:   " << counter->flag     ()  << endmsg 
     *     << " Mean+-RMS:  " << counter->flagMean () 
     *     << "+="            << counter->flagRMS  ()  << endmsg      
     *     << " Min/Max:    " << counter->flagMin  ()  
     *     << "/"             << counter->flagMax  ()  << endmsg ;
     *
     *  @endcode
     *
     *  The alternative (recommended) way to create the monitored function
     *  @code 
     *
     *  // some function to be monitored
     *  AFun fun = ... ;
     *
     *  AFun mon = monitor ( fun , counter("SomeCounter") ) ;
     *  
     *  @endcode 
     *
     *  The substitution of the function by monitored function 
     *  cound be done "on-flight" without the disturbing of the
     *  actual processing:
     *  @code
     *
     *  // some function to be monitored
     *  AFun fun = ... ;
     *
     *  if ( monitoring ) 
     *    {
     *       fun = monitor ( fun , "MonitoredFun" ) ;
     *    }
     *
     *  @endcode
     *
     *  @attention The string representation of the object 
     *             is delegated to the underlying function,  
     *             therefore the object is NOT recontructable 
     *             from its string representations. It is done 
     *             on purpose to avoid the disturbing of ids. 
     * 
     *  @see LoKi::Monitoring::Stat
     *  @see LoKi::monitor
     *  @see StatEntity
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03 
     */        
    typedef LoKi::Monitoring::Counter<LoKi::ATypes::Combination,double> ASTAT ;
    // ========================================================================
    /** @typedef ASUM
     *  Simple meta-functor to calculate the sum of some predicate over 
     *  the daughter particles 
     * 
     *  @code 
     * 
     *    // get the combination:
     *    const LHCb::Paricle::ConstVector& comb = ... ;
     *
     *    const ASUM sumPT    = ASUM  ( PT ) ;
     *
     *    const double value  = sumPT ( comb ) ;
     *
     *  @endcode 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08-05 
     */
    typedef LoKi::AParticles::Sum                                        ASUM ;
    // ========================================================================
    /** @var ASWITCH
     *
     *  Simple functor  which acts as switch:
     * 
     *   <b> result = condition ? function1 : function2 </b>
     *
     *  @code 
     *  
     *    const LHCb::Particle::ConstVector& combination = ... ;
     *
     *    AFun fun1 = ... ; // apply if there are photons
     *    AFun fun2 = ... ; // apply if no photons
     *    AFun fun  = ASWITCH ( HASCHILD ( "gamma" == ID ) , fun1 , fun2 ) ;   
     *
     *  @endcode 
     *
     *  @see LoKi::AParticles::Size  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09
     */
    typedef LoKi::Switch<LoKi::ATypes::Combination>                   ASWITCH ;
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
    /** @typedef AVCHI2
     *  Simple fuction which evaluates chi2 of the vertex fit for the 
     *  combination
     *
     *  @code
     * 
     *  IVertexFit* fitter = ... ;
     *  AFun vChi2 = AVCHI2 ( fitter ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *
     *  const double chi2 = fun ( combination ) ;
     *
     *  @endcode
     * 
     *  @see IVertexFit
     *  @see LoKi::AParticles::VertexChi2
     *  @see LoKi::Cuts::ACHI2V
     *  @see LoKi::Cuts::ACHI2VX
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    typedef LoKi::AParticles::VertexChi2                               AVCHI2 ;
    // ========================================================================
    /** @var AVALID
     *  Simpel predicate, which checks the "validity" (non-emptiness) 
     *  of the combination
     *  @see LoKi::Valid
     *  @see LoKi::valid
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-21
     */
    const   LoKi::Valid<LoKi::ATypes::Combination>                     AVALID ;
    // ========================================================================
    /** @typedef AWM 
     *  Simple evaluator of the invariant mass of combination 
     *  using some prescribed ("wrong") masses for daughter particles.
     *  It could be used e.g. for study of varisu mass-refelction
     *  of for geenric topologicla selection using various mass-windows
     *  for different mass hypothesis
     *  
     *  @code
     * 
     *  const LHCb::Particle::ConstVector& comobination = ... ;
     * 
     *  // consider the mass hypothersies of 3 kaons:
     *  AFun m3K = AWM ( "K+" , "K+" , "K-" ) ;
     *
     *  const double mass3kaon = m3k ( combination ) ;
     *
     *  // consider the another mass hypotheses:
     *  AFun m2Kpi = AWM ( "K+" , "K+" , "pi+" ) ;
     *
     *  const double mass2kaonandPion = m2Kpi ( combination ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::AWRONGMASS
     *  @see LoKi::Kinematics::wrongMass
     *  @see LoKi::Particles::massFromPID 
     *  @see LoKi::Particles::massFromName
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-26
     */
    typedef LoKi::AParticles::WrongMass                                   AWM ;
    // ========================================================================
    /** @typedef AWM 
     *  Simple evaluator of the invariant mass of combination 
     *  using some prescribed ("wrong") masses for daughter particles.
     *  It could be used e.g. for study of varisu mass-refelction
     *  of for geenric topologicla selection using various mass-windows
     *  for different mass hypothesis
     *  
     *  @code
     * 
     *  const LHCb::Particle::ConstVector& comobination = ... ;
     * 
     *  // consider the mass hypothersies of 3 kaons:
     *  AFun m3K = AWM ( "K+" , "K+" , "K-" ) ;
     *
     *  const double mass3kaon = m3k ( combination ) ;
     *
     *  // consider the another mass hypotheses:
     *  AFun m2Kpi = AWM ( "K+" , "K+" , "pi+" ) ;
     *
     *  const double mass2kaonandPion = m2Kpi ( combination ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::AWRONGMASS
     *  @see LoKi::Kinematics::wrongMass
     *  @see LoKi::Particles::massFromPID 
     *  @see LoKi::Particles::massFromName
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-26
     */
    typedef LoKi::AParticles::WrongMass                            AWRONGMASS ;
    // ========================================================================    
    /** @typedef DAMASS
     *
     *  Trivial functor to get delta mass of the combination.
     *  It uses the function LoKi::Kinematics::addMomenta
     *
     *  @code 
     *
     *    const LHCb::Particle::ConstVector& combination = ... ;
     *
     *    
     *    const AFun dmass = DAMASS ( "J/psi(1S)" )  
     *
     *    const double delta = dmass ( combination ) ;
     * 
     *  @endcode 
     * 
     *  @see LoKi::AParticles::DeltaMass
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09
     */
    typedef LoKi::AParticles::DeltaMass                          DAMASS ;
    // ========================================================================
    /** @typedef ADOCAMIN 
     *  The evaluator of the minimal distance of the closest approach 
     *  for all two-particle subcombinations
     *  
     *  @code
     * 
     *  IDistanceCalculator* doca = ... ;
     *  AFun docaMIN = ADOCAMIN ( doca ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *
     *  const double minDOCA = fun ( combination ) ;
     *
     *  @endcode
     *  
     *  @see IDistanceCalculator
     *  @see LoKi::AParticles::MinDOCA
     *  @see LoKi::Cuts::AMINDOCA
     *  @author
     *  @date
     */
    typedef LoKi::AParticles::MinDOCA                                ADOCAMIN ;
    // ========================================================================
    /** @typedef AMINDOCA 
     *  The evaluator of the minimal distance of the closest approach 
     *  for all two-particle subcombinations
     *  
     *  @code
     * 
     *  IDistanceCalculator* doca = ... ;
     *  AFun docaMIN = AMINDOCA ( doca ) ;
     *
     *  const LHCb::Particle::ConstVector& combination = ... ;
     *
     *  const double minDOCA = fun ( combination ) ;
     *
     *  @endcode
     *  
     *  @see IDistanceCalculator
     *  @see LoKi::AParticles::MinDOCA
     *  @see LoKi::Cuts::ADOCAMIN
     *  @author Patrick SPRADLIN 
     *  @date   2009-03-10
     */
    typedef LoKi::AParticles::MinDOCA                                AMINDOCA ;
    // ========================================================================
    /** @typedef AALLSAMEBPV 
     *  Predicate to determine if all daughter particles have the same
     *  related PV.
     *  
     *  @see LoKi::AParticles::AllSameBestPV
     *  @author patrick spradlin
     *  @date   10 March 2009
     */
    typedef LoKi::AParticles::AllSameBestPV                      AALLSAMEBPV ;
    // ========================================================================
    /** @typedef ACUTV 
     *  Simple adapter, that allow to use "vector-cut" (CutVal) 
     *  as "array-cut" (ACut)
     *  @see LoKi::AParticles::ACutV 
     *  @see LoKi::Types::ACut 
     *  @see LoKi::Types::ACuts 
     *  @see LoKi::Types::CutVal
     *  @see LoKi::Types::CutVals
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-16
     */
    typedef LoKi::AParticles::ACutV                                    ACUTV ;
    // ========================================================================
    /** @typedef AFUNV 
     *  Simple adapter, that allow to use "vector-fun" (FunVal) 
     *  as "array-fun" (AFun)
     *  @see LoKi::AParticles::AFunV 
     *  @see LoKi::Types::AFun
     *  @see LoKi::Types::AFunc 
     *  @see LoKi::Types::FunVal
     *  @see LoKi::Types::FunVals
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-16
     */
    typedef LoKi::AParticles::AFunV                                    AFUNV ;
    // ========================================================================
    /** @typedef AOVERLAP 
     *  simle function fo check overlaps
     *  @see LHCb::HashIDs::overlap
     *  @code
     *  ACut cut1 = AOVERLAP ( 1 , 2                       ) < 0.1 ;
     *  ACut cut2 = AOVERLAP ( 1 , 2 ,  LHCb::LHCbID::Velo ) > 0.2 ;
     *  ACut cut3 = AOVERLAP ( 1 , 2 , &LHCb::LHCbID::isTT ) < 0.5 ;
     *  @endcode 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-14
     */
    typedef LoKi::AParticles::Overlap                               AOVERLAP ;
    // ========================================================================
#ifdef __INTEL_COMPILER
#pragma warning(pop) 
#endif
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_APARTICLECUTS_H
// ============================================================================
