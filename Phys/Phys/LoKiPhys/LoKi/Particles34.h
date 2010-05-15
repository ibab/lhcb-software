// $Id: Particles34.h,v 1.1 2010-05-15 16:50:36 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES34_H 
#define LOKI_PARTICLES34_H 1
// ============================================================================
// Include files
// ============================================================================
// DaVinciInterfaces
// ============================================================================
#include "Kernel/ILifetimeFitter.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/PhysTypes.h"
// ============================================================================
/** @file
 *  Set of functions for Kazu Karvalho Akiba 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-05-15
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /** @class DaughterParticleCTau 
     *  Simple evaluator of c*tau for daughter particle 
     *  @see LoKi::Cuts::DPCTAU  
     *  @see ILifetimeFitter 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-15
     */
    class DaughterParticleCTau
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      // constructor from the index & tool  
      DaughterParticleCTau
      ( const unsigned short   index      , 
        const ILifetimeFitter* fitter = 0 ) ;
      // constructor from the index & tool  
      DaughterParticleCTau 
      ( const unsigned short                      index  , 
        const LoKi::Interface<ILifetimeFitter>&  fitter ) ;
      // constructor from the index & tool  
      DaughterParticleCTau
      ( const unsigned short index  , 
        const std::string&    fitter ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DaughterParticleCTau () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DaughterParticleCTau * clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get c*tau 
      double ctau             ( const LHCb::Particle* p ) const ;
      /// get ctau error 
      double ctauErr          ( const LHCb::Particle* p ) const ;
      /// get ctau fit chi2 
      double ctauFitChi2      ( const LHCb::Particle* p ) const ;
      /// get ctau significance 
      double ctauSignificance ( const LHCb::Particle* p ) const ;
      /// get the index 
      unsigned short index () const { return m_index ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// get results from the fit 
      StatusCode fit ( const LHCb::Particle* p           , 
                       double&               ctau        , 
                       double&               ctauerr     , 
                       double&               ctaufitchi2 ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      DaughterParticleCTau () ;
      // ======================================================================
    private:
      // ======================================================================
      /// the index for daughter particle 
      unsigned int m_index ;                 // the index for daughter particle 
      /// the actual evaluator of lifetime 
      mutable LoKi::Interface<ILifetimeFitter> m_fitter ;      // the evaluator 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DaughterParticleCTauErr
     *  Simple evaluator of error in c*tau for daughter particle 
     *  @see LoKi::Cuts::DPCTAUERR  
     *  @see ILifetimeFitter 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-15
     */
    class DaughterParticleCTauErr : public DaughterParticleCTau
    {
    public:
      // ======================================================================
      // constructor from the index & tool  
      DaughterParticleCTauErr
      ( const unsigned short   index      , 
        const ILifetimeFitter* fitter = 0 ) ;
      // constructor from the index & tool  
      DaughterParticleCTauErr 
      ( const unsigned short                      index  , 
        const LoKi::Interface<ILifetimeFitter>&  fitter ) ;
      // constructor from the index & tool  
      DaughterParticleCTauErr
      ( const unsigned short index  , 
        const std::string&    fitter ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DaughterParticleCTauErr () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DaughterParticleCTauErr* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      DaughterParticleCTauErr () ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DaughterParticleCTauFitChi2
     *  Simple evaluator of error in c*tau for daughter particle 
     *  @see LoKi::Cuts::DPCTAUFITCHI2 
     *  @see ILifetimeFitter 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-15
     */
    class DaughterParticleCTauFitChi2 : public DaughterParticleCTau
    {
    public:
      // ======================================================================
      // constructor from the index & tool  
      DaughterParticleCTauFitChi2
      ( const unsigned short   index      , 
        const ILifetimeFitter* fitter = 0 ) ;
      // constructor from the index & tool  
      DaughterParticleCTauFitChi2 
      ( const unsigned short                      index  , 
        const LoKi::Interface<ILifetimeFitter>&  fitter ) ;
      // constructor from the index & tool  
      DaughterParticleCTauFitChi2
      ( const unsigned short index  , 
        const std::string&    fitter ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DaughterParticleCTauFitChi2 () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DaughterParticleCTauFitChi2* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      DaughterParticleCTauFitChi2 () ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DaughterParticleCTauSignificance
     *  Simple evaluator of c*tau-significance for daughter particle 
     *  @see LoKi::Cuts::DPCTAUSIGNIFICANCE
     *  @see ILifetimeFitter 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-15
     */
    class DaughterParticleCTauSignificance : public DaughterParticleCTau
    {
    public:
      // ======================================================================
      // constructor from the index & tool  
      DaughterParticleCTauSignificance
      ( const unsigned short   index      , 
        const ILifetimeFitter* fitter = 0 ) ;
      // constructor from the index & tool  
      DaughterParticleCTauSignificance 
      ( const unsigned short                      index  , 
        const LoKi::Interface<ILifetimeFitter>&  fitter ) ;
      // constructor from the index & tool  
      DaughterParticleCTauSignificance
      ( const unsigned short index  , 
        const std::string&    fitter ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DaughterParticleCTauSignificance () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DaughterParticleCTauSignificance* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      DaughterParticleCTauSignificance () ;
      // ======================================================================
    } ;
    // ========================================================================
  } //                                         end of namespace LoKi::Particles 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES34_H
// ============================================================================
