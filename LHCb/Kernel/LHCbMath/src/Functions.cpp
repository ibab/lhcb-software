// $Id$ 
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <cmath>
#include <climits>
#include <iostream>
#include <complex>
#include <algorithm>
#include <numeric>
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Functions.h"
#include "LHCbMath/LHCbMath.h"
#include "LHCbMath/Power.h"
// ============================================================================
// GSL 
// ============================================================================
#include "gsl/gsl_errno.h"
#include "gsl/gsl_sf_exp.h"
#include "gsl/gsl_sf_log.h"
#include "gsl/gsl_sf_erf.h"
#include "gsl/gsl_sf_gamma.h"
#include "gsl/gsl_integration.h"
// ============================================================================
// ROOT 
// ============================================================================
#include "TMath.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
// Local
// ============================================================================
#include "GSL_sentry.h"
// ============================================================================
/** @file 
 *  Implementation file for functions from the file LHCbMath/Functions.h
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-04-19
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $author$
 */
// ============================================================================
// Rho-functions from Jackson 
// ============================================================================
/* the simplest function: constant 
 *  @see Gaudi::Math::BreitWigner
 *  @see Gaudi::Math::BreitWigner::rho_fun
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-11-30
 */
// ============================================================================
double Gaudi::Math::Jackson::jackson_0
( double /* m  */ , 
  double /* m0 */ , 
  double /* m1 */ , 
  double /* m2 */ ) { return 1 ; }
// ============================================================================
/* the simple function for \f$ 1^- \rightarrow 0^- 0^- \f$, l = 1 
 *  \f$\rho(\omega)= \omega^{-1}\f$
 *  @see Gaudi::Math::BreitWigner
 *  @see Gaudi::Math::BreitWigner::rho_fun
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-11-30
 */
// ============================================================================
double Gaudi::Math::Jackson::jackson_A2
( double    m     , 
  double /* m0 */ , 
  double /* m1 */ , 
  double /* m2 */ ) { return 1./m ; } ;
// ============================================================================
/*  the simple function for \f$ 1^- \rightarrow 0^- 1^- \f$, l = 1 
 *  \f$\rho(\omega)= \omega \f$
 *  @see Gaudi::Math::BreitWigner
 *  @see Gaudi::Math::BreitWigner::rho_fun
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-11-30
 */
// ============================================================================
double Gaudi::Math::Jackson::jackson_A3
( double    m     , 
  double /* m0 */ , 
  double /* m1 */ , 
  double /* m2 */ ) { return m ; } ;
// ============================================================================
/*  the simple function for 
 *  \f$ \frac{3}{2}^+ \rightarrow \frac{1}{2}^+ 0^- \f$, l = 1 
 *  $\rho(\omega)= \frac{ ( \omega + M )^2 - m^2 }{ \omega^2} \f$
 *  @see Gaudi::Math::BreitWigner
 *  @see Gaudi::Math::BreitWigner::rho_fun
 *  @param m the invariant mass 
 *  @param m1 the invariant mass of the first  (spinor) particle  
 *  @param m2 the invariant mass of the secodn (scalar) particle  
 *  @return the value of rho-function
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-11-30
 */
// ============================================================================
double Gaudi::Math::Jackson::jackson_A4
( double    m     , 
  double /* m0 */ , 
  double    m1    , 
  double    m2    ) 
{
  const double a = m + m1 ;
  //
  return ( a * a  - m2 * m2 ) / ( m * m ) ;
}
// ============================================================================
/*  the simple function for 
 *  \f$ \frac{3}{2}^- \rightarrow \frac{1}{2}^+ 0^- \f$, l = 2 
 *  $\rho(\omega)= \left[ ( \omega + M )^2 - m^2 \right]^{-1} \f$
 *  @see Gaudi::Math::BreitWigner
 *  @see Gaudi::Math::BreitWigner::rho_fun
 *  @param m the invariant mass 
 *  @param m1 the invariant mass of the first  (spinor) particle  
 *  @param m2 the invariant mass of the secodn (scalar) particle  
 *  @return the value of rho-function
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-11-30
 */
// ============================================================================
double Gaudi::Math::Jackson::jackson_A5
( double    m     , 
  double /* m0 */ , 
  double    m1    , 
  double    m2    ) 
{
  const double a = m + m1 ;
  //
  return 1 / ( a * a  - m2 * m2 ) ;  
}
// ============================================================================
/*  the simple function for \f$\rho^- \rightarrow \pi^+ \pi^-\f$
 *  \f$ 1- \rightarrow 0^- 0^- \f$, l = 1 
 *  $\rho(\omega)= \left[ q_0^2 + q^2 \right]^{-1}f$
 *  @see Gaudi::Math::BreitWigner
 *  @see Gaudi::Math::BreitWigner::rho_fun
 *  @param m the invariant mass 
 *  @param m the nominam   mass 
 *  @return the value of rho-function
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-11-30
 */
// ============================================================================
double Gaudi::Math::Jackson::jackson_A7
( double    m   , 
  double    m0  , 
  double    m1  , 
  double    m2  ) 
{
  //
  const double q  = Gaudi::Math::PhaseSpace2::q ( m  , m1 , m2 ) ;
  const double q0 = Gaudi::Math::PhaseSpace2::q ( m0 , m1 , m2 ) ;
  //
  if ( 0 >= q && 0 >= q0 ) { return 1 ; }
  //
  return 1. / ( q * q + q0 * q0 ) ;
}
// ============================================================================
namespace 
{
  // ==========================================================================
  typedef Gaudi::Math::GSL::GSL_Error_Handler Sentry ;
  // ==========================================================================
  /// get GSL-workspace
  gsl_integration_workspace* workspace 
  ( const Gaudi::Math::WorkSpace& ws )  
  {
    void* _ws =  ws.workspace() ;
    return (gsl_integration_workspace*) _ws ;
  }  
  // ==========================================================================
  BOOST_STATIC_ASSERT( std::numeric_limits<float>::is_specialized  ) ;
  // ==========================================================================
  /** @var s_INFINITY 
   *  representation of positive INFINITY 
   */
  const double s_INFINITY = 0.5 * std::numeric_limits<float>::max()  ;
  // ==========================================================================
  /** @var s_PRECISION 
   *  the default precision for various calculations, 
   *       in particular GSL integration
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23      
   */   
  const double s_PRECISION     = 1.e-8 ;
  // ==========================================================================
  /** @var s_PRECISION_TAIL
   *  the low-relative precision for tails in GSL integration 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23      
   */
  const double s_PRECISION_TAIL = 1.e-4 ;  
  // ===========================================================================
  /** @var s_SIZE 
   *  the workspace size parameter for GSL-integration 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23      
   */
  const std::size_t s_SIZE = 200 ;
  // ==========================================================================
  /** @var s_TRUNC
   *  trucnkating parameter for CrystalBall-functions 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23      
   */
  const double      s_TRUNC = 15.0 ;
  // ==========================================================================
  // Chebyshev & Co
  // ==========================================================================
  Gaudi::Math::Chebyshev_<2> s_c2 ;
  Gaudi::Math::Chebyshev_<3> s_c3 ;
  Gaudi::Math::Chebyshev_<4> s_c4 ;
  Gaudi::Math::Chebyshev_<5> s_c5 ;
  Gaudi::Math::Chebyshev_<6> s_c6 ;
  Gaudi::Math::Chebyshev_<7> s_c7 ;
  Gaudi::Math::Chebyshev_<8> s_c8 ;
  Gaudi::Math::Chebyshev_<9> s_c9 ;
  // ==========================================================================
  inline double _chebyshev ( const unsigned int N , const double x ) 
  {
    switch ( N ) 
    {
    case 0  : return        1   ; // 
    case 1  : return        x   ; // 
    case 2  : return s_c2 ( x ) ; // 
    case 3  : return s_c3 ( x ) ; // 
    case 4  : return s_c4 ( x ) ; // 
    case 5  : return s_c5 ( x ) ; // 
    case 6  : return s_c6 ( x ) ; // 
    case 7  : return s_c7 ( x ) ; // 
    case 8  : return s_c8 ( x ) ; //
    case 9  : return s_c9 ( x ) ; //
      //
    default : break ;
    }
    //
    return 2 * x * _chebyshev ( N - 1 , x ) - _chebyshev ( N - 2 , x ) ;
  }
  // ==========================================================================
  // Legendre & Co
  // ==========================================================================
  Gaudi::Math::Legendre_<2>  s_l2 ;
  Gaudi::Math::Legendre_<3>  s_l3 ;
  Gaudi::Math::Legendre_<4>  s_l4 ;
  Gaudi::Math::Legendre_<5>  s_l5 ;
  Gaudi::Math::Legendre_<6>  s_l6 ;
  Gaudi::Math::Legendre_<7>  s_l7 ;
  Gaudi::Math::Legendre_<8>  s_l8 ;
  Gaudi::Math::Legendre_<9>  s_l9 ;
  // ==========================================================================
  inline double _legendre  ( const unsigned int N , const double x ) 
  {
    //
    switch ( N ) 
    {
    case 0  : return        1   ; // 
    case 1  : return        x   ; // 
    case 2  : return s_l2 ( x ) ; // 
    case 3  : return s_l3 ( x ) ; // 
    case 4  : return s_l4 ( x ) ; // 
    case 5  : return s_l5 ( x ) ; // 
    case 6  : return s_l6 ( x ) ; // 
    case 7  : return s_l7 ( x ) ; // 
    case 8  : return s_l8 ( x ) ; //
    case 9  : return s_l9 ( x ) ; //
      //
    default : break ;
    }
    //
    return 
      ( ( 2 * N - 1 ) * x * _legendre ( N - 1 , x ) -
        (     N - 1 ) *     _legendre ( N - 2 , x ) ) / N ;
  }
  // ==========================================================================
  // Hermite & Co
  // ==========================================================================
  Gaudi::Math::Hermite_<2>  s_h2 ;
  Gaudi::Math::Hermite_<3>  s_h3 ;
  Gaudi::Math::Hermite_<4>  s_h4 ;
  Gaudi::Math::Hermite_<5>  s_h5 ;
  Gaudi::Math::Hermite_<6>  s_h6 ;
  Gaudi::Math::Hermite_<7>  s_h7 ;
  Gaudi::Math::Hermite_<8>  s_h8 ;
  Gaudi::Math::Hermite_<9>  s_h9 ;
  // ==========================================================================
  inline double _hermite  ( const unsigned int N , const double x ) 
  {
    //
    switch ( N ) 
    {
    case 0  : return        1   ; // 
    case 1  : return        x   ; // 
    case 2  : return s_h2 ( x ) ; // 
    case 3  : return s_h3 ( x ) ; // 
    case 4  : return s_h4 ( x ) ; // 
    case 5  : return s_h5 ( x ) ; // 
    case 6  : return s_h6 ( x ) ; // 
    case 7  : return s_h7 ( x ) ; // 
    case 8  : return s_h8 ( x ) ; //
    case 9  : return s_h9 ( x ) ; //
      //
    default : break ;
    }
    //
    return 
      x * _hermite ( N - 1 , x ) - ( N - 1 ) * _hermite ( N - 2 , x ) ;
  }
  // ==========================================================================
  // Constants 
  // ==========================================================================
  /** @var s_SQRTPIHALF 
   *  helper constant \f$ \sqrt{\frac{\pi}{2}}\f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */   
  const double s_SQRTPIHALF = std::sqrt( M_PI_2 ) ;
  // ==========================================================================
  /** @var s_SQRT2PI 
   *  helper constant \f$ \sqrt{2\pi}\f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */   
  const double s_SQRT2PI    =       std::sqrt( 2 * M_PI ) ;
  // ===========================================================================
  /** @var s_HALFSQRTPI 
   *  helper constant \f$ \frac{\sqrt{\pi}}{2}\f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */   
  const double s_HALFSQRTPI = 0.5 * std::sqrt(     M_PI ) ;
  // ==========================================================================
  // Bukin & Co
  // ==========================================================================
  /** @var s_Bukin
   *  useful constant for Bukin's function 
   *  \f$ \sqrt{ 2 \log { 2 } } \f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */
  const double s_Bukin   = std::sqrt ( 2.0 * std::log ( 2.0 ) ) ;
  // ==========================================================================
  /** @var s_ln2
   *  useful constant for Bukin's function 
   *  \f$ \log { 2 } \f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */
  const double s_ln2 = std::log ( 2.0 ) ;
  // ==========================================================================
  /// equality criteria for doubles 
  LHCb::Math::Equal_To<double> s_equal ;       // equality criteria for doubles 
  // ==========================================================================
  /** helper function for itegration of Bukin's function
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */
  double bukin_GSL ( double x , void* params )  
  {
    const Gaudi::Math::Bukin* bukin = (Gaudi::Math::Bukin*) params ;
    return (*bukin)(x) ;
  }
  // ==========================================================================
  // Novosibirsk & Co
  // ==========================================================================
  /** @var s_Novosibirsk 
   *  useful constant for evaliuation of ``Novosibirsk'' function
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */
  const double s_Novosibirsk = std::sqrt ( std::log ( 4.0 ) ) ;
  // ==========================================================================
  /** helper function for itegration of Novosibirsk's function
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double novosibirsk_GSL ( double x , void* params )  
  {
    //
    const Gaudi::Math::Novosibirsk* novosibirsk = 
      (Gaudi::Math::Novosibirsk*) params ;
    //
    return (*novosibirsk)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of CrystalBall function 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2012-06-06
   */
  double crystalball_GSL ( double x , void* params )  
  {
    //
    const Gaudi::Math::CrystalBall* cb = 
      (Gaudi::Math::CrystalBall*) params ;
    //
    return (*cb)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of CrystalBall function 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2012-06-06
   */
  double crystalball2s_GSL ( double x , void* params )  
  {
    //
    const Gaudi::Math::CrystalBallDoubleSided* cb2s = 
      (Gaudi::Math::CrystalBallDoubleSided*) params ;
    //
    return (*cb2s)(x) ;
  }
  // ==========================================================================
  /** evaluate the helper function  \f[ f = \frac{\log{1+x}}{x} \f]
   *  it allows to calculate Bukin' function in efficient and regular way  
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  inline double x_log 
  ( const double x , double precision = s_PRECISION ) 
  {
    //
    if      ( s_equal   ( x , 0 )       ) { return 1 ; } // RETURN
    else if ( x <= -1                   ) { return 0 ; } // RETURN ?
    else if ( s_equal   ( x , -1 )      ) { return 0 ; } // RETURN ?
    else if ( std::fabs ( x     ) < 0.1 ) 
    {
      double result = 1.0 ;
      double delta  = x   ;
      //
      precision = std::fabs ( precision ) ;
      precision = std::min  ( precision , std::fabs ( s_PRECISION_TAIL ) ) ;
      if ( 0 > x ) { precision *= 0.1 ; }
      //
      unsigned int n = 2 ;
      do 
      {
        result += delta / n ;
        delta  *=       - x ; // note minus here         
        ++n                 ;
      }
      while ( std::fabs ( delta ) > precision && n < 1000 ) ;
      //
      return result ;                                         // RETURN
    }
    // 
    // the generic evaluation 
    //
    return std::log ( 1.0 + x ) / x ;
  }
  // ==========================================================================
  /** evaluate the helper function \f[ f = \frac{\sinh {x}{x} \f] 
   *  it allows to calculate Novosibirsk's function in efficient and regular way  
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23      
   */
  inline double x_sinh 
  ( const double x , double precision = s_PRECISION ) 
  {
    //
    if      ( s_equal   ( x , 0 )    ) { return 1 ; }
    else if ( std::fabs ( x ) < 0.1  ) 
    {
      double result = 1.0  ;
      double delta  = x    ;
      //
      precision = std::fabs ( precision ) ;
      precision = std::min  ( precision , std::fabs ( s_PRECISION_TAIL ) ) ;
      unsigned int n = 1 ;
      //
      do 
      {
        delta  *= x * x / ( n + 1 )  / ( n + 2 )  ;
        result += delta ;
        n      += 2 ;
      }
      while ( std::fabs ( delta ) > 0.1 * precision && n < 1000 ) ;
      //
      return result ;
    }
    //
    if ( 100 < std::fabs ( x )  ) { return  s_INFINITY ; }
    //
    // the generic evaluation 
    //
    return std::sinh ( x ) / x ;
  }
  // ==========================================================================
  /** the simple wrapper for the standard error function 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23      
   */
  double error_func ( const double x ) 
  {
    //
    if      ( -30 > x ) { return -1 ; }
    else if ( +30 < x ) { return  1 ; }
    //
    Sentry sentry ;
    //
    gsl_sf_result result ;
    const int ierror = gsl_sf_erf_e ( x , &result ) ;
    if ( ierror ) 
    { 
      if ( ierror == GSL_EDOM     ||    /* input domain error, e.g sqrt(-1)    */
           ierror == GSL_ERANGE   ||    /* output range error, e.g. exp(1e100) */
           ierror == GSL_EINVAL   ||    /* invalid argument supplied by user   */
           ierror == GSL_EUNDRFLW ||    /* underflow                           */
           ierror == GSL_EOVRFLW  ||    /* overflow                            */
           ierror == GSL_ELOSS    ||    /* loss of accuracy                    */
           ierror == GSL_EROUND    )    /* failed because of roundoff error    */
      {}
      else 
      {
        gsl_error ( "Error from erf_e function" ,
                    __FILE__ , __LINE__ , ierror ) ; 
      }
      //
      if      ( -15 > x ) { return -1 ; }
      else if (  15 < x ) { return  1 ; }
    } 
    //
    return result.val ;                    // RETURN 
  }
  // ==========================================================================
  /** the protected exponent 
   *  @author Vanya BELYAEV 
   */
  double my_exp ( const double arg ) 
  {
    // 
    Sentry sentry ;
    gsl_sf_result      result ;
    const int          ierror = gsl_sf_exp_e ( arg , &result ) ;
    //
    if ( ierror ) 
    { 
      if ( ierror == GSL_EDOM     ||    /* input domain error, e.g sqrt(-1)    */
           ierror == GSL_ERANGE   ||    /* output range error, e.g. exp(1e100) */
           ierror == GSL_EINVAL   ||    /* invalid argument supplied by user   */
           ierror == GSL_EUNDRFLW ||    /* underflow                           */
           ierror == GSL_EOVRFLW  ||    /* overflow                            */
           ierror == GSL_ELOSS    ||    /* loss of accuracy                    */
           ierror == GSL_EROUND    ) {} /* failed because of roundoff error    */
      else 
      {
        gsl_error ( "Error from exp_e function" ,
                    __FILE__ , __LINE__ , ierror ) ; 
      }
      //
      if      ( -100 > arg ) { return          0 ; }
      else if (  100 < arg ) { return s_INFINITY ; }
    }
    //
    return result.val ;
  }
  // ==========================================================================
  /** the protected logarithm  
   *  @author Vanya BELYAEV 
   */
  double my_log ( const double arg ) 
  {
    // 
    if ( 0 >= arg ) { return -s_INFINITY ; } // REUTRN
    //
    Sentry sentry ;
    gsl_sf_result result ;
    const int     ierror = gsl_sf_log_e ( arg , &result ) ;
    if ( ierror ) 
    { 
      if ( ierror == GSL_EDOM     ||    /* input domain error, e.g sqrt(-1)    */
           ierror == GSL_ERANGE   ||    /* output range error, e.g. exp(1e100) */
           ierror == GSL_EINVAL   ||    /* invalid argument supplied by user   */
           ierror == GSL_EUNDRFLW ||    /* underflow                           */
           ierror == GSL_EOVRFLW  ||    /* overflow                            */
           ierror == GSL_ELOSS    ||    /* loss of accuracy                    */
           ierror == GSL_EROUND    )    /* failed because of roundoff error    */
      {}
      else 
      {
        gsl_error ( "Error from exp_e function" ,
                    __FILE__ , __LINE__ , ierror ) ; 
      }
      //
      if      (  1.e-100 > arg  ) { return -s_INFINITY ; }
      else if (  1.e+100 < arg  ) { return  s_INFINITY ; }
      //
    }
    // 
    return result.val ;
  }
  // ==========================================================================
  /** helper function for itegration
   *  \f[ f =  \exp { \kappa x^2 + \xi x }  \f]
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */
  double gauss_GSL ( double x , void* params )
  {
    const double* gauss = (double*) params ;
    //
    const double kappa = gauss[0] ;
    const double xi    = gauss[1] ;
    //
    const double arg    = kappa * x * x + xi * x ;
    //
    return my_exp ( arg ) ;
  }
  // ==========================================================================
  /** get the gaussian integral:
   *  \f[ f = \int_a^b \exp { -\alpha x^2 + \beta x } \mathrm{d}x \f]
   *  @param alpha the alpha parameter 
   *  @param beta  the beta  parameter 
   *  @param a     the low  integration limit 
   *  @param b     the high integration limit
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23      
   */ 
  double gaussian_int
  ( const double alpha , 
    const double beta  , 
    const double a     , 
    const double b     ) 
  {
    //
    if      ( s_equal ( a , b ) ) { return  0.0                        ; }
    else if (           a > b   ) { return -1 * gaussian_int ( alpha , 
                                                               beta  , 
                                                               b     ,
                                                               a     ) ; }
    //
    // alpha ? 
    //
    if      ( s_equal ( alpha , 0 ) )      // the most trivial trivial case 
    {
      /// the most trivial trivial case 
      if     ( s_equal ( beta , 0 ) ) { return b - a ; }  // RETURN 
      /// get the simple exponential integral
      return ( my_exp ( beta * b ) - my_exp ( beta * a ) ) / beta ;
    }
    // it just the standard  error function 
    else if ( alpha > 0 )        // it just the standard  error function 
    {
      //
      const double alpha_sqrt = std::sqrt ( alpha ) ;
      //
      const double factor1 = my_exp ( 0.25 * beta * beta / alpha ) / alpha_sqrt ;
      const double factor  = factor1 * s_HALFSQRTPI ;
      // 
      const double root    = 0.5 * beta / alpha ;
      const double a_prime = alpha_sqrt * ( a - root ) ;
      const double b_prime = alpha_sqrt * ( b - root ) ;
      //
      return factor * ( error_func ( b_prime ) -  
                        error_func ( a_prime ) ) ;
    }
    //
    // use GSL to evaluate the integral
    Sentry sentry ;
    //
    gsl_function F            ;
    F.function = &gauss_GSL   ;
    double params[2]          ;
    params[0]  = -alpha       ;    // ATTENTION: minus sign here!
    params[1]  =  beta        ;
    F.params   = params       ;
    //
    //
    double result   = 1.0 ;
    double error    = 1.0 ;
    //
    Gaudi::Math::WorkSpace ws ;
    const int ierror = gsl_integration_qag 
      ( &F                ,            // the function 
        a     ,   b       ,            // low & high edges 
        s_PRECISION       ,            // absolute precision            
        s_PRECISION       ,            // relative precision 
        s_SIZE            ,            // size of workspace 
        GSL_INTEG_GAUSS31 ,            // integration rule  
        workspace ( ws )  ,            // workspace  
        &result           ,            // the result 
        &error            ) ;          // the error in result 
    //
    if ( ierror ) 
    { 
      gsl_error ( "Gaudi::Math::gaussian_int " , 
                  __FILE__ , __LINE__ , ierror ) ; 
    }
    //
    return result ;    
    //
  }
  // ==========================================================================
  /** get the gaussian integral:
   *  \f[ f = \int_a^{\inf} \exp { -\alpha x^2 + \beta x } \mathrm{d}x \f]
   *  @param alpha the alpha parameter
   *  @param beta  the beta  parameter 
   *  @param a     the low integration limit 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23      
   */ 
  double gaussian_int_R ( const double alpha , 
                          const double beta  , 
                          const double a     ) 
  {
    //
    if ( s_equal ( alpha , 0 ) ) 
    {
      // 
      if ( beta < 0 ) 
      {
        // get the exponential integral 
        return - my_exp ( beta * a ) / beta ;       // RETURN 
      }
      else                     { return s_INFINITY ; }  // RETURN 
    }
    else if ( alpha < 0 )      { return s_INFINITY ; }  // RETURN 
    //
    //
    const double alpha_sqrt = std::sqrt ( alpha ) ;
    //
    const double factor1 = my_exp ( 0.25 * beta * beta / alpha ) / alpha_sqrt ;
    const double factor  = factor1 * s_HALFSQRTPI ;
    //
    const double root    = 0.5 * beta / alpha ;
    const double a_prime = alpha_sqrt * ( a - root ) ;
    //
    return factor * ( 1  - error_func ( a_prime ) ) ;   
  }
  // ==========================================================================
  /** get the gaussian integral:
   *  \f[ f = \int_{-\inf}^{b} \exp { -\alpha x^2 + \beta x } \mathrm{d}x \f]
   *  @param alpha the alpha parameter
   *  @param beta  the beta  parameter 
   *  @param b     the high integration limit 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23      
   */ 
  double gaussian_int_L ( const double alpha , 
                          const double beta  , 
                          const double b     ) 
  {
    //
    if ( s_equal ( alpha , 0 ) ) 
    {
      // 
      if ( beta > 0 ) 
      {
        // get the exponential integral 
        return std::exp ( beta * b ) / beta ;           // RETURN 
      }
      else                     { return s_INFINITY ; }  // RETURN 
    }
    else if ( alpha < 0 )      { return s_INFINITY ; }  // RETURN 
    //
    //
    const double alpha_sqrt = std::sqrt ( alpha ) ;
    //
    const double factor1    = my_exp ( 0.25 * beta * beta / alpha ) / alpha_sqrt ;
    const double factor     = factor1 * s_HALFSQRTPI ;
    // 
    const double root       = 0.5 * beta / alpha ;
    const double b_prime    = alpha_sqrt * ( b - root ) ;
    //
    return factor * ( error_func ( b_prime ) + 1  ) ;
  }
  // ==========================================================================
  /** @var x_sqrt2 
   *  \f$\sqrt{2}\f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23      
   */ 
  const double s_sqrt2  = std::sqrt ( 2.0 ) ;
  // ==========================================================================
  /** @var x_sqrt2_i 
   *  \f$\frac{1}{\sqrt{2}}\f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23      
   */ 
  const double s_sqrt2_i = 1 / s_sqrt2 ;
  // ==========================================================================
  /** helper function for itegration of Gram-Charlier A function
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double gram_charlier_A_GSL ( double x , void* params )  
  {
    //
    const Gaudi::Math::GramCharlierA* gca = 
      (Gaudi::Math::GramCharlierA*) params ;
    //
    return (*gca)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of Breit-Wigner shape 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double breit_wigner_GSL ( double x , void* params )  
  {
    //
    const Gaudi::Math::BreitWigner* bw = 
      (Gaudi::Math::BreitWigner*) params ;
    //
    return (*bw)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of Flatte shape 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double flatte_GSL ( double x , void* params )  
  {
    //
    const Gaudi::Math::Flatte* f = (Gaudi::Math::Flatte*) params ;
    //
    return (*f)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of PhaseSpace shape 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double phase_space_3_1_GSL ( double x , void* params )  
  {
    //
    const Gaudi::Math::PhaseSpace3* ps = 
      (Gaudi::Math::PhaseSpace3*) params ;
    //
    return ps -> ps2_aux (x) ;
  }
  // ==========================================================================
  /** helper function for itegration of PhaseSpace shape 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double phase_space_3_2_GSL ( double x , void* params )  
  {
    //
    const Gaudi::Math::PhaseSpace3* ps = 
      (Gaudi::Math::PhaseSpace3*) params ;
    //
    return (*ps)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of PhaseSpace shape 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double phase_space_NL_GSL ( double x , void* params )  
  {
    //
    const Gaudi::Math::PhaseSpaceNL* ps = 
      (Gaudi::Math::PhaseSpaceNL*) params ;
    //
    return (*ps)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of PhaseSpace23L shape 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double phase_space_23L_GSL ( double x , void* params )  
  {
    //
    const Gaudi::Math::PhaseSpace23L* ps23L = 
      (Gaudi::Math::PhaseSpace23L*) params ;
    //
    return (*ps23L)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of LASS23L shape 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double LASS_23L_GSL ( double x , void* params )  
  {
    //
    const Gaudi::Math::LASS23L* lass = (Gaudi::Math::LASS23L*) params ;
    //
    return (*lass)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of Bugg23L shape 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2012-05-23
   */
  double Bugg_23L_GSL ( double x , void* params )  
  {
    //
    const Gaudi::Math::Bugg23L* bugg = (Gaudi::Math::Bugg23L*) params ;
    //
    return (*bugg)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of BW23L shape 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2012-05-23
   */
  double BW_23L_GSL ( double x , void* params )  
  {
    //
    const Gaudi::Math::BW23L* bw = (Gaudi::Math::BW23L*) params ;
    //
    return (*bw)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of Flatte23L shape 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2012-05-24
   */
  double Flatte_23L_GSL ( double x , void* params )  
  {
    //
    const Gaudi::Math::Flatte23L* f = (Gaudi::Math::Flatte23L*) params ;
    //
    return (*f)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of Voigt shape 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double voigt_GSL ( double x , void* params )  
  {
    //
    const Gaudi::Math::Voigt* f = (Gaudi::Math::Voigt*) params ;
    //
    return (*f)(x) ;
  }
  // ==========================================================================
} //                                                 end of anonymous namespace 
// ============================================================================
namespace 
{
  // ==========================================================================
  /// get the complex Breit amplitude
  std::complex<double> breit_amp
  ( const double x     , 
    const double m0    , 
    const double gamma )
  {
    //
    static const std::complex<double> s_j ( 0 , 1 ) ;
    //
    const std::complex<double> v = 
      m0 * m0 - x * x - s_j * m0 * gamma ;
    //
    // attention: normalization factors and phase space are here!
    //
    const double d = 2 * std::abs ( m0 * gamma  * x ) / M_PI ;
    //
    return  std::sqrt ( d ) / v ;
  }  
  // ==========================================================================
  typedef double (*rho_fun) ( double , double , double , double ) ;
  //// calculate the current width 
  double gamma_run ( const double gam0    ,
                     const double x       , 
                     const double m1      ,
                     const double m2      ,
                     const double m0      ,
                     const unsigned int L ,
                     rho_fun fun = 0      )  
  {
    //
    if ( m1 + m2 >= x ) { return 0 ; }   // RETURN 
    //
    const double q  = Gaudi::Math::PhaseSpace2::q ( x  , m1 , m2 ) ;
    const double q0 = Gaudi::Math::PhaseSpace2::q ( m0 , m1 , m2 ) ;
    //
    if ( 0 >= q || 0 >= q0 ) { return 0 ; }  // RETURN 
    //
    const double r  = 
      0 != fun ? (*fun) ( x  , m0 , m1 , m2 ) : 1.0 ;
    const double r0 =
      0 != fun ? (*fun) ( m0 , m0 , m1 , m2 ) : 1.0 ;
    //
    if ( 0 >= r0 )           { return 0 ; }  // RETURN 
    //
    return gam0 * Gaudi::Math::pow ( q / q0 , 2 * L + 1 ) * ( r / r0 ) ;  
  }
  // ==========================================================================
} // end of the anonymous namespace 
// ============================================================================
// evaluate Chebyshev polynomial 
// ============================================================================
double Gaudi::Math::Chebyshev::operator() ( const double x ) const 
{ return _chebyshev ( m_N , x ) ; }
// ============================================================================
// evaluate Legendre polynomial 
// ============================================================================
double Gaudi::Math::Legendre::operator() ( const double x ) const 
{ return _legendre ( m_N , x ) ; }
// ============================================================================
// evaluate Hermite polynomial 
// ============================================================================
double Gaudi::Math::Hermite::operator() ( const double x ) const 
{ return _hermite  ( m_N , x ) ; }
// ============================================================================
// BifurcatedGauss 
// ============================================================================
/*  constructor from all parameters 
 *  @param peak    the peak posiion 
 *  @param sigma_L (left sigma)
 *  @param sigma_R (right sigma)
 */
// ============================================================================
Gaudi::Math::BifurcatedGauss::BifurcatedGauss 
( const double peak   , 
  const double sigmaL , 
  const double sigmaR )
  : std::unary_function<double,double> () 
  , m_peak    ( peak ) 
  , m_sigmaL  ( std::fabs ( sigmaL ) ) 
  , m_sigmaR  ( std::fabs ( sigmaR ) ) 
//
{}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::BifurcatedGauss::~BifurcatedGauss (){}
// ============================================================================
// evaluate Bifurcated Gaussian 
// ============================================================================
double Gaudi::Math::BifurcatedGauss::operator() ( const double x ) const 
{
  const double dx = x - m_peak ;
  //
  const double norm = s_SQRTPIHALF * ( sigmaL() + sigmaR() ) ;
  //
  return 
    dx < 0 ?
    my_exp ( -0.5 * dx * dx / sigmaL () / sigmaL () ) / norm :
    my_exp ( -0.5 * dx * dx / sigmaR () / sigmaR () ) / norm ;
}
// ============================================================================
// get the integral 
// ============================================================================
double Gaudi::Math::BifurcatedGauss::integral () const { return 1 ; }
// ============================================================================
// get the integral between low and high limits 
// ============================================================================
double  Gaudi::Math::BifurcatedGauss::integral 
( const double low  , 
  const double high ) const 
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN 
  if (           low > high   ) { return - integral ( high ,                                                     
                                                      low  ) ; } // RETURN 
  //
  const double norm = s_SQRTPIHALF * ( sigmaL() + sigmaR() ) ;
  //
  // left half-gaussian 
  if       ( high <= m_peak ) 
  {
    return gaussian_int ( 0.5 / sigmaL() / sigmaL() , 
                          0                       , 
                          low  - m_peak           , 
                          high - m_peak           ) / norm ;
    
  }
  // right half-gaussian 
  else if ( low >= m_peak )  
  {
    return gaussian_int ( 0.5 / sigmaR() / sigmaR() , 
                          0                       , 
                          low  - m_peak           , 
                          high - m_peak           ) / norm ;
    
  }  
  //
  return
    integral ( low    , m_peak ) + 
    integral ( m_peak , high   ) ;
}
// ============================================================================
bool Gaudi::Math::BifurcatedGauss::setSigmaL ( const double value ) 
{ 
  const double value_ = std::fabs ( value ) ; 
  if ( s_equal ( m_sigmaL , value_ ) ) { return false ; }
  m_sigmaL = value_ ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::BifurcatedGauss::setSigmaR ( const double value ) 
{ 
  const double value_ = std::fabs ( value ) ; 
  if ( s_equal ( m_sigmaR , value_ ) ) { return false ; }
  m_sigmaR = value_ ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::BifurcatedGauss::setPeak( const double value ) 
{
  if ( s_equal ( m_peak , value ) ) { return false ; }
  m_peak   = value  ; 
  //
  return true ;
}
// ============================================================================
double Gaudi::Math::BifurcatedGauss::sigma   () const 
{ return 0.5  * ( sigmaL() + sigmaR() )            ; }  
// ============================================================================
double Gaudi::Math::BifurcatedGauss::asym    () const 
{ return 0.5  * ( sigmaL() - sigmaR() ) / sigma () ; } 
 // ============================================================================


// ============================================================================
// WorskSpace 
// ============================================================================
// constructor 
// ============================================================================
Gaudi::Math::WorkSpace::WorkSpace () : m_workspace ( 0 ){}
// ============================================================================
// copy constructor 
// ============================================================================
Gaudi::Math::WorkSpace::WorkSpace 
( const Gaudi::Math::WorkSpace& /* right */ ) 
  : m_workspace ( 0 )
{}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::WorkSpace::~WorkSpace ()
{
  if ( 0 != m_workspace ) 
  { 
    gsl_integration_workspace * _ws = (gsl_integration_workspace*) m_workspace ;
    m_workspace = 0 ;
    gsl_integration_workspace_free ( _ws );
  }
}
// ============================================================================
// get the integration workspace 
// ============================================================================
void* Gaudi::Math::WorkSpace::workspace () const 
{
  if ( 0 == m_workspace )  
  { m_workspace = gsl_integration_workspace_alloc ( s_SIZE ) ; }
  //
  return m_workspace ;
} 
// ============================================================================
// fictive assignement operator 
// ============================================================================
Gaudi::Math::WorkSpace&
Gaudi::Math::WorkSpace::operator=
  ( const Gaudi::Math::WorkSpace& /* right */ ) { return *this ; }
// ============================================================================




  
// ============================================================================
// Bukin
// ============================================================================
/*  constructor from all parameters 
 *  @param peak  the peak position 
 *  @param sigma the effective sigma, defined as FWHM/2.35
 *  @param xi    the asymmetry parameter 
 *  @param rhoL  the left  tail paramter
 *  @param rhoR  the right tail paramter
 */
// ============================================================================
Gaudi::Math::Bukin::Bukin 
( const double peak   , 
  const double sigma  , 
  const double xi     ,
  const double rho_L  , 
  const double rho_R  )
//
  : std::unary_function<double,double> ()
//
  , m_peak      ( M_PI + peak  ) 
  , m_sigma     ( M_PI + sigma ) 
  , m_xi        ( M_PI + xi    ) 
  , m_rho_L     ( M_PI + rho_L ) 
  , m_rho_R     ( M_PI + rho_R ) 
//
  , m_x1        ( M_PI ) 
  , m_x2        ( M_PI ) 
//
  , m_integral  ( -1000 ) 
//
  , m_workspace () 
{
  //
  setXi    ( xi    ) ; // must be the first 
  //
  setPeak  ( peak  ) ;
  //
  setSigma ( sigma ) ;
  //
  setRhoL  ( rho_L ) ;
  //
  setRhoR  ( rho_R ) ;
  //
}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::Bukin::~Bukin () {}
// ============================================================================
bool Gaudi::Math::Bukin::setPeak  ( const double value ) 
{
  if ( s_equal ( value , m_peak ) ) { return false ; }
  //
  m_peak   = value ; 
  //
  const double xi_ = m_xi / std::sqrt ( 1 + m_xi * m_xi ) ;  
  m_x1     = m_peak + m_sigma * s_Bukin * ( xi_ - 1 ) ;
  m_x2     = m_peak + m_sigma * s_Bukin * ( xi_ + 1 ) ;
  //
  return true ;
} 
// ============================================================================
bool Gaudi::Math::Bukin::setSigma ( const double value ) 
{
  //
  const double value_ = std::fabs ( value ) ;
  if ( s_equal ( value_ , m_sigma ) ) { return false ; }
  //
  m_sigma  = value_ ;  
  //
  const double xi_ = m_xi/std::sqrt ( 1 + m_xi * m_xi ) ;
  m_x1 = m_peak + m_sigma * s_Bukin * ( xi_ - 1 ) ;
  m_x2 = m_peak + m_sigma * s_Bukin * ( xi_ + 1 ) ;
  //
  m_integral = -1000 ;
  //
  return true ;
}  
// ============================================================================
bool Gaudi::Math::Bukin::setXi    ( const double value ) 
{
  // no need for update 
  if ( s_equal ( value , m_xi ) ) { return false ; } // no need for update 
  //
  m_xi     = value ;
  //
  const double xi      = m_xi    ;
  const double xi2     =   xi*xi ;
  const double xi2sqrt = std::sqrt ( 1 + xi2 ) ;
  //
  const double alpha = 2 * xi  * xi2sqrt / s_Bukin   ;
  const double beta  = 2 * xi  * ( xi - xi2sqrt )    ;
  // well, it is actually alpha/beta:
  const double ab    = xi2sqrt / ( xi - xi2sqrt ) / s_Bukin ;
  //
  m_A     = alpha             ;
  //
  m_B2    = 1/x_log ( beta )  ;
  m_B2   *= m_B2              ;
  m_B2   *= ab*ab             ;
  //
  //
  const double delta = xi + xi2sqrt - 1 ;
  const double tail  = 
    0.5 * s_Bukin * xi2sqrt * ( 1 + xi + xi2sqrt ) 
    / ( xi + xi2sqrt ) / x_log ( delta ) ;
  //
  // left  tail parameter 
  //
  m_L  = tail ;
  m_L /= ( xi2sqrt - xi ) ;
  m_L /= ( xi2sqrt - xi ) ;
  //
  // right tail parameter 
  //
  m_R  = tail ;
  m_R /= ( xi2sqrt + xi ) ;
  m_R /= ( xi2sqrt + xi ) ;
  //
  // central region
  //
  const double xi_ = m_xi / std::sqrt ( 1 + m_xi * m_xi ) ;  
  m_x1 = m_peak + m_sigma * s_Bukin * ( xi_ - 1 ) ;
  m_x2 = m_peak + m_sigma * s_Bukin * ( xi_ + 1 ) ;
  //
  m_integral = -1000 ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::Bukin::setRhoL  ( const double value ) 
{
  //
  const double value_ = std::fabs ( value ) ;
  if ( s_equal ( value_ , m_rho_L  ) ) { return false ; }
  //
  m_rho_L    = value_ ;
  m_integral = -1000  ;
  //
  return true ;
}  
// ============================================================================
bool Gaudi::Math::Bukin::setRhoR  ( const double value ) 
{
  //
  const double value_ = std::fabs ( value ) ;
  if ( s_equal ( value_ , m_rho_R  ) ) { return false ; }
  //
  m_rho_R    = value_ ;
  m_integral = -1000  ;
  //
  return true ;
}  
// ============================================================================
// evaluate Bukin's function
// ============================================================================
double Gaudi::Math::Bukin::operator() ( const double x ) const 
{
  //
  //  left tail :
  //
  if       ( m_x1 >= x ) 
  {
    const double dx  = x - m_x1               ;
    const double dx2 = dx / ( m_peak - m_x1 ) ;
    return 0.5 * my_exp (   m_L * dx / m_sigma  - m_rho_L * dx2 * dx2 ) ;
  }
  //
  // right tail :
  //
  else if ( m_x2 <=  x ) 
  {
    const double dx  = x - m_x2               ;
    const double dx2 = dx / ( m_peak - m_x2 ) ;
    return 0.5 * my_exp ( - m_R * dx / m_sigma  - m_rho_R * dx2 * dx2 ) ;
  }
  //
  // central region 
  //
  const double dx   = ( x - m_peak ) / m_sigma ;
  const double A    = x_log ( m_A * dx ) ;
  //
  return my_exp ( - s_ln2 * dx * dx * A * A * m_B2 ) ;
}
// =========================================================================
// get the integral between low and high limits 
// =========================================================================
double Gaudi::Math::Bukin::integral 
( const double low  , 
  const double high ) const 
{
  //
  if      ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN 
  else if (           low > high   ) { return - integral ( high ,
                                                           low  ) ; } // RETURN
  //
  //
  // split into reasonable sub-intervals
  if      ( low < m_x1  && m_x1 < high ) 
  {
    return 
      integral (  low , m_x1 ) + 
      integral ( m_x1 , high ) ;
  }
  else if ( low < m_x2 && m_x2 < high ) 
  {
    return 
      integral (  low , m_x2 ) + 
      integral ( m_x2 , high ) ;
  }
  //
  // split, if the interval is too large
  //
  const double width = std::max ( std::abs  ( m_sigma )  , 0.0 ) ;
  if ( 0 < width &&  6 * width < high - low  ) 
  {
    return 
      integral ( low                   , 0.5 *  ( high + low ) ) + 
      integral ( 0.5 *  ( high + low ) ,          high         ) ;
  }
  //
  // left tail:
  if      ( high <= m_x1 )  // left tail 
  {
    const double d =  m_peak - m_x1 ;
    //
    return 0.5 * gaussian_int ( m_rho_L / d / d , 
                                m_L  / m_sigma  , 
                                low  - m_x1     , 
                                high - m_x1     ) ;
  }
  // right tail:
  else if ( low >= m_x2 )  // right tail 
  {
    const double d = m_peak - m_x2 ;
    //
    return 0.5 * gaussian_int ( m_rho_R / d / d   , 
                                - m_R  / m_sigma  , 
                                low  - m_x2       , 
                                high - m_x2       ) ;
  }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                ;
  F.function = &bukin_GSL ;
  F.params   = const_cast<Bukin*> ( this ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag 
    ( &F                ,            // the function 
      low   , high      ,            // low & high edges 
      s_PRECISION       ,            // absolute precision            
      s_PRECISION       ,            // relative precision 
      s_SIZE            ,            // size of workspace 
      GSL_INTEG_GAUSS31 ,            // integration rule  
      workspace ( m_workspace ) ,    // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    //
    gsl_error ( "Gaudi::Math::Bukin::QAG" ,
                __FILE__ , __LINE__ , ierror ) ; 
  }
  //
  return result ;
}
// =========================================================================
// get the integral
// =========================================================================
double Gaudi::Math::Bukin::integral () const
{
  if ( m_integral <= 0 ) 
  {
    Bukin* bukin = const_cast<Bukin*> ( this ) ;
    bukin -> integrate() ;
  }
  //
  return m_integral ;
}
// ============================================================================
// calculate  the integral 
// =========================================================================
void Gaudi::Math::Bukin::integrate()
{
  // 
  m_integral = 0.0 ;
  // left tail:
  {
    const double d     = m_peak - m_x1     ;
    const double alpha = m_rho_L / d / d   ;
    const double beta  = m_L     / m_sigma ;
    //
    m_integral +=  0.5 * gaussian_int_L ( alpha , beta , 0 ) ;
  }
  // right tail
  {
    const double d     =    m_peak - m_x2     ;
    const double alpha =    m_rho_R / d / d   ;
    const double beta  =  - m_R     / m_sigma ;
    //
    m_integral += 0.5 * gaussian_int_R ( alpha , beta  , 0 ) ;
  }
  //
  m_integral += integral ( m_x1 , m_x2 ) ;
  //
}
// ============================================================================


// ============================================================================
// Novosibirsk function
// ============================================================================
/*  constructor from all parameters 
 *  @param m0    the peak posiion
 *  @param sigma the effective sigma
 *  @param tau   the tail paramter
 */
// ============================================================================
Gaudi::Math::Novosibirsk::Novosibirsk
( const double m0         , 
  const double sigma      , 
  const double tau        ) 
  : std::unary_function<double,double> () 
//
  , m_m0        ( m0                   ) 
  , m_sigma     ( std::fabs ( sigma )  ) 
  , m_tau       ( std::tanh ( tau   )  ) 
//
  , m_lambda    ( 0.0   ) 
//
  , m_integral  ( -1000 ) 
  , m_workspace () 
{
  //
  m_lambda = x_sinh ( m_tau * s_Novosibirsk ) ;
  //
}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::Novosibirsk::~Novosibirsk() {}
// ============================================================================
// set parameter m0
// ============================================================================
bool Gaudi::Math::Novosibirsk::setM0    ( const double value )
{ 
  //
  if ( s_equal ( m_m0 ,  value ) ) { return false ; }
  //
  m_m0 = value ;
  //
  return true ;
}
// ============================================================================
// set parameter sigma
// ============================================================================
bool Gaudi::Math::Novosibirsk::setSigma ( const double value ) 
{ 
  const double value_ = std::fabs ( value ) ;
  if ( s_equal ( value_ , m_sigma ) ) { return false ; }
  // 
  m_sigma    = value_ ;
  m_integral = -1000 ;
  //
  return true ;
}
// ============================================================================
// set parameter tau
// ============================================================================
bool Gaudi::Math::Novosibirsk::setTau ( const double value ) 
{ 
  //
  const double value_ = std::tanh ( value )   ;
  if ( s_equal ( value_ , m_tau ) ) { return false ; }
  //
  m_tau      = value_ ;
  m_integral = -1000 ;
  //
  m_lambda   = x_sinh ( m_tau * s_Novosibirsk ) ;
  //
  return true ;
}
// ============================================================================
// evaluate Novosibirsk's function 
// ============================================================================
double Gaudi::Math::Novosibirsk::operator() ( const double x ) const 
{
  //
  const double dx     = ( x - m_m0 ) / m_sigma ;
  //
  const double arg    = m_lambda * dx * m_tau ;
  //
  if ( arg <= -1 || s_equal ( arg , -1 ) ) { return 0 ; } // RETURN
  //
  const double l      =  x_log ( arg ) * m_lambda * dx ;
  //
  const double result = l * l  + m_tau * m_tau ;
  //
  return  my_exp ( -0.5 * result ) ;
}
// =========================================================================
// get the integral between low and high limits 
// =========================================================================
double Gaudi::Math::Novosibirsk::integral 
( const double low  , 
  const double high ) const 
{
  //
  if      ( s_equal ( low , high ) ) { return                  0.0 ; } // RETURN 
  else if (           low > high   ) { return - integral ( high , 
                                                           low   ) ; } // RETURN 
  //
  // split into reasonable sub intervals
  //
  const double x1     = m_m0 - 10 * m_sigma  ;
  const double x2     = m_m0 + 10 * m_sigma  ;
  const double x_low  = std::min ( x1 , x2 ) ;
  const double x_high = std::max ( x1 , x2 ) ;
  //
  if      ( low < x_low  && x_low < high ) 
  {
    return 
      integral (   low , x_low  ) + 
      integral ( x_low ,   high ) ;
  }
  else if ( low <  x_high && x_high < high ) 
  {
    return 
      integral (   low  , x_high  ) + 
      integral ( x_high ,   high  ) ;
  }
  //
  // split, if the interval is too large
  //
  const double width = std::max ( std::abs  ( m_sigma )  , 0.0 ) ;
  if ( 0 < width &&  3 * width < high - low  ) 
  {
    return 
      integral ( low                   , 0.5 *  ( high + low ) ) + 
      integral ( 0.5 *  ( high + low ) ,          high         ) ;
  }
  //
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                ;
  F.function = &novosibirsk_GSL ;
  F.params   = const_cast<Novosibirsk*> ( this ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag 
    ( &F                ,            // the function 
      low   , high      ,            // low & high edges 
      s_PRECISION       ,            // absolute precision            
      ( high   <= x_low  ) ? s_PRECISION_TAIL :
      ( x_high <=   low  ) ? s_PRECISION_TAIL : 
      s_PRECISION       ,            // relative precision 
      s_SIZE            ,            // size of workspace 
      GSL_INTEG_GAUSS31 ,            // integration rule  
      workspace ( m_workspace ) ,    // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::Novosibirsk::QAG" ,
                __FILE__ , __LINE__ , ierror ) ; 
  }
  //
  return result ;
}
// =========================================================================
// get the integral
// =========================================================================
double Gaudi::Math::Novosibirsk::integral () const
{
  if ( m_integral <= 0 ) 
  {
    Novosibirsk* novosibirsk = const_cast<Novosibirsk*> ( this ) ;
    novosibirsk -> integrate() ;
  }
  //
  return m_integral ;
}
// ============================================================================
// calculate  the integral 
// =========================================================================
void Gaudi::Math::Novosibirsk::integrate()
{
  //
  const double x1     = m_m0 - 10 * m_sigma ;
  const double x2     = m_m0 + 10 * m_sigma ;
  const double x_low  = std::min ( x1 , x2 ) ;
  const double x_high = std::max ( x1 , x2 ) ;
  //
  // use GSL to evaluate the tails:
  //
  Sentry sentry ;
  //
  gsl_function F                ;
  F.function = &novosibirsk_GSL ;
  F.params   = const_cast<Novosibirsk*> ( this ) ;
  //
  // left tail:
  //
  double tail_l   =  0.0 ;
  double error_l  = -1.0 ;
  //
  const int ierror_l = gsl_integration_qagil
    ( &F                ,         // the function 
      x_low             ,         // "high" edge
      s_PRECISION       ,         // absolute precision  
      s_PRECISION_TAIL  ,         // relative precision 
      s_SIZE            ,         // size of workspace 
      workspace ( m_workspace ) , // workspace  
      &tail_l           ,         // the result 
      &error_l          ) ;        // the error in result 
  //
  if ( ierror_l ) 
  { 
    gsl_error ( "Gaudi::Math::Novosibirsk::QAGIL" ,
                __FILE__ , __LINE__ , ierror_l ) ; 
    tail_l = 0.0 ;
  }
  //
  //
  // right tail:
  //
  double tail_r   =  0.0 ;
  double error_r  = -1.0 ;
  //
  const int ierror_r = gsl_integration_qagiu
    ( &F                ,         // the function 
      x_high            ,         // "low" edge
      s_PRECISION       ,         // absolute precision  
      s_PRECISION_TAIL  ,         // relative precision 
      s_SIZE            ,         // size of workspace 
      workspace ( m_workspace ) , // workspace  
      &tail_r           ,         // the result 
      &error_r          ) ;       // the error in result 
  //
  if ( ierror_r ) 
  { 
    gsl_error ( "Gaudi::Math::Novosibirsk::QAGIU" ,
                __FILE__ , __LINE__ , ierror_r ) ; 
    tail_r = 0.0 ;
  }
  //
  // get the final result 
  // 
  m_integral = tail_l + integral ( x_low , x_high ) + tail_r ;
  //
}
// ============================================================================
// Crystal Ball & Co 
// ============================================================================
/*  constructor from all parameters 
 *  @param m0 m0 parameter 
 *  @param alpha alpha parameter 
 *  @param n     n-parameter 
 */
// ============================================================================
Gaudi::Math::CrystalBall::CrystalBall  
( const double m0    , 
  const double sigma , 
  const double alpha , 
  const double N     ) 
  : std::unary_function<double,double> () 
  , m_m0         ( m0                  )
  , m_sigma      (     std::fabs ( sigma ) )
  , m_alpha      ( 1 + std::fabs ( alpha ) )
  , m_N          ( 1 + std::fabs ( N     ) )
//
  , m_const      ( 0.0   )  
  , m_integral   ( -1000 ) 
  , m_workspace  () 
{
  m_const = my_exp ( -0.5 * m_alpha * m_alpha ) ;
}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::CrystalBall::~CrystalBall (){}
// ============================================================================
bool  Gaudi::Math::CrystalBall::setM0 ( const double value ) 
{
  if ( s_equal ( value , m_m0 ) ) { return false ; } 
  //
  m_m0       = value ;
  m_integral = -1000 ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBall::setSigma ( const double value ) 
{
  const double value_ = std::fabs ( value );
  if ( s_equal ( value_ , m_sigma ) ) { return false ; } 
  //
  m_sigma    = value_ ;
  m_integral = -1000  ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBall::setAlpha  ( const double value ) 
{
  const double value_ = 1 + std::fabs ( value );
  if ( s_equal ( value_ , m_alpha ) ) { return false ; } 
  //
  m_alpha    = value_ ;
  m_const    = my_exp ( -0.5 * m_alpha * m_alpha ) ;  
  m_integral = -1000  ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBall::setN      ( const double value ) 
{
  const double value_ = 1 + std::fabs ( value );
  if ( s_equal ( value_ , m_N     ) ) { return false ; } 
  //
  m_N        = value_ ;
  m_integral = -1000  ;
  //
  return true ;
}
// ============================================================================
//  evaluate CrystalBall's function 
// ============================================================================
double Gaudi::Math::CrystalBall::operator() ( const double x ) const 
{
  //
  const double dx = ( x - m_m0 ) / m_sigma ;
  //
  // tail 
  //
  if  ( dx < -m_alpha ) 
  {
    const double f1 = m_N / m_alpha ;
    const double f2 = m_N / m_alpha - m_alpha - dx ;
    //
    return m_const * std::pow ( f1 / f2 , m_N ) ;  
  }
  //
  // peak 
  // 
  return my_exp ( -0.5 * dx * dx ) ;
}
// ============================================================================
// get the integral between low and high 
// ============================================================================
double Gaudi::Math::CrystalBall::integral 
( const double low , 
  const double high ) const 
{
  //
  if      ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN 
  else if (           low > high   ) { return - integral ( high , 
                                                           low  ) ; } // RETURN 
  //
  const double x0 = m_m0 - m_alpha * m_sigma ;
  //
  // split into proper subintervals 
  if      ( low < x0 && x0 < high ) 
  { return integral ( low , x0 ) + integral ( x0 , high ) ; }
  //
  // peak 
  //
  if ( x0 <= low  ) 
  {
    return gaussian_int ( 0.5 / m_sigma / m_sigma , 
                          0                       , 
                          low  - m_m0             , 
                          high - m_m0             ) ;
  }
  //
  // tail
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                ;
  F.function = &crystalball_GSL ;
  F.params   = const_cast<CrystalBall*> ( this ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag 
    ( &F                ,            // the function 
      low   , high      ,            // low & high edges 
      s_PRECISION       ,            // absolute precision            
      s_PRECISION       ,            // relative precision 
      s_SIZE            ,            // size of workspace 
      GSL_INTEG_GAUSS31 ,            // integration rule  
      workspace ( m_workspace ) ,    // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::CrystalBall::QAG" ,
                __FILE__ , __LINE__ , ierror ) ; 
  }
  //
  return result ;
}
// ============================================================================
// get the (trunkated)  integral
// ============================================================================
void Gaudi::Math::CrystalBall::integrate () 
{
  //
  const double x0  = m_m0 - m_alpha * m_sigma ;
  //
  const double low = std::max ( m_m0 - s_TRUNC * m_sigma , 0.0 ) ;
  //
  // integrate the tail:
  m_integral  = integral       ( low , x0 ) ;
  // integrate the peak:
  m_integral += gaussian_int_R ( 0.5 / m_sigma / m_sigma ,
                                 0  ,
                                 x0 ) ;
}
// =========================================================================
// get the integral
// =========================================================================
double Gaudi::Math::CrystalBall::integral () const
{
  if ( m_integral <= 0 ) 
  {
    CrystalBall* cb = const_cast<CrystalBall*> ( this ) ;
    cb -> integrate() ;
  }
  //
  return m_integral ;
}
// ============================================================================
// Needham function 
// ============================================================================
/* constructor from all parameters 
 *  @param m0     m0       parameter 
 *  @param sigma  sigma    parameter 
 *  @param a0     a0       parameter 
 *  @param a1     a1       parameter 
 *  @param a2     a2       parameter 
 */
// ============================================================================
Gaudi::Math::Needham::Needham
( const double m0    , 
  const double sigma , 
  const double a0    , 
  const double a1    , 
  const double a2    ) 
  : std::unary_function<double,double>()
/// @see Gaudi::Math:CrystalBall
  , m_cb  ( m0 , sigma , 1 , 0 ) // Gaudi::Math:CrystalBall
  , m_a0  ( std::abs ( a0 )  ) 
  , m_a1  (            a1    ) 
  , m_a2  (            a2    ) 
{
  m_cb.setAlpha ( alpha () ) ;  
}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::Needham::~Needham(){}
// ============================================================================
bool Gaudi::Math::Needham::setA0 ( const double value ) 
{
  const double value_ = std::fabs ( value );
  if ( s_equal ( value_ , m_a0 ) ) { return false ; } 
  m_a0      = value_ ;
  return m_cb.setAlpha ( alpha () ) ;
}
// ============================================================================
bool Gaudi::Math::Needham::setA1 ( const double value ) 
{
  if ( s_equal ( value , m_a1 ) ) { return false ; } 
  m_a1 = value ;
  return m_cb.setAlpha ( alpha () ) ;
}
// ============================================================================
bool Gaudi::Math::Needham::setA2 ( const double value ) 
{
  if ( s_equal ( value , m_a2 ) ) { return false ; } 
  m_a2 = value ;
  return m_cb.setAlpha ( alpha () ) ;
}
// ===========================================================================
// evaluate Needham's function 
// ===========================================================================
double Gaudi::Math::Needham::operator() ( const double x ) const 
{ return m_cb ( x ) ; }
// ============================================================================
/*  constructor from all parameters 
 *  @param m0 m0 parameter 
 *  @param alpha alpha parameter 
 *  @param n     n-parameter 
 */
// ============================================================================
Gaudi::Math::CrystalBallDoubleSided::CrystalBallDoubleSided  
( const double m0      , 
  const double sigma   , 
  const double alpha_L , 
  const double N_L     ,   
  const double alpha_R , 
  const double N_R     ) 
  : std::unary_function<double,double> () 
  , m_m0         (  m0                        )
  , m_sigma      (      std::fabs ( sigma   ) )
  , m_alpha_L    (  1 + std::fabs ( alpha_L ) )
  , m_N_L        (  1 + std::fabs ( N_L     ) )
  , m_alpha_R    (  1 + std::fabs ( alpha_R ) )
  , m_N_R        (  1 + std::fabs ( N_R     ) )
//
  , m_const_L    (  1 ) 
  , m_const_R    (  1 )
  , m_integral   ( -1000 ) 
  , m_workspace  () 
{
  //
  m_const_L = my_exp ( -0.5 * m_alpha_L * m_alpha_L ) ;  
  m_const_R = my_exp ( -0.5 * m_alpha_R * m_alpha_R ) ;  
}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::CrystalBallDoubleSided::~CrystalBallDoubleSided(){}
// ============================================================================
bool Gaudi::Math::CrystalBallDoubleSided::setM0 ( const double value ) 
{
  if ( s_equal ( value , m_m0 ) ) { return false ; } 
  //
  m_m0       = value ;
  m_integral = -1000 ;
  // 
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBallDoubleSided::setSigma ( const double value ) 
{
  const double value_ = std::fabs ( value );
  if ( s_equal ( value_ , m_sigma ) ) { return false ; } 
  //
  m_sigma    = value_ ;
  m_integral = -1000  ;
  // 
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBallDoubleSided::setAlpha_L ( const double value ) 
{
  const double value_ = 1 + std::fabs ( value );
  if ( s_equal ( value_ , m_alpha_L ) ) { return false ; } 
  //
  m_alpha_L  = value_ ;
  m_const_L  = my_exp ( -0.5 * m_alpha_L * m_alpha_L ) ;  
  m_integral = -1000  ;
  // 
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBallDoubleSided::setAlpha_R ( const double value ) 
{
  const double value_ = 1 + std::fabs ( value );
  if ( s_equal ( value_ , m_alpha_R ) ) { return false ; } 
  //
  m_alpha_R  = value_ ;
  m_const_R  = my_exp ( -0.5 * m_alpha_R * m_alpha_R ) ;  
  m_integral = -1000  ;
  // 
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBallDoubleSided::setN_L     ( const double value ) 
{
  const double value_ = 1 + std::fabs ( value );
  if ( s_equal ( value_ , m_N_L    ) ) { return false ; } 
  //
  m_N_L      = value_ ;
  m_integral = -1000  ;
  // 
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBallDoubleSided::setN_R     ( const double value ) 
{
  const double value_ = 1 + std::fabs ( value );
  if ( s_equal ( value_ , m_N_R    ) ) { return false ; } 
  //
  m_N_R      = value_ ;
  m_integral = -1000  ;
  // 
  return true ;
}
// ============================================================================
//  evaluate CrystalBall's function 
// ============================================================================
double Gaudi::Math::CrystalBallDoubleSided::operator() ( const double x ) const 
{
  //
  const double dx = ( x - m_m0 ) / m_sigma ;
  //
  // left tail 
  if      ( dx  < -m_alpha_L )  // left tail 
  {
    const double f1 = m_N_L / m_alpha_L ;
    const double f2 = m_N_L / m_alpha_L - m_alpha_L - dx ;
    //
    return m_const_L * std::pow ( f1 / f2 , m_N_L ) ;  
  }
  // right tail
  else if  ( dx >  m_alpha_R )  // right tail
  {
    const double f1 = m_N_R / m_alpha_R ;
    const double f2 = m_N_R / m_alpha_R - m_alpha_R + dx ;
    //
    return m_const_R * std::pow ( f1 / f2 , m_N_R ) ;  
  }
  //
  // peak 
  //
  return my_exp ( -0.5 * dx * dx ) ;
}
// ============================================================================
// get the integral between low and high 
// ============================================================================
double Gaudi::Math::CrystalBallDoubleSided::integral 
( const double low , 
  const double high ) const 
{
  //
  if      ( s_equal ( low , high ) ) { return 0.0 ; } // RETURN 
  else if (           low > high   ) { return - integral ( high , low ) ; }
  //
  const double x_low  = m_m0 - m_alpha_L * m_sigma ;
  const double x_high = m_m0 + m_alpha_R * m_sigma ;
  //
  // split into proper subintervals 
  if ( low < x_low  && x_low  < high ) 
  { return integral ( low , x_low  ) + integral ( x_low  , high ) ; }
  if ( low < x_high && x_high < high ) 
  { return integral ( low , x_high ) + integral ( x_high , high ) ; }
  //
  //
  // the peak 
  //
  if ( x_low <= low && high <= x_high ) 
  {
    return gaussian_int ( 0.5 / ( m_sigma * m_sigma ) , 
                          0            , 
                          low   - m_m0 ,
                          high  - m_m0 ) ;
  }
  //
  // tails 
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                ;
  F.function = &crystalball2s_GSL ;
  F.params   = const_cast<CrystalBallDoubleSided*> ( this ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag 
    ( &F                ,            // the function 
      low   , high      ,            // low & high edges 
      s_PRECISION       ,            // absolute precision            
      s_PRECISION       ,            // relative precision 
      s_SIZE            ,            // size of workspace 
      GSL_INTEG_GAUSS31 ,            // integration rule  
      workspace ( m_workspace ) ,    // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::CrystalBallDoubleSided::QAG" ,
                __FILE__ , __LINE__ , ierror ) ; 
  }
  //
  return result ;
}
// ============================================================================
// get the (trunkated)  integral
// ============================================================================
void Gaudi::Math::CrystalBallDoubleSided::integrate () 
{
  //
  const double x_low  = m_m0 - m_alpha_L * m_sigma ;
  const double x_high = m_m0 + m_alpha_R * m_sigma ;
  //
  const double low  = x_low  - std::max ( s_TRUNC , m_alpha_L ) * m_sigma ;
  const double high = x_high + std::max ( s_TRUNC , m_alpha_R ) * m_sigma ;
  //
  // left tail 
  m_integral  = integral ( low    , x_low ) ;
  // rigth tail
  m_integral += integral ( x_high , high  ) ;
  // integrate the peak:
  m_integral += gaussian_int ( 0.5 / ( m_sigma * m_sigma ) , 
                               0.0                         , 
                               x_low  , 
                               x_high ) ;
}
// =========================================================================
// get the integral
// =========================================================================
double Gaudi::Math::CrystalBallDoubleSided::integral () const
{
  if ( m_integral <= 0 ) 
  {
    CrystalBallDoubleSided* cb = const_cast<CrystalBallDoubleSided*> ( this ) ;
    cb -> integrate() ;
  }
  //
  return m_integral ;
}
// ============================================================================


// ============================================================================
// Gram-Charlier type A 
// ============================================================================
// constructor from all parameters
// ============================================================================
Gaudi::Math::GramCharlierA::GramCharlierA 
( const double mean   , 
  const double sigma  , 
  const double kappa3 , 
  const double kappa4 ) 
  : std::unary_function<double,double> () 
//
  , m_mean   ( mean )
  , m_sigma  ( std::fabs ( sigma ) ) 
  , m_kappa3 ( kappa3 ) 
  , m_kappa4 ( kappa4 ) 
//
  , m_workspace () 
//
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::GramCharlierA::~GramCharlierA() {}
// ============================================================================
// evaluate Gram-Charlier type A approximation 
// ============================================================================
double Gaudi::Math::GramCharlierA::operator() ( const double x ) const 
{
  //
  const double dx = ( x - m_mean ) / m_sigma ;
  //
  const double result_0 = my_exp ( -0.5 * dx * dx ) / m_sigma / s_SQRT2PI ;
  //
  double correction = 1 ;
  //
  correction += m_kappa3 * s_h3 ( dx ) /  6 ; 
  //
  correction += m_kappa4 * s_h4 ( dx ) / 24 ; 
  //
  return correction * result_0 ;
}
// ============================================================================
// integral 
// ============================================================================
double Gaudi::Math::GramCharlierA::integral () const { return 1 ; }
// ============================================================================
// integral 
// ============================================================================
double Gaudi::Math::GramCharlierA::integral 
( const double low  , 
  const double high ) const 
{
  //
  if      ( s_equal ( low , high ) ) { return                  0.0 ; } // RETURN 
  else if (           low > high   ) { return - integral ( high , 
                                                           low   ) ; } // RETURN 
  //
  const double x_low  = m_mean - 5 * m_sigma ;
  const double x_high = m_mean + 5 * m_sigma ;
  //
  // split for the reasonable sub intervals: 
  // 
  if      ( low < x_low  && x_low < high ) 
  {
    return 
      integral (   low , x_low  ) + 
      integral ( x_low ,   high ) ;
  }
  else if ( low <  x_high && x_high < high ) 
  {
    return 
      integral (   low  , x_high  ) + 
      integral ( x_high ,   high  ) ;
  }
  //
  //
  // split, if the interval is too large
  //
  const double width = std::max ( std::abs  ( m_sigma)  , 0.0 ) ;
  if ( 0 < width &&  3 * width < high - low  ) 
  {
    return 
      integral ( low                   , 0.5 *  ( high + low ) ) + 
      integral ( 0.5 *  ( high + low ) ,          high         ) ;
  }
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                ;
  F.function = &gram_charlier_A_GSL ;
  F.params   = const_cast<GramCharlierA*> ( this ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag 
    ( &F                ,            // the function 
      low   , high      ,            // low & high edges 
      s_PRECISION       ,            // absolute precision            
      ( high   <= x_low  ) ? s_PRECISION_TAIL :
      ( x_high <=   low  ) ? s_PRECISION_TAIL : 
      s_PRECISION       ,            // relative precision 
      s_SIZE            ,            // size of workspace 
      GSL_INTEG_GAUSS31 ,            // integration rule  
      workspace ( m_workspace ) ,    // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::GramCharlierA::QAG" ,
                __FILE__ , __LINE__ , ierror ) ; 
  }
  //
  return result ;
}
// ============================================================================
bool Gaudi::Math::GramCharlierA::setM0  ( const double value ) 
{
  //
  if ( s_equal ( m_mean , value ) ) { return false ; }
  //
  m_mean  = value ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::GramCharlierA::setSigma  ( const double value ) 
{
  //
  const double value_ = std::fabs ( value ) ;
  if ( s_equal ( m_sigma , value_ ) ) { return false ; }
  //
  m_sigma  = value_ ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::GramCharlierA::setKappa3 ( const double value ) 
{
  if ( s_equal ( m_kappa3 , value )  ) { return false ; }
  //
  m_kappa4  = value ;
  //
  return false ;
}
// ============================================================================
bool Gaudi::Math::GramCharlierA::setKappa4 ( const double value ) 
{
  if ( s_equal ( m_kappa4 , value )  ) { return false ; }
  //
  m_kappa4  = value ;
  //
  return false ;
}
// ============================================================================


// ============================================================================
// constructor from two masses
// ============================================================================
Gaudi::Math::PhaseSpace2::PhaseSpace2
( const double m1 , 
  const double m2 ) 
  : std::unary_function<double,double> () 
  , m_m1 ( std::abs ( m1 ) ) 
  , m_m2 ( std::abs ( m2 ) ) 
{}
// ============================================================================
// desctructor 
// ============================================================================
Gaudi::Math::PhaseSpace2::~PhaseSpace2(){}
// ============================================================================
// evaluate 2-body phase space 
// ============================================================================
double Gaudi::Math::PhaseSpace2::operator () ( const double x ) const 
{ return phasespace ( x , m_m1 , m_m2 ) ; }
// ============================================================================
/* calculate the phase space for   m -> m1 + m2 
 *  \f$ \Phi = \frac{1}{8\pi} \frac{ \lambda^{\frac{1}{2}} \left( m^2 , m_1^2, m_2_2 \right) }{ m^2 }\f$, 
 *  where \f$\lambda\f$ is a triangle function 
 */
// ============================================================================
double Gaudi::Math::PhaseSpace2::phasespace 
( const double         m  , 
  const double         m1 , 
  const double         m2 , 
  const unsigned short L  ) 
{
  //
  if ( 0 >= m || 0 > m1 || 0 > m2 ) { return 0 ; } // RETURN  
  if ( m < m1 + m2                ) { return 0 ; } // RETURN 
  //
  const double msq = m * m ;
  const double lam = triangle ( msq  , m1 * m1 , m2 * m2 ) ;
  //
  static const double s_inv8pi = 1.0 / ( 8 * M_PI ) ;
  // 
  return 0 < lam ? 
    s_inv8pi * Gaudi::Math::pow ( std::sqrt ( lam ) / msq , 2 * L + 1 ) : 0.0 ;
}
// ============================================================================
/*  calculate the triangle function 
 *  \f$ \lambda ( a , b, c ) = a^2 + b^2 + c^2 - 2ab - 2bc - 2 ca \f$ 
 *  @param a parameter a 
 *  @param b parameter b 
 *  @param c parameter b 
 */
// ============================================================================
double 
Gaudi::Math::PhaseSpace2::triangle
( const double a , 
  const double b , 
  const double c ) 
{ return a * a + b * b + c * c - 2 * a * b - 2 * b * c - 2 * a * c ; }
// ============================================================================
/*  calculate the particle momentum in rest frame 
 *  @param m  the mass 
 *  @param m1 the mass of the first particle 
 *  @param m2 the mass of the second particle 
 *  @return the momentum in rest frame (physical values only)
 */
// ============================================================================
double Gaudi::Math::PhaseSpace2::q 
( const double m  , 
  const double m1 , 
  const double m2 ) 
{
  // 
  if ( 0 >= m || 0 > m1 || 0 > m2 ) { return 0 ; }
  //
  const double lam = triangle ( m * m  , m1 * m1 , m2 * m2 ) ;
  //
  return 0 < lam ? 0.5  * std::sqrt (  lam ) / m : 0 ;  
}
// ============================================================================
/** calculate the particle momentum in rest frame 
 *  @param m the mass 
 *  @param m1 the mass of the first particle 
 *  @param m2 the mass of the second particle 
 *  @return the momentum in rest frame 
 *  @return the momentum in rest frame  (imaginary for non-physical branch)
 */
// ============================================================================
std::complex<double>
Gaudi::Math::PhaseSpace2::q1 
( const double m  , 
  const double m1 , 
  const double m2 ) 
{
  // 
  if ( 0 >= m || 0 > m1 || 0 > m2 ) { return 0 ; }
  //
  const double lam = triangle ( m * m , m1 * m1 , m2 * m2 ) ;
  //
  return 
    0 <= lam ? 
    std::complex<double> (     0.5  * std::sqrt (  lam ) / m , 0 ) :
    std::complex<double> ( 0 , 0.5  * std::sqrt ( -lam ) / m     ) ;
}
// ============================================================================

// ============================================================================
/*  constructor from three masses
 *  @param m1 the mass of the first  particle 
 *  @param m2 the mass of the second particle 
 *  @param m3 the mass of the third  particle 
 *  @param l1 the angular momentum between 1st and 2nd particle 
 *  @param l2 the angular momentum between the pair and 3rd particle 
 */
// ============================================================================
Gaudi::Math::PhaseSpace3::PhaseSpace3
( const double         m1 , 
  const double         m2 , 
  const double         m3 , 
  const unsigned short l1 , 
  const unsigned short l2 ) 
  : std::unary_function<double,double> () 
  , m_m1 ( std::abs ( m1 ) ) 
  , m_m2 ( std::abs ( m2 ) ) 
  , m_m3 ( std::abs ( m3 ) ) 
  , m_l1 ( l1 ) 
  , m_l2 ( l2 ) 
{}
// ============================================================================
// deststructor 
// ============================================================================
Gaudi::Math::PhaseSpace3::~PhaseSpace3 () {}
// ============================================================================
// evaluate 3-body phase space 
// ============================================================================
double Gaudi::Math::PhaseSpace3::operator () ( const double x ) const 
{
  //
  if ( x <= lowEdge() ) { return 0 ; }
  //
  /// set the temporary mass 
  m_tmp = x ;
  //
  // make integral of ps2_aux from m_m1+m_m2 till x-m_m3
  //
  const double low  = m_m1 + m_m2 ;
  const double high = x    - m_m3 ;
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function = &phase_space_3_1_GSL ;
  const PhaseSpace3* _ps = this  ;
  F.params   = const_cast<PhaseSpace3*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag 
    ( &F                ,            // the function 
      low   , high      ,            // low & high edges 
      s_PRECISION       ,            // absolute precision            
      s_PRECISION       ,            // relative precision 
      s_SIZE            ,            // size of workspace 
      GSL_INTEG_GAUSS31 ,            // integration rule  
      workspace ( m_workspace ) ,    // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::PhaseSpace3::QAG" ,
                __FILE__ , __LINE__ , ierror ) ; 
  }
  //
  return result ;
}
// ============================================================================
// helper function to get the phase space as 
// ============================================================================
double Gaudi::Math::PhaseSpace3::ps2_aux 
( const double m12 ) const
{
  //
  if ( m_tmp <= lowEdge()    ) { return 0 ; }
  //
  if ( m12   <= m_m1  + m_m2 ) { return 0 ; }
  if ( m12   >= m_tmp - m_m3 ) { return 0 ; }
  //  
  // represent 3-body phase space as extention of 2-body phase space 
  return  m12 / M_PI *  
    Gaudi::Math::PhaseSpace2::phasespace ( m12   , m_m1 , m_m2 , m_l1 ) * 
    Gaudi::Math::PhaseSpace2::phasespace ( m_tmp , m12  , m_m3 , m_l2 ) ;
  //  
}
// ============================================================================
// get the integral between low and high limits 
// ============================================================================
double  Gaudi::Math::PhaseSpace3::integral 
( const double low  , 
  const double high ) const 
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN 
  if (           low > high   ) { return - integral ( high ,                                                     
                                                      low  ) ; } // RETURN 
  //
  if ( lowEdge() >= high  ) { return 0 ; }
  if ( lowEdge() >  low   ) { return integral ( lowEdge() , high ) ; }
  
  //
  if ( 0 < lowEdge() && 5 * lowEdge() < ( high - low ) ) 
  {
    return 
      integral ( low                   , 0.5 *  ( high + low ) ) + 
      integral ( 0.5 *  ( high + low ) ,          high         ) ;
  }
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function = &phase_space_3_2_GSL ;
  const PhaseSpace3* _ps = this  ;
  F.params   = const_cast<PhaseSpace3*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag 
    ( &F                ,             // the function 
      low   , high      ,             // low & high edges 
      s_PRECISION       ,             // absolute precision            
      s_PRECISION       ,             // relative precision 
      s_SIZE            ,             // size of workspace 
      GSL_INTEG_GAUSS31 ,             // integration rule  
      workspace ( m_workspace2 ) ,    // workspace  
      &result           ,             // the result 
      &error            ) ;           // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::PhaseSpace3::QAG" ,
                __FILE__ , __LINE__ , ierror ) ; 
  }
  //
  return result ;
}
// ============================================================================
// constructor from threshold and number of particles 
// ============================================================================
Gaudi::Math::PhaseSpaceLeft::PhaseSpaceLeft 
( const double         threshold , 
  const unsigned short num       ) 
  : std::unary_function<double,double> () 
  , m_threshold ( std::abs ( threshold ) ) 
  , m_num       ( num ) 
{}
// ============================================================================
// constructor from list of masses
// ============================================================================
Gaudi::Math::PhaseSpaceLeft::PhaseSpaceLeft 
( const std::vector<double>& masses ) 
  : std::unary_function<double,double> () 
  , m_threshold ( 0              ) 
  , m_num       ( masses.size()  ) 
{
  //
  for ( std::vector<double>::const_iterator im = masses.begin() ; 
        masses.end() != im ; ++im ) 
  { 
    m_threshold += std::abs ( *im ) ;
  }
  //
}
// ============================================================================
// desctructor 
// ============================================================================
Gaudi::Math::PhaseSpaceLeft::~PhaseSpaceLeft(){}
// ============================================================================
// evaluate N-body phase space near left threhsold 
// ============================================================================
double Gaudi::Math::PhaseSpaceLeft::operator () ( const double x ) const 
{
  //
  if ( m_threshold >= x ) { return 0 ; }
  //
  return std::pow ( x - m_threshold , 3 * 0.5 * m_num - 5 * 0.5  ) ;
}
// ============================================================================
// set the new value for threshold
// ============================================================================
bool Gaudi::Math::PhaseSpaceLeft::setThreshold ( const double x ) 
{
  //
  if ( s_equal ( x , m_threshold ) ) { return false ; } // RETURN
  //
  m_threshold = x ;
  //
  return true ;
  //
}
// ============================================================================
// constructor from threshold and number of particles 
// ============================================================================
Gaudi::Math::PhaseSpaceRight::PhaseSpaceRight 
( const double         threshold , 
  const unsigned short l         , 
  const unsigned short n         ) 
  : std::unary_function<double,double> () 
  , m_threshold ( std::abs ( threshold ) ) 
  , m_N         ( std::max ( l , n ) ) 
  , m_L         ( std::min ( l , n ) ) 
{}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::PhaseSpaceRight::~PhaseSpaceRight (){}
// ============================================================================
// evaluate N-body phase space near right threshold 
// ============================================================================
double Gaudi::Math::PhaseSpaceRight::operator () ( const double x ) const 
{
  //
  if ( m_threshold <= x ) { return 0 ; }
  //
  return std::pow ( m_threshold - x , 1.5 * ( m_N - m_L ) - 1  ) ;
}
// ============================================================================
// set the new value for threshold
// ============================================================================
bool Gaudi::Math::PhaseSpaceRight::setThreshold ( const double x )
{
  //
  if ( s_equal ( x , m_threshold ) ) { return false ; } // RETURN
  //
  m_threshold = x ;
  //
  return true ;
  //
}

// ============================================================================
// constructor from thresholds and number of particles 
// ============================================================================
Gaudi::Math::PhaseSpaceNL::PhaseSpaceNL 
( const double         threshold1 , 
  const double         threshold2 , 
  const unsigned short l          , 
  const unsigned short n          ) 
  : std::unary_function<double,double> () 
  , m_threshold1 ( std::min ( std::abs ( threshold1 ) , std::abs ( threshold2 ) ) ) 
  , m_threshold2 ( std::max ( std::abs ( threshold1 ) , std::abs ( threshold2 ) ) )                           
  , m_N          ( std::max ( l , n ) ) 
  , m_L          ( std::min ( l , n ) ) 
  , m_norm       ( 1 ) 
//
  , m_workspace  () 
//
{
  if ( ( 3 * m_N * 0.5 - 3       * 0.5 ) < GSL_SF_GAMMA_XMAX && 
       ( 3 * m_L * 0.5 - 3       * 0.5 ) < GSL_SF_GAMMA_XMAX && 
       ( 3 * m_N * 0.5 - 3 * m_L * 0.5 ) < GSL_SF_GAMMA_XMAX ) 
  {
    m_norm  = gsl_sf_gamma   ( 3 * m_N * 0.5 - 3       * 0.5 ) ;
    m_norm /= gsl_sf_gamma   ( 3 * m_L * 0.5 - 3       * 0.5 ) ;
    m_norm /= gsl_sf_gamma   ( 3 * m_N * 0.5 - 3 * m_L * 0.5 ) ;
  }
  else
  {
    m_norm  = gsl_sf_lngamma ( 3 * m_N * 0.5 - 3       * 0.5 ) ;
    m_norm -= gsl_sf_lngamma ( 3 * m_L * 0.5 - 3       * 0.5 ) ;
    m_norm -= gsl_sf_lngamma ( 3 * m_N * 0.5 - 3 * m_L * 0.5 ) ;
    m_norm  = gsl_sf_exp     ( m_norm ) ;
  }     
}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::PhaseSpaceNL::~PhaseSpaceNL() {}
// ============================================================================
// evaluate N/L-body phase space
// ============================================================================
double Gaudi::Math::PhaseSpaceNL::operator () ( const double x ) const 
{
  //
  if ( m_threshold1 >= x ) { return 0 ; }
  if ( m_threshold2 <= x ) { return 0 ; }
  //
  const double y = (  x - m_threshold1 ) / ( m_threshold2 - m_threshold1 ) ;
  if ( 0 >= y || 1 <= y )  { return 0 ; }
  //
  return m_norm 
    / std::abs ( m_threshold2 - m_threshold1               ) 
    * std::pow (     y , 3 * 0.5 *   m_L         - 5 * 0.5 ) 
    * std::pow ( 1 - y , 3 * 0.5 * ( m_N - m_L ) - 1       ) ; 
}
// =======================================================================
// set the thresholds 
// =======================================================================
bool Gaudi::Math::PhaseSpaceNL::setThresholds 
( const double mn , 
  const double mx ) 
{
  const double v1 = std::min ( std::abs ( mn ) ,std::abs ( mx ) ) ;
  const double v2 = std::max ( std::abs ( mn ) ,std::abs ( mx ) ) ;
  //
  if ( s_equal ( v1 , m_threshold1 ) && 
       s_equal ( v2 , m_threshold2 ) ) { return false ; }
  //
  m_threshold1 = v1 ;
  m_threshold2 = v2 ;
  //
  return true ;
}
// ============================================================================
// get the integral between low and high limits 
// ============================================================================
double  Gaudi::Math::PhaseSpaceNL::integral 
( const double low  , 
  const double high ) const 
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN 
  if (           low > high   ) { return - integral ( high ,                                                     
                                                      low  ) ; } // RETURN 
  //
  if ( m_threshold2 <= low  ) { return 0 ; }
  if ( m_threshold1 >= high ) { return 0 ; }
  //
  if ( m_threshold1 >  low  ) { return integral ( m_threshold1 ,  high        ) ; }
  if ( m_threshold2 <  high ) { return integral ( low          , m_threshold2 ) ; }
  //
  // split, if the interval is too large
  //
  const double width = 0.2 * std::abs  ( m_threshold2 - m_threshold1 ) ;
  if ( 0 < width &&  width < high - low  ) 
  {
    return 
      integral ( low                   , 0.5 *  ( high + low ) ) + 
      integral ( 0.5 *  ( high + low ) ,          high         ) ;
  }
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function = &phase_space_NL_GSL ;
  const PhaseSpaceNL* _ps = this  ;
  F.params   = const_cast<PhaseSpaceNL*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag 
    ( &F                ,            // the function 
      low   , high      ,            // low & high edges 
      s_PRECISION       ,            // absolute precision            
      s_PRECISION       ,            // relative precision 
      s_SIZE            ,            // size of workspace 
      GSL_INTEG_GAUSS31 ,            // integration rule  
      workspace ( m_workspace ) ,    // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::PhaseSpaceNL::QAG" ,
                __FILE__ , __LINE__ , ierror ) ; 
  }
  //
  return result ;
}
// ============================================================================
// get the integral
// ============================================================================
double  Gaudi::Math::PhaseSpaceNL::integral() const
{ return integral ( m_threshold1 , m_threshold2 ) ; }
// ============================================================================
// constructor 
// ============================================================================
Gaudi::Math::BreitWigner::BreitWigner
( const double         m0   , 
  const double         gam0 ,
  const double         m1   , 
  const double         m2   , 
  const unsigned short L    ) 
  : std::unary_function<double,double> () 
//
  , m_m0        (             m0    )   
  , m_gam0      ( std::abs ( gam0 ) ) 
  , m_m1        ( std::abs (   m1 ) )  
  , m_m2        ( std::abs (   m2 ) )  
  , m_L         (              L    )   
  , m_rho_fun   ( &Gaudi::Math::Jackson::jackson_0 )   
//
  , m_workspace () 
//
{}
// ============================================================================
// constructor 
// ============================================================================ 
Gaudi::Math::BreitWigner::BreitWigner
( const double                                m0   , 
  const double                                gam0 ,
  const double                                m1   , 
  const double                                m2   , 
  const unsigned short                        L    ,
  const Gaudi::Math::BreitWigner::JacksonRho  r    ) 
  : std::unary_function<double,double> ()  
//
  , m_m0        (             m0    )   
  , m_gam0      ( std::abs ( gam0 ) ) 
  , m_m1        ( std::abs (   m1 ) )  
  , m_m2        ( std::abs (   m2 ) )  
  , m_L         (              L    )   
//
  , m_rho_fun   ( &Gaudi::Math::Jackson::jackson_0 )   
//
  , m_workspace () 
//
{
  //
  switch ( r ) 
  {
  case Jackson_0  : 
    m_rho_fun = &Gaudi::Math::Jackson::jackson_0  ; break ;
  case Jackson_A2 : 
    m_rho_fun = &Gaudi::Math::Jackson::jackson_A2 ; break ;
  case Jackson_A3 : 
    m_rho_fun = &Gaudi::Math::Jackson::jackson_A3 ; break ;
  case Jackson_A4 : 
    m_rho_fun = &Gaudi::Math::Jackson::jackson_A4 ; break ;
  case Jackson_A5 : 
    m_rho_fun = &Gaudi::Math::Jackson::jackson_A5 ; break ;
  case Jackson_A7 : 
    m_rho_fun = &Gaudi::Math::Jackson::jackson_A7 ; break ;
  default         : 
    m_rho_fun = &Gaudi::Math::Jackson::jackson_0  ; break ;
  }
  //
}
// ============================================================================
// destructor
// ============================================================================ 
Gaudi::Math::BreitWigner::~BreitWigner (){}
// ============================================================================
//  calculate the Breit-Wigner amplitude  
// ============================================================================
std::complex<double> 
Gaudi::Math::BreitWigner::amplitude ( const double x ) const 
{
  //
  if ( m_m1 + m_m2 >= x ) { return 0 ; }
  //
  const double g  = gamma ( x ) ;
  if ( 0 >= g ) { return 0 ; }
  //
  static const std::complex<double> s_j ( 0 , 1 ) ;
  //
  const std::complex<double> v = m0() * m0 () - x * x - s_j * m0() * g ;
  //
  const double q  = Gaudi::Math::PhaseSpace2::q ( x    , m1() , m2() ) ;
  const double q0 = Gaudi::Math::PhaseSpace2::q ( m0() , m1() , m2() ) ;
  //
  return  Gaudi::Math::pow ( q / q0 , m_L ) / v ;
}
// ============================================================================
/*  calculate the Breit-Wigner shape
 *  \f$\frac{1}{\pi}\frac{\omega\Gamma(\omega)}{ (\omega_0^2-\omega^2)^2-\omega_0^2\Gammma^2(\omega)-}\f$
 */
// ============================================================================
double Gaudi::Math::BreitWigner::breit_wigner ( const double x ) const 
{
  //
  if ( m_m1 + m_m2 >= x ) { return 0 ; }
  //
  const double g  = gamma ( x ) ;
  if ( 0 >= g ) { return 0 ; }
  //
  const double omega2 = m_m0 * m_m0 ;
  const double delta = omega2        -          x * x ;
  const double v     = delta * delta + omega2 * g * g ;
  //
  return 2 * x * m_m0 * g / v / M_PI  ;
}
// ============================================================================
/*  calculate the Breit-Wigner shape
 *  \f$\frac{1}{\pi}\frac{\omega\Gamma(\omega)}{ (\omega_0^2-\omega^2)^2-\omega_0^2\Gammma^2(\omega)-}\f$
 */
// ============================================================================
double Gaudi::Math::BreitWigner::operator() ( const double x ) const 
{ return breit_wigner ( x ) ; }
// ============================================================================ 
// calculate the current width 
// ============================================================================ 
double Gaudi::Math::BreitWigner::gamma ( const double x ) const 
{
  //
  return gamma_run ( m_gam0    , 
                     x         , 
                     m_m1      , 
                     m_m2      , 
                     m_m0      , 
                     m_L       , 
                     m_rho_fun ) ;
  //
}
// ============================================================================
bool Gaudi::Math::BreitWigner::setM0     ( const double x ) 
{
  const double v       = std::abs ( x ) ;
  if ( s_equal ( v , m_m0 ) ) { return false ; } // RETURN
  m_m0   = v ; 
  return true ;
}
// ============================================================================
bool Gaudi::Math::BreitWigner::setGamma0 ( const double x ) 
{
  const double v       = std::abs ( x ) ;
  if ( s_equal ( v , m_gam0 ) ) { return false ; } // RETURN
  m_gam0  = v ; 
  return true ;
}   
// ============================================================================
// get the integral between low and high limits 
// ============================================================================
double  Gaudi::Math::BreitWigner::integral 
( const double low  , 
  const double high ) const 
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN 
  if (           low > high   ) { return - integral ( high ,                                                     
                                                      low  ) ; } // RETURN 
  //
  if ( m_m1 + m_m2 >= high ) { return                              0   ; }
  if ( m_m1 + m_m2 >  low  ) { return integral  ( m_m1 + m_m2 , high ) ; }
  //
  //
  // split into reasonable sub intervals
  //
  const double x1     = m_m0 - 10 * m_gam0 ;
  const double x2     = m_m0 + 10 * m_gam0  ;
  const double x_low  = std::min ( x1 , x2 ) ;
  const double x_high = std::max ( x1 , x2 ) ;
  //
  if ( low < x_low  && x_low < high ) 
  {
    return 
      integral (   low , x_low  ) + 
      integral ( x_low ,   high ) ;
  }
  if ( low <  x_high && x_high < high ) 
  {
    return 
      integral (   low  , x_high  ) + 
      integral ( x_high ,   high  ) ;
  }
  //
  // split, if interval too large
  //
  const double width = std::max ( m_gam0 , 0.0 ) ;
  if ( 0 < width &&  3 * width < high - low  ) 
  {
    return 
      integral ( low                   , 0.5 *  ( high + low ) ) + 
      integral ( 0.5 *  ( high + low ) ,          high         ) ;
  }
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function = &breit_wigner_GSL ;
  const BreitWigner* _bw = this  ;
  F.params   = const_cast<BreitWigner*> ( _bw ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag 
    ( &F                ,            // the function 
      low   , high      ,            // low & high edges 
      s_PRECISION       ,            // absolute precision            
      ( high   <= x_low  ) ? s_PRECISION_TAIL :
      ( x_high <=   low  ) ? s_PRECISION_TAIL : 
      s_PRECISION       ,            // relative precision 
      s_SIZE            ,            // size of workspace 
      GSL_INTEG_GAUSS31 ,            // integration rule  
      workspace ( m_workspace ) ,    // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::BreitWigner::QAG" ,
                __FILE__ , __LINE__ , ierror ) ; 
  }
  //
  return result ;
}
// ============================================================================
// get the integral b
// ============================================================================
double  Gaudi::Math::BreitWigner::integral () const 
{
  //
  // split into reasonable sub intervals
  //
  const double x1     = m_m0 - 10 * m_gam0 ;
  const double x2     = m_m0 + 10 * m_gam0  ;
  const double x_high = std::max ( x1 , x2 ) ;
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function = &breit_wigner_GSL ;
  const BreitWigner* _bw = this  ;
  F.params   = const_cast<BreitWigner*> ( _bw ) ;
  //
  //
  // right tail:
  //
  double result  =  0.0 ;
  double error   = -1.0 ;
  //
  const int ierror = gsl_integration_qagiu
    ( &F                ,         // the function 
      x_high            ,         // "low" edge
      s_PRECISION       ,         // absolute precision  
      s_PRECISION_TAIL  ,         // relative precision 
      s_SIZE            ,         // size of workspace 
      workspace ( m_workspace ) , // workspace  
      &result           ,         // the result 
      &error            ) ;       // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::BreitWigner::QAGIU" ,
                __FILE__ , __LINE__ , ierror ) ; 
    result = 0.0 ;
  }
  //
  return result + integral ( m_m1 + m_m2 , x_high );
}
// ============================================================================
// constructor from all parameters
// ============================================================================
Gaudi::Math::Rho0::Rho0
( const double m0       , 
  const double gam0     ,
  const double pi_mass  ) 
  : Gaudi::Math::BreitWigner ( m0         , 
                               gam0       ,
                               pi_mass    , 
                               pi_mass    ,
                               1          , 
                               Jackson_A7 ) 
{}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::Rho0::~Rho0(){}


// ============================================================================
// constructor from all parameters
// ============================================================================
Gaudi::Math::Rho0FromEtaPrime::Rho0FromEtaPrime
( const double m0        , 
  const double gam0      ,
  const double pi_mass   , 
  const double eta_prime ) 
  : Gaudi::Math::Rho0 ( m0 , gam0 , pi_mass ) 
  , m_eta_prime ( std::abs ( eta_prime ) ) 
{}
// ============================================================================
// constructor from all parameters
// ============================================================================
Gaudi::Math::Rho0FromEtaPrime::Rho0FromEtaPrime
( const Gaudi::Math::Rho0& rho       , 
  const double             eta_prime ) 
  : Gaudi::Math::Rho0 ( rho ) 
  , m_eta_prime ( std::abs ( eta_prime ) ) 
{}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::Rho0FromEtaPrime::~Rho0FromEtaPrime(){}
// ============================================================================
// calculate the function 
// ============================================================================
double Gaudi::Math::Rho0FromEtaPrime::operator() ( const double x ) const 
{
  //
  if ( m_eta_prime <= x ) { return 0 ; }
  //
  const double k_gamma = Gaudi::Math::PhaseSpace2::q ( m_eta_prime , x , 0 ) ;
  if ( 0 >= k_gamma     ) { return 0 ; }
  //
  const double rho     = breit_wigner ( x ) ;
  if ( 0 >= rho         ) { return 0 ; }
  //
  return rho * Gaudi::Math::pow ( 2 * k_gamma / m_eta_prime , 3 ) * 20 ;
  //
}
// ============================================================================
//               Flatte
// ============================================================================
/* constructor  from three parameters 
 *  @param m0    the mass 
 *  @param m0g1  parameter \f$ m_0\times g_1\f$
 *  @param g2og2 parameter \f$ g2/g_1       \f$
 */
// ============================================================================
Gaudi::Math::Flatte::Flatte
( const double m0    , 
  const double m0g1  , 
  const double g2og1 ,
  const double mK    , 
  const double mPi   ) 
  : std::unary_function<double,double> () 
//
  , m_m0    ( std::fabs ( m0    ) ) 
  , m_m0g1  ( std::fabs ( m0g1  ) )  
  , m_g2og1 ( std::fabs ( g2og1 ) )  
  , m_K     ( std::fabs ( mK    ) )  
  , m_Pi    ( std::fabs ( mPi   ) )  
//
  , m_workspace ()  
{}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::Flatte::~Flatte(){}
// ============================================================================
// get the value of Flatte function 
// ============================================================================
double Gaudi::Math::Flatte::operator() ( const double x ) const
{ return flatte ( x ) ; }
// ============================================================================
// get the complex Flatte amplitude (pipi-channel)
// ============================================================================
std::complex<double> Gaudi::Math::Flatte::amplitude 
( const double x     )  const { return flatte_amp ( x ) ; }
// ============================================================================
// get the complex Flatte amplitude (pipi-channel)
// ============================================================================
std::complex<double> Gaudi::Math::Flatte::flatte_amp 
( const double x     )  const 
{
  //
  if ( 2 * mPi () >= x ) { return 0 ; }
  //
  const std::complex<double> rho_PP = 
    Gaudi::Math::PhaseSpace2::q1 ( x , mPi() , mPi() )  ;
  const std::complex<double> rho_KK = 
    Gaudi::Math::PhaseSpace2::q1 ( x , mK () , mK () )  ;
  //
  static const std::complex<double> s_j ( 0 , 1 ) ;
  //
  const std::complex<double> v = 
    m0() * m0 () - x * x - s_j * m0g1() * ( rho_PP + g2og1 () * rho_KK ) ;
  //
  return  1.0 / v ;
}
// ==========================================================================
// get the complex Flatte amplitude (KK-channel)
// ==========================================================================
std::complex<double> 
Gaudi::Math::Flatte::flatte2_amp 
( const double x ) const 
{
  //
  if ( 2 * mK() >= x ) { return 0 ; }
  //
  const std::complex<double> rho_PP = 
    Gaudi::Math::PhaseSpace2::q1 ( x , mPi () , mPi () ) ;
  const std::complex<double> rho_KK = 
    Gaudi::Math::PhaseSpace2::q1 ( x , mK  () , mK  () ) ;
  //
  static const std::complex<double> s_j ( 0 , 1 ) ;
  //
  const std::complex<double> v = 
    m0 () * m0 () - x * x - s_j * m0g1 () * ( rho_PP + g2og1 () * rho_KK ) ;
  //
  return  1.0 / v ;
}
// ==========================================================================
// get the function for pipi-channel  
// ============================================================================
double Gaudi::Math::Flatte::flatte ( const double x ) const
{
  //
  if ( 2 * mPi () >= x ) { return 0 ; }
  //
  // get the amplitude...
  std::complex<double> amp = flatte_amp ( x ) ;
  //
  const double ps = Gaudi::Math::PhaseSpace2::phasespace ( x ,  mPi() , mPi() ) ;
  //
  return x * ps * std::norm ( amp ) * 2 / M_PI * m0g1() ;
} 
// ==========================================================================
// get the function for KK-channel  
// ============================================================================
double Gaudi::Math::Flatte::flatte2 ( const double x ) const
{
  //
  if ( 2 * mK () >= x ) { return 0 ; }
  //
  // get the amplitude...
  std::complex<double> amp = flatte2_amp ( x ) ;
  //
  const double ps = Gaudi::Math::PhaseSpace2::phasespace ( x ,  mK() , mK() ) ;
  //
  return x * ps * std::norm ( amp ) * 2 / M_PI * m0g1 () * g2og1 () ;
} 
// ============================================================================
// get the integral between low and high limits 
// ============================================================================
double  Gaudi::Math::Flatte::integral 
( const double low  , 
  const double high ) const 
{
  //
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN 
  if (           low > high   ) { return - integral ( high ,                                                     
                                                      low  ) ; } // RETURN 
  //
  if ( 2 * m_Pi >= high ) { return                           0   ; } 
  if ( 2 * m_Pi >  low  ) { return integral  ( 2 * m_Pi , high ) ; }
  //
  // split into reasonable sub intervals
  //
  const double x_low  = m_m0 
    -  3 * std::abs ( m_m0g1 / m_m0           )
    -  3 * std::abs ( m_m0g1 / m_m0 * m_g2og1 ) ;
  //
  const double x_high = m_m0 
    + 10 * std::abs ( m_m0g1 / m_m0           )
    + 10 * std::abs ( m_m0g1 / m_m0 * m_g2og1 ) ;
  //
  if      ( low <  x_low  && x_low  < high )
  {
    return 
      integral (   low  , x_low   ) + 
      integral ( x_low  ,   high  ) ;
  }
  else if ( low <  x_high && x_high < high ) 
  {
    return 
      integral (   low  , x_high  ) + 
      integral ( x_high ,   high  ) ;
  }
  //
  // split, if interval too large
  //
  const double width =  
    0 > m_m0 ? 0.0 : 
    std::fabs ( m_m0g1 / m_m0 ) +
    std::fabs ( m_m0g1 / m_m0 * m_g2og1 ) ;  
  //
  if ( 0 < width &&  3 * width < high - low  ) 
  {
    return 
      integral ( low                   , 0.5 *  ( high + low ) ) + 
      integral ( 0.5 *  ( high + low ) ,          high         ) ;
  }
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function = &flatte_GSL ;
  const Flatte* _f = this  ;
  F.params   = const_cast<Flatte*> ( _f ) ;
  //
  //
  double result   =  1.0 ;
  double error    = -1.0 ;
  //
  const int ierror = gsl_integration_qag 
    ( &F                ,            // the function 
      low   , high      ,            // low & high edges 
      s_PRECISION       ,            // absolute precision            
      ( high   <= x_low  ) ? s_PRECISION_TAIL :
      ( x_high <=   low  ) ? s_PRECISION_TAIL : 
      s_PRECISION       ,            // relative precision 
      s_SIZE            ,            // size of workspace 
      GSL_INTEG_GAUSS31 ,            // integration rule  
      workspace ( m_workspace ) ,    // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::Flatte::QAG" ,
                __FILE__ , __LINE__ , ierror ) ; 
  }
  //
  return result ;
}
// ============================================================================
// get the integral b
// ============================================================================
double  Gaudi::Math::Flatte::integral () const 
{
  //
  // split into reasonable sub intervals
  //
  const double x_high = m_m0 
    + 10 * std::abs ( m_m0g1 / m_m0           )
    + 10 * std::abs ( m_m0g1 / m_m0 * m_g2og1 ) ;
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function = &flatte_GSL ;
  const Flatte* _f = this  ;
  F.params   = const_cast<Flatte*> ( _f ) ;
  //
  // right tail:
  //
  double result  =  0.0 ;
  double error   = -1.0 ;
  //
  const int ierror = gsl_integration_qagiu
    ( &F                ,         // the function 
      x_high            ,         // "low" edge
      s_PRECISION       ,         // absolute precision  
      s_PRECISION_TAIL  ,         // relative precision 
      s_SIZE            ,         // size of workspace 
      workspace ( m_workspace ) , // workspace  
      &result           ,         // the result 
      &error            ) ;       // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::Flatte::QAGIU" ,
                __FILE__ , __LINE__ , ierror ) ; 
    result = 0.0 ;
  }
  //
  return result + integral ( 2 * m_Pi , x_high );
}
// ============================================================================
// set mass 
// ============================================================================
bool Gaudi::Math::Flatte::setM0     ( const double x ) 
{ 
  const double v = std::abs ( x ) ;
  //
  if ( s_equal ( v , m_m0 ) ) { return false ; }
  //
  m_m0 = v ;
  //
  return true ;
}
// ============================================================================
// set mass times G1 
// ============================================================================
bool Gaudi::Math::Flatte::setM0G1   ( const double x ) 
{
  const double v = std::abs ( x ) ;
  //
  if ( s_equal ( v , m_m0g1 ) ) { return false ; }
  //
  m_m0g1 = v ;
  //
  return true ;
}
// ============================================================================
// set G2 over G1 
// ============================================================================
bool Gaudi::Math::Flatte::setG2oG1  ( const double x ) 
{
  const double v = std::abs ( x ) ;
  //
  if ( s_equal ( v , m_g2og1 ) ) { return false ; }
  //
  m_g2og1 = v ;
  //
  return true ;
}
// ============================================================================



// ============================================================================
//               Flatte
// ============================================================================
/* constructor  from three parameters 
 *  @param m0    the mass 
 *  @param m0g1  parameter \f$ m_0\times g_1\f$
 *  @param g2og2 parameter \f$ g2/g_1       \f$
 */
// ============================================================================
Gaudi::Math::Flatte2::Flatte2
( const double m0    , 
  const double m0g1  , 
  const double g2og1 ,
  const double mK    , 
  const double mPi   )
  : Gaudi::Math::Flatte ( m0 , m0g1 , g2og1 ,mK , mPi ) 
{}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::Flatte2::~Flatte2(){}
// ============================================================================
// get the value of Flatte function 
// ============================================================================
double Gaudi::Math::Flatte2::operator() ( const double x ) const
{ return flatte2 ( x ) ; }
// ============================================================================
// get the complex Flatte amplitude (pipi-channel)
// ============================================================================
std::complex<double> Gaudi::Math::Flatte2::amplitude 
( const double x     )  const { return flatte2_amp ( x ) ; }
// ============================================================================


// ============================================================================
// Voigtian
// ============================================================================
// constructor  from all parameters
// ============================================================================
Gaudi::Math::Voigt::Voigt
( const double m0    , 
  const double gamma , 
  const double sigma ,  
  const int    r     ) 
  : std::unary_function<double,double>() 
//
  , m_m0        ( m0 ) 
  , m_gamma     ( std::abs ( gamma ) ) 
  , m_sigma     ( std::abs ( sigma ) )
  , m_r         ( std::min ( std::max ( r , 2 ) , 5 ) )  
//
  , m_workspace () 
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::Voigt::~Voigt(){}
// ============================================================================
// get the value of Voigt function 
// ============================================================================
double Gaudi::Math::Voigt::operator() ( const double x ) const 
{ return TMath::Voigt ( x - m_m0 , m_sigma , m_gamma , m_r ) ; }
// ============================================================================
// get the integral between low and high limits 
// ============================================================================
double  Gaudi::Math::Voigt::integral 
( const double low  , 
  const double high ) const 
{
  //
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN 
  if (           low > high   ) { return - integral ( high ,                                                     
                                                      low  ) ; } // RETURN 
  //
  const double width = std::max ( m_sigma , m_gamma ) ;
  //
  // split into reasonable sub intervals
  //
  const double x_low   = m_m0 - 3 * width ;
  const double x_high  = m_m0 + 3 * width ;
  //
  if      ( low <  x_low  && x_low  < high )
  {
    return 
      integral (   low  , x_low   ) + 
      integral ( x_low  ,   high  ) ;
  }
  else if ( low <  x_high && x_high < high ) 
  {
    return 
      integral (   low  , x_high  ) + 
      integral ( x_high ,   high  ) ;
  }
  //
  // split, if interval too large
  //
  if ( 0 < width && 3 * width < high - low  ) 
  {
    return 
      integral ( low                   , 0.5 *  ( high + low ) ) + 
      integral ( 0.5 *  ( high + low ) ,          high         ) ;
  }
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function = &voigt_GSL ;
  const Voigt* _f = this  ;
  F.params   = const_cast<Voigt*> ( _f ) ;
  //
  //
  double result   =  1.0 ;
  double error    = -1.0 ;
  //
  const int ierror = gsl_integration_qag 
    ( &F                ,            // the function 
      low   , high      ,            // low & high edges 
      s_PRECISION       ,            // absolute precision            
      ( high   <= x_low  ) ? s_PRECISION_TAIL :
      ( x_high <=   low  ) ? s_PRECISION_TAIL : 
      s_PRECISION       ,            // relative precision 
      s_SIZE            ,            // size of workspace 
      GSL_INTEG_GAUSS31 ,            // integration rule  
      workspace ( m_workspace ) ,    // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::Voigt::QAG" ,
                __FILE__ , __LINE__ , ierror ) ; 
  }
  //
  return result ;
}
// ============================================================================
// get the integral between low and high limits 
// ============================================================================
double  Gaudi::Math::Voigt::integral () const { return 1 ; }
// ============================================================================
// set the proper parameters 
// ============================================================================
bool Gaudi::Math::Voigt::setM0 ( const double x ) 
{
  //
  if ( s_equal ( x , m_m0 ) ) { return false ; }
  //
  m_m0 = x ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters 
// ============================================================================
bool Gaudi::Math::Voigt::setGamma ( const double x ) 
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_gamma ) ) { return false ; }
  //
  m_gamma = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters 
// ============================================================================
bool Gaudi::Math::Voigt::setSigma ( const double x ) 
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_sigma ) ) { return false ; }
  //
  m_sigma = v ;
  //
  return true ;
}
// ============================================================================
/*  constructor from four masses and angular momenta 
 *  @param m1 the mass of the first  particle 
 *  @param m2 the mass of the second particle 
 *  @param m3 the mass of the third  particle 
 *  @param m4 the mass of the mother particle (m4>m1+m2+m3)
 *  @param L  the angular momentum between the first pair and 
 *  the third particle
 *  @param l  the angular momentum between the first and the second particle
 */
// ============================================================================
Gaudi::Math::PhaseSpace23L::PhaseSpace23L 
( const double         m1 , 
  const double         m2 , 
  const double         m3 , 
  const double         m  , 
  const unsigned short L  ,
  const unsigned short l  ) 
  : std::unary_function<double,double> () 
//
  , m_m1   ( std::abs ( m1 ) ) 
  , m_m2   ( std::abs ( m2 ) ) 
  , m_m3   ( std::abs ( m3 ) ) 
  , m_m    ( std::abs ( m  ) ) 
  , m_l    (            l    )  
  , m_L    (            L    )  
//
  , m_norm ( -1 ) 
//
  , m_workspace  () 
//
{
  m_norm = integral() ;
}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::PhaseSpace23L::~PhaseSpace23L() {}
// get the momentum of 1st particle in rest frame of (1,2)
// ============================================================================
double Gaudi::Math::PhaseSpace23L::q ( const double x ) const 
{ return Gaudi::Math::PhaseSpace2::q ( x , m_m1 , m_m2 ) ; }
// ============================================================================
// get the momentum of 3rd particle in rest frame of mother
// ============================================================================
double Gaudi::Math::PhaseSpace23L::p ( const double x ) const 
{ return Gaudi::Math::PhaseSpace2::q ( m_m , x , m_m3 ) ; }
// ============================================================================
// calculate the phase space
// ============================================================================
double Gaudi::Math::PhaseSpace23L::operator () ( const double x ) const 
{ return ps23L( x ) ; }
// ============================================================================
// calculate the phase space
// ============================================================================
double Gaudi::Math::PhaseSpace23L::ps23L ( const double x ) const 
{
  //
  if ( lowEdge() >= x || highEdge() <= x ) { return  0 ; }
  //
  // represent 3-body phase space as extention of 2-body phase space 
  double ps =  x / M_PI *  
    Gaudi::Math::PhaseSpace2::phasespace ( x   , m_m1 , m_m2 , m_l  ) * 
    Gaudi::Math::PhaseSpace2::phasespace ( m_m ,    x , m_m3 , m_L  ) ;
  //
  return 0 < m_norm ? ps / m_norm : ps ;
}
// ============================================================================
// get the integral between low and high limits 
// ============================================================================
double  Gaudi::Math::PhaseSpace23L::integral 
( const double low  , 
  const double high ) const 
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN 
  if (           low > high   ) { return - integral ( high ,                                                     
                                                      low  ) ; } // RETURN 
  //
  if ( high <= lowEdge  () ) { return 0 ; }
  if ( low  >= highEdge () ) { return 0 ; }
  //
  if ( low  <  lowEdge  () ) { return integral ( lowEdge() , high        ) ; }
  if ( high >  highEdge () ) { return integral ( low       , highEdge () ) ; }
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function               = &phase_space_23L_GSL ;
  const PhaseSpace23L* _ps = this  ;
  F.params                 = const_cast<PhaseSpace23L*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag 
    ( &F                ,            // the function 
      low   , high      ,            // low & high edges 
      s_PRECISION       ,            // absolute precision            
      s_PRECISION       ,            // relative precision 
      s_SIZE            ,            // size of workspace 
      GSL_INTEG_GAUSS31 ,            // integration rule  
      workspace ( m_workspace ) ,    // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::PhaseSpace23L::QAG" ,
                __FILE__ , __LINE__ , ierror ) ; 
  }
  //
  return result ;
}
// ============================================================================
// get the integral 
// ============================================================================
double  Gaudi::Math::PhaseSpace23L::integral () const 
{ return integral ( lowEdge () , highEdge() ) ; }
// ============================================================================


// ============================================================================
// LASS: Kpi S-wave for  X -> (K pi) Y decays..
// ============================================================================
/*  constructor from all masses and angular momenta 
 *  @param m1 the mass of the first  particle 
 *  @param m2 the mass of the second particle 
 *  @param m3 the mass of the third  particle 
 *  @param m  the mass of the mother particle (m>m1+m2+m3)
 *  @param L  the angular momentum between the first pair and the third 
 *  @param a  the LASS parameter 
 *  @param r  the LASS parameter 
 */
// ============================================================================
Gaudi::Math::LASS23L::LASS23L
( const double         m1 , 
  const double         m2 , 
  const double         m3 , 
  const double         m  , 
  const double         m0 , 
  const double         g0 ,
  const unsigned short L  ,
  const double         a  , 
  const double         r  , 
  const double         e  ) 
  : std::unary_function<double,double> () 
  , m_m0 ( std::abs ( m0 ) ) 
  , m_g0 ( std::abs ( g0 ) ) 
  , m_a  ( std::abs ( a  ) ) 
  , m_r  ( std::abs ( r  ) ) 
  , m_e  ( std::abs ( e  ) ) 
// phase space
  , m_ps ( m1 , m2 , m3 , m , L , 0 )  
//
  , m_workspace () 
{}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::LASS23L::~LASS23L(){}
// ============================================================================
// set the proper parameters 
// ============================================================================
bool Gaudi::Math::LASS23L::setM0 ( const double x ) 
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_m0 ) ) { return false ; }
  //
  m_m0 = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters 
// ============================================================================
bool Gaudi::Math::LASS23L::setG0 ( const double x ) 
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_g0 ) ) { return false ; }
  //
  m_g0 = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters 
// ============================================================================
bool Gaudi::Math::LASS23L::setA ( const double x ) 
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_a ) ) { return false ; }
  //
  m_a = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters 
// ============================================================================
bool Gaudi::Math::LASS23L::setR ( const double x ) 
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_r ) ) { return false ; }
  //
  m_r = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters 
// ============================================================================
bool Gaudi::Math::LASS23L::setE ( const double x ) 
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_e ) ) { return false ; }
  //
  m_e = v ;
  //
  return true ;
}
// ============================================================================
// get the (complex) LASS amplitude 
// ============================================================================
std::complex<double> 
Gaudi::Math::LASS23L::amplitude ( const double x ) const 
{
  //
  if ( x <= m_ps.lowEdge  () ) { return 0 ; }  // RETURN 
  if ( x >= m_ps.highEdge () ) { return 0 ; }  // RETURN 
  //
  const double q  = m_ps.q ( x    ) ;
  if ( 0 >= q                ) { return 0 ; }  // RETURN 
  //
  // get the width:
  const double gs = gamma_run ( m_g0       , 
                                x          , 
                                m_ps.m1 () , 
                                m_ps.m2 () , 
                                m_m0       , 
                                m_ps.l  () ) * m_m0 / x  ; 
  //
  // phase shift:
  const double cotB = 1.0 / ( m_a * q ) + 0.5 * m_r * q  ;
  // phase shift: 
  const double cotR = ( m_m0 * m_m0 - x * x )  / m_m0 / gs ;
  //
  const double sinB =  1.0 / std::sqrt ( 1 + cotB*cotB ) ;
  const double cosB = cotB * sinB ;
  //
  // exp( i*pi/2 ) 
  static const std::complex<double> i = std::complex<double>( 0 , 1 );
  //
  // exp( i*Delta_B )
  std::complex<double> deltaB ( cosB , sinB ) ;
  //
  // the amplitude
  std::complex<double> A = 
    1.0 / ( cotB - i ) + m_e * deltaB * deltaB / ( cotR - i ) ;
  //
  // scale it! 
  std::complex<double> T = A * ( x / q ) ;
  //
  return T ;
}
// ============================================================================
// get the phase space factor
// ============================================================================
double Gaudi::Math::LASS23L::phaseSpace ( const double x ) const 
{ return std::max ( 0.0 , m_ps ( x ) ) ; }
// ============================================================================
// evaluate LASS 
// ============================================================================
double Gaudi::Math::LASS23L::operator () ( const double x ) const 
{ 
  const double result = phaseSpace  ( x ) ;
  if ( 0 >= result ) { return 0 ; }
  //
  return result * std::norm ( amplitude( x ) ) ;
}
// ============================================================================
// get the integral between low and high limits 
// ============================================================================
double  Gaudi::Math::LASS23L::integral 
( const double low  , 
  const double high ) const 
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN 
  if (           low > high   ) { return - integral ( high ,                                                     
                                                      low  ) ; } // RETURN 
  //
  if ( high <= m_ps.lowEdge  () ) { return 0 ; }
  if ( low  >= m_ps.highEdge () ) { return 0 ; }
  //
  if ( low  <  m_ps.lowEdge  () ) 
  { return integral ( m_ps.lowEdge() , high             ) ; }
  if ( high >  m_ps.highEdge () ) 
  { return integral ( low            , m_ps.highEdge () ) ; }
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function         = &LASS_23L_GSL ;
  const LASS23L* _ps = this  ;
  F.params           = const_cast<LASS23L*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag 
    ( &F                ,            // the function 
      low   , high      ,            // low & high edges 
      s_PRECISION       ,            // absolute precision            
      s_PRECISION       ,            // relative precision 
      s_SIZE            ,            // size of workspace 
      GSL_INTEG_GAUSS31 ,            // integration rule  
      workspace ( m_workspace ) ,    // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::LASS23L::QAG" ,
                __FILE__ , __LINE__ , ierror ) ; 
  }
  //
  return result ;
}
// ============================================================================
// get the integral 
// ============================================================================
double  Gaudi::Math::LASS23L::integral () const 
{ return integral ( m_ps.lowEdge () , m_ps.highEdge() ) ; }
// ============================================================================



// ============================================================================
// Bugg 
// ============================================================================
/*  constructor from all masses and angular momenta 
 *  @param M  mass of sigma (very different from the pole positon!)
 *  @param g2 width parameter g2 (4pi width)
 *  @param b1 width parameter b1  (2pi coupling)
 *  @param b2 width parameter b2  (2pi coupling)
 *  @param s1 width parameter s1  (cut-off for 4pi coupling)
 *  @param s2 width parameter s2  (cut-off for 4pi coupling)
 *  @param a  parameter a (the exponential cut-off) 
 *  @param m1 the mass of the first  particle 
 *  @param m3 the mass of the third  particle 
 *  @param m  the mass of the mother particle (m>m1+m2+m3)
 *  @param L  the angular momentum between the first pair and the third 
 */
// ============================================================================
Gaudi::Math::Bugg23L::Bugg23L
( const double         M  ,
  const double         g2 ,
  const double         b1 ,
  const double         b2 ,
  const double         s1 ,
  const double         s2 ,
  const double         a  ,
  const double         m1 ,
  const double         m3 ,
  const double         m  ,
  const unsigned short L  ) 
  : std::unary_function<double,double> () 
//
  , m_M  ( std::abs ( M  ) ) 
  , m_g2 ( std::abs ( g2 ) ) 
  , m_b1 ( std::abs ( b1 ) ) 
  , m_b2 ( std::abs ( b2 ) ) 
  , m_s1 ( std::abs ( s1 ) ) 
  , m_s2 ( std::abs ( s2 ) ) 
  , m_a  ( std::abs ( a  ) )
// phase space
  , m_ps ( m1 , m1 , m3 , m , L , 0 )  
//
  , m_workspace () 
{}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::Bugg23L::~Bugg23L(){}
// ============================================================================
double Gaudi::Math::Bugg23L::rho2_ratio ( const double x ) const 
{
  if ( lowEdge() >= x ) { return 0 ; }
  //
  return 
    Gaudi::Math::PhaseSpace2::phasespace ( x    , m1() , m2 () ) / 
    Gaudi::Math::PhaseSpace2::phasespace ( M () , m1() , m2 () ) ; 
}
// ============================================================================
std::complex<double> 
Gaudi::Math::Bugg23L::rho4_ratio ( const double x ) const 
{
  //
  if ( 4 * m1() >= x ) { return 0 ; }
  //
  return rho4 ( x ) / rho4 ( M() ) ;
}
// ============================================================================
std::complex<double> 
Gaudi::Math::Bugg23L::rho4 ( const double x ) const 
{
  const double s  = x * x ;
  //
  const double r2 = 1 - 16 * m1() * m1() / s ; 
  //
  const double r  = 
    std::sqrt ( std::abs ( r2 ) ) * 
    ( 1 + std::exp ( ( s1 () - s )  / s2 () ) ) ;
  //
  return 0 <= r2 ? 
    std::complex<double> ( r , 0 ) :
    std::complex<double> ( 0 , r ) ;
}
// ============================================================================
// Adler's pole 
// ============================================================================
double Gaudi::Math::Bugg23L::adler ( const double x ) const 
{
  if ( lowEdge() >= x ) { return 0 ; }
  //
  const double pole = 0.5 * m1 () * m1 ()  ;
  //
  return ( x * x - pole ) / ( M2 () - pole ) ;
}
// ============================================================================
// get the running width by Bugg
// ============================================================================
std::complex<double> 
Gaudi::Math::Bugg23L::gamma ( const double x ) const 
{
  //
  if ( lowEdge() >= x || highEdge() <= x ) { return 0 ; }
  //
  const double s = x * x ;
  //
  const double g1 = 
    b     ( x ) * 
    adler ( x ) * std::exp ( -1 * ( s - M2() )  / a() ) ;
  //
  return g1 * rho2_ratio ( x ) + g2 () * rho4_ratio ( x ) ;
}
// ============================================================================
// get the amlitude  (not normalized!)
// ============================================================================
std::complex<double> 
Gaudi::Math::Bugg23L::amplitude (  const double x ) const 
{
  if ( lowEdge() >= x || highEdge() <= x ) { return 0 ; }
  //
  static const std::complex<double> j ( 0 , 1 ) ;
  //
  std::complex<double> d = M2() - x * x  - j * M() * gamma ( x ) ;
  //
  return 1.0 / d ;
}
// ============================================================================
// evaluate Bugg
// ============================================================================
double Gaudi::Math::Bugg23L::operator () ( const double x ) const 
{ 
  //
  if ( lowEdge() >= x || highEdge() <= x ) { return 0 ; }
  //
  const double result = phaseSpace  ( x ) ;
  if ( 0 >= result ) { return 0 ; }
  //
  return result * std::norm ( amplitude ( x ) ) ;
}
// ============================================================================
// set the proper parameters 
// ============================================================================
bool Gaudi::Math::Bugg23L::setM ( const double x ) 
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_M ) ) { return false ; }
  //
  m_M = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters 
// ============================================================================
bool Gaudi::Math::Bugg23L::setG2 ( const double x ) 
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_g2 ) ) { return false ; }
  //
  m_g2 = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters 
// ============================================================================
bool Gaudi::Math::Bugg23L::setB1 ( const double x ) 
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_b1 ) ) { return false ; }
  //
  m_b1 = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters 
// ============================================================================
bool Gaudi::Math::Bugg23L::setB2 ( const double x ) 
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_b2 ) ) { return false ; }
  //
  m_b2 = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters 
// ============================================================================
bool Gaudi::Math::Bugg23L::setS1 ( const double x ) 
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_s1 ) ) { return false ; }
  //
  m_s1 = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters 
// ============================================================================
bool Gaudi::Math::Bugg23L::setS2 ( const double x ) 
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_s2 ) ) { return false ; }
  //
  m_s2 = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters 
// ============================================================================
bool Gaudi::Math::Bugg23L::setA ( const double x ) 
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_a ) ) { return false ; }
  //
  m_a = v ;
  //
  return true ;
}
// ============================================================================
// get the integral between low and high limits 
// ============================================================================
double  Gaudi::Math::Bugg23L::integral 
( const double low  , 
  const double high ) const 
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN 
  if (           low > high   ) { return - integral ( high ,                                                     
                                                      low  ) ; } // RETURN 
  //
  if ( high <= lowEdge  () ) { return 0 ; }
  if ( low  >= highEdge () ) { return 0 ; }
  //
  if ( low  <  lowEdge  () ) 
  { return integral ( lowEdge() , high        ) ; }
  //
  if ( high >  highEdge () ) 
  { return integral ( low       , highEdge () ) ; }
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function         = &Bugg_23L_GSL ;
  const Bugg23L* _ps = this  ;
  F.params           = const_cast<Bugg23L*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag 
    ( &F                ,            // the function 
      low   , high      ,            // low & high edges 
      s_PRECISION       ,            // absolute precision            
      s_PRECISION       ,            // relative precision 
      s_SIZE            ,            // size of workspace 
      GSL_INTEG_GAUSS31 ,            // integration rule  
      workspace ( m_workspace ) ,    // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::BUGG23L::QAG" ,
                __FILE__ , __LINE__ , ierror ) ; 
  }
  //
  return result ;
}
// ============================================================================
// get the integral 
// ============================================================================
double  Gaudi::Math::Bugg23L::integral () const 
{ return integral ( lowEdge () , highEdge() ) ; }
// ============================================================================



// ============================================================================
// constructor from all parameters
// ============================================================================
Gaudi::Math::BW23L::BW23L
( const double         m0   , 
  const double         gam0 ,
  const double         m1   , 
  const double         m2   , 
  const double         m3   , 
  const double         m    , 
  const unsigned short L1   , 
  const unsigned short L2   ) 
  : std::unary_function<double,double>() 
//
  , m_bw ( m0 , gam0 , m1  , m2 , L1      ) 
  , m_ps ( m1 , m2   , m3  , m  , L2 , L1 ) 
//
  , m_workspace () 
{}
// ============================================================================
// constructor from all parameters
// ============================================================================
Gaudi::Math::BW23L::BW23L
( const double                               m0   , 
  const double                               gam0 ,
  const double                               m1   , 
  const double                               m2   , 
  const double                               m3   , 
  const double                               m    , 
  const unsigned short                       L1   , 
  const unsigned short                       L2   ,
  const Gaudi::Math::BreitWigner::JacksonRho r    ) 
  : std::unary_function<double,double>() 
//
  , m_bw ( m0 , gam0 , m1  , m2 , L1 , r  ) 
  , m_ps ( m1 , m2   , m3  , m  , L2 , L1 ) 
//
  , m_workspace () 
{}
// ============================================================================
// constructor from BreitWigner  
// ============================================================================
Gaudi::Math::BW23L::BW23L
( const Gaudi::Math::BreitWigner& bw , 
  const double                    m3 , 
  const double                    m  , 
  const unsigned short            L2 ) 
  : std::unary_function<double,double>() 
//
  , m_bw ( bw ) 
  , m_ps ( bw.m1() , bw.m2() , m3  , m  , L2 , bw. L()) 
//
  , m_workspace () 
{}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::BW23L::~BW23L (){}
// ============================================================================
// calculate the shape 
// ============================================================================
double Gaudi::Math::BW23L::operator() ( const double x ) const 
{
  if (  lowEdge() >= x || highEdge()  <= x ) { return 0 ; }
  //
  const double bw = m_bw ( x ) ;
  //
  // get the incomplete phase space factor 
  const double ps  =                   // get the incomplete phase space factor 
    x / M_PI *  
    // =======================================================================
    // the second factor is already in our BW !!! 
    // Gaudi::Math::PhaseSpace2::phasespace ( x   , m_m1 , m_m2 , m_l  ) * 
    // =======================================================================
    Gaudi::Math::PhaseSpace2::phasespace ( m_ps.m  () ,
                                           x          , 
                                           m_ps.m3 () , 
                                           m_ps.L  () ) ;
  // 
  return bw * ps ;  
}
// ============================================================================
// get the integral between low and high limits 
// ============================================================================
double  Gaudi::Math::BW23L::integral 
( const double low  , 
  const double high ) const 
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN 
  if (           low > high   ) { return - integral ( high ,                                                     
                                                      low  ) ; } // RETURN 
  //
  if ( high <= lowEdge  () ) { return 0 ; }
  if ( low  >= highEdge () ) { return 0 ; }
  //
  if ( low  <  lowEdge  () ) 
  { return integral ( lowEdge() , high        ) ; }
  //
  if ( high >  highEdge () ) 
  { return integral ( low       , highEdge () ) ; }
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                   ;
  F.function         = &BW_23L_GSL ;
  const BW23L* _ps   = this  ;
  F.params           = const_cast<BW23L*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag 
    ( &F                ,            // the function 
      low   , high      ,            // low & high edges 
      s_PRECISION       ,            // absolute precision            
      s_PRECISION       ,            // relative precision 
      s_SIZE            ,            // size of workspace 
      GSL_INTEG_GAUSS31 ,            // integration rule  
      workspace ( m_workspace ) ,    // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::BW23L::QAG" ,
                __FILE__ , __LINE__ , ierror ) ; 
  }
  //
  return result ;
}
// ============================================================================
// get the integral 
// ============================================================================
double  Gaudi::Math::BW23L::integral () const 
{ return integral ( lowEdge () , highEdge() ) ; }
// ============================================================================



// ============================================================================
// Flatte23L 
// ============================================================================
/*  constructor  from all parameters 
 *  @param m0    the mass 
 *  @param m0g1  parameter \f$ m_0\times g_1\f$
 *  @param g2og2 parameter \f$ g2/g_1       \f$
 *  @param mK    kaon mass 
 *  @param mPi   pion mass
 *  @param m3    the mass of the third particle 
 *  @param m     the mass of mother particle  
 *  @param L     the orbital momentum between the pair and the third particle
 */
// ============================================================================
Gaudi::Math::Flatte23L::Flatte23L
( const double         m0    ,     // MeV 
  const double         m0g1  ,     // MeV^2
  const double         g2og1 ,     // dimensionless 
  const double         mK    ,     // MeV 
  const double         mPi   ,     // MeV 
  const double         m3    ,     // MeV 
  const double         m     ,     // MeV 
  const unsigned short L     ) 
  : std::unary_function<double,double> () 
// 
  , m_flatte    ( m0  , m0g1 , g2og1 , mK , mPi  ) 
  , m_ps        ( mPi , mPi  , m3    , m  , L    ) 
//
  , m_workspace () 
{}
// ============================================================================
/* constructor  from flatte function  
 *  @param m3    the mass of the third particle 
 *  @param m     the mass of mother particle  
 *  @param L     the orbital momentum between the pair and the third particle
 */
// ============================================================================
Gaudi::Math::Flatte23L::Flatte23L
( const Gaudi::Math::Flatte& fun ,     // MeV 
  const double               m3  ,     // MeV 
  const double               m   ,     // MeV 
  const unsigned short       L   ) 
  : std::unary_function<double,double> () 
// 
  , m_flatte    ( fun ) 
  , m_ps        ( fun.mPi() , fun.mPi()  , m3    , m  , L    ) 
//
  , m_workspace () 
{}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::Flatte23L::~Flatte23L (){}
// ============================================================================
// get the value of Flatte function 
// ============================================================================
double Gaudi::Math::Flatte23L::operator() ( const double x ) const 
{
  //
  if ( lowEdge () >= x || highEdge() <= x ) { return 0 ; } // RETURN
  //
  // get the amplitude...
  std::complex<double> amp = m_flatte.flatte_amp ( x ) ;
  //
  return m_ps ( x ) * std::norm ( amp ) * 2 / M_PI * m0g1() ;    
}
// ============================================================================
// get the integral between low and high limits 
// ============================================================================
double  Gaudi::Math::Flatte23L::integral 
( const double low  , 
  const double high ) const 
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN 
  if (           low > high   ) { return - integral ( high ,                                                     
                                                      low  ) ; } // RETURN 
  //
  if ( high <= lowEdge  () ) { return 0 ; }
  if ( low  >= highEdge () ) { return 0 ; }
  //
  if ( low  <  lowEdge  () ) 
  { return integral ( lowEdge() , high        ) ; }
  //
  if ( high >  highEdge () ) 
  { return integral ( low       , highEdge () ) ; }
  //
  // use GSL to evaluate the integral 
  //
  Sentry sentry ;
  //
  gsl_function F                   ;
  F.function             = &Flatte_23L_GSL ;
  const Flatte23L* _ps   = this  ;
  F.params               = const_cast<Flatte23L*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag 
    ( &F                ,            // the function 
      low   , high      ,            // low & high edges 
      s_PRECISION       ,            // absolute precision            
      s_PRECISION       ,            // relative precision 
      s_SIZE            ,            // size of workspace 
      GSL_INTEG_GAUSS31 ,            // integration rule  
      workspace ( m_workspace ) ,    // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    gsl_error ( "Gaudi::Math::BW23L::QAG" ,
                __FILE__ , __LINE__ , ierror ) ; 
  }
  //
  return result ;
}
// ============================================================================
// get the integral 
// ============================================================================
double  Gaudi::Math::Flatte23L::integral () const 
{ return integral ( lowEdge () , highEdge() ) ; }


// ============================================================================
// constructor from the order 
// ============================================================================
Gaudi::Math::Bernstein::Bernstein
( const unsigned short      N    ,   
  const double              xmin ,
  const double              xmax ) 
  : std::unary_function<double,double> () 
//
  , m_pars ( N + 1 , 0.0 )
// 
  , m_xmin ( std::min ( xmin , xmax ) ) 
  , m_xmax ( std::max ( xmin , xmax ) )
//
{}
// ============================================================================
// constructor from the order 
// ============================================================================
Gaudi::Math::Bernstein::Bernstein
( const std::vector<double>& pars ,
  const double               xmin ,
  const double               xmax ) 
  : std::unary_function<double,double> () 
//
  , m_pars ( pars ) 
// 
  , m_xmin ( std::min ( xmin , xmax ) ) 
  , m_xmax ( std::max ( xmin , xmax ) )
//
{
  if ( m_pars.empty() ) { m_pars.push_back( 0 ) ; }
}
// ============================================================================
// get the integral between xmin and xmax
// ============================================================================
double Gaudi::Math::Bernstein::integral () const 
{
  //
  return 
    ( m_xmax - m_xmin ) * 
    std::accumulate ( m_pars.begin() , m_pars.end() , 0.0 ) / npars() ;
}
// ============================================================================
// set k-parameter
// ============================================================================
bool Gaudi::Math::Bernstein::setPar
( const unsigned short k , const double value ) 
{
  //
  if ( k >= npars() ){ return false ; }
  //
  if ( s_equal ( par ( k ) , value ) ) { return false ; }
  //
  m_pars [ k ] = value ;
  //
  return true ;
}
// ============================================================================
namespace 
{
  // ==========================================================================
  // De Casteljau's algorithm
  double _casteljau_
  ( const std::vector<double>& beta , 
    const double               t0   , 
    const double               t1   ) 
  {
    // the trivial cases 
    if      ( beta.empty()     ) { return 0       ; }
    else if ( 1 == beta.size() ) { return beta[0] ; }  
    else if ( 2 == beta.size() ) { return beta[0]*t1 + beta[1]*t0 ; }
    //
    // prepare recursion 
    const std::size_t N = beta.size() - 1 ;
    std::vector<double> beta_prime ( N , 0 ) ;
    for ( std::size_t i = 0 ; i < N ; ++i ) 
    { beta_prime[i] = beta [ i ] * t1 + beta [ i+1 ] * t0 ; }
    //
    // recursion 
    return _casteljau_ ( beta_prime , t0 , t1 ) ;
  }
  // ==========================================================================
  // De Casteljau's algorithm
  template <class IT> 
  double _casteljau_
  ( IT first , 
    IT last  , 
    const double               t0   , 
    const double               t1   ) 
  {
    // the trivial cases 
    if      ( first == last    ) { return 0       ; }
    //
    const std::size_t len  = std::distance ( first , last  ) ;
    //
    if      ( 1 == len ) { return       *first                        ; }  
    else if ( 2 == len ) { return t1 * (*first) + t0 * ( *(first+1) ) ; }
    //
    IT second = --last ;
    //
    // prepare recursion 
    for ( IT it = first ; it != second ; ++it ) 
    { *it = t1 * ( *it )  + t0 * ( *( it+1 ) ) ; }
    //
    // recursion 
    return _casteljau_ ( first , second , t0 , t1 ) ;
  }
  // ==========================================================================
}
// ============================================================================
// get the value 
// ============================================================================
double Gaudi::Math::Bernstein::operator () ( const double x ) const 
{
  /// the trivial cases 
  if ( x < m_xmin || x > m_xmax ) { return 0.0  ; }
  //
  if      ( 0 == npars () ) { return 0.0        ; } 
  else if ( 1 == npars () ) { return m_pars [0] ; }
  ///
  // get the t-values 
  const double t0 = t ( x ) ;
  const double t1 = 1 - t0  ;
  //
  std::vector<double> beta = m_pars ;
  return _casteljau_ ( beta.begin() , beta.end() , t0 , t1 ) ;
}
// ============================================================================
namespace 
{
  // ==========================================================================
  inline void _phi0_ ( std::vector<double>& phi0 ) 
  {
    const std::size_t N = phi0.size() ;
    for ( std::size_t i = 0 ; i < N ; ++i ) 
    {
      const long double ni = N - i ;
      phi0[i] = std::atan2 ( std::sqrt ( ni ) , 1.0L ) ;
    }
  }
  // ==========================================================================
}
// ============================================================================
// constructor from the order 
// ============================================================================
Gaudi::Math::Positive::Positive
( const unsigned short      N    ,   
  const double              xmin ,
  const double              xmax ) 
  : std::unary_function<double,double> () 
//
  , m_bernstein ( N , xmin , xmax ) 
  , m_phases    ( N , 0 ) 
  , m_phi0      ( N , 0 ) 
  , m_sin2      ( N , 0 ) 
{
  // 
  _phi0_ ( m_phi0 ) ;
  //
  for ( unsigned short i = 0 ; i < N ; ++i ) 
  {
    const double s = std::sin ( m_phi0[i] ) ;
    m_sin2 [ i ] = s * s ;
  }
  //
  updateBernstein () ;
}
// ============================================================================
// constructor from the order 
// ============================================================================
Gaudi::Math::Positive::Positive
( const std::vector<double>& pars ,
  const double               xmin ,
  const double               xmax ) 
  : std::unary_function<double,double> () 
//
  , m_bernstein ( pars.size () , xmin , xmax )
  , m_phases    ( pars             ) 
  , m_phi0      ( pars.size () , 0 ) 
  , m_sin2      ( pars.size () , 0 ) 
{
  //
  _phi0_ ( m_phi0 ) ;
  // 
  for ( unsigned int i =  0 ; i < m_phases.size() ; ++i ) 
  {
    const double s = std::sin ( m_phases[i]  + m_phi0 [ i ] ) ;
    m_sin2 [ i ]   = s * s ;
  }
  //
  updateBernstein () ;
}
// ============================================================================
// set k-parameter
// ============================================================================
bool Gaudi::Math::Positive::setPar ( const unsigned short k , const double value ) 
{ 
  //
  if (  k >= m_phases.size() )         { return false ; } // FALSE 
  //
  if ( s_equal ( value , par ( k ) ) ) { return false ; }
  //
  const double s  = std::sin ( value + m_phi0[k] ) ;
  const double s2 =  s * s ;
  //
  if ( s_equal ( s2 , m_sin2 [ k ] ) ) { return false ; }
  //
  m_phases [ k ] = value ;
  m_sin2   [ k ] = s2    ;
  //
  return updateBernstein ( k ) ;  
}
// =============================================================================
// update bernstein coefficients 
// =============================================================================
bool Gaudi::Math::Positive::updateBernstein ( const unsigned short k ) 
{
  // 
  double psin2 = 1 ;
  for ( unsigned int i = 0 ; i < k ; ++ i ) { psin2 *= m_sin2[i] ; }
  //
  bool update = false ;
  const std::size_t np = npars() + 1 ;
  for ( unsigned int i = k ; i < npars() ; ++i ) 
  {
    const double sin2 = m_sin2   [i] ;
    const double cos2 = 1 - sin2     ;
    bool up = m_bernstein.setPar ( i , psin2 * cos2 * np ) ;
    update  = up || update ;
    psin2  *= sin2 ;
  }
  //
  const bool up = m_bernstein.setPar ( npars() , psin2 * np ) ;
  update  = up || update ;  
  //
  return update ;
}
// ============================================================================
// The END 
// ============================================================================


