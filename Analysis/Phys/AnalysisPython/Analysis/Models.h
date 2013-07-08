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
   *
   *  @author Vanya BELYAEV  Ivan.Belyaev@cern.ch
   *  @date   2011-11-30
   */
  namespace Models 
  {
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
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
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
                    const Gaudi::Math::BreitWigner::JacksonRho rho ) ;
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
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::BreitWigner& function() const { return m_bw ; }
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
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
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
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
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
      Flatte ( const char*          name      , 
               const char*          title     ,
               RooAbsReal&          x         ,
               RooAbsReal&          m0        ,
               RooAbsReal&          m0g1      ,
               RooAbsReal&          g2og1     ,
               const double         k_mass    , 
               const double         pi_mass   ) ;
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
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Flatte& function() const { return m_flatte ; }
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x     ;
      RooRealProxy m_m0    ;
      RooRealProxy m_m0g1  ;
      RooRealProxy m_g2og1 ;
      double       m_K     ;
      double       m_pi    ;
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
     *  KK-channel
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2011-11-30
     */
    class GAUDI_API Flatte2 : public Analysis::Models::Flatte
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::Flatte2, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters 
      Flatte2 ( const char*          name      , 
                const char*          title     ,
                RooAbsReal&          x         ,
                RooAbsReal&          m0        ,
                RooAbsReal&          m0g1      ,
                RooAbsReal&          g2og1     ,
                const double         k_mass    , 
                const double         pi_mass   ) ;
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
    } ;
    // ========================================================================
    /** @class PhaseSpace2
     *  simple model for 2-body phase space 
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
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
    public:
      // ======================================================================
      /// access to underlying function 
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
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
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
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
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
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
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
     *  @see Gaudi::Math::PhaseSpaceNL
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
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
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::LASS23L& function() const { return m_lass ; }
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
    public:
      // ======================================================================
      /// get the complex amplitude 
      std::complex<double> amplitude() const ; // get the complex amplitude 
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::LASS23L m_lass ;              // the actual function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Needham
     *  The special parametrization by Matthew NEEDHAM of 
     *  ``Crystal Ball-function'' nicely suitable for \f$J/\psi\f$-peak
     *  @thank Matthew Needham 
     *  @see Gaudi::Math::Needham
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
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
      /// constructor from all parameters 
      Needham
      ( const char*          name             , 
        const char*          title            ,
        RooAbsReal&          x                ,
        RooAbsReal&          m0               ,
        RooAbsReal&          sigma            ,  
        const double         a0    =  1.975   ,   
        const double         a1    =  0.0011  ,  
        const double         a2    = -0.00018 ) ;
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
    public:
      // ======================================================================
      /// access to underlying function 
      const Gaudi::Math::Needham& function() const { return m_needham ; }
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
      bool   m_fixed ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Needham m_needham ;                  // the function 
      // ======================================================================
    } ;    
    // ========================================================================
    /** @class CrystalBallDS 
     *  double-sided ``Crystal Ball-function'' 
     *  for description of gaussian with the tail
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
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
        RooAbsReal&          alphaL    ,  // alpha_L - 1   
        RooAbsReal&          nL        ,  //     n_L - 1   
        RooAbsReal&          alphaR    ,  // alpha_R - 1   
        RooAbsReal&          nR        ); //     n_R - 1 
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
    /** @class GramCharlierA
     *  The peak with Gram-Charlier type A parameterization
     *  @see Gaudi::Math::GramCharlierA 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
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
    /** @class Bukin
     *  "Bukin"-function
     *  @see Gaudi::Math::Bukin
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
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
        RooAbsReal&          peak      , 
        RooAbsReal&          sigma     , 
        RooAbsReal&          xi        ,
        RooAbsReal&          rhoL      ,
        RooAbsReal&          rhoR      ) ;
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
    /** @class Voigt
     *  "Voigt"-function
     *  @see Gaudi::Math::Voigt
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
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
      /// linear 
      PolyPositive
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        RooAbsReal&          phi1      , 
        const double         xmin      , 
        const double         xmax      ) ;
      /// quadric
      PolyPositive
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        RooAbsReal&          phi1      , 
        RooAbsReal&          phi2      , 
        const double         xmin      , 
        const double         xmax      ) ;
      /// cubic 
      PolyPositive
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        RooAbsReal&          phi1      , 
        RooAbsReal&          phi2      , 
        RooAbsReal&          phi3      , 
        const double         xmin      , 
        const double         xmax      ) ;
      /// quartic
      PolyPositive
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        RooAbsReal&          phi1      , 
        RooAbsReal&          phi2      , 
        RooAbsReal&          phi3      , 
        RooAbsReal&          phi4      , 
        const double         xmin      , 
        const double         xmax      ) ;
      /// quintic 
      PolyPositive
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        RooAbsReal&          phi1      , 
        RooAbsReal&          phi2      , 
        RooAbsReal&          phi3      , 
        RooAbsReal&          phi4      , 
        RooAbsReal&          phi5      , 
        const double         xmin      , 
        const double         xmax      ) ;
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
      /// linear 
      ExpoPositive
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        RooAbsReal&          tau       , 
        RooAbsReal&          phi1      , 
        const double         xmin      , 
        const double         xmax      ) ;
      /// quadric
      ExpoPositive
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        RooAbsReal&          tau       , 
        RooAbsReal&          phi1      , 
        RooAbsReal&          phi2      , 
        const double         xmin      , 
        const double         xmax      ) ;
      /// cubic 
      ExpoPositive
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        RooAbsReal&          tau       , 
        RooAbsReal&          phi1      , 
        RooAbsReal&          phi2      , 
        RooAbsReal&          phi3      , 
        const double         xmin      , 
        const double         xmax      ) ;
      /// quartic
      ExpoPositive
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        RooAbsReal&          tau       , 
        RooAbsReal&          phi1      , 
        RooAbsReal&          phi2      , 
        RooAbsReal&          phi3      , 
        RooAbsReal&          phi4      , 
        const double         xmin      , 
        const double         xmax      ) ;
      /// quintic 
      ExpoPositive
      ( const char*          name      , 
        const char*          title     ,
        RooAbsReal&          x         ,
        RooAbsReal&          tau       , 
        RooAbsReal&          phi1      , 
        RooAbsReal&          phi2      , 
        RooAbsReal&          phi3      , 
        RooAbsReal&          phi4      , 
        RooAbsReal&          phi5      , 
        const double         xmin      , 
        const double         xmax      ) ;
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
      mutable Gaudi::Math::Positive m_positive ;               // the function 
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
    /** @class Product 
     *  helper utility for building run-time PDFs.
     *  In many aspects it is similar to RooProduct
     *  @see RooProduct
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2013-06-08
     */
    class GAUDI_API Product : public RooAbsPdf 
    {
    public:
      // ======================================================================
      ClassDef(Analysis::Models::Product, 1) ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from all parameters
      Product ( const char* name  , 
                const char* title ,
                RooAbsPdf&  pdf1  , 
                RooAbsPdf&  pdf2  ) ;
      /// "copy constructor"
      Product ( const Product& right    , 
                const char*    name = 0 ) ;
      /// virtual destructor 
      virtual ~Product() ;
      // clone 
      virtual Product* clone ( const char* name ) const ;
      // ======================================================================
    public:
      // ======================================================================
      // the actual evaluation of function 
      virtual Double_t evaluate() const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the first  pdf 
      RooRealProxy m_pdf1 ;  // the first  pdf 
      /// the second pdf 
      RooRealProxy m_pdf2 ;  // the second pdf 
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
