// $Id$
// ============================================================================
#ifndef LOKI_PARTICLES41_H 
#define LOKI_PARTICLES41_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
// ============================================================================ 
/** @file LoKi/Particles41.h
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-03-31
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
namespace LoKi
{
  // ==========================================================================
  namespace Particles
  {
    // ========================================================================
    /** @class MinMaxPair 
     *  get the min(max) value of 2-argument function for subset of daughters 
     *  @see LoKi::Cuts::MINMAXPAIR 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-03-31
     */
    class MinMaxPair
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    protected:
      // ======================================================================
      /// the actual type of "distance"-function
      typedef double (*dist_func) ( const LHCb::Particle* , 
                                    const LHCb::Particle* ) ;
      // ======================================================================
    public:
      // ======================================================================
      /** constructor 
       *  @param minmax  min/max flag
       *  @param cut     the selection of daughters 
       *  @param fun     the function 
       */
      MinMaxPair 
      ( const bool minmax                                                , 
        const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut ,
        dist_func                                                    fun ) ;
      /// MANDATORY: destructor 
      virtual ~MinMaxPair() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinMaxPair* clone() const ;
      /// MANDATORY: the only essential method
      virtual result_type operator() ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MinMaxPair () ;                // the default constructor is disabled 
      // ======================================================================
    protected:
      // ======================================================================
      double distance 
      ( const LHCb::Particle* p1 , 
        const LHCb::Particle* p2 ) const ; 
      // ======================================================================
      /// the actual computation
      double distance ( const LHCb::Particle* p ) const ;
      bool   minimum  () const { return m_minimum ;}
      // ======================================================================
      const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut() const 
      { return m_cut.func () ; }
      // ======================================================================
    private:
      // ======================================================================
      /// minmax flag 
      bool      m_minimum  ;                           // minmax flag 
      /// the selection of daughters 
      LoKi::Assignable<LoKi::BasicFunctors<const LHCb::Particle*>::Predicate>::Type m_cut ;
      /// the distance function 
      dist_func m_distance ;                           // the distance function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinKullback
     *  @see LoKi::PhysKinematics::kullback
     *  @see LoKi::Cuts::MINKULLBACK
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-03-31
     */
    class MinKullback : public LoKi::Particles::MinMaxPair 
    {
    public:
      // ======================================================================
      /// constructor from daughter selection 
      MinKullback 
      ( const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut ) ;
      /// MANDATORY: destructor 
      virtual ~MinKullback() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinKullback* clone() const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      MinKullback () ;                       // default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
    /** @class MinAng
     *  @see LoKi::PhysKinematics::kullback
     *  @see LoKi::Cuts::MINANG
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-03-31
     */
    class MinAng : public LoKi::Particles::MinMaxPair 
    {
    public:
      // ======================================================================
      /// constructor from daughter selection 
      MinAng 
      ( const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut ) ;
      /// MANDATORY: destructor 
      virtual ~MinAng() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinAng* clone() const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      MinAng () ;                            // default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
    /** @class MinDeltaM2
     *  @see LoKi::PhysKinematics::deltaM2 
     *  @see LoKi::Cuts::MINDELTAM2
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-03-31
     */
    class MinDeltaM2 : public LoKi::Particles::MinMaxPair 
    {
    public:
      // ======================================================================
      /// constructor from daughter selection 
      MinDeltaM2 
      ( const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut ) ;
      /// MANDATORY: destructor 
      virtual ~MinDeltaM2 () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinDeltaM2* clone() const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      MinDeltaM2 () ;                        // default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
    /** @class MaxOverlap
     *  @see LoKi::PhysKinematics::overlap
     *  @see LoKi::Cuts::MAXOVERLAP
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-03-31
     */
    class MaxOverlap : public LoKi::Particles::MinMaxPair 
    {
    public:
      // ======================================================================
      /// constructor from daughter selection 
      MaxOverlap 
      ( const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut ) ;
      /// MANDATORY: destructor 
      virtual ~MaxOverlap () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MaxOverlap* clone() const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      MaxOverlap () ;                        // default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
  } //                                         end of namespace LoKi::Particles 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef MINMAXPAIR 
     *  functor to get the min/max values for pairs of good particles,
     *  e.g. it can be used to suppress certain reflections, clones, etc..
     *
     *  @code
     * 
     *   const MINMAXPAIR kl = MINPAXPAIR ( true                            , 
     *                                      BASIC & HASTRACK                , 
     *                                      &LoKi::PhysKinematics::kullback ) ;
     *  
     *   const LHCb::Particle* B = ... ;
     *  
     *   const bool OK = kl ( B ) > xxx ;
     *
     *  @endcode 
     *
     *  Probably a bit more useful to use it separately for 
     *   different charges:
     *  
     *  @code
     * 
     *   const MINMAXPAIR kl1 = MINPAXPAIR ( true                           , 
     *                                      ( Q > 0 ) & BASIC & HASTRACK    , 
     *                                      &LoKi::PhysKinematics::kullback ) ;
     *  
     *   const MINMAXPAIR kl2 = MINPAXPAIR ( true                           , 
     *                                      ( Q < 0 ) & BASIC & HASTRACK    , 
     *                                      &LoKi::PhysKinematics::kullback ) ;
     *  
     *   const Cut cut = ( kl1 > xxx )  & ( kl2 > xxx ) ;
     *
     *   const LHCb::Particle* B = ... ;
     *  
     *   const bool OK = cut ( B ) ;
     *
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::MinMaxPair 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-03-31
     */
    typedef LoKi::Particles::MinMaxPair                            MINMAXPAIR ;
    // ========================================================================    
    /** @typedef MINKULLBACK 
     *  @code
     * 
     *   const MINKULKBACK kl = MINKULLBACK ( BASIC & HASTRACK ) ; 
     *  
     *   const LHCb::Particle* B = ... ;
     *  
     *   const bool OK = kl ( B ) > xxx ;
     *
     *  @endcode 
     *
     *  Probably a bit more useful to use it separately for different charges:
     *  
     *  @code
     * 
     *   const MINKULLBACK kl1 = MINKULLBACK ( Q > 0 ) & BASIC & HASTRACK ) ;
     *   const MINKULLBACK kl2 = MINKULLBACK ( Q < 0 ) & BASIC & HASTRACK ) ;
     *
     *   const Cut cut = ( kl1 > xxx )  & ( kl2 > xxx ) ;
     *
     *   const LHCb::Particle* B = ... ;
     *  
     *   const bool OK = cut ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::PhysKinematics::kullback
     *  @see LoKi::Particles::MinKullback
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-03-31
     */
    typedef LoKi::Particles::MinKullback                          MINKULLBACK ;
    // ========================================================================
    /** @typedef MINANG
     *  @code
     * 
     *   const MINANG kl = MINANG ( BASIC & HASTRACK ) ; 
     *  
     *   const LHCb::Particle* B = ... ;
     *  
     *   const bool OK = kl ( B ) > xxx ;
     *
     *  @endcode 
     *
     *  Probably a bit more useful to use it separately for different charges:
     *  
     *  @code
     * 
     *   const MINANG kl1 = MINANG ( Q > 0 ) & BASIC & HASTRACK ) ;
     *   const MINANG kl2 = MINANG ( Q < 0 ) & BASIC & HASTRACK ) ;
     *
     *   const Cut cut = ( kl1 > xxx )  & ( kl2 > xxx ) ;
     *
     *   const LHCb::Particle* B = ... ;
     *  
     *   const bool OK = cut ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::PhysKinematics::deltaAlpha
     *  @see LoKi::Particles::MinAng
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-03-31
     */
    typedef LoKi::Particles::MinAng                                    MINANG ;
    // ========================================================================
    /** @typedef MINDELTAM2
     *  @code
     * 
     *   const MINDELTAM2 kl = MINDELTAM2 ( BASIC & HASTRACK ) ; 
     *  
     *   const LHCb::Particle* B = ... ;
     *  
     *   const bool OK = kl ( B ) > xxx ;
     *
     *  @endcode 
     *
     *  Probably a bit more useful to use it separately for different charges:
     *  
     *  @code
     * 
     *   const MINDELTAM2 kl1 = MINDELTAM2 ( Q > 0 ) & BASIC & HASTRACK ) ;
     *   const MINDELTAM2 kl2 = MINDELTAM2 ( Q < 0 ) & BASIC & HASTRACK ) ;
     *
     *   const Cut cut = ( kl1 > xxx )  & ( kl2 > xxx ) ;
     *
     *   const LHCb::Particle* B = ... ;
     *  
     *   const bool OK = cut ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::PhysKinematics::deltaM2
     *  @see LoKi::Particles::MinDeltaM2
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-03-31
     */
    typedef LoKi::Particles::MinDeltaM2                            MINDELTAM2 ;
    // ========================================================================
    /** @typedef MAXOVERLAP
     *  @code
     * 
     *   const MAXOVERLAP kl = MAXOVERLAP ( BASIC & HASTRACK ) ; 
     *  
     *   const LHCb::Particle* B = ... ;
     *  
     *   const bool OK = kl ( B ) < xxx ;
     *
     *  @endcode 
     *
     *  Probably a bit more useful to use it separately for different charges:
     *  
     *  @code
     * 
     *   const MAXOVERLAP kl1 = MAXOVERLAP ( Q > 0 ) & BASIC & HASTRACK ) ;
     *   const MAXOVERLAP kl2 = MAXOVERLAP ( Q < 0 ) & BASIC & HASTRACK ) ;
     *
     *   const Cut cut = ( kl1 < xxx )  & ( kl2 < xxx ) ;
     *
     *   const LHCb::Particle* B = ... ;
     *  
     *   const bool OK = cut ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::PhysKinematics::overlap
     *  @see LoKi::Particles::MaxOverlap
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-03-31
     */
    typedef LoKi::Particles::MaxOverlap                            MAXOVERLAP ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLES41_H
// ============================================================================
