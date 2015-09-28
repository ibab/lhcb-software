// $Id$
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
      typedef std::vector<ValueWithError>                          Vector     ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the value and covariance 
      ValueWithError ( const double value      = 0 , 
                       const double covariance = 0 ) ;
      // ======================================================================
      /** constructor from the (value,error)-pair 
       *   - first  element is "value" 
       *   - second element is "error" 
       *  @param pair_  (value,error)-pair 
       */
      ValueWithError ( const std::pair<double,double>& pair_ ) ;
      // ======================================================================
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
      void setValue      ( const double v ) {  m_value = v   ; }
      void setCov2       ( const double c ) {  m_cov2  = c   ; }
      void setCovariance ( const double c ) { setCov2  ( c ) ; }
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
      /** get ``residual''
       *  defined as  signed \f$\sqrt \chi^2 \f$
       */
      double residual ( const ValueWithError& right ) const ;
      /** get ``residual''
       *  defined as  signed \f$\sqrt \chi^2 \f$
       */
      double residual ( const double          right ) const ;
      /** get Kullback-Liebler divergency 
       *  @return KL-divergency for valid arguments, -1 otherwise
       */
      double kullback ( const ValueWithError& right ) const ;
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
    public: // NaN and Finite 
      // ======================================================================
      ///  finite ? 
      bool isfinite () const ;      
      ///  normal ? 
      bool isnormal () const ;      
      ///  check for NaN
      bool isnan    () const ;
      ///  check for inf
      bool isinf    () const ;
      // ======================================================================
    public: // good ?
      // ======================================================================
      /// check for goodness: finite values and non-negative covariance 
      bool isgood   () const ;
      /// check for goodness: finite values and non-negative covariance 
      bool good     () const { return isgood   () ; }
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
      ValueWithError __exp__    () const ;
      /// expm1(me) 
      ValueWithError __expm1__  () const ;
      /// log(me) 
      ValueWithError __log__    () const ;
      /// log10(me) 
      ValueWithError __log10__  () const ;
      /// log1p(me) 
      ValueWithError __log1p__  () const ;
      /// sqrt(me) 
      ValueWithError __sqrt__   () const ;
      /// sqrt(me) 
      ValueWithError __cbrt__   () const ;
      /// sin(me) 
      ValueWithError __sin__    () const ;
      /// cos(me) 
      ValueWithError __cos__    () const ;
      /// tan(me) 
      ValueWithError __tan__    () const ;
      /// sinh(me) 
      ValueWithError __sinh__   () const ;
      /// cosh(me) 
      ValueWithError __cosh__   () const ;
      /// tanh(me) 
      ValueWithError __tanh__   () const ;
      /// erf 
      ValueWithError __erf__    () const ;
      /// erf 
      ValueWithError __erfc__   () const ;
      /// asin 
      ValueWithError __asin__   () const ;
      /// acos
      ValueWithError __acos__   () const ;
      /// atan
      ValueWithError __atan__   () const ;
      /// asinh 
      ValueWithError __asinh__  () const ;
      /// acosh
      ValueWithError __acosh__  () const ;
      /// atanh
      ValueWithError __atanh__  () const ;
      /// tgamma 
      ValueWithError __tgamma__ () const ;
      /// lgamma 
      ValueWithError __lgamma__ () const ;
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
    } ;
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
    /** get Kullback-Liebler divergency 
     *  return the divergency for valid arguments, -1 otherwise
     */
    inline double kullback 
    ( const ValueWithError& a , 
      const ValueWithError& b ) { return a.kullback ( b ) ; }
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
     *  @param n_success (INPUT) number of 'success' 
     *  @param N_total   (INPUT) total number 
     *  @return the binomial efficiency 
     */
    GAUDI_API
    ValueWithError binomEff   
    ( const size_t n_success , 
      const size_t N_total   ) ;
    // ========================================================================
    /** evaluate the binomial efficiency interval using Wilson's prescription
     *  @param n_success (INPUT) number of 'success' 
     *  @param N_total   (INPUT) total number 
     *  @return the binomial efficiency 
     */
    GAUDI_API
    ValueWithError wilsonEff   
    ( const size_t n_success , 
      const size_t N_total   ) ;
    // ========================================================================
    /** evaluate the binomial efficiency interval 
     *  using Agresti-Coull's prescription
     *  @param n_success (INPUT) number of 'success' 
     *  @param N_total   (INPUT) total number 
     *  @return the binomial efficiency 
     */
    GAUDI_API
    ValueWithError agrestiCoullEff   
    ( const size_t n_success , 
      const size_t N_total   ) ;
    // ========================================================================
    /** simple evaluation of efficiency from statistically independend
     * "exclusive" samples "accepted" and "rejected"
     *  \f$ \varepsilon = \frac{1}{ 1 + \frac{N_{rejected}}{N_accepted}}\f$ 
     *  @param accepted  (IN) accepted sample 
     *  @param rejected  (IN) rejected sample 
     *  @return the binomial efficiency 
     *  @see Gaudi::Math::binomEff2
     */
    GAUDI_API 
    ValueWithError exclusiveEff
    ( const ValueWithError& accepted , 
      const ValueWithError& rejected ) ;
    // ========================================================================
    /** evaluate the binomial efficiency for Bernulli scheme with weights 
     *  \f[ R = \frac{N_acc}{N_tot} = \frac{N_acc}{N_acc+N_rej} = 
     *          \left( 1 + \frac{N_rej}{N_acc}\right)^{-1} \f]
     *  @param nAccepted (INPUT) number of accepted (weighted) events 
     *  @param nRejected (INPUT) number of rejected (weighted) events 
     *  @return the binomial efficiency 
     *  @see Gaudi::Math::exclusiveEff 
     */
    GAUDI_API
    ValueWithError binomEff2   
    ( const ValueWithError& nAccepted , 
      const ValueWithError& nRejected ) ;
    // ========================================================================
    /** simple evaluation of efficiency using Zech's prescription 
     *  @param n_success (IN) accepted sub-sample 
     *  @param N_total   (IN) total     sample 
     */
    GAUDI_API 
    ValueWithError zechEff
    ( const ValueWithError& n_success , 
      const ValueWithError& N_total   ) ;
    // ========================================================================
    /** calculate the ratio of weighted to unweighted statistic 
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
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date 2014-01-25
     */
    GAUDI_API 
    ValueWithError effJackknife  
    ( const ValueWithError& nWeighted , 
      const unsigned long   n         ) ;  
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
    /** evaluate expm1(b)
     *  @param b (INPUT) the exponent 
     *  @return  expm1(b) 
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError expm1
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
    /** evaluate log1p(b)
     *  @param b (INPUT) the parameter 
     *  @return  log1p(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError log1p
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate sqrt(b)
     *  @param b (INPUT) the parameter 
     *  @return  sqrt(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError sqrt
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate cbrt(b)
     *  @param b (INPUT) the parameter 
     *  @return  cbrt(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError cbrt
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate sin(b)
     *  @param b (INPUT) the parameter 
     *  @return  sin(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError sin 
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate cos(b)
     *  @param b (INPUT) the parameter 
     *  @return  cos(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError cos 
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate tan(b)
     *  @param b (INPUT) the parameter 
     *  @return  tan(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError tan 
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate sinh(b)
     *  @param b (INPUT) the parameter 
     *  @return  sinh(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError sinh 
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate cosh(b)
     *  @param b (INPUT) the parameter 
     *  @return  cosh(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError cosh 
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate tanh(b)
     *  @param b (INPUT) the parameter 
     *  @return  tanh(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError tanh 
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate erf(b)
     *  @param b (INPUT) the parameter 
     *  @return  erf(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError erf
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate erfc(b)
     *  @param b (INPUT) the parameter 
     *  @return  erfc(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError erfc
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate asin(b)
     *  @param b (INPUT) the parameter 
     *  @return  asin(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError asin
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate acos(b)
     *  @param b (INPUT) the parameter 
     *  @return  acos(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError acos
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate atan(b)
     *  @param b (INPUT) the parameter 
     *  @return  atan(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError atan
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate asinh(b)
     *  @param b (INPUT) the parameter 
     *  @return  asinh(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError asinh
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate acosh(b)
     *  @param b (INPUT) the parameter 
     *  @return  acosh(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError acosh
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate atanh(b)
     *  @param b (INPUT) the parameter 
     *  @return  atanh(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError atanh
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate tgamma(b)
     *  @param b (INPUT) the parameter 
     *  @return  Gamma(b)
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError tgamma
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate lgamma(b)
     *  @param b (INPUT) the parameter 
     *  @return  log(Gamma(b))
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API
    ValueWithError lgamma
    ( const ValueWithError& b ) ;
    // ========================================================================    
    /** evaluate <code>hypot(x,y)</code>
     *  \f$ \sqrt( x^2 + y^2 ) \f$
     *   @param x (INPUT) the first parameter
     *   @param y (INPUT) the second parameter
     *   @return the valueof <code>hypot</code> function
     */
    GAUDI_API 
    ValueWithError  hypot
    ( const ValueWithError& x , 
      const ValueWithError& y ) ;
    // ========================================================================    
    /// check for NaN
    inline bool isnan    ( const ValueWithError& v ) { return v.isnan    () ; }
    /// finite ?
    inline bool isfinite ( const ValueWithError& v ) { return v.isfinite () ; }
    /// infinte ? 
    inline bool isinf    ( const ValueWithError& v ) { return v.isinf    () ; }    
    /// normal ?
    inline bool isnormal ( const ValueWithError& v ) { return v.isnormal () ; }    
    /// check for goodness 
    inline bool isgood   ( const ValueWithError& v ) { return v.isgood   () ; }    
    /// check for goodness 
    inline bool good     ( const ValueWithError& v ) { return v.good     () ; }    
    // ========================================================================    
    /** Does this object represent natual number?
     *  - non-negative integer value 
     *  - cov2 == value  or cov2 == 0 
     */
    GAUDI_API 
    bool natural_number ( const ValueWithError& value ) ;
    // ========================================================================
    /** Does this object represent natual entry in histogram
     *  - non-negative integer value 
     *  - cov2 == value or ( 0 == value && 1 == cov2 ) 
     */
    GAUDI_API 
    bool natural_entry  ( const ValueWithError& value ) ;
    // ========================================================================
    /** make a sum two elements taking into account the 
     *  correlation coefficient  
     *  @param a  (input) the first value 
     *  @param b  (input) the second value 
     *  @param c  (input) the correlation coefficient
     *  @return a+b 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2012-11-09
     */
    GAUDI_API 
    ValueWithError  sum 
    ( const ValueWithError& a     , 
      const ValueWithError& b     , 
      const double          c = 0 ) ;
    // ========================================================================
    /** make a sum two elements taking into account the 
     *  correlation coefficient  
     *  @param a  (input) the first value 
     *  @param b  (input) the second value 
     *  @param c  (input) the correlation coefficient
     *  @return a+b 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2012-11-09
     */
    GAUDI_API 
    ValueWithError  sum2 
    ( const ValueWithError& a     , 
      const ValueWithError& b     , 
      const double          c = 0 ) ;
    // ========================================================================
    /** make a subtraction of two elements taking into account the 
     *  correlation coefficient  
     *  @param a  (input) the first value 
     *  @param b  (input) the second value 
     *  @param c  (input) the correlation coefficient
     *  @return a-b 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2012-11-09
     */
    GAUDI_API 
    ValueWithError  subtract
    ( const ValueWithError& a     , 
      const ValueWithError& b     , 
      const double          c = 0 ) ;
    // ========================================================================
    /** make a  multiplication two elements taking into account the 
     *  correlation coefficient  
     *  @param a  (input) the first value 
     *  @param b  (input) the second value 
     *  @param c  (input) the correlation coefficient
     *  @return a*b 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2012-11-09
     */
    GAUDI_API 
    ValueWithError  multiply
    ( const ValueWithError& a     , 
      const ValueWithError& b     , 
      const double          c = 0 ) ;
    // ========================================================================
    /** make a division of two elements taking into account the 
     *  correlation coefficient  
     *  @param a  (input) the first value 
     *  @param b  (input) the second value 
     *  @param c  (input) the correlation coefficient
     *  @return a/b 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2012-11-09
     */
    GAUDI_API 
    ValueWithError  divide
    ( const ValueWithError& a     , 
      const ValueWithError& b     , 
      const double          c = 0 ) ;
    // ========================================================================
    /** simple linear interpolation 
     *  @param  x  the point to evaluate the function 
     *  @param  x0 the abscissa for the first  point
     *  @param  y0 the function value for the first  point
     *  @param  x1 the abscissa for the second point
     *  @param  y1 the function value for the second point
     *  @return linear interpolation at point x
     */
    GAUDI_API 
    ValueWithError interpolate_1D 
    ( const double          x  , 
      const double          x0 ,
      const ValueWithError& y0 , 
      const double          x1 ,
      const ValueWithError& y1 ) ;    
    // ========================================================================
    /** simple (bi)linear interpolation 
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
    GAUDI_API 
    ValueWithError interpolate_2D 
    ( const double          x   , 
      const double          y   , 
      const double          x0  ,
      const double          x1  ,
      const double          y0  ,
      const double          y1  ,
      const ValueWithError& v00 , 
      const ValueWithError& v01 , 
      const ValueWithError& v10 , 
      const ValueWithError& v11 ) ;
    // ========================================================================
    /** get the sum of the vector 
     *  @param vct the vecor
     *  @param ini the intial value 
     *  @return the sum over the vector 
     */
    GAUDI_API 
    ValueWithError sum 
    ( const std::vector<ValueWithError>& vct                    , 
      ValueWithError                     ini = ValueWithError() ) ;
    // ========================================================================    
    /** get the sum of the vector 
     *  @param vct the vecor
     *  @param ini the intial value 
     *  @return the sum over the vector 
     */
    GAUDI_API 
    ValueWithError accumulate 
    ( const std::vector<ValueWithError>& vct                    , 
      ValueWithError                     ini = ValueWithError() ) ;
    // ========================================================================    
    /** get the sum of the absolute values
     *  @param vct the vecor
     *  @param ini the intial value 
     *  @return the sum over the vector 
     */
    GAUDI_API 
    ValueWithError abssum ( const std::vector<ValueWithError>& vct ) ;
    // ========================================================================    
    /** get the sum of the absolute values
     *  @param vct the vecor
     *  @param ini the intial value 
     *  @return the sum over the vector 
     */
    inline 
    ValueWithError sumabs ( const std::vector<ValueWithError>& vct )
    { return abssum ( vct )  ; }
    // ========================================================================    
    /** evaluate polynomial
     *  \f$f(x) = a_0 + a_1x + a_2x^2 + ... + a_{n-1}x^{n-1} + a_nx^n\f$
     *  such as \f$f(0) = a_0 \f$      
     *  using Horner rule
     *  @param poly  INPUT the coefficients
     *  @param x     INPUT argument 
     *  @return value of polynomial
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API 
    ValueWithError horner_a0 ( const std::vector<double>& poly ,
                               const ValueWithError&      x    ) ;
    // ======================================================================
    /** evaluate polynomial
     *  \f$f(x) = a_0x^n + a_1x^{n-1}+ ... + a_{n-1}x + a_n\f$, 
     *  such as \f$f(0) = a_n \f$      
     *  using Horner rule
     *  @param poly  INPUT the coefficients 
     *  @param x     INPUT argument 
     *  @return value of polynomial
     *  @warning invalid and small covariances are ignored 
     */
    GAUDI_API 
    ValueWithError horner_aN ( const std::vector<double>& poly ,
                               const ValueWithError&      x    ) ;
    // ========================================================================    
    /// the output operator for the vector 
    GAUDI_API 
    std::ostream& operator<<
    ( std::ostream& s , 
      const std::vector<Gaudi::Math::ValueWithError>& v ) ;
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
namespace Gaudi
{
  // ==========================================================================
  namespace Utils 
  {
    // ========================================================================
    inline 
    std::ostream& toStream
    ( const Gaudi::Math::ValueWithError& obj , std::ostream& s )
    { return obj.fillStream ( s ) ; }
    // =======================================================================
    /// print the vector
    GAUDI_API 
    std::ostream& toStream
    ( const std::vector<Gaudi::Math::ValueWithError>& obj , std::ostream& s ) ;
    // =======================================================================
    inline 
    std::string toString ( const Gaudi::Math::ValueWithError& obj )
    { return obj.toString () ; }
    // ========================================================================
  } //                                            end of namespace Gaudi::Utils 
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_ERRORS_H
// ============================================================================
