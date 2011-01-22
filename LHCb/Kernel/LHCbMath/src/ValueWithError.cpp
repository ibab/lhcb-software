// $Id: ValueWithError.cpp,v 1.5 2010-03-18 18:26:01 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <cmath>
#include <string>
#include <sstream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Parsers.h"
#include "GaudiKernel/Lomont.h"
// ============================================================================
// local
// ============================================================================
#include "LHCbMath/ValueWithError.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/format.hpp"
// ============================================================================
/** @file 
 *  Implementation file for class Gaudi::Math::ValueWithError
 *  @date 2009-06-03 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
// constructor from the value and covariance 
// ============================================================================
Gaudi::Math::ValueWithError::ValueWithError 
( const double value      , 
  const double covariance ) 
  : m_value ( value      ) 
  , m_cov2  ( covariance ) 
{}
// ============================================================================
// constructor from the (value,error)-pair 
// ============================================================================
Gaudi::Math::ValueWithError::ValueWithError
( const std::pair<double,double>& value ) 
  : m_value ( value.first ) 
  , m_cov2  () 
{
  setError ( value.second ) ;
}
// ============================================================================
// set the error 
// ============================================================================
void Gaudi::Math::ValueWithError::setError ( const double e ) 
{ 
  m_cov2  = e * e ;
  if ( 0 > e ) { m_cov2 = -m_cov2 ; }
} 
// ============================================================================
// get the error 
// ============================================================================
double Gaudi::Math::ValueWithError::error      () const 
{ return 0 <= m_cov2 ? std::sqrt ( m_cov2 ) : -std::sqrt ( -m_cov2 ) ; }
// ============================================================================
// += 
// ============================================================================
Gaudi::Math::ValueWithError& 
Gaudi::Math::ValueWithError::operator+= 
( const Gaudi::Math::ValueWithError& right )                             // += 
{
  m_value += right.m_value ;
  m_cov2  += right.m_cov2  ;
  return *this ;
}
// ============================================================================
// -= 
// ============================================================================
Gaudi::Math::ValueWithError& 
Gaudi::Math::ValueWithError::operator-= 
( const Gaudi::Math::ValueWithError& right )                              // -= 
{
  m_value -= right.m_value ;
  m_cov2  += right.m_cov2  ;
  return *this ;
}
// ============================================================================
// *= 
// ============================================================================
Gaudi::Math::ValueWithError& 
Gaudi::Math::ValueWithError::operator*= 
( const Gaudi::Math::ValueWithError& right )                              // *= 
{
  const double _a2 =       m_value *       m_value ;
  const double _b2 = right.m_value * right.m_value ;
  m_cov2  *= _b2                 ;
  m_cov2  += _a2 * right.m_cov2  ;
  m_value *=      right.m_value ;
  return *this ;
}
// ============================================================================
// /= 
// ============================================================================
Gaudi::Math::ValueWithError& 
Gaudi::Math::ValueWithError::operator/= 
( const Gaudi::Math::ValueWithError& right )                              // /= 
{
  const double _a2 =       m_value *       m_value ;
  const double _b2 = right.m_value * right.m_value ;
  const double _b4 = _b2 * _b2 ;
  //
  m_cov2  /= _b2 ;
  m_cov2  += ( _a2 / _b4 ) * right.m_cov2 ;
  m_value /= right.m_value ;
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
  if      ( 0 >=   cov2 () && 0 >= b.cov2 () ) { return 0.5*( value() + b.value() ) ; }
  else if ( 0 >=   cov2 ()                   ) { return *this ; }
  else if ( 0 >= b.cov2 ()                   ) { return b     ; }
  
  const double _cov2 = 1.0/( 1.0/cov2() + 1.0/b.cov2() ) ;
  
  return Gaudi::Math::ValueWithError 
    ( _cov2 * ( value()/cov2() + b.value()/b.cov2() ) ,  _cov2 ) ;  
}
// =============================================================================
// evaluate chi2 
// =============================================================================
double Gaudi::Math::ValueWithError::chi2 
( const Gaudi::Math::ValueWithError& b ) const
{
  const double s_cov2 = cov2() + b.cov2() ;
  if      ( 0 >= s_cov2 ) { return -1 ; } // RETURN 

  const double diff = value() - b.value() ;  
  return diff*diff/s_cov2 ;
}
// =============================================================================
// evaluate chi2 
// =============================================================================
double Gaudi::Math::ValueWithError::chi2 ( const double b ) const
{
  if      ( 0 >= cov2 () ) { return -1 ; } // RETURN
  const double diff = value() - b ;  
  return diff*diff/cov2() ;
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
// for easy pythonization
// =============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__add__ 
( const Gaudi::Math::ValueWithError& right ) const 
{
  ValueWithError tmp ( *this ) ;
  return tmp += right ;
}
// =============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__sub__ 
( const Gaudi::Math::ValueWithError& right ) const 
{
  ValueWithError tmp ( *this ) ;
  return tmp -= right ;
}
// =============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__mul__ 
( const Gaudi::Math::ValueWithError& right ) const 
{
  ValueWithError tmp ( *this ) ;
  return tmp *= right ;
}
// =============================================================================
Gaudi::Math::ValueWithError
Gaudi::Math::ValueWithError::__div__ 
( const Gaudi::Math::ValueWithError& right ) const 
{
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
{ return ValueWithError( right - value() , cov2()  ) ; }
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
  if  ( n >  N ) { return binomEff       ( N , n ) ; }
  if  ( 0 == N ) { return ValueWithError ( 1 , 1 ) ; }
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
namespace 
{
  // ==========================================================================
  const unsigned int _maxULPs = 10000 ;
  // ==========================================================================
  // check if the double value close to zero 
  inline bool _zero ( const double value ) 
  { return 0.0 == value || Gaudi::Math::lomont_compare_double ( value , 0.0 , _maxULPs ) ; }
  // check if the double value close to one 
  inline bool _one  ( const double value ) 
  { return 1.0 == value || Gaudi::Math::lomont_compare_double ( value , 1.0 , _maxULPs ) ; }
  // ==========================================================================
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
  if      ( 0 == b         ) { return 1 ; }          // RETURN
  else if ( 1 == b         ) { return a ; }          // RETURN
  else if ( 0 >= a.cov2 () || _zero ( a.cov2() ) )  
  { return std::pow ( a.value() , b ) ; }            // RETURN
  //
  const double v  =     std::pow ( a.value () , b     ) ;
  const double e1 = b * std::pow ( a.value () , b - 1 ) ;
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
  const double v  = std::log ( b.value() ) ;
  const double e1 = 1.0 / b.value()        ;
  return Gaudi::Math::ValueWithError ( v , e1 * e1 * b.cov2 () ) ;
}
// ============================================================================
/*  evaluate log(b)
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
  return Gaudi::Math::ValueWithError ( v , e1 * e1 * b.cov2 () ) ;
}
// ============================================================================
// Boost.Bind
// ============================================================================
#include "boost/bind.hpp"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Parsers.h"
#include "GaudiKernel/Grammars.h"
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
     *  @date 2009-09-13
     */
    class ValueWithErrorGrammar 
      : public grammar
    <
      ValueWithErrorGrammar,
      ClosureGrammar<Gaudi::Math::ValueWithError>::context_t
    >
    {
    public:
      // ======================================================================
      typedef RealGrammar<double>                               ValueGrammarT ;
      typedef Gaudi::Math::ValueWithError                             ResultT ;
      // ======================================================================
    public:
      // ======================================================================
      /// callback. Action when we match first value
      void matchValue  ( const ValueGrammarT::ResultT v ) const
      { this->val().setValue ( v ) ; }
      /// callback. Action when we match second value
      void matchError  ( const ValueGrammarT::ResultT e ) const 
      { this->val().setError ( e ) ; }
      // ======================================================================
    public:
      // ============================================================
      template <typename ScannerT>
      struct definition
      {
        //
        definition ( ValueWithErrorGrammar const &self)
        {
          vale = 
            ( 
             str_p("(") 
             >> grvalue [boost::bind(&ValueWithErrorGrammar::matchValue,&self,_1)]
             >> !( ( str_p( "+-" ) | ',' | ';' ) 
                   >> grvalue [boost::bind(&ValueWithErrorGrammar::matchError,&self,_1)]) 
             >> ')' 
             ) | grvalue [boost::bind(&ValueWithErrorGrammar::matchValue,&self,_1)] ;
        }
        // 
        rule<ScannerT> const& start() const { return vale; }
        rule<ScannerT> vale;
        ValueGrammarT  grvalue ;
      } ;
      // ======================================================================
    } ;
    // ========================================================================
  } //                                          end of namespace Gaudi::Parsers 
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================

  





// =============================================================================
/*  parse the input string into the result 
 *  @param result (output) the result 
 *  @param input  (input) input string
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-13
 */     
// =============================================================================
StatusCode Gaudi::Parsers::parse 
( Gaudi::Math::ValueWithError& result , 
  const std::string&           input  ) 
{
  ValueWithErrorGrammar gr ;
  
  return parse
    ( input.begin () , 
      input.end   () ,
      gr[var(result)=arg1] , 
      SkipperGrammar()).full;
}
// =============================================================================
/* parse the input string into the result 
 *  @param result (output) the result 
 *  @param input  (input) input string
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-13
 */     
// =============================================================================
StatusCode Gaudi::Parsers::parse 
( std::vector<Gaudi::Math::ValueWithError>& result , 
  const std::string&                        input  ) 
{
  VectorGrammar<ValueWithErrorGrammar> gr ;
  
  return parse ( input.begin () , 
                 input.end   () , 
                 gr[var(result)=arg1],
                 SkipperGrammar()).full;
}
// =============================================================================



// =============================================================================
// The END 
// =============================================================================


