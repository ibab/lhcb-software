// $Id:$
// ============================================================================
#ifndef LHCBMATH_POLYNOMIALS_H 
#define LHCBMATH_POLYNOMIALS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD& STL 
// ============================================================================
#include <functional>
#include <vector>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
/** @file LHCbMath/Polynomials.h
 *
 *  various polinomials 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2010-04-19
 *
 *                    $Revision: 184160 $
 *  Last modification $Date: 2015-02-21 17:19:02 +0100 (Sat, 21 Feb 2015) $
 *                 by $author$
 */
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Math
  {
    // ========================================================================
    //  Chebyshev 1st kind 
    // ========================================================================
    template <unsigned int N> class  Chebyshev_  ;
    // ========================================================================
    /** @class Chebychev_
     *  Efficient evaluator of Chebyshev polynomial
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
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
    //  Chebyshev 2nd kind 
    // ========================================================================
    template <unsigned int N> class  ChebyshevU_ ;
    // ========================================================================
    /** @class ChebychevU_
     *  Efficient evaluator of Chebyshev polynomial of the secon kind 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2011-04-19
     */
    template <unsigned int N>
    class  ChebyshevU_ : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /// the only one important method
      inline double operator() ( const double    x    ) const
      { return x * m_U1 ( x ) + m_TN ( x ) ; }
      // ======================================================================
    private:
      // ======================================================================
      /// U (N-1)
      ChebyshevU_<N-1>  m_U1 ;                                        // U (N-1)
      /// T (N)
      Chebyshev_<N>     m_TN ;                                        // T (N)
      // ======================================================================
    } ;
    // ========================================================================    
    /// specialization for N=0
    template <>
    class  ChebyshevU_<0> : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /// the only one important method
      inline double operator() ( const double /* x */ ) const { return 1 ; }
      // ======================================================================
    } ;
    // ========================================================================
    /// specialization for N=1
    template <>
    class  ChebyshevU_<1> : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /// the only one important method
      inline double operator() ( const double x ) const { return 2 * x ; }
      // ======================================================================
    } ;
    // ========================================================================
    //  Legendre 
    // ========================================================================
    template <unsigned int N> class  Legendre_   ;
    // ========================================================================
    /** @class Legendre_
     *  Efficient evaluator of Legendre polynomial
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2011-04-19
     */
    template <unsigned int N>
    class  Legendre_ : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /// the only one important method
      inline double operator() ( const double    x    ) const
      { return ( ( 2 * N - 1 ) * x * m_N1 ( x ) - ( N - 1 ) * m_N2 ( x ) ) / N ; }
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
    // Hermite
    // ========================================================================
    template <unsigned int N> class  Hermite_ ;
    // ========================================================================
    /** @class Hermite_
     *  Efficienct evaluator of Hermite polynomial
     *  These are "probabilistic" polinomials,
     *  \f$He(x)\f$ 
     *  such as coefficienst at maximar degree is always equal to 1 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
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
    // Non-templated 
    // ========================================================================
    /** @class Chebyshev
     *  evaluate the chebyshev polynomials
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2011-04-19
     */
    class GAUDI_API Chebyshev : public std::unary_function<double,double>
    {
    public :
      // ======================================================================
      /// constructor
      Chebyshev ( const unsigned int N = 0 ) : m_N ( N ) {}
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the polynomial
      double operator() ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      unsigned int degree () const { return m_N ; }
      // ======================================================================
    public:
      // ======================================================================
      /// derivative 
      double derivative ( const double x    ) const ;
      // ======================================================================
      /// get integral between low and high 
      double integral   ( const double low  , 
                          const double high ) const ;
      // ======================================================================
    private:
      // ======================================================================
      unsigned int m_N ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ChebyshevU
     *  evaluate the chebyshev polynomials of the second kind 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2011-04-19
     */
    class GAUDI_API ChebyshevU : public std::unary_function<double,double>
    {
    public :
      // ======================================================================
      /// constructor
      ChebyshevU ( const unsigned int N = 0 ) : m_N ( N ) {}
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the polynomial
      double operator() ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      unsigned int degree () const { return m_N ; }
      // ======================================================================
    public:
      // ======================================================================
      /// derivative 
      double derivative ( const double x ) const ;
      // ======================================================================
      /// get integral between low and high 
      double integral   ( const double low  , 
                          const double high ) const ;
      // ======================================================================
    private:
      // ======================================================================
      unsigned int m_N ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Legendre
     *  evaluate the Legendre polynomials
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2011-04-19
     */
    class GAUDI_API Legendre : public std::unary_function<double,double>
    {
    public :
      // ======================================================================
      /// constructor
      Legendre ( const unsigned int N = 0  ) : m_N ( N ) {}
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the polynomial
      double operator() ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      unsigned int degree () const { return m_N ; }
      // ======================================================================
    public:
      // ======================================================================
      /// derivative 
      double derivative  ( const double x ) const ;
      // ======================================================================
      /// get integral between low and high 
      double integral    ( const double low  , 
                           const double high ) const ;
      // ======================================================================
    private:
      // ======================================================================
      unsigned int m_N ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Hermite
     *  evaluate the Hermite polynomials
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2011-04-19
     */
    class GAUDI_API Hermite : public std::unary_function<double,double>
    {
    public :
      // ======================================================================
      /// constructor
      Hermite ( const unsigned int N = 0 ) : m_N ( N ) {}
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the polynomial
      double operator() ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      unsigned int degree () const { return m_N ; }
      // ======================================================================
    private:
      // ======================================================================
      unsigned int m_N ;
      // ======================================================================
    } ;
    // ========================================================================
    /** affine transformation of polynomial
     *  \f$ x ^{\prime} = \alpha x + \beta \f$
     *  @param input  (INPUT)  input polynomial coefficients 
     *  @param output (UPDATE) coefficients of transformed polynomial 
     *  @param alpha  (INPUT)  alpha
     *  @param beta   (INPUT)  beta
     *  @return true for valid transformations
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-03-09
     */
    GAUDI_API 
    bool affine_transform
    ( const std::vector<double>& input      , 
      std::vector<double>&       result     ,  
      const double               alpha  = 1 , 
      const double               beta   = 0 ) ;              
    // ========================================================================
    /// forward declarations 
    class Bernstein ; // forward declarations 
    // ========================================================================
    /** @class PolySum
     *  Base class for polynomial sums 
     *  \f$ f(x) = \sum_i \alpha_i P_i(x) \f$
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-10
     */
    class GAUDI_API PolySum : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /// constructor from polynomial degree 
      PolySum ( const unsigned short degree = 0 ) ;
      /// constructor from vector of parameters 
      PolySum ( const std::vector<double>&  pars ) ;
      /// constructor from sequence of parameters 
      template <class ITERATOR>
        PolySum ( ITERATOR begin , 
                  ITERATOR end   )
        : std::unary_function<double,double>() 
        , m_pars ( begin , end )
      { if ( m_pars.empty() ) { m_pars.push_back ( 0 ) ; } }
      // ======================================================================
      /// copy contructor
      PolySum ( const PolySum&  right ) ;
      /// move constructor 
      PolySum (       PolySum&& right ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// degree  of polynomial 
      unsigned short degree () const { return m_pars.size() - 1 ; }
      /// number of parameters 
      unsigned short npars  () const { return m_pars.size()     ; }
      /// all zero ?
      bool           zero   () const ;
      /** set k-parameter
       *  @param k index
       *  @param value new value 
       *  @return true if parameter is actually changed 
       */
      bool setPar          ( const unsigned short k , const double value ) ;
      /** set k-parameter
       *  @param k index
       *  @param value new value 
       *  @return true iof parameter is actually changed 
       */
      bool setParameter    ( const unsigned short k , const double value )
      { return setPar      ( k , value ) ; }
      /// get the parameter value
      double  par          ( const unsigned short k ) const
      { return ( k < m_pars.size() ) ? m_pars[k] : 0.0 ; }
      /// get the parameter value
      double  parameter    ( const unsigned short k ) const { return par ( k ) ; }
      /// get all parameters:
      const std::vector<double>& pars () const { return m_pars ; }
      // ======================================================================
    public: // simple  manipulations with polynoms 
      // ======================================================================
      /// simple  manipulations with polynoms: scale it! 
      PolySum& operator *= ( const double a ) ;     // scale it! 
      /// simple  manipulations with polynoms: scale it! 
      PolySum& operator /= ( const double a ) ;     // scale it! 
      // ======================================================================
    protected:
      // ======================================================================
      /// copy assignement  
      PolySum& operator=( const PolySum&  right ) ;
      /// move assignement 
      PolySum& operator=(       PolySum&& right ) ;
      // ======================================================================
    protected :
      // ======================================================================
      /// parameters 
      std::vector<double>  m_pars ; // parameters 
      // ======================================================================
    } ;  
    // ========================================================================
    /// forward declarations 
    class Bernstein    ; // forward declarations 
    class Polynomial   ; // forward declarations 
    class LegendreSum  ; // forward declarations 
    class ChebyshevSum ; // forward declarations 
    class HermiteSum   ; // forward declarations 
    // ========================================================================
    // Polynomial sums
    // ========================================================================
    /** @class Polynomial
     *  Trivial polynomial
     *  \f$ f(x) = \sum_i \p_i x^i\f$
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-22
     */
    class GAUDI_API Polynomial : public PolySum 
    {
    public:
      // =====================================================================
      /// constructor from the degree  
      Polynomial ( const unsigned short       degree =   0  , 
                   const double               xmin   =  -1  , 
                   const double               xmax   =   1  ) ;
      // ======================================================================
      /// constructor from the parameter list 
      Polynomial ( const std::vector<double>& pars          , 
                   const double               low    =  -1  , 
                   const double               high   =   1  ) ;
      /// template constructor from sequence of parameters 
      template <class ITERATOR>
        Polynomial ( ITERATOR                 first , 
                     ITERATOR                 last  , 
                     const double             xmin  , 
                     const double             xmax  ) 
        : Gaudi::Math::PolySum ( first , last ) 
        , m_xmin ( std::min ( xmin, xmax ) )
        , m_xmax ( std::max ( xmin, xmax ) )
      {}
      // ======================================================================
      /// copy 
      Polynomial ( const Polynomial&  ) = default ;
      /// move 
      Polynomial (       Polynomial&& ) = default ;
      // ======================================================================
      ///  constructor from Bernstein polinomial (efficient) 
      explicit Polynomial ( const Bernstein&     poly ) ;
      ///  constructor from Legendre polinomial  (efficient)
      explicit Polynomial ( const LegendreSum&   poly ) ;
      ///  constructor from Chebyshev polinomial (delegation) 
      explicit Polynomial ( const ChebyshevSum&  poly ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the value
      double operator () ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get lower edge
      double xmin  () const { return m_xmin ; }
      /// get upper edge
      double xmax  () const { return m_xmax ; }
      // ======================================================================
    public:
      // ======================================================================
      double x ( const double t ) const
      { return  0.5 * ( t * ( m_xmax - m_xmin ) +   m_xmax + m_xmin ) ; }
      double t ( const double x ) const
      { return (  2 *   x   - m_xmax - m_xmin ) / ( m_xmax - m_xmin ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the integral between xmin and xmax
      double integral   () const ;
      /// get the integral between low and high 
      double integral   ( const double low , const double high ) const ;
      /// get the derivative at point "x" 
      double derivative ( const double x     ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get indefinte integral 
      Polynomial indefinite_integral ( const double C = 0 ) const ;
      /// get the derivative
      Polynomial derivative          () const ;
      // ======================================================================
     public:
      // ======================================================================
      /// simple  manipulations with polynoms: shift it! 
      Polynomial& operator += ( const double a ) ; 
      /// simple  manipulations with polynoms: shift it! 
      Polynomial& operator -= ( const double a ) ; 
      // ======================================================================
    private:
      // ======================================================================
      /// x-min 
      double              m_xmin ; // x-min
      /// x-max 
      double              m_xmax ; // x-max
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class ChebyshevSum 
     *  Sum of chebychev polinomials 
     *  \f$ f(x) = \sum_i \p_i T_i(x)\f$
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-22
     */
    class GAUDI_API ChebyshevSum : public PolySum 
    {
    public:
      // =====================================================================
      /// constructor from the degree  
      ChebyshevSum ( const unsigned short       degree =  0 , 
                     const double               xmin   = -1 , 
                     const double               xmax   =  1 )  ;
      // ======================================================================
      /// constructor from the parameter list 
      ChebyshevSum ( const std::vector<double>& pars       , 
                     const double               xmin  = -1 , 
                     const double               xmax  =  1 )  ;
      /// template constructor from sequence of parameters 
      template <class ITERATOR>
        ChebyshevSum ( ITERATOR                 first , 
                       ITERATOR                 last  , 
                       const double             xmin  , 
                       const double             xmax  ) 
        : Gaudi::Math::PolySum ( first , last ) 
        , m_xmin ( std::min ( xmin, xmax ) )
        , m_xmax ( std::max ( xmin, xmax ) )
      {}
      // ======================================================================
      /// copy 
      ChebyshevSum ( const ChebyshevSum&  ) = default ;
      /// copy 
      ChebyshevSum (       ChebyshevSum&& ) = default ;
      // ======================================================================
      ///  constructor from Polinomial           (efficient)
      explicit ChebyshevSum ( const Polynomial&  poly ) ;
      ///  constructor from Bernstein            (delegation) 
      explicit ChebyshevSum ( const Bernstein&   poly ) ;
      ///  constructor from Legendre             (delegation) 
      explicit ChebyshevSum ( const LegendreSum& poly ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the value
      double operator () ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get lower edge
      double xmin () const { return m_xmin ; }
      /// get upper edge
      double xmax () const { return m_xmax ; }
      // ======================================================================
    public:
      // ======================================================================
      double x ( const double t ) const
      { return  0.5 * ( t * ( m_xmax - m_xmin ) +   m_xmax + m_xmin ) ; }
      double t ( const double x ) const
      { return (  2 *   x   - m_xmax - m_xmin ) / ( m_xmax - m_xmin ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the integral between xmin and xmax
      double integral   () const ;
      /// get the integral between low and high 
      double integral   ( const double low , const double high ) const ;
      /// get the derivative at point "x" 
      double derivative ( const double x     ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get indefinte integral 
      ChebyshevSum indefinite_integral ( const double C = 0 ) const ;
      /// get the derivative 
      ChebyshevSum derivative          () const ;
      // ======================================================================
    public:
      // ======================================================================
      /// simple  manipulations with polynoms: shift it! 
      ChebyshevSum& operator += ( const double a ) ; 
      /// simple  manipulations with polynoms: shift it! 
      ChebyshevSum& operator -= ( const double a ) ; 
      // ======================================================================
    private:
      // ======================================================================
      /// x-min 
      double              m_xmin ; // x-min
      /// x-max 
      double              m_xmax ; // x-max
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class LegendreSum 
     *  Sum of Legendre polinomials 
     *  \f$ f(x) = \sum_i \p_i P_i(x)\f$
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-22
     */
    class GAUDI_API LegendreSum : public PolySum
    {
    public:
      // =====================================================================
      /// constructor from the degree 
      LegendreSum ( const unsigned short        degree =  0 , 
                    const double                xmin   = -1 , 
                    const double                xmax   =  1 )  ;
      // ======================================================================
      /// constructor from the parameter list 
      LegendreSum ( const std::vector<double>&  pars       , 
                    const double                xmin   = -1 , 
                    const double                xmax   =  1 )  ;
      /// template constructor from sequence of parameters 
      template <class ITERATOR>
        LegendreSum ( ITERATOR                  first , 
                      ITERATOR                  last  , 
                      const double              xmin  , 
                      const double              xmax  ) 
        : Gaudi::Math::PolySum ( first , last ) 
        , m_xmin ( std::min ( xmin, xmax ) )
        , m_xmax ( std::max ( xmin, xmax ) )
      {}
      // ======================================================================
      /// copy 
      LegendreSum  ( const LegendreSum&  ) = default ;
      /// move
      LegendreSum  (       LegendreSum&& ) = default ;
      // ======================================================================
      /**  constructor from Bernstein polinomial (efficient)
       *  @see http://www.sciencedirect.com/science/article/pii/S0377042700003769 eq.21
       */
      explicit LegendreSum ( const Bernstein&     poly ) ;
      /// constructor from polynoimial            (delegation)
      explicit LegendreSum ( const Polynomial&    poly ) ;
      /// constructor from Chebyshev              (delegation)
      explicit LegendreSum ( const ChebyshevSum&  poly ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the value
      double operator () ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get lower edge
      double xmin () const { return m_xmin ; }
      /// get upper edge
      double xmax () const { return m_xmax ; }
      // ======================================================================
    public:
      // ======================================================================
      double x ( const double t ) const
      { return  0.5 * ( t * ( m_xmax - m_xmin ) +   m_xmax + m_xmin ) ; }
      double t ( const double x ) const
      { return (  2 *   x   - m_xmax - m_xmin ) / ( m_xmax - m_xmin ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the integral between xmin and xmax
      double integral   () const ;
      /// get the integral between low and high 
      double integral   ( const double low , const double high ) const ;
      /// get the derivative at point "x" 
      double derivative ( const double x     ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get indefinte integral 
      LegendreSum indefinite_integral ( const double C = 0 ) const ;
      /// get the derivative 
      LegendreSum derivative          () const ;
      // ======================================================================
    public:
      // ======================================================================
      /// simple  manipulations with polynoms: shift it! 
      LegendreSum& operator += ( const double a ) ;
      /// simple  manipulations with polynoms: shift it! 
      LegendreSum& operator -= ( const double a ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// x-min 
      double              m_xmin ; // x-min
      /// x-max 
      double              m_xmax ; // x-max
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class HermiteSum 
     *  Sum of Hermitepolinomials 
     *  \f$ f(x) = \sum_i \p_i He_i(x)\f$
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-08-08
     */
    class GAUDI_API HermiteSum : public PolySum
    {
    public:
      // =====================================================================
      /// constructor from the degree 
      HermiteSum ( const unsigned short       degree =   0  ,
                   const double               xmin   =  -1  , 
                   const double               xmax   =   1  ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the value
      double operator () ( const double x ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get lower edge
      double xmin () const { return m_xmin ; }
      /// get upper edge
      double xmax () const { return m_xmax ; }
      // ======================================================================
    public:
      // ======================================================================
      double x ( const double t ) const 
      { return 0.5 * ( t / m_scale + m_xmin + m_xmax ) ; }
      double t ( const double x ) const 
      { return m_scale * ( 2 * x   - m_xmin - m_xmax ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the integral between low and high 
      double integral   ( const double low , const double high ) const ;
      /// get the derivative at point "x" 
      double derivative ( const double x     ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get indefinte integral 
      HermiteSum indefinite_integral ( const double C = 0 ) const ;
      /// get the derivative 
      HermiteSum derivative          () const ;
      // ======================================================================
    public:
      // ======================================================================
      /// simple  manipulations with polynoms: shift it! 
      HermiteSum& operator += ( const double a ) ;
      /// simple  manipulations with polynoms: shift it! 
      HermiteSum& operator -= ( const double a ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// low  edge 
      double m_xmin  ; // low  edge 
      /// high edge 
      double m_xmax  ; // high edge 
      /// scale 
      double m_scale ; // scale 
      // ======================================================================      
    } ;
  } //                                             end of namespace Gaudi::Math
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Math 
  {
    // ========================================================================
    // helper utilities for integration of product of polynomial and an exponent
    // ========================================================================
    /** get the integral between low and high for a product of Bernstein
     *  polynom and the exponential function with the exponent tau
     *  \f[  \int_{a}^{b} \mathcal{B} e^{\tau x } \mathrm{d}x \f] 
     *  @param poly  bernstein polynomial
     *  @param tau   slope parameter for exponential 
     *  @param a     low  integration range 
     *  @param b     high integration range 
     */
    GAUDI_API 
    double integrate 
    ( const Gaudi::Math::Bernstein& poly ,
      const double                  tau  ,
      const double                  a    , 
      const double                  b    ) ;
    // ========================================================================    
    /** get the integral between low and high for a product of
     *  polynom and the exponential function with the exponent tau
     *  \f[  \int_{a}^{b} \mathcal{P} e^{\tau x } \mathrm{d}x \f] 
     *  @param poly  polynomial
     *  @param tau   slope parameter for exponential 
     *  @param a     low  integration range 
     *  @param b     high integration range 
     */
    GAUDI_API 
    double integrate 
    ( const Gaudi::Math::Polynomial& poly ,
      const double                   tau  ,
      const double                   a    , 
      const double                   b    ) ;
    // ========================================================================    
    /** get the integral between low and high for a product of
     *  Chebyshev polynom and the exponential function with the exponent tau
     *  \f[  \int_{a}^{b} \mathcal{T} e^{\tau x } \mathrm{d}x \f] 
     *  @param poly  chebyshev polynomial
     *  @param tau   slope parameter for exponential 
     *  @param a     low  integration range 
     *  @param b     high integration range 
     */
    GAUDI_API 
    double integrate 
    ( const Gaudi::Math::ChebyshevSum& poly ,
      const double                     tau  ,
      const double                     a    , 
      const double                     b    ) ;
    // ========================================================================    
    /** get the integral between low and high for a product of
     *  Legendre polynom and the exponential function with the exponent tau
     *  \f[  \int_{a}^{b} \mathcal{L} e^{\tau x } \mathrm{d}x \f] 
     *  @param poly  Legendre polynomial
     *  @param tau   slope parameter for exponential 
     *  @param a     low  integration range 
     *  @param b     high integration range 
     */
    GAUDI_API 
    double integrate 
    ( const Gaudi::Math::LegendreSum& poly ,
      const double                    tau  ,
      const double                    a    , 
      const double                    b    ) ;
    // ========================================================================    

    // ========================================================================    
    // special cases:
    // ========================================================================    
    
    // ========================================================================    
    /** get the integral between low and high for a product of
     *  polynom and the exponential function with the exponent tau
     *  \f[  \int_{a}^{b} \mathcal{P} e^{\tau x } \mathrm{d}x \f] 
     *  @param poly  polynomial
     *  @param tau   slope parameter for exponential 
     */
    GAUDI_API 
    double integrate 
    ( const Gaudi::Math::Polynomial& poly ,
      const double                   tau  ) ;
    // ========================================================================    
    
    // ========================================================================    
  } //                                             end of namespace Gaudi::Math 
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LHCBMATH_POLYNOMIALS_H
// ============================================================================
