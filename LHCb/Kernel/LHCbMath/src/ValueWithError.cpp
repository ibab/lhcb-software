// $Id: ValueWithError.cpp,v 1.4 2009-09-13 18:58:07 ibelyaev Exp $
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
// ============================================================================
// local
// ============================================================================
#include "LHCbMath/ValueWithError.h"
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
// conversion to string
// ============================================================================
std::string Gaudi::Math::ValueWithError::toString   () const 
{
  std::ostringstream s ;
  fillStream ( s ) ;
  return s.str() ;
}
// =============================================================================
// evaluate the mean of a and b 
// =============================================================================
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


