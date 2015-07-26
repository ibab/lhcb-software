// $Id:$
// ============================================================================
// Include files 
// ============================================================================
// STD&STL
// ============================================================================
#include <climits>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/LHCbMath.h"
#include "LHCbMath/Polynomials.h"
#include "LHCbMath/Power.h"
#include "LHCbMath/Choose.h"
#include "LHCbMath/Bernstein.h"
#include "LHCbMath/MoreFunctions.h"
// ============================================================================
/** @file Implementation file for classes from file LHcbMath/Polynomials.h
 *  @see LHCbMath/Polynomials.h
 *  @date 2015-02-23 
 *  @author Vanya Belyaev Ivan Belyaev@itep.ru
 */
// ============================================================================
namespace 
{
  static_assert ( std::numeric_limits<double>::is_specialized           , 
                  "mumeric_limits are not specialized for doubles"      ) ;
  // ==========================================================================
  /// equality criteria for doubles
  LHCb::Math::Equal_To<double> s_equal ;       // equality criteria for doubles
  /// zero for doubles  
  LHCb::Math::Zero<double>     s_zero  ;       // zero for doubles
  /// zero fo vectors 
  LHCb::Math::Zero< std::vector<double> > s_vzero ; // zero for vectors
  /// small element 
  const long double            s_epsilon =  2 * std::numeric_limits<double>::epsilon() ;
  /// small element 
  LHCb::Math::Small<double>    s_small ( s_epsilon ) ;
  // ==========================================================================
  /// get a factorial 
  inline long double 
  _factorial_d_ ( const unsigned short N ) 
  {
    return 
      0 == N ?  1 : 
      0 == N ?  1 : 
      2 == N ?  2 : 
      3 == N ?  6 : 
      4 == N ? 24 : N * _factorial_d_ ( N - 1 ) ;
  }
  // ==========================================================================
}
// ============================================================================
// useful local utilities 
// ============================================================================
namespace 
{
  // =========================================================================
  // Chebyshev polynomials (1st and 2nd kind) 
  // =========================================================================
  /** evaluate chebychev polynomial (the 1st kind)
   *  @param N  the polynomial degree
   *  @param x  the argument 
   *  @return value of chebyshev polynomial (1st kind) at point x 
   */
  inline double _chebyshev_ 
  ( const unsigned int N , 
    const double       x )
  {
    //
    return 
      0 == N             ? 1.0 :
      1 == N             ?   x :
      s_equal ( x ,  1 ) ?                 1.0          :     
      s_equal ( x , -1 ) ?  ( 0 == N % 2 ? 1.0 : -1.0 ) :
      2 * x * _chebyshev_ ( N - 1 , x ) - _chebyshev_ ( N - 2 , x ) ;  
  }
  // ==========================================================================
  /** evaluate chebyshev polinomial (the second kind)
   *  @param N  the polynomial degree 
   *  @param x  the argument 
   *  @return value of chebyshev polynomial (2nd kind) at point x 
   */
  inline double _chebyshevU_ 
  ( const unsigned int N , 
    const double       x )
  {
    return 
      0 == N ? 1.0   :
      1 == N ? 2 * x : 
      s_equal ( x ,  1 ) ?                1.0 + N            :
      s_equal ( x , -1 ) ? ( 0 == N % 2 ? 1.0 + N : -1.0-N ) :
      _chebyshev_ ( N , x ) + x * _chebyshevU_ ( N - 1 , x ) ;
  } 
  // ==========================================================================
  /** evaluate the integral for chebychev polynomial (1st kind) 
   *  @param N    the polynomial degree 
   *  @param low  low edge of integration 
   *  @param high high edge of integration 
   */  
  inline double _chebyshev_int_ 
  ( const unsigned int N    ,
    const double       low  , 
    const double       high ) 
  {
    // trivial cases 
    if      ( s_equal ( low , high ) ) { return 0          ; }
    else if ( 0 == N                 ) { return high - low ; }
    else if ( 1 == N                 )
    { return 0.5 * ( high * high - low * low ) ; }
    else if ( high < low ) 
    { return -_chebyshev_int_ ( N ,  high , low )  ; }
    //
    const double ihigh = 
      _chebyshev_ ( N + 1 , high ) / ( N + 1 ) -
      _chebyshev_ ( N - 1 , high ) / ( N - 1 ) ;
    //
    const double ilow = 
      _chebyshev_ ( N + 1 , low  ) / ( N + 1 ) -
      _chebyshev_ ( N - 1 , low  ) / ( N - 1 ) ;
    //
    return 0.5 * ( ihigh - ilow ) ;
  }
  // ==========================================================================
  /** evaluate the derivative for chebychev polynomial (1st kind)
   *  @param N the polynomial degree
   *  @param x the point 
   */
  inline double _chebyshev_der_ 
  ( const unsigned int N ,
    const double       x )  
  {
    return  
      0 == N ? 0.0 : 
      1 == N ? 1.0 :
      2 == N ? 4*x :  N * _chebyshevU_ ( N - 1 , x ) ;
  }
  // ==========================================================================
  /** evaluate the integral for chebychev polynomial (2nd kind) 
   *  @param N    the polynomial degree 
   *  @param low  low edge of integration 
   *  @param high high edge of integration 
   */  
  inline double _chebyshevU_int_
  ( const unsigned int N    ,
    const double       low  , 
    const double       high ) 
  {
    // trival cases 
    if      ( s_equal ( low , high ) ) { return 0          ; }
    else if ( 0 == N                 ) { return high - low ; }
    else if ( 1 == N                 ) { return  ( high * high - low * low ) ; }
    else if ( high < low ) 
    { return -_chebyshevU_int_ ( N ,  high , low )  ; }
    //
    const double ihigh = _chebyshev_ ( N + 1 , high ) / ( N + 1 ) ;
    const double ilow  = _chebyshev_ ( N + 1 , low  ) / ( N + 1 ) ;
    //
    return ihigh - ilow ;
  }
  // =========================================================================
  /** evaluate the derivative for chebychev polynomial (2nd kind)
   *  @param N the polynomial degree
   *  @param x the point 
   */
  inline double _chebyshevU_der_
  ( const unsigned int N ,
    const double       x )  
  {
    //
    if      ( 0 == N ) { return 0   ; }
    else if ( 1 == N ) { return 2   ; }
    else if ( 2 == N ) { return 8*x ; }
    //
    const unsigned long n = N + 1 ;
    if      ( s_equal ( x ,  1 ) ) { return  n * ( n * n - 1 ) / 3.0 ; }
    else if ( s_equal ( x , -1 ) ) { return  
        n * ( n * n - 1 ) / 3.0 * ( 0 == N % 2 ? 1 : -1 ) ; }
    //
    const double v1 = ( N + 1 ) * _chebyshev_  ( N + 1 , x ) ;
    const double v2 =       x   * _chebyshevU_ ( N     , x ) ;
    const double d2 = v1 - v2 ;
    //
    return d2 / ( x * x - 1 ) ;  // ATTENTION HERE!!! it should be safe...
  }
  // ========================================================================== 
}
// ============================================================================
// evaluate Chebyshev polynomial
// ============================================================================
double Gaudi::Math::Chebyshev::operator() ( const double x ) const
{ return _chebyshev_     ( m_N , x ) ; }
// ============================================================================/
// evaluate the derivative of Chebyshev polynomial
// ============================================================================
double Gaudi::Math::Chebyshev::derivative ( const double x ) const
{ return _chebyshev_der_ ( m_N , x ) ; }
// ============================================================================
// get integral between low and high 
// ============================================================================
double Gaudi::Math::Chebyshev::integral   
( const double low  , 
  const double high ) const 
{ return _chebyshev_int_ ( m_N , low , high ) ; }
// ============================================================================
// evaluate Chebyshev polynomial of second kind 
// ============================================================================
double Gaudi::Math::ChebyshevU::operator() ( const double x ) const
{ return _chebyshevU_ ( m_N , x ) ; }
// ============================================================================
// evaluate the derivative of Chebyshev polynomial of the second kind 
// ============================================================================
double Gaudi::Math::ChebyshevU::derivative ( const double x ) const
{ return _chebyshevU_der_ ( m_N , x ) ; }
// ============================================================================
// get the integral between low and high 
// ============================================================================
double Gaudi::Math::ChebyshevU::integral   
( const double low  , 
  const double high ) const 
{ return _chebyshevU_int_ ( m_N , low , high ) ; }
// ============================================================================
namespace 
{
  // =========================================================================
  // Legendre polynomial
  // =========================================================================
  /** evaluate Legendre polynomial 
   *  @param N  the polynomial degree
   *  @param x  the argument 
   *  @return value of chebyshev polynomial (1st kind) at point x 
   */
  inline long double _legendre_ 
  ( const unsigned int N , 
    const long double       x )
  {
    //
    return 
      0 == N             ? 1.0 :
      1 == N             ?   x :
      ( ( 2 * N - 1 ) * x * _legendre_ ( N - 1 , x ) - 
        (     N - 1 )     * _legendre_ ( N - 2 , x ) ) / N ;
  }
  // ==========================================================================
  /** evaluate the integral for legendre polynomial
   *  @param N    the polynomial degree 
   *  @param low  low edge of integration 
   *  @param high high edge of integration 
   */  
  inline long double _legendre_int_ 
  ( const unsigned int N    ,
    const long double  low  , 
    const long double  high ) 
  {
    // trivial cases 
    if      ( s_equal ( low , high ) ) { return 0          ; }
    else if ( 0 == N                 ) { return high - low ; }
    else if ( 1 == N                 )
    { return 0.5 * ( high * high - low * low ) ; }
    else if ( high < low ) 
    { return -_legendre_int_ ( N ,  high , low )  ; }
    //
    const long double ihigh = _legendre_ ( N + 1 , high ) - _legendre_ ( N - 1 , high ) ;
    const long double ilow  = _legendre_ ( N + 1 , low  ) - _legendre_ ( N - 1 , low  ) ;
    //
    return ( ihigh - ilow ) / ( 2 * N + 1 ) ;
  }
  // ========================================================================== 
  /** evaluate the derivative for legendre polynomial
   *  @param N the polynomial degree
   *  @param x the point 
   */
  inline long double _legendre_der_
  ( const unsigned int N ,
    const long double  x )  
  {
    //
    if      ( 0 == N ) { return 0   ; }
    else if ( 1 == N ) { return 1   ; }
    //
    if      ( s_equal ( x ,  1 ) ) { return 0.5 * N * ( N + 1 ) ; }
    else if ( s_equal ( x , -1 ) ) 
    { return 1 == N % 2 ?  0.5 * N * ( N + 1 ) : -0.5 * N * ( N + 1 ) ; }
    //
    const long double t1 = x * _legendre_ ( N , x ) - _legendre_ ( N - 1 , x  ) ;
    //
    return N * t1 / ( x * x - 1 ) ;  // ATTENTION HERE!!! it should be safe...
  }
  // ==========================================================================
}
// ============================================================================
// evaluate Legendre polynomial
// ============================================================================
double Gaudi::Math::Legendre::operator() ( const double x ) const
{ return _legendre_     ( m_N , x ) ; }
// ============================================================================/
// evaluate the derivative of Chebyshev polynomial
// ============================================================================
double Gaudi::Math::Legendre::derivative ( const double x ) const
{ return _legendre_der_ ( m_N , x ) ; }
// ============================================================================
// get integral between low and high 
// ============================================================================
double Gaudi::Math::Legendre::integral   
( const double low  , 
  const double high ) const 
{ return _legendre_int_ ( m_N , low , high ) ; }
// ============================================================================
namespace 
{
  // =========================================================================
  //  Hermite polynomial
  // =========================================================================
  /** evaluate Hermite polynomial 
   *  @param N  the polynomial degree
   *  @param x  the argument 
   *  @return value of hermite polynomial at point x 
   */
  inline double _hermite_ 
  ( const unsigned int N , 
    const double       x )
  {
    //
    return 
      0 == N             ? 1.0 :
      1 == N             ?   x :
       x * _hermite_ ( N - 1 , x ) - ( N - 1 ) * _hermite_ ( N - 2 , x ) ;
  }
  // ==========================================================================
}
// ============================================================================
// evaluate Hermite polynomial
// ============================================================================
double Gaudi::Math::Hermite::operator() ( const double x ) const
{ return _hermite_     ( m_N , x ) ; }
// ============================================================================/

// ============================================================================
// Base class for all polynomial sums 
// ============================================================================
// constructor from polynomial degree
// ============================================================================
Gaudi::Math::PolySum::PolySum ( const unsigned short degree ) 
  : std::unary_function<double,double>() 
  , m_pars ( degree + 1 , 0 ) 
{}
// ============================================================================
// constructor from vector of parameters 
// ============================================================================
Gaudi::Math::PolySum::PolySum ( const std::vector<double>& pars ) 
  : std::unary_function<double,double>() 
  , m_pars ( pars  ) 
{ if ( m_pars.empty() ) { m_pars.push_back ( 0 ) ; } }
// ============================================================================
// copy constructor 
// ============================================================================
Gaudi::Math::PolySum::PolySum 
( const Gaudi::Math::PolySum&  right ) 
  : std::unary_function<double,double>( right )
 , m_pars ( right.m_pars ) 
{}
// ============================================================================
// move constructor 
// ============================================================================
Gaudi::Math::PolySum::PolySum 
(       Gaudi::Math::PolySum&& right ) 
  : std::unary_function<double,double>( right )
  , m_pars ( std::move ( right.m_pars ) )  
{}
// ============================================================================
// copy assignement  
// ============================================================================
Gaudi::Math::PolySum& 
Gaudi::Math::PolySum::operator=( const Gaudi::Math::PolySum&  right ) 
{
  if ( &right == this ) { return *this ; }
  m_pars = right.m_pars ;
  return *this ;
}
// ============================================================================
// move assignement 
// ============================================================================
Gaudi::Math::PolySum& 
Gaudi::Math::PolySum::operator=(      Gaudi::Math::PolySum&& right ) 
{
  if ( &right == this ) { return *this ; }
  m_pars = std::move ( right.m_pars ) ;
  return *this ;
}
// ============================================================================
// all zero ?
// ============================================================================
bool Gaudi::Math::PolySum::zero  () const { return s_vzero ( m_pars ) ; }
// ============================================================================
// set k-parameter
// ============================================================================
bool Gaudi::Math::PolySum::setPar 
( const unsigned short k , const double value ) 
{
  if ( m_pars.size() <= k            ) { return false ; }
  if ( s_equal ( m_pars[k] , value ) ) { return false ; }
  m_pars[k] = value ;
  return true ;
}
// ============================================================================
// simple  manipulations with polynoms: scale it! 
// ============================================================================
Gaudi::Math::PolySum&
Gaudi::Math::PolySum::operator*=( const double a ) 
{
  for ( std::vector<double>::iterator it = m_pars.begin() ; m_pars.end() != it ; ++it ) 
  {  (*it ) *= a ; }
  return *this ;
}
// ============================================================================
// simple  manipulations with polynoms: scale it! 
// ============================================================================
Gaudi::Math::PolySum&
Gaudi::Math::PolySum::operator/=( const double a ) 
{
  for ( std::vector<double>::iterator it = m_pars.begin() ; m_pars.end() != it ; ++it ) 
  {  (*it ) /= a ; }
  return *this ;
}
// ============================================================================
namespace 
{
  // ==========================================================================
  /// Clenshaw algorithm for summation of Chebyshev polynomials 
  inline double _clenshaw_chebyshev_ 
  ( const std::vector<double>& pars , 
    const long double          x    ) 
  {
    if ( pars.empty() ) { return 0 ; }
    //
    long double b2 = 0 ;
    long double b1 = 0 ;
    long double b0 = 0 ;
    for ( std::vector<double>::const_reverse_iterator ia = 
            pars.rbegin() ; pars.rend() != ia ; ++ia ) 
    {
      b2 = b1 ;
      b1 = b0 ;
      // b0 = (*ia) + 2 * x * b1 - b2 ;
      b0 = std::fma ( 2 * x , b1 , (*ia) - b2 ) ;
    }
    //
    b0 += pars[0] ;
    //
    return 0.5 * ( b0 - b2) ;
  }
  // ==========================================================================
  /// clenshaw algorithm for summation of Legendre series 
  inline double _clenshaw_legendre_  
  ( const std::vector<double>& pars , 
    const long double          x    ) 
  {
    if ( pars.empty() ) { return 0 ; }
    //
    long double b2 = 0 ;
    long double b1 = 0 ;
    long double b0 = 0 ;
    for ( int j = pars.size() - 1 ; 0 <= j ; --j ) 
    {
      b2 = b1 ;
      b1 = b0 ;
      b0 = pars[j] + ( 2 * j + 1 ) * x * b1 / ( j + 1 ) - ( j + 1 ) *  b2 / ( j + 2 ) ;
    }
    //
    return b0 ;
  }
  // ==========================================================================  
  /// clenshaw algorithm for summation of monomial series 
  template <class ITERATOR>
  inline std::pair<double,double>
  _clenshaw_monomial_( ITERATOR     first , 
                       ITERATOR     last  , 
                       const double x     ) 
  {
    if ( first == last ) { return std::make_pair(0.,0.0) ; }
    //
    long double p = *first ;
    long double q = 0      ;
    while ( ++first != last ) 
    {
      q = std::fma ( x , q ,  p     ) ; // x * q + p       ;
      p = std::fma ( x , p , *first ) ; // x * p + *first  ;
    }
    //
    return std::make_pair ( p , q ) ;
  }
  // Step 1: Set p = a[n] and q = 0
  // Step 2: Do steps 3 and 4 for i from n-1 to 0, decreasing by 1
  // Step 3: set q = p + x0 * q
  // Step 4: set p = a[i] + x0 * p
  // Step 5: The value of P(x_0) is p and the value of P'(x_0) is q
}
// ============================================================================
/* Clenshaw algorithm for summation of Chebyshev polynomials 
 *  \f$ f(x) = \sum_i p_i T_i(x)\f$
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-02-10
 */
// ============================================================================
double Gaudi::Math::clenshaw_chebyshev 
( const std::vector<double>& pars , 
  const double               x    ) 
{ return _clenshaw_chebyshev_ ( pars , x ) ; }
// ============================================================================
/*  Clenshaw algorithm for summation of Legendre polynomials 
 *  \f$ f(x) = \sum_i p_i P_i(x) \f$
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-02-10
 */
// ============================================================================
double Gaudi::Math::clenshaw_legendre
( const std::vector<double>& pars , 
  const double               x    ) 
{ return _clenshaw_legendre_ ( pars , x ) ; }
// ============================================================================
/*  Clenshaw algorithm for summation of monomial series 
 *  (aka Horner rule) 
 *  \f$ f(x) = \sum_i p_i x^i \f$
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-02-10
 */
// ============================================================================
double Gaudi::Math::clenshaw_polynom
( const std::vector<double>& pars , 
  const double               x    ) 
{ return _clenshaw_monomial_ ( pars.rbegin() , pars.rend() , x ).first ; }
// ============================================================================
/*  Clenshaw algorithm for summation of monomial series (aka Horner rule) 
 *  \f$ f(x) = \sum_i p_i x^i \f$
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-02-10
 */
// ============================================================================
double Gaudi::Math::horner_a0 
( const std::vector<double>& pars , 
  const double               x    ) 
{ return _clenshaw_monomial_ ( pars.rbegin() , pars.rend() , x ).first ; }
// ============================================================================
/*  Clenshaw algorithm for summation of monomial series (aka Horner rule) 
 *  \f$ f(x) = \sum_i p_i x^{n-i} \f$
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-02-10
 */
// ============================================================================
double Gaudi::Math::horner_aN 
( const std::vector<double>& pars , 
  const double               x    ) 
{ return _clenshaw_monomial_ ( pars.begin() , pars.end() , x ).first ; }
// ============================================================================
namespace 
{
  // ==========================================================================
  /// affine tranformation of polynomial coeffficients  
  inline double _affine_ 
  ( const unsigned short j , 
    const unsigned short k , 
    const long double    a , 
    const long double    b ) 
  {
    return 
      k < j ? 0.0 : 
      Gaudi::Math::choose ( k , j     ) * 
      Gaudi::Math::pow    ( a , j     ) *  
      Gaudi::Math::pow    ( b , k - j ) ;
  }
  // ==========================================================================
}
// ============================================================================
/* affine transformation of polynomial
 *  \f$ x ^{\prime} = \alpha x + \beta \f$
 *  @param input  (INPUT)  input polynomial coeffeicients 
 *  @param output (UPDATE) coefficinects of transformed polynomial 
 *  @param alpha  (INPUT)  alpha
 *  @param beta   (INPUT)  beta
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-03-09
 */
// ============================================================================
bool Gaudi::Math::affine_transform
( const std::vector<double>& input  , 
  std::vector<double>&       result ,  
  const double               alpha  , 
  const double               beta   ) 
{
  /// invalid transform 
  if      ( s_zero  ( alpha     ) ) { return false ; } // invalid transform 
  else if ( s_equal ( alpha , 1 ) && s_zero ( beta ) ) 
  { result = input ;                  return true  ; } // trivial transformation 
  //
  result.resize ( input.size() ) ;
  std::fill ( result.begin() , result.end() , 0 ) ;
  //
  for ( unsigned int i = 0 ; i < input.size() ; ++i ) 
  { 
    for ( unsigned short k = i ; k < input.size () ; ++k ) // ATTENTNION!!! 
    { 
      const double p = input[  k ] ;
      if ( s_zero ( p ) ) { continue ; }  // SKIP nulls... 
      result[i] += _affine_ ( i , k , alpha , beta ) * p ; 
    } 
  }  
  return true ;
}
// ============================================================================
namespace 
{
  // ==========================================================================
  /** evaluate the integral for monomial 
   *  @param N    the polynomial degree 
   *  @param low  low edge of integration 
   *  @param high high edge of integration 
   */  
  inline long double _monomial_int_ 
  ( const unsigned int N    ,
    const long double  low  , 
    const long double  high ) 
  {
    // trivial cases 
    if      ( s_equal ( low , high ) ) { return 0          ; }
    else if ( 0 == N                 ) { return high - low ; }
    else if ( 1 == N                 )
    { return 0.5 * ( high * high - low * low ) ; }
    else if ( high < low ) 
    { return -_monomial_int_ ( N ,  high , low )  ; }
    //
    const long double ihigh = Gaudi::Math::pow ( high , N + 1 ) ;
    const long double ilow  = Gaudi::Math::pow ( low  , N + 1 ) ;
    //
    return ( ihigh - ilow ) / ( N + 1 ) ;
  }
  // ========================================================================== 
  /** evaluate the derivative for monomial 
   *  @param N the polynomial degree
   *  @param x the point 
   */
  inline long double _monomial_der_
  ( const unsigned int N ,
    const double       x )  
  {
    //
    if      ( 0 == N       ) { return 0   ; }
    else if ( 1 == N       ) { return 1   ; }
    //
    return N * Gaudi::Math::pow ( x , N - 1 ) ;
  }
  // ==========================================================================
}
// ============================================================================
/* class Polynomial
 *  Trivial polynomial
 *  \f$ f(x) = \sum_i \p_i x^i\f$
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-02-22
 */
// ============================================================================
// constructor from the degree  
// ============================================================================
Gaudi::Math::Polynomial::Polynomial
( const unsigned short       degree , 
  const double               xmin   , 
  const double               xmax   )  
  : Gaudi::Math::PolySum ( degree ) 
  , m_xmin ( std::min ( xmin, xmax ) )
  , m_xmax ( std::max ( xmin, xmax ) )
{}
// ============================================================================
// constructor from parameters 
// ============================================================================
Gaudi::Math::Polynomial::Polynomial
( const std::vector<double>& pars  , 
  const double               xmin  , 
  const double               xmax  )  
  : Gaudi::Math::PolySum ( pars ) 
  , m_xmin ( std::min ( xmin, xmax ) )
  , m_xmax ( std::max ( xmin, xmax ) )
{}
// ============================================================================
// get the value
// ============================================================================
double Gaudi::Math::Polynomial::operator () ( const double x ) const 
{
  // trivial cases 
  if      ( x < m_xmin || x > m_xmax ) { return         0 ; }  // RETURN 
  else if ( 1 == m_pars.size()       ) { return m_pars[0] ; }  // RETURN 
  else if ( zero ()                  ) { return 0         ; }
  // 1) transform argument:
  const double tx = t ( x ) ;  
  // 2) use Clenshaw's algorithm 
  return clenshaw_polynom ( m_pars , tx ) ;              // RETURN 
}
// ============================================================================
double Gaudi::Math::Polynomial::integral   
( const double low  , 
  const double high ) const 
{
  //
  if      ( s_equal ( low , high ) ) { return 0 ; }
  else if ( high < low             ) { return -integral ( high , low ) ; }
  else if ( high <= m_xmin         ) { return 0 ; }
  else if ( low  >= m_xmax         ) { return 0 ; }
  else if ( zero ()                ) { return 0 ; }
  //
  if      ( low  <  m_xmin         ) { return integral ( m_xmin , high   ) ; }
  else if ( high >  m_xmax         ) { return integral ( low    , m_xmax ) ; }
  //
  const double xl = t ( low  ) ;
  const double xh = t ( high ) ;
  //
  std::vector<double> npars ( m_pars.size() + 1 , 0 ) ;
  for ( unsigned int i = 0 ; i < m_pars.size() ; ++i ) 
  {
    if ( s_zero ( m_pars[i] ) ) { continue ; }           // CONTINUE
    npars [i+1] = m_pars[i] / ( i + 1 ) ; 
  }
  //
  const double result = 
    clenshaw_polynom ( npars , xh ) -
    clenshaw_polynom ( npars , xl ) ;     
  //
  return result  * 0.5 * ( m_xmax - m_xmin ) ;
}
// ============================================================================
// get indefinte integral 
// ============================================================================
Gaudi::Math::Polynomial 
Gaudi::Math::Polynomial::indefinite_integral ( const double C ) const 
{
  const double dx = 0.5 * ( m_xmax - m_xmin ) ;
  std::vector<double> npars ( m_pars.size() + 1 , 0 ) ;
  for ( unsigned int i = 0 ; i < m_pars.size() ; ++i ) 
  { 
    if ( s_zero ( m_pars[i] ) ) { continue ; }           // CONTINUE
    npars [i+1] = m_pars[i] / ( i + 1 ) * dx ; 
  }
  //
  npars[0] = C ;
  //
  return Gaudi::Math::Polynomial( npars , m_xmin , m_xmax ) ;
}
// ============================================================================
// get the derivative at point "x" 
// ============================================================================
double Gaudi::Math::Polynomial::derivative ( const double x     ) const 
{
  if ( x < m_xmin || x > m_xmax ) { return 0 ; }
  //
  const double tx = t ( x  ) ;
  //
  const double dx = 2 / ( m_xmax - m_xmin ) ;
  std::vector<double> npars ( m_pars.size() - 1 , 0 ) ;
  for ( unsigned int i = 0 ; i < npars.size() ; ++i ) 
  {
    const double p = m_pars[i+1] ;
    if ( s_zero ( p ) ) { continue ; }      // CONTINUE 
    npars [i] = ( i + 1 ) * p * dx ; 
  }
  //
  return clenshaw_polynom ( npars , tx ) ;
}
// ============================================================================
// get the derivative 
// ============================================================================
Gaudi::Math::Polynomial 
Gaudi::Math::Polynomial::derivative          () const 
{
  if ( 1 == m_pars.size() ) 
  { return Gaudi::Math::Polynomial( 0 , m_xmin , m_xmax ) ; }
  //
  const double dx = 2 / ( m_xmax - m_xmin ) ;
  std::vector<double> npars ( m_pars.size() - 1 , 0 ) ;
  for ( unsigned int i = 0 ; i < npars.size() ; ++i ) 
  {    
    const double p = m_pars[i+1] ;
    if ( s_zero ( p ) ) { continue ; }       // CONTINUE
    npars [i] = ( i + 1 ) * p * dx ; 
  }
  //
  return Gaudi::Math::Polynomial ( npars , m_xmin , m_xmax ) ;
}
// ============================================================================ 
// simple  manipulations with polynoms: shift it! 
// ============================================================================
Gaudi::Math::Polynomial& 
Gaudi::Math::Polynomial::operator+= ( const double a ) 
{ m_pars[0] += a ; return *this ; }
// ============================================================================
// simple  manipulations with polynoms: shift it! 
// ============================================================================
Gaudi::Math::Polynomial& 
Gaudi::Math::Polynomial::operator-= ( const double a ) 
{ m_pars[0] -= a ; return *this ; }
// ============================================================================

// ============================================================================
// Chebyshev sum 
// ============================================================================
// constructor from parameters 
// ============================================================================
Gaudi::Math::ChebyshevSum::ChebyshevSum
( const unsigned short degree , 
  const double         xmin   , 
  const double         xmax   )  
  : Gaudi::Math::PolySum ( degree ) 
  , m_xmin ( std::min ( xmin, xmax ) )
  , m_xmax ( std::max ( xmin, xmax ) )
{}
// ============================================================================
// constructor from parameters 
// ============================================================================
Gaudi::Math::ChebyshevSum::ChebyshevSum
( const std::vector<double>& pars  , 
  const double               xmin  , 
  const double               xmax  )  
  : Gaudi::Math::PolySum ( pars ) 
  , m_xmin ( std::min ( xmin, xmax ) )
  , m_xmax ( std::max ( xmin, xmax ) )
{}
// ============================================================================
// get the value
// ============================================================================
double Gaudi::Math::ChebyshevSum::operator () ( const double x ) const 
{
  // trivial cases 
  if      ( x < m_xmin || x > m_xmax ) { return         0 ; }  // RETURN 
  else if ( 1 == m_pars.size()       ) { return m_pars[0] ; }  // RETURN 
  else if ( zero ()                  ) { return 0         ; }
  // 1) transform argument:
  const double tx = t ( x ) ;  
  // 2) use Clenshaw's algorithm 
  return _clenshaw_chebyshev_   ( m_pars , tx ) ;              // RETURN 
}
// ============================================================================
// get the integral between xmin and xmax
// ============================================================================
double Gaudi::Math::ChebyshevSum::integral   () const 
{ return integral ( m_xmin , m_xmax ) ; }
// ============================================================================
double Gaudi::Math::ChebyshevSum::integral   
( const double low  , 
  const double high ) const 
{
  //
  if      ( s_equal ( low , high ) ) { return 0 ; }
  else if ( high < low             ) { return -integral ( high , low ) ; }
  else if ( high <= m_xmin         ) { return 0 ; }
  else if ( low  >= m_xmax         ) { return 0 ; }
  else if ( zero ()                ) { return 0 ; }
  //
  if      ( low  <  m_xmin         ) { return integral ( m_xmin , high   ) ; }
  else if ( high >  m_xmax         ) { return integral ( low    , m_xmax ) ; }
  //
  const double xl = t ( low  ) ;
  const double xh = t ( high ) ;
  //
  // // Trvial sum to be replaced with Clenshaw 
  // double result = 0 ;
  // for ( std::vector<double>::const_iterator ip = m_pars.begin() ; m_pars.end() != ip ; ++ip ) 
  // { 
  //   const double p = *ip ;
  //   if ( s_zero ( p ) ) { continue ; } // SKIP IT! 
  //   result += p * _chebyshev_int_ ( ip - m_pars.begin() , xl , xh  ) ;
  // }
  //
  const double dx = 0.5 * ( m_xmax - m_xmin ) ;
  std::vector<double> npars ( m_pars.size() + 1 , 0 ) ;
  for ( unsigned short i = 0 ; i < m_pars.size() ; ++i ) 
  {
    if ( s_zero ( m_pars[i] ) ) { continue ; }              // CONTINUE 
    //
    if      ( 0 == i ) { npars [1] +=        m_pars[0] * dx ; }
    else if ( 1 == i ) { npars [2] += 0.25 * m_pars[1] * dx ; }
    else 
    {
      npars [ i + 1] += m_pars[i] * 0.5 / ( i + 1.0 ) * dx ;  
      npars [ i - 1] -= m_pars[i] * 0.5 / ( i - 1.0 ) * dx ; 
    }
  }
  //
  const double result = 
    _clenshaw_chebyshev_   ( npars , xh ) - 
    _clenshaw_chebyshev_   ( npars , xl ) ;
  //
  return result ;
}
// ============================================================================
// get indefinte integral 
// ============================================================================
Gaudi::Math::ChebyshevSum 
Gaudi::Math::ChebyshevSum::indefinite_integral ( const double C ) const 
{
  // 
  const double dx = 0.5 * ( m_xmax - m_xmin ) ;
  std::vector<double> npars ( m_pars.size() + 1 , 0 ) ;
  for ( unsigned short i = 0 ; i < m_pars.size() ; ++i ) 
  {
    if ( s_zero ( m_pars[i] ) ) { continue ; }           // CONTINUE 
    //
    if      ( 0 == i ) { npars [1] +=        m_pars[0] * dx ; }
    else if ( 1 == i ) { npars [2] += 0.25 * m_pars[1] * dx ; }
    else 
    {
      npars [ i + 1 ] += m_pars[i] * 0.5 / ( i + 1.0 ) * dx ;  
      npars [ i - 1 ] -= m_pars[i] * 0.5 / ( i - 1.0 ) * dx ; 
    }
  }
  //
  npars[0] += C ;
  return Gaudi::Math::ChebyshevSum ( npars , m_xmin , m_xmax ) ;
}
// ============================================================================
// get the derivative at point "x" 
// ============================================================================
double Gaudi::Math::ChebyshevSum::derivative ( const double x     ) const 
{
  if ( x < m_xmin || x > m_xmax ) { return 0 ; }
  //
  const double tx = t ( x  ) ;
  //
  // // Trvial sum to be replaced with Clenshaw 
  // double result = 0 ;
  // for ( std::vector<double>::const_iterator ip = m_pars.begin() ; m_pars.end() != ip ; ++ip ) 
  // {
  //   const double p = *ip ;
  //   if ( s_zero ( p ) ) { continue ; } // SKIP IT! 
  //   result += p * _chebyshev_der_ ( ip - m_pars.begin() , tx ) ; 
  // }
  //
  std::vector<double> npars ( m_pars.size() - 1 , 0 ) ;
  for ( unsigned short i = 1 ; i < m_pars.size() ; ++i ) 
  { 
    //
    if ( s_zero ( m_pars[i] ) ) { continue ; }           // CONTINUE 
    //
    const unsigned short id = i - 1 ;
    if ( 0 == id % 2 ) 
    {
      for ( unsigned short j = 0 ; j <= id ; j+=2 ) 
      { npars[j] += 2 * i * m_pars[i] ; } 
      npars [0]  -=     i * m_pars[i] ;
    }
    else 
    {
      for ( unsigned short j = 1 ; j <= id ; j+=2 ) 
      { npars[j] += 2 * i * m_pars[i] ; } 
    }
  }
  //
  const double result = _clenshaw_chebyshev_   ( npars , tx ) ;
  //
  const double dx = 2 / ( m_xmax - m_xmin ) ;
  return result * dx ;  
}
// ============================================================================
// get the derivative 
// ============================================================================
Gaudi::Math::ChebyshevSum 
Gaudi::Math::ChebyshevSum::derivative () const 
{
  //
  if ( 1 == m_pars.size() ) 
  { return Gaudi::Math::ChebyshevSum ( 0 , m_xmin , m_xmax ) ; }
  //
  const double dx = 2 / ( m_xmax - m_xmin ) ;
  std::vector<double> npars ( m_pars.size() - 1 , 0 ) ;
  for ( unsigned short i = 1 ; i < m_pars.size() ; ++i ) 
  { 
    //
    if ( s_zero ( m_pars[i] ) ) { continue ; }           // CONTINUE 
    //
    const unsigned short id = i - 1 ;
    if ( 0 == id % 2 ) 
    {
      for ( unsigned short j = 0 ; j <= id ; j+=2 ) 
      { npars[j] += 2 * i * m_pars[i] * dx ; } 
      npars [0]  -=     i * m_pars[i] * dx ;
    }
    else 
    {
      for ( unsigned short j = 1 ; j <= id ; j+=2 ) 
      { npars[j] += 2 * i * m_pars[i] * dx ; } 
    }
  }
  //
  return Gaudi::Math::ChebyshevSum ( npars , m_xmin , m_xmax ) ;
}
// ============================================================================
// simple  manipulations with polynoms: shift it! 
// ============================================================================
Gaudi::Math::ChebyshevSum& 
Gaudi::Math::ChebyshevSum::operator+= ( const double a ) 
{ m_pars[0] += a ; return *this ; }
// ============================================================================
// simple  manipulations with polynoms: shift it! 
// ============================================================================
Gaudi::Math::ChebyshevSum& 
Gaudi::Math::ChebyshevSum::operator-= ( const double a ) 
{ m_pars[0] -= a ; return *this ; }
// ============================================================================


// ============================================================================
// LegendreSum 
// ============================================================================
// constructor from the degree 
// ============================================================================
Gaudi::Math::LegendreSum::LegendreSum
( const unsigned short degree , 
  const double         xmax   , 
  const double         xmin   )  
  : Gaudi::Math::PolySum ( degree ) 
  , m_xmin ( std::min ( xmin, xmax ) )
  , m_xmax ( std::max ( xmin, xmax ) )
{}
// ============================================================================
// constructor from parameters
// ============================================================================
Gaudi::Math::LegendreSum::LegendreSum
( const std::vector<double>& pars , 
  const double               xmin , 
  const double               xmax )  
  : Gaudi::Math::PolySum ( pars ) 
  , m_xmin ( std::min ( xmin, xmax ) )
  , m_xmax ( std::max ( xmin, xmax ) )
{}
// ============================================================================
// get the value
// ============================================================================
double Gaudi::Math::LegendreSum::operator () ( const double x ) const 
{
  if      ( x < m_xmin || x > m_xmax ) { return         0 ; }
  else if ( 1 == m_pars.size()       ) { return m_pars[0] ; }
  else if ( zero ()                  ) { return         0 ; }
  // transform argument:
  const double tx = t ( x ) ;  
  // use Clenshaw's algorithm 
  return _clenshaw_legendre_ ( m_pars , tx ) ;
}
// ============================================================================
// get the integral between xmin and xmax
// ============================================================================
double Gaudi::Math::LegendreSum::integral   () const 
{ return m_pars[0] * ( m_xmax - m_xmin ) ; }
// ============================================================================
// get the integral between low and high 
// ============================================================================
double Gaudi::Math::LegendreSum::integral   
( const double low  , 
  const double high ) const 
{
  //
  if      ( s_equal ( low , high ) ) { return 0 ; }
  else if ( high < low             ) { return -integral ( high , low ) ; }
  else if ( high <= m_xmin         ) { return 0 ; }
  else if ( low  >= m_xmax         ) { return 0 ; }
  else if ( zero ()                ) { return 0 ; }
  //
  if ( s_equal( low  , m_xmin ) && s_equal ( high , m_xmax ) ) { return integral () ; }
  //
  if      ( low  <  m_xmin         ) { return integral ( m_xmin , high   ) ; }
  else if ( high >  m_xmax         ) { return integral ( low    , m_xmax ) ; }
  //
  const double xl = t ( low  ) ;
  const double xh = t ( high ) ;
  //
  // // Trvial sum to be replaced with Clenshaw 
  // double result = 0 ;
  // for ( std::vector<double>::const_iterator ip = m_pars.begin() ; m_pars.end() != ip ; ++ip ) 
  // { 
  //   const double p = *ip ;
  //   if ( s_zero ( p ) ) { continue ; } // SKIP IT! 
  //   result += p * _legendre_int_ ( ip - m_pars.begin() , xl , xh  ) ;
  // }
  //
  std::vector<double> npars ( m_pars.size() + 1 , 0 ) ;
  for ( unsigned int i = 0 ; i < m_pars.size() ; ++i ) 
  { 
    if ( s_zero ( m_pars[i] ) ) { continue ; }           // CONTINUE 
    //
    npars [i+1]                += m_pars[i] / ( 2*i + 1 ) ; 
    if ( 0 < i ) { npars [i-1] -= m_pars[i] / ( 2*i + 1 ) ; }
  }
  //
  const double result = 
    clenshaw_legendre ( npars , xh ) -
    clenshaw_legendre ( npars , xl ) ;     
  //
  const double dx = 0.5 *  ( m_xmax - m_xmin ) ;
  return result * dx ;
}
// ============================================================================
// get indefinite integral 
// ============================================================================
Gaudi::Math::LegendreSum
Gaudi::Math::LegendreSum::indefinite_integral ( const double C ) const
{
  //
  const double dx = 0.5 * ( m_xmax - m_xmin ) ;
  std::vector<double> npars ( m_pars.size() + 1 , 0 ) ;
  for ( unsigned int i = 0 ; i < m_pars.size() ; ++i ) 
  { 
    if ( s_zero ( m_pars[i] ) ) { continue ; }           // CONTINUE 
    //
    npars [i+1]                += m_pars[i] / ( 2*i + 1 ) * dx ; 
    if ( 0 < i ) { npars [i-1] -= m_pars[i] / ( 2*i + 1 ) * dx ; }
  }
  //
  npars[0] += C ;
  return Gaudi::Math::LegendreSum ( npars , m_xmin , m_xmax ) ;
}
// ============================================================================
// get the derivative at point "x" 
// ============================================================================
double Gaudi::Math::LegendreSum::derivative ( const double x     ) const 
{
  if ( x < m_xmin || x > m_xmax || 1 == m_pars.size() ) { return 0 ; }
  //
  const double tx = t ( x  ) ;
  //
  // // Trvial sum to be replaced with Clenshaw 
  // double result = 0 ;
  // for ( std::vector<double>::const_iterator ip = m_pars.begin() ; m_pars.end() != ip ; ++ip ) 
  // {
  //   const double p = *ip ;
  //   if ( s_zero ( p ) ) { continue ; } // SKIP IT! 
  //   result += p * _legendre_der_ ( ip - m_pars.begin() , tx ) ; 
  //
  std::vector<double> npars ( m_pars.size() - 1 , 0 ) ;
  for ( unsigned short i = 1 ; i < m_pars.size() ; ++i )
  { 
    if ( s_zero ( m_pars[i] ) ) { continue ; }           // CONTINUE 
    //
    for ( int j = i-1 ; 0<=j  ; j-=2 ) 
    { npars[j] += m_pars[i] * ( 2 * j + 1 ) ; }
  }
  //
  const double dx = 2 / ( m_xmax - m_xmin ) ;
  return clenshaw_legendre ( npars , tx ) * dx ;  
}
// ============================================================================
// get the derivative 
// ============================================================================
Gaudi::Math::LegendreSum 
Gaudi::Math::LegendreSum::derivative () const 
{
  if ( 1 == m_pars.size() ) 
  { return Gaudi::Math::LegendreSum ( 0 , m_xmin , m_xmax ) ; }
  //
  const double dx = 2 / ( m_xmax - m_xmin ) ;
  std::vector<double> npars ( m_pars.size() - 1 , 0 ) ;
  for ( unsigned short i = 1 ; i < m_pars.size() ; ++i )
  { 
    if ( s_zero ( m_pars[i] ) ) { continue ; }           // CONTINUE 
    //
    for ( int j = i-1 ; 0<=j  ; j-=2 ) 
    { npars[j] += m_pars[i] * ( 2 * j + 1 ) * dx ; }
  }
  //
  return Gaudi::Math::LegendreSum ( npars , m_xmin , m_xmax ) ;
}
// ============================================================================
// simple  manipulations with polynoms: shift it! 
// ============================================================================
Gaudi::Math::LegendreSum& 
Gaudi::Math::LegendreSum::operator+= ( const double a ) 
{ m_pars[0] += a ; return *this ; }
// ============================================================================
// simple  manipulations with polynoms: shift it! 
// ============================================================================
Gaudi::Math::LegendreSum& 
Gaudi::Math::LegendreSum::operator-= ( const double a ) 
{ m_pars[0] -= a ; return *this ; }
// ============================================================================




// ============================================================================
/// legendre to bernstein transformation 
namespace 
{
  // ==========================================================================
  /** transformation matrix from bernstein to legendre basis 
   *  @see http://www.sciencedirect.com/science/article/pii/S0377042700003769 eq.21
   */
  inline 
  double b2l_mtrx 
  ( const unsigned short j , 
    const unsigned short k ,
    const unsigned short n ) 
  {
    //
    long long r = 0 ;
    for ( unsigned short i = 0 ; i <= j ; ++i ) 
    {
      0 == ( j + i ) % 2 ?
        r +=
        Gaudi::Math::choose ( j              , i     ) * 
        Gaudi::Math::choose ( k + i          , k     ) * 
        Gaudi::Math::choose ( n - k + j - i  , n - k ) : 
        r -=
        Gaudi::Math::choose ( j              , i     ) * 
        Gaudi::Math::choose ( k + i          , k     ) * 
        Gaudi::Math::choose ( n - k + j - i  , n - k ) ;
    }
    //
    return r * ( 2* j + 1 ) / double ( Gaudi::Math::choose ( n + j , n ) ) / ( n + j + 1 ) ;
  }
  // ==========================================================================
  /** transformation matrix from bernstein basic to monomial basis 
   *  @see http://www.sciencedirect.com/science/article/pii/S0377042700003769 eq.21
   */
  inline 
  long long b2m_mtrx 
  ( const unsigned short j , 
    const unsigned short k ,
    const unsigned short n ) 
  {
    const long long r = 
      Gaudi::Math::choose ( n , j ) * 
      Gaudi::Math::choose ( j , k ) ;
    return 0 == ( j + k ) % 2 ? r : -r ;
  }
  // ==========================================================================
  /// affine transformation of polynomial
  inline 
  double m2m_mtrx_1
  ( const unsigned short j , 
    const unsigned short k ) 
  {
    if ( k < j ) { return 0 ; }
    const double c = Gaudi::Math::choose ( k , j ) ;
    return c / Gaudi::Math::pow ( 2 , k ) ;
  }
  // ==========================================================================
  /// transformation matrix from monomial to chebyshev basis 
  inline 
  double m2c_mtrx 
  ( const unsigned short j , 
    const unsigned short k )
  {
    return 
      ( 1 == ( j + k ) % 2 ) ? 0. :
      Gaudi::Math::choose ( k , ( k - j ) / 2 ) * ( j == 0 ? 1. : 2. ) / Gaudi::Math::pow ( 2 , k ) ;
  }
  // ============================================================================
  /// transformation matrix from legendre basic to monomial basis 
  inline 
  double l2m_mtrx 
  ( const unsigned short j , 
    const unsigned short k )
  {
    return 
      1 == ( j + k ) % 2 ? 0.0 : 
      Gaudi::Math::choose       ( k         , j ) * 
      Gaudi::Math::choose_half  ( j + k - 1 , k ) *
      Gaudi::Math::pow          ( 2         , k ) ;
  }
  // ==========================================================================
}
// ============================================================================
// constructor from bernstein polynomial
// ============================================================================
Gaudi::Math::LegendreSum::LegendreSum
( const Gaudi::Math::Bernstein& poly )  
  : Gaudi::Math::PolySum ( poly.degree () ) 
  , m_xmin ( poly.xmin() ) 
  , m_xmax ( poly.xmax() ) 
{
  const unsigned short np = npars  () ;
  const unsigned short d  = degree () ;
  for ( unsigned short i  = 0 ; i < np ; ++i ) 
  { 
    for ( unsigned short k = 0 ; k < np ; ++k ) 
    { 
      const double p = poly.par ( k ) ;
      if ( s_zero ( p ) ) { continue ; }
      m_pars[i] += b2l_mtrx ( i , k , d ) * p ; 
    } 
  }
}
// ============================================================================
// constructor from bernstein polynomial
// ============================================================================
Gaudi::Math::Polynomial::Polynomial
( const Gaudi::Math::Bernstein& poly )  
  : Gaudi::Math::PolySum ( poly.degree () ) 
  , m_xmin ( poly.xmin() ) 
  , m_xmax ( poly.xmax() ) 
{
  //
  const unsigned short np = npars  () ;
  const unsigned short d  = degree () ;
  //
  // 2-step tranfromation
  //
  // 1: tranfrom to 'regular' ponynomial basic 
  std::vector<double> _pars( m_pars.size() ) ;
  for ( unsigned short i = 0 ; i < np ; ++i ) 
  { 
    for ( unsigned short k = 0 ; k <= i ; ++k )  // ATTENTION!!! 
    { 
      const double p = poly.par ( k ) ;
      if ( s_zero ( p ) ) { continue ; }
      _pars[i] += b2m_mtrx ( i , k , d ) * p ; 
    } 
  }
  //
  // 2: affine tranform to use [-1,-1] range 
  for ( unsigned short i = 0 ; i < np ; ++i ) 
  { 
    for ( unsigned short k = i ; k < np ; ++k )  // ATTENTION!!!
    { 
      const double p = _pars[  k ] ;
      if ( s_zero ( p ) ) { continue ; }
      m_pars[i] += m2m_mtrx_1 ( i , k ) * p ; 
    } 
  }  
}
// ============================================================================
// constructor from legendre polynomial
// ============================================================================
Gaudi::Math::Polynomial::Polynomial
( const Gaudi::Math::LegendreSum& poly )  
  : Gaudi::Math::PolySum ( poly.degree () ) 
  , m_xmin ( poly.xmin() ) 
  , m_xmax ( poly.xmax() ) 
{
  //
  const unsigned short np = npars  () ;
  //
  for ( unsigned short i = 0 ; i < np ; ++i ) 
  { 
    for ( unsigned short k = i ; k < np ; k+=2 )   // ATTENTION!!! 2!
    { 
      const double p = poly.par ( k ) ;
      if ( s_zero ( p ) ) { continue ; }
      m_pars[i] += l2m_mtrx ( i , k ) * p ; 
    } 
  }
}
// =============================================================================
//  constructor from Polinomial 
// =============================================================================
Gaudi::Math::ChebyshevSum::ChebyshevSum
( const Gaudi::Math::Polynomial& poly )  
  : Gaudi::Math::PolySum ( poly.degree () ) 
  , m_xmin ( poly.xmin() ) 
  , m_xmax ( poly.xmax() ) 
{
  const unsigned short np = npars  () ;
  for ( unsigned short i = 0 ; i < np ; ++i ) 
  { 
    for ( unsigned short k = i ; k < np  ; k+=2 ) // ATTENTION !!! 2!
    { 
      const double p = poly.par ( k ) ;
      if ( s_zero ( p ) ) { continue ; }
      m_pars[i] += m2c_mtrx ( i , k ) * p ; 
    } 
  }  
}
// ============================================================================


// ============================================================================
// Delegation
// ============================================================================

// ============================================================================
// constructor from chebyshev polynomial (delegation)
// ============================================================================
Gaudi::Math::Polynomial::Polynomial
( const Gaudi::Math::ChebyshevSum& poly )  
  : Gaudi::Math::Polynomial ( Gaudi::Math::Bernstein ( poly ) ) 
{}
// ============================================================================
// constructor from bernstein polynomial (deleghation)
// ============================================================================
Gaudi::Math::ChebyshevSum::ChebyshevSum
( const Gaudi::Math::Bernstein& poly )  
  : Gaudi::Math::ChebyshevSum ( Gaudi::Math::Polynomial ( poly ) ) 
{}
// ============================================================================
// constructor from legendre polynomial (delegation)
// ============================================================================
Gaudi::Math::ChebyshevSum::ChebyshevSum
( const Gaudi::Math::LegendreSum& poly )  
  : Gaudi::Math::ChebyshevSum ( Gaudi::Math::Polynomial ( poly ) ) 
{}
// ============================================================================
// constructor from polynomial (delegation)
// ============================================================================
Gaudi::Math::LegendreSum::LegendreSum
( const Gaudi::Math::Polynomial& poly )  
  : Gaudi::Math::LegendreSum ( Gaudi::Math::Bernstein ( poly ) ) 
{}
// ============================================================================
// constructor from chebyshev polynomial (delegation)
// ============================================================================
Gaudi::Math::LegendreSum::LegendreSum
( const Gaudi::Math::ChebyshevSum& poly )  
  : Gaudi::Math::LegendreSum ( Gaudi::Math::Bernstein ( poly ) ) 
{}


// ============================================================================
// integation with an exponent 
// ============================================================================
namespace 
{
  // ===========================================================================
  // integation with an exponent 
  // ==========================================================================
  template <class POLYNOMIAL>
  inline double _integrate_
  ( const POLYNOMIAL& poly , 
    const long double tau  , 
    const long double low  , 
    const long double high ) 
  {
    const long double xlow  = std::max ( low  , (long double) poly.xmin() ) ;
    const long double xhigh = std::min ( high , (long double) poly.xmax() ) ;
    //
    // a bit esoteric way to get numerically correct resuls...
    //
    const long double eH = std::expm1 ( tau * xhigh ) ;
    const long double eL = std::expm1 ( tau * xlow  ) ;
    const long double pH = poly       (       xhigh ) ;
    const long double pL = poly       (       xlow  ) ;
    //
    const long double p1 = ( eH * pH - eL * pL ) + ( pH - pL ) ;
    //
    if ( 1 >= poly.npars  () ) { return p1 / tau ; } // RETURN 
    //
    return ( p1 - _integrate_ ( poly.derivative() , tau , xlow , xhigh ) )  / tau ;
  }
  // ==========================================================================
}
// ============================================================================
/* get the integral between low and high for a product of
 *  polynom and the exponential function with the exponent tau
 *  \f[  \int_{a}^{b} \mathcal{P} e^{\tau x } \mathrm{d}x \f] 
 *  @param poly  polynomial
 *  @param tau   slope parameter for exponential 
 *  @param low   low  integration range 
 *  @param high  high integration range 
 */
// ============================================================================
double Gaudi::Math::integrate 
( const Gaudi::Math::Polynomial& poly ,
  const double                   tau  ,
  const double                   low  , 
  const double                   high ) 
{
  //
  // the exponent is totally redundant...
  if      ( s_zero  ( tau )         ) { return  poly.integral ( low  , high         ) ; } 
  else if ( s_small ( tau )         ) { return  poly.integral ( low  , high         ) ; } 
  // zero range 
  else if ( s_equal ( low , high )  ) { return 0 ; }
  // ingertaion of zeros is simple 
  else if ( poly.zero ()            ) { return 0 ; }
  // invert range 
  else if ( low  >  high            ) { return -integrate ( poly , tau , high , low ) ; }
  // our the range 
  else if ( high <  poly.xmin () || 
            low  >  poly.xmax ()    ) { return  0 ; }
  // nice range to use analytic expression 
  else if ( s_equal ( low  , poly.xmin() ) && 
            s_equal ( high , poly.xmax() ) ) { return integrate ( poly , tau )  ; }
  //
  // check if special "small-tau" algorithm is needed here 
  // 
  const long double  xmin = poly.xmin () ;
  const long double  xmax = poly.xmax () ;
  const long double  _tau = ( xmax - xmin ) * tau / 2   ;
  const unsigned int N    = poly.degree() ;
  const long double  t1   = Gaudi::Math::pow ( std::abs (  tau ) , N + 1 ) ;
  const long double  t2   = Gaudi::Math::pow ( std::abs ( _tau ) , N + 1 ) ;
  if ( s_small ( t1 ) || s_small ( t2 ) )
  {
    const long double _fac = std::exp   ( ( xmax + xmin ) * tau / 2 ) ;
    const long double tmin = poly.t ( low ) ;
    const long double tmax = poly.t ( high ) ;    
    //
    long double result = 0 ;
    //
    const std::vector<double>& pars = poly.pars() ;
    for ( unsigned short i = 0 ; i < pars.size() ; ++i )
    { 
      const long double p = pars[i] ;
      if ( s_zero ( p ) ) { continue ; }                 // CONTINUE 
      const long double tl = Gaudi::Math::pow ( tmin , i + 1 ) ;
      const long double th = Gaudi::Math::pow ( tmax , i + 1 ) ;
      result += p * _factorial_d_ ( i ) * 
        ( th * Gaudi::Math::gamma_star ( i + 1 , -_tau * tmax ) -
          tl * Gaudi::Math::gamma_star ( i + 1 , -_tau * tmin ) ) ;
    }
    //
    result *= ( xmax - xmin ) * _fac / 2 ;
    //
    return result ;
  }
  // try generic recursive efficient, but numericaly highly unstable scheme  
  return _integrate_ ( poly , tau , low , high );
}
// ============================================================================
/*  get the integral between low and high for a product of
 *  Chebyshev polynom and the exponential function with the exponent tau
 *  \f[  \int_{a}^{b} \mathcal{T} e^{\tau x } \mathrm{d}x \f] 
 *  @param poly  chebyshev polynomial
 *  @param tau   slope parameter for exponential 
 *  @param low   low  integration range 
 *  @param high  high integration range 
 */
// ============================================================================
double Gaudi::Math::integrate 
( const Gaudi::Math::ChebyshevSum& poly ,
  const double                     tau  ,
  const double                     low  , 
  const double                     high ) 
{
  //
  if      ( s_zero  ( tau )         ) { return  poly.integral ( low  , high         ) ; } 
  else if ( s_small ( tau )         ) { return  poly.integral ( low  , high         ) ; } 
  else if ( s_equal ( low , high )  ) { return 0 ; }
  else if ( poly.zero ()            ) { return 0 ; }
  else if ( low  >  high            ) { return -integrate ( poly , tau , high , low ) ; }
  else if ( high <  poly.xmin () || 
            low  >  poly.xmax ()    ) { return  0 ; }
  //
  // check if special "small-tau" algorithm is needed here 
  // 
  const long double  xmin = poly.xmin () ;
  const long double  xmax = poly.xmax () ;
  const long double  _tau = ( xmax - xmin ) * tau / 2   ;
  const unsigned int N    = poly.degree() ;
  const long double  t1   = Gaudi::Math::pow ( std::abs (  tau ) , N + 1 ) ;
  const long double  t2   = Gaudi::Math::pow ( std::abs ( _tau ) , N + 1 ) ;
  if ( s_small ( t1 ) || s_small ( t2 ) )
  { 
    const Gaudi::Math::Polynomial p ( poly ) ;
    return integrate ( p , tau , low , high ) ; 
  }
  //
  // try generic recursive, but numericaly highly unstable scheme  
  return _integrate_ ( poly , tau , low , high );
}
// ========================================================================    
/** get the integral between low and high for a product of
 *  Legendre polynom and the exponential function with the exponent tau
 *  \f[  \int_{a}^{b} \mathcal{L} e^{\tau x } \mathrm{d}x \f] 
 *  @param poly  Legendre polynomial
 *  @param tau   slope parameter for exponential 
 *  @param a     low  integration range 
 *  @param b     high integration range 
 */
// ============================================================================
double Gaudi::Math::integrate 
( const Gaudi::Math::LegendreSum& poly ,
  const double                    tau  ,
  const double                    low  , 
  const double                    high ) 
{
  //
  if      ( s_zero  ( tau )         ) { return  poly.integral ( low  , high         ) ; } 
  else if ( s_small ( tau )         ) { return  poly.integral ( low  , high         ) ; } 
  else if ( s_equal ( low , high )  ) { return 0 ; }
  else if ( poly.zero ()            ) { return 0 ; }
  else if ( low  >  high            ) { return -integrate ( poly , tau , high , low ) ; }
  else if ( high <  poly.xmin () || 
            low  >  poly.xmax ()    ) { return  0 ; }
  //
  //
  // check if special "small-tau" algorithm is needed here 
  // 
  const long double  xmin = poly.xmin () ;
  const long double  xmax = poly.xmax () ;
  const long double  _tau = ( xmax - xmin ) * tau / 2   ;
  const unsigned int N    = poly.degree() ;
  const long double  t1   = Gaudi::Math::pow ( std::abs (  tau ) , N + 1 ) ;
  const long double  t2   = Gaudi::Math::pow ( std::abs ( _tau ) , N + 1 ) ;
  if ( s_small ( t1 ) || s_small ( t2 ) )
  { 
    const Gaudi::Math::Polynomial p ( poly ) ;
    return integrate ( p , tau , low , high ) ; 
  }
  //
  // try generic recursive, but numericaly highly unstable scheme  
  return _integrate_ ( poly , tau , low , high );
}
// ============================================================================


// ============================================================================
/*  get the integral between low and high for a product of
 *  polynom and the exponential function with the exponent tau
 *  \f[  \int \mathcal{P} e^{\tau x } \mathrm{d}x \f] 
 *  @param poly  polynomial
 *  @param tau   slope parameter for exponential 
 */
// ============================================================================
double Gaudi::Math::integrate 
( const Gaudi::Math::Polynomial& poly ,
  const double                   tau  ) 
{
  //
  if      ( s_zero  ( tau ) || s_small ( tau ) ) 
  { return  poly.integral ( poly.xmin() , poly.xmax() ) ; } 
  else if ( poly.zero()                        ) { return  0 ; }
  //
  const long double xmin = poly.xmin () ;
  const long double xmax = poly.xmax () ;
  //
  const long double _tau =              ( xmax - xmin ) * tau / 2   ;
  const long double _fac = std::exp   ( ( xmax + xmin ) * tau / 2 ) ;
  //
  if      ( s_zero  ( _tau ) || s_small ( _tau ) ) 
  { return  poly.integral ( poly.xmin() , poly.xmax() ) * _fac ; }
  //
  long double result = 0 ;
  const std::vector<double>& pars = poly.pars() ;
  for ( unsigned short i = 0 ; i < pars.size() ; ++i )
  { 
    const long double p = pars[i] ;
    if ( s_zero ( p ) ) { continue ; }                 // CONTINUE 
    //
    result += p * Gaudi::Math::beta_N ( i , -_tau ) ;  // NOTE THE SIGN!!
  }
  //
  return result * ( xmax - xmin ) * _fac / 2 ;
}
// ============================================================================


// ============================================================================
// The END  
// ============================================================================
