// $Id: Particles28.h,v 1.1 2009-04-30 12:58:29 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES28_H 
#define LOKI_PARTICLES28_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Particles27.h"
// ============================================================================
/** @file LoKi/Particles28.h
 *  The file with functors for various "refit"
 *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
 *  @date   2009-04-30
 */
namespace LoKi 
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /** @class ReFitter 
     *  Helper functor to perform re-fit of the particle using 
     *  IParticleReFit tool 
     *  @see LoKi::Cuts::REFIT_
     *  @see LoKi::Cuts::REFIT
     *  @see LoKi::Particles::ReFit 
     *  @see IParticleReFitter  
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date   2009-04-30
     */
    class ReFitter : public LoKi::Particles::ReFit 
    {
    public:
      // ======================================================================
      /// constructor from the tool name 
      ReFitter ( const std::string& name ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ReFitter(){} ;
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  ReFitter* clone() const { return new ReFitter ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the defautl constructor is disabled 
      ReFitter() ;                       // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the refitter name 
      std::string m_name ;                                 // the refitter name 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MassFitter 
     *  Helper functor to perform mass-constrained fit of the particle using 
     *  IMassFit tool 
     *  @see LoKi::Cuts::MFIT_
     *  @see LoKi::Cuts::MFIT
     *  @see LoKi::Particles::MassFit 
     *  @see IMassFit  
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date   2009-04-30
     */
    class MassFitter : public LoKi::Particles::MassFit
    {
    public:
      // ======================================================================
      /// constructor from the tool name 
      MassFitter ( const std::string& name = "" ) ;
      /// constructor from the tool name 
      MassFitter ( const double       mass      ,  
                   const std::string& name = "" ) ;
      /// constructor from the tool name 
      MassFitter ( const std::string& name      , 
                   const double       mass      ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MassFitter(){} ;
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  MassFitter* clone() const { return new MassFitter ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the toolname 
      std::string m_name ; // the toolname 
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace Particles 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef REFIT
     *  Simple predicate to perform the re=-fit of particle candidate 
     * 
     *  @code 
     *  
     *   const REFIT fit = REFIT ( fitter ) ;
     *  
     *   const LHCb::Particle* B = ... ;
     * 
     *   const bool ok = fit ( B ) ;
     *   if ( !ok ) {  ... error here ... }
     *
     *  @endcode 
     *  @see IParticleReFitter 
     *  @see LoKi::Particle::ReFitter 
     *  @see LoKi::Particle::ReFit 
     *  @see LoKi::Cuts::REFIT_
     *  @see LoKi::Cuts::REFITTER
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date   2009-04-30
     */ 
    typedef LoKi::Particles::ReFitter                                   REFIT ;
    // ========================================================================
    /** @typedef REFITTER
     *  Simple predicate to perform the re=-fit of particle candidate 
     * 
     *  @code 
     *  
     *   const REFITER fit = REFITTER ( fitter ) ;
     *  
     *   const LHCb::Particle* B = ... ;
     * 
     *   const bool ok = fit ( B ) ;
     *   if ( !ok ) {  ... error here ... }
     *
     *  @endcode 
     *  @see IParticleReFitter 
     *  @see LoKi::Particle::ReFitter 
     *  @see LoKi::Particle::ReFit 
     *  @see LoKi::Cuts::REFIT_
     *  @see LoKi::Cuts::REFIT
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date   2009-04-30
     */ 
    typedef LoKi::Particles::ReFitter                                REFITTER ;
    // ========================================================================
    /** @var MFIT
     *  Simple predicate to perform mass-constained fit fo particle candidate 
     * 
     *  @code 
     *  
     *   const LHCb::Particle* B1 = ... ;
     *   // fir to the nominal mass 
     *   const bool ok1 = MFIT ( B1 ) ; 
     *   if ( !ok1 ) {  ... error here ... }
     *
     *  @endcode 
     *  @see IMassFit
     *  @see LoKi::Particle::MassFit 
     *  @see LoKi::Particle::MFIT_
     *  @see LoKi::Particle::MassFit 
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date   2009-04-30
     */ 
    const LoKi::Particles::MassFitter                                    MFIT ;
    // ========================================================================
    /** @typedef MFITTER
     *  Simple predicate to perform mass-constained fit fo particle candidate 
     * 
     *  @code 
     *  
     *   const LHCb::Particle* B1 = ... ;
     * 
     *   const MFITTER mfit = MFITTER ( 5.40 * GeV ) ;
     * 
     *   const bool ok = mfit ( B ) 
     *   if ( !ok ) {  ... error here ... }
     *
     *  @endcode 
     *  @see IMassFit
     *  @see LoKi::Particle::MassFit 
     *  @see LoKi::Particle::MFIT_
     *  @see LoKi::Particle::MFIT
     *  @see LoKi::Particle::MASSFIT
     *  @see LoKi::Particle::MassFit 
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date   2009-04-30
     */ 
    typedef LoKi::Particles::MassFitter                               MFITTER ;
    // ========================================================================
    /** @typedef MASSFIT
     *  Simple predicate to perform mass-constained fit fo particle candidate 
     * 
     *  @code 
     *  
     *   const LHCb::Particle* B1 = ... ;
     * 
     *   const MASSFIT mfit = MASSFIT( 5.40 * GeV ) ;
     * 
     *   const bool ok = mfit ( B ) 
     *   if ( !ok ) {  ... error here ... }
     *
     *  @endcode 
     *  @see IMassFit
     *  @see LoKi::Particle::MassFit 
     *  @see LoKi::Particle::MFIT
     *  @see LoKi::Particle::MFIT_
     *  @see LoKi::Particle::MASSFIT
     *  @see LoKi::Particle::MassFit 
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date   2009-04-30
     */ 
    typedef LoKi::Particles::MassFitter                               MASSFIT ;
    // ========================================================================
  } // end of namespace LoKi::Cuts 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES28_H
// ============================================================================
