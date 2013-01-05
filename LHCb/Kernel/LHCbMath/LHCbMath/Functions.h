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
#include <vector>
#include <complex>
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
     *  Efficient evaluator of Chebyshev polynomial
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
    /** @class Bernstein
     *  The Bernstein's polynomial of order N
     */
    class GAUDI_API Bernstein : public std::unary_function<double,double>
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the order
      Bernstein ( const unsigned short       N          ,
                  const double               xmin  =  0 ,
                  const double               xmax  =  1 ) ;
      // ======================================================================
      /// constructor from N+1 coefficients
      Bernstein ( const std::vector<double>& pars       ,
                  const double               xmin  =  0 ,
                  const double               xmax  =  1 ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the value
      double operator () ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get number of parameters
      std::size_t npars () const { return m_pars.size() ; }
      /// set k-parameter
      bool setPar       ( const unsigned short k , const double value ) ;
      /// set k-parameter
      bool setParameter ( const unsigned short k , const double value )
      { return setPar   ( k , value ) ; }
      /// get the parameter value
      double  par       ( const unsigned short k ) const
      { return ( k < m_pars.size() ) ? m_pars[k] : 0.0 ; }
      /// get the parameter value
      double  parameter ( const unsigned short k ) const { return par ( k ) ; }
      /// get lower edge
      double xmin () const { return m_xmin ; }
      /// get upper edge
      double xmax () const { return m_xmax ; }
      // ======================================================================
    public:
      // ======================================================================
      double x ( const double t ) const
      { return       m_xmin   + ( m_xmax - m_xmin ) * t ; }
      double t ( const double x ) const
      { return ( x - m_xmin ) / ( m_xmax - m_xmin )     ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the integral between xmin and xmax
      double integral () const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the list of parameters
      std::vector<double>  m_pars ;                // the list of parameters
      /// the left edge of interval
      double m_xmin  ;                             // the left edge of interval
      /// the right edge of interval
      double m_xmax  ;                             // the right edge of interval
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Positive
     *  The "positive" polynomial of order N
     */
    class GAUDI_API Positive : public std::unary_function<double,double>
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the order
      Positive ( const unsigned short       N          ,
                 const double               xmin  =  0 ,
                 const double               xmax  =  1 ) ;
      // ======================================================================
      /// constructor from N phases
      Positive ( const std::vector<double>& pars       ,
                 const double               xmin  =  0 ,
                 const double               xmax  =  1 ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the value
      double operator () ( const double x ) const { return m_bernstein ( x ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get number of parameters
      std::size_t npars () const { return m_phases.size() ; }
      /// set k-parameter
      bool setPar       ( const unsigned short k , const double value ) ;
      /// set k-parameter
      bool setParameter ( const unsigned short k , const double value )
      { return setPar   ( k , value ) ; }
      /// get the parameter value
      double  par       ( const unsigned short k ) const
      { return ( k < m_phases.size() ) ? m_phases [k] : 0.0 ; }
      /// get the parameter value
      double  parameter ( const unsigned short k ) const { return par ( k ) ; }
      /// get lower edge
      double xmin () const { return m_bernstein.xmin () ; }
      /// get upper edge
      double xmax () const { return m_bernstein.xmax () ; }
      // ======================================================================
    public:
      // ======================================================================
      double x ( const double t ) const { return m_bernstein. x ( t )  ; }
      double t ( const double x ) const { return m_bernstein. t ( x )  ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the integral between xmin and xmax
      double integral () const { return m_bernstein.integral() ; }
      /// get the underlying Bernstein polynomial
      const Gaudi::Math::Bernstein& bernstein() const { return m_bernstein ; }
      // ======================================================================
    private:
      // ======================================================================
      /// update bernstein coefficinects
      bool updateBernstein ( const unsigned short i = 0 ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual bernstein polynomial
      Gaudi::Math::Bernstein m_bernstein ; // the actual bernstein polynomial
      /// the external parameters
      std::vector<double>    m_phases ;
      /// the starting phases
      std::vector<double>    m_phi0   ;
      /// sin^2 for parameters
      std::vector<double>    m_sin2   ;
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
       *  @param sigmaL  (left  sigma)
       *  @param sigmaR  (right sigma)
       */
      BifurcatedGauss
      ( const double peak  ,
        const double sigmaL ,
        const double asym  ) ;
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
      double sigmaL  () const { return m_sigmaL  ; }
      double sigmaR  () const { return m_sigmaR  ; }
      double sigma   () const ;
      double asym    () const ;
      // ======================================================================
    public:
      // ======================================================================
      bool setPeak    ( const double value ) ;
      bool setM0      ( const double value ) { return setPeak ( value ) ; }
      bool setMass    ( const double value ) { return setPeak ( value ) ; }
      bool setSigmaL  ( const double value ) ;
      bool setSigmaR  ( const double value ) ;
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
      /// sigma left
      double m_sigmaL ;       // sigma-left
      /// sigma right
      double m_sigmaR ;       // sigma-right
      // ======================================================================
    } ;
    // ========================================================================
    /** @class WorkSpace
     *  helper utility to keep the integration workspace
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date 2011-12-03
     */
    class WorkSpace
    {
    public:
      // ======================================================================
      /// constructor
      WorkSpace () ;
      /// copy constructor
      WorkSpace ( const WorkSpace& right );
      /// destructor
      ~WorkSpace () ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the integration workspace
      void* workspace () const ;               // get the integrtaion workspace
      // ======================================================================
    public:
      // ======================================================================
      /// the assignement operator
      WorkSpace& operator= ( const WorkSpace& right ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual GSL-workspace
      mutable void*  m_workspace ;  /// the actual GSL-workspace
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
      bool setPeak  ( const double value ) ;
      bool setM0    ( const double value ) { return setPeak ( value ) ; }
      bool setMass  ( const double value ) { return setPeak ( value ) ; }
      bool setSigma ( const double value ) ;
      bool setXi    ( const double value ) ;
      bool setRhoL  ( const double value ) ;
      bool setRhoR  ( const double value ) ;
      bool setRho_L ( const double value ) { return setRhoL ( value ) ; }
      bool setRho_R ( const double value ) { return setRhoR ( value ) ; }
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
      /// workspace
      Gaudi::Math::WorkSpace m_workspace ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Novosibirsk
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
      double mass  () const { return   m0    () ; }
      double sigma () const { return m_sigma    ; }
      double tau   () const { return m_tau      ; }
      // ======================================================================
    public:
      // ======================================================================
      bool setM0    ( const double value ) ;
      bool setPeak  ( const double value ) { return setM0 ( value ) ; }
      bool setMass  ( const double value ) { return setM0 ( value ) ; }
      bool setSigma ( const double value ) ;
      bool setTau   ( const double value ) ;
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
      /// workspace
      Gaudi::Math::WorkSpace m_workspace ;
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
      bool setM0    ( const double value ) ;
      bool setPeak  ( const double value ) { return setM0 ( value ) ; }
      bool setMass  ( const double value ) { return setPeak ( value ) ; }
      bool setSigma ( const double value ) ;
      bool setAlpha ( const double value ) ;
      bool setN     ( const double value ) ;
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
    private:
      // ======================================================================
      /// workspace
      Gaudi::Math::WorkSpace m_workspace ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Needham
     *  The special parametrization by Matthew NEEDHAM of
     *  ``Crystal Ball-function'' suitable for \f$J/\psi\f$-peak
     *  @thank Matthew Needham
     *  @see Gaudi::Math::CrystalBall
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2012-05-13
     */
    class GAUDI_API Needham : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /** constructor from all parameters
       *  @param m0     m0       parameter
       *  @param sigma  sigma    parameter
       *  @param a0     a0       parameter
       *  @param a1     a1       parameter
       *  @param a2     a2       parameter
       */
      Needham
      ( const double m0    = 3096.0     ,
        const double sigma =   13.5     ,
        const double a0    =    1.975   ,
        const double a1    =    0.0011  ,
        const double a2    =   -0.00018 ) ;
      /// destructor
      ~Needham() ;
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate Needham's function
      double operator() ( const double x ) const ;
      // ======================================================================
    public: // trivial accessors
      // ======================================================================
      double m0    () const { return m_cb.m0    () ; }
      double peak  () const { return      m0    () ; }
      double sigma () const { return m_cb.sigma () ; }
      double a0    () const { return m_a0          ; }
      double a1    () const { return m_a1          ; }
      double a2    () const { return m_a2          ; }
      double alpha () const { return a0()+sigma()*(a1()+sigma()*a2()) ; }
      // ======================================================================
    public: // trivial accessors
      // ======================================================================
      bool setM0    ( const double value ) { return m_cb.setM0    ( value ) ; }
      bool setPeak  ( const double value ) { return      setM0    ( value ) ; }
      bool setMass  ( const double value ) { return      setPeak  ( value ) ; }
      bool setSigma ( const double value ) { return m_cb.setSigma ( value ) ; }
      bool setA0    ( const double value ) ;
      bool setA1    ( const double value ) ;
      bool setA2    ( const double value ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// get (possibly trunkated) integral
      double integral () const { return m_cb.integral() ; }
      /// get integral between low and high
      double integral ( const double low ,
                        const double high ) const
      { return m_cb.integral ( low , high ) ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the function itself
      Gaudi::Math::CrystalBall m_cb ; // the function itself
      /// a0-parameter
      double m_a0       ;  // a0_parameter
      /// a0-parameter
      double m_a1       ;  // a1_parameter
      /// a0-parameter
      double m_a2       ;  // a2_parameter
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
      bool setM0      ( const double value ) ;
      bool setPeak    ( const double value ) { return setM0 ( value ) ; }
      bool setMass    ( const double value ) { return setPeak ( value ) ; }
      bool setSigma   ( const double value ) ;
      bool setAlpha_L ( const double value ) ;
      bool setN_L     ( const double value ) ;
      bool setAlpha_R ( const double value ) ;
      bool setN_R     ( const double value ) ;
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
      // ======================================================================
    private:
      // ======================================================================
      /// workspace
      Gaudi::Math::WorkSpace m_workspace ;
      // ======================================================================
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
       *  @param mean   the mean value for distribution
       *  @param sigma  the sigma
       *  @param kappa3 the standartized 3rd cumulant
       *  @param kappa4 the standartized 4th cumulant
       */
      GramCharlierA  ( const double mean   ,
                       const double sigma  ,
                       const double kappa3 ,
                       const double kappa4 ) ;
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
      bool setM0      ( const double value ) ;
      bool setMean    ( const double value ) { return setM0   ( value ) ; }
      bool setPeak    ( const double value ) { return setM0   ( value ) ; }
      bool setMass    ( const double value ) { return setPeak ( value ) ; }
      //
      bool setSigma   ( const double value ) ;
      bool setKappa3  ( const double value ) ;
      bool setKappa4  ( const double value ) ;
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
    private:
      // ======================================================================
      /// integration workspace
      Gaudi::Math::WorkSpace m_workspace ;    // integration workspace
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PhaseSpace2
     *  simple function to represent two-body phase space
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2011-11-30
     */
    class GAUDI_API PhaseSpace2
      : public std::unary_function<double,double>
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from two masses
      PhaseSpace2 ( const double m1 ,
                    const double m2 ) ;
      /// deststructor
      ~PhaseSpace2 () ;                                         // deststructor
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate 2-body phase space
      double operator () ( const double x ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      PhaseSpace2 () ;                   // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the first mass
      double m_m1 ; // the first  mass
      /// the second mass
      double m_m2 ; // the second mass
      // ======================================================================
    public :
      // ======================================================================
      /** calculate the triangle function
       *  \f[ \lambda ( a , b, c ) = a^2 + b^2 + c^2 - 2ab - 2bc - 2ca \f]
       *  @param a parameter a
       *  @param b parameter b
       *  @param c parameter b
       *  @return the value of triangle function
       */
      static double triangle
      ( const double a ,
        const double b ,
        const double c ) ;
      // ======================================================================
      /** calculate the particle momentum in rest frame
       *  \f[ q = \frac{1}{2}\frac{ \lambda^{\frac{1}{2}}
       *        \left( m^2 , m_1^2, m_2^2 \right) }{ m }\f],
       *  @param m the mass
       *  @param m1 the mass of the first particle
       *  @param m2 the mass of the second particle
       *  @return the momentum in rest frame (physical values only)
       */
      static double  q
      ( const double m  ,
        const double m1 ,
        const double m2 ) ;
      // ======================================================================
      /** calculate the particle momentum in rest frame
       *  @param m the mass
       *  @param m1 the mass of the first particle
       *  @param m2 the mass of the second particle
       *  @return the momentum in rest frame  (imaginary for non-physical branch)
       */
      static std::complex<double> q1
      ( const double m  ,
        const double m1 ,
        const double m2 ) ;
      // ======================================================================
      /** calculate the phase space for   m -> m1 + m2
       *  \f[ \Phi = \frac{1}{8\pi} \left( \frac{ \lambda^{\frac{1}{2}}
       *       \left( m^2 , m_1^2, m_2^2 \right) }{ m^2 }\right)^{2L+1}\f],
       *  where \f$\lambda\f$ is a triangle function
       *  @param m the mass
       *  @param m1 the mass of the first particle
       *  @param m2 the mass of the second particle
       *  @return two-body phase space
       */
      static double phasespace
      ( const double         m      ,
        const double         m1     ,
        const double         m2     ,
        const unsigned short L  = 0 ) ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PhaseSpace3
     *  simple function to represent three-body phase space
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2011-11-30
     */
    class GAUDI_API PhaseSpace3
      : public std::unary_function<double,double>
    {
      // ======================================================================
    public:
      // ======================================================================
      /** constructor from three masses
       *  @param m1 the mass of the first  particle
       *  @param m2 the mass of the second particle
       *  @param m3 the mass of the third  particle
       *  @param l1 the angular momentum between 1st and 2nd particle
       *  @param l2 the angular momentum between the pair and 3rd particle
       */
      PhaseSpace3 ( const double         m1     ,
                    const double         m2     ,
                    const double         m3     ,
                    const unsigned short l1 = 0 ,
                    const unsigned short l2 = 0 ) ;
      /// deststructor
      ~PhaseSpace3 () ;                                         // deststructor
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate 3-body phase space
      double operator () ( const double x ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      PhaseSpace3 () ;                   // the default constructor is disabled
      // ======================================================================
    public:
      // ======================================================================
      double lowEdge () const { return m_m1 + m_m2 + m_m3 ; }
      // ======================================================================
    public:
      // ======================================================================
      /// helper phase space ("23L")
      double ps2_aux ( const double m12 ) const ;
      /// get the integral between low and high limits
      double integral ( const double low  ,
                        const double high ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the mass of the first particle
      double         m_m1 ; // the mass of the first particle
      /// the mass of the second particle
      double         m_m2 ; // the mass of the second particle
      /// the mass of the third particle
      double         m_m3 ; // the mass of the third  particle
      /// the orbital momentum of the first pair
      unsigned short m_l1 ; // the orbital momentum of the first pair
      /// the orbital momentum between the pair and the third particle
      unsigned short m_l2 ; // the orbital momentum between the pair and the third particle
      // ======================================================================
    private:
      // ======================================================================
      /// the temporary mass
      mutable double m_tmp ; /// the temporary mass
      // ======================================================================
    private:
      // ======================================================================
      /// integration workspace
      Gaudi::Math::WorkSpace m_workspace  ;    // integration workspace
      /// integration workspace
      Gaudi::Math::WorkSpace m_workspace2 ;    // integration workspace
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PhaseSpaceLeft
     *  simple function to represent N-body phase space near left-threshold
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2011-11-30
     */
    class GAUDI_API PhaseSpaceLeft
      : public std::unary_function<double,double>
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from threshold and number of particles
      PhaseSpaceLeft ( const double         threshold ,
                       const unsigned short num       ) ;
      /// constructor from list of masses
      PhaseSpaceLeft ( const std::vector<double>& masses ) ;
      /// deststructor
      ~PhaseSpaceLeft() ;                                       // deststructor
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate N-body phase space near left threhsold
      double operator () ( const double x    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      bool setThreshold ( const double x ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      PhaseSpaceLeft () ;               // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the threshold
      double         m_threshold ; // the threshold
      /// number of particles
      unsigned short m_num       ; // number of particles
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PhaseSpaceRight
     *  simple function to represent N/L-body phase space near right-threshold
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2011-11-30
     */
    class GAUDI_API PhaseSpaceRight
      : public std::unary_function<double,double>
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from threshold and number of particles
      PhaseSpaceRight ( const double         threshold ,
                        const unsigned short l         ,
                        const unsigned short n         ) ;
      /// deststructor
      ~PhaseSpaceRight () ;                                     // deststructor
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate N/L-body phase space near right  threhsold
      double operator () ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      bool setThreshold ( const double x ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      PhaseSpaceRight () ;               // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the threshold
      double         m_threshold ; // the threshold
      /// number of particles
      unsigned short m_N         ; // number of particles
      /// number of particles
      unsigned short m_L         ; // number of particles
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PhaseSpaceNL
     *  simple function to represent N/L-body phase space
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2011-11-30
     */
    class GAUDI_API PhaseSpaceNL
      : public std::unary_function<double,double>
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from thresholds and number of particles
      PhaseSpaceNL ( const double         threshold1 ,
                     const double         threshold2 ,
                     const unsigned short l          ,
                     const unsigned short n          ) ;
      /// deststructor
      ~PhaseSpaceNL () ;                                     // deststructor
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate N/L-body phase space
      double operator () ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// set the thresholds
      bool setThresholds
      ( const double mn ,
        const double mx ) ;
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
      /// the default constructor is disabled
      PhaseSpaceNL () ;               // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the threshold
      double         m_threshold1 ; // the threshold
      double         m_threshold2 ; // the threshold
      /// number of particles
      unsigned short m_N          ; // number of particles
      /// number of particles
      unsigned short m_L          ; // number of particles
      /// normalization
      double m_norm               ; // normalization
      // ======================================================================
    private:
      // ======================================================================
      /// integration workspace
      Gaudi::Math::WorkSpace m_workspace ;    // integration workspace
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PhaseSpace23L
     *  simple function to represent the phase
     *   space of 2 particles from 3-body decays:
     *   \f$ f \propto q^{2\ell+1}p^{2L+1}\f$, where
     *     \f$\ell\f$ is the orbital momentum of the pair of particles,
     *    and \f$L\f$ is the orbital momentum between the pair and
     *    the third particle.
     *   E.g. taking \f$\ell=0, L=1\f$, one can get the S-wave contribution for
     *   \f$\pi^+\pi^-\f$-mass from \f$B^0\rightarrowJ/\psi\pi^+\pi^-\f$ decay.
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2012-04-01
     */
    class GAUDI_API PhaseSpace23L
      : public std::unary_function<double,double>
    {
      // ======================================================================
    public:
      // ======================================================================
      /** constructor from four masses and angular momenta
       *  @param m1 the mass of the first  particle
       *  @param m2 the mass of the second particle
       *  @param m3 the mass of the third  particle
       *  @param m  the mass of the mother particle (m>m1+m2+m3)
       *  @param L  the angular momentum between the first pair and
       *  the third particle
       *  @param l  the angular momentum between the first and the second particle
       */
      PhaseSpace23L ( const double         m1     ,
                      const double         m2     ,
                      const double         m3     ,
                      const double         m      ,
                      const unsigned short L      ,
                      const unsigned short l  = 0 ) ;
      /// deststructor
      ~PhaseSpace23L () ;                                     // deststructor
      // ======================================================================
    public:
      // ======================================================================
      /// calculate the phase space
      double operator () ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// calculate the phase space
      double ps23L ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      double m1        () const { return m_m1 ; }
      double m2        () const { return m_m2 ; }
      double m3        () const { return m_m3 ; }
      double m         () const { return m_m  ; }
      unsigned short l () const { return m_l  ; }
      unsigned short L () const { return m_L  ; }
      // ======================================================================
      double lowEdge   () const { return m1 () + m2 () ; }
      double highEdge  () const { return m  () - m3 () ; }
      /// get the momentum of 1st particle in rest frame of (1,2)
      double         q ( const double x ) const ;
      /// get the momentum of 3rd particle in rest frame of mother
      double         p ( const double x ) const ;
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
      /// the default constructor is disabled
      PhaseSpace23L () ;               // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the first mass
      double m_m1 ;            // the first mass
      /// the second mass
      double m_m2 ;            // the second mass
      /// the third  mass
      double m_m3 ;            // the third mass
      /// the mass of mother particle
      double m_m  ;            // the mass of mother particle
      /// the orbital momentum between the first and the second particle
      unsigned short m_l ; // the orbital momentum between the 1st and 2nd
      /// the orbital momentum between the pair an dthe third particle
      unsigned short m_L ; // the orbital momentum between the (12) and 3rd
      // ======================================================================
      /// helper normalization parameter
      double m_norm ; // helper normalization parameter
      // ======================================================================
    private:
      // ======================================================================
      /// integration workspace
      Gaudi::Math::WorkSpace m_workspace ;    // integration workspace
      // ======================================================================
    } ;
    // ========================================================================
    /** @class BreitWigner
     *
     *  J.D.Jackson,
     *  "Remarks on the Phenomenological Analysis of Resonances",
     *  In Nuovo Cimento, Vol. XXXIV, N.6
     *
     *  http://www.springerlink.com/content/q773737260425652/
     *
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2011-11-30
     */
    class GAUDI_API BreitWigner
      : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /** parameterization for \f$\rho(\omega)\f$-function from (A.1)
       *  J.D.Jackson,
       *  "Remarks on the Phenomenological Analysis of Resonances",
       *  In Nuovo Cimento, Vol. XXXIV, N.6
       */
      enum JacksonRho {
        Jackson_0  = 0 ,/// \f$\rho(\omega) = 1 \f$
        Jackson_A2 ,/// \f$ 1^- \rightarrow 0^- 0^- \f$ , l = 1
        Jackson_A3 ,/// \f$          1^- \rightarrow 0^- 1^- \f$ , l = 1
        Jackson_A4 ,/// \f$ \frac{3}{2}+ \rightarrow 0^- \frac{1}{2}^+ \f$ , l = 1
        Jackson_A5 ,/// \f$ \frac{3}{2}- \rightarrow 0^- \frac{1}{2}^+ \f$ , l = 2
        Jackson_A7 /// recommended for rho0 -> pi+ pi-
      } ;
      // ======================================================================
    protected:
      // ======================================================================
      /** @typedef rho
       *  the \f$\rho(\omega)\f$ function from Jackson
       *  Arguments
       *    - the        mass
       *    - the pole   mass
       *    - the first  daughter mass
       *    - the second daughter mass
       */
      typedef double (*rho_fun) ( double , double , double , double ) ;
      // ======================================================================
    public:
      // ======================================================================
      // constructor from all parameters
      BreitWigner ( const double         m0       ,
                    const double         gam0     ,
                    const double         m1       ,
                    const double         m2       ,
                    const unsigned short L    = 0 ) ;
      // constructor from all parameters
      BreitWigner ( const double         m0       ,
                    const double         gam0     ,
                    const double         m1       ,
                    const double         m2       ,
                    const unsigned short L        ,
                    const JacksonRho     r        ) ;
      /// destructor
      virtual ~BreitWigner () ;
      // ======================================================================
    private:
      // ======================================================================
      BreitWigner() ;
      // ======================================================================
    public:
      // ======================================================================
      /** calculate the Breit-Wigner shape
       *  \f$\frac{1}{\pi}\frac{\omega\Gamma(\omega)}
       *   { (\omega_0^2-\omega^2)^2-\omega_0^2\Gammma^2(\omega)-}\f$
       */
      virtual double operator() ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get breit-wigner amplitude
      std::complex<double> amplitude ( const double x ) const ;
      // ======================================================================
      double breit_wigner  ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      double         m0     () const { return m_m0      ; }
      double         mass   () const { return   m0   () ; }
      double         peak   () const { return   m0   () ; }
      double         gam0   () const { return m_gam0    ; }
      double         gamma0 () const { return   gam0 () ; }
      double         gamma  () const { return   gam0 () ; }
      double         width  () const { return   gam0 () ; }
      // ======================================================================
    public:
      // ======================================================================
      double         m1     () const { return m_m1 ; }
      double         m2     () const { return m_m2 ; }
      unsigned short L      () const { return m_L  ; }
      // ======================================================================
    public:
      // ======================================================================
      bool setM0     ( const double x ) ;
      bool setMass   ( const double x ) { return setM0     ( x ) ; }
      bool setPeak   ( const double x ) { return setM0     ( x ) ; }
      bool setGamma0 ( const double x ) ;
      bool setGamma  ( const double x ) { return setGamma0 ( x ) ; }
      bool setWidth  ( const double x ) { return setGamma0 ( x ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// set rho-function
      void setRhoFun ( rho_fun rho ) { m_rho_fun = rho ; }
      // ======================================================================
    public:
      // ======================================================================
      /// calculate the current width
      double gamma ( const double x ) const ;
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
      /// the mass
      double m_m0   ; // the mass
      /// the width
      double m_gam0 ; // the width
      // ======================================================================
    private:
      // ======================================================================
      /// the mass of the first  particle
      double       m_m1      ;
      /// the mass of the second particle
      double       m_m2      ;
      /// the orbital momentum
      unsigned int m_L       ; // the orbital momentum
      /// the Jackson-Rho function
      rho_fun      m_rho_fun ; // the Jackson-Rho function
      // ======================================================================
    private:
      // ======================================================================
      /// integration workspace
      Gaudi::Math::WorkSpace m_workspace ;    // integration workspace
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Rho0
     *  J.D.Jackson,
     *  "Remarks on the Phenomenological Analysis of Resonances",
     *  In Nuovo Cimento, Vol. XXXIV, N.6
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2011-11-30
     */
    class GAUDI_API Rho0 : public Gaudi::Math::BreitWigner
    {
    public:
      // ======================================================================
      // constructor from all parameters
      Rho0  ( const double m0       = 770   ,     // MeV
              const double gam0     = 150   ,     // MeV
              const double pi_mass  = 139.6 ) ;   // MeV
      /// destructor
      virtual ~Rho0 () ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Rho0FromEtaPrime
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2011-11-30
     */
    class GAUDI_API Rho0FromEtaPrime : public Gaudi::Math::Rho0
    {
    public:
      // ======================================================================
      /// constructor from all parameters
      Rho0FromEtaPrime  ( const double m0        = 770   ,    // MeV
                          const double gam0      = 150   ,    // MeV
                          const double pi_mass   = 139.6 ,    // MeV
                          const double eta_prime = 957.7 ) ;  // MeV
      /// constructor from all parameters
      Rho0FromEtaPrime  ( const Gaudi::Math::Rho0& rho   ,
                          const double eta_prime = 957.7 ) ;  // MeV
      /// destructor
      virtual ~Rho0FromEtaPrime () ;
      // ======================================================================
    public:
      // ======================================================================
      /// calculate the function
      virtual double operator() ( const double x ) const ;
      // ======================================================================
    private:
      // ======================================================================
      double m_eta_prime ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Flatte
     *
     *  S.M. Flatt�
     *  "Coupled-channel analysis of the \f$\pi\eta\f$ and \f$K\bar{K}\f$
     *  systems near \f$K\bar{K}\f$threshold"
     *  Physics Letters B, Volume 63, Issue 2, 19 July 1976, Pages 224-227
     *
     *  http://www.sciencedirect.com/science/article/pii/0370269376906547
     *
     *  \f$\pi\pi\f$-channel
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2011-11-30
     */
    class GAUDI_API Flatte
      : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /** constructor  from all parameters
       *  @param m0    the mass
       *  @param m0g1  parameter \f$ m_0\times g_1\f$
       *  @param g2og2 parameter \f$ g2/g_1       \f$
       *  @param mK    kaon mass
       *  @param mPi   pion mass
       */
      Flatte  ( const double m0    = 980    ,
                const double m0g1  = 165    ,
                const double g2og1 = 4.21   ,
                const double mK    = 493.7  ,
                const double mPi   = 139.6  ) ;
      /// destructor
      virtual ~Flatte () ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the value of Flatte function
      virtual double operator() ( const double x ) const ;
      /// get the value of Flatte amplitude
      virtual std::complex<double> amplitude ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the amplitude for pipi-channel
      std::complex<double> flatte_amp  ( const double x ) const ;
      /// get the amplitude for KK-channel
      std::complex<double> flatte2_amp ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the curve for pipi-channel
      double flatte  ( const double x ) const ;
      /// get the curve for   KK-channel
      double flatte2 ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      double m0     () const { return m_m0      ; }
      double mass   () const { return   m0   () ; }
      double peak   () const { return   m0   () ; }
      double m0g1   () const { return m_m0g1    ; }
      double g2og1  () const { return m_g2og1   ; }
      double mK     () const { return m_K       ; }
      double mPi    () const { return m_Pi       ; }
      // ======================================================================
    public:
      // ======================================================================
      bool setM0     ( const double x ) ;
      bool setMass   ( const double x ) { return setM0 ( x ) ; }
      bool setPeak   ( const double x ) { return setM0 ( x ) ; }
      bool setM0G1   ( const double x ) ;
      bool setG2oG1  ( const double x ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the integral
      virtual double integral () const ;
      /// get the integral between low and high limits
      virtual double integral ( const double low  ,
                                const double high ) const ;
      // ======================================================================
    private:
      // ======================================================================
      double m_m0     ;
      double m_m0g1   ;
      double m_g2og1  ;
      double m_K      ;
      double m_Pi     ;
      // ======================================================================
    private:
      // ======================================================================
      /// integration workspace
      Gaudi::Math::WorkSpace m_workspace ;    // integration workspace
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Flatte2
     *
     *  S.M. Flatt�
     *  "Coupled-channel analysis of the \f$\pi\eta\f$ and \f$K\bar{K}\f$
     *  systems near \f$K\bar{K}\f$threshold"
     *  Physics Letters B, Volume 63, Issue 2, 19 July 1976, Pages 224-227
     *
     *  http://www.sciencedirect.com/science/article/pii/0370269376906547
     *
     *  KK-channel
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2011-11-30
     */
    class GAUDI_API Flatte2 : public Gaudi::Math::Flatte
    {
    public:
      // ======================================================================
      /** constructor  from three parameters
       *  @param m0    the mass
       *  @param m0g1  parameter \f$ m_0\times g_1\f$
       *  @param g2og2 parameter \f$ g2/g_1       \f$
       */
      Flatte2  ( const double m0    = 980      ,
                 const double m0g1  = 165      ,
                 const double g2og1 = 4.21     ,
                 const double mK    = 493.7    ,
                 const double mPi   = 139.6    ) ;
      /// destructor
      virtual ~Flatte2 () ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the value of Flatte function (KK-channel)
      virtual double operator() ( const double x ) const ;
      /// get the value of Flatte amplitude
      virtual std::complex<double> amplitude ( const double x ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Voight
     *  simple Voightian function
     */
    class GAUDI_API Voigt
      : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /// constructor  from three parameters
      Voigt  ( const double m0         ,
               const double gamma      ,
               const double sigma      ,
               const int    r     = 5  ) ;
      /// destructor
      virtual ~Voigt () ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the value of Voigt function
      // ======================================================================
      virtual double operator() ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      double m0     () const { return m_m0      ; }
      double mass   () const { return   m0   () ; }
      double peak   () const { return   m0   () ; }
      double gamma  () const { return m_gamma   ; }
      double sigma  () const { return m_sigma   ; }
      // ======================================================================
    public:
      // ======================================================================
      bool setM0     ( const double x ) ;
      bool setMass   ( const double x ) { return setM0 ( x ) ; }
      bool setPeak   ( const double x ) { return setM0 ( x ) ; }
      bool setGamma  ( const double x ) ;
      bool setSigma  ( const double x ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the integral
      virtual double integral () const ;
      /// get the integral between low and high limits
      virtual double integral ( const double low  ,
                                const double high ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      Voigt() ;                          // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      double m_m0     ;
      double m_gamma  ;
      double m_sigma  ;
      int    m_r      ;
      // ======================================================================
    private:
      // ======================================================================
      /// integration workspace
      Gaudi::Math::WorkSpace m_workspace ;    // integration workspace
      // ======================================================================
    } ;
    // ========================================================================
    namespace Jackson
    {
      // ======================================================================
      /** the simplest function: constant
       *  @see Gaudi::Math::BreitWigner
       *  @see Gaudi::Math::BreitWigner::rho_fun
       *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
       *  @date 2011-11-30
       */
      GAUDI_API
      double jackson_0 ( double /* m  */ ,
                         double /* m0 */ ,
                         double /* m1 */ ,
                         double /* m2 */ ) ;
      // ======================================================================
      /** the simple function for \f$ 1^- \rightarrow 0^- 0^- \f$, l = 1
       *  \f$\rho(\omega)= \omega^{-1} \f$
       *  @see Gaudi::Math::BreitWigner
       *  @see Gaudi::Math::BreitWigner::rho_fun
       *  @param m the invariant mass
       *  @return the value of rho-function
       *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
       *  @date 2011-11-30
       */
      GAUDI_API
      double jackson_A2 ( double    m     ,
                          double /* m0 */ ,
                          double /* m1 */ ,
                          double /* m2 */ ) ;
      // ======================================================================
      /** the simple function for \f$ 1^- \rightarrow 0^- 1^- \f$, l = 1
       *  \f$\rho(\omega)= \omega \f$
       *  @see Gaudi::Math::BreitWigner
       *  @see Gaudi::Math::BreitWigner::rho_fun
       *  @param m the invariant mass
       *  @return the value of rho-function
       *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
       *  @date 2011-11-30
       */
      GAUDI_API
      double jackson_A3 ( double    m     ,
                          double /* m0 */ ,
                          double /* m1 */ ,
                          double /* m2 */ ) ;
      // ======================================================================
      /** the simple function for
       *  \f$ \frac{3}{2}^+ \rightarrow \frac{1}{2}^+ 0^- \f$, l = 1
       *  $\rho(\omega)= \frac{ ( \omega + M )^2 - m^2 }{ \omega^2} \f$
       *  @see Gaudi::Math::BreitWigner
       *  @see Gaudi::Math::BreitWigner::rho_fun
       *  @param m the invariant mass
       *  @param m1 the invariant mass of the first  (spinor) particle
       *  @param m2 the invariant mass of the secodn (scalar) particle
       *  @return the value of rho-function
       *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
       *  @date 2011-11-30
       */
      GAUDI_API
      double jackson_A4 ( double    m     ,
                          double /* m0 */ ,
                          double    m1    ,
                          double    m2     ) ;
      // ======================================================================
      /** the simple function for
       *  \f$ \frac{3}{2}^- \rightarrow \frac{1}{2}^+ 0^- \f$, l = 2
       *  $\rho(\omega)= \left[ ( \omega + M )^2 - m^2 \right]^{-1} \f$
       *  @see Gaudi::Math::BreitWigner
       *  @see Gaudi::Math::BreitWigner::rho_fun
       *  @param m the invariant mass
       *  @param m1 the invariant mass of the first  (spinor) particle
       *  @param m2 the invariant mass of the secodn (scalar) particle
       *  @return the value of rho-function
       *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
       *  @date 2011-11-30
       */
      GAUDI_API
      double jackson_A5 ( double    m     ,
                          double /* m0 */ ,
                          double    m1    ,
                          double    m2     ) ;
      // ======================================================================
      /** the simple function for \f$\rho^- \rightarrow \pi^+ \pi^-\f$
       *  \f$ 1- \rightarrow 0^- 0^- \f$, l = 1
       *  $\rho(\omega)= \left[ q_0^2 + q^2 \right]^{-1}f$
       *  @see Gaudi::Math::BreitWigner
       *  @see Gaudi::Math::BreitWigner::rho_fun
       *  @param m the invariant mass
       *  @param m the nominam   mass
       *  @return the value of rho-function
       *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
       *  @date 2011-11-30
       */
      GAUDI_API
      double jackson_A7 ( double    m     ,
                          double    m0    ,
                          double    m1    ,
                          double    m2    ) ;
      // ======================================================================
    } //                                               end of namespace Jackson
    // ========================================================================
    /** @class LASS23L
     *  simple function to represent the phase
     *   space of 2 particles from 3-body decays:
     *   \f$ f \propto q^{2\ell+1}p^{2L+1}\f$, where
     *     \f$\ell\f$ is the orbital momentum of the pair of particles,
     *    and \f$L\f$ is the orbital momentum between the pair and
     *    the third particle.
     *   E.g. taking \f$\ell=0, L=1\f$, one can get the S-wave contribution for
     *   \f$\pi^+\pi^-\f$-mass from \f$B^0\rightarrowJ/\psi\pi^+\pi^-\f$ decay.
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-04-01
     */
    class GAUDI_API LASS23L : public std::unary_function<double,double>
    {
      // ======================================================================
    public:
      // ======================================================================
      /** constructor from all masses and angular momenta
       *  @param m1 the mass of the first  particle
       *  @param m2 the mass of the second particle
       *  @param m3 the mass of the third  particle
       *  @param m  the mass of the mother particle (m>m1+m2+m3)
       *  @param L  the angular momentum between the first pair and the third
       *  @param a  the LASS parameter
       *  @param r  the LASS parameter
       */
      LASS23L ( const double         m1 =  493.7  ,
                const double         m2 =  139.6  ,
                const double         m3 = 3097    ,
                const double         m  = 5278    ,
                const double         m0 = 1435    ,
                const double         g0 =  279    ,
                const unsigned short L  =    1    ,
                const double         a  = 1.94e-3 ,
                const double         r  = 1.76e-3 ,
                const double         e  = 1.0     ) ;
      /// destructor
      virtual ~LASS23L () ;                                     // deststructor
      // ======================================================================
    public:
      // ======================================================================
      /// get the (complex) LASS amplitude
      std::complex<double> amplitude  ( const double x ) const ;
      /// get the phase space factor
      double               phaseSpace ( const double x ) const ;
      /// evaluate LASS
      double operator () ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      bool setM0 ( const double value ) ;
      bool setG0 ( const double value ) ;
      bool setA  ( const double value ) ;
      bool setR  ( const double value ) ;
      bool setE  ( const double value ) ;
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
      /// the pole position for scalar meson
      double   m_m0 ;
      double   m_g0 ;
      /// LASS-parameters
      double   m_a  ;
      double   m_r  ;
      double   m_e  ;
      // ======================================================================
    private:
      // ======================================================================
      /// phase space
      Gaudi::Math::PhaseSpace23L m_ps    ;    // phase space
      // ======================================================================
    private:
      // ======================================================================
      /// integration workspace
      Gaudi::Math::WorkSpace m_workspace ;    // integration workspace
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Bugg23L
     *  parametrisation of sigma-pole for
     *  two pion mass distribution from three body decays
     *
     *  The parameterization of sigma pole by
     *  B.S.Zou and D.V.Bugg, Phys.Rev. D48 (1993) R3948.
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-04-01
     */
    class GAUDI_API Bugg23L
      : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /** constructor from all masses and angular momenta
       *  @param M  mass of sigma (very different from the pole positon!)
       *  @param g2 width parameter g2 (4pi width)
       *  @param b1 width parameter b1  (2pi coupling)
       *  @param b2 width parameter b2  (2pi coupling)
       *  @param s1 width parameter s1  (cut-off for 4pi coupling)
       *  @param s2 width parameter s2  (cut-off for 4pi coupling)
       *  @param a  parameter a (the exponential cut-off)
       *  @param m1 the mass of the first  particle
       *  @param m3 the mass of the third  particle
       *  @param m  the mass of the mother particle (m>m1+m2+m3)
       *  @param L  the angular momentum between the first pair and the third
       */
      Bugg23L ( const double         M  = 0.9264        ,  // GeV
                const double         g2 = 0.0024        ,  // GeV
                const double         b1 = 0.5848        ,  // GeV
                const double         b2 = 1.6663        ,  // GeV-1
                const double         a  = 1.082         ,  // GeV^2
                const double         s1 = 2.8           ,  // GeV^2
                const double         s2 = 3.5           ,
                const double         m1 =  139.6 / 1000 ,  // MeV
                const double         m3 = 3097.0 / 1000 ,  // MeV
                const double         m  = 5278.0 / 1000 ,  // MeV
                const unsigned short L  =    1          ) ;
      /// destructor
      ~Bugg23L () ;
      // ======================================================================
    public:
      // ======================================================================
      /// calculate the Breit-Wigner shape
      double operator() ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the amlitude  (not normalized!)
      std::complex<double> amplitude (  const double x ) const ;
      /// get the phase space factor (taking into account L)
      double phaseSpace ( const double x ) const { return m_ps  ( x ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // phase space variables
      // ======================================================================
      double m1        () const { return m_ps.m1 () ; }
      double m2        () const { return m_ps.m2 () ; }
      double m3        () const { return m_ps.m3 () ; }
      double m         () const { return m_ps.m  () ; }
      // ======================================================================
      double lowEdge   () const { return m_ps. lowEdge() ; }
      double highEdge  () const { return m_ps.highEdge() ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the running width by Bugg
      std::complex<double>
      gamma ( const double x ) const ; // get the running width by Bugg
      // ======================================================================
    public:
      // ======================================================================
      /// adler factor
      double               adler       ( const double x ) const ; // adler factor
      /// ratio of 2pi-phase spaces
      double               rho2_ratio  ( const double x ) const ;
      /// ratio of 4pi-phase spaces
      std::complex<double> rho4_ratio  ( const double x ) const ;
      /// b-factor for 2-pi coupling
      double b( const double x ) const { return  b1 () + x * x * b2 () ; }
      // ======================================================================
    private:
      // ======================================================================
      /// approximation for  4pi-phase space
      std::complex<double> rho4        ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      // sigma & Bugg variables
      // ======================================================================
      double M     () const  { return m_M       ; }
      double M2    () const  { return m_M * m_M ; }
      double m0    () const  { return   M ()    ; }
      double mass  () const  { return   M ()    ; }
      double peak  () const  { return   M ()    ; }
      // ======================================================================
      double g2    () const  { return m_g2   ; }
      double b1    () const  { return m_b1   ; }
      double b2    () const  { return m_b2   ; }
      double s1    () const  { return m_s1   ; }
      double s2    () const  { return m_s2   ; }
      double a     () const  { return m_a    ; }
      // ======================================================================
      bool setM    ( const double value  ) ;
      bool setM0   ( const double value  ) { return setM ( value )  ; }
      bool setMass ( const double value  ) { return setM ( value )  ; }
      bool setPeak ( const double value  ) { return setM ( value )  ; }
      // ======================================================================
      bool setG2   ( const double value  ) ;
      bool setB1   ( const double value  ) ;
      bool setB2   ( const double value  ) ;
      bool setS1   ( const double value  ) ;
      bool setS2   ( const double value  ) ;
      bool setA    ( const double value  ) ;
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
      // sigma & Bugg varibales
      // ======================================================================
      /// mass of sigma (very different from the pole positon!)
      double m_M  ; // mass of sigma (very different from the pole positon!)
      /// width parameter g2 (4pi width)
      double m_g2 ; // width parameter g2 (4-p width)
      /// width parameter b1  (2pi coupling)
      double m_b1 ; // width parameter b1  (2pi coupling)
      /// width parameter b2  (2pi coupling)
      double m_b2 ; // width parameter b2  (2pi coupling)
      /// width parameter s1  (cut-off for 4pi coupling)
      double m_s1 ; // width parameter b1  (cut-off for 4pi coupling)
      /// width parameter s2  (cut-off for 4pi coupling)
      double m_s2 ; // width parameter b2  (cut-off for 4pi coupling)
      /// parameter a (the exponential cut-off)
      double m_a  ; // parameter a (the exponential cut-off)
      // ======================================================================
    private:
      // ======================================================================
      /// phase space
      Gaudi::Math::PhaseSpace23L m_ps         ; // phase space
      // ======================================================================
    private:
      /// integration workspace
      Gaudi::Math::WorkSpace     m_workspace  ;    // integration workspace
      // ======================================================================
    } ;
    // ========================================================================
    /** @class BW23L
     *  @see Gaudi::Math::BreittWigner
     *  @see Gaudi::Math::PhaseSpace23L
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2012-05-23
     */
    class GAUDI_API BW23L
      : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      // constructor from all parameters
      BW23L ( const double         m0       ,
              const double         gam0     ,
              const double         m1       ,
              const double         m2       ,
              const double         m3       ,
              const double         m        ,
              const unsigned short L1  = 0  ,
              const unsigned short L2  = 0  ) ;
      // constructor from all parameters
      BW23L ( const double         m0       ,
              const double         gam0     ,
              const double         m1       ,
              const double         m2       ,
              const double         m3       ,
              const double         m        ,
              const unsigned short L1       ,
              const unsigned short L2       ,
              const Gaudi::Math::BreitWigner::JacksonRho r ) ;
      /// constructor from BreitWigner
      BW23L ( const Gaudi::Math::BreitWigner& bw ,
              const double                    m3 ,
              const double                    m  ,
              const unsigned short            L2 ) ;
      /// destructor
      virtual ~BW23L () ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      BW23L () ;                         // the default constructor is disabled
      // ======================================================================
    public:
      // ======================================================================
      /// calculate the shape
      double operator() ( const double x ) const ;
      /// get the amplitude
      std::complex<double>
      amplitude ( const double x ) const { return m_bw.amplitude ( x ) ; }
      // ======================================================================
    public:
      // ======================================================================
      double m0     () const { return m_bw . m0   () ; }
      double mass   () const { return        m0   () ; }
      double peak   () const { return        m0   () ; }
      double gam0   () const { return m_bw . gam0 () ; }
      double gamma0 () const { return        gam0 () ; }
      double gamma  () const { return        gam0 () ; }
      double width  () const { return        gam0 () ; }
      // ======================================================================
    public:
      // ======================================================================
      bool setM0     ( const double x ) { return m_bw.setM0     ( x ) ; }
      bool setMass   ( const double x ) { return setM0          ( x ) ; }
      bool setPeak   ( const double x ) { return setM0          ( x ) ; }
      bool setGamma0 ( const double x ) { return m_bw.setGamma0 ( x ) ; }
      bool setGamma  ( const double x ) { return setGamma0      ( x ) ; }
      bool setWidth  ( const double x ) { return setGamma0      ( x ) ; }
      // ======================================================================
    public:
      // ======================================================================
      double lowEdge   () const { return m_ps. lowEdge() ; }
      double highEdge  () const { return m_ps.highEdge() ; }
      // ======================================================================
    public:
      // ======================================================================
      /// calculate the current width
      double gamma ( const double x ) const { return m_bw.gamma ( x ) ; }
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
      /// the breit wigner
      Gaudi::Math::BreitWigner   m_bw        ;    // the breit wigner
      /// the phase space
      Gaudi::Math::PhaseSpace23L m_ps        ;    // the phase space
      /// integration workspace
      Gaudi::Math::WorkSpace     m_workspace ;    // integration workspace
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Flatte23L
     *  \f$\pi\pi\f$-channel
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2011-11-30
     */
    class GAUDI_API Flatte23L
      : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /** constructor  from all parameters
       *  @param m0    the mass
       *  @param m0g1  parameter \f$ m_0\times g_1\f$
       *  @param g2og2 parameter \f$ g2/g_1       \f$
       *  @param mK    kaon mass
       *  @param mPi   pion mass
       *  @param m3    the mass of the third particle
       *  @param m     the mass of mother particle
       *  @param L     the orbital momentum between the pair and the third particle
       */
      Flatte23L  ( const double         m0    =  980.0   ,     // MeV
                   const double         m0g1  =  165     ,     // MeV^2
                   const double         g2og1 =    4.21  ,     // dimensionless
                   const double         mK    =  493.7   ,     // MeV
                   const double         mPi   =  139.6   ,     // MeV
                   const double         m3    = 3096.9   ,     // MeV
                   const double         m     = 5366.0   ,     // MeV
                   const unsigned short L     = 1        ) ;
      // ======================================================================
      /** constructor  from flatte function
       *  @param m3    the mass of the third particle
       *  @param m     the mass of mother particle
       *  @param L     the orbital momentum between the pair and the third particle
       */
      Flatte23L  ( const Flatte&        fun              ,     // MeV
                   const double         m3    = 3096.9   ,     // MeV
                   const double         m     = 5366.0   ,     // MeV
                   const unsigned short L     = 1        ) ;
      /// destructor
      virtual ~Flatte23L () ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the value of Flatte function
      // ======================================================================
      double operator() ( const double x ) const ;
      // ======================================================================
      /// get the value of complex Flatte amplitude (pipi-channel)
      std::complex<double> amplitude ( const double x ) const
      { return m_flatte.flatte_amp ( x ) ; }
      // ======================================================================
    public:
      // ======================================================================
      double m0     () const { return m_flatte . m0    () ; }
      double mass   () const { return            m0    () ; }
      double peak   () const { return            m0    () ; }
      double m0g1   () const { return m_flatte . m0g1  () ; }
      double g2og1  () const { return m_flatte . g2og1 () ; }
      // ======================================================================
    public:
      // ======================================================================
      bool setM0     ( const double x ) { return m_flatte . setM0    ( x ) ; }
      bool setMass   ( const double x ) { return            setM0    ( x ) ; }
      bool setPeak   ( const double x ) { return            setM0    ( x ) ; }
      bool setM0G1   ( const double x ) { return m_flatte . setM0G1  ( x ) ; }
      bool setG2oG1  ( const double x ) { return m_flatte . setG2oG1 ( x ) ; }
      // ======================================================================
    public:
      // ======================================================================
      double lowEdge   () const { return m_ps .  lowEdge () ; }
      double highEdge  () const { return m_ps . highEdge () ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the integral
      virtual double integral () const ;
      /// get the integral between low and high limits
      virtual double integral ( const double low  ,
                                const double high ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual Flatte function
      Gaudi::Math::Flatte        m_flatte ; // the actual Flatte function
      /// phase space factor
      Gaudi::Math::PhaseSpace23L m_ps     ; // phase space factor
      // ======================================================================
    private:
      // ======================================================================
      /// integration workspace
      Gaudi::Math::WorkSpace m_workspace ;    // integration workspace
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Gounaris23L
     *  parametrisation of rho0 for
     *  two pion mass distribution from three body decays
     *
     *  G.J.Gounaris and J.J.Sakurai,
     *  "Finite width corrections to the vector meson dominance
     *  predictions for \f$\rho\rightarrow e^+e^-\f$",
     *  Phys.Rev.Lett. 21 (1968) 244
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-04-01
     */
    class GAUDI_API Gounaris23L
      : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /** constructor from all masses and angular momenta
       *  @param M  mass of rho
       *  @param g0 width parameter
       *  @param m1 the mass of the first  particle (the same as the second)
       *  @param m3 the mass of the third  particle
       *  @param m  the mass of the mother particle (m>m1+m2+m3)
       *  @param L  the angular momentum between the first pair and the third
       */
      Gounaris23L ( const double         M  = 0.770         ,  // GeV
                    const double         g0 = 0.150         ,  // GeV
                    const double         m1 =  139.6 / 1000 ,  // MeV
                    const double         m3 = 3097.0 / 1000 ,  // MeV
                    const double         m  = 5278.0 / 1000 ,  // MeV
                    const unsigned short L  =    1          ) ;
      /// destructor
      ~Gounaris23L () ;
      // ======================================================================
    public:
      // ======================================================================
      /// calculate the Gounaris-Sakurai shape
      double operator() ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the amlitude  (not normalized!)
      std::complex<double> amplitude (  const double x ) const ;
      /// get the phase space factor (taking into account L)
      double phaseSpace ( const double x ) const { return m_ps  ( x ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // phase space variables
      // ======================================================================
      double m1        () const { return m_ps.m1 () ; }
      double m2        () const { return m_ps.m2 () ; }
      double m3        () const { return m_ps.m3 () ; }
      double m         () const { return m_ps.m  () ; }
      // ======================================================================
      double lowEdge   () const { return m_ps. lowEdge() ; }
      double highEdge  () const { return m_ps.highEdge() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// get h-factor
      double h       ( const double x ) const ;
      /// get h-factor
      double h       ( const double x , const double k ) const ;
      /// get h'-factor
      double h_prime ( const double x ) const ;
      /// get h'-factor
      double h_prime ( const double x , const double k ) const ;
      // ======================================================================
    public:
      // ======================================================================
      // Gounaris & Sakurai variables
      // ======================================================================
      double M      () const  { return m_M     ; }
      double m0     () const  { return   M  () ; }
      double mass   () const  { return   M  () ; }
      double peak   () const  { return   M  () ; }
      // ======================================================================
      double g0     () const  { return m_g0    ; }
      double gamma  () const  { return   g0 () ; }
      double width  () const  { return   g0 () ; }
      // ======================================================================
      bool setM     ( const double value  ) ;
      bool setM0    ( const double value  ) { return setM  ( value ) ; }
      bool setMass  ( const double value  ) { return setM  ( value ) ; }
      bool setPeak  ( const double value  ) { return setM  ( value ) ; }
      // ======================================================================
      bool setG0    ( const double value  ) ;
      bool setGamma ( const double value  ) { return setG0 ( value ) ; }
      bool setWidth ( const double value  ) { return setG0 ( value ) ; }
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
      //  Gounaris and Sakurai variables
      // ======================================================================
      /// mass of rho
      double m_M  ; // mass of sigma (very different from the pole positon!)
      /// width parameter
      double m_g0 ; // width parameter
      // ======================================================================
    private:
      // ======================================================================
      /// phase space
      Gaudi::Math::PhaseSpace23L m_ps         ; // phase space
      // ======================================================================
    private:
      // ======================================================================
      /// integration workspace
      Gaudi::Math::WorkSpace     m_workspace  ;    // integration workspace
      // ======================================================================
    } ;
    // ========================================================================
    /** @class StudentT
     *  simple function to parameterize the symmetric peak using 
     *  Student's ditribution 
     *  
     *  \f[  f(y) = \frac{1}{\sqrt{\pi n}} \frac { \Gamma( \frac{n+1}{2}) } { \Gamma( \frac{n}{2}  ) }
     *  \left( 1 + \frac{y^2}{n} \right)^{ -\frac{n+1}{2}} \f], 
     *  where \f$ y = \frac{x - \mu}{\sigma} \f$  
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2013-01-05
     */
    class GAUDI_API StudentT
      : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /** constructor from mass, resolution and "n"-parameter 
       *  @param M     mass 
       *  @param sigma width parameter
       *  @param N     n-parameter  ( actually  n=1+|N| ) 
       */
      StudentT ( const double mass  , 
                 const double sigma ,
                 const double n     ) ;
      /// destructor
      ~StudentT() ;
      // ======================================================================
    public:
      // ======================================================================
      /// calculate StudentT's shape
      double operator() ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      // variables
      // ======================================================================
      double M      () const  { return m_M      ; }
      double m0     () const  { return   M   () ; }
      double mass   () const  { return   M   () ; }
      double peak   () const  { return   M   () ; }
      // ======================================================================
      double sigma  () const  { return m_s      ; }
      double s      () const  { return sigma () ; }
      double gamma  () const  { return sigma () ; }
      double width  () const  { return sigma () ; }
      // ======================================================================
      double n      () const  { return m_n      ; }
      // ======================================================================
      bool setM     ( const double value  ) ;
      bool setM0    ( const double value  ) { return setM  ( value ) ; }
      bool setMass  ( const double value  ) { return setM  ( value ) ; }
      bool setPeak  ( const double value  ) { return setM  ( value ) ; }
      // ======================================================================
      bool setSigma ( const double value  ) ;
      bool setS     ( const double value  ) { return setSigma ( value ) ; }
      bool setGamma ( const double value  ) { return setSigma ( value ) ; }
      bool setWidth ( const double value  ) { return setSigma ( value ) ; }
      // ======================================================================
      bool setN     ( const double value  ) ;
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
      /// default consytructor is disabled 
      StudentT() ;  // default consytructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// mass 
      double m_M  ; // 
      /// width parameter
      double m_s ; // width parameter
      /// n-parameter
      double m_n ; // n-parameter
      // ======================================================================
    private: // normalization 
      // ======================================================================
      double m_norm  ;
      // ======================================================================
    private:
      // ======================================================================
      /// integration workspace
      Gaudi::Math::WorkSpace     m_workspace  ;    // integration workspace
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
