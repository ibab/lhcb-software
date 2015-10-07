// $Id$ 
// ============================================================================
// Include files 
// ============================================================================
// STD& STL
// ============================================================================
#include <array>
#include <climits>
#include <cassert>
// ============================================================================
// LHCbMath 
// ============================================================================
#include "LHCbMath/LHCbMath.h"
#include "LHCbMath/NSphere.h"
#include "LHCbMath/Power.h"
#include "LHCbMath/Choose.h"
#include "LHCbMath/Bernstein.h"
#include "LHCbMath/Polynomials.h"
#include "LHCbMath/MoreFunctions.h"
// ============================================================================
/** @file 
 *  Implementation file for functions, related to Bernstein's polynomnials 
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
namespace 
{
  // ==========================================================================
  /// equality criteria for doubles
  const LHCb::Math::Equal_To<double> s_equal ; // equality criteria for doubles
  /// zero for doubles  
  const LHCb::Math::Zero<double>     s_zero  ; // zero for doubles
  /// zero fo vectors 
  const LHCb::Math::Zero< std::vector<double> > s_vzero ; // zero for vectors
  ///
  static_assert( std::numeric_limits<double>::is_specialized , 
                 "std::numeric_limits<double> is not specialized" ) ;
  static_assert( std::numeric_limits<long double>::is_specialized , 
                 "std::numeric_limits<long double> is not specialized" ) ;
  /// machine epsilon (double)
  const double s_epsilon   = std::numeric_limits<double>::epsilon() ;
  /// machine epsilon (long double) 
  const double s_ld_epsilon = std::numeric_limits<long double>::epsilon() ;
  /// small value 
  const LHCb::Math::Small<long double> s_small
  ( 2.0L * std::numeric_limits<double>::epsilon() ) ;
  // ==========================================================================
  // De Casteljau's algorithm
  template <class ITERATOR>
  long double _casteljau_
  ( ITERATOR          first ,
    ITERATOR          last  ,
    const long double t0    ,
    const long double t1    )
  {
    // the trivial cases
    if      ( first == last    ) { return 0       ; }
    //
    const std::size_t len  = std::distance ( first , last  ) ;
    //
    if      ( 1 == len ) { return       *first                        ; }
    else if ( 2 == len ) { return t1 * (*first) + t0 * ( *(first+1) ) ; }
    //
    ITERATOR second = --last ;
    //
    // prepare recursion
    for ( ITERATOR it = first ; it != second ; ++it )
    { *it = t1 * ( *it )  + t0 * ( *( it+1 ) ) ; }
    //
    // recursion
    return _casteljau_ ( first , second , t0 , t1 ) ;
  }
  // ==========================================================================
}
// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Bernstein::Bernstein
( const unsigned short      N    ,
  const double              xmin ,
  const double              xmax )
  : Gaudi::Math::PolySum ( N ) 
  , m_xmin ( std::min ( xmin , xmax ) )
  , m_xmax ( std::max ( xmin , xmax ) )
{}
// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Bernstein::Bernstein
( const std::vector<double>& pars ,
  const double               xmin ,
  const double               xmax )
  : Gaudi::Math::PolySum ( pars ) 
  , m_xmin ( std::min ( xmin , xmax ) )
  , m_xmax ( std::max ( xmin , xmax ) )
{}
// ============================================================================
// construct the basic bernstein polinomial
// ============================================================================
Gaudi::Math::Bernstein::Bernstein 
( const Gaudi::Math::Bernstein::Basic& bb   , 
  const double                         xmin , 
  const double                         xmax ) 
  : Gaudi::Math::PolySum ( bb.N()  ) 
  , m_xmin ( std::min ( xmin , xmax ) )
  , m_xmax ( std::max ( xmin , xmax ) )
    //
{
  if ( bb.k() <= bb.N() ) { m_pars[ bb.k() ] = 1 ; } 
}
// ============================================================================
// copy assignement 
// ============================================================================
Gaudi::Math::Bernstein&
Gaudi::Math::Bernstein::operator=( const Gaudi::Math::Bernstein&  right ) 
{
  if ( &right == this ) { return *this ; }
  m_xmin = right.m_xmin ;
  m_xmax = right.m_xmax ;
  Gaudi::Math::PolySum::operator=( right ) ;
  return *this ;
}
// ============================================================================
// move assignement 
// ============================================================================
Gaudi::Math::Bernstein&
Gaudi::Math::Bernstein::operator=(       Gaudi::Math::Bernstein&& right ) 
{
  if ( &right == this ) { return *this ; }
  m_xmin = right.m_xmin ;
  m_xmax = right.m_xmax ;
  Gaudi::Math::PolySum::operator=( std::move ( right ) ) ;
  return *this ;
}
// ============================================================================
// is it a increasing function?
// ============================================================================
bool Gaudi::Math::Bernstein::increasing   () const 
{
  if ( m_pars.size() <= 1 ) { return true ; }
  for ( std::vector<double>::const_iterator it = m_pars.begin() + 1 ; 
        m_pars.end() != it ; ++it ) 
  { if (  (*(it-1)) > (*it) && !s_equal ( *(it-1) , *it ) ) { return false ; } }
  return true ;
}
// ============================================================================
// is it a decreasing function?
// ============================================================================
bool Gaudi::Math::Bernstein::decreasing   () const 
{
  if ( m_pars.size() <= 1 ) { return true ; }
  for ( std::vector<double>::const_iterator it = m_pars.begin() + 1 ; 
        m_pars.end() != it ; ++it ) 
  { if (  (*(it-1)) < (*it) && !s_equal ( *(it-1) , *it ) ) { return false ; } }
  return true ;
}
// ============================================================================
// is it a constant function?
// ============================================================================
bool Gaudi::Math::Bernstein::constant () const 
{
  //
  if ( m_pars.size() <= 1 ) { return true ; }
  for ( std::vector<double>::const_iterator it = m_pars.begin() + 1 ; 
        m_pars.end() != it ; ++it ) 
  { if ( !s_equal ( *(it-1) ,  *it ) ) { return false ; } }
  //
  return true ;
}
// ============================================================================
// get the integral between xmin and xmax
// ============================================================================
double Gaudi::Math::Bernstein::integral () const
{
  return
    ( m_xmax - m_xmin ) *
    std::accumulate ( m_pars.begin() , m_pars.end() , 0.0 ) / npars() ;
}
// ============================================================================
/*  get indefinite integral  as function object 
 *  \f$ I(x) = \int^{x}_{x_{min}} B(t) dt + C \f$
 *  @param C the integration constant   
 */
// ============================================================================
Gaudi::Math::Bernstein
Gaudi::Math::Bernstein::indefinite_integral 
( const double C ) const 
{
  //
  std::vector<long double> ck ( npars() + 1 , 0.0 ) ;
  std::partial_sum   ( m_pars.begin () , m_pars.end   () ,  ck.begin() + 1 ) ;
  Gaudi::Math::scale ( ck , ( m_xmax - m_xmin ) / npars() ) ;
  //
  // add the integration constant 
  if ( !s_zero ( C ) ) 
  {
    for ( std::vector<long double>::iterator ic = ck.begin() ; ck.end() != ic ; ++ic ) 
    { (*ic) += C ; }
  }
  //
  return Gaudi::Math::Bernstein ( ck.begin() , ck.end  () , m_xmin , m_xmax ) ;
}
// ============================================================================
double Gaudi::Math::Bernstein::integral ( const double low  ,
                                          const double high ) const 
{
  //
  if      ( s_equal ( low , high )           ) { return  0 ; }
  else if ( low  >  high                     ) { return -1*integral ( high   , low    ) ; }
  else if ( high <= xmin () || low >= xmax() ) { return  0 ; }
  else if ( s_vzero ( m_pars )               ) { return  0 ; }  
  else if ( s_equal ( low  , m_xmin ) && 
            s_equal ( high , m_xmax )        ) { return integral () ; }          
  //
  const double xlow  = std::max ( low  , m_xmin ) ;
  const double xhigh = std::min ( high , m_xmax ) ;
  if ( xlow > xhigh                          ) { return 0 ;}
  //
  if ( 1 == npars() ) { return ( xhigh - xlow ) * m_pars[0] ; }
  //
  if ( s_equal ( xlow  , m_xmin ) && 
       s_equal ( xhigh , m_xmax ) ) { return integral () ; }
  //
  // make integration: 
  //
  std::vector<long double> ck ( npars() + 1 , 0.0 ) ;
  std::partial_sum ( m_pars.begin () , m_pars.end   () ,  ck.begin() + 1 ) ;
  Gaudi::Math::scale ( ck , ( m_xmax - m_xmin ) / npars() ) ;
  //
  const Gaudi::Math::Bernstein b_int ( ck.begin() ,
                                       ck.end  ()  , m_xmin , m_xmax ) ;
  //
  return b_int ( xhigh ) - b_int ( xlow ) ;
}
// ============================================================================
Gaudi::Math::Bernstein
Gaudi::Math::Bernstein::derivative () const 
{
  //
  std::vector<long double>   ck ( npars() , 0 ) ;
  std::adjacent_difference ( m_pars.begin () , m_pars.end() , ck.begin() ) ;
  Gaudi::Math::scale ( ck , ( npars() - 1 )/ ( m_xmax - m_xmin ) ) ;
  //
  return Gaudi::Math::Bernstein ( ck.begin() + 1 , ck.end() ,  m_xmin  , m_xmax ) ;
}
// ============================================================================
double Gaudi::Math::Bernstein::derivative ( const double x   ) const 
{
  if      ( m_pars.size() <= 1       ) { return 0 ; }
  else if ( x < m_xmin || x > m_xmax ) { return 0 ; }
  //
  std::vector<long double>   ck ( npars() , 0 ) ;
  std::adjacent_difference ( m_pars.begin () , m_pars.end() , ck.begin() ) ;
  //
  // get the t-values
  //
  const double t0 = t ( x ) ;
  const double t1 = 1 - t0  ;
  //
  return
    _casteljau_ ( ck.begin() + 1 , ck.end() , t0 , t1 ) * ( npars()-1 )  / ( m_xmax - m_xmin ) ;
}
// ============================================================================
// get the value
// ============================================================================
double Gaudi::Math::Bernstein::operator () ( const double x ) const
{
  //
  // treat the trivial cases
  //
  if      ( m_pars.empty()                             ) { return 0 ; }
  //  needed for the proper integration with an exponential 
  else if ( s_equal ( x , m_xmin )                     ) { return m_pars [0]    ; }
  else if ( s_equal ( x , m_xmax )                     ) { return m_pars.back() ; }
  else if ( x < m_xmin || x > m_xmax                   ) { return 0 ; }
  else if ( 1 == npars ()                              ) { return m_pars [0]    ; }
  else if ( s_vzero ( m_pars )                         ) { return 0 ; }
  //
  // get the t-values
  //
  const long double t0 = t ( x ) ;
  const long double t1 = 1 - t0  ;
  //
  // start de casteljau algorithm:
  //
  // use fixed size: 
  if (  npars() < 16 ) 
  {
    std::array<long double,16> _pars;
    std::copy( m_pars.begin() , m_pars.end() , _pars.begin() ) ;
    return _casteljau_ ( _pars.begin() , _pars.begin() + npars() , t0 , t1 ) ;
  }
  // generic case:
  std::vector<long double> dcj ( m_pars.begin() , m_pars.end() ) ;
  return _casteljau_ ( dcj.begin() , dcj.end() , t0 , t1 ) ;
}
// ============================================================================
Gaudi::Math::Bernstein&
Gaudi::Math::Bernstein::operator+=( const double a ) 
{
  LHCb::Math::shift ( m_pars , a ) ;
  return *this ;
}
// ============================================================================
Gaudi::Math::Bernstein&
Gaudi::Math::Bernstein::operator*=( const double a ) 
{
  LHCb::Math::scale ( m_pars , a ) ;
  return *this ;
}
// ============================================================================
Gaudi::Math::Bernstein&
Gaudi::Math::Bernstein::operator-=( const double a ) 
{
  LHCb::Math::shift ( m_pars , -a ) ;
  return *this ;
}
// ============================================================================
Gaudi::Math::Bernstein&
Gaudi::Math::Bernstein::operator/=( const double a ) 
{
  LHCb::Math::scale ( m_pars , 1/a ) ;
  return *this ;
}
// ============================================================================
Gaudi::Math::Bernstein
Gaudi::Math::Bernstein::operator-() const 
{
  Bernstein b ( *this ) ;
  LHCb::Math::negate ( b.m_pars ) ;
  return b ;
}
// ============================================================================
// Sum of Bernstein polynomial and a constant 
// ============================================================================
Gaudi::Math::Bernstein
Gaudi::Math::Bernstein::__add__   ( const double value ) const 
{ return (*this) + value ; }
// ============================================================================
// Sum of Bernstein polynomial and a constant 
// ============================================================================
Gaudi::Math::Bernstein
Gaudi::Math::Bernstein::__radd__  ( const double value ) const 
{ return value + (*this) ; }
// ============================================================================
// Product of Bernstein polynomial and a constant
// ============================================================================
Gaudi::Math::Bernstein
Gaudi::Math::Bernstein::__mult__  ( const double value ) const 
{ return (*this) * value ; }
// ============================================================================
// Product of Bernstein polynomial and a constant
// ============================================================================
Gaudi::Math::Bernstein
Gaudi::Math::Bernstein::__rmult__ ( const double value ) const 
{ return value * (*this) ; }
// ============================================================================
// Subtract a constant from Benrstein polynomial
// ============================================================================
Gaudi::Math::Bernstein
Gaudi::Math::Bernstein::__sub__  ( const double value ) const 
{ return (*this) - value ; }
// ============================================================================
// Subtract Bernstein polynomial from a constant 
// ============================================================================
Gaudi::Math::Bernstein
Gaudi::Math::Bernstein::__rsub__ ( const double value ) const 
{ return value - (*this) ; }
// ============================================================================
// Divide Benrstein polynomial by a constant 
// ============================================================================
Gaudi::Math::Bernstein
Gaudi::Math::Bernstein:: __div__   ( const double value ) const 
{ return (*this) / value ; }
// ============================================================================
// Negate Bernstein polynomial 
// ============================================================================
Gaudi::Math::Bernstein
Gaudi::Math::Bernstein::__neg__ ()  const 
{ return -(*this); }
// ============================================================================
namespace 
{
  // ==========================================================================
  inline long double
  c_nk ( const unsigned short n , 
         const unsigned short k ) 
  {
    return 
      n < 63 ? 
      Gaudi::Math::choose        ( n , k ) : 
      Gaudi::Math::choose_double ( n , k ) ;  
  }
  // ==========================================================================
}
// ============================================================================
/*  elevate it: 
 *  represent as Bernstein polynomial of order N+r 
 *  @param r  INPUT increase of degree 
 *  @return new polynomial of order N+r 
 */
// ============================================================================
Gaudi::Math::Bernstein
Gaudi::Math::Bernstein::elevate ( const unsigned short r ) const 
{
  // no need in elevation 
  if ( 0 == r ){ return *this ; }
  //
  std::vector<long double>    nc ( npars  () + r ) ; // new coefficients 
  const std::vector<double>&  oc =  pars  () ;       // old coefficients 
  const unsigned short         n = degree () ;
  //
  for ( unsigned short k = 0 ; k < nc.size() ; ++k ) 
  {
    const unsigned short jmax = std::min ( n , k ) ;
    const unsigned short jmin = k <= r ? 0 : k - r ;
    for ( unsigned short j = jmin ; j <= jmax ; ++j ) 
    {
      const long double  cj = oc[j] ;
      if ( s_zero ( cj ) ) { continue ; }
      //
      nc[k] +=  c_nk ( r , k - j ) * c_nk ( n , j ) / c_nk ( n+r , k ) * cj ;
    }
  }
  return Bernstein ( nc.begin() , nc.end  () , xmin() , xmax() ) ;
}
// ============================================================================
/*  reduce it
 *  represent as Bernstein polynomial of order N0r 
 *  @param r  INPUT increase of degree 
 *  @return new polynomial of order N-r 
 */
// ============================================================================
Gaudi::Math::Bernstein
Gaudi::Math::Bernstein::reduce ( const unsigned short r ) const 
{
  // no need in reducing
  if ( 0 == r ){ return *this ; }
  //
  const unsigned short n    = degree () ;
  const unsigned short newd = r < n ?  n - r : 0 ;
  const unsigned short newr = n - newd ;
  //
  std::vector<long double>    nc ( newd + 1 ) ; // new coefficients 
  const std::vector<double>&  oc =  pars  ()  ; // old coefficients 
  //
  for ( unsigned short k = 0 ; k < nc.size() ; ++k ) 
  {
    for ( unsigned short j = 0 ; j <= k ; ++j ) 
    {
      const long double  cj = oc[j] ;
      if ( s_zero ( cj ) ) { continue ; }
      //
      nc[k] +=  
        ( 0 == ( k - j) %2 ? 1 : -1 ) 
        * c_nk ( k - j + newr - 1 , newr - 1 ) * c_nk ( n , j ) 
        / c_nk ( n - newr , k ) * cj ;
    }
  }
  return Bernstein ( nc.begin() , nc.end  () , xmin() , xmax() ) ;
}
// ============================================================================
/* de Casteljau algorithm for summation of Bernstein polynomials 
 *  \f$ f(x) = \sum_i p_i B_ik(x) \f$
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-02-10
 */
// ============================================================================
double Gaudi::Math::casteljau
( const std::vector<double>& pars , 
  const double               x    ) 
{
  std::vector<long double> _tmp ( pars.begin() , pars.end () ) ;
  //
  const long double t0 =     x  ;
  const long double t1 = 1 - t0 ;
  //
  return _casteljau_ ( _tmp.begin() , _tmp.end  () , t0 , t1 ) ;
}
// ============================================================================
namespace 
{
  inline short signm1 ( const long i ) { return 0 == i%2 ? 1 : -1 ; }
  // ==========================================================================
  /** transformation matrix from legendre to bernstein basis 
   *  @see http://www.sciencedirect.com/science/article/pii/S0377042700003769 eq.20 
   */
  inline 
  double l2b_mtrx ( const unsigned short j , 
                    const unsigned short k ,
                    const unsigned short n ) 
  {
    //
    const unsigned short imin = std::max ( 0 , j + k - n ) ;
    const unsigned short imax = std::min ( j ,     k     ) ;
    long long r = 0 ;
    for ( unsigned short i = imin ; i <= imax ; ++i ) 
    {
      0 == ( k + i ) % 2 ? 
        r +=
        Gaudi::Math::choose ( j     , i     ) * 
        Gaudi::Math::choose ( k     , i     ) * 
        Gaudi::Math::choose ( n - j , k - i ) :
        r -=
        Gaudi::Math::choose ( j     , i     ) * 
        Gaudi::Math::choose ( k     , i     ) * 
        Gaudi::Math::choose ( n - j , k - i ) ;
    }
    //
    return r / double ( Gaudi::Math::choose ( n , k ) ) ;
  }
  // ==========================================================================
  /** transformation matrix from chebyshev to bernstein basis 
   *  http://www.degruyter.com/view/j/cmam.2003.3.issue-4/cmam-2003-0038/cmam-2003-0038.xml  eq. 15
   */
  inline 
  long double c2b_mtrx ( const unsigned short j , 
                         const unsigned short k ,
                         const unsigned short n ) 
  {
    const unsigned short imin = std::max ( 0 , j + k - n ) ;
    const unsigned short imax = std::min ( j ,     k     ) ;
    long long r = 0 ;
    for ( unsigned short i = imin ; i <= imax ; ++i ) 
    {
      0 == ( k - i ) % 2 ? 
        r +=
        Gaudi::Math::choose ( 2 * k , 2 * i ) * 
        Gaudi::Math::choose ( n - k , j - i ) :
        r -=
        Gaudi::Math::choose ( 2 * k , 2 * i ) * 
        Gaudi::Math::choose ( n - k , j - i ) ;
    }
    //
    return r / (  (long double)  Gaudi::Math::choose ( n , j ) ) ;
  }
  // ==========================================================================
  /** transformation matrix from monomial to bernstein basis
   */
  inline 
  long double m2b_mtrx 
  ( const unsigned short j , 
    const unsigned short k ,
    const unsigned short n ) 
  {
    //
    return
      j < k ? 0.0 : 
      (double) ( Gaudi::Math::choose ( j , k ) ) / 
      (double) ( Gaudi::Math::choose ( n , k ) ) ; 
  }
  // ==========================================================================
  /// affine transformation of polynomial
  inline 
  long double m2m_mtrx_2
  ( const unsigned short j , 
    const unsigned short k ) 
  {
    if ( k < j ) { return 0 ; }
    const long double c = 
      Gaudi::Math::choose ( k , j ) * Gaudi::Math::pow ( 2 , j ) ;
    //
    return 0 == ( k - j ) % 2 ?  c : -c ;
  }
  // ==========================================================================
}
// ============================================================================
// constructor from Legendre polynomial
// ============================================================================
Gaudi::Math::Bernstein::Bernstein
( const Gaudi::Math::LegendreSum& poly )  
  : Gaudi::Math::PolySum ( poly.degree () ) 
  , m_xmin ( poly.xmin() ) 
  , m_xmax ( poly.xmax() ) 
{
  for ( unsigned short i = 0 ; i < npars() ; ++i ) 
  { 
    for ( unsigned short k = 0 ; k < npars() ; ++k ) 
    { 
      const double p = poly.par ( k ) ;
      if ( s_zero ( p ) ) { continue ; }
      m_pars[i] += l2b_mtrx ( i , k , degree() ) * p ; 
    } 
  }
}
// ============================================================================
// constructor from Chebyshev polynomial
// ============================================================================
Gaudi::Math::Bernstein::Bernstein
( const Gaudi::Math::ChebyshevSum& poly )  
  : Gaudi::Math::PolySum ( poly.degree () ) 
  , m_xmin ( poly.xmin() ) 
  , m_xmax ( poly.xmax() ) 
{
  //
  for ( unsigned short i = 0 ; i < npars() ; ++i ) 
  { 
    for ( unsigned short k = 0 ; k < npars() ; ++k ) 
    { 
      const double p = poly.par ( k ) ;
      if ( s_zero ( p ) ) { continue ; }
      m_pars[i] += c2b_mtrx ( i , k , degree() ) * p ; 
    } 
  }
  //
}
// ============================================================================
// constructor from simple monomial form 
// ============================================================================
Gaudi::Math::Bernstein::Bernstein
( const Gaudi::Math::Polynomial& poly )  
  : Gaudi::Math::PolySum ( poly.degree () ) 
  , m_xmin ( poly.xmin() ) 
  , m_xmax ( poly.xmax() ) 
{
  //
  const unsigned short np = npars() ;
  // 2-step transformation
  //
  // 1: affine transform to [0,1]
  //
  std::vector<double> _pars ( np ) ;
  for ( unsigned short i = 0 ; i < np ; ++i ) 
  { 
    for ( unsigned short k = i ; k < np ; ++k )  
    { 
      const double p = poly.par ( k )  ;
      if ( s_zero ( p ) ) { continue ; }
      _pars[i] += m2m_mtrx_2 ( i , k ) * p ; 
    } 
  }  
  //
  // 2: tramsform from shifted poly basis:
  //
  for ( unsigned short i = 0 ; i < np ; ++i ) 
  { 
    for ( unsigned short k = 0 ; k <= i ;  ++k )  // ATTENTION!!
    { 
      const double p = _pars[ k ] ;
      if ( s_zero ( p ) ) { continue ; }
      m_pars[i] += m2b_mtrx ( i , k , degree() ) * p ; 
    } 
  }
  //
}
// ============================================================================

// ============================================================================
/* get the integral between 0 and 1 for a product of basic  Bernstein
 *  polynom and the exponential function with the exponent tau
 *  \f[ \int_{0}^{1} \mathcal{B}_{ik} e^{\tau x } \mathrm{d}x \f] 
 *  @param poly  bernstein polynomial
 *  @param tau   slope parameter for an exponential function
 */
// ============================================================================
double Gaudi::Math::integrate 
( const Gaudi::Math::Bernstein::Basic& b    ,
  const double                         tau  ) 
{
  //
  if ( b.k() > b.N()  ) { return 0                   ; }
  if ( s_zero ( tau ) ) { return 1.0 / ( b.N() + 1 ) ; }
  //
  // make use Kummer function as default scenario 
  return Gaudi::Math::kummer ( b.k () + 1 , b.N () + 2 , tau ) / ( b.N() + 1 ) ;
  //
}
// ============================================================================
/* get the integral between \f$x_{min}\f$ and \f$x_{max}\f$ for a product of Bernstein
 *  polynom and the exponential function with the exponent tau
 *  \f[  \int_{x_{min}}^{x_{max}} \mathcal{B} e^{\tau x } \mathrm{d}x \f] 
 *  @param poly  bernstein polynomial
 *  @param tau   slope parameter for exponential 
 */
// ============================================================================
double Gaudi::Math::integrate 
( const Gaudi::Math::Bernstein& poly ,
  const double                  tau  ) 
{
  if ( s_zero ( tau ) ) { return poly.integral() ; }
  //
  const long double xmin = poly.xmin () ;
  const long double xmax = poly.xmax () ;
  //
  const long double _tau =            ( xmax - xmin ) * tau ;
  const long double _fac = std::exp   (  tau * xmin ) ;
  //
  long double result = 0 ;
  const unsigned short       N    = poly.degree () ;
  const std::vector<double>& pars = poly.pars   () ;
  //
  for ( std::vector<double>::const_iterator ip = pars.begin() ; pars.end() != ip ; ++ip ) 
  {
    if ( s_zero ( *ip ) ) { continue ; } // skip zeroes 
    const unsigned short k =  ip - pars.begin() ;
    const long double    p = *ip ;
    result +=  p * integrate ( Gaudi::Math::Bernstein::Basic ( k , N ) , _tau ) ;
  }
  //
  return result * ( xmax - xmin ) * _fac ;
}
// ============================================================================
namespace 
{
  // ==========================================================================
  inline long double r_kNm  
  ( const unsigned short k , 
    const unsigned short N , 
    const unsigned short m ) 
  {
    long double r = 1.0L ;
    for ( unsigned short i = 1 ; i <= m ; ++i ) 
    {
      r *= ( k + i ) ;
      r /= ( N + i ) ;
      r /= i         ;
    }    
    return r ;
  }
  // ==========================================================================
}
// ============================================================================
/* get the integral between 0 and 1 for a product of basic  Bernstein
 *  polynom and monomial or degree m 
 *  \f[  \int_{0}^{1} \mathcal{B} \frac{x^m}{m!} \mathrm{d}x \f] 
 *  @param b     basic bernstein polynomial
 *  @param m     degree of monomial 
 */
// ============================================================================ 
double Gaudi::Math::integrate_poly 
( const Gaudi::Math::Bernstein::Basic& b ,
  const unsigned short                 m )
{
  //
  const unsigned short N = b.N () ;
  const unsigned short k = b.k () ;
  //
  return r_kNm ( k , N , m ) / ( N + m + 1 ) ;
}
// ============================================================================ 
/*  get the integral between xmin and xmax Bernstein
 *  polynom and monomial or degree m 
 *  \f[  \int_{x_min}^{x_max} \mathcal{B} \frac{x^m}{m!} \mathrm{d}x \f] 
 *  @param b     basic bernstein polynomial
 *  @param m     degree of monomial 
 */
// ============================================================================ 
double Gaudi::Math::integrate_poly 
( const Gaudi::Math::Bernstein& b ,
  const unsigned short          m ) 
{
  //
  if ( 0 == m ) { return b.integral () ; }
  //
  const std::vector<double>& pars = b.pars()   ;
  const unsigned short       N    = b.degree() ;
  std::vector<long double>   nc  ( pars.size() , 0.0L ) ;
  for ( unsigned short k = 0 ; k < nc.size() ; ++k ) 
  {
    const long double ci = pars[k] ;
    if ( s_zero ( ci ) ) { continue ; }
    nc[k] = r_kNm ( k , N , m ) * ci ;  
  }
  //
  return 
    Gaudi::Math::pow ( b.xmax() - b.xmin()  , m + 1 ) * 
    std::accumulate  ( nc.begin() , nc.end() , 0.0L ) / ( N + m + 1 ) ;
}
// ============================================================================ 
namespace 
{
  // ==========================================================================
  long double _integrate_poly_ 
  ( const Gaudi::Math::Bernstein& b    ,
    const unsigned short          m    , 
    const double                  low  , 
    const double                  high )
  {
    const std::vector<double>& pars = b.pars()   ;
    const unsigned short       N    = b.degree() ;
    std::vector<long double>   nc  ( pars.size() + m , 0.0L ) ;
    for ( unsigned short k = 0 ; k < pars.size() ; ++k ) 
    {
      const long double ci = pars[k] ;
      if ( s_zero ( ci ) ) { continue ; }
      nc[ k + m ] = r_kNm ( k , N , m ) * ci ;  
    }
    //
    Gaudi::Math::Bernstein a ( nc.begin() , nc.end  () , b.xmax() , b.xmin() ) ;
    //
    return Gaudi::Math::pow ( b.xmax() - b.xmin()  , m ) * a.integral ( low , high ) ;  
  }
  // ==========================================================================
}
// ============================================================================ 
/** get the integral between xmin and xmax Bernstein
 *  polynom and monomial or degree m 
 *  \f[  \int_{low}^{high} \mathcal{B} \frac{(x-x_min)^m}{m!} \mathrm{d}x \f] 
 *  @param b     basic bernstein polynomial
 *  @param m     degree of monomial 
 *  @param low   low  integration limit 
 *  @param high  high integtation limit 
 */
// ============================================================================ 
double Gaudi::Math::integrate_poly 
( const Gaudi::Math::Bernstein& b    ,
  const unsigned short          m    , 
  const double                  low  , 
  const double                  high )
{
  //
  if      ( s_equal ( low , high )      ) { return  0 ; }
  else if ( 0 == m                      ) { return  b.integral ( low ,high ) ; }
  else if ( low  > high                 ) { return -integrate_poly ( b , m , high , low ) ; }
  else if ( high < b.xmin ()            ) { return  0 ; }
  else if ( low  > b.xmax ()            ) { return  0 ; } 
  else if ( low  < b.xmin ()            ) { return  integrate_poly ( b , m , b.xmin() , high     ) ; }
  else if ( high > b.xmax ()            ) { return  integrate_poly ( b , m , low      , b.xmax() ) ; }
  else if ( s_equal ( low  , b.xmin() ) && 
            s_equal ( high , b.xmax() ) ) { return  integrate_poly ( b , m ) ; }
  //
  // make the actual integration
  return _integrate_poly_ ( b , m , low , high ) ;
}
// ============================================================================
/*  get the integral between low and high for a product of Bernstein
 *  polynom and the exponential function with the exponent tau
 *  \f[  \int_{a}^{b} \mathcal{B} e^{\tau x } \mathrm{d}x \f] 
 *  @param poly  bernstein polynomial
 *  @param tau   slope parameter for exponential 
 *  @param low   low  integration range 
 *  @param high  high integration range 
 */
// ============================================================================
double Gaudi::Math::integrate 
( const Gaudi::Math::Bernstein& poly ,
  const double                  tau  ,
  const double                  low  , 
  const double                  high ) 
{
  if      ( s_small ( tau )           ) { return  poly.integral ( low , high ) ; }
  else if ( s_equal ( low , high )    ) { return  0 ; }
  else if ( poly.zero ()              ) { return  0 ; }
  else if ( low  >  high              ) { return -integrate ( poly , tau , high , low ) ; }  
  else if ( high <  poly.xmin () || 
            low  >  poly.xmax ()      ) { return  0 ; }
  else if ( low  <  poly.xmin ()      ) { return  integrate ( poly , tau , poly.xmin() , high        ) ; }
  else if ( high >  poly.xmax ()      ) { return  integrate ( poly , tau , low         , poly.xmax() ) ; }
  //
  if ( s_equal ( low  , poly.xmin() ) && 
       s_equal ( high , poly.xmax() ) ) { return integrate ( poly , tau ) ; }               
  //
  // start series expansion
  // 
  long double result =  poly.integral ( low , high ) ;
  long double dd1    = 1 ;
  long double dd2    = 1 ;
  long double taum   = 1 ;
  //
  const long double xmin = poly.xmin () ;
  // const long double xmax = poly.xmax () ;
  //
  // const long double _tau =            ( xmax - xmin ) * tau ;
  const long double _fac = std::exp   (  tau * xmin ) ;
  //
  for ( unsigned int m = 1 ; m < 10000 ; ++m ) 
  {
    taum   *=  tau ;
    dd2     = _integrate_poly_ ( poly , m , low , high ) * taum  ;
    result += dd2 ;
    if ( s_small ( dd1 / result ) && s_small ( dd2 / result ) ) { break ; }
    dd1     = dd2 ;
  }
  //
  return result * _fac ; 
}
// ============================================================================
// POSITIVE 
// ============================================================================


// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Positive::Positive
( const unsigned short      N    ,
  const double              xmin ,
  const double              xmax )
  : std::unary_function<double,double> ()
  , m_bernstein ( N , xmin , xmax )
  , m_sphere    ( N , 3 ) 
{
  updateBernstein () ;
}
// ============================================================================
// constructor from the list of phases 
// ============================================================================
Gaudi::Math::Positive::Positive
( const std::vector<double>& pars ,
  const double               xmin ,
  const double               xmax )
  : std::unary_function<double,double> ()
  , m_bernstein ( pars.size() , xmin , xmax )
  , m_sphere    ( pars , 3 ) 
{
  updateBernstein () ;
}
// ============================================================================
// constructor from the sphere with coefficients  
// ============================================================================
Gaudi::Math::Positive::Positive
( const Gaudi::Math::NSphere& sphere , 
  const double                xmin   , 
  const double                xmax   )
  : std::unary_function<double,double> ()
  , m_bernstein ( sphere.dim() , xmin , xmax )
  , m_sphere    ( sphere ) 
{
  updateBernstein () ;
}
// ============================================================================
// copy 
// ============================================================================
Gaudi::Math::Positive::Positive
( const Gaudi::Math::Positive&  right ) 
  : std::unary_function<double,double> ( right )
  , m_bernstein ( right.m_bernstein ) 
  , m_sphere    ( right.m_sphere    ) 
{}
// ============================================================================
// move 
// ============================================================================
Gaudi::Math::Positive::Positive
(       Gaudi::Math::Positive&& right ) 
  : std::unary_function<double,double> ( right )
  , m_bernstein ( std::move ( right.m_bernstein ) ) 
  , m_sphere    ( std::move ( right.m_sphere    ) ) 
{}
// ============================================================================
Gaudi::Math::Positive::~Positive() {}
// ============================================================================
// set k-parameter
// ============================================================================
bool Gaudi::Math::Positive::setPar ( const unsigned short k , const double value )
{
  //
  const bool update = m_sphere.setPhase ( k , value ) ;
  if ( !update ) { return false ; }   // no actual change 
  //
  return updateBernstein () ;
}
// =============================================================================
// update bernstein coefficients
// =============================================================================
bool Gaudi::Math::Positive::updateBernstein ()
{
  ///
  bool         update = false ;
  /// degree 
  const unsigned short o = degree() ;
  //
  const double   norm    = m_bernstein.npars() / 
    ( m_bernstein.xmax() -  m_bernstein.xmin () ) ;
  //
  // few simple cases 
  //
  if       ( 0 == o ) { return m_bernstein.setPar( 0 , norm ) ; }
  else if  ( 1 == o )  
  {
    const bool updated0 = m_bernstein.setPar ( 0 , m_sphere.x2(0) * norm ) ;
    update              = updated0 || update ;
    const bool updated1 = m_bernstein.setPar ( 1 , m_sphere.x2(1) * norm ) ;
    update              = updated1 || update ;
    //
    return update ;
  }
  //
  // get the parameters of "global" parabola 
  //
  const double a0     = m_sphere.x2 ( 0 ) ;
  const double a1_    = m_sphere.x2 ( 1 ) ;
  const double a2     = m_sphere.x2 ( 2 ) ;
  //
  const double a1_min = - std::sqrt ( a0 * a2 ) ; //
  const double a1     = a1_min + a1_ ;            // positivity constraint 
  //
  // simple parabola (probably the most common case in practice) 
  //
  if ( 2 == o ) 
  {
    const double norm2  = norm / ( a0 + a1 + a2 ) ;
    //
    const bool updated0 = m_bernstein.setPar ( 0 , a0 * norm2 ) ;
    update              = updated0 || update ;
    const bool updated1 = m_bernstein.setPar ( 1 , a1 * norm2 ) ;
    update              = updated1 || update ;
    const bool updated2 = m_bernstein.setPar ( 2 , a2 * norm2 ) ;
    update              = updated2 || update ;
    //
    return update ;
  }
  //
  // generic case 
  //
  // get the coefficients from the sphere 
  // this actually represent the positive polynomial with 
  //   - f  (0)=0 
  //   - f' (0)=0  
  //   - f''(0)=0 
  std::vector<double> v ( m_sphere.nX() ) ;
  const unsigned short vs = v.size() ;
  for ( unsigned short ix = 3 ; ix < vs ; ++ix ) { v[ix] = m_sphere.x2 ( ix ) ; }
  //
  const double c0 = a0         ;
  const double c1 = 2*(a1-a0)  ;
  const double c2 = a0+a2-2*a1 ; 
  //
  for ( unsigned short k = 0 ; k < vs ; ++k ) 
  {
    double vv = c0 ;
    const double r1 =  double(k) / o ;
    if ( 0 != k ) { vv += r1             * c1             ; }
    if ( 1 <  k ) { vv += r1 * ( k - 1 ) * c2 / ( o - 1 ) ; }
    v[k] +=  vv ;
    if ( 0 != v[k] && s_zero ( v[k] ) ) {  v[k] = 0 ; }
  }
  //
  const double isum = norm / std::accumulate ( v.begin() , v.end() , 0.0 ) ;
  //
  for ( unsigned short ix = 0 ; ix < m_sphere.nX() ; ++ix ) 
  {
    const bool updated = m_bernstein.setPar ( ix , v[ix] * isum ) ;
    update = updated || update ;
  }
  //
  return update ;
}
// ============================================================================
// copy assignement 
// ============================================================================
Gaudi::Math::Positive&
Gaudi::Math::Positive::operator=( const Gaudi::Math::Positive&  right ) 
{
  if ( &right == this ) { return *this ; }
  m_bernstein = right.m_bernstein ;
  m_sphere    = right.m_sphere    ;
  return *this ;
}
// ============================================================================
// move assignement 
// ============================================================================
Gaudi::Math::Positive&
Gaudi::Math::Positive::operator=(      Gaudi::Math::Positive&& right ) 
{
  if ( &right == this ) { return *this ; }
  m_bernstein = std::move ( right.m_bernstein ) ;
  m_sphere    = std::move ( right.m_sphere    ) ;
  return *this ;
}
// =============================================================================
// get the integral between xmin and xmax
// =============================================================================
double Gaudi::Math::Positive::integral () const { return 1 ; } 
// =============================================================================
// get the integral between low and high 
// =============================================================================
double Gaudi::Math::Positive::integral
( const double low , const double high ) const 
{ 
  return 
    s_equal ( low  , xmin() ) && s_equal ( high , xmax() ) ? 1 :
    m_bernstein.integral ( low , high )  ; 
}
// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Monothonic::Monothonic 
( const unsigned short      N          ,
  const double              xmin       ,
  const double              xmax       , 
  const bool                increasing ) 
  : Gaudi::Math::Positive ( N , xmin , xmax )  
  , m_increasing          ( increasing      )  
{
  updateBernstein () ;
}
// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Monothonic::Monothonic 
( const std::vector<double>& pars       ,
  const double               xmin       ,
  const double               xmax       ,
  const bool                 increasing ) 
  : Gaudi::Math::Positive ( pars , xmin , xmax )  
  , m_increasing          ( increasing      )  
{
  updateBernstein () ;
}
// ============================================================================
// constructor from the spline 
// ============================================================================
Gaudi::Math::Monothonic::Monothonic 
( const Gaudi::Math::Positive& spline   ,
  const bool                 increasing ) 
  : Gaudi::Math::Positive ( spline      )  
  , m_increasing          ( increasing  )  
{
  updateBernstein () ;
}
// ============================================================================
// constructor from the spline 
// ============================================================================
Gaudi::Math::Monothonic::Monothonic 
( const Gaudi::Math::Monothonic& right ) 
  : Gaudi::Math::Positive ( right              )  
  , m_increasing          ( right.m_increasing )
{}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::Monothonic::~Monothonic (){}
// ============================================================================
// update bernstein coefficients
// =============================================================================
bool Gaudi::Math::Monothonic::updateBernstein ()
{
  //
  bool   update = false ;
  //
  // get sphere coefficients 
  std::vector<double> v ( m_sphere.nX() ) ;
  for ( unsigned short ix = 0 ; ix < m_sphere.nX() ; ++ix ) 
  { v[ix] = m_sphere.x2 ( ix ) * ( ix + 1 ) ; }
  //
  // integrate them and to get new coefficients
  if   ( m_increasing ) { std::partial_sum ( v. begin() , v. end() ,  v. begin() ) ; }
  else                  { std::partial_sum ( v.rbegin() , v.rend() ,  v.rbegin() ) ; }
  //
  const double isum = m_bernstein.npars() 
    / std::accumulate ( v.begin() , v.end() , 0.0 ) 
    / ( m_bernstein.xmax() -  m_bernstein.xmin () ) ;
  //
  for ( unsigned short ix = 0 ; ix < m_sphere.nX() ; ++ix ) 
  { 
    const bool updated = m_bernstein.setPar ( ix , v [ix] * isum ) ; 
    update = updated || update ;
  }
  //
  return update ;
}
// ============================================================================
// get the minimal value of function 
// ============================================================================
double Gaudi::Math::Monothonic::fun_min () const
{
  const std::vector<double>& ps = m_bernstein.pars() ;
  return  std::min( ps.front() , ps.back() ) ;
}
// ============================================================================
// get the maximal value of function 
// ============================================================================
double Gaudi::Math::Monothonic::fun_max () const
{
  const std::vector<double>& ps = m_bernstein.pars() ;
  return  std::max( ps.front() , ps.back() ) ;
}
// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Convex::Convex
( const unsigned short      N          ,
  const double              xmin       ,
  const double              xmax       , 
  const bool                increasing ,
  const bool                convex     ) 
  : Gaudi::Math::Monothonic ( N , xmin, xmax , increasing ) 
  , m_convex                ( convex )  
{
  updateBernstein () ;
}
// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Convex::Convex
( const std::vector<double>& pars       ,
  const double               xmin       ,
  const double               xmax       ,
  const bool                 increasing ,
  const bool                 convex     ) 
  : Gaudi::Math::Monothonic ( pars  , xmin, xmax , increasing ) 
  , m_convex                ( convex     )  
{
  updateBernstein () ;
}
// ============================================================================
// constructor from the 
// ============================================================================
Gaudi::Math::Convex::Convex
( const Gaudi::Math::Positive& poly      ,
  const bool                  increasing ,
  const bool                  convex     ) 
  : Gaudi::Math::Monothonic ( poly , increasing ) 
  , m_convex                ( convex     )  
{
  updateBernstein () ;
}
// ============================================================================
// constructor from the 
// ============================================================================
Gaudi::Math::Convex::Convex
( const Gaudi::Math::Monothonic& poly   ,
  const bool                     convex ) 
  : Gaudi::Math::Monothonic ( poly       ) 
  , m_convex                ( convex     )  
{
  updateBernstein () ;
}
// ============================================================================
// copy constructor 
// ============================================================================
Gaudi::Math::Convex::Convex ( const Gaudi::Math::Convex& right  ) 
  : Gaudi::Math::Monothonic ( right           ) 
  , m_convex                ( right.m_convex  )  
{}
// ============================================================================
Gaudi::Math::Convex::~Convex (){}
// ============================================================================
// update bernstein coefficients
// =============================================================================
bool Gaudi::Math::Convex::updateBernstein ()
{
  //
  bool   update = false ;
  //
  // get sphere coefficients 
  //
  std::vector<double>  v ( m_sphere.nX() ) ;
  const unsigned short vs = v.size()    ;
  //
  const std::array<double,2> a = { { m_sphere.x2(0) , m_sphere.x2(1) } };
  for ( unsigned short ix = 2 ; ix < vs ; ++ix ) 
  { v[ix] = m_sphere.x2 ( ix ) ; }
  //
  // integrate them twice and to get new coefficients
  std::partial_sum ( v.  begin() + 2 , v.  end()     ,  v.  begin() + 2 ) ; 
  std::partial_sum ( v.  begin() + 2 , v.  end()     ,  v.  begin() + 2 ) ; 
  //
  if ( !m_convex ) 
  {
    const  double last = v.back() ;
    for ( unsigned short k = 0 ; k < vs; ++k) 
    { 
      v[k] = last  - v[k] ; 
      if ( 0 != v[k] && s_zero ( v[k] ) ) {  v[k] = 0 ; }
    }
  }
  //
  if ( m_increasing != m_convex )
  { std::reverse ( v.begin() , v.end() ) ; }
  //
  // add a positive linear function 
  //
  const unsigned short d = degree() ;
  for ( unsigned short k = 0 ; k < vs ; ++k ) 
  {
    const double r1 =  double(k) / d ;
    //
    v[k] +=  
      m_increasing ?  
      a[0] +       r1   * a[1] :
      a[0] + ( 1 - r1 ) * a[1] ;
    //
    if ( 0 != v[k] && s_zero ( v[k] ) ) {  v[k] = 0 ; }
  }
  //
  const double isum = m_bernstein.npars() 
    / std::accumulate ( v.begin() , v.end() , 0.0 ) 
    / ( m_bernstein.xmax() -  m_bernstein.xmin () ) ;
  //
  for ( unsigned short ix = 0 ; ix < vs ; ++ix ) 
  { 
    const bool updated = m_bernstein.setPar ( ix , v [ix] * isum ) ; 
    update = updated || update ;
  }
  //
  return update ;  
}
// ============================================================================




// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::ConvexOnly::ConvexOnly 
( const unsigned short      N     ,
  const double              xmin   ,
  const double              xmax   , 
  const bool                convex ) 
  : Gaudi::Math::Positive ( N , xmin , xmax )  
  , m_convex          ( convex )  
{
  updateBernstein () ;
}
// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::ConvexOnly::ConvexOnly 
( const std::vector<double>& pars    ,
  const double               xmin    ,
  const double               xmax    ,
  const bool                 convex  ) 
  : Gaudi::Math::Positive ( pars , xmin , xmax )  
  , m_convex          ( convex      )  
{
  updateBernstein () ;
}
// ============================================================================
// constructor from the spline 
// ============================================================================
Gaudi::Math::ConvexOnly::ConvexOnly 
( const Gaudi::Math::Positive& poly   ,
  const bool                   convex ) 
  : Gaudi::Math::Positive ( poly   )  
  , m_convex              ( convex )  
{
  updateBernstein () ;
}
// ============================================================================
// constructor from the spline 
// ============================================================================
Gaudi::Math::ConvexOnly::ConvexOnly 
( const Gaudi::Math::ConvexOnly& right ) 
  : Gaudi::Math::Positive ( right )  
  , m_convex ( right.m_convex )  
{}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::ConvexOnly::~ConvexOnly (){}
// ============================================================================
// update bernstein coefficients
// =============================================================================
bool Gaudi::Math::ConvexOnly::updateBernstein ()
{
  //
  // linear function...
  if ( 2 > degree() ) { return Gaudi::Math::Positive::updateBernstein() ; }
  //
  bool   update = false ;
  //
  // get sphere coefficients 
  //
  std::vector<double>  v ( m_sphere.nX() ) ;
  const unsigned short vs = v.size()    ;
  //
  // get parameters from the sphere:
  //
  if ( !m_convex ) 
  {
    const std::array<double,2> a = { { m_sphere.x2(0) , m_sphere.x2(1) } };
    for ( unsigned short ix = 2 ; ix < vs ; ++ix ) 
    { v[ix] = m_sphere.x2 ( ix ) ; }
    //
    // integrate them twice and to get new coefficients
    std::partial_sum ( v.  begin() + 2 , v.  end()     ,  v.  begin() + 2 ) ; 
    std::partial_sum ( v.  begin() + 2 , v.  end()     ,  v.  begin() + 2 ) ; 
    //
    {
      const  double last = v.back() ;
      for ( unsigned short k = 0 ; k < vs; ++k) 
      { 
        v[k] = last  - v[k] ; 
        if ( 0 != v[k] && s_zero ( v[k] ) ) {  v[k] = 0 ; }
      }
    }
    //
    // subtract the linear component and 
    // add positive linear function
    //
    const double v1 = a[0] - v.front() ;
    const double v2 = a[1] - v.back()  ;
    const unsigned int   d = degree() ;
    for ( unsigned short k = 0 ; k < vs ; ++k ) 
    {
      const double r1 =  double(k)  / d ;
      v[k] +=  ( 1 - r1 ) * v1  + r1 * v2 ;
      if ( 0 != v[k] && s_zero ( v[k] ) ) {  v[k] = 0 ; }
    }
  }
  else 
  { 
    std::array<double,3> a = { { m_sphere.x2(0) , 
                                 m_sphere.x2(1) , 
                                 m_sphere.x2(2) } };
    for ( unsigned short ix = 3 ; ix < vs ; ++ix ) 
    { v[ix] = m_sphere.x2 ( ix ) ; }
    // integrate them twice and to get new coefficients
    std::partial_sum ( v.  begin() + 3 , v.  end()     ,  v.  begin() + 3 ) ; 
    std::partial_sum ( v.  begin() + 3 , v.  end()     ,  v.  begin() + 3 ) ; 
    //    
    const double a0 = a[0] ;
    const double a2 = a[2] ;
    const double a1_min = -1*std::sqrt ( a0 * a2 ) ;
    const double a1_max = 0.5 * ( a0 + a2 ) ;
    //
    const double a1 = a1_min + a[1] * ( a1_max - a1_min ) ;
    //
    const double c0 = a0         ;
    const double c1 = 2*(a1-a0)  ;
    const double c2 = a0+a2-2*a1 ; 
    //
    const unsigned int   d = degree() ;
    for ( unsigned short k = 0 ; k < vs ; ++k ) 
    {
      double vv = c0 ;
      const double r1 =  double(k) / d ;
      if ( 0 != k ) { vv += r1 * c1 ; }
      if ( 1 <  k ) { vv += r1 * ( k - 1 ) * c2 / ( d - 1 ) ; }
      v[k] +=  vv ;
      if ( 0 != v[k] && s_zero ( v[k] ) ) {  v[k] = 0 ; }
    }
  }
  //
  const double isum = m_bernstein.npars() 
    / std::accumulate ( v.begin() , v.end() , 0.0 ) 
    / ( m_bernstein.xmax() -  m_bernstein.xmin () ) ;
  //
  for ( unsigned short ix = 0 ; ix < vs ; ++ix ) 
  { 
    const bool updated = m_bernstein.setPar ( ix , v [ix] * isum ) ; 
    update = updated || update ;
  }
  //
  return update ;
}
// ============================================================================






// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Bernstein2D::Bernstein2D
( const unsigned short      nX   ,
  const unsigned short      nY   ,
  const double              xmin ,
  const double              xmax ,
  const double              ymin ,
  const double              ymax )
  : std::binary_function<double,double,double> ()
//
  , m_nx   ( nX ) 
  , m_ny   ( nY )
//
  , m_pars ( ( nX + 1 ) * ( nY + 1 ) , 0.0 )
//
  , m_xmin ( std::min ( xmin , xmax ) )
  , m_xmax ( std::max ( xmin , xmax ) )
  , m_ymin ( std::min ( ymin , ymax ) )
  , m_ymax ( std::max ( ymin , ymax ) )
    //
  , m_bx   () 
  , m_by   ()
{
  //
  typedef  Gaudi::Math::Bernstein::Basic BB ;
  for ( unsigned short ix = 0 ; ix <= nX ; ++ix ) 
  { m_bx.push_back ( Bernstein ( BB ( ix , nX ) , xmin , xmax ) ) ; }
  //
  for ( unsigned short iy = 0 ; iy <= nY ; ++iy ) 
  { m_by.push_back ( Bernstein ( BB ( iy , nY ) , ymin , ymax ) ) ; }
  //
}
// ============================================================================
// get the value
// ============================================================================
double Gaudi::Math::Bernstein2D::operator () ( const double x ,
                                               const double y ) const
{
  /// the trivial cases
  if ( x < m_xmin || x > m_xmax ) { return 0.0        ; }
  if ( y < m_ymin || y > m_ymax ) { return 0.0        ; }
  //
  if      ( 0 == npars ()       ) { return 0.0        ; }
  else if ( 1 == npars ()       ) { return m_pars [0] ; }
  ///
  std::vector<double> fy ( m_ny + 1 , 0 ) ;
  for ( unsigned short i = 0 ; i <= m_ny ; ++i )
  { fy[i] = m_by[i] ( y )  ; }
  //
  std::vector<double> fx ( m_nx + 1 , 0 ) ;
  for  ( unsigned short i = 0 ; i <= m_nx ; ++i ) 
  { fx[i] = m_bx[i] ( x )  ; }
  //
  double       result = 0 ;
  for  ( unsigned short ix = 0 ; ix <= m_nx ; ++ix )
  { 
    for  ( unsigned short iy = 0 ; iy <= m_ny ; ++iy ) 
    { result += par ( ix , iy ) * fx[ix] * fy[iy] ; }
  }
  //
  const double scalex = ( m_nx + 1 ) / ( xmax() - xmin() ) ;
  const double scaley = ( m_ny + 1 ) / ( ymax() - ymin() ) ;
  //
  return result * ( scalex * scaley ) ;
}
// ============================================================================
/** get the integral over 2D-region 
 *  \f[  x_min < x < x_max, y_min< y< y_max\f] 
 */
// ============================================================================
double Gaudi::Math::Bernstein2D::integral() const 
{ return std::accumulate ( m_pars.begin() , m_pars.end() , 0.0 ) ; }
// ============================================================================
/* get the integral over 2D-region 
 *  \f[ \int_{x_low}^{x_high}\int_{y_low}^{y_high} \mathcal{B}(x,y) \mathrm{d}x\mathrm{d}y\f] 
 *  @param xlow  low  edge in x 
 *  @param xhigh high edge in x 
 *  @param ylow  low  edge in y 
 *  @param yhigh high edge in y 
 */
// ============================================================================
double Gaudi::Math::Bernstein2D::integral 
( const double xlow , const double xhigh , 
  const double ylow , const double yhigh ) const 
{
  if      ( s_equal ( xlow , xhigh ) || s_equal ( ylow  , yhigh ) ) { return 0 ; }
  //
  else if ( s_equal ( xlow  , m_xmin ) && 
            s_equal ( xhigh , m_xmax ) && 
            s_equal ( ylow  , m_ymin ) && 
            s_equal ( yhigh , m_ymax )  )  { return integral () ; }
  //
  else if ( xlow  > xhigh ) { return -1*integral ( xhigh , xlow  , ylow  , yhigh ) ; }
  else if ( ylow  > yhigh ) { return -1*integral ( xlow  , xhigh , yhigh , ylow  ) ; }
  //
  else if ( xhigh <  xmin () || xlow >  xmax() ) { return 0 ; }
  else if ( yhigh <  ymin () || ylow >  ymax() ) { return 0 ; }
  //
  const double  x_low  = std::max ( xmin() , xlow  ) ;
  const double  x_high = std::min ( xmax() , xhigh ) ;
  if ( x_low >= x_high ) { return 0 ; }
  //
  const double  y_low  = std::max ( ymin() , ylow  ) ;
  const double  y_high = std::min ( ymax() , yhigh ) ;
  if ( y_low >= y_high ) { return 0 ; }
  //
  std::vector<double> fy ( m_ny + 1 , 0 ) ;
  for ( unsigned short i = 0 ; i <= m_ny ; ++i ) 
  { fy[i] = m_by[i].integral ( y_low , y_high ) ; }
  //
  std::vector<double> fx ( m_nx + 1 , 0 ) ;
  for  ( unsigned short i = 0 ; i <= m_nx ; ++i ) 
  { fx[i] = m_bx[i].integral ( x_low , x_high ) ; }
  //
  double result = 0 ;
  for  ( unsigned short ix = 0 ; ix <= m_nx ; ++ix ) 
  {
    for  ( unsigned short iy = 0 ; iy <= m_ny ; ++iy ) 
    { result += par ( ix , iy ) * fx[ix] * fy[iy] ; }
    //
  }
  //
  const double scalex = ( m_nx + 1 ) / ( xmax() - xmin() ) ;
  const double scaley = ( m_ny + 1 ) / ( ymax() - ymin() ) ;
  //
  return result * ( scalex * scaley ) ;
}
// ============================================================================
/*  integral over x-dimension 
 *  \f[ \int_{y_low}^{y_high} \mathcal{B}(x,y) \mathrm{d}y\f] 
 *  @param x     variable 
 *  @param ylow  low  edge in y 
 *  @param yhigh high edge in y 
 */
// ============================================================================
double Gaudi::Math::Bernstein2D::integrateX 
( const double y    , 
  const double xlow , const double xhigh ) const 
{
  if      ( s_equal ( xlow , xhigh ) ) { return 0 ; }
  else if ( xlow  > xhigh  ) { return -1*integrateX ( y , xhigh , xlow ) ; }
  else if ( xhigh <= xmin () || xlow >= xmax() ) { return 0 ; }
  else if ( y     <  ymin () || y    >  ymax() ) { return 0 ; }
  else if ( s_equal ( xlow  , m_xmin ) && 
            s_equal ( xhigh , m_xmax )         ) { return integrateX ( y ) ; }
  //
  const double  x_low  = std::max ( xmin() , xlow  ) ;
  const double  x_high = std::min ( xmax() , xhigh ) ;
  if ( x_low >= x_high ) { return 0 ; }
  //
  std::vector<double> fy ( m_ny + 1 , 0 ) ;
  for ( unsigned short i = 0 ; i <= m_ny ; ++i ) 
  { fy[i] = m_by[i] ( y ) ; }
  //
  std::vector<double> fx ( m_nx + 1 , 0 ) ;
  for  ( unsigned short i = 0 ; i <= m_nx ; ++i ) 
  { fx[i] = m_bx[i].integral ( x_low , x_high ) ; }
  //
  double result = 0 ;
  for  ( unsigned short ix = 0 ; ix <= m_nx ; ++ix ) 
  {
    for  ( unsigned short iy = 0 ; iy <= m_ny ; ++iy ) 
    { result += par ( ix , iy ) * fx[ix] * fy[iy] ; }
    //
  }
  //
  const double scalex = ( m_nx + 1 ) / ( xmax() - xmin() ) ;
  const double scaley = ( m_ny + 1 ) / ( ymax() - ymin() ) ;
  //
  return result * ( scalex * scaley ) ;
}
// ============================================================================
/** integral over x-dimension 
 *  \f[ \int_{x_low}^{x_high} \mathcal{B}(x,y) \mathrm{d}x\f] 
 *  @param y     variable 
 *  @param xlow  low  edge in x 
 *  @param xhigh high edge in x 
 */
// ============================================================================
double Gaudi::Math::Bernstein2D::integrateY 
( const double x    , 
  const double ylow , const double yhigh ) const 
{
  if      ( s_equal ( ylow  , yhigh ) ) { return 0 ; }
  else if ( ylow  >  yhigh ) { return -1*integrateY ( x , yhigh , ylow  ) ; }
  else if ( x     <  xmin () || x    >  xmax() ) { return 0 ; }
  else if ( yhigh <= ymin () || ylow >= ymax() ) { return 0 ; }
  else if ( s_equal ( ylow  , m_ymin ) && 
            s_equal ( yhigh , m_ymax )         ) { return integrateY ( x ) ; }
  //
  const double  y_low  = std::max ( ymin() , ylow  ) ;
  const double  y_high = std::min ( ymax() , yhigh ) ;
  if ( y_low >= y_high ) { return 0 ; }
  //
  std::vector<double> fy ( m_ny + 1 , 0 ) ;
  for ( unsigned short i = 0 ; i <= m_ny ; ++i ) 
  { fy[i] = m_by[i].integral ( y_low , y_high ) ; }
  //
  std::vector<double> fx ( m_nx + 1 , 0 ) ;
  for  ( unsigned short i = 0 ; i <= m_nx ; ++i ) 
  { fx[i] = m_bx[i] ( x ) ; }
  //
  double result = 0 ;
  for  ( unsigned short ix = 0 ; ix <= m_nx ; ++ix ) 
  {
    for  ( unsigned short iy = 0 ; iy <= m_ny ; ++iy ) 
    { result += par ( ix , iy ) * fx[ix] * fy[iy] ; }
    //
  }
  //
  const double scalex = ( m_nx + 1 ) / ( xmax() - xmin() ) ;
  const double scaley = ( m_ny + 1 ) / ( ymax() - ymin() ) ;
  //
  return result * ( scalex * scaley ) ;
}
// ============================================================================
/*  integral over x-dimension 
 *  \f[ \int_{y_{min}}^{y_{max}} \mathcal{B}(x,y) \mathrm{d}y\f] 
 *  @param x     variable 
 */
// ============================================================================
double Gaudi::Math::Bernstein2D::integrateX ( const double y ) const 
{
  if ( y < ymin () || y > ymax() ) { return 0 ; }
  //
  std::vector<double> fy ( m_ny + 1 , 0 ) ;
  for ( unsigned short i = 0 ; i <= m_ny ; ++i ) 
  { fy[i] = m_by[i] ( y ) ; }
  //
  const std::vector<double> fx ( m_nx + 1 , 1 ) ;
  //
  double result = 0 ;
  for  ( unsigned short ix = 0 ; ix <= m_nx ; ++ix ) 
  {
    for  ( unsigned short iy = 0 ; iy <= m_ny ; ++iy ) 
    { result += par ( ix , iy ) * fx[ix] * fy[iy] ; }
    //
  }
  //
  // const double scalex = ( m_nx + 1 ) / ( xmax() - xmin() ) ;
  const double scaley = ( m_ny + 1 ) / ( ymax() - ymin() ) ;
  //
  return result * scaley  ;
}
// ============================================================================
/** integral over x-dimension 
 *  \f[ \int_{x_{min}}^{x_{max}} \mathcal{B}(x,y) \mathrm{d}x\f] 
 *  @param y     variable 
 */
// ============================================================================
double Gaudi::Math::Bernstein2D::integrateY 
( const double x ) const 
{
  if ( x < xmin () || x > xmax() ) { return 0 ; }
  //
  const std::vector<double> fy ( m_ny + 1 , 1.0 ) ;
  //
  std::vector<double> fx ( m_nx + 1 , 0 ) ;
  for  ( unsigned short i = 0 ; i <= m_nx ; ++i ) 
  { fx[i] = m_bx[i] ( x ) ; }
  //
  double result = 0 ;
  for  ( unsigned short ix = 0 ; ix <= m_nx ; ++ix ) 
  {
    for  ( unsigned short iy = 0 ; iy <= m_ny ; ++iy ) 
    { result += par ( ix , iy ) * fx[ix] * fy[iy] ; }
    //
  }
  //
  const double scalex = ( m_nx + 1 ) / ( xmax() - xmin() ) ;
  // const double scaley = ( m_ny + 1 ) / ( ymax() - ymin() ) ;
  //
  return result * scalex ; // * scaley ) ;
}
// ============================================================================
// set (l,m)-parameter
// ============================================================================
bool Gaudi::Math::Bernstein2D::setPar
( const unsigned short l     , 
  const unsigned short m     , 
  const double         value )
{
  if ( l > m_nx || m > m_ny )             { return false ; }
  const unsigned int k =  l * ( m_ny + 1 ) + m ;
  return setPar ( k , value ) ;
}
// ============================================================================
// set k-parameter
// ============================================================================
bool Gaudi::Math::Bernstein2D::setPar
( const unsigned int   k     , 
  const double         value )
{
  if ( k >= npars() )                     { return false ; }
  if ( s_equal ( m_pars [ k ] , value ) ) { return false ; }
  m_pars [ k ] = value ;
  return true ;
}
// ============================================================================
// get (l,m)-parameter 
// ============================================================================
double  Gaudi::Math::Bernstein2D::par 
( const unsigned short l ,
  const unsigned short m ) const 
{
  if ( l > m_nx || m > m_ny ) { return 0 ; }
  const unsigned int k =  l * ( m_ny + 1 ) + m ;
  return par ( k ) ;
}
// ============================================================================

  

// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Bernstein2DSym::Bernstein2DSym
( const unsigned short      n    ,
  const double              xmin ,
  const double              xmax )
  : std::binary_function<double,double,double> ()
//
  , m_n    ( n ) 
//
  , m_pars ( ( n + 1 ) * ( n + 2 ) / 2 , 0.0 )
//
  , m_xmin ( std::min ( xmin , xmax ) )
  , m_xmax ( std::max ( xmin , xmax ) )
//
  , m_b    () 
{
  //
  typedef  Gaudi::Math::Bernstein::Basic BB ;
  for ( unsigned short i = 0 ; i <= n ; ++i ) 
  { m_b.push_back ( Bernstein ( BB ( i , n ) , xmin , xmax ) ) ; }
  //
}
// ============================================================================
// get the value
// ============================================================================
double Gaudi::Math::Bernstein2DSym::operator () 
  ( const double x ,
    const double y ) const
{
  /// the trivial cases
  if ( x < xmin () || x > xmax () ) { return 0.0        ; }
  if ( y < ymin () || y > ymax () ) { return 0.0        ; }
  //
  if      ( 0 == npars ()       ) { return 0.0        ; }
  else if ( 1 == npars ()       ) { return m_pars [0] ; }
  ///
  std::vector<double> fy ( m_n + 1 , 0 ) ;
  for ( unsigned short i = 0 ; i <= m_n ; ++i ) 
  { fy[i] = m_b[i] ( y ) ; }
  //
  std::vector<double> fx ( m_n + 1 , 0 ) ;
  for  ( unsigned short i = 0 ; i <= m_n ; ++i ) 
  { fx[i] = m_b[i] ( x ) ; }
  //
  double       result = 0 ;
  for  ( unsigned short ix = 0 ; ix <= m_n ; ++ix ) 
  {
    for  ( unsigned short iy = 0 ; iy <= m_n ; ++iy ) 
    { 
      result += ( ix == iy ) ? 
        par ( ix , iy ) * fx[ix] * fy[iy]       : 
        par ( ix , iy ) * fx[ix] * fy[iy] * 0.5 ;
    }
  }
  //
  const double scale = ( m_n + 1 ) / ( xmax() - xmin() ) ;
  return result * ( scale * scale ) ;
}
// ============================================================================
/* get the integral over 2D-region 
 *  \f[ \int_{x_{low}}^{x_{high}}\int_{y_{low}}^{y_{high}} 
 *  \mathcal{B}(x,y) \mathrm{d}x\mathrm{d}y\f] 
 *  @param xlow  low  edge in x 
 *  @param xhigh high edge in x 
 *  @param ylow  low  edge in y 
 *  @param yhigh high edge in y 
 */
// ============================================================================
double Gaudi::Math::Bernstein2DSym::integral 
( const double xlow , const double xhigh ,
  const double ylow , const double yhigh ) const 
{
  //
  if      ( xlow  > xhigh   ) { return -integral ( xhigh , xlow    , ylow   , yhigh  ) ; }
  else if ( ylow  > yhigh   ) { return -integral ( xlow  , xhigh   , yhigh  , ylow   ) ; }
  //
  else if ( xlow  < xmin () ) { return  integral ( xmin() , xhigh  , ylow   , yhigh  ) ; }
  else if ( xhigh > xmax () ) { return  integral ( xlow   , xmax() , ylow   , yhigh  ) ; }
  else if ( ylow  < ymin () ) { return  integral ( xlow   , xhigh  , ymin() , yhigh  ) ; }
  else if ( yhigh > ymax () ) { return  integral ( xlow   , xhigh  , ylow   , ymax() ) ; }
  //
  else if ( s_equal ( xlow , xhigh ) || s_equal ( ylow , yhigh ) ) { return 0 ; }
  //
  //
  std::vector<double> fy ( m_n + 1 , 0 ) ;
  for ( unsigned short i = 0 ; i <= m_n ; ++i ) 
  { fy[i] = m_b[i].integral ( ylow , yhigh ) ; }
  //
  std::vector<double> fx ( m_n + 1 , 0 ) ;
  for  ( unsigned short i = 0 ; i <= m_n ; ++i ) 
  { fx[i] = m_b[i].integral ( xlow , xhigh ) ; }
  //
  double       result = 0 ;
  for  ( unsigned short ix = 0 ; ix <= m_n ; ++ix ) 
  {
    for  ( unsigned short iy = 0 ; iy <= m_n ; ++iy ) 
    { 
      result += ( ix == iy ) ? 
        par ( ix , iy ) * fx[ix] * fy[iy]       : 
        par ( ix , iy ) * fx[ix] * fy[iy] * 0.5 ;
    }
  }
  //
  const double scale = ( m_n + 1 ) / ( xmax() - xmin() ) ;
  return result * ( scale * scale ) ;
}
// ============================================================================
/*  integral over x-dimension 
 *  \f[ \int_{x_{low}}^{x_{high}} \mathcal{B}(x,y) \mathrm{d}x\f] 
 *  @param x     variable 
 *  @param xlow  low  edge in x 
 *  @param xhigh high edge in x 
 */
// ============================================================================
double Gaudi::Math::Bernstein2DSym::integrateX 
( const double y    ,  
  const double xlow , const double xhigh ) const 
{ return integrateY ( y , xlow , xhigh ) ; }
// ============================================================================
/*  integral over y-dimension 
 *  \f[ \int_{y_{low}}^{x_{high}} \mathcal{B}(x,y) \mathrm{d}y\f] 
 *  @param y     variable 
 *  @param xlow  low  edge in x 
 *  @param xhigh high edge in x 
 */
// ============================================================================
double Gaudi::Math::Bernstein2DSym::integrateY
( const double x    ,
  const double ylow , const double yhigh ) const 
{
  //
  if      ( s_equal ( ylow  , yhigh ) ) { return 0 ; }
  else if ( ylow  > yhigh ) { return -integrateY ( x , yhigh , ylow  ) ; }
  else if ( x     <  xmin () || x    >  xmax() ) { return 0 ; }
  else if ( yhigh <  ymin () || ylow >  ymax() ) { return 0 ; }
  else if ( s_equal ( ylow  , ymin () ) && 
            s_equal ( yhigh , ymax () )  ) { return integrateY ( x ) ; }
  //
  const double  y_low  = std::max ( ymin() , ylow  ) ;
  const double  y_high = std::min ( ymax() , yhigh ) ;
  if ( y_low >= y_high ) { return 0 ; }
  //
  std::vector<double> fy ( m_n + 1 , 0 ) ;
  for ( unsigned short i = 0 ; i <= m_n ; ++i ) 
  { fy[i] = m_b[i].integral ( y_low , y_high ) ; }
  //
  std::vector<double> fx ( m_n + 1 , 0 ) ;
  for  ( unsigned short i = 0 ; i <= m_n ; ++i ) 
  { fx[i] = m_b[i] ( x ) ; }
  //
  double       result = 0 ;
  for  ( unsigned short ix = 0 ; ix <= m_n ; ++ix ) 
  {
    for  ( unsigned short iy = 0 ; iy <= m_n ; ++iy ) 
    { 
      result += ( ix == iy ) ? 
        par ( ix , iy ) * fx[ix] * fy[iy]       : 
        par ( ix , iy ) * fx[ix] * fy[iy] * 0.5 ;
    }
  }
  //
  const double scale = ( m_n + 1 ) / ( xmax() - xmin() ) ;
  return result * ( scale * scale ) ;
}
// ============================================================================
/* get the integral over 2D-region 
 *  \f[ \int_{x_{min}}^{x_{max}}\int_{y_{min}}^{y_{max}} 
 *  \mathcal{B}(x,y) \mathrm{d}x\mathrm{d}y\f] 
 */
// ============================================================================
double  Gaudi::Math::Bernstein2DSym::integral   () const 
{ return std::accumulate ( m_pars.begin() , m_pars.end() , 0.0 ) ; }
// ============================================================================
/* integral over x-dimension 
 *  \f[ \int_{x_{min}}^{x_{max}} \mathcal{B}(x,y) \mathrm{d}x\f] 
 *  @param x     variable 
 */
// ============================================================================
double Gaudi::Math::Bernstein2DSym::integrateX ( const double y ) const 
{ return integrateY ( y ) ; }
// ============================================================================
/*  integral over y-dimension 
 *  \f[ \int_{y_{min}}^{x_{max}} \mathcal{B}(x,y) \mathrm{d}y\f] 
 *  @param y     variable 
 */
// ============================================================================
double Gaudi::Math::Bernstein2DSym::integrateY ( const double x ) const 
{
  //
  if ( x     <  xmin () || x    >  xmax() ) { return 0 ; }
  //
  std::vector<double> fx ( m_n + 1 , 0 ) ;
  for  ( unsigned short i = 0 ; i <= m_n ; ++i ) { fx[i] = m_b[i] ( x ) ; }
  //
  double       result = 0 ;
  for  ( unsigned short ix = 0 ; ix <= m_n ; ++ix ) 
  {
    for  ( unsigned short iy = 0 ; iy <= m_n ; ++iy ) 
    { 
      result += ( ix == iy ) ? 
        par ( ix , iy ) * fx[ix]       : 
        par ( ix , iy ) * fx[ix] * 0.5 ;
    }
  }
  //
  const double scale = ( m_n + 1 ) / ( xmax() - xmin() ) ;
  return result * scale  ;
}
// ============================================================================
// set (k)-parameter
// ============================================================================
bool Gaudi::Math::Bernstein2DSym::setPar
( const unsigned int   k     , 
  const double         value )
{
  //
  if ( k >= npars() )                     { return false ; }
  if ( s_equal ( m_pars [ k ] , value ) ) { return false ; }
  m_pars [ k ] = value ;
  //
  return true ;
}
// ============================================================================
// set (l,m)-parameter
// ============================================================================
bool Gaudi::Math::Bernstein2DSym::setPar
( const unsigned short l     , 
  const unsigned short m     , 
  const double         value )
{
  //
  if ( l > m_n || m > m_n )               { return false ; }
  //
  const unsigned int k = ( l < m ) ? 
    ( m * ( m + 1 ) / 2 + l ) : 
    ( l * ( l + 1 ) / 2 + m ) ;
  //
  return setPar ( k , value ) ;
}
// ============================================================================
// get (l,m)-parameter 
// ============================================================================
double Gaudi::Math::Bernstein2DSym::par
( const unsigned short l ,
  const unsigned short m ) const 
{
  //
  if ( l > m_n || m > m_n )               { return 0 ; }
  //
  const unsigned int k = ( l < m ) ? 
    ( m * ( m + 1 ) / 2 + l ) : 
    ( l * ( l + 1 ) / 2 + m ) ;
  //
  return par ( k ) ;
}
// ============================================================================
// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Positive2D::Positive2D
( const unsigned short      nX   ,
  const unsigned short      nY   ,
  const double              xmin ,
  const double              xmax ,
  const double              ymin ,
  const double              ymax )
  : std::binary_function<double,double,double> ()
//
  , m_bernstein (   nX , nY , xmin , xmax , ymin , ymax ) 
  , m_sphere    ( ( nX + 1 ) * ( nY + 1 ) - 1 )
{
  updateBernstein () ;
}
// ============================================================================
// set k-parameter
// ============================================================================
bool Gaudi::Math::Positive2D::setPar 
( const unsigned int k     , 
  const double       value )
{
  //
  const bool update = m_sphere.setPhase ( k , value ) ;
  if ( !update ) { return false ; }   // no actual change 
  //
  return updateBernstein () ;
}
// =============================================================================
// update bernstein coefficients
// =============================================================================
bool Gaudi::Math::Positive2D::updateBernstein ()
{
  //
  bool update = false ;
  for ( unsigned int ix = 0 ; ix < m_sphere.nX() ; ++ix ) 
  { 
    const bool updated = m_bernstein.setPar ( ix , m_sphere.x2 ( ix ) ) ;
    update = updated || update ;  
  }
  //
  return update ;
}
// ============================================================================
// get the parameter value
// ============================================================================
double Gaudi::Math::Positive2D::par ( const unsigned int k ) const 
{ return m_sphere.phase ( k ) ; }
// ============================================================================
/*  get the integral over 2D-region           
 *  \f[ \int_{x_{min}}^{x_{max}}\int_{y_{min}}^{y_{max}} 
 *        \mathcal{B}(x,y) \mathrm{d}x\mathrm{d}y\f] 
 */
// ============================================================================
double  Gaudi::Math::Positive2D::integral   () const { return 1 ; }
// ============================================================================
/* get the integral over 2D-region 
 *  \f[ \int_{x_low}^{x_high}\int_{y_low}^{y_high} \mathcal{B}(x,y) \mathrm{d}x\mathrm{d}y\f] 
 *  @param xlow  low  edge in x 
 *  @param xhigh high edge in x 
 *  @param ylow  low  edge in y 
 *  @param yhigh high edge in y 
 */
// ============================================================================
double Gaudi::Math::Positive2D::integral   
( const double xlow , const double xhigh , 
  const double ylow , const double yhigh ) const 
{ 
  return
    s_equal ( xlow  , xmin() ) && 
    s_equal ( xhigh , xmax() ) && 
    s_equal ( ylow  , ymin() ) && 
    s_equal ( yhigh , ymax() )  ?  1.0 :
    m_bernstein.integral ( xlow , xhigh , ylow , yhigh ) ; 
}
// ============================================================================




// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Positive2DSym::Positive2DSym
( const unsigned short      N    ,
  const double              xmin ,
  const double              xmax )
  : std::binary_function<double,double,double> ()
//
  , m_bernstein (   N , xmin , xmax ) 
  , m_sphere    ( ( N + 1 ) * ( N + 2 ) / 2 - 1  )
{
  updateBernstein () ;
}
// ============================================================================
// set k-parameter
// ============================================================================
bool Gaudi::Math::Positive2DSym::setPar 
( const unsigned int k     , 
  const double       value )
{
  //
  const bool update = m_sphere.setPhase ( k , value ) ;
  if ( !update ) { return false ; }   // no actual change 
  //
  return updateBernstein () ;
}
// =============================================================================
// update bernstein coefficients
// =============================================================================
bool Gaudi::Math::Positive2DSym::updateBernstein ()
{
  //
  //
  bool update = false ;
  for ( unsigned int ix = 0 ; ix < m_sphere.nX() ; ++ix ) 
  { 
    const bool updated = m_bernstein.setPar ( ix , m_sphere.x2 ( ix ) ) ; 
    update = updated || update ; 
  }
  //
  return update ;
}
// ============================================================================
// get the parameter value
// ============================================================================
double Gaudi::Math::Positive2DSym::par ( const unsigned int  k ) const 
{ return m_sphere.phase ( k ) ; }
// ============================================================================
/*  get the integral over 2D-region 
 *  \f[ \int_{x_{min}}^{x_{max}}\int_{y_{min}}^{y_{max}} 
 *   \mathcal{B}(x,y) \mathrm{d}x\mathrm{d}y \f] 
 */
// ============================================================================
double  Gaudi::Math::Positive2DSym::integral   () const { return 1 ; }
// ============================================================================
/*  get the integral over 2D-region 
 *  \f[ \int_{x_low}^{x_high}\int_{y_low}^{y_high} 
 *   \mathcal{B}(x,y) \mathrm{d}x\mathrm{d}y \f] 
 *  @param xlow  low  edge in x 
 *  @param xhigh high edge in x 
 *  @param ylow  low  edge in y 
 *  @param yhigh high edge in y 
 */
// ============================================================================
double Gaudi::Math::Positive2DSym::integral
( const double xlow , const double xhigh , 
  const double ylow , const double yhigh ) const
{
  return 
    s_equal ( xlow  , xmin () ) &&
    s_equal ( xhigh , xmax () ) &&
    s_equal ( ylow  , ymin () ) &&
    s_equal ( yhigh , ymax () ) ?  1.0 :
    m_bernstein.integral ( xlow , xhigh , ylow , yhigh ) ; 
}
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
