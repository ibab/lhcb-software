// $Id: ValueWithError.h,v 1.5 2010-03-18 18:26:00 ibelyaev Exp $
// ============================================================================
#ifndef LHCBMATH_ERRORS_H 
#define LHCBMATH_ERRORS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD& STL 
// ============================================================================
#include <iosfwd>
#include <utility>
#include <vector>
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
/** @file 
 *  Collection fo useful objects with associated "covarinaces".
 *  The concept has been stollen from Wouter Hulsbergen's lines 
 *  @author Vanya BELYAEV Ivane.BElyaev@nikhef.nl
 *  @date 20090603
 */
namespace Gaudi
{
  // ==========================================================================
  namespace Math 
  {
    // ========================================================================
    /** @class ValueWithError 
     *  The most simple representation of "value with associated error"
     *  The concept has been stollen from Wouter Hulsbergen's lines 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 20090603
     */
    class GAUDI_API ValueWithError 
    {
    public:
      // ======================================================================
      typedef double                                               Value      ;
      typedef double                                               Covariance ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the value and covariance 
      ValueWithError ( const double value      = 0 , 
                       const double covariance = 0 ) ;
      /** constructor from the (value,error)-pair 
       *   - first  element is "value" 
       *   - second element is "error" 
       *  @param pair_  (value,error)-pair 
       */
      ValueWithError ( const std::pair<double,double>& pair_ ) ;
      /** constructor from string representation 
       *  - ( value +- error ) 
       *  - ( value  , error ) 
       *  - ( value  ; error ) 
       */
      ValueWithError ( const std::string& value ) ;
      // ======================================================================
    public: // trivial accessors 
      // ======================================================================
      /// get the value 
      double value      () const { return m_value ; }
      /// get the covariance 
      double cov2       () const { return m_cov2  ; }
      /// get the covariance 
      double covariance () const { return m_cov2  ; }
      /** get the error 
       *  @attention negative erorr is returned for invalid covariance 
       *  @return the error estimate 
       */
      double error      () const ;
      // ======================================================================
    public: // setters 
      // ======================================================================
      void setValue      ( const double v ) { m_value = v      ; }
      void setCovariance ( const double c ) { m_cov2  = c      ; }
      /** set the error 
       *  @attention invalid covariance is set for negative error 
       */
      void setError      ( const double e ) ; 
      // ======================================================================
    public: // finally it is just a value 
      // ======================================================================
      /// cast 
      operator double () const { return value () ; }                    // cast 
      // ======================================================================
    public:  // operators
      // ======================================================================      
      /// += 
      ValueWithError& operator+= ( const ValueWithError& right ) ;        // += 
      /// -= 
      ValueWithError& operator-= ( const ValueWithError& right ) ;        // -= 
      /// *= 
      ValueWithError& operator*= ( const ValueWithError& right ) ;        // *= 
      /// /= 
      ValueWithError& operator/= ( const ValueWithError& right ) ;        // /= 
      // ======================================================================
    public: 
      // ======================================================================
      /// += 
      ValueWithError& operator+= ( const double v ) ;                     // += 
      /// -= 
      ValueWithError& operator-= ( const double v ) ;                     // -= 
      /// *= 
      ValueWithError& operator*= ( const double v ) ;                     // *= 
      /// /= 
      ValueWithError& operator/= ( const double v ) ;                     // /= 
      // ======================================================================
    public: // unary - 
      // ======================================================================
      /// unary - 
      ValueWithError operator-() const ;                             // unary - 
      // ======================================================================
    public: // as pair:
      // ======================================================================
      /// get as std::pair 
      std::pair<double,double> asPair () const 
      { return std::pair<double,double> ( value () , error () ) ; }
      /// conversion to std::pair
      operator std::pair<double,double>() const 
      { return std::pair<double,double> ( value () , error () ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the mean value 
      ValueWithError mean ( const ValueWithError& right ) const ;
      /// get chi2 distance 
      double chi2 ( const ValueWithError& right ) const ;
      /// get chi2 distance 
      double chi2 ( const double          right ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /** evaluate the "fraction" \f$  \frac{a}{a+b} \f$ 
       *  @param  b the parameter "b" for the fraction 
       *  @return a/(a+b) 
       */
      ValueWithError frac ( const ValueWithError& b ) const ;
      /** evaluate the "fraction" \f$  \frac{a}{a+b} \f$ 
       *  @param  b the parameter "b" for the fraction 
       *  @return a/(a+b) 
       */
      ValueWithError frac ( const double          b ) const ;
      /** evaluate the "asymmetry" \f$  \frac{a-b}{a+b} \f$ 
       *  @param  b the parameter "b" for the fraction 
       *  @return (a-b)/(a+b) 
       */
      ValueWithError asym ( const ValueWithError& b ) const ;
      /** evaluate the "asymmetry" \f$  \frac{a-b}{a+b} \f$ 
       *  @param  b the parameter "b" for the fraction 
       *  @return (a-b)/(a+b) 
       */
      ValueWithError asym ( const double          b ) const ;
      // ======================================================================
    public: // helper functions for Python:
      // ======================================================================
      ///    a + right 
      ValueWithError __add__  ( const ValueWithError& right ) const ;
      ///    a + right 
      ValueWithError __add__  ( const double          right ) const ;
      ///        right + a  
      ValueWithError __radd__ ( const double          right ) const { return __add__ ( right ) ; }
      ///    a - right  
      ValueWithError __sub__  ( const ValueWithError& right ) const ;
      ///    a - right  
      ValueWithError __sub__  ( const double          right ) const ;
      ///        right - a   
      ValueWithError __rsub__ ( const double          right ) const ;
      ///    a * right  
      ValueWithError __mul__  ( const ValueWithError& right ) const ;
      ///    a * right  
      ValueWithError __mul__  ( const double          right ) const ;
      ///        right * a   
      ValueWithError __rmul__ ( const double          right ) const { return __mul__ ( right ) ; }
      ///    a / right  
      ValueWithError __div__  ( const ValueWithError& right ) const ;
      ///    a / right  
      ValueWithError __div__  ( const double          right ) const ;
      ///        right / a   
      ValueWithError __rdiv__ ( const double          right ) const ;
      ///  abs ( a )   
      ValueWithError __abs__  () const ;
      /// -me
      ValueWithError __neg__  () const ;
      /// +me  (no effect) 
      ValueWithError __pos__  () const ; 
      /// me**e 
      ValueWithError __pow__  ( const int             e ) const ;
      /// me**e 
      ValueWithError __pow__  ( const double          e ) const ;
      /// me**e 
      ValueWithError __pow__  ( const ValueWithError& e ) const ;
      /// e**me 
      ValueWithError __rpow__ ( const int             e ) const ;
      /// e**me 
      ValueWithError __rpow__ ( const double          e ) const ;
      /// exp(me) 
      ValueWithError __exp__   () const ;
      /// log(me) 
      ValueWithError __log__   () const ;
      /// log10(me) 
      ValueWithError __log10__ () const ;
      // ======================================================================
    public:
      // ======================================================================
      /// printout 
      std::ostream& fillStream ( std::ostream& s  ) const ;          // printout 
      /// printout using format 
      std::ostream& fillStream 
      ( std::ostream&      s    , 
        const std::string& fmt  ) const ;          // printout 
      /// conversion to the string 
      std::string   toString   ( ) const ;
      /// conversion to the string using format 
      std::string   toString   ( const std::string& fmt ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual value 
      double m_value ;                             //          the actual value 
      /// the associated covariance
      double m_cov2 ;                              // the associated covariance
      // ======================================================================
    };
    // ========================================================================
    /// addition 
    inline ValueWithError 
    operator+ ( const ValueWithError& v1 , const ValueWithError& v2 ) 
    { return v1.__add__ ( v2 ) ; }
    /// addition 
    inline ValueWithError 
    operator+ ( const ValueWithError& v1 , const double          v2 ) 
    { return v1.__add__ ( v2 ) ; }
    /// addition 
    inline ValueWithError 
    operator+ ( const double          v1 , const ValueWithError& v2 ) 
    { return v2 + v1 ; }
    // ========================================================================
    /// subtraction 
    inline ValueWithError 
    operator- ( const ValueWithError& v1 , const ValueWithError& v2 ) 
    { return v1.__sub__ ( v2 ) ; }
    /// subtraction 
    inline ValueWithError 
    operator- ( const ValueWithError& v1 , const double          v2 ) 
    { return v1.__sub__ ( v2 ) ; }
    /// subtraction 
    inline ValueWithError 
    operator- ( const double          v1 , const ValueWithError& v2 ) 
    { return v2.__rsub__ ( v1 ) ; }
    // ========================================================================
    /// multiplication 
    inline ValueWithError 
    operator* ( const ValueWithError& v1 , const ValueWithError& v2 ) 
    { return v1.__mul__ ( v2 ) ; }
    /// multiplication 
    inline ValueWithError 
    operator* ( const ValueWithError& v1 , const double          v2 ) 
    { return v1.__mul__ ( v2 ) ; }
    /// multiplication 
    inline ValueWithError 
    operator* ( const double          v1 , const ValueWithError& v2 ) 
    { return v2 * v1 ; }
    // ========================================================================
    /// division
    inline ValueWithError 
    operator/ ( const ValueWithError& v1 , const ValueWithError& v2 ) 
    { return v1.__div__ ( v2 ) ; }
    /// subtraction 
    inline ValueWithError 
    operator/ ( const ValueWithError& v1 , const double          v2 ) 
    { return v1.__div__ ( v2 ) ; }
    /// subtraction 
    inline ValueWithError 
    operator/ ( const double          v1 , const ValueWithError& v2 ) 
    { return v2.__rdiv__ ( v1 ) ; }
    // ========================================================================
    inline std::ostream& operator<< 
      ( std::ostream& s , const ValueWithError& v ) 
    { return v.fillStream ( s ) ; }
    // ========================================================================
    /// evaluate the mean of a and b 
    inline ValueWithError mean
    ( const ValueWithError& a , 
      const ValueWithError& b ) { return a.mean ( b ) ; }
    // ========================================================================
    /// evaluate chi2 
    inline double chi2
    ( const ValueWithError& a , 
      const ValueWithError& b ) { return a.chi2 ( b ) ; }
    /// evaluate chi2 
    inline double chi2
    ( const ValueWithError& a , 
      const double          b ) { return a.chi2 ( b ) ; }
    /// evaluate chi2 
    inline double chi2
    ( const double          b ,
      const ValueWithError& a ) { return a.chi2 ( b ) ; }
    // ========================================================================
    /// evaluate the "fraction"  a/(a+b)
    inline ValueWithError frac 
    ( const ValueWithError& a , 
      const ValueWithError& b ) { return a.frac ( b ) ; }
    /// evaluate the "fraction"  a/(a+b)
    inline ValueWithError frac 
    ( const ValueWithError& a , 
      const double          b ) { return a.frac ( b ) ; }
    /// evaluate the "fraction"  a/(a+b)
    inline ValueWithError frac 
    ( const double          a , 
      const ValueWithError& b ) { return frac ( ValueWithError ( a ) , b ) ; }
    // ========================================================================
    /// evaluate the "asymmetry"  (a-b)/(a+b)
    inline ValueWithError asym 
    ( const ValueWithError& a , 
      const ValueWithError& b ) { return a.asym ( b ) ; }
    /// evaluate the "asymmetry"  (a-b)/(a+b)
    inline ValueWithError asym 
    ( const ValueWithError& a , 
      const double          b ) { return a.asym ( b ) ; }
    /// evaluate the "asymmetry"  (a-b)/(a+b)
    inline ValueWithError asym 
    ( const double          a , 
      const ValueWithError& b ) { return asym ( ValueWithError ( a ) , b ) ; }
    // ========================================================================
    /** evaluate abs(a) 
     *  @param a (INPUT) the value 
     *  @return the absolute value 
     */
    GAUDI_API
    ValueWithError abs 
    ( const ValueWithError& a ) ;
    // ========================================================================    
    /** evaluate the binomial efficiency for Bernulli scheme 
     *  @param n (INPUT) number of 'success' 
     *  @param N (INPUT) total number 
     *  @return the binomial efficiency 
     */
    GAUDI_API
    ValueWithError binomEff   
    ( const size_t n , 
      const size_t N ) ;
    // ========================================================================
    /** evaluate pow(a,b)
     *  @param a (INPUT) the base 
     *  @param b (INPUT) the exponent 
     *  @return the <c>a</c> raised to power <c>b</b> 
     *  @warning invalid and small covarinaces are ignored 
     */
    GAUDI_API
    ValueWithError pow 
    ( const ValueWithError& a , 
      const int             b ) ;
    // ========================================================================    
    /** evaluate pow(a,b)
     *  @param a (INPUT) the base 
     *  @param b (INPUT) the exponent 
     *  @return the <c>a</c> raised to power <c>b</b> 
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError pow 
    ( const ValueWithError& a , 
      const double          b ) ;
    // ========================================================================
    /** evaluate pow(a,b)
     *  @param a (INPUT) the base 
     *  @param b (INPUT) the exponent 
     *  @return the <c>a</c> raised to power <c>b</b> 
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError pow 
    ( const int             a , 
      const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate pow(a,b)
     *  @param a (INPUT) the base 
     *  @param b (INPUT) the exponent 
     *  @return the <c>a</c> raised to power <c>b</b> 
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError pow 
    ( const double          a , 
      const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate pow(a,b)
     *  @param a (INPUT) the base 
     *  @param b (INPUT) the exponent 
     *  @return the <c>a</c> raised to power <c>b</b> 
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError pow 
    ( const ValueWithError& a , 
      const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate exp(b)
     *  @param b (INPUT) the exponent 
     *  @return the <c>e</c> raised to power <c>b</b> 
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError exp
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate log(b)
     *  @param b (INPUT) the parameter 
     *  @return logarithm
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError log
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate log10(b)
     *  @param b (INPUT) the parameter 
     *  @return logarithm on base 10 
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError log10
    ( const ValueWithError& b ) ;
    // ========================================================================    
  } //                                             end of namespace Gaudi::Math 
  // ==========================================================================
  namespace Parsers 
  {
    // ========================================================================
    /** parse the input string into the result 
     * 
     *  The valid representations:
     *
     *     -   value 
     *     - ( value ) 
     *     - ( value ,  error ) 
     *     - ( value +- error ) 
     *     - ( value ;  error ) 
     *
     *  @param result (output) the result 
     *  @param input  (input) input string
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-13
     */     
    GAUDI_API StatusCode 
    parse ( Gaudi::Math::ValueWithError& result , 
            const std::string&           input  ) ;
    // ========================================================================
    /** parse the input string into the result 
     *  @param result (output) the result 
     *  @param input  (input) input string
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-13
     */     
    GAUDI_API StatusCode 
    parse ( std::vector<Gaudi::Math::ValueWithError>& result , 
            const std::string&                        input  ) ;
    // ========================================================================
  } //                                          end of namespace Gaudi::Parsers 
  // ==========================================================================
} //                                                    end of namespace  Gaudi
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_ERRORS_H
// ============================================================================
