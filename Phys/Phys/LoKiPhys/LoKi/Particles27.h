// $Id: Particles27.h,v 1.1 2009-04-30 12:58:29 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES27_H 
#define LOKI_PARTICLES27_H 1
// ============================================================================
// Include files
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleReFitter.h"
#include "Kernel/IMassFit.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/PhysTypes.h"
// ============================================================================
/** @file LoKi/Particles27.h
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
    /** @class ReFit_ 
     *  Helper functor to perform re-fit of the particle using 
     *  IParticleReFit tool 
     *  @see LoKi::Cuts::REFIT_
     *  @see IParticleReFitter  
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date   2009-04-30
     */
    class ReFit : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate 
    {
    public:
      // ======================================================================
      /// constructor from the tool 
      ReFit ( const IParticleReFitter*                  fitter ) ;
      /// constructor form the tool 
      ReFit ( const LoKi::Interface<IParticleReFitter>& fitter ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ReFit(){} ;
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  ReFit* clone() const { return new ReFit ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the fitter 
      const LoKi::Interface<IParticleReFitter>& fitter   () const
      { return m_fitter ; }
      /// cast to the fitter 
      operator const LoKi::Interface<IParticleReFitter>& () const 
      { return fitter() ; }
      // ======================================================================
      /// refit the particle 
      StatusCode reFit ( LHCb::Particle* p ) const ;
      // ======================================================================      
    protected:
      // ======================================================================      
      void setFitter ( const LoKi::Interface<IParticleReFitter>& fitter ) const 
      { m_fitter = fitter ; }
      // ======================================================================      
    private:
      // ======================================================================
      /// the defaut constructor is disabled 
      ReFit() ;                          // the defaut constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the refitter itself 
      mutable LoKi::Interface<IParticleReFitter> m_fitter ; // the refitter itself 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MassFit
     *  Helper functor to perform mass-constrained fit of the particle using 
     *  IMassFit tool 
     *  @see LoKi::Cuts::MFIT_
     *  @see IMassFit  
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date   2009-04-30
     */
    class MassFit : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate 
    {
    public:
      // ======================================================================
      /// constructor from the tool 
      MassFit ( const IMassFit*                  fitter ) ;
      /// constructor form the tool 
      MassFit ( const LoKi::Interface<IMassFit>& fitter ) ;
      /// constructor from the tool & mass 
      MassFit ( const IMassFit*                  fitter , 
                const double                     mass   ) ;
      /// constructor form the tool & mass 
      MassFit ( const LoKi::Interface<IMassFit>& fitter ,
                const double                     mass   ) ;
      /// constructor form the tool & mass 
      MassFit ( const double                     mass   , 
                const IMassFit*                  fitter ) ;
      /// constructor form the tool & mass 
      MassFit ( const double                     mass   , 
                const LoKi::Interface<IMassFit>& fitter ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MassFit(){} ;
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  MassFit* clone() const { return new MassFit ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the fitter 
      const LoKi::Interface<IMassFit>& fitter   () const { return m_fitter ; }
      /// cast to the fitter 
      operator const LoKi::Interface<IMassFit>& () const { return fitter() ; }
      // ======================================================================
      /// fit the particle 
      StatusCode fit ( LHCb::Particle* p                     ) const ;
      /// fit the particle 
      StatusCode fit ( LHCb::Particle* p , const double mass ) const ;
      // ======================================================================      
      // get the mass & flag 
      double mass () const { return m_mass ; }
      bool   flag () const { return m_flag ; }
      // ======================================================================      
    protected:
      // ======================================================================
      void setMass ( const double mass ) 
      {
        m_mass = mass ;
        m_flag = true ;
      }
      // ======================================================================      
    protected:
      // ======================================================================      
      void setFitter ( const LoKi::Interface<IMassFit>& fitter ) const 
      { m_fitter = fitter ; }
      // ======================================================================      
    private:
      // ======================================================================
      /// the defaut constructor is disabled 
      MassFit() ;                         // the defaut constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the refitter itself 
      mutable LoKi::Interface<IMassFit> m_fitter ;       // the refitter itself 
      /// the mass 
      double  m_mass ;
      /// flag 
      bool    m_flag ;
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace Particles 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef REFIT_
     *  Simple predicate to perform the re=-fit of particle candidate 
     * 
     *  @code 
     *  
     *   const IParticleReFitter* fitter = ... ;
     *   const REFIT_ fit = REFIT_ ( fitter ) ;
     *  
     *   const LHCb::Particle* B = ... ;
     * 
     *   const bool ok = fit ( B ) ;
     *   if ( !ok ) {  ... error here ... }
     *
     *  @endcode 
     *  @see IParticleReFitter 
     *  @see LoKi::Particle::ReFit 
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date   2009-04-30
     */ 
    typedef LoKi::Particles::ReFit                                     REFIT_ ;
    // ========================================================================
    /** @typedef MFIT_
     *  Simple predicate to perform mass-constained fit fo particle candidate 
     * 
     
     *  @code 
     *  
     *   const IParticleReFitter* fitter = ... ;
     * 
     *   // fit to the nominal mass: 
     *   const REFIT_ nfit = REFIT_ ( fitter ) ;
     *
     *   // fit to som othe rmass  
     *   const REFIT_ ofit = REFIT_ ( fitter , 5.4 * Gaudi::Units::GeV ) ;
     *  
     * 
     *   const LHCb::Particle* B1 = ... ;
     *   const bool ok1 = nfit ( B1 ) ;
     *   if ( !ok1 ) {  ... error here ... }
     *
     *   const LHCb::Particle* B2 = ... ;
     *   const bool ok2 = ofit ( B2 ) ;
     *   if ( !ok2 ) {  ... error here ... }
     *
     *  @endcode 
     *  @see IMassFit
     *  @see LoKi::Particle::MassFit 
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date   2009-04-30
     */ 
    typedef LoKi::Particles::MassFit                                    MFIT_ ;
    // ========================================================================
  } // end of namespace LoKi::Cuts 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES27_H
// ============================================================================
