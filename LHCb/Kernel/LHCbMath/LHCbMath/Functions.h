// $Id$
// ============================================================================
#ifndef LHCBMATH_FUNCTIONS_H 
#define LHCBMATH_FUNCTIONS_H 1
// ============================================================================
// include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
/** @file LHCbMath/Functions.h
 *
 *  set of useful math-functions
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-04-19
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $author$
 */
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Math
  {
    // ========================================================================
    //  Chebyshev 
    // ========================================================================
    template <unsigned int N> class  Chebyshev_ ;
    // ========================================================================
    /** @class Chebychev_
     *  Efficienct evaluator of Chebyshev polynomial
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-04-19
     */
    template <unsigned int N> 
    class  Chebyshev_ : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /// the only one important method 
      inline double operator() ( const double    x    ) const
      { return 2 * x * m_N1 ( x ) - m_N2 ( x ) ; }
      // ======================================================================
    private:
      // ======================================================================
      /// T (N-1)
      Chebyshev_<N-1>  m_N1 ;                                        // T (N-1)
      /// T (N-2)
      Chebyshev_<N-2>  m_N2 ;                                        // T (N-2)
      // ======================================================================
    } ;
    // ========================================================================
    /// specialization for 0 
    template <> 
    class  Chebyshev_<0> : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      inline double operator() ( const double /* x */ ) const { return    1 ; }
      // ======================================================================
    } ;
    // ========================================================================
    /// specialization for N=1
    template <> 
    class  Chebyshev_<1> : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /// the only one important method 
      inline double operator() ( const double    x    ) const { return   x ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Chebyshev 
     *  evaluate the chebyshev polynomials 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-04-19
     */
    class GAUDI_API Chebyshev : public std::unary_function<double,double>
    {
    public :
      // ======================================================================
      /// constructor 
      Chebyshev ( const unsigned int N ) : m_N ( N ) {}
      // ======================================================================
    public:
      // ======================================================================      
      /// evaluate the polynomial 
      double operator() ( const double x ) const ;
      // ======================================================================      
    public:
      // ======================================================================      
      unsigned int order() const { return m_N ; }
      // ======================================================================      
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Chebyshev () ;                     // the default constructor is disabled 
      // ======================================================================      
    private:
      // ======================================================================      
      unsigned int m_N ;
      // ======================================================================      
    } ;
    // ========================================================================
    // Legendre 
    // ========================================================================
    template <unsigned int N> class  Legendre_ ;
    // ========================================================================
    /** @class Legendre_
     *  Efficienct evaluator of Legendre polynomial
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-04-19
     */
    template <unsigned int N> 
    class  Legendre_ : public std::unary_function<double,double>
    {
    private:
      // ======================================================================
      enum 
        {
          A = 2 * N - 1 , 
          B =     N - 1 
        } ;
      // =======================================================================
    public:
      // ======================================================================
      /// the only one important method 
      inline double operator() ( const double    x    ) const
      { return ( A * x * m_N1 ( x ) - B * m_N2 ( x ) ) / N ; }
      // ======================================================================
    private:
      // ======================================================================
      /// L (N-1)
      Legendre_<N-1>  m_N1 ;                                         // L (N-1)
      /// L (N-2)
      Legendre_<N-2>  m_N2 ;                                         // L (N-2)
      // ======================================================================
    } ;
    // ========================================================================
    /// specialization for 0 
    template <> 
    class  Legendre_<0> : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /// the only one important method 
      inline double operator() ( const double /* x */ ) const { return    1 ; }
      // ======================================================================
    } ;
    // ========================================================================
    /// specialization for N=1
    template <> 
    class  Legendre_<1> : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /// the only one important method 
      inline double operator() ( const double    x    ) const { return   x ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Legendre
     *  evaluate the Legendre polynomials 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-04-19
     */
    class GAUDI_API Legendre : public std::unary_function<double,double>
    {
    public :
      // ======================================================================
      /// constructor 
      Legendre ( const unsigned int N ) : m_N ( N ) {}
      // ======================================================================
    public:
      // ======================================================================      
      /// evaluate the polynomial 
      double operator() ( const double x ) const ;
      // ======================================================================      
    public:
      // ======================================================================      
      unsigned int order() const { return m_N ; }
      // ======================================================================      
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Legendre () ;                      // the default constructor is disabled 
      // ======================================================================      
    private:
      // ======================================================================      
      unsigned int m_N ;
      // ======================================================================      
    } ;
    // ========================================================================
    // Hermite 
    // ========================================================================
    template <unsigned int N> class  Hermite_ ;
    // ========================================================================
    /** @class Hermite_
     *  Efficienct evaluator of Hermite polynomial
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-04-19
     */
    template <unsigned int N> 
    class  Hermite_ : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /// the only one important method 
      inline double operator() ( const double    x    ) const
      { return x * m_N1 ( x ) - ( N - 1 ) * m_N2 ( x ) ; }
      // ======================================================================
    private:
      // ======================================================================
      /// H (N-1)
      Hermite_<N-1>  m_N1 ;                                        // H (N-1)
      /// H (N-2)
      Hermite_<N-2>  m_N2 ;                                        // H (N-2)
      // ======================================================================
    } ;
    // ========================================================================
    /// specialization for 0 
    template <> 
    class  Hermite_<0> : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      inline double operator() ( const double /* x */ ) const { return 1 ; }
      // ======================================================================
    } ;
    // ========================================================================
    /// specialization for N=1
    template <> 
    class  Hermite_<1> : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /// the only one important method 
      inline double operator() ( const double    x    ) const { return   x ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Hermite
     *  evaluate the Hermite polynomials 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-04-19
     */
    class GAUDI_API Hermite : public std::unary_function<double,double>
    {
    public :
      // ======================================================================
      /// constructor 
      Hermite ( const unsigned int N ) : m_N ( N ) {}
      // ======================================================================
    public:
      // ======================================================================      
      /// evaluate the polynomial 
      double operator() ( const double x ) const ;
      // ======================================================================      
    public:
      // ======================================================================      
      unsigned int order() const { return m_N ; }
      // ======================================================================      
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Hermite () ;                      // the default constructor is disabled 
      // ======================================================================      
    private:
      // ======================================================================      
      unsigned int m_N ;
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class BifurcatedGauss 
     *  @date 2011-04-19
     */
    class GAUDI_API BifurcatedGauss : public std::unary_function<double,double>
    {
      // ======================================================================
    public:
      // ======================================================================
      /** constructor from all parameters 
       *  @param peak    the peak posiion 
       *  @param sigma_L (left sigma)
       *  @param sigma_R (right sigma)
       */
      BifurcatedGauss 
      ( const double peak       , 
        const double sigma_L    , 
        const double sigma_R    ) ;
      // ======================================================================
      /// destructor 
      ~BifurcatedGauss() ;
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate Bifurcated Gaussian 
      double operator() ( const double x ) const ;
      // ====================================================================== 
    public:
      // ====================================================================== 
      double peak    () const { return m_peak    ; }
      double m0      () const { return peak()    ; }
      double sigma_L () const { return m_sigma_L ; }
      double sigma_R () const { return m_sigma_R ; }
      // ====================================================================== 
    public:
      // ====================================================================== 
      void setPeak    ( const double value ) { m_peak   = value  ; }
      void setM0      ( const double value ) { setPeak ( value ) ; }
      void setSigma_L ( const double value ) ;
      void setSigma_R ( const double value ) ;
      // ====================================================================== 
    public:
      // ====================================================================== 
      /// get the integral 
      double integral () const ;
      /// get the integral between low and high limits 
      double integral ( const double low  , 
                        const double high ) const ;
      // ====================================================================== 
    private:
      // ======================================================================
      /// default constructor is disabled 
      BifurcatedGauss () ;                   // default constructor is disabled 
      // ======================================================================
    private: // parameters
      // ======================================================================
      /// the peak position 
      double m_peak    ;      //                              the peak position 
      /// (left) sigma 
      double m_sigma_L ;      // (left) sigma 
      /// (right) sigma 
      double m_sigma_R ;      // (right) sigma
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Bukin
     *  ``Bukin-function'' 
     *  for description of asymmetric peaks with the exponential tails 
     *  @date 2011-04-19
     */
    class GAUDI_API Bukin : public std::unary_function<double,double>
    {
    public :
      // ======================================================================
      /** constructor from all parameters 
       *  @param peak  the peak posiion 
       *  @param sigma the effective sigma, defined as FWHM/2.35
       *  @param xi    the asymmetry parameter 
       *  @param rhoL  the left  tail parameter
       *  @param rhoR  the right tail parameter
       */
      Bukin 
      ( const double peak       , 
        const double sigma      , 
        const double xi     = 0 ,
        const double rhoL   = 0 , 
        const double rhoR   = 0 ) ;
      // ======================================================================
      /// copy constructor 
      Bukin ( const Bukin& right ) ;
      /// destructor 
      ~Bukin () ;
      // ======================================================================
    public:
      // ====================================================================== 
      /// evaluate Bukin's function 
      double operator() ( const double x ) const ;
      // ====================================================================== 
    public:
      // ====================================================================== 
      double peak  () const { return m_peak    ; }
      double m0    () const { return   peak () ; }
      double sigma () const { return m_sigma   ; }
      double xi    () const { return m_xi      ; }
      double rho_L () const { return m_rho_L   ; }
      double rho_R () const { return m_rho_R   ; }
      // ====================================================================== 
    public:
      // ====================================================================== 
      void setPeak  ( const double value ) ;
      void setM0    ( const double value ) { setPeak ( value ) ; }
      void setSigma ( const double value ) ;
      void setXi    ( const double value ) ;
      void setRho_L ( const double value ) ;
      void setRho_R ( const double value ) ;
      // ====================================================================== 
    public:
      // ====================================================================== 
      /// get the integral 
      double integral () const ;
      /// get the integral between low and high limits 
      double integral ( const double low  , 
                        const double high ) const ;
      // ====================================================================== 
    private:
      // ======================================================================
      /// default constructor is disabled 
      Bukin () ;                             // default constructor is disabled 
      // ======================================================================
    private: // recalculate constants 
      // ======================================================================
      /// recalculate the integral 
      void integrate ()       ;                    // recalculate the integral 
      // ======================================================================
    private: // parameters
      // ======================================================================
      /// the peak position 
      double m_peak    ;      //                              the peak position 
      /// the effective resolution, defined as FWHM/2.35
      double m_sigma   ;      // the effective resolution, defined as FWHM/2.35
      /// the asymmetry parameter 
      double m_xi      ;      //                        the asymmetry parameter 
      /// the left  tail parameter 
      double m_rho_L   ;      //                        the left tail parameter 
      /// the right tail parameter 
      double m_rho_R   ;      //                        the right tail parameter 
      // ======================================================================
    private: // internals 
      // ======================================================================
      /// A/2 -region : left edge 
      double m_x1        ; // A/2 -region : left edge 
      /// A/2 -region : right  edge 
      double m_x2        ; // A/2 -region : right  edge 
      // 
      /// the first magic constant for central region
      double m_A         ; // the first  magic constant for central region
      /// the second magic constant for central region
      double m_B2        ; // the second magic constant for central region
      //
      /// tails parameters (times  Bukin's  constants)
      double m_L         ;   // left  tail  
      double m_R         ;   // right tail  
      /// normalization integral 
      double m_integral  ;   // normalization integral 
      mutable void*  m_workspace ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Novosibitsk
     *  ``Novosibirsk-function'' for description of gaussian with tails
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-04-19
     */
    class GAUDI_API Novosibirsk : public std::unary_function<double,double>
    {
    public :
      // ======================================================================
      /** constructor from all parameters 
       *  @param m0    the peak posiion
       *  @param sigma the effective sigma
       *  @param tau   the tail paramter
       */
      Novosibirsk
      ( const double m0        , 
        const double sigma     , 
        const double tau   = 0 ) ;
      // ======================================================================
      /// copy constructor 
      Novosibirsk ( const Novosibirsk& right ) ;
      /// destructor 
      ~Novosibirsk () ;                                           // destructor 
      // ======================================================================
    public:
      // ====================================================================== 
      /// evaluate Novosibirsk's function 
      double operator() ( const double x ) const ;
      // ====================================================================== 
    public:
      // ====================================================================== 
      double m0    () const { return m_m0       ; }
      double peak  () const { return   m0    () ; }
      double sigma () const { return m_sigma    ; }
      double tau   () const { return m_tau      ; }
      // ====================================================================== 
    public:
      // ====================================================================== 
      void setM0    ( const double value ) ;
      void setPeak  ( const double value ) { setM0 ( value ) ; }
      void setSigma ( const double value ) ;
      void setTau   ( const double value ) ;
      // ====================================================================== 
    public:
      // ====================================================================== 
      /// get the integral 
      double integral () const ;
      /// get the integral between low and high limits 
      double integral ( const double low  , 
                        const double high ) const ;
      // ====================================================================== 
    private:
      // ======================================================================
      /// default constructor is disabled 
      Novosibirsk () ; // default constructor is disabled 
      // ======================================================================
    private: // recalculate constants 
      // ======================================================================
      /// recalculate integral 
      void integrate () ; // recalculate integral 
      /// get parameter lambda  
      void getLambda () ; // get parameter lambda  
      // ======================================================================
    private: // parameters
      // ======================================================================
      /// the peak position 
      double m_m0      ;      //                              the peak position 
      /// the effective resolution
      double m_sigma   ;      //                       the effective resolution
      /// the tail parameter 
      double m_tau     ;      //                             the tail parameter
      // ======================================================================
    private: // internals 
      // ======================================================================
      /// lambda value 
      double m_lambda    ;   // lambda value 
      // integration 
      double m_integral  ;
      mutable void*  m_workspace ;
      // ======================================================================
    } ;
    // ========================================================================
    // Crystal Ball & Co
    // ========================================================================
    /** @class CrystalBall 
     *  ``Crystal Ball-function'' for description of gaussian with the tail
     *  @date 2011-05-25
     */
    class GAUDI_API CrystalBall : public std::unary_function<double,double>     
    {
    public:
      // ======================================================================
      /** constructor from all parameters 
       *  @param m0     m0       parameter 
       *  @param sigma  sigma    parameter 
       *  @param alpha (alpha-1) parameter 
       *  @param n     (N-1)     parameter 
       */
      CrystalBall  
      ( const double m0    , 
        const double sigma , 
        const double alpha , 
        const double N     ) ;
      /// destructor 
      ~CrystalBall() ;
      // ======================================================================     
    public:
      // ====================================================================== 
      /// evaluate CrystalBall's function 
      double operator() ( const double x ) const ;
      // ====================================================================== 
    public: // trivial accessors 
      // ======================================================================  
      double m0    () const { return m_m0    ; }
      double peak  () const { return   m0 () ; }
      double sigma () const { return m_sigma ; }
      double alpha () const { return m_alpha ; }
      double n     () const { return m_N     ; }
      // ======================================================================
    public: // trivial accessors 
      // ======================================================================
      void setM0    ( const double value ) ;
      void setPeak  ( const double value ) { setM0 ( value ) ; }
      void setSigma ( const double value ) ;
      void setAlpha ( const double value ) ;
      void setN     ( const double value ) ;
      // ======================================================================
    public: 
      // ======================================================================
      /// get (possibly trunkated) integral 
      double integral () const ;
      /// get integral between low and high 
      double integral ( const double low , 
                        const double high ) const ;
      // ======================================================================
    private:
      // ====================================================================== 
      /// default constructor is disabled 
      CrystalBall () ;                      // default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// recalculate integral 
      void integrate () ;                               // recalculate integral 
      // ======================================================================
    private:
      // ======================================================================
      /// the peak position 
      double m_m0       ;  // the peak position 
      /// the peak resolution 
      double m_sigma    ;  // the peak resolution 
      /// parameter alpha 
      double m_alpha    ;  // parameter alpha 
      /// parameter N
      double m_N        ;  // parameter N
      /// helper constant
      double m_const    ;
      /// integral  
      double m_integral ;  // the integral  
      // ======================================================================     
    } ;
    // ========================================================================
    /** @class CrystalBallDoubleSided 
     *  ``Crystal Ball-function'' for description of gaussian with the tail
     *  @date 2011-05-25
     */
    class GAUDI_API CrystalBallDoubleSided 
      : public std::unary_function<double,double>     
    {
    public:
      // ======================================================================
      /** constructor from all parameters 
       *  @param m0      m0          parameter 
       *  @param sigma   sigma       parameter 
       *  @param alpha_L (alpha_L-1) parameter 
       *  @param n_L     (n_L-1)     parameter 
       *  @param alpha_R (alpha_R-1) parameter 
       *  @param n_R     (n_R-1)     parameter 
       */
      CrystalBallDoubleSided
      ( const double m0      , 
        const double sigma   , 
        const double alpha_L ,  
        const double n_L     ,
        const double alpha_R , 
        const double n_R     ) ;
      /// destructor 
      ~CrystalBallDoubleSided() ;
      // ======================================================================     
    public:
      // ====================================================================== 
      /// evaluate CrystalBall's function 
      double operator() ( const double x ) const ;
      // ====================================================================== 
    public: // trivial accessors 
      // ======================================================================
      double m0      () const { return m_m0      ; }
      double peak    () const { return   m0 ()   ; }
      double sigma   () const { return m_sigma   ; }
      double alpha_L () const { return m_alpha_L ; }
      double n_L     () const { return m_N_L     ; }
      double alpha_R () const { return m_alpha_R ; }
      double n_R     () const { return m_N_R     ; }
      // ======================================================================
    public: // trivial accessors 
      // ======================================================================    
      void setM0      ( const double value ) ;
      void setPeak    ( const double value ) { setM0 ( value ) ; }
      void setSigma   ( const double value ) ;
      void setAlpha_L ( const double value ) ;
      void setN_L     ( const double value ) ;
      void setAlpha_R ( const double value ) ;
      void setN_R     ( const double value ) ;
      // ======================================================================    
    public: // 
      // ======================================================================
      /// get (possibly trunkated) integral 
      double integral () const ;
      /// get integral between low and high 
      double integral ( const double low , 
                        const double high ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      CrystalBallDoubleSided () ;            // default constructor is disabled 
      // ======================================================================   
    private:
      // ======================================================================
      /// recalculate integral 
      void integrate () ;                               // recalculate integral 
      // ======================================================================
    private:
      // ======================================================================
      /// the peak position 
      double m_m0       ;  // the peak position 
      /// the peak resolution 
      double m_sigma    ;  // the peak resolution 
      /// parameter alpha 
      double m_alpha_L  ;  // parameter alpha 
      /// parameter N
      double m_N_L      ;  // parameter N
      /// parameter alpha_R
      double m_alpha_R  ;  // parameter alpha 
      /// parameter N_R
      double m_N_R      ;  // parameter N
      /// 
      double m_const_L  ;
      double m_const_R  ;      
      /// integral  
      double m_integral ;  // the integral  
      //  
    } ;
    // ========================================================================
    /** @class GramCharlierA4
     *  Gram-Charlier type A approximation  
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-06-13
     */
    class GAUDI_API GramCharlierA 
      : public std::unary_function<double,double>     
    {
    public:
      // ======================================================================
      /** constructor from all parameters
       *  @param mean   the mean value for districbution 
       *  @param sigma  the sigma 
       *  @param kappa3 the standartized 3rd cumulant 
       *  @param kappa4 the standartized 4th cumulant 
       */
      GramCharlierA  ( const double mean   , 
                       const double sigma  , 
                       const double kappa3 , 
                       const double kappa4 ) ;
      /// copy constructor 
      GramCharlierA  ( const GramCharlierA& right ) ;
      /// destructor 
      ~GramCharlierA () ;
      // ======================================================================     
    public:
      // ====================================================================== 
      /// evaluate Gram-Charlier type A approximation 
      double operator () ( const double x ) const ;
      // ====================================================================== 
    public: // trivial accessors 
      // ======================================================================
      double   mean   () const { return m_mean          ; }
      double   m0     () const { return   mean   ()     ; }
      double   peak   () const { return   mean   ()     ; }
      double   sigma  () const { return m_sigma         ; }
      double   kappa3 () const { return m_kappa3        ; }
      double   kappa4 () const { return m_kappa4        ; }
      // ======================================================================
    public: // trivial accessors 
      // ======================================================================    
      void setM0      ( const double value ) { m_mean = value  ; }
      void setMean    ( const double value ) { setM0 ( value ) ; }
      void setPeak    ( const double value ) { setM0 ( value ) ; }
      //
      void setSigma   ( const double value ) ;
      void setKappa3  ( const double value ) ;
      void setKappa4  ( const double value ) ;
      // ======================================================================    
    private:
      // ======================================================================
      /// default constructor is disabled 
      GramCharlierA () ;                     // default constructor is disabled 
      // ======================================================================   
    public: // 
      // ======================================================================
      /// get (possibly trunkated) integral 
      double integral () const ;
      /// get integral between low and high 
      double integral ( const double low , 
                        const double high ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// mean value 
      double              m_mean   ;           //         mean 
      /// rms 
      double              m_sigma  ;           //          rms
      /// the standartized 3rd cumulant 
      double              m_kappa3 ;           // the standartized 3rd cumulant 
      /// the standartized 4th cumulant 
      double              m_kappa4 ;           // the standartized 4th cumulant 
      // ======================================================================
      // integration 
      mutable void*  m_workspace ;
      // ======================================================================
    } ;
    // ========================================================================
  } //                                             end of namespace Gaudi::Math
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LHCBMATH_FUNCTIONS_H
// ============================================================================
