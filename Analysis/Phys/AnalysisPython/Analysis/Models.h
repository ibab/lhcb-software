 // $Id: $
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
#include "RooAbsPdf.h"
#include "RooRealProxy.h"
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
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x     ;
      RooRealProxy m_mass  ;
      RooRealProxy m_width ;
      // ======================================================================
      // the actual evaluation of function 
      Double_t evaluate() const ;
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
      // the actual evaluation of function 
      Double_t evaluate() const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::Flatte m_flatte ;             // the actual function 
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
      // ======================================================================
    protected:
      // ======================================================================
      RooRealProxy m_x     ;
      RooRealProxy m_low   ;
      RooRealProxy m_high  ;
      // ======================================================================
      // the actual evaluation of function 
      Double_t evaluate() const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::PhaseSpaceNL m_ps ;           // the actual function 
      // ======================================================================
    };
    // ========================================================================
    /** @class CrystalBallDS 
     *  double-sided ``Crystal Ball-function'' 
     *  for description of gaussian with the tail
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
      // the actual evaluation of function 
      Double_t evaluate() const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      mutable Gaudi::Math::CrystalBallDoubleSided m_cb2 ;       // the function 
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
