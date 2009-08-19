// $Id: ValueWithError.h,v 1.3 2009-08-19 14:32:33 ibelyaev Exp $
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
    class ValueWithError 
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
      // ======================================================================
    public: // trivial accessors 
      // ======================================================================
      /// get the value 
      double value      () const { return m_value ; }
      /// get the covariance 
      double cov2       () const { return m_cov2  ; }
      /// get the covariance 
      double covariance () const { return m_cov2  ; }
      /// get the error 
      double error      () const ;
      // ======================================================================
    public: // setters 
      // ======================================================================
      void setValue      ( const double v ) { m_value = v      ; }
      void setCovariance ( const double c ) { m_cov2  = c      ; }
      void setError      ( const double e ) { m_cov2  = e * e  ; }
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
      // ======================================================================
    public:
      // ======================================================================
      /// printout 
      std::ostream& fillStream ( std::ostream& s ) const ;          // printout 
      /// conversion to the string 
      std::string   toString   () const ;           // conversion to the string 
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
  } //                                             end of namespace Gaudi::Math 
  // ==========================================================================
} //                                                    end of namespace  Gaudi
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_ERRORS_H
// ============================================================================
