// $Id$ 
// ============================================================================
#ifndef LHCBMATH_BERNSTEIN_H 
#define LHCBMATH_BERNSTEIN_H 1
// ============================================================================
// Include files
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
// Local
// ============================================================================
#include "LHCbMath/NSphere.h"
#include "LHCbMath/Polynomials.h"
// ============================================================================
/** @file LHCbMath/Bernstein.h
 *  Set of useful math-functions, related to Bernstein polynomials 
 *
 *  @see http://en.wikipedia.org/wiki/Bernstein_polynomial
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
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
    /// forward declaration 
    class LegendreSum  ; // forward declaration 
    class ChebyshevSum ; // forward declaration 
    class Polynomial   ; // forward declaration 
    // ========================================================================
    /** @class Bernstein
     *  The sum of bernstein's polynomial of order N
     *  \f$f(x) = \sum_i a_i B^n_i(x)\f$, where 
     *  \f$ B^n_k(x) = C^n_k x^k(1-x)^{n-k}\f$ 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     */
    class GAUDI_API Bernstein : public Gaudi::Math::PolySum 
    {
      // ======================================================================
    public:
      // ======================================================================
      /// helper structure to denote the basic Bernstein polynomials B(k,N)
      class GAUDI_API Basic 
      {
      public:
        // ====================================================================
        explicit  
          Basic ( const unsigned short k = 0 , 
                  const unsigned short N = 0 ) 
          : m_k ( k ) 
          , m_N ( N ) 
        {}
        // ====================================================================
      public :
        // ====================================================================
        unsigned short k () const { return m_k ; }
        unsigned short N () const { return m_N ; }
        // ====================================================================
      private: 
        // ====================================================================
        unsigned short m_k ;
        unsigned short m_N ;
        // ====================================================================
      } ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the order
      Bernstein ( const unsigned short        N     = 0 ,
                  const double                xmin  = 0 ,
                  const double                xmax  = 1 ) ;
      // ======================================================================
      /// constructor from N+1 coefficients
      Bernstein ( const std::vector<double>&  pars      ,
                  const double                xmin  = 0 ,
                  const double                xmax  = 1 ) ;
      // ======================================================================
      /// construct the basic bernstein polinomial  B(k,N)
      Bernstein  ( const Basic&              basic     ,
                   const double              xmin  = 0 , 
                   const double              xmax  = 1 ) ;
      // ======================================================================
      /// template constructor from sequence of parameters 
      template <class ITERATOR>
        Bernstein ( ITERATOR                 first , 
                    ITERATOR                 last  , 
                    const double             xmin  , 
                    const double             xmax  ) 
        : Gaudi::Math::PolySum ( first , last ) 
        , m_xmin ( std::min ( xmin, xmax ) )
        , m_xmax ( std::max ( xmin, xmax ) )
      {}
      // ======================================================================
      /// copy
      Bernstein ( const Bernstein&  ) = default ;
      /// move
      Bernstein (       Bernstein&& ) = default ;
      // ======================================================================
      /** constructor from Legendre polynomial
       *  @see http://www.sciencedirect.com/science/article/pii/S0377042700003769 eq.20 
       */
      explicit Bernstein ( const LegendreSum&  poly ) ;
      // ======================================================================
      /// constructor from Chebyshev polynomial
      explicit Bernstein ( const ChebyshevSum& poly ) ;
      // ======================================================================
      /// constructor from simple monomial form 
      explicit Bernstein ( const Polynomial&   poly ) ;
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
      /// get minimal value of the function on (xmin,xmax) interval 
      double fun_min       () const ;
      /// get maximal value of the function on (xmin,xmax) interval 
      double fun_max       () const ;
      // ======================================================================
    public:
      // ======================================================================
      /// positive      function ?
      bool   positive      () const ;
      /// negative      function ?
      bool   negative      () const ; 
      /// non-positive  function ?
      bool   nonpositive   () const ; 
      /// non-negative  function ?
      bool   nonnegative   () const ; 
      ///  has roots at [xmin,xmax] ? 
      bool   hasroots      () const { return   !positive () &&   !negative () ; }
      ///  keep sign at [xmin,xmax] ? 
      bool   keepsign      () const { return nonpositive () || nonnegative () ; }
      /// is it a decreasing function?
      bool   decreasing    () const ;
      /// is it a increasing function?
      bool   increasing    () const ;
      /// is it a monothonical function?
      bool   monothonic    () const { return increasing() || decreasing() ; }
      /// is it a constant function?
      bool   constant      () const ;
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
      double    integral   () const ;
      /// get the integral between low and high 
      double    integral   ( const double low , const double high ) const ;
      /** get indefinite integral  as function object 
       *  \f$ I(x) = \int^{x}_{x_{min}} B(t) dt + C \f$
       *  @param C the integration constant   
       */
      Bernstein indefinite_integral ( const double C = 0 ) const ;
      /// get the derivative at point "x" 
      double    derivative          ( const double x     ) const ;
      /// get derivative as function object 
      Bernstein derivative          () const ;
      // ======================================================================
    public : 
      // ======================================================================
      /** elevate it: 
       *  represent as Bernstein polynomial of order N+r 
       *  @param r  INPUT increase of degree 
       *  @return new polynomial of order N+r 
       */
      Bernstein elevate ( const unsigned short r ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// simple  manipulations with polynoms: shift it! 
      Bernstein& operator += ( const double a ) ;
      /// simple  manipulations with polynoms: shift it!
      Bernstein& operator -= ( const double a ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the underlying Bernstein polynomial  (self here)
      const Gaudi::Math::Bernstein& bernstein () const { return *this ; }
      // ======================================================================      
    public:
      // ======================================================================
      /// copy assignement  
      Bernstein& operator=( const Bernstein&  right ) ;
      /// move assignement 
      Bernstein& operator=(       Bernstein&& right ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// the left edge of interval
      double m_xmin  ;                             // the left edge of interval
      /// the right edge of interval
      double m_xmax  ;                             // the right edge of interval
      // ======================================================================
    };
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
    /** get the integral between 0 and 1 for a product of basic  Bernstein
     *  polynom and the exponential function with the exponent tau
     *  \f[  \int_{0}^{1} \mathcal{B} e^{\tau x } \mathrm{d}x \f] 
     *  @param b     basic bernstein polynomial
     *  @param tau   slope parameter for exponential 
     */
    GAUDI_API 
    double integrate 
    ( const Gaudi::Math::Bernstein::Basic& b    ,
      const double                         tau  ) ;    
    // =======================================================================
    /** get the integral between \f$x_{min}\f$ and \f$x_{max}\f$ for 
     *  a product of Bernstein polynom and the exponential function 
     *   with the exponent tau
     *  \f[  \int_{x_{min}}^{x_{max}} \mathcal{B} e^{\tau x } \mathrm{d}x \f] 
     *  @param poly  bernstein polynomial
     *  @param tau   slope parameter for exponential 
     */
    GAUDI_API 
    double integrate 
    ( const Gaudi::Math::Bernstein& poly ,
      const double                  tau  ) ;
    // ========================================================================
    /** get the integral between 0 and 1 for a product of basic  Bernstein
     *  polynom and monomial or degree m 
     *  \f[  \int_{0}^{1} \mathcal{B} \frac{x^m}{m!} \mathrm{d}x \f] 
     *  @param b     basic bernstein polynomial
     *  @param m     degree of monomial 
     */
    GAUDI_API
    double integrate_poly 
    ( const Gaudi::Math::Bernstein::Basic& b ,
      const unsigned short                 m ) ;
    // =======================================================================
    /** get the integral between xmin and xmax Bernstein
     *  polynom and monomial or degree m 
     *  \f[  \int_{x_min}^{x_max} \mathcal{B} \frac{(x-x_min)^m}{m!} \mathrm{d}x \f] 
     *  @param b     basic bernstein polynomial
     *  @param m     degree of monomial 
     */
    GAUDI_API
    double integrate_poly 
    ( const Gaudi::Math::Bernstein& b ,
      const unsigned short          m ) ;
    // ========================================================================
    /** get the integral between xmin and xmax Bernstein
     *  polynom and monomial or degree m 
     *  \f[  \int_{low}^{high} \mathcal{B} \frac{(x-x_min)^m}{m!} \mathrm{d}x \f] 
     *  @param b     basic bernstein polynomial
     *  @param m     degree of monomial 
     *  @param low   low  integration limit 
     *  @param high  high integtation limit 
     */
    GAUDI_API
    double integrate_poly 
    ( const Gaudi::Math::Bernstein& b    ,
      const unsigned short          m    , 
      const double                  low  , 
      const double                  high ) ;
    // =======================================================================
    /** de Casteljau algorithm for summation of Bernstein polynomials 
     *  \f$ f(x) = \sum_i p_i B_ik(x) \f$
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-10
     */
    GAUDI_API double casteljau
    ( const std::vector<double>& pars , 
      const double               x    ) ;
    // ========================================================================
    /** @class Positive
     *  The "positive" polynomial of order N
     *  Actually it is a sum of basic bernstein polynomials with
     *  non-negative coefficients
     *  \f$ f(x) = \sum_i  \alpha^2_i B^n_i(x)\f$, where 
     *  \f$  \sum_i \alpha^2_i = 1\f$ and 
     *  parameters \f$ \alpha_i(p_0,p_1,....p_{n-1})\f$ form 
     *  n-dimension sphere 
     */
    class GAUDI_API Positive : public std::unary_function<double,double>
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the order
      Positive ( const unsigned short       N     =  1 ,
                 const double               xmin  =  0 ,
                 const double               xmax  =  1 ) ;
      // ======================================================================
      /// constructor from N phases
      Positive ( const std::vector<double>& pars       ,
                 const double               xmin  =  0 ,
                 const double               xmax  =  1 ) ;
      // ======================================================================
      /// copy 
      Positive ( const Positive&  right ) ;
      /// move 
      Positive (       Positive&& right ) ;    
      // ======================================================================
      virtual ~Positive () ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the value
      double operator () ( const double x ) const { return m_bernstein ( x ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get number of parameters
      std::size_t npars () const { return m_sphere.nPhi () ; }
      /// set k-parameter
      bool setPar       ( const unsigned short k , const double value ) ;
      /// set k-parameter
      bool setParameter ( const unsigned short k , const double value )
      { return setPar   ( k , value ) ; }
      /// get the parameter value 
      double  par       ( const unsigned short k ) const 
      { return m_sphere.par ( k ) ; }
      /// get all parameters (phases on sphere) 
      const std::vector<double>& pars  () const { return m_sphere   .pars () ; }
      /// get bernstein coefficients
      const std::vector<double>& bpars () const { return m_bernstein.pars () ; }
      // ======================================================================
    public:  // some characteristcs 
      // ======================================================================
      /// degree
      unsigned short degree      () const { return m_bernstein.degree() ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the parameter value
      double  parameter ( const unsigned short k ) const { return par ( k ) ; }
      /// get lower edge
      double xmin () const { return m_bernstein.xmin () ; }
      /// get upper edge
      double xmax () const { return m_bernstein.xmax () ; }
      /// transform variables 
      double x ( const double t ) const { return m_bernstein. x ( t )  ; }
      double t ( const double x ) const { return m_bernstein. t ( x )  ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get minimal value of the function on (xmin,xmax) interval 
      double fun_min   () const { return m_bernstein.fun_min   () ; }
      /// get maximal value of the function on (xmin,xmax) interval 
      double fun_max   () const { return m_bernstein.fun_max   () ; }
      // ======================================================================
      bool positive    () const { return m_bernstein.positive () ; }
      bool hasroots    () const { return !positive() ; }
      //
      bool   negative  () const { return false ; }
      bool nonegative  () const { return true  ; }
      bool nonpositive () const { return false ; }
      bool keepsign    () const { return true  ; }
      //
      bool decreasing  () const { return m_bernstein.decreasing()     ; }
      bool increasing  () const { return m_bernstein.increasing()     ; }
      bool monothonic  () const { return increasing() || decreasing() ; }
      bool constant    () const { return m_bernstein.constant()       ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the integral between xmin and xmax
      double integral   () const ;
      /// get the integral between low and high 
      double integral   ( const double low , const double high ) const ;
      /// get the derivative 
      double derivative ( const double x ) const 
      { return m_bernstein.derivative ( x ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the underlying Bernstein polynomial
      const Gaudi::Math::Bernstein& bernstein () const { return m_bernstein ; }
      /// get the parameter sphere 
      const Gaudi::Math::NSphere&   sphere    () const { return m_sphere    ; }
      /// get the indefinite integral 
      Bernstein indefinite_integral ( const double C = 0 ) const 
      { return m_bernstein.indefinite_integral ( C ) ; }
      /// get the derivative 
      Bernstein derivative          () const 
      { return m_bernstein.derivative          () ; }
      // ======================================================================
    public:
      // ======================================================================
      /// copy assignement 
      Positive& operator=( const Positive&  right ) ;
      /// move assignement 
      Positive& operator=(       Positive&& right ) ;
      // ======================================================================
    protected:
      // ======================================================================
      /// update bernstein coefficinects
      virtual bool updateBernstein () ;
      // ======================================================================
    protected:
      // ======================================================================
      /// the actual bernstein polynomial
      Gaudi::Math::Bernstein m_bernstein ; // the actual bernstein polynomial
      /// arameters sphere 
      Gaudi::Math::NSphere   m_sphere    ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Monothonic
     *  The "positive" monothonic polynomial of order N
     *  Actually it is a sum of basic bernstein polynomials with
     *  non-negative coefficients
     */
    class GAUDI_API Monothonic : public Gaudi::Math::Positive 
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the order
      Monothonic
        ( const unsigned short       N          =    1 ,
          const double               xmin       =    0 ,
          const double               xmax       =    1 , 
          const bool                 increasing = true ) ;
      // ======================================================================
      /// constructor from N phases
      Monothonic
        ( const std::vector<double>& pars              ,
          const double               xmin       =    0 ,
          const double               xmax       =    1 ,
          const bool                 increasing = true ) ;
      // ======================================================================
      /// constructor positive spline 
      Monothonic
        ( const Positive&            poly              ,
          const bool                 increasing        ) ;
      // ======================================================================
      /// copy  constructor  
      Monothonic ( const Monothonic&  right ) ;
      /// move 
      Monothonic (       Monothonic&& right ) = default ;
      // ======================================================================
      virtual ~Monothonic() ;
      // ======================================================================      
    public:
      // ======================================================================
      /// increasing ? 
      bool increasing () const { return  m_increasing  ; }
      /// decreasing ? 
      bool decreasing () const { return !increasing () ; }
      /// monothonic 
      bool monothonic () const { return  true  ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// update bernstein coefficients
      virtual bool updateBernstein () ;
      // ======================================================================
    protected:
      // ======================================================================
      /// increasing ? 
      bool                   m_increasing ; // increasing ? 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Convex
     *  The "positive" polynomial of order N with 
     *  fixed sign of first and second derivatives 
     *  Actually it is a sum of basic bernstein polynomials with
     *  non-negative coefficients
     */
    class GAUDI_API Convex : public Gaudi::Math::Monothonic 
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the order
      Convex 
        ( const unsigned short       N          =    1 ,
          const double               xmin       =    0 ,
          const double               xmax       =    1 , 
          const bool                 increasing = true ,
          const bool                 convex     = true ) ;
      // ======================================================================
      /// constructor from N phases
      Convex 
        ( const std::vector<double>& pars              ,
          const double               xmin       =    0 ,
          const double               xmax       =    1 ,
          const bool                 increasing = true ,
          const bool                 convex     = true ) ;
      // ======================================================================
      /// constructor from polynom
      Convex 
        ( const Positive&            poly       , 
          const bool                 increasing ,
          const bool                 convex     ) ;
      // ======================================================================
      /// constructor from polynom
      Convex 
        ( const Monothonic&          poly      , 
          const bool                 convex    ) ;
      // ======================================================================
      /// copy constructor
      Convex ( const Convex&         right     ) ;
      /// move 
      Convex (       Convex&&        right     ) = default ;
      // ======================================================================
      virtual ~Convex() ;
      // ======================================================================
    public:
      // ======================================================================
      /// convex     ? 
      bool   convex    () const { return  m_convex    ; }
      /// convex     ? 
      bool   concave   () const { return   !convex () ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// update bernstein coefficients
      virtual bool updateBernstein () ;
      // ======================================================================
    protected:
      // ======================================================================
      /// convex ? 
      bool                   m_convex     ; // iconvex ? 
      // ======================================================================
    } ;
    // ========================================================================

    // ========================================================================
    /** @class ConvexOnly
     *  The "positive" polynomial of order N with 
     *  fixed sign the second derivatives 
     *  Actually it is a sum of basic bernstein polynomials with
     *  non-negative coefficients
     */
    class GAUDI_API ConvexOnly : public Gaudi::Math::Positive 
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the order
      ConvexOnly 
        ( const unsigned short       N          =    1 ,
          const double               xmin       =    0 ,
          const double               xmax       =    1 , 
          const bool                 convex     = true ) ;
      // ======================================================================
      /// constructor from N phases
      ConvexOnly 
        ( const std::vector<double>& pars              ,
          const double               xmin       =    0 ,
          const double               xmax       =    1 ,
          const bool                 convex     = true ) ;
      // ======================================================================
      /// constructor from polynom
      ConvexOnly 
        ( const Positive&            poly       , 
          const bool                 convex     ) ;
      // ======================================================================
      /// copy constructor
      ConvexOnly ( const ConvexOnly&   right     ) ;
      /// move 
      ConvexOnly (       ConvexOnly&&  right     ) = default ;
      // ======================================================================
      virtual ~ConvexOnly() ;
      // ======================================================================
    public:
      // ======================================================================
      /// convex     ? 
      bool   convex    () const { return  m_convex    ; }
      /// convex     ? 
      bool   concave   () const { return   !convex () ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// update bernstein coefficients
      virtual bool updateBernstein () ;
      // ======================================================================
    protected:
      // ======================================================================
      /// convex ? 
      bool                   m_convex     ; // iconvex ? 
      // ======================================================================
    } ;
    // ========================================================================

    // ========================================================================
    // 2D-models
    // ========================================================================
    /** @class Bernstein2D
     *  The Bernstein's polynomial of order Nx*Ny
     */
    class GAUDI_API Bernstein2D 
      : public std::binary_function<double,double,double>
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the order
      Bernstein2D ( const unsigned short       nX    =  1 ,
                    const unsigned short       nY    =  1 ,
                    const double               xmin  =  0 ,
                    const double               xmax  =  1 ,
                    const double               ymin  =  0 ,
                    const double               ymax  =  1 ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the value
      double operator () ( const double x ,
                           const double y ) const ;
      // ======================================================================
    public: // setters 
      // ======================================================================
      /// set k-parameter
      bool setPar       ( const unsigned int   k     ,
                          const double         value ) ;
      /// set k-parameter
      bool setParameter ( const unsigned int   k     ,
                          const double         value ) 
      { return ( k < m_pars.size() ) && setPar ( k , value ) ; }
      /// set (l,m)-parameter
      bool setPar       ( const unsigned short l     ,
                          const unsigned short m     , 
                          const double         value ) ;
      /// set (l,m)-parameter
      bool setParameter ( const unsigned short l     ,
                          const unsigned short m     , 
                          const double         value ) 
      { return setPar   ( l , m  , value ) ; }
      // ======================================================================
    public: // getters  
      // ======================================================================
      /// get (l,m)-parameter 
      double  par       ( const unsigned short l ,
                          const unsigned short m ) const ;
      /// get (l,m)-parameter 
      double  parameter ( const unsigned short l , 
                          const unsigned short m ) const { return par (  l , m  ) ; }
      /// get k-parameter 
      double  par       ( const unsigned int k ) const 
      { return k < m_pars.size() ? m_pars[k] : 0.0 ; }
      /// get k-parameter
      double  parameter ( const unsigned int k ) const { return par ( k ) ; }
      /// get all parameters at once 
      const std::vector<double>& pars() const { return m_pars ; }
      // ======================================================================      
    public:
      // ======================================================================
      /// get the actual number of parameters
      std::size_t npars () const { return m_pars.size() ; }
      /// get lower edge
      double xmin () const { return m_xmin ; }
      /// get upper edge
      double xmax () const { return m_xmax ; }
      /// get lower edge
      double ymin () const { return m_ymin ; }
      /// get upper edge
      double ymax () const { return m_ymax ; }
      /// get the polynomial order (X)
      unsigned short nX () const { return m_nx ; } 
      /// get the polynomial order (Y)
      unsigned short nY () const { return m_ny ; }      
      // ======================================================================
    public:  // transformations 
      // ======================================================================
      double x  ( const double tx ) const
      { return xmin ()  + ( xmax () - xmin () ) * tx ; }
      double y  ( const double ty ) const
      { return ymin ()  + ( ymax () - ymin () ) * ty ; }
      double tx ( const double x ) const
      { return  ( x - xmin () ) / ( xmax () - xmin () )      ; }
      double ty ( const double y ) const
      { return  ( y - ymin () ) / ( ymax () - ymin () )      ; }
      // ======================================================================
    public: // general integration 
      // ======================================================================
      /** get the integral over 2D-region 
       *  \f[ \int_{x_low}^{x_high}\int_{y_low}^{y_high} \mathcal{B}(x,y) \mathrm{d}x\mathrm{d}y\f] 
       *  @param xlow  low  edge in x 
       *  @param xhigh high edge in x 
       *  @param ylow  low  edge in y 
       *  @param yhigh high edge in y 
       */
      double integral   ( const double xlow , const double xhigh , 
                          const double ylow , const double yhigh ) const ;
      /** integral over x-dimension 
       *  \f[ \int_{x_{low}}^{x_{high}} \mathcal{B}(x,y) \mathrm{d}x\f] 
       *  @param x     variable 
       *  @param ylow  low  edge in y 
       *  @param yhigh high edge in y 
       */
      double integrateX ( const double y    , 
                          const double xlow , const double xhigh ) const ;
      /** integral over y-dimension 
       *  \f[ \int_{y_{low}}^{y_{high}} \mathcal{B}(x,y) \mathrm{d}y\f] 
       *  @param y     variable 
       *  @param xlow  low  edge in x 
       *  @param xhigh high edge in x 
       */
      double integrateY ( const double x    , 
                          const double ylow , const double yhigh ) const ;
      // ======================================================================
    public: // special cases
      // ======================================================================
      /** get the integral over 2D-region 
       *  \f[  x_min < x < x_max, y_min< y< y_max\f] 
       */
      double integral   () const ;
      /** integral over x-dimension
       *  \f[ \int_{x_{min}}^{x_{max}} \mathcal{B}(x,y) \mathrm{d}x\f] 
       *  @param x     variable 
       */
      double integrateX ( const double y    ) const ;
      /** integral over y-dimension 
       *  \f[ \int_{y_{min}}^{y_{max}} \mathcal{B}(x,y) \mathrm{d}y\f] 
       *  @param y     variable 
       */
      double integrateY ( const double x    ) const ;
      // ======================================================================      
    public: // few helper functions to expose internals 
      // ======================================================================
      /// evaluate the basic polynomials 
      double basicX ( const unsigned short i , const double         x ) const 
      { return ( i > m_nx || x < m_xmin || x < m_xmax ) ? 0.0 : m_bx[i](x) ; }
      /// evaluate the basic polynomials 
      double basicY ( const unsigned short i , const double         y ) const
      { return ( i > m_ny || y < m_ymin || y < m_ymax ) ? 0.0 : m_by[i](y) ; }
      /// expose some internals 
      const Bernstein& basicX ( const unsigned short i ) const { return m_bx[i] ; }
      /// expose some internals 
      const Bernstein& basicY ( const unsigned short i ) const { return m_by[i] ; }
      // ======================================================================
    private:
      // ======================================================================
      // polynom order in x-dimension 
      unsigned short m_nx ; // polynom order in x-dimension 
      // polynom order in y-dimension 
      unsigned short m_ny ; // polynom order in y-dimension 
      /// the list of parameters
      std::vector<double>  m_pars ;                // the list of parameters
      /// the left edge of interval
      double m_xmin  ;                             // the left edge of interval
      /// the right edge of interval
      double m_xmax  ;                             // the right edge of interval
      /// the left edge of interval
      double m_ymin  ;                             // the left edge of interval
      /// the right edge of interval
      double m_ymax  ;                             // the right edge of interval
      // ======================================================================
    private:
      // ======================================================================
      ///  vectors of basic  Bernstein polynomials 
      typedef std::vector<Bernstein>  VB ;
      ///  vector  of basic  Bernstein polynomials 
      VB m_bx ; //  vector  of basic  Bernetin polynomials 
      ///  vector  of basic  Bernstein polynomials 
      VB m_by ; //  vector  of basic  Bernetin polynomials 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Positive2D
     *  The "positive" 2D-polynomial of order Nx*Ny
     *  Actually it is a sum of basic bernstein 2D-polynomials with
     *  non-negative coefficients
     */
    class GAUDI_API Positive2D 
      : public std::binary_function<double,double,double>
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the order
      Positive2D ( const unsigned short       Nx    =  1 ,
                   const unsigned short       Ny    =  1 ,
                   const double               xmin  =  0 ,
                   const double               xmax  =  1 ,
                   const double               ymin  =  0 ,
                   const double               ymax  =  1 ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the value
      double operator () ( const double x , const double y ) const 
      { return m_bernstein ( x , y ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get number of parameters
      std::size_t npars () const { return m_sphere.nPhi () ; }
      /// set k-parameter
      bool setPar       ( const unsigned int k , const double value ) ;
      /// set k-parameter
      bool setParameter ( const unsigned int k , const double value )
      { return setPar   ( k , value ) ; }
      /// get the parameter value
      double  par       ( const unsigned int k ) const ;
      /// get the parameter value
      double  parameter ( const unsigned int k ) const { return par ( k ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get lower/upper edges
      double         xmin () const { return m_bernstein.xmin () ; }
      double         xmax () const { return m_bernstein.xmax () ; }
      double         ymin () const { return m_bernstein.ymin () ; }
      double         ymax () const { return m_bernstein.ymax () ; }
      // polynom order 
      unsigned short nX   () const { return m_bernstein.nX   () ; }
      unsigned short nY   () const { return m_bernstein.nY   () ; }      
      // ======================================================================
    public:
      // ======================================================================
      // transform variables 
      double tx ( const double  x ) const { return m_bernstein.tx (  x ) ; }
      double ty ( const double  y ) const { return m_bernstein.ty (  y ) ; }      
      double  x ( const double tx ) const { return m_bernstein. x ( tx ) ; }
      double  y ( const double ty ) const { return m_bernstein. y ( ty ) ; }      
      // ======================================================================
    public:
      // ======================================================================
      /** get the integral over 2D-region 
       *  \f[ \int_{x_low}^{x_high}\int_{y_low}^{y_high} \mathcal{B}(x,y) \mathrm{d}x\mathrm{d}y\f] 
       *  @param xlow  low  edge in x 
       *  @param xhigh high edge in x 
       *  @param ylow  low  edge in y 
       *  @param yhigh high edge in y 
       */
      double integral   ( const double xlow , const double xhigh , 
                          const double ylow , const double yhigh ) const ;
      /** integral over x-dimension 
       *  \f[ \int_{x_{low}}^{x_{high}} \mathcal{B}(x,y) \mathrm{d}x\f] 
       *  @param x     variable 
       *  @param xlow  low  edge in x 
       *  @param xhigh high edge in x 
       */
      double integrateX ( const double y    , 
                          const double xlow , const double xhigh ) const 
      { return m_bernstein.integrateX ( y , xlow , xhigh ) ; }
      /** integral over y-dimension 
       *  \f[ \int_{y_{low}}^{y_{high}} \mathcal{B}(x,y) \mathrm{d}y\f] 
       *  @param x     variable 
       *  @param ylow  low  edge in x 
       *  @param yhigh high edge in x 
       */
      double integrateY ( const double x    , 
                          const double ylow , const double yhigh ) const 
      { return m_bernstein.integrateY ( x , ylow , yhigh ) ; }
      // ======================================================================
    public: // specific 
      // ======================================================================
      /** get the integral over 2D-region           
       *  \f[ \int_{x_{min}}^{x_{max}}\int_{y_{min}}^{y_{max}} 
       *        \mathcal{B}(x,y) \mathrm{d}x\mathrm{d}y\f] 
       */
      double integral   () const ;
      /** integral over x-dimension 
       *  \f[ \int_{x_{min}}^{x_{max}} \mathcal{B}(x,y) \mathrm{d}x\f] 
       *  @param x     variable 
       *  @param xlow  low  edge in x 
       *  @param xhigh high edge in x 
       */
      double integrateX ( const double y    ) const 
      { return m_bernstein.integrateX ( y ) ; }
      /** integral over y-dimension 
       *  \f[ \int_{y_{min}}^{y_{max}} \mathcal{B}(x,y) \mathrm{d}y\f] 
       *  @param x     variable 
       *  @param ylow  low  edge in x 
       *  @param yhigh high edge in x 
       */
      double integrateY ( const double x    ) const 
      { return m_bernstein.integrateY ( x ) ; }
      // ======================================================================
    public: // ingeredients 
      // =====================================================================
      // get the bernstein polinomial in 2D 
      const  Gaudi::Math::Bernstein2D& bernstein () const 
      { return m_bernstein ; }
      /// get the parameter sphere  
      const  Gaudi::Math::NSphere&     sphere    () const 
      { return m_sphere ; }  
      // ======================================================================
    private:
      // ======================================================================
      /// update bernstein coefficients 
      bool updateBernstein () ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual bernstein polynomial
      Gaudi::Math::Bernstein2D m_bernstein ; // the actual bernstein polynomial
      /// the external parameter sphere 
      Gaudi::Math::NSphere     m_sphere    ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Bernstein2DSym
     *  The symmetric Bernstein's polynomial of order N*N
     */
    class GAUDI_API Bernstein2DSym 
      : public std::binary_function<double,double,double>
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the order
      Bernstein2DSym ( const unsigned short       n     =  1 ,
                       const double               xmin  =  0 ,
                       const double               xmax  =  1 ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the value
      double operator () ( const double x , 
                           const double y ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get number of parameters
      std::size_t npars () const { return m_pars.size() ; }
      /// set k-parameter
      bool setPar       ( const unsigned int   k     ,
                          const double         value ) ;
      /// set k-parameter
      bool setParameter ( const unsigned int   k     ,
                          const double         value ) 
      { return ( k < m_pars.size() ) && setPar ( k , value ) ; }
      /// set (l,m)-parameter
      bool setPar       ( const unsigned short l     ,
                          const unsigned short m     , 
                          const double         value ) ;
      /// set (l,m)-parameter
      bool setParameter ( const unsigned short l     ,
                          const unsigned short m     , 
                          const double         value ) 
      { return setPar   ( l , m  , value ) ; }
      /// get (l,m)-parameter 
      double  par       ( const unsigned short l ,
                          const unsigned short m ) const ;
      /// get (l,m)-parameter value
      double  parameter ( const unsigned short l , 
                          const unsigned short m ) const { return par (  l , m  ) ; }
      /// get k-parameter 
      double  par       ( const unsigned int   k ) const 
      { return k < m_pars.size() ? m_pars [k] : 0.0 ; }
      /// get k-parameter
      double  parameter ( const unsigned int   k ) const { return par ( k ) ; }
      /// get all parameters at once 
      const std::vector<double>& pars() const { return m_pars ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get lower edge
      double xmin () const { return m_xmin    ; }
      /// get upper edge
      double xmax () const { return m_xmax    ; }
      /// get lower edge
      double ymin () const { return   xmin () ; }
      /// get upper edge
      double ymax () const { return   xmax () ; }
      // ======================================================================
      unsigned short n  () const { return m_n  ; }
      unsigned short nX () const { return n () ; }
      unsigned short nY () const { return n () ; }      
      // ======================================================================
    public:
      // ======================================================================
      double x  ( const double tx ) const
      { return xmin ()  + ( xmax () - xmin () ) * tx ; }
      double y  ( const double ty ) const
      { return ymin ()  + ( ymax () - ymin () ) * ty ; }
      double tx ( const double x ) const
      { return  ( x - xmin () ) / ( xmax () - xmin () ) ; }
      double ty ( const double y ) const
      { return  ( y - ymin () ) / ( ymax () - ymin () ) ; }
      // ======================================================================
    public: // generic integrals 
      // ======================================================================
      /** get the integral over 2D-region 
       *  \f[ \int_{x_{low}}^{x_{high}}\int_{y_{low}}^{y_{high}} 
       *  \mathcal{B}(x,y) \mathrm{d}x\mathrm{d}y\f] 
       *  @param xlow  low  edge in x 
       *  @param xhigh high edge in x 
       *  @param ylow  low  edge in y 
       *  @param yhigh high edge in y 
       */
      double integral   ( const double xlow , const double xhigh , 
                          const double ylow , const double yhigh ) const ;
      /** integral over x-dimension 
       *  \f[ \int_{x_{low}}^{x_{high}} \mathcal{B}(x,y) \mathrm{d}x\f] 
       *  @param x     variable 
       *  @param xlow  low  edge in x 
       *  @param xhigh high edge in x 
       */
      double integrateX ( const double y    , 
                          const double xlow , const double xhigh ) const ;
      /** integral over y-dimension 
       *  \f[ \int_{y_{low}}^{x_{high}} \mathcal{B}(x,y) \mathrm{d}y\f] 
       *  @param y     variable 
       *  @param xlow  low  edge in x 
       *  @param xhigh high edge in x 
       */
      double integrateY ( const double x    , 
                          const double ylow , const double yhigh ) const ;
      // ======================================================================
    public: // specific integrals 
      // ======================================================================
      /** get the integral over 2D-region 
       *  \f[ \int_{x_{min}}^{x_{max}}\int_{y_{min}}^{y_{max}} 
       *  \mathcal{B}(x,y) \mathrm{d}x\mathrm{d}y\f] 
       */
      double integral   () const ;
      /** integral over x-dimension 
       *  \f[ \int_{x_{min}}^{x_{max}} \mathcal{B}(x,y) \mathrm{d}x\f] 
       *  @param x     variable 
       */
      double integrateX ( const double y ) const ;
      /** integral over y-dimension 
       *  \f[ \int_{y_{min}}^{x_{max}} \mathcal{B}(x,y) \mathrm{d}y\f] 
       *  @param y     variable 
       */
      double integrateY ( const double x ) const ;
      // ======================================================================
    public: // few helper functions to expose internals 
      // ======================================================================
      /// evaluate the basic polynomials 
      double basic  ( const unsigned short i , const double         x ) const 
      { return ( i > m_n || x < m_xmin || x < m_xmax ) ? 0.0 : m_b[i](x) ; }
      /// expose some internals 
      const Bernstein& basic ( const unsigned short i ) const { return m_b[i] ; }
      // ======================================================================
    private:
      // ======================================================================
      // polynom order
      unsigned short m_n  ; // polynom order in x-dimension 
      /// the list of parameters
      std::vector<double>  m_pars ;                // the list of parameters
      /// the left edge of interval
      double m_xmin  ;                             // the left edge of interval
      /// the right edge of interval
      double m_xmax  ;                             // the right edge of interval
      // ======================================================================
    private:
      // ======================================================================
      ///  vectors of basic  Bernstein polynomials 
      typedef std::vector<Bernstein>  VB ;
      ///  vector  of basic  Bernetin polynomials 
      VB m_b  ; //  vector  of basic  Bernstein polynomials 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Positive2DSym
     *  The "positive" symmetrical polynomial of order Nx*Ny
     *  Actually it is a sum of basic bernstein 2D-polynomials with
     *  non-negative coefficients
     */
    class GAUDI_API Positive2DSym 
      : public std::binary_function<double,double,double>
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the order
      Positive2DSym ( const unsigned short       Nx    =  1 ,
                      const double               xmin  =  0 ,
                      const double               xmax  =  1 ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the value
      double operator () ( const double x , const double y ) const 
      { return m_bernstein ( x , y ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get number of parameters
      std::size_t npars () const { return m_sphere.nPhi () ; }
      /// set k-parameter
      bool setPar       ( const unsigned int k , const double value ) ;
      /// set k-parameter
      bool setParameter ( const unsigned int k , const double value )
      { return setPar   ( k , value ) ; }
      /// get the parameter value
      double  par       ( const unsigned int k ) const ;
      /// get the parameter value
      double  parameter ( const unsigned int k ) const { return par ( k ) ; }
      /// get lower/upper edges
      double         xmin () const { return m_bernstein.xmin () ; }
      double         xmax () const { return m_bernstein.xmax () ; }
      double         ymin () const { return m_bernstein.ymin () ; }
      double         ymax () const { return m_bernstein.ymax () ; }
      // polynom order 
      unsigned short n    () const { return m_bernstein.n    () ; }
      unsigned short nX   () const { return m_bernstein.nX   () ; }
      unsigned short nY   () const { return m_bernstein.nY   () ; }      
      // ======================================================================
    public:
      // ======================================================================
      double tx ( const double  x ) const { return m_bernstein.tx (  x ) ; }
      double ty ( const double  y ) const { return m_bernstein.ty (  y ) ; }      
      double  x ( const double tx ) const { return m_bernstein. x ( tx ) ; }
      double  y ( const double ty ) const { return m_bernstein. y ( ty ) ; }      
      // ======================================================================
    public:
      // ======================================================================
      /** get the integral over 2D-region 
       *  \f[ \int_{x_low}^{x_high}\int_{y_low}^{y_high} 
       *   \mathcal{B}(x,y) \mathrm{d}x\mathrm{d}y \f] 
       *  @param xlow  low  edge in x 
       *  @param xhigh high edge in x 
       *  @param ylow  low  edge in y 
       *  @param yhigh high edge in y 
       */
      double integral   ( const double xlow , const double xhigh , 
                          const double ylow , const double yhigh ) const ;
      // ======================================================================
      /** integral over x-dimension 
       *  \f[ \int_{y_low}^{y_high} \mathcal{B}(x,y) \mathrm{d}y\f] 
       *  @param x     variable 
       *  @param ylow  low  edge in y 
       *  @param yhigh high edge in y 
       */
      double integrateX ( const double y    , 
                          const double xlow , const double xhigh ) const 
      { return m_bernstein.integrateX ( y , xlow , xhigh ) ; }
      // ======================================================================
      /** integral over x-dimension 
       *  \f[ \int_{x_low}^{x_high} \mathcal{B}(x,y) \mathrm{d}x\f] 
       *  @param y     variable 
       *  @param xlow  low  edge in x 
       *  @param xhigh high edge in x 
       */
      double integrateY ( const double x    , 
                          const double ylow , const double yhigh ) const 
      { return m_bernstein.integrateY ( x , ylow , yhigh ) ; }
      // ======================================================================
    public: // specific 
      // ======================================================================
      /** get the integral over 2D-region 
       *  \f[ \int_{x_{min}}^{x_{max}}\int_{y_{min}}^{y_{max}} 
       *   \mathcal{B}(x,y) \mathrm{d}x\mathrm{d}y \f] 
       */
      double integral   () const ; 
      /** integral over x-dimension 
       *  \f[ \int_{x_{min}}^{x_{max}} \mathcal{B}(x,y) \mathrm{d}x\f] 
       *  @param x     variable 
       */
      double integrateX ( const double y ) const 
      { return m_bernstein.integrateX ( y ) ; }
      /** integral over y-dimension 
       *  \f[ \int_{y_{min}}^{y_{max}} \mathcal{B}(x,y) \mathrm{d}y\f] 
       *  @param y     variable 
       */
      double integrateY ( const double x ) const 
      { return m_bernstein.integrateY ( x ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // get the bernstein 2D polynom
      const Gaudi::Math::Bernstein2DSym& bernstein() const 
      { return m_bernstein ; }
      /// get the parameter sphere  
      const  Gaudi::Math::NSphere&       sphere   () const 
      { return m_sphere ; }  
      // ======================================================================
    private:
      // ======================================================================
      /// update bernstein coefficients 
      bool updateBernstein () ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual bernstein polynomial
      Gaudi::Math::Bernstein2DSym m_bernstein ; // the actual bernstein polynomial
      /// Parameter sphere 
      Gaudi::Math::NSphere        m_sphere ;
      // ======================================================================
    } ;
    // ========================================================================
  } //                                             end of namespace Gaudi::Math 
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LHCBMATH_BERNSTEIN_H
// ============================================================================
