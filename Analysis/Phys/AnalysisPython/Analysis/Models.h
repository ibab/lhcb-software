// $Id$
// ============================================================================
#ifndef ANALYSIS_MODELS_H 
#define ANALYSIS_MODELS_H 1
// ============================================================================
// Include files
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Functions.h"
#include "LHCbMath/BSpline.h"
// ============================================================================
// ROOT 
// ============================================================================
using std::size_t ;
// ============================================================================
// #include "RooArgList.h"
// #include "RooArgSet.h"
#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooListProxy.h"
#include "RooAbsReal.h"
// ============================================================================
namespace Analysis
{
  // ==========================================================================
  /** @namespace  Models Models.h Analysis/Models.h
   *  
   *  Naturally "wide" models:
   *
   *  - BreitWigner, Rho0, Kstar, Phi, ...
   *  - BreitWigner from 3-body decay of mother particle: BW23L 
   *  - LASS (kappa pole) 
   *  - LASS from 3-body decay of mother particle: LASS23L 
   *  - Bugg (sigma pole) 
   *  - Bugg from 3-body decay of mother particle: Bugg23L 
   *  - Voigt 
   * 
   *  Empirical resolution models:
   *
   *  - Crystal Ball
   *  - right side Crystal Ball
   *  - double-sided Crystal Ball 
   *  - Needham: Crystal Ball with \f$\alpha(\sigma)\f$
   *  - Apolonios 
   *  - Apolonios2 (bifurcated apolonious)  
   *  - Bifurcated Gauissian 
   *  - Generalized Gaussian v1 
   *  - Generalized Gaussian v2 
   *  - Skew Gaussian
   *  - Bukin 
   *  - Student-T 
   *  - bifurcated Student-T 
   *  - Gram-Charlier-A 
   *
   *  Smooth phase-space induced models for background 
   *
   *  - 2-body phase space
   *  - L-body phase space at low-edge 
   *  - L-body phase space in N-body decays at high-edge 
   *  - L-body phase space from N-body decay 
   *  - L-body phase space from N-body decay times the positive polynomial
   *  - 2-body phase space from 3-body decays taking into accout orbital momenta 
   *
   *  Various smooth empirical models for background
   *
   *  - positive polynomial 
   *  - exponential times positive polynomial 
   *  - gamma distribution
   *  - generalized gamma distribution
   *  - Amoroso function
   *  - log  (Gamma-distribution)
   *  - log10(Gamma-distribution)
   *  - Log-Gamma distribution 
   *  - Beta-prime distribution
   * 
   *  Non-factorazeable smooth 2D-models 
   *
   *  - generic   positive non-factorizable polynomial in 2D    
   *   \f$ P^+(x,y) = \sum_i \sum_j \alpha^2_{i,j} B^n_i(x) B^k_j(y) \f$ 
   *  - symmetric positive non-factorizable polynomial in 2D \f$ P^+_{sym}(x,y) \f$ 
   *  - \f$ f(x,y)       = \Phi_1(x)\times\Phi_2(y)\timesP^+(x,y)       \f$
   *  - \f$ f_{sym}(x,y) = \Phi  (x)\times\Phi  (y)\timesP^+_{sym}(x,y) \f$
   *  - \f$ f(x,y)       = exp   (x)\times\Phi  (y)\timesP^+(x,y)       \f$
   *  - \f$ f(x,y)       = exp   (x)\times exp  (y)\timesP^+(x,y)       \f$
   *  - \f$ f_{sym}(x,y) = exp   (x)\times exp  (y)\timesP^+_{sym}(x,y) \f$
   *
   *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
   *  @date   2011-11-30
   */
  namespace Models 
  {
    // ========================================================================
    // Naturally "wide" models 
    // ========================================================================

    // ========================================================================
    /** @class BreitWigner 
     *
     *  J.D.Jackson, 
     *  "Remarks on the Phenomenological Analysis of Resonances",
     *  In Nuovo Cimento, Vol. XXXIV, N.6
     *
     *  http://www.springerlink.com/content/q773737260425652/
     *
     *  @see Gaudi::Math::BreitWigner
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2011-11-30
     */
    class GAUDI_API BreitWigner : public RooAbsPdf 
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::BreitWigner, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      BreitWigner ( const char*          name      , 
                    const char*          title     ,
                    RooAbsReal&          x         ,
                    RooAbsReal&          mass      ,
                    RooAbsReal&          width     ,
                    const double         m1        , 
                    const double         m2        ,
                    const unsigned short L     = 0 ) ;
      /// constructor from all parameters 
      BreitWigner ( const char*          name      , 
                    const char*          title     ,
                    RooAbsReal&          x         ,
                    RooAbsReal&          mass      ,
                    RooAbsReal&          width     ,
                    const double         m1        , 
                    const double         m2        ,
                    const unsigned short L                         ,
                    const Gaudi::Math::FormFactors::JacksonRho rho ) ;
      /// constructor from main parameters and "shape"
      BreitWigner ( const char*          name      , 
                    const char*          title     , 
                    RooAbsReal&          x         ,
                    RooAbsReal&          mass      ,
                    RooAbsReal&          width     ,
                    const Gaudi::Math::BreitWigner& bw ) ;
      /// "copy" constructor 
      BreitWigner ( const BreitWigner& , const char* name = 0 ) ;
      /// virtual destructor 
      virtual ~BreitWigner() ;
      /// clone 
      virtual BreitWigner* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
      ( RooArgSet&     allVars      , 
        RooArgSet&     analVars     ,
        const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
      ( Int_t          code         ,  
        const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// get the amplitude 
      std::complex<double>            amplitude () const  ;
      /// access to underlying function 
      const Gaudi::Math::BreitWigner& function  () const { return m_bw ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x     ;
      RooRealProxy m_mass  ;
      RooRealProxy m_width ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::BreitWigner m_bw ;            // the actual function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Rho0
     *
     *  J.D.Jackson, 
     *  "Remarks on the Phenomenological Analysis of Resonances",
     *  In Nuovo Cimento, Vol. XXXIV, N.6
     *
     *  http://www.springerlink.com/content/q773737260425652/
     *
     *  @see Analysis::Models::BreitWigner
     *  @see Gaudi::Math::BreitWigner
     *  @see Gaudi::Math::Rho0
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2011-11-30
     */
    class GAUDI_API Rho0 : public Analysis::Models::BreitWigner
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::Rho0, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      Rho0 ( const char*          name      , 
             const char*          title     ,
             RooAbsReal&          x         ,
             RooAbsReal&          mass      ,
             RooAbsReal&          width     ,
             const double         pi_mass   ) ;
      /// "copy" constructor 
      Rho0 ( const Rho0& , const char* name = 0 ) ;
      /// virtual destructor 
      virtual ~Rho0 () ;
      /// clone 
      virtual Rho0* clone ( const char* name ) const ; 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Kstar
     *
     *  J.D.Jackson, 
     *  "Remarks on the Phenomenological Analysis of Resonances",
     *  In Nuovo Cimento, Vol. XXXIV, N.6
     *
     *  http://www.springerlink.com/content/q773737260425652/
     *
     *  @see Analysis::Models::BreitWigner
     *  @see Gaudi::Math::BreitWigner
     *  @see Gaudi::Math::Kstar
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2011-11-30
     */
    class GAUDI_API Kstar : public Analysis::Models::BreitWigner
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::Kstar, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      Kstar ( const char*          name      , 
              const char*          title     ,
              RooAbsReal&          x         ,
              RooAbsReal&          mass      ,
              RooAbsReal&          width     ,
              const double         k_mass    , 
              const double         pi_mass   ) ;
      /// "copy" constructor 
      Kstar ( const Kstar& , const char* name = 0 ) ;
      /// virtual destructor 
      virtual ~Kstar () ;
      /// clone 
      virtual  Kstar* clone ( const char* name ) const ; 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Phi
     *
     *  J.D.Jackson, 
     *  "Remarks on the Phenomenological Analysis of Resonances",
     *  In Nuovo Cimento, Vol. XXXIV, N.6
     *
     *  http://www.springerlink.com/content/q773737260425652/
     *
     *  @see Analysis::Models::BreitWigner
     *  @see Gaudi::Math::BreitWigner
     *  @see Gaudi::Math::Rho0
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2011-11-30
     */
    class GAUDI_API Phi : public Analysis::Models::BreitWigner
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::Phi, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      Phi  ( const char*          name      , 
             const char*          title     ,
             RooAbsReal&          x         ,
             RooAbsReal&          mass      ,
             RooAbsReal&          width     ,
             const double         k_mass    ) ;
      /// "copy" constructor 
      Phi ( const Phi& , const char* name = 0 ) ;
      /// virtual destructor 
      virtual ~Phi () ;
      /// clone 
      virtual  Phi* clone ( const char* name ) const ; 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class BW23L
     *
     *  J.D.Jackson, 
     *  "Remarks on the Phenomenological Analysis of Resonances",
     *  In Nuovo Cimento, Vol. XXXIV, N.6
     *
     *  http://www.springerlink.com/content/q773737260425652/
     *
     *  @see Gaudi::Math::BreitWigner
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2011-11-30
     */
    class GAUDI_API BW23L : public RooAbsPdf 
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::BW23L, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      BW23L ( const char*          name      , 
              const char*          title     ,
              RooAbsReal&          x         ,
              RooAbsReal&          mass      ,
              RooAbsReal&          width     ,
              const double         m1        , 
              const double         m2        ,
              const unsigned short l         , 
              //
              const double         m3        , 
              const double         m         , 
              const double         L         ) ;      
      /// constructor from all parameters 
      BW23L ( const char*          name      , 
              const char*          title     ,
              RooAbsReal&          x         ,
              RooAbsReal&          mass      ,
              RooAbsReal&          width     ,
              const double         m1        , 
              const double         m2        ,
              const unsigned short l                         ,
              const Gaudi::Math::FormFactors::JacksonRho rho , 
              //
              const double         m3        , 
              const double         m         , 
              const double         L         ) ;
      /// constructor from main parameters and "shape"
      BW23L ( const char*          name      , 
              const char*          title     , 
              RooAbsReal&          x         ,
              RooAbsReal&          mass      ,
              RooAbsReal&          width     ,
              const Gaudi::Math::BW23L& bw   ) ; // shape 
      /// "copy" constructor 
      BW23L ( const BW23L& , const char* name = 0 ) ;
      /// virtual destructor 
      virtual ~BW23L() ;
      /// clone 
      virtual BW23L* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t     evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
      ( RooArgSet&     allVars      , 
        RooArgSet&     analVars     ,
        const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
      ( Int_t          code         ,  
        const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the amplitude 
      std::complex<double>      amplitude () const  ;
      /// access to underlying function 
      const Gaudi::Math::BW23L& function  () const { return m_bw ; }
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x     ;
      RooRealProxy m_mass  ;
      RooRealProxy m_width ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::BW23L m_bw ;            // the actual function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Flatte
     *
     *  S.M.Flatte, 
     *  "Coupled-channel analysis of the \f$\pi\eta\f$ 
     *  and \f$K\bar{K}\f$ systems near \f$K\bar{K}\f$ threshold  
     *  Phys. Lett. B63, 224 (1976 
     *
     *  http://www.sciencedirect.com/science/article/pii/0370269376906547
     *
     *  \f$\pi\pi\f$-channel
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2011-11-30
     */
    class GAUDI_API Flatte : public RooAbsPdf
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::Flatte, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      Flatte ( const char*                name      , 
               const char*                title     ,
               RooAbsReal&                x         ,
               RooAbsReal&                m0        ,
               RooAbsReal&                m0g1      ,
               RooAbsReal&                g2og1     ,
               const Gaudi::Math::Flatte& flatte    ) ;
      /// "copy" constructor 
      Flatte ( const Flatte& , const char* name = 0 ) ;
      /// virtual destructor 
      virtual ~Flatte () ;
      /// clone 
      virtual Flatte* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
      ( RooArgSet&     allVars      , 
        RooArgSet&     analVars     ,
        const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
      ( Int_t          code         ,  
        const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the amplitude 
      virtual std::complex<double> amplitude () const  ;
      /// access to underlying function 
      const Gaudi::Math::Flatte&   function  () const { return m_flatte ; }
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x     ;
      RooRealProxy m_m0    ;
      RooRealProxy m_m0g1  ;
      RooRealProxy m_g2og1 ;
      // ======================================================================
    protected:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Flatte m_flatte ;             // the actual function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Flatte2
     *
     *  S.M.Flatte, 
     *  "Coupled-channel analysis of the \f$\pi\eta\f$ 
     *  and \f$K\bar{K}\f$ systems near \f$K\bar{K}\f$ threshold  
     *  Phys. Lett. B63, 224 (1976 
     *
     *  http://www.sciencedirect.com/science/article/pii/0370269376906547
     *
     *  \f$\pi\pi\f$-channel
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2011-11-30
     */
    class GAUDI_API Flatte2 : public RooAbsPdf
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::Flatte2, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      Flatte2 ( const char*                name      , 
                const char*                title     ,
                RooAbsReal&                x         ,
                RooAbsReal&                m0        ,
                RooAbsReal&                m0g1      ,
                RooAbsReal&                g2og1     ,
                const Gaudi::Math::Flatte& flatte    ) ;
      /// "copy" constructor 
      Flatte2 ( const Flatte2& , const char* name = 0 ) ;
      /// virtual destructor 
      virtual ~Flatte2 () ;
      /// clone 
      virtual Flatte2* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the amplitude 
      std::complex<double>          amplitude () const  ;
      /// access to underlying function 
      const Gaudi::Math::Flatte2&   function  () const { return m_flatte2 ; }
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x     ;
      RooRealProxy m_m0    ;
      RooRealProxy m_m0g1  ;
      RooRealProxy m_g2og1 ;
      // ======================================================================
    protected:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Flatte2 m_flatte2 ;          // the actual function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class LASS
     *  S-wave Kpi amplitude for S-wave Kpi distribtion
     *  @see Gaudi::Math::LASS
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2013-10-05
     */
    class GAUDI_API LASS : public RooAbsPdf 
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::LASS, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      LASS  ( const char*          name          , 
              const char*          title         ,
              RooAbsReal&          x             ,
              RooAbsReal&          m1430         , // mass  of K*(1430) 
              RooAbsReal&          g1430         , // width of K*(1430) 
              RooAbsReal&          a             , 
              RooAbsReal&          r             , 
              RooAbsReal&          e             ,                
              const double         m1    = 493.7 ,   // mass of K 
              const double         m2    = 139.6 ) ; // mass of pi 
      /// "copy constructor"
      LASS  ( const LASS& right , const char* name = 0 )  ;
      /// destructor 
      virtual ~LASS () ;
      /// clone 
      virtual  LASS * clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
      ( RooArgSet&     allVars      , 
        RooArgSet&     analVars     ,
        const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
      ( Int_t          code         ,  
        const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// get the amplitude 
      std::complex<double>     amplitude () const ;
      /// access to underlying function 
      const Gaudi::Math::LASS& function  () const { return m_lass ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// the mass 
      RooRealProxy m_x     ;
      /// K*(1430) parameters
      RooRealProxy m_m0    ;
      RooRealProxy m_g0    ;
      /// LASS parameters 
      RooRealProxy m_a     ;
      RooRealProxy m_r     ;
      RooRealProxy m_e     ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::LASS m_lass ;              // the actual function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class LASS23L
     *  S-wave Kpi amplitude for Kpi from B-> Kpi X decays
     *  @see Gaudi::Math::LASS23L
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-04-02
     */
    class GAUDI_API LASS23L : public RooAbsPdf 
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::LASS23L, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      LASS23L ( const char*          name          , 
                const char*          title         ,
                RooAbsReal&          x             ,
                RooAbsReal&          m1430         ,
                RooAbsReal&          g1430         ,
                RooAbsReal&          a             , 
                RooAbsReal&          r             , 
                RooAbsReal&          e             ,                
                const double         m1    = 493.7 , 
                const double         m2    = 139.6 ,
                const double         m3    = 3097  , 
                const double         m     = 5278  ,
                const unsigned short L     = 1     ) ;
      /// "copy constructor"
      LASS23L ( const LASS23L& right , const char* name = 0 )  ;
      /// destructor 
      virtual ~LASS23L() ;
      /// clone 
      virtual  LASS23L* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
      ( RooArgSet&     allVars      , 
        RooArgSet&     analVars     ,
        const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
      ( Int_t          code         ,  
        const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// get the complex amplitude 
      std::complex<double>        amplitude () const ; // get the complex amplitude 
      /// access to underlying function 
      const Gaudi::Math::LASS23L& function  () const { return m_lass ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// the mass 
      RooRealProxy m_x     ;
      /// K*(1430) parameters:
      RooRealProxy m_m0    ;
      RooRealProxy m_g0    ;
      /// LASS parameters 
      RooRealProxy m_a     ;
      RooRealProxy m_r     ;
      RooRealProxy m_e     ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::LASS23L m_lass ;              // the actual function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Bugg
     *  parametrisation of sigma-pole for
     *  two pion mass distribution
     *
     *  The parameterization of sigma pole by
     *  B.S.Zou and D.V.Bugg, Phys.Rev. D48 (1993) R3948.
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-04-01
     *  @see Gaudi::Math::Bugg
     */
    class GAUDI_API Bugg : public RooAbsPdf 
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::Bugg, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      Bugg  ( const char*          name               , 
              const char*          title              ,
              RooAbsReal&          x                  ,
              RooAbsReal&          M                  ,   // sigma M 
              RooAbsReal&          g2                 ,   // sigma G2 
              RooAbsReal&          b1                 ,   // sigma B1 
              RooAbsReal&          b2                 ,   // sigma B2
              RooAbsReal&          a                  ,   // sigma a 
              RooAbsReal&          s1                 ,   // sigma s1 
              RooAbsReal&          s2                 ,   // sigma s2 
              const double         m1    = 139.6/1000 ) ; // mass of pi GeV 
      /// "copy constructor"
      Bugg  ( const Bugg& right , const char* name = 0 )  ;
      /// destructor 
      virtual ~Bugg () ;
      /// clone 
      virtual  Bugg* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
      ( RooArgSet&     allVars      , 
        RooArgSet&     analVars     ,
        const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
      ( Int_t          code         ,  
        const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// get the amplitude 
      std::complex<double>     amplitude () const ;
      /// access to underlying function 
      const Gaudi::Math::Bugg& function  () const { return m_bugg ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// the mass 
      RooRealProxy m_x     ;
      /// sigma/bugg parameters 
      RooRealProxy m_M     ;
      RooRealProxy m_g2    ;
      RooRealProxy m_b1    ;
      RooRealProxy m_b2    ;
      RooRealProxy m_a     ;
      RooRealProxy m_s1    ;
      RooRealProxy m_s2    ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Bugg m_bugg ;              // the actual function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Bugg23L
     *  parametrisation of sigma-pole for
     *  two pion mass distribution form three body decays 
     *
     *  The parameterization of sigma pole by
     *  B.S.Zou and D.V.Bugg, Phys.Rev. D48 (1993) R3948.
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-04-01
     *  @see Gaudi::Math::Bugg23L
     */
    class GAUDI_API Bugg23L : public RooAbsPdf 
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::Bugg23L, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      Bugg23L ( const char*          name               , 
                const char*          title              ,
                RooAbsReal&          x                  ,
                RooAbsReal&          M                  ,   // sigma M 
                RooAbsReal&          g2                 ,   // sigma G2 
                RooAbsReal&          b1                 ,   // sigma B1 
                RooAbsReal&          b2                 ,   // sigma B2
                RooAbsReal&          a                  ,   // sigma a 
                RooAbsReal&          s1                 ,   // sigma s1 
                RooAbsReal&          s2                 ,   // sigma s2 
                const double         m1    = 139.6/1000 ,   // mass of pi GeV 
                const double         m3 = 3097.0 / 1000 ,   //  GeV
                const double         m  = 5278.0 / 1000 ,   // GeV
                const unsigned short L  =    1          ) ;
      /// "copy constructor"
      Bugg23L ( const Bugg23L& right , const char* name = 0 )  ;
      /// destructor 
      virtual ~Bugg23L () ;
      /// clone 
      virtual  Bugg23L* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
      ( RooArgSet&     allVars      , 
        RooArgSet&     analVars     ,
        const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
      ( Int_t          code         ,  
        const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// get the amplitude 
      std::complex<double>        amplitude () const ;
      /// access to underlying function 
      const Gaudi::Math::Bugg23L& function  () const { return m_bugg ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// the mass 
      RooRealProxy m_x     ;
      /// sigma/bugg parameters 
      RooRealProxy m_M     ;
      RooRealProxy m_g2    ;
      RooRealProxy m_b1    ;
      RooRealProxy m_b2    ;
      RooRealProxy m_a     ;
      RooRealProxy m_s1    ;
      RooRealProxy m_s2    ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Bugg23L m_bugg ;              // the actual function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Voigt
     *  "Voigt"-function
     *  @see Gaudi::Math::Voigt
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2011-12-05
     */
    class GAUDI_API Voigt : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::Voigt, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      Voigt
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         , 
        RooAbsReal&          m0        , 
        RooAbsReal&          gamma     , 
        RooAbsReal&          sigma     ) ;
      /// "copy" constructor 
      Voigt ( const Voigt& right , const char* name = 0  ) ;
      /// virtual destructor  
      virtual ~Voigt () ;
      /// clone 
      virtual  Voigt* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Voigt& function() const { return m_voigt ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x      ;
      RooRealProxy m_m0     ;
      RooRealProxy m_gamma  ;
      RooRealProxy m_sigma  ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Voigt m_voigt ;                      // the function 
      // ======================================================================      
    };
    // ========================================================================

    // ========================================================================
    // Resolution models 
    // ========================================================================

    // ========================================================================
    /** @class CrystalBall
     *  The special parametrization of ``Crystal Ball-function''
     *  @see Gaudi::Math::CrystalBall
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-05-13
     */
    class GAUDI_API CrystalBall : public RooAbsPdf 
    { 
    public:
      // ====================================================================== 
      ClassDef(Analysis::Models::CrystalBall, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      CrystalBall
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        RooAbsReal&          m0        ,
        RooAbsReal&          sigma     ,  
        RooAbsReal&          alpha     ,  
        RooAbsReal&          n         ) ;  // n-1
      /// "copy" constructor 
      CrystalBall ( const CrystalBall& right , const char* name = 0 ) ;
      /// virtual destructor 
      virtual ~CrystalBall () ;
      /// clone 
      virtual  CrystalBall* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::CrystalBall& function() const { return m_cb ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x      ;
      RooRealProxy m_m0     ;
      RooRealProxy m_sigma  ;
      RooRealProxy m_alpha  ;
      RooRealProxy m_n      ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::CrystalBall m_cb ;                  // the function 
      // ======================================================================
    } ;    
    // ========================================================================
    /** @class CrystalBallRS
     *  The special parametrization of ``Crystal Ball-function''
     * rigth-side crystal ball 
     *  @see Gaudi::Math::CrystalBallRightSide 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-05-13
     */
    class GAUDI_API CrystalBallRS : public RooAbsPdf 
    { 
    public:
      // ====================================================================== 
      ClassDef(Analysis::Models::CrystalBallRS, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      CrystalBallRS
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        RooAbsReal&          m0        ,
        RooAbsReal&          sigma     ,  
        RooAbsReal&          alpha     ,  
        RooAbsReal&          n         ) ;  // n-1
      /// "copy" constructor 
      CrystalBallRS ( const CrystalBallRS& right , const char* name = 0 ) ;
      /// virtual destructor 
      virtual ~CrystalBallRS () ;
      /// clone 
      virtual  CrystalBallRS* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::CrystalBallRightSide& function() const { return m_cb ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x      ;
      RooRealProxy m_m0     ;
      RooRealProxy m_sigma  ;
      RooRealProxy m_alpha  ;
      RooRealProxy m_n      ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::CrystalBallRightSide m_cb ;  // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class CrystalBallDS 
     *  double-sided ``Crystal Ball-function'' 
     *  for description of gaussian with the tail
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @see Gaudi::Math::CrystalBallDoubleSided
     *  @date 2011-05-25
     */
    class GAUDI_API CrystalBallDS : public RooAbsPdf 
    { 
    public:
      // ====================================================================== 
      ClassDef(Analysis::Models::CrystalBallDS, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      CrystalBallDS
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        RooAbsReal&          m0        ,
        RooAbsReal&          sigma     ,  //    
        RooAbsReal&          alphaL    ,  // alpha_L 
        RooAbsReal&          nL        ,  //     n_L - 1   
        RooAbsReal&          alphaR    ,  // alpha_R - 1   
        RooAbsReal&          nR        ); //     n_R 
      /// "copy" constructor 
      CrystalBallDS ( const CrystalBallDS& right , const char* name = 0 ) ;
      /// virtual destructor 
      virtual ~CrystalBallDS() ;
      /// clone 
      virtual  CrystalBallDS* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::CrystalBallDoubleSided& function() const 
      { return m_cb2 ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x      ;
      RooRealProxy m_m0     ;
      RooRealProxy m_sigma  ;
      RooRealProxy m_alphaL ;
      RooRealProxy m_nL     ;
      RooRealProxy m_alphaR ;
      RooRealProxy m_nR     ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::CrystalBallDoubleSided m_cb2 ;       // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Needham
     *  The special parametrization by Matthew NEEDHAM of 
     *  ``Crystal Ball-function'' nicely suitable for \f$J/\psi\f$-peak
     *  @thank Matthew Needham 
     *  @see Gaudi::Math::Needham
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-05-13
     */
    class GAUDI_API Needham : public RooAbsPdf 
    { 
    public:
      // ====================================================================== 
      ClassDef(Analysis::Models::Needham, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      Needham
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        RooAbsReal&          m0        ,
        RooAbsReal&          sigma     ,  
        RooAbsReal&          a0        ,  
        RooAbsReal&          a1        ,  
        RooAbsReal&          a2        ) ;
      /// "copy" constructor 
      Needham ( const Needham& right , const char* name = 0 ) ;
      /// virtual destructor 
      virtual ~Needham () ;
      /// clone 
      virtual  Needham* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Needham& function() const { return m_needham ; }
      /// get current alpha 
      double                      alpha   () const ;
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x      ;
      RooRealProxy m_m0     ;
      RooRealProxy m_sigma  ;
      RooRealProxy m_a0     ;
      RooRealProxy m_a1     ;
      RooRealProxy m_a2     ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Needham m_needham ;                  // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Apolonios
     *  A modified gaussian with power-law tail on rigth ride and exponential
     *  tail on low-side 
     *  The function is proposed by Diego Martinez Santos 
     *  @see http://arxiv.org/abs/1312.5000
     *  Here a bit modified version is used with redefined parameter <code>n</code>
     *  to be coherent with local definitions of Crystal Ball
     *  
     *  @see Gaudi::Math::Apolonios
     *  @author Vanya BELYAEV Ivane.BElyaev@itep.ru
     *  @date 2013-12-01
     */
    // ========================================================================
    class GAUDI_API Apolonios : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::Apolonios, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      Apolonios
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         , 
        RooAbsReal&          mean      , 
        RooAbsReal&          sigma     , 
        RooAbsReal&          alpha     ,
        RooAbsReal&          n         , 
        RooAbsReal&          b         ) ;
      /// "copy" constructor 
      Apolonios  ( const Apolonios& right , const char* name = 0  ) ;
      /// virtual destructor  
      virtual ~Apolonios () ;
      /// clone 
      virtual  Apolonios* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Apolonios& function() const { return m_apo ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x      ;
      RooRealProxy m_m0     ;
      RooRealProxy m_sigma  ;
      RooRealProxy m_alpha  ;
      RooRealProxy m_n      ;
      RooRealProxy m_b      ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Apolonios m_apo ;                // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Apolonios2
     *  A modified gaussian with exponential
     *  tails on low-side 
     *  
     *  @see Gaudi::Math::Apolonios2
     *  @author Vanya BELYAEV Ivane.BElyaev@itep.ru
     *  @date 2013-12-01
     */
    // ========================================================================
    class GAUDI_API Apolonios2 : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::Apolonios2, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      Apolonios2
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         , 
        RooAbsReal&          mean      , 
        RooAbsReal&          sigmaL    , 
        RooAbsReal&          sigmaR    , 
        RooAbsReal&          beta      ) ;
      /// "copy" constructor 
      Apolonios2  ( const Apolonios2& right , const char* name = 0  ) ;
      /// virtual destructor  
      virtual ~Apolonios2 () ;
      /// clone 
      virtual  Apolonios2* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Apolonios2& function() const { return m_apo2 ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x      ;
      RooRealProxy m_m0     ;
      RooRealProxy m_sigmaL ;
      RooRealProxy m_sigmaR ;
      RooRealProxy m_beta   ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Apolonios2 m_apo2 ;                // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class BifurcatedGauss 
     *  @see Gaudi::Math::BifurkatedGauss 
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2013-08-27
     */
    // ========================================================================
    class GAUDI_API BifurcatedGauss : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::BifurcatedGauss, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      BifurcatedGauss 
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         , 
        RooAbsReal&          peak      , 
        RooAbsReal&          sigmaL    , 
        RooAbsReal&          sigmaR    ) ;
      /// "copy" constructor 
      BifurcatedGauss ( const BifurcatedGauss& right , const char* name = 0  ) ;
      /// virtual destructor  
      virtual ~BifurcatedGauss () ;
      /// clone 
      virtual  BifurcatedGauss* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::BifurcatedGauss& function() const { return m_bg ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x      ;
      RooRealProxy m_peak   ;
      RooRealProxy m_sigmaL ;
      RooRealProxy m_sigmaR ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::BifurcatedGauss m_bg ;               // the function 
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class GenGaussV1
     *  Simple class that implements the generalized normal distribution v1
     *  @see http://en.wikipedia.org/wiki/Generalized_normal_distribution#Version_1
     *  @see Gaudi::Math::GenGaussV1 
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2013-08-27
     */
    // ========================================================================
    class GAUDI_API GenGaussV1 : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::GenGaussV1, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      GenGaussV1
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         , 
        RooAbsReal&          mu        , 
        RooAbsReal&          alpha     , 
        RooAbsReal&          beta      ) ;
      /// "copy" constructor 
      GenGaussV1 ( const GenGaussV1& right , const char* name = 0  ) ;
      /// virtual destructor  
      virtual ~GenGaussV1 () ;
      /// clone 
      virtual  GenGaussV1* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::GenGaussV1& function() const { return m_ggv1 ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x      ;
      RooRealProxy m_mu     ;
      RooRealProxy m_alpha  ;
      RooRealProxy m_beta   ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::GenGaussV1 m_ggv1 ;                 // the function 
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class GenGaussV2
     *  Simple class that implements the generalized normal distribution v2
     *  @see http://en.wikipedia.org/wiki/Generalized_normal_distribution#Version_2
     *  @see Gaudi::Math::GenGaussV2 
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2013-08-27
     */
    // ========================================================================
    class GAUDI_API GenGaussV2 : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::GenGaussV2, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      GenGaussV2
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         , 
        RooAbsReal&          xi        , 
        RooAbsReal&          alpha     , 
        RooAbsReal&          kappa     ) ;
      /// "copy" constructor 
      GenGaussV2 ( const GenGaussV2& right , const char* name = 0  ) ;
      /// virtual destructor  
      virtual ~GenGaussV2 () ;
      /// clone 
      virtual  GenGaussV2* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::GenGaussV2& function() const { return m_ggv2 ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x      ;
      RooRealProxy m_xi     ;
      RooRealProxy m_alpha  ;
      RooRealProxy m_kappa  ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::GenGaussV2 m_ggv2 ;                 // the function 
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class SkewGauss
     *  Simple class that implements the skew normal distribution
     *  @see http://en.wikipedia.org/wiki/Skew_normal_distribution
     *  @see Gaudi::Math::SkewGauss 
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2013-08-27
     */
    // ========================================================================
    class GAUDI_API SkewGauss : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::SkewGauss, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      SkewGauss
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         , 
        RooAbsReal&          xi        , 
        RooAbsReal&          omega     , 
        RooAbsReal&          alpha     ) ;
      /// "copy" constructor 
      SkewGauss ( const SkewGauss& right , const char* name = 0  ) ;
      /// virtual destructor  
      virtual ~SkewGauss () ;
      /// clone 
      virtual  SkewGauss* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::SkewGauss& function() const { return m_sg ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x      ;
      RooRealProxy m_xi     ;
      RooRealProxy m_omega  ;
      RooRealProxy m_alpha  ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::SkewGauss m_sg ;                     // the function 
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class Bukin
     *  "Bukin"-function, aka "Modified Novosibirsk function"
     *  @see http://arxiv.org/abs/1107.5751
     *  @see http://dx.doi.org/10.1007/JHEP06(2012)141     
     *  @see Gaudi::Math::Bukin
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2011-12-05
     */
    class GAUDI_API Bukin : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::Bukin, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      Bukin
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         , 
        RooAbsReal&          peak      ,   // peak position 
        RooAbsReal&          sigma     ,   // "width"
        RooAbsReal&          xi        ,   // asymmetry pareamneter 
        RooAbsReal&          rhoL      ,   // left tail 
        RooAbsReal&          rhoR      ) ; // right tail 
      /// "copy" constructor 
      Bukin ( const Bukin& right , const char* name = 0  ) ;
      /// virtual destructor  
      virtual ~Bukin () ;
      /// clone 
      virtual  Bukin* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Bukin& function() const { return m_bukin ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x      ;
      RooRealProxy m_peak   ;
      RooRealProxy m_sigma  ;
      RooRealProxy m_xi     ;
      RooRealProxy m_rhoL   ;
      RooRealProxy m_rhoR   ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Bukin m_bukin ;                      // the function 
      // ======================================================================      
    } ;  
    // ========================================================================
    /** @class StudentT 
     *  Student-T distribution
     *
     *  \f[  f(y) = \frac{1}{\sqrt{\pi n}} \frac { \Gamma( \frac{n+1}{2}) } { \Gamma( \frac{n}{2}  ) }
     *  \left( 1 + \frac{y^2}{n} \right)^{ -\frac{n+1}{2}} \f], 
     *  where \f$ y = \frac{x - \mu}{\sigma} \f$  
     * 
     *  @see Gaudi::Math::StudentT
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2013-01-05
     */
    class GAUDI_API StudentT: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::StudentT, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      StudentT ( const char*          name      , 
                 const char*          title     ,
                 RooAbsReal&          x         ,
                 RooAbsReal&          mu        ,
                 RooAbsReal&          sigma     ,
                 RooAbsReal&          n         ) ;
      /// "copy constructor"
      StudentT ( const StudentT&      right     , 
                 const char*          name  = 0 )  ;
      /// destructor 
      virtual ~StudentT() ;
      /// clone 
      virtual  StudentT* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::StudentT& function() const { return m_stt ; }
      // ======================================================================
    protected: 
      // ======================================================================
      RooRealProxy m_x        ;
      RooRealProxy m_mu       ;
      RooRealProxy m_sigma    ;
      RooRealProxy m_n        ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::StudentT m_stt ;           // the actual function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class BifurcatedStudentT 
     *  @see Gaudi::Math::BifurcatedStudentT
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2013-01-05
     */
    class GAUDI_API BifurcatedStudentT: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::BifurcatedStudentT, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      BifurcatedStudentT ( const char*          name      , 
                           const char*          title     ,
                           RooAbsReal&          x         ,
                           RooAbsReal&          mu        ,
                           RooAbsReal&          sigmaL    ,
                           RooAbsReal&          sigmaR    ,
                           RooAbsReal&          nL        , 
                           RooAbsReal&          nR        ) ;
      /// "copy constructor"
      BifurcatedStudentT ( const BifurcatedStudentT& right     , 
                           const char*               name  = 0 )  ;
      /// destructor 
      virtual ~BifurcatedStudentT() ;
      /// clone 
      virtual  BifurcatedStudentT* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::BifurcatedStudentT& function() const { return m_stt ; }
      // ======================================================================
    protected: 
      // ======================================================================
      RooRealProxy m_x        ;
      RooRealProxy m_mu       ;
      RooRealProxy m_sigmaL   ;
      RooRealProxy m_sigmaR   ;
      RooRealProxy m_nL       ;
      RooRealProxy m_nR       ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::BifurcatedStudentT m_stt ; // the actual function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class GramCharlierA
     *  The peak with Gram-Charlier type A parameterization
     *  @see Gaudi::Math::GramCharlierA 
     *  http://en.wikipedia.org/wiki/Edgeworth_series
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2011-12-05
     */
    class GAUDI_API GramCharlierA : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::GramCharlierA, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      GramCharlierA 
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         , 
        RooAbsReal&          mean      , 
        RooAbsReal&          sigma     , 
        RooAbsReal&          kappa3    ,
        RooAbsReal&          kappa4    );
      /// "copy" constructor 
      GramCharlierA ( const GramCharlierA& right , const char* name = 0  ) ;
      /// virtual destructor  
      virtual ~GramCharlierA () ;
      /// clone 
      virtual  GramCharlierA* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::GramCharlierA& function() const { return m_gca ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x      ;
      RooRealProxy m_m0     ;
      RooRealProxy m_sigma  ;
      RooRealProxy m_kappa3 ;
      RooRealProxy m_kappa4 ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::GramCharlierA m_gca ;                // the function 
      // ======================================================================
    } ;  
    // ========================================================================

    // ========================================================================
    // Smooth functions for background 
    // ========================================================================
    
    // ========================================================================
    /** @class PhaseSpace2
     *  simple model for 2-body phase space 
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2011-11-30
     */
    class PhaseSpace2 : public RooAbsPdf 
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::PhaseSpace2, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      PhaseSpace2 ( const char*          name      , 
                    const char*          title     ,
                    RooAbsReal&          x         ,
                    const double         m1        , 
                    const double         m2        ) ;
      /// "copy constructor"
      PhaseSpace2 ( const PhaseSpace2& right     , 
                    const char*        name  = 0 )  ;
      /// destructor 
      virtual ~PhaseSpace2() ;
      /// clone 
      virtual PhaseSpace2* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function       /// access to underlying function 
      const Gaudi::Math::PhaseSpace2& function() const { return m_ps2 ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x     ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::PhaseSpace2 m_ps2 ;           // the actual function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PhaseSpaceLeft
     *  simple model for left-edge of N-body phase-space 
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2011-11-30
     */
    class PhaseSpaceLeft : public RooAbsPdf 
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::PhaseSpaceLeft, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      PhaseSpaceLeft ( const char*          name      , 
                       const char*          title     ,
                       RooAbsReal&          x         ,
                       RooAbsReal&          threshold ,
                       const unsigned short N         ) ;
      /// "copy constructor"
      PhaseSpaceLeft ( const PhaseSpaceLeft& right     , 
                       const char*           name  = 0 )  ;
      /// destructor 
      virtual ~PhaseSpaceLeft() ;
      /// clone 
      virtual PhaseSpaceLeft* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::PhaseSpaceLeft& function() const { return m_left ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x         ;
      RooRealProxy m_threshold ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::PhaseSpaceLeft m_left ;        // the actual function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PhaseSpaceRight
     *  simple model for right-edge of L-body phase-space in N-body decays 
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2011-11-30
     */
    class PhaseSpaceRight : public RooAbsPdf 
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::PhaseSpaceRight, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      PhaseSpaceRight ( const char*          name      , 
                        const char*          title     ,
                        RooAbsReal&          x         ,
                        RooAbsReal&          threshold ,
                        const unsigned short L         , 
                        const unsigned short N         ) ;
      /// "copy constructor"
      PhaseSpaceRight ( const PhaseSpaceRight& right     , 
                        const char*            name  = 0 )  ;
      /// destructor 
      virtual ~PhaseSpaceRight () ;
      /// clone 
      virtual PhaseSpaceRight* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::PhaseSpaceRight& function() const { return m_right ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x         ;
      RooRealProxy m_threshold ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::PhaseSpaceRight m_right ;     // the actual function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PhaseSpaceNL
     *  
     *  The phase space function for L-body systema from N-body decay
     *
     *  @see Gaudi::Math::PhaseSpaceNL
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2011-11-30
     */
    class PhaseSpaceNL : public RooAbsPdf
    {    
    public:
      // ======================================================================
      ClassDef(Analysis::Models::PhaseSpaceNL, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      PhaseSpaceNL ( const char*          name      , 
                     const char*          title     ,
                     RooAbsReal&          x         ,
                     RooAbsReal&          low       ,
                     RooAbsReal&          high      ,    
                     const unsigned short N         , 
                     const unsigned short L         ) ;
      /// "copy" constructor 
      PhaseSpaceNL ( const PhaseSpaceNL& , const char* name = 0 ) ;
      /// virtual destructor 
      virtual ~PhaseSpaceNL () ;
      /// clone 
      virtual PhaseSpaceNL* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::PhaseSpaceNL& function() const { return m_ps ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x     ;
      RooRealProxy m_low   ;
      RooRealProxy m_high  ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::PhaseSpaceNL m_ps ;           // the actual function 
      // ======================================================================
    };
    // ========================================================================
    /** @class PhaseSpacePol
     *  The mass-ditribtion of L-particles from N-body phase space decays, 
     *  modulate with non-negative polynomial 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2013-10-05
     */
    class PhaseSpacePol : public RooAbsPdf 
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::PhaseSpacePol, 1) ;
      // ======================================================================
      /// constructor from all parameters 
      PhaseSpacePol 
      ( const char*                      name   , 
        const char*                      title  ,
        RooAbsReal&                      x      ,
        const double                     low    , 
        const double                     high   ,
        const unsigned short             N      , 
        const unsigned short             L      , 
        RooAbsReal&                      phi1   ) ;
      /// constructor from all parameters 
      PhaseSpacePol 
      ( const char*                      name   , 
        const char*                      title  ,
        RooAbsReal&                      x      ,
        const double                     low    , 
        const double                     high   ,
        const unsigned short             N      , 
        const unsigned short             L      , 
        RooAbsReal&                      phi1   ,
        RooAbsReal&                      phi2   ) ;
      /// constructor from all parameters 
      PhaseSpacePol 
      ( const char*                      name   , 
        const char*                      title  ,
        RooAbsReal&                      x      ,
        const double                     low    , 
        const double                     high   ,
        const unsigned short             N      , 
        const unsigned short             L      , 
        RooAbsReal&                      phi1   ,
        RooAbsReal&                      phi2   , 
        RooAbsReal&                      phi3   ) ;
      /// constructor from all parameters 
      PhaseSpacePol 
      ( const char*                      name   , 
        const char*                      title  ,
        RooAbsReal&                      x      ,
        const double                     low    , 
        const double                     high   ,
        const unsigned short             N      , 
        const unsigned short             L      , 
        RooArgList&                      phis   ) ;
      // ======================================================================
      /// constructor from all parameters 
      PhaseSpacePol 
      ( const char*                      name   , 
        const char*                      title  ,
        RooAbsReal&                      x      ,
        const Gaudi::Math::PhaseSpaceNL& ps     , 
        RooAbsReal&                      phi1   ) ;
      /// constructor from all parameters 
      PhaseSpacePol 
      ( const char*                      name   , 
        const char*                      title  ,
        RooAbsReal&                      x      ,
        const Gaudi::Math::PhaseSpaceNL& ps     , 
        RooAbsReal&                      phi1   ,
        RooAbsReal&                      phi2   ) ;
      /// constructor from all parameters 
      PhaseSpacePol 
      ( const char*                      name   , 
        const char*                      title  ,
        RooAbsReal&                      x      ,
        const Gaudi::Math::PhaseSpaceNL& ps     , 
        RooAbsReal&                      phi1   ,
        RooAbsReal&                      phi2   ,
        RooAbsReal&                      phi3   ) ;
      /// constructor from all parameters 
      PhaseSpacePol 
      ( const char*                      name   , 
        const char*                      title  ,
        RooAbsReal&                      x      ,
        const Gaudi::Math::PhaseSpaceNL& ps     , 
        RooArgList&                      phis   ) ;
      // ======================================================================
      // "copy" constructor 
      // ======================================================================
      PhaseSpacePol ( const PhaseSpacePol& right    , 
                      const char*          name = 0 ) ;
      /// destructor 
      virtual ~PhaseSpacePol () ;
      /// clone 
      virtual  PhaseSpacePol* clone( const char* name )  const ;
      // ======================================================================
    public:
      // ======================================================================
      virtual Double_t evaluate () const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
      ( RooArgSet&     allVars      , 
        RooArgSet&     analVars     ,
        const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
      ( Int_t          code         ,  
        const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      const Gaudi::Math::PhaseSpacePol& function() const { return m_ps ; }
      // ====================================================================== 
   private:
      // ======================================================================
      RooRealProxy m_x    ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual phase space function 
      mutable Gaudi::Math::PhaseSpacePol m_ps ;  // the actual function
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class PhaseSpace23L
     *  simple model for 2-body phase space from 3-body decays with 
     *  the orbital momenta:
     *
     *   \f$ f \propto q^{2\ell+1}p^{2L+1}\f$, where
     *     \f$\ell\f$ is the orbital momentum of the pair of particles, 
     *    and \f$L\f$ is the orbital momentum between the pair and 
     *    the third particle. 
     *   E.g. taking \f$\ell=0, L=1\f$, one can get the S-wave contribution for 
     *   \f$\pi^+\pi^-\f$-mass from \f$B^0\rightarrowJ/\psi\pi^+\pi^-\f$ decay.
     * 
     *  @see Gaudi::Math::PhaseSpace23L 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-04-01
     */
    class PhaseSpace23L : public RooAbsPdf 
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::PhaseSpace23L, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /** constructor from all parameters 
       *  @param m1 the mass of the first  particle 
       *  @param m2 the mass of the second particle 
       *  @param m3 the mass of the third  particle 
       *  @param m  the mass of the mother particle (m>m1+m2+m3)
       *  @param L  the angular momentum between the first pair and the third particle
       *  @param l  the angular momentum between the first and the second particle
       */
      PhaseSpace23L ( const char*          name      , 
                      const char*          title     ,
                      RooAbsReal&          x         ,
                      const double         m1        , 
                      const double         m2        ,
                      const double         m3        ,
                      const double         m         ,
                      const unsigned short L         , 
                      const unsigned short l     = 0 ) ;
      /// "copy constructor"
      PhaseSpace23L ( const PhaseSpace23L& right , const char* name = 0 )  ;
      /// destructor 
      virtual ~PhaseSpace23L() ;
      /// clone 
      virtual PhaseSpace23L* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
      ( RooArgSet&     allVars      , 
        RooArgSet&     analVars     ,
        const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
      ( Int_t          code         ,  
        const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::PhaseSpace23L& function() const { return m_ps23L ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x     ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      Gaudi::Math::PhaseSpace23L m_ps23L ;               // the actual function 
      // ======================================================================
    } ;
    // ========================================================================

    // ========================================================================
    // Smooth empirical models for background  
    // ========================================================================
    
    // ========================================================================
    /** @class PolyPositive
     *  PolyPositive polynomial
     *  @see Gaudi::Math::Positive
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2011-05-25
     */
    class GAUDI_API PolyPositive: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::PolyPositive, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// general 
      PolyPositive
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        const RooArgList&    coeffs    ,
        const double         xmin      , 
        const double         xmax      ) ;
      /// copy
      PolyPositive
      ( const PolyPositive&     right     , 
        const char*          name = 0  ) ;
      /// destructor 
      virtual ~PolyPositive() ;
      /// clone 
      virtual  PolyPositive* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
      ( RooArgSet&     allVars      , 
        RooArgSet&     analVars     ,
        const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
      ( Int_t          code         ,  
        const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Positive& function() const { return m_positive ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x    ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Positive m_positive ;               // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PolyMonothonic
     *  positive monothonic  polynomial
     *  @see Gaudi::Math::Monothonic
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2015-02-07
     */
    class GAUDI_API PolyMonothonic: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::PolyMonothonic, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// general 
      PolyMonothonic
        ( const char*          name       , 
          const char*          title      ,
          RooAbsReal&          x          ,
          const RooArgList&    coeffs     ,
          const double         xmin       , 
          const double         xmax       ,
          const bool           increasing ) ;
      /// copy
      PolyMonothonic
        ( const PolyMonothonic&     right     , 
          const char*          name = 0  ) ;
      /// destructor 
      virtual ~PolyMonothonic() ;
      /// clone 
      virtual  PolyMonothonic* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
      ( RooArgSet&     allVars      , 
        RooArgSet&     analVars     ,
        const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
      ( Int_t          code         ,  
        const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Monothonic& function() const { return m_monothonic ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x    ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Monothonic m_monothonic ;            // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PolyConvex
     *  Positive polynomial with fixes sign first and second derivatives 
     *  @see Gaudi::Math::Convex
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2015-02-07
     */
    class GAUDI_API PolyConvex: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::PolyConvex, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// general 
      PolyConvex
        ( const char*          name       , 
          const char*          title      ,
          RooAbsReal&          x          ,
          const RooArgList&    coeffs     ,
          const double         xmin       , 
          const double         xmax       ,
          const bool           increasing ,
          const bool           convex     ) ;
      /// copy
      PolyConvex
        ( const PolyConvex&    right     , 
          const char*          name = 0  ) ;
      /// destructor 
      virtual ~PolyConvex () ;
      /// clone 
      virtual  PolyConvex* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
      ( RooArgSet&     allVars      , 
        RooArgSet&     analVars     ,
        const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
      ( Int_t          code         ,  
        const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Convex& function() const { return m_convex ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x    ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Convex m_convex ;                    // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ExpoPositive
     *  exponential multiplied on positive polynomial
     *  @see Gaudi::Math::Positive
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2011-05-25
     */
    class GAUDI_API ExpoPositive: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::ExpoPositive, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// general 
      ExpoPositive
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        RooAbsReal&          tau       , 
        const RooArgList&    coeffs    ,
        const double         xmin      , 
        const double         xmax      ) ;
      /// copy
      ExpoPositive
      ( const ExpoPositive&     right     , 
        const char*          name = 0  ) ;
      /// destructor 
      virtual ~ExpoPositive() ;
      /// clone 
      virtual  ExpoPositive* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::ExpoPositive& function() const { return m_positive ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x    ;
      RooRealProxy m_tau  ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::ExpoPositive m_positive ;           // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Sigmoid
     *  The product of sigmoid function and positive polynomial 
     *  @see Gaudi::Math::Sigmoid
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2015-02-07
     */
    class GAUDI_API PolySigmoid: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::PolySigmoid, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// general 
      PolySigmoid
        ( const char*          name       , 
          const char*          title      ,
          RooAbsReal&          x          ,
          const RooArgList&    coeffs     ,
          const double         xmin       , 
          const double         xmax       ,
          RooAbsReal&          alpha      , 
          RooAbsReal&          x0         ) ;
      /// copy
      PolySigmoid
        ( const PolySigmoid&   right     , 
          const char*          name = 0  ) ;
      /// destructor 
      virtual ~PolySigmoid() ;
      /// clone 
      virtual  PolySigmoid* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
      ( RooArgSet&     allVars      , 
        RooArgSet&     analVars     ,
        const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
      ( Int_t          code         ,  
        const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Sigmoid& sigmoid () const { return m_sigmoid    ; }
      /// access to underlying function 
      const Gaudi::Math::Sigmoid& function() const { return   sigmoid () ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x     ;
      RooListProxy m_phis  ;
      RooRealProxy m_alpha ;
      RooRealProxy m_x0    ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Sigmoid m_sigmoid ;                 // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class TwoExpoPositive
     *  simple difference of two exponents modulated with positive polynomials 
     *  \f$ f(x) = e_2(x) * p_n(x) \f$, where 
     *  \f$ e_2(x) \propto 
     *        \mathrm{e}^{-a_1    x}       -\mathrm{e}^{-a_2 x} = 
     *        \mathrm{e}^{-\alpha x}\left(1-\mathrm{e}^{-\delta x}\right) \f$
     *  @see Gaudi::Math::TwoExpoPositive
     *  @see Gaudi::Math::ExpoPositive
     *  @see Gaudi::Math::Positive
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2011-05-25
     */
    class GAUDI_API TwoExpoPositive: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::TwoExpoPositive, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// general 
      TwoExpoPositive
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        RooAbsReal&          alpha     , 
        RooAbsReal&          delta     , 
        RooAbsReal&          x0        , 
        const RooArgList&    coeffs    ,
        const double         xmin      , 
        const double         xmax      ) ;
      /// copy
      TwoExpoPositive
        ( const TwoExpoPositive& right     , 
          const char*           name = 0  ) ;
      /// destructor 
      virtual ~TwoExpoPositive() ;
      /// clone 
      virtual  TwoExpoPositive* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      const Gaudi::Math::TwoExpoPositive& function() const { return m_2expopos ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x     ;
      RooRealProxy m_alpha ;
      RooRealProxy m_delta ;
      RooRealProxy m_x0    ;
      RooListProxy m_phis  ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::TwoExpoPositive m_2expopos;         // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class GammaDist 
     *  Gamma-distribution shape/scale parameters 
     *  http://en.wikipedia.org/wiki/Gamma_distribution
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2013-05-11
     *  @see Gaudi::Math::GammaDist 
     */
    class GAUDI_API GammaDist : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::GammaDist, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters
      GammaDist ( const char*          name      , 
                  const char*          title     ,
                  RooAbsReal&          x         , 
                  RooAbsReal&          k         ,
                  RooAbsReal&          theta     ) ;
      /// "copy constructor"
      GammaDist ( const GammaDist&     right     , 
                  const char*          name  = 0 )  ;
      /// destructor 
      virtual ~GammaDist () ;
      /// clone 
      virtual  GammaDist* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::GammaDist& function() const { return m_gamma ; }
      // ======================================================================
    protected: 
      // ======================================================================
      RooRealProxy m_x        ;
      RooRealProxy m_k        ;
      RooRealProxy m_theta    ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      mutable Gaudi::Math::GammaDist m_gamma ; // the actual function
      // ======================================================================
    } ;
    // ========================================================================
    /** @class GenGammaDist 
     *  Generalized Gamma-distribution with additional shift parameter 
     *  http://en.wikipedia.org/wiki/Generalized_gamma_distribution
     *  special cases : 
     *   - p == 1      : Gamma  distribution
     *   - p == k      : Weibull distribution
     *   - p == k == 1 : Exponential distribution
     *   - p == k == 2 : Rayleigh    distribution
     *  @see Gaudi::Math::GenGammaDist
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2013-05-11
     *  @see Gaudi::Math::GenGammaDist 
     */
    class GAUDI_API GenGammaDist : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::GenGammaDist, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters
      GenGammaDist 
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         , 
        RooAbsReal&          k         ,
        RooAbsReal&          theta     ,
        RooAbsReal&          p         ,
        RooAbsReal&          low       ) ;
      /// "copy constructor"
      GenGammaDist ( const GenGammaDist&  right     , 
                     const char*          name  = 0 )  ;
      /// destructor 
      virtual ~GenGammaDist () ;
      /// clone 
      virtual  GenGammaDist* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::GenGammaDist& function() const { return m_ggamma ; }
      // ======================================================================
    protected: 
      // ======================================================================
      RooRealProxy m_x        ;
      RooRealProxy m_k        ;
      RooRealProxy m_theta    ;
      RooRealProxy m_p        ;
      RooRealProxy m_low      ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      mutable Gaudi::Math::GenGammaDist m_ggamma ; // the actual function
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Amoroso
     *  Another view on generalized gamma distribtion
     *  http://arxiv.org/pdf/1005.3274
     *  @see Gaudi::Math::Amoroso
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2011-12-05
     */
    class GAUDI_API Amoroso : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::Amoroso, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      Amoroso
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         , 
        RooAbsReal&          theta     , 
        RooAbsReal&          alpha     , 
        RooAbsReal&          beta      ,
        RooAbsReal&          a         ) ;
      /// "copy" constructor 
      Amoroso ( const Amoroso& right , const char* name = 0  ) ;
      /// virtual destructor  
      virtual ~Amoroso () ;
      /// clone 
      virtual  Amoroso* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Amoroso& function() const { return m_amoroso ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x      ;
      RooRealProxy m_theta  ;
      RooRealProxy m_alpha  ;
      RooRealProxy m_beta   ;
      RooRealProxy m_a      ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Amoroso m_amoroso ;                  // the function 
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class LogGammaDist 
     *  Distribution for log(x), where x follows 
     *  gamma-distribution shape/scale parameters 
     *  http://en.wikipedia.org/wiki/Gamma_distribution
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2013-05-11
     *  @see Gaudi::Math::LogGammaDist 
     */
    class GAUDI_API LogGammaDist : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::LogGammaDist, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters
      LogGammaDist ( const char*          name      , 
                     const char*          title     ,
                     RooAbsReal&          x         , 
                     RooAbsReal&          k         ,
                     RooAbsReal&          theta     ) ;
      /// "copy constructor"
      LogGammaDist ( const LogGammaDist&  right     , 
                     const char*          name  = 0 )  ;
      /// destructor 
      virtual ~LogGammaDist () ;
      /// clone 
      virtual  LogGammaDist* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::LogGammaDist& function() const { return m_gamma ; }
      // ======================================================================
    protected: 
      // ======================================================================
      RooRealProxy m_x        ;
      RooRealProxy m_k        ;
      RooRealProxy m_theta    ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      mutable Gaudi::Math::LogGammaDist m_gamma ; // the actual function
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class Log10GammaDist 
     *  Distribution for log10(x), where x follows 
     *  gamma-distribution shape/scale parameters 
     *  http://en.wikipedia.org/wiki/Gamma_distribution
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2013-05-11
     *  @see Gaudi::Math::Log10GammaDist 
     */
    class GAUDI_API Log10GammaDist : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::Log10GammaDist, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters
      Log10GammaDist ( const char*           name      , 
                       const char*           title     ,
                       RooAbsReal&           x         , 
                       RooAbsReal&           k         ,
                       RooAbsReal&           theta     ) ;
      /// "copy constructor"
      Log10GammaDist ( const Log10GammaDist& right     , 
                       const char*           name  = 0 )  ;
      /// destructor 
      virtual ~Log10GammaDist () ;
      /// clone 
      virtual  Log10GammaDist* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Log10GammaDist& function() const { return m_gamma ; }
      // ======================================================================
    protected: 
      // ======================================================================
      RooRealProxy m_x        ;
      RooRealProxy m_k        ;
      RooRealProxy m_theta    ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      mutable Gaudi::Math::Log10GammaDist m_gamma ; // the actual function
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class LogGamma
     *  - http://arxiv.org/pdf/1005.3274
     *  - Prentice, R. L. (1974). A log gamma model and its maximum likelihood
     *                            estimation. Biometrika 61, 539
     *  - Johnson, N. L., Kotz, S., and Balakrishnan, N. (1995). Continuous
     *            univariate distributions, 2nd ed. Vol. 2. Wiley, New York.
     *  - Bartlett, M. S. and G., K. M. (1946). The statistical analysis of
     *                  variance-heterogeneity and the logarithmic transformation. 
     *                 J. Roy. Statist. Soc. Suppl. 8, 1, 128.
     *
     *  dot not mix with Analysis::Models::LogGammaDist        
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2013-05-11
     *  @see Gaudi::Math::LogGamma 
     */
    class GAUDI_API LogGamma : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::LogGamma, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters
      LogGamma ( const char*          name      , 
                 const char*          title     ,
                 RooAbsReal&          x         , 
                 RooAbsReal&          nu        ,
                 RooAbsReal&          lambda    ,
                 RooAbsReal&          alpha     ) ;
      /// "copy constructor"
      LogGamma ( const LogGamma&      right     , 
                 const char*          name  = 0 )  ;
      /// destructor 
      virtual ~LogGamma () ;
      /// clone 
      virtual  LogGamma* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::LogGamma& function() const { return m_lgamma ; }
      // ======================================================================
    protected: 
      // ======================================================================
      RooRealProxy m_x        ;
      RooRealProxy m_nu       ;
      RooRealProxy m_lambda   ;
      RooRealProxy m_alpha    ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      mutable Gaudi::Math::LogGamma m_lgamma ; // the actual function
      // ======================================================================
    } ;
    // ========================================================================
    /** @class BetaPrime
     *  http://en.wikipedia.org/wiki/Beta_prime_distribution
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2013-05-11
     *  @see Gaudi::Math::BetaPrime 
     */
    class GAUDI_API BetaPrime : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::BetaPrime, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters
      BetaPrime 
      ( const char*           name      , 
        const char*           title     ,
        RooAbsReal&           x         , 
        RooAbsReal&           alpha     ,
        RooAbsReal&           beta      ,
        RooAbsReal&           scale     ,
        RooAbsReal&           shift     ) ;
      /// "copy constructor"
      BetaPrime 
      ( const BetaPrime&      right     , 
        const char*           name  = 0 )  ;
      /// destructor 
      virtual ~BetaPrime () ;
      /// clone 
      virtual  BetaPrime* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::BetaPrime& function() const { return m_betap ; }
      // ======================================================================
    protected: 
      // ======================================================================
      RooRealProxy m_x        ;
      RooRealProxy m_alpha    ;
      RooRealProxy m_beta     ;
      RooRealProxy m_scale    ;
      RooRealProxy m_shift    ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      mutable Gaudi::Math::BetaPrime m_betap ; // the actual function
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Landau
     *  http://en.wikipedia.org/wiki/Landau_distribution
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2014-06-07
     *  @see Gaudi::Math::Landau
     */
    class GAUDI_API Landau : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::Landau, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters
      Landau
      ( const char*           name      , 
        const char*           title     ,
        RooAbsReal&           x         , 
        RooAbsReal&           scale     ,
        RooAbsReal&           shift     ) ;
      /// "copy constructor"
      Landau
      ( const Landau&         right     , 
        const char*           name  = 0 )  ;
      /// destructor 
      virtual ~Landau  () ;
      /// clone 
      virtual  Landau* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Landau& function() const { return m_landau ; }
      // ======================================================================
    protected: 
      // ======================================================================
      RooRealProxy m_x        ;
      RooRealProxy m_scale    ;
      RooRealProxy m_shift    ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      mutable Gaudi::Math::Landau m_landau ; // the actual function
      // ======================================================================
    } ;
    // ========================================================================
    /** @class SinhAsinh
     *  
     *  Jones, M. C.; Pewsey, A. (2009). 
     *  "Sinh-arcsinh distributions". Biometrika 96 (4): 761. 
     *  doi:10.1093/biomet/asp053
     *  http://oro.open.ac.uk/22510
     *
     *  Location & scale  parameters are the 
     *  usual representation of the family of 
     *  distributions 
     *  - \f$\epsilon\f$ parameter control the skewness 
     *  - \f$\delta\f$   parameter control the kurtosis 
     *  Normal distribtion reappears as \f$\epsilon=0\f$ 
     *  and \f$\delta=1\f$ 
     *  The heavy tails correspond to \f$\delta<1\f$, 
     *  light tails correpond to \f$\delta>1\f$
  
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2014-08-02
     *  @see Gaudi::Math::SinhAsinh
     */
    class GAUDI_API SinhAsinh : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::SinhAsinh, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters
      SinhAsinh
        ( const char*           name      , 
          const char*           title     ,
          RooAbsReal&           x         , 
          RooAbsReal&           mu        ,
          RooAbsReal&           sigma     ,
          RooAbsReal&           epsilon   ,
          RooAbsReal&           delta     ) ;
      /// "copy constructor"
      SinhAsinh
        ( const SinhAsinh&      right     , 
          const char*           name  = 0 )  ;
      /// destructor 
      virtual ~SinhAsinh  () ;
      /// clone 
      virtual  SinhAsinh* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::SinhAsinh& function() const { return m_sinhasinh ; }
      // ======================================================================
    protected: 
      // ======================================================================
      RooRealProxy m_x        ;
      RooRealProxy m_mu       ;
      RooRealProxy m_sigma    ;
      RooRealProxy m_epsilon  ;
      RooRealProxy m_delta    ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      mutable Gaudi::Math::SinhAsinh m_sinhasinh ; // the actual function
      // ======================================================================
    } ;
    // ========================================================================
    /** @class JohnsonSU 
     *  
     *  Johnson, N. L. (1949) 
     *  "Systems of frequency curves generated by methods of translation"
     *  Biometrika 36: 149–176 JSTOR 2332539
     *  @see https://en.wikipedia.org/wiki/Johnson_SU_distribution
     *
     *  When variable \f$x\f$ follows Johnson-SU distribution, 
     *  the variable 
     *  \f$ z = \gamma + \delta \sinh^{-1}\frac{ x - \xi}{\lambda} \f$
     *  follows normal distribtion with mean 0 and sigma 1.
     *
     *  Note:
     *  Symmetric case of JonhsonSU distribution is 
     *  recovere by \f$\delta\rightarrow0\f$ for 
     *  "sinh-asinh" distribution, see 
     *  Jones, M. C.; Pewsey, A. (2009). 
     *  "Sinh-arcsinh distributions". Biometrika 96 (4): 761. 
     *  doi:10.1093/biomet/asp053
     *  http://oro.open.ac.uk/22510
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2015-07-11
     *  @see Gaudi::Math::JohnsonSU
     */
    class GAUDI_API JohnsonSU : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::JohnsonSU, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters
      JohnsonSU 
        ( const char*           name      , 
          const char*           title     ,
          RooAbsReal&           x         , 
          RooAbsReal&           xi        ,
          RooAbsReal&           lam       ,
          RooAbsReal&           delta     ,
          RooAbsReal&           gamma     ) ;
      /// "copy constructor"
      JohnsonSU 
        ( const JohnsonSU&      right     , 
          const char*           name  = 0 )  ;
      /// destructor 
      virtual ~JohnsonSU  () ;
      /// clone 
      virtual  JohnsonSU* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::JohnsonSU& function() const { return m_johnsonSU ; }
      // ======================================================================
    protected: 
      // ======================================================================
      RooRealProxy m_x        ;
      RooRealProxy m_xi       ;
      RooRealProxy m_lambda   ;
      RooRealProxy m_delta    ;
      RooRealProxy m_gamma    ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      mutable Gaudi::Math::JohnsonSU m_johnsonSU ; // the actual function
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Atlas 
     *  Modified gaussian function 
     *  \f$  f(x) \propto \exp( -frac{\delta x^{1+\frac{1}{1+\deltax/2}}}{2})\f$,
     *  where \f$\delta x = \left| x - \mu \right|/\sigma\f$
     *  Fuction is taken from http://arxiv.org/abs/arXiv:1507.07099
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-08-21
     *  @see Gaudi::Math::Atlas 
     */
    class GAUDI_API Atlas : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::Atlas, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters
      Atlas
      ( const char*           name      , 
        const char*           title     ,
        RooAbsReal&           x         , 
        RooAbsReal&           mu        ,
        RooAbsReal&           sigma     ) ;
      /// "copy constructor"
      Atlas
      ( const Atlas&          right     , 
        const char*           name  = 0 )  ;
      /// destructor 
      virtual ~Atlas  () ;
      /// clone 
      virtual  Atlas* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Atlas& function() const { return m_atlas ; }
      // ======================================================================
    protected: 
      // ======================================================================
      RooRealProxy m_x        ;
      RooRealProxy m_mu       ;
      RooRealProxy m_sigma    ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      mutable Gaudi::Math::Atlas m_atlas ; // the actual function
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Argus
     *  http://en.wikipedia.org/wiki/ARGUS_distribution
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2014-06-07
     *  @see Gaudi::Math::Landau
     */
    class GAUDI_API Argus : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::Argus, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters
      Argus
      ( const char*           name      , 
        const char*           title     ,
        RooAbsReal&           x         , 
        RooAbsReal&           shape     ,
        RooAbsReal&           high      ,
        RooAbsReal&           low       ) ;
      /// "copy constructor"
      Argus
      ( const Argus&          right     , 
        const char*           name  = 0 )  ;
      /// destructor 
      virtual ~Argus  () ;
      /// clone 
      virtual  Argus* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Argus& function() const { return m_argus ; }
      // ======================================================================
    protected: 
      // ======================================================================
      RooRealProxy m_x        ;
      RooRealProxy m_shape    ;
      RooRealProxy m_high     ;
      RooRealProxy m_low      ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      mutable Gaudi::Math::Argus m_argus ; // the actual function
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Tsallis  
     *  Useful function to describe pT-spectra of particles 
     *
     *  - C. Tsallis, 
     *  "Possible generalization of Boltzmann-Gibbs statistics,
     *  J. Statist. Phys. 52 (1988) 479.
     *  - C. Tsallis, 
     *  Nonextensive statistics: theoretical, experimental and computational 
     *  evidences and connections, Braz. J. Phys. 29 (1999) 1.
     * 
     *  \f[ \frac{d\sigma}{dp_T} \propto  
     *    p_T\times \left( 1 + \frac{E_{kin}}{Tn}\right)^{-n}\f],
     *  where \f$E_{kin} = \sqrt{p_T^2-M^2}-M\f$ 
     *  is transverse kinetic energy 
     *
     *  @see Gaudi::Math::Tsallis
     *  @author Vanya BElyaev Ivan.Belyaev@itep.ru
     *  @date 2015-07-11
     */ 
    class GAUDI_API Tsallis : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::Tsallis, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters
      Tsallis
        ( const char*           name      , 
          const char*           title     ,
          RooAbsReal&           x         , 
          RooAbsReal&           n         ,   // parameter N 
          RooAbsReal&           T         ,   // parameter T
          RooAbsReal&           mass      ) ; // particle mass (fixed)
      /// "copy constructor"
      Tsallis 
        ( const Tsallis&        right     , 
          const char*           name  = 0 )  ;
      /// destructor 
      virtual ~Tsallis  () ;
      /// clone 
      virtual  Tsallis* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Tsallis& function() const { return m_tsallis ; }
      // ======================================================================
    protected: 
      // ======================================================================
      RooRealProxy m_x        ;
      RooRealProxy m_n        ;
      RooRealProxy m_T        ;
      RooRealProxy m_mass     ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      mutable Gaudi::Math::Tsallis m_tsallis ; // the actual function
      // ======================================================================
    } ;
    // ========================================================================
    /** @class QGSM 
     *  Useful function to describe pT-spectra of particles 
     *
     * - A. B. Kaidalov and O. I. Piskunova, Z. Phys. C 30 (1986) 145.
     * - O. I. Piskounova, arXiv:1301.6539 [hep-ph]; 
     * - O. I. Piskounova, arXiv:1405.4398 [hep-ph].
     * - A. A. Bylinkin and O. I. Piskounova, 
     *  "Transverse momentum distributions of baryons at LHC energies",
     *  arXiv:1501.07706.
     *
     *  \f[ \frac{d\sigma}{dp_T} \propto 
     *  p_T \times \mathrm{e}^{ -b_0 (m_T-m)} \f], 
     *  where transverse mass is defined as \f$m_T = \sqrt{p_T^2+m^2}\f$
     *
     *  @see Gaudi::Math::QGSM
     *  @author Vanya BElyaev Ivan.Belyaev@itep.ru
     *  @date 2015-07-11
     */ 
    class GAUDI_API QGSM : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::QGSM, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters
      QGSM
        ( const char*           name      , 
          const char*           title     ,
          RooAbsReal&           x         , 
          RooAbsReal&           b         ,   // parameter b 
          RooAbsReal&           mass      ) ; // particle mass (fixed)
      /// "copy constructor"
      QGSM 
        ( const QGSM&           right     , 
          const char*           name  = 0 )  ;
      /// destructor 
      virtual ~QGSM  () ;
      /// clone 
      virtual  QGSM* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::QGSM& function() const { return m_qgsm ; }
      // ======================================================================
    protected: 
      // ======================================================================
      RooRealProxy m_x        ;
      RooRealProxy m_b        ;
      RooRealProxy m_mass     ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      mutable Gaudi::Math::QGSM m_qgsm ; // the actual function
      // ======================================================================
    } ;
    // ========================================================================
    /** @class TwoExpos
     *  simple difference of two exponents
     *  \f$ f \propto 
     *        \mathrm{e}^{-a_1    x}       -\mathrm{e}^{-a_2 x} = 
     *        \mathrm{e}^{-\alpha x}\left(1-\mathrm{e}^{-\delta x}\right) \f$
     *  @author Vanya BElyaev Ivan.Belyaev@itep.ru
     *  @date 2015-02-07
     */
    class GAUDI_API TwoExpos : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::TwoExpos, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters
      TwoExpos 
        ( const char*           name  , 
          const char*           title ,
          RooAbsReal&           x     , 
          RooAbsReal&           alpha ,
          RooAbsReal&           delta ,
          RooAbsReal&           x0    ) ;
      /// "copy constructor"
      TwoExpos
        ( const TwoExpos&          right     , 
          const char*           name  = 0 )  ;
      /// destructor 
      virtual ~TwoExpos () ;
      /// clone 
      virtual  TwoExpos* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public: // integrals  
      // ======================================================================      
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         ,  
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::TwoExpos& function() const { return m_2expos ; }
      // ======================================================================
    protected: 
      // ======================================================================
      RooRealProxy m_x     ;
      RooRealProxy m_alpha ;
      RooRealProxy m_delta ;
      RooRealProxy m_x0    ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      mutable Gaudi::Math::TwoExpos m_2expos ; // the actual function
      // ======================================================================
    } ;
    // ========================================================================

    // ========================================================================
    // 1D-splines 
    // ========================================================================

    // ========================================================================
    /** @class PositiveSpline
     *  The special spline for non-negative function
     *  Actually it is a sum of M-splines with non-negative coefficients 
     *  \f$ f(x) = \sum_i \alpha_i * M_i^k(x) \f$,
     *  with constraints  \f$  \sum_i \alpha_i=1\f$ 
     *  and \f$ 0 \le \alpha_i\f$.
     *  @see http://en.wikipedia.org/wiki/M-spline
     *  @see http://en.wikipedia.org/wiki/B-spline
     *  @see Gaudi::Math::PositiveSpline
     */
    class GAUDI_API  PositiveSpline : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::PositiveSpline, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /** constructor with the spline 
       *  @param name  the name 
       *  @param title the  title
       *  @param x     the  variable 
       *  @param spine the spline  
       *  @param phis  vector of parameters 
       */
      PositiveSpline 
        ( const char*                        name, 
          const char*                        title     ,
          RooAbsReal&                        x         ,
          const Gaudi::Math::PositiveSpline& spline    ,   // the spline 
          RooArgList&                        phis      ) ; // parameters
      /// copy
      PositiveSpline
        ( const PositiveSpline& right     , 
          const char*           name = 0  ) ;
      /// destructor 
      virtual ~PositiveSpline() ;
      /// clone 
      virtual  PositiveSpline* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public:  // integrals 
      // ======================================================================
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         , 
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::PositiveSpline& function() const { return m_spline ; }
      const Gaudi::Math::PositiveSpline& spline  () const { return m_spline ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x    ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::PositiveSpline m_spline ;            // the function 
      // ======================================================================
    };
    // ========================================================================
    /** @class MonothonicSpline
     *  The special spline for non-negative monothonic function
     *  @see http://en.wikipedia.org/wiki/I-spline
     *  @see http://en.wikipedia.org/wiki/M-spline
     *  @see http://en.wikipedia.org/wiki/B-spline
     *  @see Gaudi::Math::PositiveSpline
     */
    class GAUDI_API  MonothonicSpline : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::MonothonicSpline, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /** constructor with the spline 
       *  @param name  the name 
       *  @param title the  title
       *  @param x     the  variable 
       *  @param spine the spline  
       *  @param phis  vector of parameters 
       */
      MonothonicSpline 
        ( const char*                          name, 
          const char*                          title     ,
          RooAbsReal&                          x         ,
          const Gaudi::Math::MonothonicSpline& spline    ,   // the spline 
          RooArgList&                          phis      ) ; // parameters
      /// copy
      MonothonicSpline
        ( const MonothonicSpline& right     , 
          const char*             name = 0  ) ;
      /// destructor 
      virtual ~MonothonicSpline() ;
      /// clone 
      virtual  MonothonicSpline* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public:  // integrals 
      // ======================================================================
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         , 
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::MonothonicSpline& function() const { return m_spline ; }
      const Gaudi::Math::MonothonicSpline& spline  () const { return m_spline ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x    ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::MonothonicSpline m_spline ;          // the function 
      // ======================================================================
    };
    // ========================================================================
    /** @class MonothonicSpline
     *  The special spline for non-negative monothonic 
     convex or concave function 
     *  @see Gaudi::Math::PositiveSpline
     */
    class GAUDI_API  ConvexSpline : public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::ConvexSpline, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /** constructor with the spline 
       *  @param name  the name 
       *  @param title the  title
       *  @param x     the  variable 
       *  @param spine the spline  
       *  @param phis  vector of parameters 
       */
      ConvexSpline 
        ( const char*                      name, 
          const char*                      title     ,
          RooAbsReal&                      x         ,
          const Gaudi::Math::ConvexSpline& spline    ,   // the spline 
          RooArgList&                      phis      ) ; // parameters
      /// copy
      ConvexSpline
        ( const ConvexSpline& right     , 
          const char*         name = 0  ) ;
      /// destructor 
      virtual ~ConvexSpline() ;
      /// clone 
      virtual  ConvexSpline* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public:  // integrals 
      // ======================================================================
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         , 
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::ConvexSpline& function() const { return m_spline ; }
      const Gaudi::Math::ConvexSpline& spline  () const { return m_spline ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x    ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::ConvexSpline m_spline ;          // the function 
      // ======================================================================
    };


    // ========================================================================
    // 2D non-factorizable models  
    // ========================================================================

    // ========================================================================
    /** @class Poly2DPositive
     *  Poly2DPositive polynomial
     *  @see Gaudi::Math::Positive2D
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2011-05-25
     */
    class GAUDI_API Poly2DPositive: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::Poly2DPositive, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// linear 
      Poly2DPositive
      ( const char*          name      , 
        const char*          title     ,
        RooRealVar&          x         ,
        RooRealVar&          y         ,
        const unsigned short nX        , 
        const unsigned short nY        , 
        RooArgList&          phis      ) ; // at least (n+1)*(n+2)-1 elements 
      /// copy
      Poly2DPositive
      ( const Poly2DPositive&     right     , 
        const char*          name = 0  ) ;
      /// destructor 
      virtual ~Poly2DPositive() ;
      /// clone 
      virtual  Poly2DPositive* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public:  // integrals 
      // ======================================================================
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         , 
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Positive2D& function() const { return m_positive ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x    ;
      RooRealProxy m_y    ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Positive2D m_positive ;              // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Poly2DSymPositive
     *  Poly2DSymPositive polynomial
     *  @see Gaudi::Math::Positive2D
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2011-05-25
     */
    class GAUDI_API Poly2DSymPositive: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::Poly2DSymPositive, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// linear 
      Poly2DSymPositive
      ( const char*          name      , 
        const char*          title     ,
        RooRealVar&          x         ,
        RooRealVar&          y         ,
        const unsigned short n         , 
        RooArgList&          phis      ) ; // at least (nX+1)*(nY+1)-1 elements 
      /// copy
      Poly2DSymPositive
      ( const Poly2DSymPositive&     right     , 
        const char*          name = 0  ) ;
      /// destructor 
      virtual ~Poly2DSymPositive() ;
      /// clone 
      virtual  Poly2DSymPositive* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public:  // integrals 
      // ======================================================================
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         , 
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Positive2DSym& function() const { return m_positive ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x    ;
      RooRealProxy m_y    ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Positive2DSym m_positive ;           // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PS2DPol
     * 
     *  F(x,y) = PS(x)*PS(y)*PPOL(x,y)
     *
     *  @see Gaudi::Math::PS2DPol 
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2011-05-25
     */
    class GAUDI_API PS2DPol: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::PS2DPol, 2) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor  
      PS2DPol
        ( const char*                      name      , 
          const char*                      title     ,
          RooRealVar&                      x         ,
          RooRealVar&                      y         ,
          const Gaudi::Math::PhaseSpaceNL& psx       , 
          const Gaudi::Math::PhaseSpaceNL& psy       ,
          const unsigned short nX                    , 
          const unsigned short nY                    , 
          RooArgList&          phis      ) ; // at least (nX+1)*(nY+1)-1 elements 
      /// constructor  
      PS2DPol
        ( const char*                      name      , 
          const char*                      title     ,
          RooRealVar&                      x         ,
          RooRealVar&                      y         ,
          const Gaudi::Math::PS2DPol&      ps        , 
          RooArgList&          phis      ) ; // at least (nX+1)*(nY+1)-1 elements 
      /// copy
      PS2DPol
        ( const PS2DPol&       right     , 
          const char*          name = 0  ) ;
      /// destructor 
      virtual ~PS2DPol() ;
      /// clone 
      virtual  PS2DPol* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public:  // integrals 
      // ======================================================================
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         , 
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function(s) 
      const Gaudi::Math::PS2DPol&      function    () const { return m_function ; }
      const Gaudi::Math::Positive2D&   positive    () const { return m_function.positive   () ; }
      const Gaudi::Math::Positive2D&   polynom     () const { return m_function.positive   () ; }
      const Gaudi::Math::PhaseSpaceNL& psX         () const { return m_function.phasespaceX() ; }
      const Gaudi::Math::PhaseSpaceNL& psY         () const { return m_function.phasespaceY() ; }
      const Gaudi::Math::PhaseSpaceNL& phasespaceX () const { return psX () ; }
      const Gaudi::Math::PhaseSpaceNL& phasespaceY () const { return psY () ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x    ;
      RooRealProxy m_y    ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual functions()
      mutable Gaudi::Math::PS2DPol m_function ;              // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PS2DPolSym
     * 
     *  F(x,y) = PS(x)*PS(y)*PPOL(x,y)
     *
     *  @see Gaudi::Math::PS2DPolSym 
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2011-05-25
     */
    class GAUDI_API PS2DPolSym: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::PS2DPolSym, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor  
      PS2DPolSym
        ( const char*                      name      , 
          const char*                      title     ,
          RooRealVar&                      x         ,
          RooRealVar&                      y         ,
          const Gaudi::Math::PhaseSpaceNL& ps        , 
          const unsigned short N                    , 
          RooArgList&          phis      ) ; // at least (nX+1)*(nY+1)-1 elements 
      /// constructor  
      PS2DPolSym
        ( const char*                      name      , 
          const char*                      title     ,
          RooRealVar&                      x         ,
          RooRealVar&                      y         ,
          const Gaudi::Math::PS2DPolSym&   ps        , 
          RooArgList&          phis      ) ; // at least (nX+1)*(nY+1)-1 elements 
      /// copy
      PS2DPolSym
        ( const PS2DPolSym&    right     , 
          const char*          name = 0  ) ;
      /// destructor 
      virtual ~PS2DPolSym() ;
      /// clone 
      virtual  PS2DPolSym* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public:  // integrals 
      // ======================================================================
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         , 
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function(s) 
      const Gaudi::Math::PS2DPolSym&    function    () const { return m_function ; }
      const Gaudi::Math::Positive2DSym& positive    () const { return m_function.positive   () ; }
      const Gaudi::Math::Positive2DSym& polynom     () const { return m_function.positive   () ; }
      const Gaudi::Math::PhaseSpaceNL&  psX         () const { return m_function.phasespaceX() ; }
      const Gaudi::Math::PhaseSpaceNL&  psY         () const { return m_function.phasespaceY() ; }
      const Gaudi::Math::PhaseSpaceNL&  phasespaceX () const { return psX () ; }
      const Gaudi::Math::PhaseSpaceNL&  phasespaceY () const { return psY () ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x    ;
      RooRealProxy m_y    ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual functions()
      mutable Gaudi::Math::PS2DPolSym m_function ;              // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ExpoPS2DPol
     * 
     *  F(x,y) = exp(x)*PS(y)*PPOL(x,y)
     *
     *  @see Gaudi::Math::ExpoPS2DPol 
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2011-05-25
     */
    class GAUDI_API ExpoPS2DPol: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::ExpoPS2DPol, 2) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor  
      ExpoPS2DPol
        ( const char*                      name      , 
          const char*                      title     ,
          RooRealVar&                      x         ,
          RooRealVar&                      y         ,
          RooAbsReal&                      tau       ,
          const Gaudi::Math::PhaseSpaceNL& psy       ,
          const unsigned short nX                    , 
          const unsigned short nY                    , 
          RooArgList&          phis      ) ; // at least (nX+1)*(nY+1)-1 elements 
      /// constructor  
      ExpoPS2DPol
        ( const char*                      name      , 
          const char*                      title     ,
          RooRealVar&                      x         ,
          RooRealVar&                      y         ,
          RooAbsReal&                      tau       ,
          const Gaudi::Math::ExpoPS2DPol&  ps        , 
          RooArgList&          phis      ) ; // at least (nX+1)*(nY+1)-1 elements 
      /// copy
      ExpoPS2DPol
        ( const ExpoPS2DPol&   right     , 
          const char*          name = 0  ) ;
      /// destructor 
      virtual ~ExpoPS2DPol() ;
      /// clone 
      virtual  ExpoPS2DPol* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public:  // integrals 
      // ======================================================================
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         , 
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function(s) 
      const Gaudi::Math::ExpoPS2DPol&  function    () const { return m_function ; }
      const Gaudi::Math::Positive2D&   positive    () const { return m_function.positive   () ; }
      const Gaudi::Math::Positive2D&   polynom     () const { return m_function.positive   () ; }
      const Gaudi::Math::PhaseSpaceNL& psY         () const { return m_function.phasespaceY() ; }
      const Gaudi::Math::PhaseSpaceNL& phasespaceY () const { return psY () ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x    ;
      RooRealProxy m_y    ;
      RooRealProxy m_tau  ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual functions()
      mutable Gaudi::Math::ExpoPS2DPol m_function ;             // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Expo2DPol
     * 
     *  F(x,y) = exp(x)*exp(y)*PPOL(x,y)
     *
     *  @see Gaudi::Math::ExpoPS2DPol 
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2011-05-25
     */
    class GAUDI_API Expo2DPol: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::Expo2DPol, 2) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor  
      Expo2DPol
        ( const char*                      name      , 
          const char*                      title     ,
          RooRealVar&                      x         ,
          RooRealVar&                      y         ,
          RooAbsReal&                      taux      ,
          RooAbsReal&                      tauy      ,
          const unsigned short nX                    , 
          const unsigned short nY                    , 
          RooArgList&          phis      ) ; // at least (nX+1)*(nY+1)-1 elements 
      /// copy
      Expo2DPol
        ( const Expo2DPol&     right     , 
          const char*          name = 0  ) ;
      /// destructor 
      virtual ~Expo2DPol() ;
      /// clone 
      virtual  Expo2DPol* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public:  // integrals 
      // ======================================================================
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         , 
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function(s) 
      const Gaudi::Math::Expo2DPol&    function    () const { return m_function ; }
      const Gaudi::Math::Positive2D&   positive    () const { return m_function.positive   () ; }
      const Gaudi::Math::Positive2D&   polynom     () const { return m_function.positive   () ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x    ;
      RooRealProxy m_y    ;
      RooRealProxy m_taux ;
      RooRealProxy m_tauy ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual functions()
      mutable Gaudi::Math::Expo2DPol m_function ;               // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Expo2DPol
     * 
     *  F(x,y) = exp(x)*exp(y)*SPOL(x,y)
     *
     *  @see Gaudi::Math::Expo2DPolSym 
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2011-05-25
     */
    class GAUDI_API Expo2DPolSym: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::Expo2DPolSym, 2) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor  
      Expo2DPolSym
        ( const char*                      name      , 
          const char*                      title     ,
          RooRealVar&                      x         ,
          RooRealVar&                      y         ,
          RooAbsReal&                      tau       ,
          const unsigned short n                     , 
          RooArgList&          phis      ) ; // at least (nX+1)*(nY+1)-1 elements 
      /// copy
      Expo2DPolSym
        ( const Expo2DPolSym&  right     , 
          const char*          name = 0  ) ;
      /// destructor 
      virtual ~Expo2DPolSym() ;
      /// clone 
      virtual  Expo2DPolSym* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public:  // integrals 
      // ======================================================================
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         , 
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function(s) 
      const Gaudi::Math::Expo2DPolSym&  function    () const { return m_function ; }
      const Gaudi::Math::Positive2DSym& positive    () const { return m_function.positive   () ; }
      const Gaudi::Math::Positive2DSym& polynom     () const { return m_function.positive   () ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x    ;
      RooRealProxy m_y    ;
      RooRealProxy m_tau  ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual functions()
      mutable Gaudi::Math::Expo2DPolSym m_function ;           // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Spline2D
     *  Positive 2D-spline 
     *  @see Gaudi::Math::Spline2D
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2011-05-25
     */
    class GAUDI_API Spline2D: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::Spline2D, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// linear 
      Spline2D
        ( const char*                  name      , 
          const char*                  title     ,
          RooRealVar&                  x         ,
          RooRealVar&                  y         ,
          const Gaudi::Math::Spline2D& spline    ,
          RooArgList&                  phis      ) ; 
      /// copy
      Spline2D
        ( const Spline2D&      right     , 
          const char*          name = 0  ) ;
      /// destructor 
      virtual ~Spline2D() ;
      /// clone 
      virtual  Spline2D* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public:  // integrals 
      // ======================================================================
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         , 
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Spline2D& function() const { return m_spline ; }
      const Gaudi::Math::Spline2D& spline  () const { return m_spline ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x    ;
      RooRealProxy m_y    ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Spline2D m_spline ;                 // the function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Spline2DSym
     *  Positive symmetric 2D-spline 
     *  @see Gaudi::Math::Spline2DSym
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date 2011-05-25
     */
    class GAUDI_API Spline2DSym: public RooAbsPdf 
    {
      // ======================================================================
    public :
      // ======================================================================
      ClassDef(Analysis::Models::Spline2DSym, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// linear 
      Spline2DSym
        ( const char*                     name      , 
          const char*                     title     ,
          RooRealVar&                     x         ,
          RooRealVar&                     y         ,
          const Gaudi::Math::Spline2DSym& spline    ,
          RooArgList&                     phis      ) ; 
      /// copy
      Spline2DSym
        ( const Spline2DSym&   right     , 
          const char*          name = 0  ) ;
      /// destructor 
      virtual ~Spline2DSym() ;
      /// clone 
      virtual  Spline2DSym* clone ( const char* name ) const ; 
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    public:  // integrals 
      // ======================================================================
      virtual Int_t    getAnalyticalIntegral
        ( RooArgSet&     allVars      , 
          RooArgSet&     analVars     ,
          const char* /* rangename */ ) const ;
      virtual Double_t analyticalIntegral 
        ( Int_t          code         , 
          const char*    rangeName    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set all parameters 
      void setPars () const ; // set all parameters 
      // ======================================================================
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Spline2DSym& function() const { return m_spline ; }
      const Gaudi::Math::Spline2DSym& spline  () const { return m_spline ; }
      // ======================================================================
    protected :
      // ======================================================================
      RooRealProxy m_x    ;
      RooRealProxy m_y    ;
      RooListProxy m_phis ;
      // ======================================================================
      TIterator* m_iterator;  //! do not persist
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Spline2DSym m_spline ;               // the function 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                        end of namespace Analysis::Models
  // ==========================================================================
} //                                                  end of namespace Analysis 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // ANALYSIS_MODELS_H
// ============================================================================
