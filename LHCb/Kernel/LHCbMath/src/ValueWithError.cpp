// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <cmath>
#include <string>
#include <sstream>
#include <climits>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GAUDI_VERSION.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Lomont.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/ToStream.h"
// ============================================================================
// local
// ============================================================================
#include "LHCbMath/ValueWithError.h"
#include "LHCbMath/Power.h"
#include "LHCbMath/LHCbMath.h"
#include "LHCbMath/Clenshaw.h"
#include "LHCbMath/Combine.h"
#include "LHCbMath/MoreFunctions.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/format.hpp"
// ============================================================================
// GSL 
// ============================================================================
#include "gsl/gsl_sf_psi.h"
// ============================================================================
/** @file
 *  Implementation file for class Gaudi::Math::ValueWithError
 *  @date 2009-06-03
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
#ifdef __INTEL_COMPILER                                   // Disable ICC remark
//            floating-point equality and inequality comparisons are unreliable
#pragma warning(disable:1572) 
#endif
// ============================================================================
// local namespace to hide the details
// ============================================================================
namespace
{
  // ==========================================================================
  const unsigned int _maxULPs = 10000 ;
  // ==========================================================================
  inline bool _equal ( const double value1 ,
                       const double value2 )
  { return value1 == value2 ||
      Gaudi::Math::lomont_compare_double ( value1 ,value2 , _maxULPs ) ; }
  // ==========================================================================
  // check if the double value close to zero
  inline bool _zero  ( const double value ) { return _equal ( value , 0 ) ; }
  // check if the double value close to zero
  inline bool _zero  ( const Gaudi::Math::ValueWithError& a ) 
  { return _zero ( a.value() ) && _zero ( a.cov2() ) ; }  
  // ==========================================================================
  // check if the double value close to one
  inline bool _one   ( const double value ) { return _equal ( value , 1 ) ; }
  // check if the double value close to one
  inline bool _one  ( const Gaudi::Math::ValueWithError& a ) 
  { return _one ( a.value() ) && _zero ( a.cov2() ) ; }  
  // ==========================================================================
  /// helper wrapper
  inline double _pow ( const double v , const int n )
  {
    return
      0 <= n ?
      Gaudi::Math::pow ( v , (unsigned long) n ) :
      std::pow         ( v , n                 ) ;
  }
  // ==========================================================================
  inline bool _is_long ( const double value ) 
  { return LHCb::Math::islong ( value ) ; }
  // ==========================================================================
}
// ============================================================================
// constructor from the value and covariance
// ============================================================================
Gaudi::Math::ValueWithError::ValueWithError
( const double value      ,
  const double covariance )
  : m_value ( value      )
  , m_cov2  ( covariance )
{
  if ( _zero ( m_cov2 ) ) { m_cov2 = 0 ; }
}
// ============================================================================
// constructor from the (value,error)-pair
// ============================================================================
Gaudi::Math::ValueWithError::ValueWithError
( const std::pair<double,double>& value )
  : m_value ( value.first )
  , m_cov2  ( 0 )
{
  setError ( value.second ) ;
}
// ============================================================================
// constructor from textual representation
// ============================================================================
Gaudi::Math::ValueWithError::ValueWithError
( const std::string& value )
  : m_value ( 0 )
  , m_cov2  ( 0 )
{
  StatusCode sc = Gaudi::Parsers::parse ( *this , value ) ;
  if ( sc.isFailure() )
  { throw GaudiException ( "Unable to parse ValueWithError: '" + value + "'" ,
                           "Gaudi::Math" , sc ) ; }
}
// ============================================================================
// set the error
// ============================================================================
void Gaudi::Math::ValueWithError::setError ( const double e )
{
  if    ( _zero ( e ) ) { m_cov2 = 0 ; }
  else 
  {
    m_cov2  = e * e ;
    //
    if ( 0 > e ) { m_cov2 = -m_cov2 ; }
    //
    if ( _zero ( m_cov2 ) ) { m_cov2 = 0 ; }
  }
}
// ============================================================================
// get the error
// ============================================================================
double Gaudi::Math::ValueWithError::error      () const
{ 
  return 
    _zero ( m_cov2 ) ? 0.0 : 
    0 <=    m_cov2   ? std::sqrt ( m_cov2 ) : -std::sqrt ( -m_cov2 ) ; 
}
// ============================================================================
// +=
// ============================================================================
Gaudi::Math::ValueWithError&
Gaudi::Math::ValueWithError::operator+=
( const Gaudi::Math::ValueWithError& right )                             // +=
{
  //
  if ( &right == this ) 
  {
    m_value *= 2  ;
    m_cov2  *= 4  ;
    return  *this ;
  }
  //
  m_value += right.m_value ;
  m_cov2  += right.m_cov2  ;
  //
  if ( _zero ( m_cov2 ) ) { m_cov2 = 0 ; }
  //
  return *this ;
}
// ============================================================================
// -=
// ============================================================================
Gaudi::Math::ValueWithError&
Gaudi::Math::ValueWithError::operator-=
( const Gaudi::Math::ValueWithError& right )                              // -=
{
  //
  if ( &right == this ) 
  {
    m_value = 0   ;
    m_cov2  = 0   ;
    return  *this ;
  }
  //
  m_value -= right.m_value ;
  m_cov2  += right.m_cov2  ;  
  //
  if ( _zero ( m_cov2 ) ) { m_cov2 = 0 ; }
  //
  return *this ;
}
// ============================================================================
// *=
// ============================================================================
Gaudi::Math::ValueWithError&
Gaudi::Math::ValueWithError::operator*=
( const Gaudi::Math::ValueWithError& right )                              // *=
{
  if ( &right == this ) 
  {
    const double a = value() ;
    m_value  =     a * a ;
    m_cov2  *= 4 * a * a ;
    //
    if ( _zero ( m_cov2 ) ) { m_cov2 = 0 ; }
    //
    return  *this ;
  }
  //
  const double _a2 =       m_value *       m_value ;
  const double _b2 = right.m_value * right.m_value ;
  m_cov2  *= _b2                 ;
  m_cov2  += _a2 * right.m_cov2  ;
  m_value *=      right.m_value ;
  //
  if ( _zero ( m_cov2 ) ) { m_cov2 = 0 ; }
  //
  return *this ;
}
// ============================================================================
// /=
// ============================================================================
Gaudi::Math::ValueWithError&
Gaudi::Math::ValueWithError::operator/=
( const Gaudi::Math::ValueWithError& right )                              // /=
{
  if ( &right == this ) 
  {
    m_value  =  1 ;
    m_cov2   =  0 ;
    return  *this ;
  }
  //
  const double _a2 =       m_value *       m_value ;
  const double _b2 = right.m_value * right.m_value ;
  const double _b4 = _b2 * _b2 ;
  //
  m_cov2  /= _b2 ;
  m_cov2  += ( _a2 / _b4 ) * right.m_cov2 ;
  m_value /= right.m_value ;
  //
  if ( _zero ( m_cov2 ) ) { m_cov2 = 0 ; }
  //
  return *this ;
}
// ============================================================================
// *=
// ============================================================================
Gaudi::Math::ValueWithError&
Gaudi::Math::ValueWithError::operator*= ( const double v )                // *=
{
  m_value *= v     ;
  m_cov2  *= (v*v) ;
  //
  if ( _zero ( m_cov2 ) ) { m_cov2 = 0 ; }
  //
  return *this ;
}
// ============================================================================
// /=
// ============================================================================
Gaudi::Math::ValueWithError&
Gaudi::Math::ValueWithError::operator/= ( const double v )                // /=
{
  m_value /= v     ;
  m_cov2  /= (v*v) ;
  //
  if ( _zero ( m_cov2 ) ) { m_cov2 = 0 ; }
  //
  return *this ;
}
// ============================================================================
// +=
// ============================================================================
Gaudi::Math::ValueWithError&
Gaudi::Math::ValueWithError::operator+=( const double right )             // +=
{
  m_value += right ;
  return *this ;
}
// ============================================================================
// -=
// ============================================================================
Gaudi::Math::ValueWithError&
Gaudi::Math::ValueWithError::operator-= ( const double right )            // -=
{
  m_value -= right ;
  return *this ;
}
// ============================================================================
// unary-
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::operator-() const                        // unary-
{ return ValueWithError( -value() , cov2() ) ; }
// ============================================================================
// printout
// ============================================================================
std::ostream&
Gaudi::Math::ValueWithError::fillStream ( std::ostream& s ) const
{ return s << "( " << m_value << " +- " << error() << " )" ; }
// ============================================================================
// printout using format
// ============================================================================
std::ostream&
Gaudi::Math::ValueWithError::fillStream
( std::ostream&      s   ,
  const std::string& fmt ) const
{
  using namespace boost::io ;
  //
  boost::format printer ( fmt) ;
  printer.exceptions ( all_error_bits ^ ( too_many_args_bit | too_few_args_bit ) ) ;
  printer % value () % error () ;
  //
  return s << printer ;
}
// ============================================================================
// conversion to string
// ============================================================================
std::string Gaudi::Math::ValueWithError::toString   () const
{
  std::ostringstream s ;
  fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================
// conversion to the string using format
// ============================================================================
std::string Gaudi::Math::ValueWithError::toString
( const std::string& fmt ) const
{
  std::ostringstream s ;
  fillStream ( s , fmt ) ;
  return s.str() ;
}
// ============================================================================
// evaluate the mean of a and b
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::mean
( const Gaudi::Math::ValueWithError& b ) const
{
  //
  if ( &b == this ) { return *this ; } // self-mean ???
  //
  if      ( 0 >=   cov2 () && 0 >= b.cov2 () ) 
  { return 0.5 * ( value() + b.value() ) ; }
  else if ( 0 >=   cov2 ()                   ) { return *this ; }
  else if ( 0 >= b.cov2 ()                   ) { return b     ; }
  //
  double _cov2 = 1.0/( 1.0/cov2() + 1.0/b.cov2() ) ;
  if ( _zero ( _cov2 ) ) { _cov2 = 0 ; }
  //
  return Gaudi::Math::ValueWithError
    ( _cov2 * ( value()/cov2() + b.value()/b.cov2() ) ,  _cov2 ) ;
}
// =============================================================================
// evaluate chi2
// =============================================================================
double Gaudi::Math::ValueWithError::chi2
( const Gaudi::Math::ValueWithError& b ) const
{
  //
  if ( _equal ( value () , b.value() ) ) { return 0 ; } // RETURN
  //
  const double s_cov2 = cov2() + b.cov2() ;
  if ( 0 >= s_cov2 )                     { return -1 ; } // RETURN
  //
  const double diff = value() - b.value() ;
  return diff*diff/s_cov2 ;
}
// =============================================================================
// evaluate chi2
// =============================================================================
double Gaudi::Math::ValueWithError::chi2 ( const double b ) const
{
  //
  if ( _equal ( value() , b ) ) { return  0 ; } // RETURN
  //
  if ( 0 >= cov2 ()           ) { return -1 ; } // RETURN
  const double diff = value() - b ;
  return diff*diff/cov2() ;
}
// =============================================================================
/*  get Kullback-Liebler divergency 
 *  return the divergency for valid arguments, -1 otherwise
 */
// =============================================================================
double Gaudi::Math::ValueWithError::kullback
( const Gaudi::Math::ValueWithError& b ) const
{
  //
  if ( 0 >= cov2() || 0 >= b.cov2 () ) { return -1 ; }
  //
  const double c1 =   cov2 () ;
  const double c2 = b.cov2 () ;
  //
  return ( c1 - c2 ) * ( 1.0 / c2 - 1.0 / c1 ) + chi2 ( b ) ;  
}
// =============================================================================
// evaluate residual: signed sqrt(chi2)
// =============================================================================
double Gaudi::Math::ValueWithError::residual
( const Gaudi::Math::ValueWithError& b ) const
{
  //
  if ( _equal ( value () , b.value() ) ) { return     0 ; } // RETURN
  //
  const double s_cov2 = cov2() + b.cov2() ;
  if ( 0 >= s_cov2 )                     { return -1000 ; } // RETURN
  //
  const double diff = value() - b.value() ;
  //
  return diff / std::sqrt ( s_cov2 ) ;
}
// =============================================================================
// evaluate residual: signed sqrt(chi2)
// =============================================================================
double Gaudi::Math::ValueWithError::residual
( const double b ) const
{
  //
  if ( _equal ( value() , b ) ) { return     0 ; } // RETURN
  //
  if ( 0 >= cov2 () )           { return -1000 ; } // RETURN
  //
  const double diff = value() - b ;
  //
  return diff / error () ;
}
// ============================================================================
/*  evaluate the "fraction" \f$  \frac{a}{a+b} \f$
 *  @param  b the parameter "b" for the fraction
 *  @return a/(a+b)
 */
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::frac
( const Gaudi::Math::ValueWithError& b ) const
{
  //
  if ( &b == this ) { return fraction ( *this , b , 1 ) ; }
  //
  const double r  = value() / ( value() + b.value() ) ;
  //
  const double s  = value() + b.value() ;
  const double s2 = s  * s  ;
  const double s4 = s2 * s2 ;
  const double c2 =
    std::fabs (   cov2 () ) * b.value () * b.value () +
    std::fabs ( b.cov2 () ) *   value () *   value () ;
  //
  return ValueWithError
    ( r , 0 <= cov2() && 0 <= b.cov2() ? c2/s4 : -1.0 * c2 / s4 ) ;
  //
}
// ============================================================================
/*  evaluate the "fraction" \f$  \frac{a}{a+b} \f$
 *  @param  b the parameter "b" for the fraction
 *  @return a/(a+b)
 */
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::frac ( const double  b ) const
{ return frac ( ValueWithError ( b ) ) ; }
// ============================================================================
/*  evaluate the "asymmetry" \f$  \frac{a-b}{a+b} \f$
 *  @param  b the parameter "b" for the fraction
 *  @return (a-b)/(a+b)
 */
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::asym
( const Gaudi::Math::ValueWithError& b ) const
{
  //
  if ( &b == this ) { return asymmetry ( *this , b , 1 ) ; }
  //
  const double r  = ( value() - b.value() ) / ( value() + b.value() ) ;
  //
  const double s  = value() + b.value() ;
  const double s2 = s  * s  ;
  const double s4 = s2 * s2 ;
  //
  const double c2 =
    4 * std::fabs (   cov2 () ) * b.value () * b.value () +
    4 * std::fabs ( b.cov2 () ) *   value () *   value () ;
  //
  return ValueWithError
    ( r , 0 <= cov2() && 0 <= b.cov2() ? c2/s4 : -1.0 * c2 / s4 ) ;
  //
}
// ============================================================================
/*  evaluate the "asymmetry" \f$  \frac{a-b}{a+b} \f$
 *  @param  b the parameter "b" for the fraction
 *  @return (a-b)/(a+b)
 */
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::asym ( const double  b ) const
{ return asym ( ValueWithError ( b ) ) ; }
// =============================================================================
// check for NaN
// =============================================================================
bool Gaudi::Math::ValueWithError::isnan    () const
{
  return std::isnan    ( m_value ) || std::isnan    ( m_cov2  )  ;
}
// =============================================================================
// check for finiteness
// =============================================================================
bool Gaudi::Math::ValueWithError::isfinite () const
{
  return std::isfinite ( m_value ) && std::isfinite ( m_cov2  )  ;
}
// =============================================================================
// check for finiteness
// =============================================================================
bool Gaudi::Math::ValueWithError::isnormal () const
{
  return std::isnormal ( m_value ) && std::isfinite ( m_cov2  )  ;
}
// =============================================================================
// check for finiteness
// =============================================================================
bool Gaudi::Math::ValueWithError::isinf () const
{
  return std::isinf ( m_value ) || std::isinf ( m_cov2  )  ;
}
// ============================================================================
// check for goodness: finite values and non-negative covariance 
// ============================================================================
bool Gaudi::Math::ValueWithError::isgood   () const 
{ return isfinite () &&  ( 0 <= m_cov2 || _zero ( m_cov2 ) ) ; }
// =============================================================================
// for easy pythonization
// =============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__add__
( const Gaudi::Math::ValueWithError& right ) const
{
  //
  if ( &right == this ) { return right * 2.0 ; }
  //
  ValueWithError tmp ( *this ) ;
  return tmp += right ;
}
// =============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__sub__
( const Gaudi::Math::ValueWithError& right ) const
{
  //
  if ( &right == this ) { return  ValueWithError(0,0) ; }
  //
  ValueWithError tmp ( *this ) ;
  return tmp -= right ;
}
// =============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__mul__
( const Gaudi::Math::ValueWithError& right ) const
{
  //
  if ( &right == this ) { return  pow ( *this , 2 ) ; }
  //
  ValueWithError tmp ( *this ) ;
  return tmp *= right ;
}
// =============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__div__
( const Gaudi::Math::ValueWithError& right ) const
{
  //
  if ( &right == this ) { return  ValueWithError ( 1 , 0 ) ; }
  //
  ValueWithError tmp ( *this ) ;
  return tmp /= right ;
}
// =============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__add__ ( const double right ) const
{
  ValueWithError tmp ( *this ) ;
  return tmp += right ;
}
// =============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__sub__ ( const double right ) const
{
  ValueWithError tmp ( *this ) ;
  return tmp -= right ;
}
// =============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__mul__ ( const double right ) const
{
  ValueWithError tmp ( *this ) ;
  return tmp *= right ;
}
// =============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__div__ ( const double right ) const
{
  ValueWithError tmp ( *this ) ;
  return tmp /= right ;
}
// =============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__rsub__ ( const double right ) const
{ return ValueWithError ( right - value() , cov2()  ) ; }
// =============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__rdiv__ ( const double right ) const
{  
  ValueWithError tmp ( right ) ;
  return tmp /= (*this) ;
}
// ============================================================================
// abs(a)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__abs__ () const
{ return Gaudi::Math::abs ( *this ) ; }
// ============================================================================
// me**e
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__pow__  ( const int             e ) const
{ return pow ( *this , e ) ; }
// ============================================================================
// me**e
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__pow__  ( const double          e ) const
{ return pow ( *this , e ) ; }
// ============================================================================
// me**e
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__pow__
( const Gaudi::Math::ValueWithError&  e ) const
{ return pow ( *this , e ) ; }
// ============================================================================
// e**me
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__rpow__  ( const int             e ) const
{ return pow ( e , *this ) ; }
// ============================================================================
// e**me
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__rpow__ ( const double          e ) const
{ return pow ( e , *this ) ; }
// ============================================================================
// -me
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__neg__() const
{ return Gaudi::Math::ValueWithError ( -value() , cov2() ) ; }
// ============================================================================
// +me (no-effect)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__pos__() const { return *this ; }
// ============================================================================
// exp(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__exp__   () const { return exp   ( *this ) ; }
// ============================================================================
// expm1(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__expm1__ () const { return expm1 ( *this ) ; }
// ============================================================================
// log(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__log__   () const { return log   ( *this ) ; }
// ============================================================================
// log10(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__log10__ () const { return log10 ( *this ) ; }
// ============================================================================
// log1p(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__log1p__ () const { return log1p ( *this ) ; }
// ============================================================================
// sqrt(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__sqrt__  () const { return sqrt ( *this ) ; }
// ============================================================================
// cbrt(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__cbrt__  () const { return cbrt ( *this ) ; }
// ============================================================================
// sin(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__sin__   () const { return sin  ( *this ) ; }
// ============================================================================
// cos(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__cos__   () const { return cos  ( *this ) ; }
// ============================================================================
// tan(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__tan__   () const { return tan  ( *this ) ; }
// ============================================================================
// sinh(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__sinh__  () const { return sinh ( *this ) ; }
// ============================================================================
// cosh(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__cosh__  () const { return cosh ( *this ) ; }
// ============================================================================
// tanh(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__tanh__  () const { return tanh ( *this ) ; }
// ============================================================================
// erf(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__erf__   () const { return erf   ( *this ) ; }
// ============================================================================
// erfc(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__erfc__  () const { return erfc  ( *this ) ; }
// ============================================================================
// asin(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__asin__  () const { return asin  ( *this ) ; }
// ============================================================================
// acos(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__acos__  () const { return acos  ( *this ) ; }
// ============================================================================
// atan(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__atan__  () const { return atan  ( *this ) ; }
// ============================================================================
// asinh(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__asinh__ () const { return asinh ( *this ) ; }
// ============================================================================
// acosh(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__acosh__ () const { return acosh ( *this ) ; }
// ============================================================================
// atanh(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__atanh__ () const { return atanh ( *this ) ; }
// ============================================================================
// tgamma(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__tgamma__ () const { return tgamma ( *this ) ; }
// ============================================================================
// lgamma(me)
// ============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__lgamma__ () const { return lgamma ( *this ) ; }
// ============================================================================


// ============================================================================
/* Does this object represent natual number?
 *  - non-negative integer value 
 *  - cov2 == value  or cov2 == 0 
 */
// =============================================================================
bool Gaudi::Math::natural_number
( const Gaudi::Math::ValueWithError& v ) 
{
  return 
    0 <= v.value() && 0<= v.cov2() 
    && _is_long ( v.value() ) 
    && ( _zero ( v.cov2 () ) || _equal ( v.value() , v.cov2() ) ) ;
}
// ============================================================================
/** Does this object represent natual entry in histogram
 *  - non-negative integer value 
 *  - cov2 == value  or ( 0 == value && 1 == cov2 )
 */
// =============================================================================
bool Gaudi::Math::natural_entry 
( const Gaudi::Math::ValueWithError& v ) 
{
  return 
    0 <= v.value() && 0<= v.cov2() 
    && _is_long ( v.value() ) 
    && ( _equal ( v.value() , v.cov2() ) ||
         ( _zero ( v.value() ) && _equal ( 1 , v.cov2() ) ) ) ;
}
// ============================================================================
/*  evaluate the mean of a and b 
 *  taking into account correlation coefficient <code>rho</code>
 *  @param a (INPUT) the first argument 
 *  @param b (INPUT) the second argument 
 *  @param rho (INPUT) correlation coefficient \f$-1\le\rhi\le 1\f$
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::mean
( const Gaudi::Math::ValueWithError& a   , 
  const Gaudi::Math::ValueWithError& b   , 
  const double                       rho ) { return combine ( a , b , rho ) ; }
// ============================================================================
/*  evaluate abs(a)
 *  @param a (INPUT) the value
 *  @return the absolute value
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::abs
( const Gaudi::Math::ValueWithError& a )
{ return ValueWithError ( std::fabs ( a.value() ) , a.cov2() ) ; }
// ============================================================================
/* evaluate the binomial efficiency for Bernulli scheme with
 *  @param n (INPUT) number of 'success'
 *  @param N (INPUT) total number
 *  @return the binomial efficiency
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::binomEff
( const size_t n ,
  const size_t N )
{
  if       ( n >  N ) { return binomEff       ( N , n ) ; }
  else if  ( 0 == N ) { return ValueWithError ( 1 , 1 ) ; }
  //
  const long n1 = 0 == n ? 1 :     n ;
  const long n2 = n == N ? 1 : N - n ;
  //
  const double eff = double ( n       ) / N         ;
  const double c2  = double ( n1 * n2 ) / N / N / N ;
  //
  return Gaudi::Math::ValueWithError  ( eff , c2 ) ;
}
// ============================================================================
/*  evaluate the binomial efficiency interval using Wilson's prescription
 *  @param n (INPUT) number of 'success'
 *  @param N (INPUT) total number
 *  @return the binomial efficiency
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::wilsonEff
( const size_t n ,
  const size_t N )
{
  //
  if      ( n >  N ) { return wilsonEff      ( N , n ) ; }
  else if ( 0 == N ) { return ValueWithError ( 1 , 1 ) ; }
  //
  const long n1       = 0 == n ? 1 :     n ;
  const long n2       = n == N ? 1 : N - n ;
  //
  const double p      = double ( n1 ) / N ;
  const double q      = double ( n2 ) / N ;
  //
  const double kappa  =             1 ; // "1*sigma"
  const double kappa2 = kappa * kappa ;
  //
  const double nK     = N + kappa2 ;
  const double eff    = ( n + 0.5 * kappa2 ) / nK ;
  //
  const double prefix = kappa2 * N / ( nK * nK ) ;
  const double c2     = prefix * ( q * p + 0.25 * kappa2 / N ) ;
  //
  return Gaudi::Math::ValueWithError  ( eff , c2 ) ;
}
// ============================================================================
/*  evaluate the binomial efficiency interval using Agresti-Coull's prescription
 *  @param n (INPUT) number of 'success'
 *  @param N (INPUT) total number
 *  @return the binomial efficiency
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::agrestiCoullEff
( const size_t n ,
  const size_t N )
{
  //
  if      ( n >  N ) { return wilsonEff      ( N , n ) ; }
  else if ( 0 == N ) { return ValueWithError ( 1 , 1 ) ; }
  //
  const double kappa  =             1 ; // "1*sigma"
  const double kappa2 = kappa * kappa ;
  //
  const double n1 = n + 0.5 * kappa2 ;
  const double n2 = N +       kappa2 ;
  //
  const double p  = n1/n2 ;
  const double q  = 1 - p ;
  //
  const double eff = p ;
  const double c2  = kappa2 * p * q / n2 ;
  //
  return Gaudi::Math::ValueWithError  ( eff , c2 ) ;
}
// ============================================================================
/*  Simple evaluation of efficiency from statistically independend
 *  "exclusive" samples "accepted" and "rejected"
 *  \f$ \varepsilon = \frac{1}{ 1 + \frac{N_{rejected}}{N_accepted}}\f$ 
 *  @param accepted  (IN) accepted sample 
 *  @param rejected  (IN) rejected sample 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::exclusiveEff
( const Gaudi::Math::ValueWithError& accepted , 
  const Gaudi::Math::ValueWithError& rejected )
{ return Gaudi::Math::binomEff2 ( accepted , rejected ) ; }
// ============================================================================
/*  evaluate the binomial efficiency for Bernulli scheme with weights 
 *  @param nAccepted (INPUT) number of accepted (weighted) events 
 *  @param nRejected (INPUT) number of rejected (weighted) events 
 *  @return the binomial efficiency 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::binomEff2
( const ValueWithError& nAccepted , 
  const ValueWithError& nRejected ) 
{
  const double vA = nAccepted.value() ;
  const double vR = nRejected.value() ;
  //
  const bool zeroA = _zero ( vA      ) ;
  const bool zeroR = _zero ( vR      ) ;
  //
  if ( zeroA && zeroR ) { return ValueWithError ( 1 , -1 ) ; }
  //
  const double vB  = vA + vR ;
  const bool zeroB = _zero ( vB ) ;
  //
  if ( zeroB          ) { return ValueWithError ( 0 , -1 ) ; }
  //
  double cov2   =  vA * vA * nRejected.cov2() ;
  cov2         +=  vR * vR * nAccepted.cov2() ;
  cov2         /=  vB * vB   ;
  //
  return ValueWithError ( vA / vB , cov2 ) ;
}
// ============================================================================
/*  calculate the ratio of weighted to unweighted sample with uncertainties
 *  \f[ R = \frac{N_w}{N}  = \frac{ \sum_1^{N} w_i }{N} \f] 
 *  using jackknife method:
 *  \f[ \sigma^2(R) = \left( \sum_1^N w_i^2 - NR^2 \right) / (N-1)^2 \f] 
 *  @thanks Wouter Hulsbergen 
 *  @see http://en.wikipedia.org/wiki/Jackknife_%28statistics%29
 *  The result has proper behaviour : 
 *  uncertainty in R goes to zero if 
 *  dispersion on weights go to zero.
 *  @param   nWeighted (input) statistic of weighted sample 
 *  @param   n         (input) size      of origial sample 
 *  @return  ratio R with the proper uncertaities 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::effJackknife 
( const ValueWithError& nWeighted , 
  const unsigned long   n         ) 
{
  //
  if      ( 0 == n ) { return ValueWithError (-1,-1)                   ; }
  else if ( 1 == n ) { return ValueWithError ( nWeighted.value() , 0 ) ; }
  //
  const unsigned long n1 = n - 1 ;
  //
  const double r  = nWeighted.value() / n ;
  //
  double c2 = nWeighted.cov2 () - r*r*n ;
  //
  c2 /= n1 ;
  c2 /= n1 ;
  //
  return ValueWithError ( r , c2 ) ;
}
// ============================================================================
/*  Simple evaluation of efficiency using Zech's prescription 
 *  @param accepted  (IN) accepted sub-sample 
 *  @param total     (IN) total     sample 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::zechEff
( const Gaudi::Math::ValueWithError& accepted , 
  const Gaudi::Math::ValueWithError& total    ) 
{
  //
  const double e   =        accepted.value () / total.value () ;
  const double v2  = total.value() * total.value() ;
  const double t1  =           total.cov2  () / v2 ;
  const double t2  =        accepted.cov2  () / v2 ;
  //
  const double c2  = e * e * t1  + ( 1 - 2 * e ) * t2 ;
  //
  return Gaudi::Math::ValueWithError ( e , c2 ) ;
}
// ============================================================================
/*  evaluate pow(a,b)
 *  @param a (INPUT) the base
 *  @param b (INPUT) the exponent
 *  @return the <c>a</c> rased to power <c>b</b>
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::pow
( const Gaudi::Math::ValueWithError& a ,
  const int                          b )
{
  //
  if      ( 0 == b         ) { return 1 ; }          // RETURN
  else if ( 1 == b         ) { return a ; }          // RETURN
  //
  else if ( 0 >= a.cov2 () || _zero ( a.cov2() ) )
  { return _pow ( a.value() , b ) ;  }               // RETURN
  //
  const double v  =     _pow ( a.value () , b     ) ;
  const double e1 = b * _pow ( a.value () , b - 1 ) ;
  //
  return Gaudi::Math::ValueWithError ( v , e1 * e1 * a.cov2 () ) ;
  //
}
// ============================================================================
/*  evaluate pow(a,b)
 *  @param a (INPUT) the base
 *  @param b (INPUT) the exponent
 *  @return the <c>a</c> raised to power <c>b</b>
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::pow
( const Gaudi::Math::ValueWithError& a ,
  const double                       b )
{
  //
  if      ( _zero ( b )    ) { return 1 ; }         // RETURN
  else if ( _one  ( b )    ) { return a ; }         // RETURN
  else if ( 0 >= a.cov2 () || _zero ( a.cov2() ) )
  { return std::pow ( a.value() , b ) ; }           // RETURN
  //
  const double v  =     std::pow ( a.value () , b     ) ;
  const double e1 = b * std::pow ( a.value () , b - 1 ) ;
  //
  return Gaudi::Math::ValueWithError ( v , e1 * e1 * a.cov2 () ) ;
}
// ============================================================================
/*  evaluate pow(a,b)
 *  @param a (INPUT) the base
 *  @param b (INPUT) the exponent
 *  @return the <c>a</c> raised to power <c>b</b>
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::pow
( const int                          a ,
  const Gaudi::Math::ValueWithError& b )
{
  if      ( 0 == a && 0 < b.value()       ) { return 0 ; }    // RETURN
  else if ( 1 == a && _zero ( b.cov2 () ) ) { return 1 ; }    // RETURN
  else if ( 0 >= b.cov2() || _zero ( b.cov2() ) )
  { return std::pow ( double ( a ) , b.value() ) ; }    // RETURN
  //
  const double v  =     std::pow ( double ( a ) , b.value() ) ;
  const double e2 = v * std::log ( double ( a ) ) ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 * e2 * b.cov2 () ) ;
}
// ============================================================================
/*  evaluate pow(a,b)
 *  @param a (INPUT) the base
 *  @param b (INPUT) the exponent
 *  @return the <c>a</c> raised to power <c>b</b>
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::pow
( const double                       a ,
  const Gaudi::Math::ValueWithError& b )
{
  if      ( _zero ( a ) && 0 < b.value() ) { return 0 ; }    // RETURN
  else if ( 0 >= b.cov2() || _zero ( b.cov2() ) )
  { return std::pow ( a , b.value() ) ; }    // RETURN
  //
  const double v  =     std::pow ( a , b.value() ) ;
  const double e2 = v * std::log ( a ) ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 * e2 * b.cov2 () ) ;
  //
}
// ============================================================================
/*  evaluate pow(a,b)
 *  @param a (INPUT) the base
 *  @param b (INPUT) the exponent
 *  @return the <c>a</c> raised to power <c>b</b>
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::pow
( const Gaudi::Math::ValueWithError& a ,
  const Gaudi::Math::ValueWithError& b )
{
  //
  if ( &a == &b ) 
  {
    if      ( 0 >= a.cov2 () || _zero ( a.cov2() ) )
    { return std::pow ( a.value() , a.value() ) ; }
    //
    const double v2 = std::pow ( a.value() , a.value() ) ;
    const double v3 = std::log ( a.value() ) + 1 ;
    //
    return Gaudi::Math::ValueWithError
      ( v2 , v2 * v2 * v3 * v3 * a.cov2 () ) ;
  }
  //
  if      ( 0 >= a.cov2 () || _zero ( a.cov2() ) )
  { return pow ( a.value() , b         ) ; }
  else if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return pow ( a         , b.value() ) ; }
  //
  const double v  = std::pow ( a.value () , b.value ()     ) ;
  const double v1 = std::pow ( a.value () , b.value () - 1 ) ;
  //
  const double e1 = v1 *            b.value ()   ;
  const double e2 = v  * std::log ( a.value () ) ;
  //
  return Gaudi::Math::ValueWithError
    ( v , e1 * e1 * a.cov2 () + e2 * e2 * b.cov2 () ) ;
}
// ============================================================================
/*  evaluate exp(b)
 *  @param b (INPUT) the exponent
 *  @return the <c>e</c> raised to power <c>b</b>
 *  @warning invalid and small covariances are ignored
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::exp
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::exp ( b.value() ) ; }
  //
  const double v = std::exp ( b.value() ) ;
  return Gaudi::Math::ValueWithError ( v , v * v * b.cov2 () ) ;
}
// ============================================================================
/*  evaluate expm1(b)
 *  @param b (INPUT) the exponent
 *  @return  expm1
 *  @warning invalid and small covariances are ignored
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::expm1
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::expm1 ( b.value() ) ; }
  //
  const double v  = std::expm1 ( b.value() ) ;
  const double d1 = ( v + 1 )  ;
  const double d2 = d1 * d1    ;
  const double e2 = d2 * b.cov2() ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate tgamma(b)
 *  @param b (INPUT) the exponent
 *  @return  tgamma
 *  @warning invalid and small covariances are ignored
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::tgamma
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::tgamma ( b.value() ) ; }
  //
  const double bv = b.value() ;
  const double v  =      std::tgamma ( bv ) ;
  //
  // Gamma'/Gamma:
  const double p  = gsl_sf_psi ( bv ) ;
  const double e1 = v * p * b.error() ;
  //
  return Gaudi::Math::ValueWithError ( v , e1 * e1  ) ;
}
// ============================================================================
/*  evaluate lgamma(b)
 *  @param b (INPUT) the exponent
 *  @return  lgamma
 *  @warning invalid and small covariances are ignored
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::lgamma
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::lgamma ( b.value() ) ; }
  //
  const double bv = b.value() ;
  const double v  = std::lgamma  ( bv ) ;
  //
  const double d1 = gsl_sf_psi   ( bv ) ;
  const double d2 = d1 * d1 ;
  const double e2 = d2 * b.cov2() ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate <code>hypot(x,y)</code>
 *  \f$ \sqrt( x^2 + y^2 ) \f$
 *   @param x (INPUT) the first parameter
 *   @param y (INPUT) the second parameter
 *   @return the valueof <code>hypot</code> function
 */
// ============================================================================
Gaudi::Math::ValueWithError  Gaudi::Math::hypot
( const Gaudi::Math::ValueWithError& x , 
  const Gaudi::Math::ValueWithError& y )
{
  const bool x0 = 0 >= x.cov2() || _zero ( x.cov2() ) ;
  const bool y0 = 0 >= y.cov2() || _zero ( y.cov2() ) ;
  //
  if ( x0 && y0 ) { return std::hypot ( x.value()  , y.value()  ) ; }
  //
  const double r  = std::hypot ( x.value() , y.value() ) ;
  //
  double e2 = 0 ;
  if ( !_zero ( r ) ) 
  {
    e2 += x.cov2() * x.value() * x.value() ;
    e2 += y.cov2() * y.value() * y.value() ;
    e2 /= r * r ;
  }
  else 
  {
    e2 += x.cov2() ; 
    e2 += y.cov2() ;    
  }
  //
  return ValueWithError ( r , e2 ) ;
}

// ============================================================================


// ============================================================================
/*  evaluate log(b)
 *  @param b (INPUT) the parameter
 *  @return logarithm
 *  @warning invalid and small covariances are ignored
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::log
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) ) { return std::log ( b.value() ) ; }
  //
  const double v  = std::log ( b.value () ) ;
  const double e1 = 1.0 /      b.value ()   ;
  //
  return Gaudi::Math::ValueWithError ( v , e1 * e1 * b.cov2 () ) ;
}
// ============================================================================
/*  make a sum two elements taking into account the 
 *  correlation coefficient  
 *  @param a  (input) the first value 
 *  @param b  (input) the second value 
 *  @param c  (input) the correlation coefficient
 *  @return a+b 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2012-11-09
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::sum 
( const Gaudi::Math::ValueWithError& a , 
  const Gaudi::Math::ValueWithError& b , 
  const double                       c ) 
{
  //
  // the simplest case, ignore correlation  
  if ( &a == &b ) { return a*2.0 ; }     // RETURN
  //
  // few more trivial cases 
  //
  if      ( _zero ( a )  ) { return b ; }
  else if ( _zero ( b )  ) { return a ; }
  //
  // the second trivial case, no correlation  
  if      ( _zero ( c )  ) { return a + b ; } 
  //
  if      ( 0 > a.cov2() ) { return sum ( a.value() , b         , c ) ; }
  else if ( 0 > b.cov2() ) { return sum ( a         , b.value() , c ) ; }
  //
  const double v = a.value() + b.value();
  //
  // adjust the correlation coefficient 
  const double r   = std::max ( -1.0 , std::min ( 1.0 , c ) ) ;
  //
  const double ac2 = std::max ( a.cov2 () , 0.0 ) ;
  const double bc2 = std::max ( b.cov2 () , 0.0 ) ;
  //
  if ( _zero ( ac2 ) ) { return ValueWithError ( v , bc2 ) ; }  // RETURN
  if ( _zero ( bc2 ) ) { return ValueWithError ( v , ac2 ) ; }  // RETURN 
  //
  return ValueWithError ( v , ac2 + bc2 + 2 * r * std::sqrt ( ac2 * bc2 ) ) ;    
}
// ============================================================================
/*  make a sum two elements taking into account the 
 *  correlation coefficient  
 *  @param a  (input) the first value 
 *  @param b  (input) the second value 
 *  @param c  (input) the correlation coefficient
 *  @return a+b 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2012-11-09
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::sum2
( const Gaudi::Math::ValueWithError& a , 
  const Gaudi::Math::ValueWithError& b , 
  const double                       c ) 
{ return sum ( a , b , c ) ; }
// ============================================================================
/*  make a difference  two elements taking into acocunt the 
 *  correlation coefficient  
 *  @param a  (input) the first value 
 *  @param b  (input) the second value 
 *  @param c  (input) the correlation coefficient
 *  @return a-b 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2012-11-09
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::subtract
( const Gaudi::Math::ValueWithError& a , 
  const Gaudi::Math::ValueWithError& b , 
  const double                       c ) 
{
  //
  // the simplest case, ignore correlation  
  if ( &a == &b ) { return  ValueWithError ( 0 , 0 )  ; }     // RETURN
  //
  // few more trivial cases 
  //
  if      ( _zero ( a ) ) { return -b ; }
  else if ( _zero ( b ) ) { return  a ; }
  //
  const double v = a.value() - b.value();
  //
  if      ( 0 > a.cov2() ) { return subtract ( a.value() , b         , c ) ; }
  else if ( 0 > b.cov2() ) { return subtract ( a         , b.value() , c ) ; }
  //
  // the second trivial case, no correlation  
  if ( _zero ( c ) ) { return a - b ; } 
  //
  // adjust the correlation coefficient 
  const double r   = std::max ( -1.0 , std::min ( 1.0 , c ) ) ;
  //
  const double ac2 = std::max ( a.cov2 () , 0.0 ) ;
  const double bc2 = std::max ( b.cov2 () , 0.0 ) ;
  //
  if ( _zero ( ac2 ) ) { return ValueWithError ( v , bc2 ) ; }  // RETURN
  if ( _zero ( bc2 ) ) { return ValueWithError ( v , ac2 ) ; }  // RETURN 
  //
  return ValueWithError ( v , ac2 + bc2 - 2 * r * std::sqrt ( ac2 * bc2 ) ) ;    
}
// ============================================================================
/*  make a multiplication of two elements taking into acocunt the 
 *  correlation coefficient  
 *  @param a  (input) the first value 
 *  @param b  (input) the second value 
 *  @param c  (input) the correlation coefficient
 *  @return a*b 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2012-11-09
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::multiply
( const Gaudi::Math::ValueWithError& a , 
  const Gaudi::Math::ValueWithError& b , 
  const double                       c ) 
{
  //
  // the simplest case, ignore correlation  
  //
  if ( &a == &b ) 
  {
    const  double v = a.value() * a.value() ;
    return ValueWithError ( v , 4 * v * a.cov2()  ) ;   // RETURN
  }
  //
  // few more trivial cases 
  //
  if      ( _zero ( a ) ) { return ValueWithError ( 0 , 0 ) ; }
  else if ( _zero ( b ) ) { return ValueWithError ( 0 , 0 ) ; }
  else if ( _one  ( a ) ) { return b ; }
  else if ( _one  ( b ) ) { return a ; }
  //
  // ignore negative uncertainties 
  //
  if      ( 0 > a.cov2() ) { return multiply ( a.value() , b         , c ) ; }
  else if ( 0 > b.cov2() ) { return multiply ( a         , b.value() , c ) ; }
  //
  // the second trivial case, no correlation  
  //
  if ( _zero ( c ) ) { return a * b ; }                         // RETURN
  //
  const double  v   = a.value () * b.value () ;
  const double av2  = a.value () * a.value () ;
  const double bv2  = b.value () * b.value () ;
  //
  // adjust the correlation coefficient 
  const double r   = std::max ( -1.0 , std::min ( 1.0 , c ) ) ;
  //
  const double ac2 = std::max ( a.cov2 () , 0.0 ) ;
  const double bc2 = std::max ( b.cov2 () , 0.0 ) ;
  //
  if ( _zero ( ac2 ) ) { return ValueWithError ( v , av2 * bc2 ) ; }  // RETURN
  if ( _zero ( bc2 ) ) { return ValueWithError ( v , bv2 * ac2 ) ; }  // RETURN 
  //
  return ValueWithError ( v ,
                          bv2 * ac2 + 
                          av2 * bc2 + 
                          2 * v * r * std::sqrt ( ac2 * bc2 ) ) ;
}
// ============================================================================
/*  make a division of two elements taking into account the 
 *  correlation coefficient  
 *  @param a  (input) the first value 
 *  @param b  (input) the second value 
 *  @param c  (input) the correlation coefficient
 *  @return a/b 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2012-11-09
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::divide
( const Gaudi::Math::ValueWithError& a , 
  const Gaudi::Math::ValueWithError& b , 
  const double                       c ) 
{
  //
  // the simplest case, ignore correlation  
  //
  if ( &a == &b ) { return ValueWithError ( 1, 0 ) ; }
  //
  // few more trivial cases 
  //
  if      ( _zero ( a ) ) { return ValueWithError ( 0 , 0 ) ; }
  else if ( _one  ( a ) ) { return 1./b ; }
  else if ( _one  ( b ) ) { return a    ; }
  //
  // ignore negative uncertainties 
  //
  if      ( 0 > a.cov2() ) { return divide ( a.value() , b         , c ) ; }
  else if ( 0 > b.cov2() ) { return divide ( a         , b.value() , c ) ; }
  //
  // the second trivial case, no correlation  
  //
  if ( _zero ( c ) ) { return a / b ; }                         // RETURN
  //
  const double  v   = a.value () / b.value () ;
  const double av2  = a.value () * a.value () ;
  const double bv2  = b.value () * b.value () ;  
  //
  // adjust the correlation coefficient 
  const double r   = std::max ( -1.0 , std::min ( 1.0 , c ) ) ;
  //
  const double ac2 = std::max ( a.cov2 () , 0.0 ) ;
  const double bc2 = std::max ( b.cov2 () , 0.0 ) ;
  //
  const double ac2_n = ac2 / bv2 ;
  const double bc2_n = bc2 / bv2 ;
  const double av2_n = av2 / bv2 ;
  //
  if ( _zero ( ac2 ) ) { return ValueWithError ( v ,  av2_n * bc2_n ) ; }  // RETURN
  if ( _zero ( bc2 ) ) { return ValueWithError ( v ,          ac2_n ) ; }  // RETURN 
  //
  return ValueWithError ( v ,
                          ac2_n +  
                          av2_n * bc2_n - 
                          2 * v * r * std::sqrt ( ac2_n * bc2_n ) ) ;
}
// ===========================================================================
/*  calculate "fraction" of two elements (a/(a+b)) taking into account the 
 *  correlation coefficient  
 *  @param a  (input) the first value 
 *  @param b  (input) the second value 
 *  @param c  (input) the correlation coefficient
 *  @return a/(a+b)
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2012-11-09
 */
// ===========================================================================
Gaudi::Math::ValueWithError  
Gaudi::Math::fraction
( const Gaudi::Math::ValueWithError& a , 
  const Gaudi::Math::ValueWithError& b , 
  const double                       c ) 
{
  //
  const double av = std::abs ( a.value() ) ;
  const double bv = std::abs ( b.value() ) ;
  return 
    av > bv ? 
    1.0       / ( 1.0 + divide ( b , a , c ) ) : 
    1.0 - 1.0 / ( 1.0 + divide ( a , b , c ) ) ;
}
// ===========================================================================
/* calculate "asymmetry" of two elements $\frac{a-b}{a+b}$
 *  taking into account the correlation coefficient  
 *  @param a  (input) the first value 
 *  @param b  (input) the second value 
 *  @param c  (input) the correlation coefficient
 *  @return (a-b)/(a+b)
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2012-11-09
 */
// ===========================================================================
Gaudi::Math::ValueWithError Gaudi::Math::asymmetry
( const Gaudi::Math::ValueWithError& a , 
  const Gaudi::Math::ValueWithError& b , 
  const double                       c ) 
{
  //
  const double av = std::abs ( a.value() ) ;
  const double bv = std::abs ( b.value() ) ;
  if ( av > bv ) 
  {
    const ValueWithError d = divide ( b , a , c )  ;
    return divide ( 1.0 - d , 1.0 + d , -1.0 ) ;
  }
  const ValueWithError d = divide ( a , b , c )  ;
  return divide ( d - 1.0 , d + 1.0 , 1.0 ) ;
}
// ============================================================================
/*  evaluate log10(b)
 *  @param b (INPUT) the parameter
 *  @return logarithm
 *  @warning invalid and small covariances are ignored
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::log10
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::log10 ( b.value() ) ; }
  //
  const double v  = std::log10 ( b.value() ) ;
  ///
  static const double a  = 1.0 / std::log ( 10.0 ) ;
  //
  const double e1 = a / b.value() ;
  //
  return Gaudi::Math::ValueWithError ( v , e1 * e1 * b.cov2 () ) ;
}
// ============================================================================
/*  evaluate log1p(b)
 *  @param b (INPUT) the parameter
 *  @return  log1p(b)
 *  @warning invalid and small covariances are ignored
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::log1p
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::log1p ( b.value() ) ; }
  //
  const double bv = b.value() ;
  const double v  = std::log1p ( bv ) ;
  //
  const double d1 = 1 / ( 1 + bv ) ;
  const double e2 = d1 * d1 * b.cov2() ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate sqrt(b)
 *  @param b (INPUT) the parameter
 *  @warning invalid and small covariances are ignored
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::sqrt
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::sqrt ( b.value() ) ; }
  //
  const double v  = std::sqrt ( b.value() ) ;
  ///
  const double e2 = 0.25 * b.cov2() / b.value() ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate cbrt(b)
 *  @param b (INPUT) the parameter
 *  @warning invalid and small covariances are ignored
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::cbrt
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::cbrt ( b.value() ) ; }
  //
  const double v  = std::cbrt ( b.value() ) ;
  //
  const double e2 = b.cov2() / ( v * b.value() ) ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 / 9.0 ) ;
}
// ============================================================================
/*  evaluate sin(b)
 *  @param b (INPUT) the parameter
 *  @warning invalid and small covariances are ignored
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::sin
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::sin ( b.value() ) ; }
  //
  const double v  = std::sin ( b.value() ) ;
  const double d2 = std::max ( 1 - v*v , 0.0 ) ;
  //
  const double e2 = std::min ( d2 * b.cov2() , 1.0 ) ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate cos(b)
 *  @param b (INPUT) the parameter
 *  @warning invalid and small covariances are ignored
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::cos
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::cos ( b.value() ) ; }
  //
  const double v  = std::cos ( b.value() ) ;
  const double d2 = std::max ( 1 - v*v , 0.0 ) ;
  //
  const double e2 = std::min ( d2 * b.cov2() , 1.0 ) ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate tan (b)
 *  @param b (INPUT) the parameter
 *  @warning invalid and small covariances are ignored
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::tan
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::tan ( b.value() ) ; }
  //
  const double v  = std::tan ( b.value() ) ;
  const double d  = 1 + v * v ;
  //
  const double e2 = d * d  * b.cov2() ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate sinh(b)
 *  @param b (INPUT) the parameter
 *  @warning invalid and small covariances are ignored
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::sinh
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::sinh ( b.value() ) ; }
  //
  const double v  = std::sinh ( b.value() ) ;
  const double d2 =  1 + v * v ;
  //
  const double e2 = d2 * b.cov2() ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate cosh(b)
 *  @param b (INPUT) the parameter
 *  @warning invalid and small covariances are ignored
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::cosh
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::cosh ( b.value() ) ; }
  //
  const double v  = std::cosh ( b.value() ) ;
  const double d2 = v * v - 1 ;
  //
  const double e2 = std::max ( d2 * b.cov2() , 0.0 ) ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate tanh (b)
 *  @param b (INPUT) the parameter
 *  @warning invalid and small covariances are ignored
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::tanh
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::tanh ( b.value() ) ; }
  //
  const double v  = std::tanh ( b.value() ) ;
  const double d  = 1 - v * v ;
  //
  const double e2 = std::min ( d * d  * b.cov2() , 1.0 ) ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate erf(b)
 *  @param b (INPUT) the parameter 
 *  @return  erf(b)
 *  @warning invalid and small covariances are ignored 
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::erf 
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::erf( b.value() ) ; }
  //
  const double bv = b.value() ;
  const double v  = std::erf ( bv ) ;
  //
  static const double factor  = 4.0 / M_PI ;
  //
  const double d2 = factor * std::exp ( - bv * bv ) ;
  const double e2 = d2     * b.cov2() ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate erfc(b)
 *  @param b (INPUT) the parameter 
 *  @return  erfc(b)
 *  @warning invalid and small covariances are ignored 
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::erfc 
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::erfc( b.value() ) ; }
  //
  const double bv = b.value() ;
  const double v  = std::erfc ( bv ) ;
  //
  static const double factor  = 4.0 / M_PI ;
  //
  const double d2 = factor * std::exp ( - bv * bv ) ;
  const double e2 = d2     * b.cov2() ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate erfcx(b)
 *  @param b (INPUT) the parameter 
 *  @return  erfc(b)
 *  @warning invalid and small covariances are ignored 
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::erfcx 
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return Gaudi::Math::erfcx ( b.value() ) ; }
  //
  const double bv = b.value() ;
  const double v  = Gaudi::Math::erfcx ( bv ) ;
  //
  static const double factor  = 2.0 / std::sqrt ( M_PI ) ;
  //
  // derivative 
  const double d  = 2 * bv * v - factor ; //  
  const double e2 = d * d  * b.cov2()   ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate probit(b)
 *  @param b (INPUT) the parameter 
 *  @return  erfc(b)
 *  @warning invalid and small covariances are ignored 
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::probit 
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return Gaudi::Math::probit ( b.value() ) ; }
  //
  const double bv = b.value() ;
  const double v  = Gaudi::Math::probit ( bv ) ;
  //
  static const double factor  = std::sqrt ( 2 * M_PI ) ;
  //
  // derivative 
  const double d  = factor * std::exp ( 0.5 * v * v );
  const double e2 = d * d  * b.cov2()   ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate asin(b)
 *  @param b (INPUT) the parameter 
 *  @return  asin(b)
 *  @warning invalid and small covariances are ignored 
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::asin
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::asin( b.value() ) ; }
  //
  const double bv = b.value() ;
  const double v  = std::asin ( bv ) ;
  //
  const double b2 = bv * bv ;
  if ( _one  ( b2 ) ) { return Gaudi::Math::ValueWithError ( v , -1 ) ; }
  //
  const double d2 = 1.0 / ( 1 - b2 ) ;
  const double e2 = d2     * b.cov2() ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate acos(b)
 *  @param b (INPUT) the parameter 
 *  @return  acos(b)
 *  @warning invalid and small covariances are ignored 
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::acos
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::acos ( b.value() ) ; }
  //
  const double bv = b.value() ;
  const double v  = std::acos( bv ) ;
  //
  const double b2 = bv * bv ;
  if ( _one  ( b2 ) ) { return Gaudi::Math::ValueWithError ( v , -1 ) ; }
  //
  const double d2 = 1.0 / ( 1 - b2 ) ;
  const double e2 = d2     * b.cov2() ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate atan(b)
 *  @param b (INPUT) the parameter 
 *  @return  atan(b)
 *  @warning invalid and small covariances are ignored 
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::atan
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::atan ( b.value() ) ; }
  //
  const double bv = b.value() ;
  const double v  = std::atan( bv ) ;
  //
  const double b2 = bv * bv ;
  const double d1 = 1.0 / ( 1 + b2 ) ;
  const double d2 = d1 * d1 ;
  const double e2 = d2      * b.cov2() ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate asinh(b)
 *  @param b (INPUT) the parameter 
 *  @return  asinh(b)
 *  @warning invalid and small covariances are ignored 
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::asinh
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::asinh( b.value() ) ; }
  //
  const double bv = b.value() ;
  const double v  = std::asinh ( bv ) ;
  //
  const double b2 = bv * bv ;
  const double d2 = 1.0 / ( 1 + b2 ) ;
  const double e2 = d2     * b.cov2() ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate acosh(b)
 *  @param b (INPUT) the parameter 
 *  @return  acosh(b)
 *  @warning invalid and small covariances are ignored 
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::acosh
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::acosh( b.value() ) ; }
  //
  const double bv = b.value() ;
  const double v  = std::acosh ( bv ) ;
  //
  const double b2 = bv * bv ;
  const double d2 = 1.0 / ( b2 - 1 ) ;
  const double e2 = d2     * b.cov2() ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  evaluate atanh(b)
 *  @param b (INPUT) the parameter 
 *  @return  acosh(b)
 *  @warning invalid and small covariances are ignored 
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::atanh
( const Gaudi::Math::ValueWithError& b )
{
  if ( 0 >= b.cov2 () || _zero ( b.cov2() ) )
  { return std::atanh( b.value() ) ; }
  //
  const double bv = b.value() ;
  const double v  = std::atanh( bv ) ;
  //
  const double b2 = bv * bv ;
  const double d1 = 1.0 / ( 1 - b2 ) ;
  const double d2 = d1 * d1 ;
  const double e2 = d2      * b.cov2() ;
  //
  return Gaudi::Math::ValueWithError ( v , e2 ) ;
}
// ============================================================================
/*  simple linear interpolation 
 *  @param x  the value to evaluate the function 
 *  @param x0 the abscissa for the first  point
 *  @param y0 the function value for the first  point
 *  @param x1 the abscissa for the second point
 *  @param y1 the function value for the second point
 *  @return linear interpolation at point x
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::interpolate_1D
( const double                       x  , 
  const double                       x0 ,
  const Gaudi::Math::ValueWithError& y0 , 
  const double                       x1 ,
  const Gaudi::Math::ValueWithError& y1 ) 
{
  //
  const double c0 = ( x - x1 ) / ( x0 - x1 ) ;
  const double c1 = ( x - x0 ) / ( x1 - x0 ) ;
  //
  return c0 * y0 + c1 * y1 ;
}
// ============================================================================
/*  simple (bi)linear interpolation 
 *  @param x  the x-coordiate to evaluate the function 
 *  @param y  the y-coordiate to evaluate the function 
 *  @param x0 the x-coordinate for the first  pair of points
 *  @param x1 the x-coordinate for the second pair of points
 *  @param y0 the y-coordinate for the first  pair of points
 *  @param y1 the y-coordinate for the second pair of points
 *  @param v00 the function value 
 *  @param v01 the function value 
 *  @param v10 the function value 
 *  @param v11 the function value 
 *  @return bilinear interpolation at point (x,y)
 */
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::interpolate_2D 
( const double                       x   , 
  const double                       y   , 
  const double                       x0  ,
  const double                       x1  ,
  const double                       y0  ,
  const double                       y1  ,
  const Gaudi::Math::ValueWithError& v00 , 
  const Gaudi::Math::ValueWithError& v01 , 
  const Gaudi::Math::ValueWithError& v10 , 
  const Gaudi::Math::ValueWithError& v11 ) 
{
  //
  const double c00 =  ( x - x1 ) * ( y - y1 ) / ( x0 - x1 ) / ( y0 - y1 ) ;
  const double c01 =  ( x - x1 ) * ( y - y0 ) / ( x0 - x1 ) / ( y1 - y0 ) ;
  const double c10 =  ( x - x0 ) * ( y - y1 ) / ( x1 - x0 ) / ( y0 - y1 ) ;
  const double c11 =  ( x - x0 ) * ( y - y0 ) / ( x1 - x0 ) / ( y1 - y0 ) ;
  //
  return c00 * v00 + c01 * v01 + c10 * v10 + c11 * v11  ;
} 
// ============================================================================
/*  get the sum of the vector 
 *  @param vct the vector
 *  @param ini the intial value 
 *  @return the sum over the vector 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::sum 
( const std::vector<Gaudi::Math::ValueWithError>& vct , 
  Gaudi::Math::ValueWithError                     ini ) 
{
  //
  for ( std::vector<Gaudi::Math::ValueWithError>::const_iterator iv = 
          vct.begin() ; vct.end() != iv ; ++iv ) { ini += (*iv) ; }
  //
  return ini ;
  //
}
// ============================================================================
/*  get the sum of the vector 
 *  @param vct the vector
 *  @param ini the intial value 
 *  @return the sum over the vector 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::accumulate  
( const std::vector<Gaudi::Math::ValueWithError>& vct , 
  Gaudi::Math::ValueWithError                     ini ) 
{ return sum ( vct , ini ) ; }
// ============================================================================
/*  get the sum of absolute values for the vector 
 *  @param vct the vector
 *  @return the sum over the vector 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::abssum 
( const std::vector<Gaudi::Math::ValueWithError>& vct )
{
  //
  ValueWithError val ;
  for ( std::vector<Gaudi::Math::ValueWithError>::const_iterator iv = 
          vct.begin() ; vct.end() != iv ; ++iv ) 
  { val += abs (*iv) ; }
  //
  return val ;
  //
}
// ============================================================================
/*  evaluate polynomial
 *  \f$f(x) = a_0 + a_1x + a_2x^2 + ... + a_{n-1}x^{n-1} + a_nx^n\f$
 *  such as \f$f(0) = a_0 \f$      
 *  using Horner rule
 *  @param poly  INPUT the coefficients
 *  @param x     INPUT argument 
 *  @return value of polynomial
 *  @warning invalid and small covariances are ignored 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::horner_a0
( const std::vector<double>&         poly ,
  const Gaudi::Math::ValueWithError& x    ) 
{
  if ( poly.empty() ) { return Gaudi::Math::ValueWithError(0,0) ; }
  //
  const std::pair<long double,long double> r = 
    Gaudi::Math::Clenshaw::monomial_sum 
    ( poly.rbegin() , poly.rend() , x.value() ) ;
  //
  if ( 0 >= x.cov2() || _zero( x.cov2() ) ) { return r.first  ; }
  //
  return Gaudi::Math::ValueWithError( r.first , r.second * r.second * x.cov2() );
}
// ============================================================================
/*  evaluate polynomial
 *  \f$f(x) = a_0x^n + a_1x^{n-1}+ ... + a_{n-1}x + a_n\f$, 
 *   such as \f$f(0) = a_n \f$      
 *  using Horner rule
 *  @param poly  INPUT the coefficients 
 *  @param x     INPUT argument 
 *  @return value of polynomial
 *  @warning invalid and small covariances are ignored 
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::horner_aN
( const std::vector<double>&         poly ,
  const Gaudi::Math::ValueWithError& x    ) 
{
  if ( poly.empty() ) { return Gaudi::Math::ValueWithError(0,0) ; }
  //
  const std::pair<double,double> r = 
    Gaudi::Math::Clenshaw::monomial_sum 
    ( poly.begin() , poly.end() , x.value() ) ;
  //
  if ( 0 >= x.cov2() || _zero( x.cov2() ) ) { return r.first  ; }
  return Gaudi::Math::ValueWithError( r.first , r.second * r.second * x.cov2() ) ;
}
// ============================================================================
// Utiilties 
// ============================================================================
// print the vector
// ============================================================================
std::ostream& Gaudi::Utils::toStream
( const std::vector<Gaudi::Math::ValueWithError>& o , std::ostream& s ) 
{ 
  //
  Gaudi::Utils::toStream 
    ( o.begin () , o.end () , s  , "[ " , " ]" , " , " ) ;
  //
  return s ;
}
// ============================================================================
// the output operator for the vector 
// ============================================================================
std::ostream& 
Gaudi::Math::operator<<( std::ostream&                                   s , 
                         const std::vector<Gaudi::Math::ValueWithError>& v ) 
{ 
  return Gaudi::Utils::toStream ( v , s ) ; 
}
// ============================================================================

// ============================================================================
// Boost.Bind
// ============================================================================
#include "boost/bind.hpp"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ParsersFactory.h"
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Parsers
  {
    // ========================================================================
    /** @class ValueWithErrorGrammar
     *
     *  The valid represenation of value with error are:
     *     "( 34 +- 10 )" , "( 34 , 10 )" , "34"
     *
     *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
     *  @author Alexander Mazurov  alexander.mazurov@gmail.com
     *  @date 2011-07-27
     */
    template< typename Iterator, typename Skipper>
    class ValueWithErrorGrammar:
       public qi::grammar<Iterator,Gaudi::Math::ValueWithError(), Skipper> {
     // =======================================================================
     public: // Result type
       typedef Gaudi::Math::ValueWithError ResultT ;
     // =======================================================================
     public: // Grammar is defined at the constructor:
      ValueWithErrorGrammar() :ValueWithErrorGrammar::base_type(value) {
          value = ( qi::lit("(") >> grvalue[op(qi::_val,qi::_1)]
             >> -((qi::lit("+-") | qi::lit(',') | qi::lit(';'))
                   >> grvalue[op(qi::_val,qi::_1, tag_error())])
             >> ')'
             ) | grvalue[op(qi::_val,qi::_1)] ;
        }
     // =======================================================================
     private:  // Rules and grammars:
      // Start rule should have the same template parameters as
      // qi::grammar at the class definition:
      qi::rule<Iterator, Gaudi::Math::ValueWithError(), Skipper> value;
      RealGrammar<Iterator, double, Skipper> grvalue;
     // =======================================================================
     private: // Semantic actions:
      struct tag_error{};
      struct Operations {
        // Some magic:
        template <typename A, typename B = boost::fusion::unused_type,
            typename C = boost::fusion::unused_type,
            typename D = boost::fusion::unused_type>
        struct result { typedef void type; };
        // Actions:
        // --------------------------------------------------------------------
        void operator()(Gaudi::Math::ValueWithError& val, double v) const {
          val.setValue(v);
        }
        void operator()(Gaudi::Math::ValueWithError& val, double err,
            tag_error) const {
          val.setError(err);
        }
        // --------------------------------------------------------------------
      };  // End of Operations
      // ----------------------------------------------------------------------
      ph::function<Operations> op;
    };
    // ========================================================================
    REGISTER_GRAMMAR(Gaudi::Math::ValueWithError, ValueWithErrorGrammar);
    // ========================================================================
  } //                                          end of namespace Gaudi::Parsers
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================


// =============================================================================
/*  parse the input string into the result
 *  @param result (output) the result
 *  @param input  (input) input string
 */
// =============================================================================
StatusCode Gaudi::Parsers::parse(Gaudi::Math::ValueWithError& result,
    const std::string& input) {
  return parse_(result, input);
}
// =============================================================================
/* parse the input string into the result
 *  @param result (output) the result
 *  @param input  (input) input string
 */
// =============================================================================
StatusCode Gaudi::Parsers::parse(
    std::vector<Gaudi::Math::ValueWithError>& result,
    const std::string& input) {
  return parse_(result, input);
}
// =============================================================================


// =============================================================================
// The END
// =============================================================================


